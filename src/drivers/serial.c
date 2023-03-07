#include "drivers/serial.h"
#include "hardware/miscellaneous.h"

void serial_print(char* buffer)
{
    for (; *buffer != '\0'; buffer++)
    {
        out(COM1, *buffer);
    }
}