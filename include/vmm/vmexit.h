#pragma once

#include "vmm/vmm.h"

enum {
    HANDLER_SUCCESS,
    HANDLER_FAILURE,
} typedef handler_status_t;

handler_status_t halt_handler(cpu_state_t *state);
handler_status_t rdmsr_handler(cpu_state_t *state);
handler_status_t wrmsr_handler(cpu_state_t *state);