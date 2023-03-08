#include "vmm.h"
#include "lib/utils.h"
#include "lib/log.h"
#include "hardware/types.h"

#define REAL_MODE_BASE_ADDRESS 0x7E00
#define DAP_ADDRESS 0x500
#define DRIVE_NUMBER_ADDRESS 0x600
#define DRIVE_A 0x80


extern void real_mode_start();
extern void real_mode_end();
extern void real_mode_callback(void (*)());
extern void read_disk();
extern void real_mode_smile();

void initialize_bios();
void load_mbr();


void initialize_host() {
    
    set_log_level(DEBUG_LEVEL);
    LOG_INFO("initializing machine");
    

    // initialize_bios();
    // load_mbr();

    enter_vmx_root();
    configure_vmcs();
}

/*
Initialize communication with the bios
*/
void initialize_bios() {
    
    LOG_INFO(
        "relocating real mode code from [%p - %p] to the 'low memory' at %x", 
        real_mode_start, 
        real_mode_end, 
        REAL_MODE_BASE_ADDRESS
    );
 
    memcpy(
        (void*)REAL_MODE_BASE_ADDRESS,
        (void*)real_mode_start, 
        (size_t)(real_mode_end - real_mode_start)
    );
    
    // tests communication
    real_mode_callback(real_mode_smile);
}

/*
Loads the Master Boot Record
*/
void load_mbr() {

    int *drive_number_ptr = (int*)DRIVE_NUMBER_ADDRESS;
    disk_address_packet_t *dap_ptr = (disk_address_packet_t*)DAP_ADDRESS;
    char *mbr = (char*)0x7c00;

    dap_ptr->size = 0x10;
    dap_ptr->unused = 0x0;
    dap_ptr->sector_count = 1;
    dap_ptr->destination_addr = 0x7c00;
    dap_ptr->destination_segment = 0x0;
    dap_ptr->sector_lba = 0x0;

    *drive_number_ptr = DRIVE_A;

    real_mode_callback(read_disk);

    LOG_INFO("mbr: %s", mbr);
    
}