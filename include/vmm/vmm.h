#pragma once

#include "hardware/types.h"
#include "hardware/error_codes.h"

struct __attribute__((__packed__)) {
    qword_t rax;
    qword_t rbx;
    qword_t rcx;
    qword_t rdx;
    qword_t rsi;
    qword_t rdi;
    qword_t rbp;
    qword_t r8;
    qword_t r9;
    qword_t r10;
    qword_t r11;
    qword_t r12;
    qword_t r13;
    qword_t r14;
    qword_t r15;    
    qword_t rsp;    
    qword_t rip;    
} typedef registers_t;

void enter_vmx_root();
void configure_vmcs();
void launch_vm();