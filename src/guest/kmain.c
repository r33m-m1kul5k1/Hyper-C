#include "lib/log.h"
#include "hardware/registers.h"
#include "hardware/msr.h"
#include "vmm/vmexit.h"
#include "guest/syscalls.h"

extern void __vmcall(unsigned int number);

void exit() {
    while (1) {}
}

void mallware() {
    LOG_INFO("MALLWARE :)");
    exit();
}
void inject_mallware() {
    write_msr(MSR_IA32_SYSENTER_EIP, (qword_t)mallware);
}
void kmain() {
    LOG_INFO("Initializing OS");

    init_syscalls();
    __vmcall(PROTECET_SECURE_PAGE);
    inject_mallware();
    __vmcall(PROTECET_SYSCALL);
    
    asm volatile("hlt");

    __dispatch_syscall(42);
    asm volatile("mov [0x1812000], %rax");
    exit();
}



