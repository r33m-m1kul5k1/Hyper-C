
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "types.h"

/* 
inline means two things
1. the function will be copy past instead of called
2. the function implementation will stay in the header file
*/
__attribute__((always_inline))
void inline __out(DWORD port, BYTE data)
{
    asm volatile("out %1, %0" :: "d" (port), "a" (data));
}

#endif