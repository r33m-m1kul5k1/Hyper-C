; This entry point 
; jumps to long mode, (https://wiki.osdev.org/Entering_Long_Mode_Directly)
; calls the main c function,
; loads the first sector of the OS bootloader
extern initialize_machine
global _start

; Entry defines
%define PAGE_PRESENT (1 << 0)
%define PAGE_WRITE (1 << 1)
%define PAGE_SIZE (1 << 7)

; Memory defines
; http://www.osdever.net/tutorials/view/the-world-of-protected-mode - free space (0x500 - 0x9FFFF)
%define MEMORY_SIZE 2
%define FREE_SPACE_OFFSET 0x100_000

; the paging takes 0x1_000 * (MEMORY_SIZE + 2)
%define PML4_ENTRY (FREE_SPACE_OFFSET + 0x1000 | (PAGE_WRITE | PAGE_PRESENT)) ; <offset><flags>
%define LARGE_PAGE_SIZE 0x200_000




; Flags
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)
%define PROTECTION_ENABLE (1 << 0)

; MSRs
%define EFER_MSR 0xC0000080

; Serial
%define COM1 0x3F8
%define COM2 0x2F8
%define COM3 0x3E8
%define COM4 0x2E8

; stores a qword in little endian order
; %1 - address
; %2 - higher dword
; %3 - lower dword
%macro store_qword_little 3
    mov ebx, %1
    mov dword [ebx], %3
    mov dword [ebx + 4], %2
%endmacro

%macro output_serial 1
    mov dx, COM1
    mov al, %1
    out dx, al
%endmacro


global _start


section .multiboot
%include "src/boot/multiboot.asm"

section .data
%include "src/boot/gdt.asm"

dap_size db 0x10
unused db 0x0
number_of_sectors dw 0x1
destination_offset dw 0x7c00
destination_segment dw 0x0
source dq 0x0 ; first sector

section .text
[BITS 32]
    _start:
    

        
        output_serial '.'

        ; initialize PML4
        store_qword_little FREE_SPACE_OFFSET, 0x0, PML4_ENTRY
        
        mov ecx, MEMORY_SIZE ; each loop allocates 1G
        ; initialize pdp table
        lea ebx, [FREE_SPACE_OFFSET + 0x1000] ; pdp pointer
        lea eax, [FREE_SPACE_OFFSET + 0x2000] ; pd pointer
        
    setup_pdp_entries:

        mov edx, eax
        or edx, (PAGE_WRITE | PAGE_PRESENT)
        
        ; save pd entry to memory
        mov dword [ebx], edx
        mov dword [ebx + 4], 0x0

        ; advance entry
        add eax, 0x1000
        add ebx, 8 

        loop setup_pdp_entries
        
    mov ecx, MEMORY_SIZE
    shl ecx, 9
    mov eax, 0 
    lea ebx, [FREE_SPACE_OFFSET + 0x2000] ; first pd pointer

    ; note that this loop will overflow to other pd tables
    setup_pd_entries:
        
        mov edx, eax
        or edx, (PAGE_SIZE | PAGE_WRITE | PAGE_PRESENT) 

        ; save page table entry
        mov dword [ebx], edx
        mov dword [ebx + 4], 0x0

        add eax, LARGE_PAGE_SIZE
        add ebx, 8
        loop setup_pd_entries
    
    
    
    ; disable previous paging
    mov eax, cr0
    and eax, ~PAGING
    mov cr0, eax
    
    ; enable PAE
    mov eax, cr4
    or eax, PAE 
    mov cr4, eax
    
    ; initialize PML4 pointer
    mov eax, FREE_SPACE_OFFSET
    mov cr3, eax

    ; enable long mode
    mov ecx, EFER_MSR          
    rdmsr
    or eax, LONG_MODE               
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, PAGING
    mov cr0, eax

    
    
    lgdt [gdt.pointer]

    jmp gdt.IA32e_code_segment:setup_hypervisor


[BITS 64]
setup_hypervisor:

    mov rax, gdt.data_segment
    mov ds, rax 
    mov es, rax 
    mov ss, rax
    mov fs, rax
    mov gs, rax
    

    output_serial '.'
    
    call initialize_machine
    

    ; https://forum.nasm.us/index.php?topic=1474.0 
    push gdt.IA32_code_segment
    push compatibility_mode
    retfq
    



[BITS 32]
compatibility_mode:

    
    mov eax, gdt.IA32_data_segment
    mov ss, eax
    mov ds, eax
    mov es, eax
    mov fs, eax
	mov gs, eax


    mov eax, cr0
    and eax, ~(PAGING)
    mov cr0, eax
    
    ; note that I stay with the same page tables

    mov ecx, EFER_MSR          
    rdmsr
    and eax, ~LONG_MODE               
    wrmsr

    output_serial '.'
    mov eax, cr0
    or eax, PAGING
    mov cr0, eax

    output_serial '.'


    push gdt.IA32_code_segment
    push protected_mode
    retfd ; jmp works for HyperWin

    

[BITS 32]
protected_mode:

    cli
    ; paging must be identity mapped
    mov eax, cr0
    and eax, ~(PAGING)
    mov cr0, eax

    hlt


; [BITS 16]
; disable_protection:

;     mov eax, gdt.data
;     mov ss, eax
;     mov ds, eax
;     mov es, eax
;     mov fs, eax
; 	mov gs, eax
    
    

;     ; disable paging and protection mode
;     mov eax, cr0
;     and eax, ~(PAGING | PROTECTION_ENABLE)
;     mov cr0, eax

    
    
;     ; disable PAE only after disabling paging
;     mov eax, cr4
;     and eax, ~PAE
;     mov cr4, eax

    
;     output_serial '.'
    
;     jmp 0:load_os

; [BITS 16]
; load_os:

;     cli
;     mov ax, 0
; 	mov ds, ax
; 	mov es, ax
; 	mov fs, ax
; 	mov gs, ax
; 	mov ss, ax

;     mov sp, 0x600


;     lidt [ivt_pointer]

    
;     mov ax, 0
;     int 0x10 ; the code freezes here


;     mov ah, 0x0e    ; function number = 0Eh : Display Character
;     mov al, '!'     ; AL = code of character to display
;     int 0x10 
    

;     ; xor ax, ax
;     ; mov ah, 0x42
;     ; mov dl, 0x80 ; driver index
;     ; mov si, dap_size
;     ; xor bx, bx
;     ; int 0x13


    
    
;     sti ; can cause some undefine behavior
;     hlt
