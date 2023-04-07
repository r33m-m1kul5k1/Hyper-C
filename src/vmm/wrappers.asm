
%define VMCS_GUEST_RSP 0x0000681c
%define VMCS_GUEST_RIP 0x0000681e

global _vmexit_wrapper
extern vmexit_handler

    
; Saves the guest's registers, and enable the hv to modify this state 
_vmexit_wrapper:
    ; host's fs base points to the guest registers state
    mov [fs:0x00], rax
    mov [fs:0x08], rbx
    mov [fs:0x10], rcx
    mov [fs:0x18], rdx
    mov [fs:0x20], rsi
    mov [fs:0x28], rdi
    mov [fs:0x30], rbp
    mov [fs:0x38], r8
    mov [fs:0x40], r9
    mov [fs:0x48], r10
    mov [fs:0x50], r11
    mov [fs:0x58], r12
    mov [fs:0x60], r13
    mov [fs:0x68], r14
    mov [fs:0x70], r15

    mov rax, VMCS_GUEST_RSP
    vmread rbx, rax
    mov [fs:0x78], rbx

    mov rax, VMCS_GUEST_RIP
    vmread rbx, rax
    mov [fs:0x80], rbx

    call vmexit_handler

    mov rbx, [fs:0x78]
    mov rax, VMCS_GUEST_RSP
    vmwrite rax, rbx

    mov rbx, [fs:0x80] 
    mov rax, VMCS_GUEST_RIP
    vmwrite rax, rbx
    
    mov rax, [fs:0x00] 
    mov rbx, [fs:0x08] 
    mov rcx, [fs:0x10] 
    mov rdx, [fs:0x18] 
    mov rsi, [fs:0x20] 
    mov rdi, [fs:0x28] 
    mov rbp, [fs:0x30] 
    mov r8,  [fs:0x38] 
    mov r9,  [fs:0x40] 
    mov r10, [fs:0x48] 
    mov r11, [fs:0x50] 
    mov r12, [fs:0x58] 
    mov r13, [fs:0x60] 
    mov r14, [fs:0x68] 
    mov r15, [fs:0x70]

    ; we cannot call a function here because it may change important registers such as rbp
    vmresume
    
    