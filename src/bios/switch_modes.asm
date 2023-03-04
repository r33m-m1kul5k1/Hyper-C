

;------------------------------------------------------------------
; Calls a real mode function
; di - points to the real mode function
[bits 64]
call_real_mode_function:
    call long_to_protected

[bits 32]
    call protected_to_real

[bits 16]
    call di ; function pointer
    call real_to_protected

[bits 32]
    call protected_to_long_lower_memory

[bits 64]
    ret

;------------------------------------------------------------------



;------------------------------------------------------------------
; Jumps from real mode to protected mode
; Safty - the gdt must be loaded before calling this function
; Returns the new stack inside eax
[bits 16]
real_to_protected:
    pop di
    cli

    ; enable protection
    mov eax, cr0
    or eax, (PROTECTION_ENABLE)
    mov cr0, eax
    jmp gdt.IA32_code_segment:REAL_MODE_RELOCATION(protected_mode)

[bits 32]
protected_mode:
    setup_data_segments gdt.IA32_data_segment

    ; Note that I don't initialize an IDT
    and edi, 0xFFFF
    push edi
    ret

;------------------------------------------------------------------
    

;------------------------------------------------------------------
; Jumps from protected mode to long mode.
; Returns the long mode stack pointer address
[bits 32]
protected_to_long_lower_memory:

    pop esi
    
    ; disable previous paging
    mov eax, cr0
    and eax, ~PAGING
    mov cr0, eax
    
    ; enable PAE
    mov eax, cr4
    or eax, PAE 
    mov cr4, eax

    ; initialize PML4 pointer
    mov eax, IA32e_PAGING_BASE
    mov cr3, eax

    mov ecx, EFER_MSR          
    rdmsr
    or eax, LONG_MODE               
    wrmsr
    
    mov eax, cr0
    or eax, PAGING
    mov cr0, eax

    jmp gdt.IA32e_code_segment:REAL_MODE_RELOCATION(.long_mode)

[bits 64]
.long_mode:
    setup_data_segments gdt.IA32e_data_segment

    and rsi, 0xFFFFFFFF
    push rsi
    ret
;------------------------------------------------------------------


;------------------------------------------------------------------
; Jumps from long mode to protected mode with no pae & paging.
; Returns the protected mode stack pointer address
[bits 64]
long_to_protected:
    ; https://forum.nasm.us/index.php?topic=1474.0
    push gdt.IA32_code_segment
    push REAL_MODE_RELOCATION(compatibility_mode)
    retfq


[bits 32]
compatibility_mode:

    setup_data_segments gdt.IA32_data_segment

    mov eax, cr0
    and eax, ~(PAGING)
    mov cr0, eax

    mov ecx, EFER_MSR          
    rdmsr
    and eax, ~LONG_MODE               
    wrmsr

    mov eax, cr4
    and eax, ~(PAE)
    mov cr4, eax

    ret 4
;------------------------------------------------------------------
    
;------------------------------------------------------------------
; Jumps from protected mode to real mode.
; Note that the caller must be from the first megabyte region otherwise this function will fail to return
; Returns the real mode stack pointer address
[bits 32]
protected_to_real:
    cli
    jmp gdt.real_mode_code_segment:REAL_MODE_RELOCATION(protected_real_mode)
 
[bits 16]
protected_real_mode:
    setup_data_segments gdt.real_mode_data_segment
    
    lidt [REAL_MODE_RELOCATION(ivt_pointer)]

    ; disable protection
    mov eax, cr0
    and eax, ~(PROTECTION_ENABLE)
    mov cr0, eax

  jmp 0:REAL_MODE_RELOCATION(real_mode)


real_mode:
    setup_data_segments 0x0
    sti
    ret 2 
;------------------------------------------------------------------

ivt_pointer:
    dw 0x3ff
    dq 0x0 ; the IVT location is 0x0000

