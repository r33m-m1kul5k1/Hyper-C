#include <stddef.h>
#include "lib/log.h"
#include "guest/syscalls.h"

void lstar_syscall_displacement() {
    LOG_INFO("Now this syscall is hooked by the VMM :)");
}

void lstar_hook(unsigned int number) {

    ssdt_t *ssdt = (ssdt_t *)SSDT_ADDRESS;
    ssdt->entries[number] = lstar_syscall_displacement;
    if (ssdt->entries[number] != lstar_syscall_displacement) {
        LOG_INFO("faild to inject the ssdt in entry %d", number);
    } else {
        LOG_INFO("ssdt at entry %d was injected", number);
    }

    dispatch_handler(number);
}

void ept_hook(unsigned int number) {
    LOG_INFO("Hooked read :)");
    dispatch_handler(number);
}