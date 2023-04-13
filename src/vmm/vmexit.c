#include "vmm/vmexit.h"
#include "hardware/vmcs.h"
#include "hardware/vmx.h"
#include "lib/log.h"

handler_status_t halt_handler(cpu_state_t *state) {
    LOG_DEBUG("handling halt");
    state->guest_registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    return HANDLER_SUCCESS;
}

handler_status_t rdmsr_handler(cpu_state_t *state) {
    state->guest_registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("handling rdmsr from msr: %u", state->guest_registers.rcx);

    return HANDLER_SUCCESS;
}

handler_status_t wrmsr_handler(cpu_state_t *state) {
    state->guest_registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("handling wrmsr from msr: %u", state->guest_registers.rcx);
    
    return HANDLER_SUCCESS;
}