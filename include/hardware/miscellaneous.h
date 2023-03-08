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