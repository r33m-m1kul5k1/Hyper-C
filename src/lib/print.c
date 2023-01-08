#include "lib/print.h"
#include "lib/log.h"
#include "drivers/serial.h"

const char *HEX_DIGITS = "0123456789abcdef";

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

void sprint_integer(char *str, int number, int base)
{
    if (number < 0)
    {
        number *= -1;
        *str = '-';
        str++;
    }
    
    if (base == 2)
    {
        strcpy(str, "b");
        str += strlen(str);
    }
    else if (base == 16)
    {
        strcpy(str, "0x");
        str += strlen(str);
    }

    char number_buffer[MAXIMUM_BINARY_LENGTH];
    number_buffer[MAXIMUM_BINARY_LENGTH] = '\0';
    for (int i = MAXIMUM_BINARY_LENGTH - 1; i >= 0; i--)
    {
        number_buffer[i] = HEX_DIGITS[number % base];
        number = number / base;
    }

    strcpy(str, get_number_start(number_buffer));
}

void sprintf(char *str, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
}

void vsprintf(char *str, const char *fmt, va_list args)
{
    char *buffer;
    int number;

    for (;*fmt != '\0'; fmt++)
    {
        if (*fmt != '%')
        {
            *str = *fmt;
            str++;
            continue;
        }

        fmt++;
        switch (*fmt)
        {
        case 's':
            buffer = va_arg(args, char*);
            strcpy(str, buffer);
            str += strlen(str);

            break;

        case 'd':
            number = va_arg(args, int);
            sprint_integer(str, number, 10);
            str += strlen(str);
            break;

        case 'b':
            number = va_arg(args, int);
            sprint_integer(str, number, 2);
            str += strlen(str);

            break;

        case 'x':
            number = va_arg(args, int);
            sprint_integer(str, number, 16);
            str += strlen(str);
            break;

        default:
            log_error("unknown format character");
            break;
        }
    }

    *str = '\0';
}