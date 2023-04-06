#pragma once

#include "vmm/vmm.h"

enum {
    HANDLER_SUCCESS,
    HANDLER_FAILURE,
} typedef handler_status_t;

handler_status_t halt_handler(registers_t *state);
handler_status_t rdmsr_handler(registers_t *state);
handler_status_t wrmsr_handler(registers_t *state);