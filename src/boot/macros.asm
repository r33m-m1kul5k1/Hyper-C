;-----------------Control Registers-----------------
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)
%define PROTECTION_ENABLE (1 << 0)
;----------------------------------------------------

;-----------------------Memory-----------------------
%define PAGE_LENGTH 0x1_000
%define MEMORY_SIZE 2
%define LARGE_PAGE_SIZE (1 << 21)

; after 20 Kib of .text section
%define PML4_ADDRESS 0x5000
%define PDP_ADDRESS PML4_ADDRESS + 0x1000
%define PD_ADDRESS PDP_ADDRESS + (0x1000 * MEMORY_SIZE)
%define HIGHER_STACK PD_ADDRESS + 0x1000
%define LOWER_STACK 0x500
;----------------------------------------------------

;-----------------------MSRs-------------------------
%define EFER_MSR 0xC0000080
;----------------------------------------------------

;----------------------Entry-------------------------
%define PAGE_PRESENT (1 << 0)
%define PAGE_WRITE (1 << 1)
%define PAGE_SIZE (1 << 7)
;----------------------------------------------------


;----------------------------------------------------
; Stores a qword in little endian order
; %1 - address
; %2 - higher dword
; %3 - lower dword
%macro store_qword_little 3
    mov ebx, %1
    mov dword [ebx], %3
    mov dword [ebx + 4], %2
%endmacro
;----------------------------------------------------


;----------------------------------------------------
; Set the data segment registers
; %1 - index inside the gdt
%macro setup_data_segments 1
    mov ax, %1
    mov es, ax 
    mov ss, ax
    mov fs, ax
    mov ds, ax 
    mov gs, ax
%endmacro
;----------------------------------------------------