#pragma once

#include "types.h"

/* 
Inline improves performance by copy and pasts the function contents
static is used to create a local copy of the inline functions inside other object files
*/
static inline void out(dword_t port, byte_t data) {
    /*
    volatile means the value may change, even if it doesn't appear to be modified,
    and the compiler should not make any optimizations
    */ 
    asm volatile("out %1, %0" :: "d"(port), "a"(data));
}

/*
https://www.felixcloutier.com/x86/wrmsr
*/
static inline void write_msr(dword_t msr, qword_t value) {
    // No need to use clobbered registers because the constraints are specific
    asm volatile("wrmsr" :: "c"(msr), "d"(value >> 32), "a"(value & 0xFFFFFFFF));
}

/*
https://www.felixcloutier.com/x86/rdmsr
*/
static inline qword_t read_msr(dword_t msr) {
    dword_t msb, lsb;
    qword_t value;

    asm volatile("rdmsr" : "=a"(lsb), "=d"(msb) : "c"(msr));
    value = msb;
    return (value << 32 | lsb);
}

