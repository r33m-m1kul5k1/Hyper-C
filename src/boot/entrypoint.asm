; Flags
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)
%define PROTECTION_ENABLE (1 << 0)
; after megabyte of .text section
%define FREE_SPACE_OFFSET 0x100_000
%define PML4_DESCRIPTOR (FREE_SPACE_OFFSET) << 12
%define PDP_DESCRIPTOR (FREE_SPACE_OFFSET + 0x1000) << 12
%define STACK_LOACTION 0x300_000
; MSRs
%define EFER_MSR 0xC0000080


global _start
extern initialize_machine
extern setup_pml4_map

%include "src/boot/gdt.asm"

section .text
[bits 32]
_start:
    mov esp, STACK_LOACTION

    mov ebx, FREE_SPACE_OFFSET
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
    mov eax, PML4_DESCRIPTOR
    mov cr3, eax

    mov ecx, EFER_MSR          
    rdmsr
    or eax, LONG_MODE               
    wrmsr

    mov eax, cr0
    or eax, PAGING
    mov cr0, eax
    hlt
    
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
    mov eax, PDP_DESCRIPTOR
    mov cr3, eax

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