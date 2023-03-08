#pragma once
#include <stddef.h>

#define BIT_N(value, n) (value & 1LL << n) >> n

void strcpy(char *dst, const char *src);
int strcmp(const char *str1, const char *str2);
int strlen(const char *str);
void *memcpy( void *dest, const void *src, size_t count);
void *memset(void *str, int c, size_t n);
const char *get_number_start(const char *str);