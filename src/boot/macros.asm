;-----------------Control Registers-----------------
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)
%define PROTECTION_ENABLE (1 << 0)
;----------------------------------------------------

;-----------------------Memory-----------------------
%define PAGE_LENGTH 0x1_000

; at most can be 512
%define MEMORY_SIZE 4
; 2 MiB
%define LARGE_PAGE_SIZE (1 << 21)
%define FOUR_MEGAS (1 << 22)
%define HV_BASE_ADDRESS 0x100000
; the code must be smaller then 32 KiB till 0xFFFF
%define REAL_MODE_BASE_ADDRESS 0x7E00 
; offset inside the real mode code + base
%define REAL_MODE_RELOCATION(addr) addr - real_mode_start + REAL_MODE_BASE_ADDRESS

; Free memory regions
; 0x500  - 0x7BFF => 28 KiB
%define STACK_TOP 0x7BFF

; 0x7E00 - 0x7FFFF - 480 KiB

%define IA32e_PAGING_BASE 0x12000
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
    push ebx
    mov ebx, %1
    mov dword [ebx], %3
    mov dword [ebx + 4], %2
    pop ebx
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