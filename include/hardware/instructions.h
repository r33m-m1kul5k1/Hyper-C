
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "types.h"

/* 
Inline improves performance by copy and pasts the function contents
*/
__attribute__((always_inline))
void inline __out(DWORD port, BYTE data)
{
    /*
    volatile means the value may change, even if it doesn't appear to be modified,
    and the compiler should not make any optimizations
    */ 
    asm volatile("out %1, %0" :: "d" (port), "a" (data));
}

__attribute((always_inline))
void inline __rep_movsb(DWORD src, DWORD dst, DWORD size)
{
    //asm volatile("rep movsb");
}

#endif