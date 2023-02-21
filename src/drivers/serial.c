#include "drivers/serial.h"
#include "hardware/instructions.h"

void serial_print(char* buffer)
{
    for (; *buffer != '\0'; buffer++)
    {
        out(BOCH_DEBUG, *buffer);
    }
}