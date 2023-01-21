
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


    call REAL_MODE_RELOCATION(real_mode_callback)

section .multiboot
%include "src/boot/multiboot.asm"

section .data
%include "src/boot/gdt.asm"