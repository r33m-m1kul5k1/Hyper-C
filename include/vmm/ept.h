/* 
SDM Intel :
EPT Pointer - 24.6.11
VMX capability MSR -> Appendix A.10
EPT in general -> 28.3
*/

#include "hardware/types.h"

#define TABLE_SIZE 1 << 9

struct __attribute__((packed, aligned(8))) {
    qword_t read_access             : 1;
    qword_t write_access            : 1;
    qword_t supervisor_execute      : 1;
    qword_t memory_type             : 3; // cache related
    qword_t ignore_pat              : 1; // ignore page attribute
    qword_t                         : 1;
    qword_t dirty                   : 1;
    qword_t user_execute            : 1;
    qword_t                         : 1;
    qword_t physical_address       : 36; // bit 12:47. The physical address length is 48 bits 0:47
    qword_t                         : 4;  // must be zero
    qword_t                         : 5;  // ignored
    qword_t verify_guest_paging     : 1; 
    qword_t paging_write_access     : 1; 
    qword_t                         : 1; 
    qword_t supervisor_shadow_stack : 1; 
    qword_t sub_page_write_prems    : 1;
    qword_t                         : 1; 
    qword_t supress_ve              : 1; 
} typedef ept_entry_t;

struct __attribute__((packed, aligned(8))) {
    qword_t read_access             : 1;
    qword_t write_access            : 1;
    qword_t supervisor_execute      : 1;
    qword_t memory_type             : 3; // cache related
    qword_t ignore_pat              : 1; // ignore page attribute
    qword_t                         : 1;
    qword_t dirty                   : 1;
    qword_t user_execute            : 1;
    qword_t                         : 1;
    qword_t                         : 36; 
    qword_t                         : 4;  
    qword_t                         : 5;  
    qword_t verify_guest_paging     : 1; 
    qword_t paging_write_access     : 1; 
    qword_t                         : 1; 
    qword_t supervisor_shadow_stack : 1; 
    qword_t sub_page_write_prems    : 1;
    qword_t                         : 1; 
    qword_t supress_ve              : 1; 
} typedef ept_flags_t;

union {

    struct __attribute__((packed, aligned(8))) {
        qword_t memory_type                      : 3;
        qword_t ept_walk_length                  : 3;
        qword_t enable_dirty_pages               : 1;
        qword_t enable_shadow_stack_enforcement  : 1;
        qword_t                                  : 4;
        qword_t ept_pml4_physical_address       : 36;
    };
    qword_t qword_value;
} typedef eptp_t;

struct __attribute__((packed, aligned(4096))) {
    ept_entry_t pml4[TABLE_SIZE];
    ept_entry_t pdpt[TABLE_SIZE];
    ept_entry_t pd[TABLE_SIZE];
    ept_entry_t pt[TABLE_SIZE];
} typedef extended_paging_tables_t;

enum {
    EPT_MEMORY_TYPE_UNCACHEABLE = 0,
    EPT_MEMORY_TYPE_WRITEBACK = 6
} typedef ept_memory_type_t;

enum {
    EPT_PAGE_MAP_LEVEL_FOUR,
    EPT_PAGE_DIRECTORY_POINTER_TABLE,
    EPT_PAGE_DIRECTORY,
    EPT_PAGE_TABLE,
} typedef ept_table_level_t;

eptp_t initialize_extended_page_tables(extended_paging_tables_t *epts);
void ept_map(extended_paging_tables_t *epts, qword_t guest_physical_address, qword_t physical_address, ept_flags_t access_rights);