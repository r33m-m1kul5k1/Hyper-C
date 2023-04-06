#include <stdbool.h>
#include "lib/print.h"
#include "lib/log.h"
#include "lib/utils.h"
#include "drivers/serial.h"

const char *HEX_DIGITS = "0123456789ABCDEF";

char *add_number_prefix(char *str, int *number, int base, bool is_unsigned)
{
    if (*number < 0 && !is_unsigned)
    {
        *number *= -1;
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

    return str;
}

void sprint_integer(char *str, int number, int base, bool is_unsigned)
{
    int value = number;
    char number_buffer[MAXIMUM_BINARY_LENGTH];
    number_buffer[MAXIMUM_BINARY_LENGTH] = '\0';
    

    str = add_number_prefix(str, &value, base, is_unsigned);

    for (int i = MAXIMUM_BINARY_LENGTH - 1; i >= 0; i--)
    {
        number_buffer[i] = HEX_DIGITS[(unsigned)value % base];
        value = (unsigned)value / base;
    }

    if (number == 0)
        strcpy(str, "0");
    else
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
    char *buffer, c;
    int number;
    bool is_unsigned = false;

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
        
        case 'c':
            LOG_DEBUG("printing a character");
            c = (char)va_arg(args, int);
            *str = c;
            str++;
            break;

        case 's':
            buffer = va_arg(args, char*);
            strcpy(str, buffer);
            str += strlen(str);
            break;

        case 'd':
            number = va_arg(args, int);
            sprint_integer(str, number, 10, is_unsigned);
            str += strlen(str);
            break;

        case 'b':
            binary_print:
            number = va_arg(args, int);
            sprint_integer(str, number, 2, is_unsigned);
            str += strlen(str);

            break;

        case 'x':
            hex_print:
            number = va_arg(args, int);
            sprint_integer(str, number, 16, is_unsigned);
            str += strlen(str);
            break;

        case 'p':
            goto hex_print;
            break;
        
        case 'u':
            is_unsigned = true;
            goto hex_print;
            break;
        
        case 'U':
            is_unsigned = true;
            goto binary_print;
            break;
            
        default:
            LOG_ERROR("unknown format character");
            break;
        }
    }

    *str = '\0';
}