#pragma once

void init_syscalls();
void syscall_handler(unsigned int number);
void __sysenter_handler(unsigned int number);
extern void __syscall(unsigned int number);
extern void __sysenter(unsigned int number);