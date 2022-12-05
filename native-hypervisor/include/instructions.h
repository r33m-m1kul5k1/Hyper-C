
typedef unsigned char BYTE;
typedef unsigned int DWORD;


__attribute__((always_inline))
void inline __out(DWORD port, BYTE data)
{
    asm volatile("out %1, %0" :: "d" (port), "a" (data));
}