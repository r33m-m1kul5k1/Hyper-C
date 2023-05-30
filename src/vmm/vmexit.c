#include "vmm/vmexit.h"
#include "hardware/vmcs.h"
#include "hardware/vmx.h"
#include "hardware/msr.h"
#include "lib/log.h"
#include "vmm/ept.h"
#include "vmm/msr_bitmaps.h"

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

handler_status_t vmcall_handler(guest_cpu_state_t *guest_state) {
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    switch (guest_state->registers.rdi) {
        case PROTECET_SECURE_PAGE:
            LOG_DEBUG("disabling read from the secure page");
            ept_flags_t secure_page_flags = { 
                                    .write_access = 1, 
                                    .supervisor_execute = 1,
                                    .memory_type = EPT_MEMORY_TYPE_WRITEBACK,
                                    };
            update_gpa_access_rights(&guest_state->cpu_data->epts, (qword_t)guest_state->secure_page, &secure_page_flags);
            break;

        case PROTECET_SYSCALL:
            LOG_DEBUG("protecting IA32_SYSENTER_EIP msr from writes");
            monitor_wrmsr(guest_state->cpu_data->msr_bitmaps, MSR_IA32_SYSENTER_EIP);
            break;

        default:
            LOG_ERROR("unsupported vmcall");
            return HANDLER_FAILURE;
    }

    return HANDLER_SUCCESS;
}

