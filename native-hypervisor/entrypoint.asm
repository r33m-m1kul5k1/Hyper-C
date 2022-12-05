; This entry point 
; jumps to long mode, (https://wiki.osdev.org/Entering_Long_Mode_Directly)
; calls the main c function,
; loads the first sector of the OS bootloader

; Entry defines
%define PAGE_PRESENT (1 << 0)
%define PAGE_WRITE (1 << 1)
%define PAGE_SIZE (1 << 7)

; Memory defines
%define MEMORY_SIZE 2
%define FREE_SPACE 0x11000
%define PML4_ENTRY (FREE_SPACE + 0x1000 | (PAGE_WRITE | PAGE_PRESENT)) ; <offset><flags>
%define LARGE_PAGE_SIZE 0x200000
%define GDT_PTR_LOCATION 0x9000
%define GDT_LOCATION 0x10000



; Flags
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)

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
        ; disable previous paging
        mov eax, cr0
        and eax, ~PAGING
        mov cr0, eax

        ; initialize PML4
        store_qword_little FREE_SPACE, 0x0, PML4_ENTRY

        output_serial '1'

        
        mov ecx, MEMORY_SIZE ; each loop allocates 1G **if Page Size is 2M
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
    
    ; initialize gdt
    
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
    mov cr0, eax ; crushes here
    

    lgdt [gdt.pointer]

    jmp gdt.code:long_mode

[BITS 64]
long_mode:

    output_serial '2'


    jmp $

section .rodata
gdt:
    dq 0
.code: equ $ - gdt
    ; type code |  present  | read/write| executable|  64-bit
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 53)
.data: equ $ - gdt
    ; type data | present| read write    
    dq (1<<44) | (1<<47) | (1<<41)

.pointer:
    dw .pointer - gdt - 1 ; limit (the size of our gdt) 
    dq gdt