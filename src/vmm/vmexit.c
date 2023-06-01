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

    if (guest_state->registers.rcx == MSR_IA32_LSTAR) {
        // return value is EDX:EAX, memory size is 4 GiB so the biggest address can fit in eax.
        guest_state->registers.rax = (qword_t)lstar_hook;
        guest_state->registers.rdx = 0x0;
    }

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
    LOG_DEBUG("guest tried to access physical address at %u", physical_address);

    if ((qword_t)&guest_state->ssdt  <= physical_address && physical_address < (qword_t)&guest_state->ssdt + 0x1000) {
        LOG_DEBUG("the guest OS tried to read it's ssdt");
        // finds the offset of the ssdt
        guest_state->registers.rdi = (physical_address - ALIGN_DOWN(physical_address)) / sizeof(qword_t);
        guest_state->registers.rax = (qword_t)ept_hook;
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

        case VMM_ATTACK_LSTAR:
            LOG_DEBUG("hooking lstar msr");
            monitor_rdmsr(guest_state->cpu_data->msr_bitmaps, MSR_IA32_LSTAR);
            break;
        
        case PROTECT_SSDT: {
            LOG_DEBUG("protecting the ssdt page from writes");
            ept_flags_t ssdt_page_flags = { 
                                    .read_access = 1,
                                    .write_access = 0, 
                                    .memory_type = EPT_MEMORY_TYPE_WRITEBACK,
                                    };
            LOG_DEBUG("epts: %u", &guest_state->cpu_data->epts);
            LOG_DEBUG("ssdt: %u", &guest_state->ssdt);
            update_gpa_access_rights(&guest_state->cpu_data->epts, (qword_t)&guest_state->ssdt, &ssdt_page_flags);
            break;
        }
            
        default:
            LOG_ERROR("unsupported vmcall");
            return HANDLER_FAILURE;
    }

    return HANDLER_SUCCESS;
}

