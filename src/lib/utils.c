#include "lib/utils.h"
#define MAXIMUM_BINARY_LENGTH 64

int strcmp(const char *str1, const char *str2)
{
    if (strlen(str1) != strlen(str2))
        return *str1 - *str2;

    for (;*str1 != '\0'; str1++, str2++)
    {
        if (*str1 != *str2)
            return *str1 - *str2;
    }

    return 0;
}

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

// Remove leading zeros, note that the number must be positive.
const char *get_number_start(const char *str)
{
    while (*str == '0')
    {
        str++;
    }

    return str;
}