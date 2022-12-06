#include "serial.h"
#include "instructions.h"


void print(char* buffer)
{
    for (int i = 0; buffer[i] != '\0'; i++)
    {
        __out(COM1, buffer[i]);
    }
}

void println(char* buffer)
{
    print(buffer);
    print("\n");
}

void putchar(char c)
{
    __out(COM1, c);
}

void print_integer(int integer)
{
    int reversed = 0, digit = 0;

    if (integer < 0)
    {
        integer *= -1;
        putchar('-');
    }

    reversed = reverse_integer(integer);
    while (reversed > 0)
    {
        digit = reversed % 10;
        putchar(digit + '0');
        reversed /= 10;
    }

    if (!integer)
        putchar('0');
}

int reverse_integer(int integer)
{
    int reversed = 0;
    while (integer > 0)
    {
        reversed *= 10;
        reversed += integer % 10;
        integer /= 10;
    }

    return reversed;
}