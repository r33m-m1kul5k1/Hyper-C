
#ifndef SERIAL_H
#define SERIAL_H

#define COM1 0x3F8


void print(char* buffer);
void println(char* buffer);
void putchar(char c);
void print_integer(int integer);
int reverse_integer(int integer);

#endif