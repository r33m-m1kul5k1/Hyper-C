#include "lib/utils.h"

void strcpy(char *dst, const char *src)
{
    for (;*src != '\0'; src++, dst++)
    {
        *dst = *src;
    }
    *dst = '\0';
}

int strlen(const char *str)
{
    int length = 0;
    while(str[length])
        length++;

    return length;
}

// Copies `count` bytes from `src` to `dst` 
void* memcpy( void *dst, const void *src, size_t count )
{
    for (int i = 0; i < count; i++)
    {
        ((char*)dst)[i] = ((char*)src)[i];
    }

    return dst;
}

const char *get_number_start(const char *str)
{
    if (strlen(str) == 1)
    {
        return str;
    }

    while (*str == '0')
    {
        str++;
    }

    return str;
}