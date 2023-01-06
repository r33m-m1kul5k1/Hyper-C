
global _start
extern initialize_machine
extern setup_pml4_map
extern enabling_paging_mode

%include "src/boot/gdt.asm"

section .text
[bits 32]
_start:

    call setup_pml4_map
    call enabling_paging_mode
    
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

    hlt