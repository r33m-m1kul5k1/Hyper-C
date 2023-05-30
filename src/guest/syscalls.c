#include "hardware/registers.h"
#include "hardware/msr.h"
#include "lib/log.h"
#include "guest/syscalls.h"


#define STACK_SIZE 0x1000

// we need to save rsp before sysenter because sysenter trashes rdx and rcx
// and this is because the jump is not between rings.
qword_t sysenter_rsp = 0;

void syscall_handler();

void __sysenter_handler() {
    syscall_handler();
    qword_t rip = *(qword_t *)sysenter_rsp;
    void (*ret)() = (void (*)())rip;
    ret();
}


byte_t kernel_stack[STACK_SIZE] = { 0 };

void init_syscalls() {
    write_msr(MSR_IA32_SYSENTER_CS, read_cs());
    write_msr(MSR_IA32_SYSENTER_EIP, (qword_t)__sysenter_handler);
    write_msr(MSR_IA32_SYSENTER_ESP, (qword_t)&kernel_stack + STACK_SIZE);
}

void syscall_handler() {
    unsigned int number = 0;
    asm volatile("mov %%edi, %0" : "=r"(number));

    LOG_INFO("handling syscall %d", number);
}