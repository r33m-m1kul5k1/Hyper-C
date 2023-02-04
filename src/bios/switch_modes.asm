

;------------------------------------------------------------------
; Calls a real mode function
; di - points to the real mode function
[bits 64]
call_real_mode_function:
    mov rbp, rsp ; saves the return address inside rbp
    call long_to_protected
[bits 32]
    mov esp, eax
    
    call protected_to_real

[bits 16]
    mov sp, ax

    call di ; function pointer
    call real_to_protected

[bits 32]
    mov esp, eax
    call protected_to_long

[bits 64]
    mov rsp, rbp

    ret
;------------------------------------------------------------------



;------------------------------------------------------------------
[bits 16]
; Jumps from real mode to protected mode
; Returns the new stack inside eax
real_to_protected:

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

    
    ; Note that I don't initialize an IDT
    mov eax, HIGHER_STACK
    ret

;------------------------------------------------------------------
    

;------------------------------------------------------------------
[bits 32]
; Jumps from protected mode to long mode.
; Returns the long mode stack pointer address
protected_to_long:

    call setup_pml4_map
    call enabling_paging_mode

    lgdt [gdt.pointer]
    
    jmp gdt.IA32e_code_segment:long_mode

[bits 64]
long_mode:
    setup_data_segments gdt.IA32e_data_segment
    mov rax, HIGHER_STACK
    ret
;------------------------------------------------------------------


;------------------------------------------------------------------
; Jumps from long mode to protected mode.
; Returns the protected mode stack pointer address
[bits 64]
long_to_protected:
    ; https://forum.nasm.us/index.php?topic=1474.0
    pop rdx ; saves ip 
    push gdt.IA32_code_segment
    push compatibility_mode
    retfq


[bits 32]
compatibility_mode:

    push edx ; puts the ip on the stack
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
    
    ret
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

