#include "serial.h"

void initialize_machine()
{
    println("\ninitializing machine");
    int* pml_pointer = 0x10000;
    print_integer(*pml_pointer);
}