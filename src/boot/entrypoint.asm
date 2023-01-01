; Flags
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)
%define PROTECTION_ENABLE (1 << 0)
%define PML4_POINTER_ADDRESS 0x1_0000
; MSRs
%define EFER_MSR 0xC0000080

%include "src/boot/gdt.asm"

global _start
extern initialize_machine
extern setup_paging


section .text
[bits 32]
_start:
    
    call setup_paging
    
    ; disable previous paging
    mov eax, cr0
    and eax, ~PAGING
    mov cr0, eax
    
    ; enable PAE
    mov eax, cr4
    or eax, PAE 
    mov cr4, eax
    
    ; initialize PML4 pointer
    mov eax, PML4_POINTER_ADDRESS
    mov cr3, eax

    mov ecx, EFER_MSR          
    rdmsr
    or eax, LONG_MODE               
    wrmsr

    mov eax, cr0
    or eax, PAGING
    mov cr0, eax
    
    lgdt [gdt.pointer]

    jmp gdt.IA32e_code_segment:setup_hypervisor


[bits 64]
setup_hypervisor:

    mov rax, gdt.data_segment
    mov ds, rax 
    mov es, rax 
    mov ss, rax
    mov fs, rax
    mov gs, rax

    call initialize_machine

    ; https://forum.nasm.us/index.php?topic=1474.0 
    push gdt.IA32_code_segment
    push compatibility_mode
    retfq



[bits 32]
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

    mov eax, cr0
    or eax, PAGING
    mov cr0, eax

    hlt

    jmp gdt.IA32_code_segment:protected_mode

    

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

;     mov eax, gdt.data_segment
;     mov ss, eax
;     mov ds, eax
;     mov es, eax
;     mov fs, eax
;     mov gs, eax
    
    

;     ; disable paging and protection mode
;     mov eax, cr0
;     and eax, ~(PAGING | PROTECTION_ENABLE)
;     mov cr0, eax

    
    
;     ; disable PAE only after disabling paging
;     mov eax, cr4
;     and eax, ~PAE
;     mov cr4, eax

    
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

;     lidt [ivt_pointer]
;     sti 

;     mov sp, 0x600



    
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

;     hlt

