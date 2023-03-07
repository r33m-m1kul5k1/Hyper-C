#pragma once

enum {
    // rflags == 0
    vm_success,
    // carry flag is set
    vm_fail_invalid,
    // zero flag is set, the vmcs pointer is valid
    vm_fail_valid,
} typedef vmx_error_codes_t;