#pragma once 
#include "error_codes.h"
#include "types.h"

#define CARRY_FLAG 1
#define ZERO_FLAG (1 << 6)

static inline vmx_error_codes_t vmxon(void *vmxon_region) {
    qword_t flags;
    asm volatile("vmxon %1; pushf; pop %0" : "=r"(flags) : "m"(vmxon_region));

    if (flags & CARRY_FLAG)
        return VM_FAIL_INVALID;
    if (flags & ZERO_FLAG)
        return VM_FAIL_VALID;
    return VM_SUCCESS;

}

static inline void vmxoff() {
    asm volatile("vmxoff");
}

static inline vmx_error_codes_t vmclear(void *vmcs_region) {
    qword_t flags;
    asm volatile("vmclear %1; pushf; pop %0" : "=r"(flags) : "m"(vmcs_region));
    
    if (flags & CARRY_FLAG)
        return VM_FAIL_INVALID;
    if (flags & ZERO_FLAG)
        return VM_FAIL_VALID;
    return VM_SUCCESS;
}

static inline vmx_error_codes_t vmptrld(void *vmcs_region) {
    qword_t flags;
    asm volatile("vmptrld %1; pushf; pop %0" : "=r"(flags) : "m"(vmcs_region));
    
    if (flags & CARRY_FLAG)
        return VM_FAIL_INVALID;
    if (flags & ZERO_FLAG)
        return VM_FAIL_VALID;
    return VM_SUCCESS;
}

static inline void vmread(qword_t field, qword_t *value) {
    // note that the argument sequnce is swaped because we use AT&T format
    // format: operand src, dst
    asm volatile("vmread %1, %0" : "=rm"(*value) : "r"(field));
}

static inline void vmwrite(qword_t field, qword_t value) {
    asm volatile("vmwrite %0, %1" :: "rm"(value), "r"(field));
}

static inline vmx_error_codes_t vmlaunch() {
    qword_t flags;
    asm volatile("vmlaunch; pushf; pop %0" : "=r"(flags));
    
    if (flags & CARRY_FLAG)
        return VM_FAIL_INVALID;
    if (flags & ZERO_FLAG)
        return VM_FAIL_VALID;
    return VM_SUCCESS;
}

static inline vmx_error_codes_t vmresume() {
    qword_t flags;
    asm volatile("vmresume; pushf; pop %0" : "=r"(flags));
    
    if (flags & CARRY_FLAG)
        return VM_FAIL_INVALID;
    if (flags & ZERO_FLAG)
        return VM_FAIL_VALID;
    return VM_SUCCESS;
}
