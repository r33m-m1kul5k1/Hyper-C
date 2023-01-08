
global _start
extern initialize_vmm
extern protected_to_long
extern long_to_protected
extern protected_to_real

section .text

[bits 32]
_start:
    call protected_to_long

[bits 64]
    mov rsp, rax

    call initialize_vmm

    call long_to_protected
[bits 32]
    mov esp, eax

    ; call protected_to_real
    ; mov sp, ax

    hlt