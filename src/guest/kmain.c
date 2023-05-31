#include "lib/log.h"
#include "hardware/registers.h"
#include "hardware/msr.h"
#include "vmm/vmexit.h"
#include "guest/syscalls.h"

extern void __vmcall(unsigned int number);

void exit() {
    while (1) {}
}

void mallware(unsigned int number) {
    LOG_INFO("MALLWARE infected syscall :)");
    syscall_handler(number);
}

void sysenter_mallware(unsigned int number) {
    
    LOG_INFO("MALLWARE infected sysenter :)");
    __sysenter_handler(number);
}

void inject_mallware() {
    write_msr(MSR_IA32_SYSENTER_EIP, (qword_t)sysenter_mallware);
    write_msr(MSR_IA32_LSTAR, (qword_t)mallware);
}


void kmain() {
    LOG_INFO("Initializing OS");

    init_syscalls();
    __vmcall(PROTECET_SECURE_PAGE);
    inject_mallware();
    __vmcall(PROTECET_SYSCALL);
    
    // after protecting LSATR from writes
    __vmcall(VMM_ATTACK_LSTAR);
    void (*syscall_handler)(unsigned int) = (void (*)(unsigned int))read_msr(MSR_IA32_LSTAR);
    syscall_handler(123);

    // privilege instruction
    asm volatile("hlt");

    __syscall(42);
    __sysenter(44);
    
    asm volatile("mov [0x1812000], %rax");
    exit();
}



