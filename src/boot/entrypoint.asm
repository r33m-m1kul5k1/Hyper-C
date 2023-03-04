
global _start
global real_mode_start
global real_mode_end
global real_mode_callback
global read_disk
global real_mode_smile

extern initialize_vmm

section .data
%include "src/boot/gdt.asm"

section .text
%include "src/boot/macros.asm"


[bits 32]
_start:
    mov esp, STACK_TOP
    call protected_to_long

[bits 64]  
    call initialize_vmm

after:
    hlt


;------------------------------------------------------------------
; Jumps from protected mode to long mode.
; Returns the long mode stack pointer address
[bits 32]
protected_to_long:

    pop esi
    
    ; setup long mode structures
    lgdt [gdt.pointer]
    call setup_pml4_map

    ; disable previous paging
    mov eax, cr0
    and eax, ~PAGING
    mov cr0, eax
    
    ; enable PAE
    mov eax, cr4
    or eax, PAE 
    mov cr4, eax

    ; initialize PML4 pointer
    mov eax, IA32e_PAGING_BASE
    mov cr3, eax

    mov ecx, EFER_MSR          
    rdmsr
    or eax, LONG_MODE               
    wrmsr
    
    mov eax, cr0
    or eax, PAGING
    mov cr0, eax

    jmp gdt.IA32e_code_segment:.long_mode

[bits 64]
.long_mode:
    setup_data_segments gdt.IA32e_data_segment

    and rsi, 0xFFFFFFFF
    push rsi
    ret
;------------------------------------------------------------------

%include "src/bios/real_mode.asm"
section .multiboot
%include "src/boot/multiboot.asm"
