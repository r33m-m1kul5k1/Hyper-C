#include "lib/log.h"
#include "drivers/serial.h"
#include "lib/print.h"

static log_level min_level = DEBUG_LEVEL;
static char *level_strings[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};

void set_log_level(log_level level) 
{
    min_level = level;
}

void log_log(log_level level, const char *file, int line, const char *fmt, ...)
{
    if (level < min_level)
    {
        return;
    }

    char buffer[0x100];
    char log_message[0x400];
    va_list args;

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    sprintf(log_message, "[%s] %s:%d: %s\n", level_strings[level], file, line, buffer);


    serial_print(log_message);

}