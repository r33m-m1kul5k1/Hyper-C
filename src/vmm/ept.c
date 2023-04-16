#include "vmm/ept.h"
#include "lib/log.h"
#include <stddef.h>

#define EIGHT_BYTES 8
#define NESTED_TABLES_COUNT 4
#define PAGE_SIZE 0x1000


eptp_t initialize_extended_page_tables(extended_paging_tables_t *epts) {
    eptp_t eptp = { 0 };
    ept_flags_t default_flags = { 
                                  .read_access = 1, 
                                  .write_access = 1, 
                                  .supervisor_execute = 1, 
                                };
    // alignment checks
    ASSERT((sizeof(ept_entry_t) == EIGHT_BYTES));
    ASSERT((sizeof(eptp_t) == EIGHT_BYTES));

    // initialize the ept pointer
    eptp.memory_type = EPT_MEMORY_TYPE_WRITEBACK;
    eptp.ept_walk_length = NESTED_TABLES_COUNT -1;
    eptp.ept_pml4_physical_address = (qword_t)epts->pml4 >> 12;

    // initialize the ept pml4
    set_entry_address(&epts->pml4[0], (qword_t)epts->pdpt);
    set_entry_flags(&epts->pml4[0], &default_flags);

    // initialize ept pdp table 
    for (int i = 0; i < MEMORY_SIZE; i++) {
        set_entry_address(&epts->pdpt[i], (qword_t)&epts->pds[i * TABLE_SIZE]);
        set_entry_flags(&epts->pdpt[i], &default_flags);
    }

    // initialize ept pd tables
    for (int i = 0; i < MEMORY_SIZE * TABLE_SIZE; i++) {
        set_entry_address(&epts->pds[i], (qword_t)&epts->pts[i * TABLE_SIZE]);
        set_entry_flags(&epts->pds[i], &default_flags);
    }

    qword_t physical_address = 0;
    default_flags.memory_type = EPT_MEMORY_TYPE_WRITEBACK;
    for (int i = 0; i < MEMORY_SIZE * TABLE_SIZE * TABLE_SIZE; i++, physical_address += PAGE_SIZE) {
        set_entry_address(&epts->pts[i], physical_address);
        set_entry_flags(&epts->pts[i], &default_flags);
    }

    return eptp;
}

void update_gpa_access_rights(extended_paging_tables_t *epts, qword_t gpa, ept_flags_t *flags) {
    ASSERT(gpa % PAGE_SIZE == 0);
    size_t gpa_page = gpa / PAGE_SIZE;
    set_entry_flags(&epts->pts[gpa_page], flags);
}

void set_entry_address(ept_entry_t *entry, qword_t address) {
    entry->physical_address = address >> 12;
}
void set_entry_flags(ept_entry_t *entry, ept_flags_t *flags) {
    entry->read_access = flags->read_access;
    entry->write_access = flags->write_access;
    entry->supervisor_execute = flags->supervisor_execute;
    entry->memory_type = flags->memory_type;
    entry->ignore_pat = flags->ignore_pat;
    entry->dirty = flags->dirty;
    entry->user_execute = flags->user_execute;
    entry->verify_guest_paging = flags->verify_guest_paging;
    entry->paging_write_access = flags->paging_write_access;
    entry->supervisor_shadow_stack = flags->supervisor_shadow_stack;
    entry->sub_page_write_prems = flags->sub_page_write_prems;
    entry->supress_ve = flags->supress_ve;
}
