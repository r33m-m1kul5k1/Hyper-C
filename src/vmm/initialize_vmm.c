#include "lib/log.h"
#include "drivers/serial.h"

void initialize_vmm()
{
    set_log_level(INFO_LEVEL);
    log_info("initializing machine %x", -0xff);
}