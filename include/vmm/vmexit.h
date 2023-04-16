#pragma once

#include "vmm/vmm.h"

enum {
    HANDLER_SUCCESS,
    HANDLER_FAILURE,
} typedef handler_status_t;

handler_status_t halt_handler(guest_cpu_state_t *guest_state);
handler_status_t rdmsr_handler(guest_cpu_state_t *guest_state);
handler_status_t wrmsr_handler(guest_cpu_state_t *guest_state);
handler_status_t ept_violation_handler(guest_cpu_state_t *guest_state);