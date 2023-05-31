#include "hardware/registers.h"
#include "hardware/msr.h"
#include "lib/log.h"
#include "guest/syscalls.h"


#define STACK_SIZE 0x1000
#define EFER_SYSCALL_ENABLE 0x1

// we need to save rsp before sysenter because sysenter trashes rdx and rcx
// and this is because the jump is not between rings.
qword_t sysenter_rsp = 0;

extern void __syscall_handler();

void __sysenter_handler(unsigned int number) {
    syscall_handler(number);
    qword_t rip = *(qword_t *)sysenter_rsp;
    void (*ret)() = (void (*)())rip;
    ret();
}


byte_t kernel_stack[STACK_SIZE] = { 0 };

void init_syscalls() {
    write_msr(MSR_IA32_SYSENTER_CS, read_cs());
    write_msr(MSR_IA32_SYSENTER_EIP, (qword_t)__sysenter_handler);
    write_msr(MSR_IA32_SYSENTER_ESP, (qword_t)&kernel_stack + STACK_SIZE);

    write_msr(MSR_IA32_LSTAR, (qword_t)__syscall_handler);
    write_msr(MSR_IA32_STAR, ((qword_t)read_cs() << 32));
    write_msr(MSR_IA32_EFER, read_msr(MSR_IA32_EFER) | EFER_SYSCALL_ENABLE);

}

void syscall_handler(unsigned int number) {
    LOG_INFO("handling syscall %d", number);
}