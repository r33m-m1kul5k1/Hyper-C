
global _start
extern initialize_machine

section .text

[bits 32]
_start:
    call protected_to_long

[bits 64]
    call initialize_machine

    hlt