#include "log.h"
#include "instructions.h"


#define COM1 0x3F8

void print(char* buffer)
{
    for (int i = 0; buffer[i] != '\0'; i++)
    {
        __out(COM1, buffer[i]);
    }
}