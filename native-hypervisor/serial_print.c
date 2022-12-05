#include "include/instructions.h"
#include "include/serial_print.h"
#define COM1 0x3F8

void print(char* buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        __out(COM1, buffer[i]);
    }
}