#include "vmm/ept.h"
#include "lib/log.h"

#define EIGHT_BYTES 8
#define NESTED_TABLES_COUNT 4

eptp_t initialize_extended_page_tables(extended_paging_tables_t *epts) {
    eptp_t eptp = { 0 };
    // alignment checks
    ASSERT((sizeof(ept_entry_t) == EIGHT_BYTES));
    ASSERT((sizeof(eptp_t) == EIGHT_BYTES));


    eptp.memory_type = EPT_MEMORY_TYPE_WRITEBACK;
    eptp.ept_walk_length = NESTED_TABLES_COUNT -1;
    eptp.ept_pml4_physical_address = (qword_t)epts->pml4 >> 12;
    return eptp;
}

void ept_map(extended_paging_tables_t *epts, qword_t guest_physical_address, qword_t physical_address, ept_flags_t access_rights) {
    
}