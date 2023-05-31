#include "hardware/registers.h"
#include "hardware/msr.h"
#include "lib/log.h"
#include "lib/utils.h"
#include "guest/syscalls.h"


#define STACK_SIZE 0x1000
#define EFER_SYSCALL_ENABLE 0x1
#define SSDT_ADDRESS 0x1813000

// we need to save rsp before sysenter because sysenter trashes rdx and rcx
// and this is because the jump is not between rings.
qword_t sysenter_rsp = 0;

extern void __syscall_handler();

void __sysenter_handler(unsigned int number) {
    dispatch_handler(number);
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

    memset((void *)SSDT_ADDRESS, 0, 0x1000);

}

void dispatch_handler(unsigned int number) {
    LOG_INFO("handling syscall %d", number);
    ssdt_t *ssdt = (ssdt_t *)SSDT_ADDRESS;

    if (number > SSDT_SIZE) {
        LOG_ERROR("out of bounds syscall number: %d", number);
    } else if (ssdt->entries[number] == NULL) {
        LOG_ERROR("syscall %d doesn't have a handler", number);
    } else {
        syscall_handler_t handler = ssdt->entries[number];
        handler();
    }
}

void set_syscall_handler(unsigned int number, syscall_handler_t handler) {
    ssdt_t *ssdt = (ssdt_t *)SSDT_ADDRESS;
    if (number > SSDT_SIZE) {
        return;
    }
    ssdt->entries[number] = handler;
}