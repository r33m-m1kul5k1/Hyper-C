#pragma once 
#include <stdarg.h>

typedef enum log_level { 
    DEBUG_LEVEL,
    INFO_LEVEL, 
    WARN_LEVEL, 
    ERROR_LEVEL, 
    FATAL_LEVEL,
} log_level;

#define log_debug(...) log_log(DEBUG_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(INFO_LEVEL,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(WARN_LEVEL,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(ERROR_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(FATAL_LEVEL, __FILE__, __LINE__, __VA_ARGS__)

void set_log_level(log_level level);
void log_log(log_level level, const char *file, int line, const char *fmt, ...);