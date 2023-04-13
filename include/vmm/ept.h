/* 
SDM Intel :
EPT Pointer - 24.6.11
VMX capability MSR -> Appendix A.10
EPT in general -> 28.3
*/

#include <stdint.h>

#define TABLE_SIZE 1 << 9

struct __attribute__((packed, aligned(8))) {
    uint64_t read_access             : 1;
    uint64_t write_access            : 1;
    uint64_t supervisor_execute      : 1;
    uint64_t memory_type             : 3; // cache related
    uint64_t ignore_pat              : 1; // ignore page attribute
    uint64_t                         : 1;
    uint64_t dirty                   : 1;
    uint64_t user_execute            : 1;
    uint64_t                         : 1;
    uint64_t physical_address       : 36; // bit 12:47. The physical address length is 48 bits 0:47
    uint64_t                         : 4;  // must be zero
    uint64_t                         : 5;  // ignored
    uint64_t verify_guest_paging     : 1; 
    uint64_t paging_write_access     : 1; 
    uint64_t                         : 1; 
    uint64_t supervisor_shadow_stack : 1; 
    uint64_t sub_page_write_prems    : 1;
    uint64_t                         : 1; 
    uint64_t supress_ve              : 1; 
} typedef ept_entry_t;


struct __attribute__((packed, aligned(8))) {
    uint64_t memory_type                      : 3;
    uint64_t ept_walk_length                  : 3;
    uint64_t enable_dirty_pages               : 1;
    uint64_t enable_shadow_stack_enforcement  : 1;
    uint64_t                                  : 4;
    uint64_t ept_pml4_physical_address       : 36;
} typedef eptp_t;

struct __attribute__((packed, aligned(4096))) {
    ept_entry_t pml4[TABLE_SIZE];
    ept_entry_t pdpt[TABLE_SIZE];
    ept_entry_t pd[TABLE_SIZE];
    ept_entry_t pt[TABLE_SIZE];
} typedef extended_paging_tables_t;