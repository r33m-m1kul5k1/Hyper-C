
global _start
global real_mode_start
global real_mode_end

extern initialize_vmm


section .text
%include "src/boot/macros.asm"


[bits 32]
_start:

    call protected_to_long

[bits 64]
    mov rsp, rax

    call initialize_vmm

    mov rdi, real_mode_smile
    call real_mode_callback




%include "src/bios/real_mode.asm"
%include "src/boot/paging.asm"

section .multiboot
%include "src/boot/multiboot.asm"
