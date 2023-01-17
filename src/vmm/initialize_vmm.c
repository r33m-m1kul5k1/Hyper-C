#include "lib/utils.h"
#include "lib/log.h"
#include "drivers/serial.h"

extern int real_mode_start;
extern int real_mode_end;

void initialize_vmm()
{
    set_log_level(DEBUG_LEVEL);
    int* a = 0;
    int b = 240;
    a = &b;
    
    log_info("initializing machine");
    log_info("test %p", a);
    log_info("real mode code starts at: %p", real_mode_start);
    log_info("real mode code ends at: %p", real_mode_end);
}