#pragma once
#include <stdarg.h>
#include <stdbool.h>

#define MAXIMUM_BINARY_LENGTH 64


void sprint_integer(char *str, int number, int base, bool is_unsigned);
void sprintf(char *str, const char *fmt, ...);
void vsprintf(char *str, const char *fmt, va_list arg);