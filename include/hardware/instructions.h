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


static inline qword_t read_cr0() {
    qword_t cr0;
    asm volatile("mov %%cr0, %0"
                    : "=r"(cr0));
    return cr0;
}

static inline qword_t read_cr1() {
    qword_t cr1;
    asm volatile("mov %%cr1, %0"
                    : "=r"(cr1));
    return cr1;
}


static inline qword_t read_cr2() {
    qword_t cr2;
    asm volatile("mov %%cr2, %0"
                    : "=r"(cr2));
    return cr2;
}

static inline qword_t read_cr3() {
    qword_t cr3;
    asm volatile("mov %%cr3, %0"
                    : "=r"(cr3));
    return cr3;
}

static inline qword_t read_cr4() {
    qword_t cr4;
    asm volatile("mov %%cr4, %0"
                    : "=r"(cr4));
    return cr4;
}

static inline void write_cr0(qword_t cr0) {
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

static inline void write_cr1(qword_t cr1) {
    asm volatile("mov %0, %%cr1" :: "r"(cr1));
}

static inline void write_cr2(qword_t cr2) {
    asm volatile("mov %0, %%cr2" :: "r"(cr2));
}

static inline void write_cr3(qword_t cr3) {
    asm volatile("mov %0, %%cr3" :: "r"(cr3));
}

static inline void write_cr4(qword_t cr4) {
    asm volatile("mov %0, %%cr4" :: "r"(cr4));
}
