#include "hardware/error_codes.h"

#define CARRY_FLAG 1
#define ZERO_FLAG (1 << 6)

static inline vmx_error_codes_t vmxon(void *vmxon_region) {
    qword_t flags;
    asm volatile("vmxon %1; pushf; pop %0" : "=r" (flags) : "m" (*(char*)vmxon_region));

    if (flags & CARRY_FLAG)
        return vm_fail_invalid;
    if (flags & ZERO_FLAG)
        return vm_fail_valid;
    return vm_success;

}

static inline vmx_error_codes_t vmclear(void *vmcs_region) {
    qword_t flags;
    asm volatile("vmclear %1; pushf; pop %0" : "=r" (flags) : "m" (*(char*)vmcs_region));
    
    if (flags & CARRY_FLAG)
        return vm_fail_invalid;
    if (flags & ZERO_FLAG)
        return vm_fail_valid;
    return vm_success;
}

static inline vmx_error_codes_t vmptrld(void *vmcs_region) {
    qword_t flags;
    asm volatile("vmptrld %1; pushf; pop %0" : "=r" (flags) : "m" (*(char*)vmcs_region));
    
    if (flags & CARRY_FLAG)
        return vm_fail_invalid;
    if (flags & ZERO_FLAG)
        return vm_fail_valid;
    return vm_success;
}