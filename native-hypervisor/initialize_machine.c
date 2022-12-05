
#define COM1 0x3F8

void initialize_machine()
{
    asm volatile("out %1, %0" :: "d" (COM1), "a" ('8'));
}