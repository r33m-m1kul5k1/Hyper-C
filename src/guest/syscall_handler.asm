extern sysenter_rsp
extern dispatch_handler
global __sysenter
global __syscall
global __syscall_handler

[bits 64]
__sysenter:
    mov [sysenter_rsp], rsp
    sysenter

__syscall:
    syscall
    ret

__syscall_handler:
    push rcx
    push r11

    call dispatch_handler

    popf
    pop rcx
    jmp rcx
    ; can't use sysexit because we stay in the current privilege level.

