#pragma once

#define SSDT_SIZE 512

typedef void (*syscall_handler_t)();

struct __attribute__((packed, aligned(4096))){
    syscall_handler_t entries[SSDT_SIZE];
} typedef ssdt_t;

void init_syscalls();
void set_syscall_handler(unsigned int number, syscall_handler_t handler);
void dispatch_handler(unsigned int number);
void __sysenter_handler(unsigned int number);
extern void __syscall(unsigned int number);
extern void __sysenter(unsigned int number);