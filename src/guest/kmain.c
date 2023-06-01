#include "lib/log.h"
#include "hardware/registers.h"
#include "hardware/msr.h"
#include "vmm/vmexit.h"
#include "guest/syscalls.h"

extern void __vmcall(unsigned int number);

void exit();
void syscall_mallware(unsigned int number);
void sysenter_mallware(unsigned int number);
void mallware();
void inject_mallwares();
void print_a_crab();


void kmain() {
    LOG_INFO("Initializing OS");

    init_syscalls();
    __vmcall(PROTECET_SYSCALL);
    set_syscall_handler(42, print_a_crab);
    __vmcall(PROTECT_SSDT);
    __vmcall(HOOK_SSDT_READ);
    asm volatile("mov %0, [0x1812000]" :: "r"(mallware));
    inject_mallwares();
    __syscall(42);

    // after protecting LSATR from writes
    __vmcall(HOOK_LSTAR_READ);
    void (*syscall_handler)(unsigned int) = (void (*)(unsigned int))read_msr(MSR_IA32_LSTAR);
    syscall_handler(42);

    // privilege instruction
    asm volatile("hlt");

    __sysenter(42);
    
    exit();
}

void exit() {
    while (1) {}
}

void syscall_mallware(unsigned int number) {
    LOG_INFO("MALLWARE infected syscall :)");
    dispatch_handler(number);
}

void sysenter_mallware(unsigned int number) {
    
    LOG_INFO("MALLWARE infected sysenter :)");
    __sysenter_handler(number);
}

void inject_mallwares() {
    write_msr(MSR_IA32_SYSENTER_EIP, (qword_t)sysenter_mallware);
    write_msr(MSR_IA32_LSTAR, (qword_t)syscall_mallware);
    set_syscall_handler(42, mallware);
}

void mallware() {
    LOG_INFO("::<>");
}
void print_a_crab() {
    LOG_INFO("(\\/) (°,,,,°) (\\/)");
}