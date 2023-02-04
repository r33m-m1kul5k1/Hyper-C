#pragma once
#include <stdarg.h>

#define MAXIMUM_BINARY_LENGTH 64


void sprint_integer(char *str, int number, int base);
void sprintf(char *str, const char *fmt, ...);
void vsprintf(char *str, const char *fmt, va_list arg);