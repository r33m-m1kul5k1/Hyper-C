
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
%include "src/bios/real_mode.asm"
%include "src/boot/paging.asm"

[bits 32]
_start:

    ; setup long mode structures
    lgdt [gdt.pointer]
    call setup_pml4_map
    

    call protected_to_long

[bits 64]
    
    mov rsp, HIGHER_MEMORY_STACK_TOP
    call initialize_vmm

after:
    hlt




section .multiboot
%include "src/boot/multiboot.asm"
