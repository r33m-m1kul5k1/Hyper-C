

extern setup_pml4_map
extern enabling_paging_mode
global protected_to_long

%include "src/boot/gdt.asm"

protected_to_long:

    call setup_pml4_map
    call enabling_paging_mode

    lgdt [gdt.pointer]
    
    jmp gdt.IA32e_code_segment:long_mode

[bits 64]
long_mode:
    setup_data_segments gdt.data_segment
    mov rsp, 
    ret


long_to_protected:
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
    mov eax, PDP_LOCATION
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