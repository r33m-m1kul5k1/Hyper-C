;-----------------Control Registers-----------------
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)
%define PROTECTION_ENABLE (1 << 0)
;----------------------------------------------------

;-----------------------Memory-----------------------
%define PAGE_LENGTH 0x1_000
%define MEMORY_SIZE 2
; 2 MiB
%define LARGE_PAGE_SIZE (1 << 21)
%define HV_BASE_ADDRESS 0x100000
%define REAL_MODE_BASE_ADDRESS 0x7E00
; offset inside the real mode code + base
%define REAL_MODE_RELOCATION(addr) addr - real_mode_start + REAL_MODE_BASE_ADDRESS

; 1 MiB after HV code start
%define PML4_ADDRESS HV_BASE_ADDRESS + 0x100000
%define PDPT_ADDRESS PML4_ADDRESS + 0x1000
%define PD_ADDRESS PDPT_ADDRESS + 0x1000
; NOTE: code using this stack can overflow to the dap structure
%define LOWER_MEMORY_STACK_TOP 0x7BFF
%define HIGHER_MEMORY_STACK_TOP PD_ADDRESS + (0x1000 * MEMORY_SIZE) +0x4000
%define DAP_ADDRESS 0x500
%define DRIVE_NUMBER_ADDRESS 0x600

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