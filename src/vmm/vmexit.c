#include "vmm/vmexit.h"
#include "hardware/vmcs.h"
#include "hardware/vmx.h"
#include "hardware/msr.h"
#include "lib/log.h"
#include "vmm/ept.h"
#include "vmm/msr_bitmaps.h"
#include "vmm/hooks.h"

#define ALIGN_DOWN(address) ((address >> 12) << 12)

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
    qword_t physical_address = vmread(VMCS_GUEST_PHYSICAL_ADDRESS);
    LOG_DEBUG("tried to update guest physical address %u", physical_address);
    return HANDLER_SUCCESS;
}

handler_status_t ept_misconfig_handler(guest_cpu_state_t *guest_state) {
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    qword_t physical_address = vmread(VMCS_GUEST_PHYSICAL_ADDRESS);
    
    if ((qword_t)&guest_state->ssdt  <= physical_address && physical_address < (qword_t)&guest_state->ssdt + 0x1000) {
        // arg1 - the entry number
        guest_state->registers.rdi = (physical_address - ALIGN_DOWN(physical_address)) / sizeof(qword_t);
        // arg2 - the syscall handler
        guest_state->registers.rsi = *(qword_t *)physical_address;
        // read result is kept in rax
        guest_state->registers.rax = (qword_t)ssdt_hook;
    } else {
        LOG_DEBUG("guest tried to access physical address at %u", physical_address);
    }

    return HANDLER_SUCCESS;
}

handler_status_t vmcall_handler(guest_cpu_state_t *guest_state) {
    guest_state->registers.rip += vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    switch (guest_state->registers.rdi) {

        case PROTECET_SYSCALL:
            LOG_DEBUG("protecting IA32_SYSENTER_EIP and IA32_LSTAR msrs from writes");
            monitor_wrmsr(guest_state->cpu_data->msr_bitmaps, MSR_IA32_SYSENTER_EIP);
            monitor_wrmsr(guest_state->cpu_data->msr_bitmaps, MSR_IA32_LSTAR);
            break;
        
        default:
            LOG_ERROR("unsupported vmcall");
            return HANDLER_FAILURE;
    }

    return HANDLER_SUCCESS;
}

