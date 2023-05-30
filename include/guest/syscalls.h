#pragma once

void init_syscalls();
extern void __dispatch_syscall(unsigned int number);