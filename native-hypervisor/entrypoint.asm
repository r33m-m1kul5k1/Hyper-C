; This entry point 
; jumps to long mode, (https://wiki.osdev.org/Entering_Long_Mode_Directly)
; calls the main c function,
; loads the first sector of the OS bootloader

extern initialize_machine
; Entry defines
%define PAGE_PRESENT (1 << 0)
%define PAGE_WRITE (1 << 1)
%define PAGE_SIZE (1 << 7)

; Memory defines
; http://www.osdever.net/tutorials/view/the-world-of-protected-mode
%define MEMORY_SIZE 2
%define FREE_SPACE 0x80_000
; the paging takes 0x1_000 * (MEMORY_SIZE + 2)
%define PML4_ENTRY (FREE_SPACE + 0x1000 | (PAGE_WRITE | PAGE_PRESENT)) ; <offset><flags>
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

[BITS 32]
section .text

multiboot2_header_start:
; double word - 32
; word - 16
; byte - 8

    dd 0xE85250D6 ; magic
    dd 0 ; architecture => 32-bit (protected) mode of i386
    dd (multiboot2_header_end - multiboot2_header_start) ; header_length
    dd -(0xE85250D6 + 0 + (multiboot2_header_end - multiboot2_header_start)) ; checksum
    
        multiboot2_tag_start:
        ; this doesn't work
        dw 3 ; tage type 
        dw 0 ; and it's flags 
        dd multiboot2_tag_end - multiboot2_tag_start ; size
        dd _start ; entry addresss
        multiboot2_tag_end:
    
        ; terminating tag
        dd 0
        dd 0
        dw 8
multiboot2_header_end:

    _start:
        
        output_serial '.'

        ; disable previous paging
        mov eax, cr0
        and eax, ~PAGING
        mov cr0, eax

        ; initialize PML4
        store_qword_little FREE_SPACE, 0x0, PML4_ENTRY

        
        mov ecx, MEMORY_SIZE ; each loop allocates 1G
        ; initialize pdp table
        lea ebx, [FREE_SPACE + 0x1000] ; pdp pointer
        lea eax, [FREE_SPACE + 0x2000] ; pd pointer
        
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
    lea ebx, [FREE_SPACE + 0x2000] ; first pd pointer

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
    
    
    cli ; disable IRQs
    
    ; enable PAE
    mov eax, cr4
    or eax, PAE 
    mov cr4, eax
    

    ; initialize PML4 pointer
    mov eax, FREE_SPACE
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

    jmp gdt.code64:setup_hypervisor


[BITS 64]
setup_hypervisor:

    mov rax, gdt.data
    mov ds, rax 
    mov es, rax 
    mov ss, rax
    mov fs, rax
    mov gs, rax
    

    output_serial '.'
    
    call initialize_machine
    
    ; https://forum.nasm.us/index.php?topic=1474.0 
    push gdt.code32
    push setup_real_mode
    retfq
    



[BITS 32]
setup_real_mode:
    

    mov esp, 0x90_000
    mov eax, gdt.data32
    mov ss, eax
    mov ds, eax
    mov es, eax
    mov fs, eax
	mov gs, eax


    ; disable long mode
    mov ecx, EFER_MSR          
    rdmsr
    and eax, ~LONG_MODE               
    wrmsr

    ; disable paging and protection mode
    mov eax, cr0
    and eax, ~(PAGING | PROTECTION_ENABLE)
    mov cr0, eax
    
    ; disable PAE
    mov eax, cr4
    and eax, ~PAE
    mov cr4, eax
    

    jmp 0:load_os


[BITS 16]   
load_os:
    
	mov ax, gdt.data
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

    
    lidt [ivt_pointer]
    sti
    hlt

section .rodata


gdt:
    dq 0
.code64: equ $ - gdt
    ; type c/d   |  present  | read/write| executable |  64-bit
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 53)

.code32: equ $ - gdt
    ; type c/d   |  present  | read/write| executable| 32-bit
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 54)

.code16: equ $ - gdt
    ; type c/d   |  present  | read/write| executable
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) 

.data32: equ $ - gdt
    ; type c/d   | present   | read write| 32-bit    
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 54)

.data: equ $ - gdt
    ; type c/d   | present   | read write  
    dq (1 << 44) | (1 << 47) | (1 << 41)

.pointer:
    dw .pointer - gdt - 1 ; limit (the size of our gdt) 
    dq gdt

ivt_pointer:
    dw 0x3ff
    dd 0 ; the IVT location is 0x0000