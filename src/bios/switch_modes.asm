

;------------------------------------------------------------------
; Calls a real mode function
; di - points to the real mode function
[bits 64]
call_real_mode_function:
    call REAL_MODE_RELOCATION(long_to_protected)
[bits 32]
    call REAL_MODE_RELOCATION(protected_to_real)
[bits 16]
    call di ; function pointer
    call REAL_MODE_RELOCATION(real_to_protected)
[bits 32]
    call REAL_MODE_RELOCATION(protected_to_long)
[bits 64]
    ret
;------------------------------------------------------------------



;------------------------------------------------------------------
; Jumps from real mode to protected mode
; Returns the new stack inside eax
[bits 16]
real_to_protected:

    pop di
    cli
    
    lgdt [REAL_MODE_RELOCATION(gdt.pointer)]

    ; enable protection
    mov eax, cr0
    or eax, (PROTECTION_ENABLE)
    mov cr0, eax
    jmp gdt.IA32_code_segment:REAL_MODE_RELOCATION(protected_mode)

[bits 32]
protected_mode:
    setup_data_segments gdt.IA32_data_segment

    and edi, 0xFFFF
    ; Note that I don't initialize an IDT
    mov eax, HIGHER_STACK
    push edi
    ret

;------------------------------------------------------------------
    

;------------------------------------------------------------------
; Jumps from protected mode to long mode.
; Returns the long mode stack pointer address
[bits 32]
protected_to_long:

    pop esi
    call REAL_MODE_RELOCATION(setup_pml4_map)
    call REAL_MODE_RELOCATION(enabling_paging_mode)

    lgdt [gdt.pointer]
    
    jmp gdt.IA32e_code_segment:REAL_MODE_RELOCATION(long_mode)

[bits 64]
long_mode:
    setup_data_segments gdt.IA32e_data_segment
    and rsi, 0xFFFFFFFF ; safty reasons
    mov rax, HIGHER_STACK
    push rsi
    ret
;------------------------------------------------------------------

;------------------------------------------------------------------
; Jumps from protected mode to long mode inside the "higher memory".
; Returns the long mode stack pointer address
[bits 32]
protected_to_long_higher_memory:

    pop esi
    call setup_pml4_map
    call enabling_paging_mode

    lgdt [gdt.pointer]
    
    jmp gdt.IA32e_code_segment:protected_to_long_higher_memory.long_mode

[bits 64]
.long_mode:
    setup_data_segments gdt.IA32e_data_segment
    and rsi, 0xFFFFFFFF ; safty reasons
    mov rax, HIGHER_STACK
    push rsi
    ret
;------------------------------------------------------------------

;------------------------------------------------------------------
; Jumps from long mode to protected mode.
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

    ; note that I stay with the same page tables
    mov eax, PDP_ADDRESS
    mov cr3, eax

    mov ecx, EFER_MSR          
    rdmsr
    and eax, ~LONG_MODE               
    wrmsr

    mov eax, cr0
    or eax, PAGING
    mov cr0, eax

    ; Note: in section 9.8.5.4 step 6 intel instructs to make a branch instruction.
    ; I don't do it because it does not effects the current state of the segment registers
    mov eax, HIGHER_STACK
    
    ret 2
;------------------------------------------------------------------
    
;------------------------------------------------------------------
; Jumps from protected mode to real mode.
; Note that the caller must be from the first megabyte region otherwise this function will fail to return
; Returns the real mode stack pointer address
[bits 32]
protected_to_real:
    
    cli
    ; paging must be identity mapped
    mov eax, cr0
    and eax, ~(PAGING)
    mov cr0, eax

    ; flush TLB
    mov eax, 0x0
    mov cr3, eax

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
    mov ax, LOWER_STACK
    ret 2 
;------------------------------------------------------------------

ivt_pointer:
    dw 0x3ff
    dq 0x0 ; the IVT location is 0x0000
