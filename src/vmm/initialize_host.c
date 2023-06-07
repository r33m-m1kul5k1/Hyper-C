#include "vmm/vmm.h"
#include "lib/utils.h"
#include "lib/log.h"
#include "hardware/types.h"
#include "guest/kmain.h"

#define CPU_DATA_ADDRESS 0x01000000
#define REAL_MODE_BASE_ADDRESS 0x7E00
#define DAP_ADDRESS 0x500
#define DRIVE_NUMBER_ADDRESS 0x600
#define DRIVE_A 0x80

extern void real_mode_start();
extern void real_mode_end();
extern void real_mode_callback(void (*)());
extern void read_disk();
extern void real_mode_smile();
extern void bios_mmap();

void initialize_bios();
void load_mbr();
void initialize_ssdt(ssdt_t *ssdt);
void initialize_read_hook(extended_paging_tables_t *epts, ssdt_t *ssdt);


void initialize_host() {
    set_log_level(DEBUG_LEVEL);
    LOG_INFO("initializing machine");

    cpu_data_t *cpu_data = (cpu_data_t *)CPU_DATA_ADDRESS;
    cpu_data->guest_cpu_state.cpu_data = cpu_data; // what a lovely loop :)
    LOG_INFO("VMM memory size is %x and its layout is as follows:", sizeof(cpu_data_t));
    LOG_INFO("[vmxon region]: %u", cpu_data->vmxon_region);
    LOG_INFO("[vmcs]: %u", cpu_data->vmcs);
    LOG_INFO("[extended paging tables]: %u", &cpu_data->epts);
    LOG_INFO("[msr bitmaps]: %u", cpu_data->msr_bitmaps);
    LOG_INFO("[guest registers]: %u", &cpu_data->guest_cpu_state.registers);
    LOG_INFO("[guest stack]: %u", cpu_data->guest_cpu_state.stack_top);
    LOG_INFO("[guest ssdt]: %u", &cpu_data->guest_cpu_state.ssdt);
    
    initialize_bios();
    load_mbr();
    enter_vmx_root(cpu_data);
    configure_vmcs(cpu_data);
    initialize_ssdt(&cpu_data->guest_cpu_state.ssdt);
    // initialize_read_hook(&cpu_data->epts, &cpu_data->guest_cpu_state.ssdt);
    launch_vm();
}

/*
Initialize communication with the bios
*/
void initialize_bios() {
    
    LOG_INFO(
        "copying real mode code from [%p - %p] to the 'low memory' at %x", 
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

    int *drive_number_ptr = (int *)DRIVE_NUMBER_ADDRESS;
    disk_address_packet_t *dap_ptr = (disk_address_packet_t *)DAP_ADDRESS;
    char *mbr = (char *)0x7c00;

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

void initialize_ssdt(ssdt_t *ssdt) {
    // memset((void *)ssdt, 0, 0x1000);
    set_syscall_handler(42, print_a_crab);
}

void initialize_read_hook(extended_paging_tables_t *epts, ssdt_t *ssdt) {
    ept_flags_t ssdt_page_flags = { 
                                    .read_access = 0,
                                    .write_access = 1, 
                                    .memory_type = EPT_MEMORY_TYPE_WRITEBACK,
                                    };

    update_gpa_access_rights(epts, (qword_t)ssdt, &ssdt_page_flags);
}