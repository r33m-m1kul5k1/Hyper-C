
%define VMCS_GUEST_RSP 0x0000681c
global _vmlaunch_handler

_vmlaunch_handler:
    mov rax, VMCS_GUEST_RSP
    vmwrite rax, rsp
    vmlaunch
    ret

