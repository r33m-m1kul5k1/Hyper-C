#include "lib/log.h"
#include "guest/syscalls.h"


void lstar_hook(unsigned int number) {

    LOG_INFO("ssdt at entry %d was injected", number);

    dispatch_handler(number);
}