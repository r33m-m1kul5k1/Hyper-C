#include "lib/log.h"
#include "hardware/registers.h"
#include "hardware/msr.h"
#include "guest/syscalls.h"



void exit() {
    while (1) {}
}

void kmain() {
    LOG_INFO("Initializing OS");

    init_syscalls();
    dispatch_syscall(42);

    read_msr(MSR_IA32_EFER);
    
    LOG_DEBUG("After handling rdmsr");
    asm volatile("hlt");

    asm volatile("mov [0x1812000], %rax");
    exit();
}

