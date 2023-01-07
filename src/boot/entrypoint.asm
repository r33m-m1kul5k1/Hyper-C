
global _start
extern initialize_machine
extern protected_to_long
extern long_to_protected

section .text

[bits 32]
_start:
    call protected_to_long

[bits 64]
    mov rsp, rax

    call initialize_machine

    call long_to_protected
[bits 32]
    mov esp, eax
    hlt