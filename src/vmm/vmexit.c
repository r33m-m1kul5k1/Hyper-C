#include "vmm/vmexit.h"
#include "hardware/vmcs.h"
#include "hardware/vmx.h"
#include "lib/log.h"

handler_status_t halt_handler(registers_t *state) {
    LOG_DEBUG("handling halt");
    state->rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    return HANDLER_SUCCESS;
}

handler_status_t rdmsr_handler(registers_t *state) {
    state->rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("handling rdmsr from msr: %u", state->rcx);

    return HANDLER_SUCCESS;
}

handler_status_t wrmsr_handler(registers_t *state) {
    state->rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("handling wrmsr from msr: %u", state->rcx);

    return HANDLER_SUCCESS;
}