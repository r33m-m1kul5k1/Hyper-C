#include "lib/log.h"
#include "hardware/registers.h"
#include "hardware/msr.h"
#include "vmm/vmexit.h"
#include "guest/syscalls.h"
#include "vmm/hooks.h"

extern void __vmcall(unsigned int number);

void exit();
void syscall_mallware(unsigned int number);
void sysenter_mallware(unsigned int number);
void inject_mallwares();



void kmain() {
    LOG_INFO("Initializing OS");
    init_syscalls();

    __vmcall(PROTECET_SYSCALL);
    inject_mallwares();

    __syscall(42);
    __sysenter(42);

    // privilege instruction
    asm volatile("hlt");
    exit();
}

void exit() {
    while (1) {}
}

void syscall_mallware(unsigned int number) {
    __syscall_handler(number);
    LOG_INFO("MALLWARE infected syscall :)");
}

void sysenter_mallware(unsigned int number) {
    __sysenter_handler(number);
    LOG_INFO("MALLWARE infected sysenter :)");
}

void inject_mallwares() {
    write_msr(MSR_IA32_SYSENTER_EIP, (qword_t)sysenter_mallware);
    write_msr(MSR_IA32_LSTAR, (qword_t)syscall_mallware);
}

void print_a_crab() {
    LOG_INFO("(\\/) (°,,,,°) (\\/)");
}