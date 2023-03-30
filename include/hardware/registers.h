#pragma once
#include "types.h"

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

static inline word_t read_cs() {
    word_t selector;
    asm volatile("mov %%cs, %0" 
                    : "=r"(selector));
    return selector;
}

static inline word_t read_ss() {
    word_t selector;
    asm volatile("mov %%ss, %0" 
                    : "=r"(selector));
    return selector;
}

static inline word_t read_ds() {
    word_t selector;
    asm volatile("mov %%ds, %0" 
                    : "=r"(selector));
    return selector;
}

static inline word_t read_es() {
    word_t selector;
    asm volatile("mov %%es, %0" 
                    : "=r"(selector));
    return selector;
}

static inline word_t read_fs() {
    word_t selector;
    asm volatile("mov %%fs, %0" 
                    : "=r"(selector));
    return selector;
}

static inline word_t read_gs() {
    word_t selector;
    asm volatile("mov %%gs, %0" 
                    : "=r"(selector));
    return selector;
}

static inline gdtr_t read_gdtr() {
    gdtr_t gdtr = { .limit = 0, .base = 0 };
    asm volatile("sgdt %0" : "=m"(gdtr));
    return gdtr;
}

static inline idtr_t read_idtr() {
    idtr_t idtr = { .limit = 0, .base = 0 };
    asm volatile("sidt %0" : "=m"(idtr));
    return idtr;
}

static inline qword_t read_rflags() {
    qword_t rflags = 0;
    asm volatile("pushf; pop %0" : "=r"(rflags));
    return rflags;
}

static inline qword_t read_dr7() {
    qword_t dr7;
    asm volatile("mov %%dr7, %0" : "=r"(dr7));
    return dr7;
}

static inline void write_dr7(qword_t dr7) {
    asm volatile("mov %0, %%dr7" :: "r"(dr7));
}