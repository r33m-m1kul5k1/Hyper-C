#include "serial.h"

extern int protected_mode_pml4;

void initialize_machine()
{
    println("\nInitializing Machine");
    print_integer(protected_mode_pml4);
    print("\n");
    print_integer(0);
}