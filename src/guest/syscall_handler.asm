extern sysenter_rsp
global __dispatch_syscall

[bits 64]
__dispatch_syscall:
    mov [sysenter_rsp], rsp
    sysenter