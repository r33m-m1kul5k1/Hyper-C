#include "vmm/vmexit.h"
#include "hardware/vmcs.h"
#include "hardware/vmx.h"
#include "lib/log.h"

handler_status_t halt_handler(guest_cpu_state_t *guest_state) {
    LOG_DEBUG("handling halt");
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    return HANDLER_SUCCESS;
}

handler_status_t rdmsr_handler(guest_cpu_state_t *guest_state) {
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("handling rdmsr from msr: %u", guest_state->registers.rcx);

    return HANDLER_SUCCESS;
}

handler_status_t wrmsr_handler(guest_cpu_state_t *guest_state) {
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("handling wrmsr from msr: %u", guest_state->registers.rcx);
    
    return HANDLER_SUCCESS;
}

handler_status_t ept_violation_handler(guest_cpu_state_t *guest_state) {
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("guest physical address %u", vmread(VMCS_GUEST_PHYSICAL_ADDRESS));

    return HANDLER_SUCCESS;
}

handler_status_t ept_misconfig_handler(guest_cpu_state_t *guest_state) {
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("guest tried to access physical address at %u", vmread(VMCS_GUEST_PHYSICAL_ADDRESS));

    return HANDLER_SUCCESS;
}