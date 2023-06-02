#include <stddef.h>
#include "lib/log.h"
#include "guest/syscalls.h"


void ssdt_hook(unsigned int number, syscall_handler_t handler) {
    LOG_INFO("hooked ssdt read at entry %d", number);
    handler();
}