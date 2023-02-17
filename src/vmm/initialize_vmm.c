#include "lib/utils.h"
#include "lib/log.h"

#define REAL_MODE_BASE_ADDRESS 0x7E00

extern void real_mode_start();
extern void real_mode_end();
extern void real_mode_callback(void (*)());
extern void read_disk();

void initialize_vmm() {
    set_log_level(DEBUG_LEVEL);
    
    log_info("initializing machine");
    log_info("relocating real mode code from [%p - %p] to the 'low memory' at %x", real_mode_start, real_mode_end, REAL_MODE_BASE_ADDRESS);
 
    memcpy(
        (void*)REAL_MODE_BASE_ADDRESS,
        (void*)real_mode_start, 
        (size_t)(real_mode_end - real_mode_start)
    );

    real_mode_callback(read_disk);

}