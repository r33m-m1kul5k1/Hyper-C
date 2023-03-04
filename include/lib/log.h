#pragma once 
#include <stdarg.h>

typedef enum log_level { 
    DEBUG_LEVEL,
    INFO_LEVEL, 
    WARN_LEVEL, 
    ERROR_LEVEL, 
    FATAL_LEVEL,
} log_level;

#define PANIC() {                     \
    LOG_FATAL("Hypervisor paniced");  \
    while (1) {                       \
        asm volatile("hlt;");         \
    }                                 \
}

#define STR_VALUE(arg) #arg

#define ASSERT(condition) {                                         \
    if (!condition) {                                               \
        LOG_ERROR("Assertion failed: %s", STR_VALUE(condition));    \
        PANIC();                                                    \
    }                                                               \
}

#define LOG_DEBUG(...) log_log(DEBUG_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_log(INFO_LEVEL,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_log(WARN_LEVEL,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_log(ERROR_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_log(FATAL_LEVEL, __FILE__, __LINE__, __VA_ARGS__)

void set_log_level(log_level level);
void log_log(log_level level, const char *file, int line, const char *fmt, ...);