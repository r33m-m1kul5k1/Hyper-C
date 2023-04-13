#include "vmm/ept.h"
#include "lib/log.h"

#define EIGHT_BYTES 8

eptp_t initialize_extended_page_tables(extended_paging_tables_t* epts) {
    
    eptp_t eptp = { 0 };
    ASSERT((sizeof(ept_entry_t) == EIGHT_BYTES));
    ASSERT((sizeof(eptp_t) == EIGHT_BYTES));

    LOG_DEBUG("initialzing extended paging tables at %x", epts);

    return eptp;
}