extern sysenter_rsp
global dispatch_syscall

[bits 64]
dispatch_syscall:
    mov [sysenter_rsp], rsp
    sysenter