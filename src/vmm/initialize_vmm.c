#include "lib/utils.h"
#include "lib/log.h"
#include "drivers/serial.h"

void initialize_vmm()
{
    set_log_level(DEBUG_LEVEL);
    char a = 'a', b = 'b';
    
    memcpy(&b, &a, 1);
    log_info("initializing machine %c", b);
}