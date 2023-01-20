
global _start
global real_mode_start
global real_mode_end

extern initialize_vmm


section .text

%include "src/boot/macros.asm"
%include "src/boot/paging.asm"
%include "src/bios/real_mode.asm"


[bits 32]
_start:

    call protected_to_long

[bits 64]
    mov rsp, rax

    call initialize_vmm

    call long_to_protected
[bits 32]
    mov esp, eax

    call protected_to_real
[bits 16]
    mov sp, ax

    hlt

section .multiboot
%include "src/boot/multiboot.asm"

section .data
%include "src/boot/gdt.asm"