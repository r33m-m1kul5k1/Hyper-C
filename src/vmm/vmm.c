#include "vmm/vmm.h"
#include "vmm/vmexit.h"
#include "vmm/msr_bitmaps.h"
#include "hardware/registers.h"
#include "hardware/miscellaneous.h"
#include "hardware/vmcs.h"
#include "hardware/types.h"
#include "hardware/vmx.h"
#include "hardware/exit_reason.h"
#include "hardware/msr.h"
#include "lib/log.h"
#include "lib/utils.h"

/* VMX related data */
#define CR4_VMX_ENABLE (1 << 13)
#define CR0_NE_ENABLE (1 << 5)
#define IA32_FEATURE_CONTROL_LOCK_BIT (1 << 0)
#define IA32_FEATURE_CONTROL_ENABLE_VMXON_INSIDE_SMX (1 << 1)
#define IA32_FEATURE_CONTROL_ENABLE_VMXON_OUTSIDE_SMX (1 << 2)
#define VMXON_REGION_ADDRESS 0x10000
#define VMCS_REGION_ADDRESS 0x11000

/* Memory related data */
#define STACK_TOP 0x7BFF
#define GUEST_REGISTERS_STATE 0x7FF00
#define EFER_MSR 0xC0000080

/* VMCS related data */
#define CANONICAL_ADDRESS 0xffffffff
#define CANONICAL_SELECTOR 0xff
#define VMCS_SELECTOR_UNUSABLE (1 << 16)
#define DESCRIPTOR_MAX_LIMIT 0xfffff
#define GUEST_DS_ACCESS_RIGHTS 0xc093
#define GUEST_CS_ACCESS_RIGHTS 0xa09b
#define GUEST_TR_ACCESS_RIGHTS 0xc08b
#define RFLAGS_DEFAULT0 0xfffd7fd7
#define RFLAGS_DEFAULT1 (1 << 1)
#define DEFAULT_INTERRUPTIBILITY_STATE 0UL
#define IA32_DEBUGCTL_DEFAULT0 0xffc3

vm_instruction_error_t check_vm_instruction_error();
dword_t get_default_bits(dword_t default_bits_msr, dword_t true_default_bits_msr);
void vmexit_handler();
void vmentry_handler();

extern void _vmexit_wrapper();

const char *VM_INSTRUCTION_ERROR_STRINGS[] = {
    [UNDEFINE_VM_INSTRUCTION_ERROR] = "UNDEFINE_VM_INSTRUCTION_ERROR",
    [VMCALL_EXECUTED_IN_VMX_ROOT_OPERATION] = "VMCALL_EXECUTED_IN_VMX_ROOT_OPERATION",
    [VMCLEAR_WITH_INVALID_PHYSICAL_ADDRESS] = "VMCLEAR_WITH_INVALID_PHYSICAL_ADDRESS",
    [VMCLEAR_WITH_VMXON_POINTER] = "VMCLEAR_WITH_VMXON_POINTER",
    [VMLAUNCH_WITH_NON_CLEAR_VMCS] = "VMLAUNCH_WITH_NON_CLEAR_VMCS",
    [VMRESUME_WITH_NON_LAUNCHED_VMCS] = "VMRESUME_WITH_NON_LAUNCHED_VMCS",
    [VMRESUME_AFTER_VMXOFF] = "VMRESUME_AFTER_VMXOFF",
    [VMENTRY_WITH_INVALID_CONTROL_FIELDS] = "VMENTRY_WITH_INVALID_CONTROL_FIELDS",
    [VM_ENTRY_WITH_INVALID_HOST_STATE_FIELDS] = "VM_ENTRY_WITH_INVALID_HOST_STATE_FIELDS",
    [VMPTRLD_WITH_INVALID_PHYSICAL_ADDRESS] = "VMPTRLD_WITH_INVALID_PHYSICAL_ADDRESS",
    [VMPTRLD_WITH_VMXON_POINTER] = "VMPTRLD_WITH_VMXON_POINTER",
    [VMPTRLD_WITH_INCORRECT_VMCS_REVISION_IDENTIFIER] = "VMPTRLD_WITH_INCORRECT_VMCS_REVISION_IDENTIFIER",
    [VMREAD_OR_VMWRITE_TO_UNSUPPORTED_VMCS_COMPONENT] = "VMREAD_OR_VMWRITE_TO_UNSUPPORTED_VMCS_COMPONENT",
    [VMWRITE_TO_READ_ONLY_VMCS_COMPONENT] = "VMWRITE_TO_READ_ONLY_VMCS_COMPONENT",
    [VMXON_EXECUTED_IN_VMX_ROOT_OPERATION] = "VMXON_EXECUTED_IN_VMX_ROOT_OPERATION",
    [VM_ENTRY_WITH_INVALID_EXECUTIVE_VMCS_POINTER] = "VM_ENTRY_WITH_INVALID_EXECUTIVE_VMCS_POINTER",
    [VM_ENTRY_WITH_NON_LAUNCHED_EXECUTIVE_VMCS] = "VM_ENTRY_WITH_NON_LAUNCHED_EXECUTIVE_VMCS",
    [VM_ENTRY_WITH_WXECUTIVE_VMCS_POINTER_NOT_VMXON_POINTER] = "VM_ENTRY_WITH_WXECUTIVE_VMCS_POINTER_NOT_VMXON_POINTER",
    [VMCALL_WITH_NON_CLEAR_VMCS] = "VMCALL_WITH_NON_CLEAR_VMCS",
    [VMCALL_WITH_INVALID_VM_EXIT_CONTROL_FIELDS] = "VMCALL_WITH_INVALID_VM_EXIT_CONTROL_FIELDS",
    [VMCALL_WITH_INCORRECT_MSEG_REVISION_IDENTIFIER] = "VMCALL_WITH_INCORRECT_MSEG_REVISION_IDENTIFIER",
    [VMXOFF_UNSER_DUAL_MONITOR_TREATMENT] = "VMXOFF_UNSER_DUAL_MONITOR_TREATMENT",
    [VMCALL_WITH_INVALID_SMM_MONITOR] = "VMCALL_WITH_INVALID_SMM_MONITOR",
    [VM_ENTRY_WITH_INVALID_VM_EXECUTION_CONTROL_FIELDS] = "VM_ENTRY_WITH_INVALID_VM_EXECUTION_CONTROL_FIELDS",
    [VM_ENTRY_WITH_EVENTS_BLOCKED_BY_MOV_SS] = "VM_ENTRY_WITH_EVENTS_BLOCKED_BY_MOV_SS",
    [INVALID_OPERAND_TO_INVEPT_OR_IVVPID] = "INVALID_OPERAND_TO_INVEPT_OR_IVVPID",
};

// SDM 24.4.2
enum {
    CPU_STATE_ACTIVE,
    CPU_STATE_HLT,
    CPU_STATE_SHUTDOWN,
    CPU_STATE_WAIT_FOR_SIPI
} typedef activity_state_t;

void enter_vmx_root(cpu_data_t *cpu_data) {
    // Note that PE & PG should be 0 with "unrestricted guest".
    write_cr0((read_cr0() | CR0_NE_ENABLE | read_msr(MSR_IA32_VMX_CR0_FIXED0)) & read_msr(MSR_IA32_VMX_CR0_FIXED1));
    write_cr4((read_cr4() | CR4_VMX_ENABLE | read_msr(MSR_IA32_VMX_CR4_FIXED0)) & read_msr(MSR_IA32_VMX_CR4_FIXED1));
    
    int feature_control_msr = read_msr(MSR_IA32_FEATURE_CONTROL);
    if (feature_control_msr & IA32_FEATURE_CONTROL_LOCK_BIT) {
        // meaning the bios initialized the msr and locked it
        if (feature_control_msr & IA32_FEATURE_CONTROL_ENABLE_VMXON_INSIDE_SMX) {
            LOG_INFO("support of vmxon inside smx operation");
        } else if (feature_control_msr & IA32_FEATURE_CONTROL_ENABLE_VMXON_OUTSIDE_SMX) {
            LOG_INFO("support of vmxon outside smx operation");
        } else {
            PANIC("no support for vmxon instruction");
        }
    } else {
        write_msr(
            MSR_IA32_FEATURE_CONTROL,
            feature_control_msr | 
            IA32_FEATURE_CONTROL_LOCK_BIT |
            IA32_FEATURE_CONTROL_ENABLE_VMXON_INSIDE_SMX | 
            IA32_FEATURE_CONTROL_ENABLE_VMXON_OUTSIDE_SMX
            );

        LOG_INFO("support of vmxon inside and outside of smx operation");
    }
    

    // see Intel SDM, 26.3.1.1, 5th and 6th points
    write_msr(MSR_IA32_DEBUGCTL, read_msr(MSR_IA32_DEBUGCTL) & IA32_DEBUGCTL_DEFAULT0);
	write_dr7(read_dr7() & 0xffffffff);
     
    // initialize vmm regions
    *(dword_t *)cpu_data->vmxon_region = read_msr(MSR_IA32_VMX_BASIC);
    *(dword_t *)cpu_data->vmcs = read_msr(MSR_IA32_VMX_BASIC);
    
    ASSERT(vmxon((void *)cpu_data->vmxon_region) == VM_SUCCESS);
    ASSERT(vmclear((void *)cpu_data->vmcs) == VM_SUCCESS);
    ASSERT(vmptrld((void *)cpu_data->vmcs) == VM_SUCCESS);
    
    LOG_INFO("entered VMX-root operation");
}

void configure_vmcs(cpu_data_t *cpu_data) {
    dword_t default_bits;
    
    // pin base VM-execution control fields
    default_bits = get_default_bits(MSR_IA32_VMX_PINBASED_CTLS, MSR_IA32_VMX_TRUE_PINBASED_CTLS);
    ASSERT(BIT_N(default_bits, 1) && BIT_N(default_bits, 2) && BIT_N(default_bits, 4));
    vmwrite(VMCS_PIN_BASED_VM_EXEC_CONTROL, default_bits);

    // primary-processor based VM-execution control fields
    default_bits = get_default_bits(MSR_IA32_VMX_PROCBASED_CTLS, MSR_IA32_VMX_TRUE_PROCBASED_CTLS);
    vmwrite(VMCS_CPU_BASED_VM_EXEC_CONTROL, default_bits | CPU_BASED_HLT_EXITING | CPU_BASED_ACTIVATE_MSR_BITMAP | CPU_BASED_ACTIVATE_SECONDARY_CONTROLS);
    
    // secondary-processor based VM-execution control fields
    default_bits = get_default_bits(MSR_IA32_VMX_PROCBASED_CTLS2, MSR_IA32_VMX_PROCBASED_CTLS2); // for this control there is no true default bits
    vmwrite(VMCS_SECONDARY_VM_EXEC_CONTROL, default_bits | CPU_BASED_CTL2_ENABLE_EPT | CPU_BASED_CTL2_UNRESTRICTED_GUEST);

    // VM-exit control fields
    default_bits = get_default_bits(MSR_IA32_VMX_EXIT_CTLS, MSR_IA32_VMX_TRUE_EXIT_CTLS);
    vmwrite(VMCS_VM_EXIT_CONTROLS, default_bits | VM_EXIT_IA32E_MODE);

    // VM-entry control fields
    default_bits = get_default_bits(MSR_IA32_VMX_ENTRY_CTLS, MSR_IA32_VMX_TRUE_ENTRY_CTLS);
    vmwrite(VMCS_VM_ENTRY_CONTROLS, default_bits | VM_ENTRY_IA32E_MODE);

    // Host guest_state area
    vmwrite(VMCS_HOST_CR0, read_cr0());
    vmwrite(VMCS_HOST_CR4, read_cr4());
    vmwrite(VMCS_HOST_CR3, read_cr3());
    vmwrite(VMCS_HOST_RIP, (qword_t)_vmexit_wrapper);
    vmwrite(VMCS_HOST_RSP, STACK_TOP);

    vmwrite(VMCS_HOST_SYSENTER_CS, CANONICAL_SELECTOR);
    vmwrite(VMCS_HOST_SYSENTER_EIP, CANONICAL_ADDRESS);
    vmwrite(VMCS_HOST_SYSENTER_ESP, CANONICAL_ADDRESS);
    
    vmwrite(VMCS_HOST_CS_SELECTOR, read_cs());
    vmwrite(VMCS_HOST_SS_SELECTOR, read_ss());
    vmwrite(VMCS_HOST_DS_SELECTOR, read_ds());
    vmwrite(VMCS_HOST_ES_SELECTOR, read_es());
    vmwrite(VMCS_HOST_FS_SELECTOR, read_fs());
    vmwrite(VMCS_HOST_GS_SELECTOR, read_gs());
    vmwrite(VMCS_HOST_TR_SELECTOR, read_ds());

    vmwrite(VMCS_HOST_FS_BASE, (qword_t)&cpu_data->guest_cpu_state);
    vmwrite(VMCS_HOST_GS_BASE, 0ULL);
    vmwrite(VMCS_HOST_TR_BASE, read_ds());
    gdtr_t gdtr = read_gdtr();
    idtr_t idtr = read_idtr();
    LOG_DEBUG("gdtr base: %x, limit: %x", gdtr.base, gdtr.limit);
    LOG_DEBUG("idtr base: %x, limit: %x", idtr.base, idtr.limit);
    vmwrite(VMCS_HOST_GDTR_BASE, gdtr.base);
    vmwrite(VMCS_HOST_IDTR_BASE, idtr.base);

    // Guest guest_state area
    vmwrite(VMCS_GUEST_CR0, read_cr0());
    vmwrite(VMCS_GUEST_CR3, read_cr3());
    vmwrite(VMCS_GUEST_CR4, read_cr4());
    vmwrite(VMCS_GUEST_DR7, read_dr7());
    vmwrite(VMCS_GUEST_RIP, (qword_t)vmentry_handler); 
    vmwrite(VMCS_GUEST_RSP, (qword_t)cpu_data->guest_cpu_state.stack_top);
    vmwrite(VMCS_GUEST_RFLAGS, (read_rflags() | RFLAGS_DEFAULT1) & RFLAGS_DEFAULT0);

    vmwrite(VMCS_GUEST_IA32_DEBUGCTL, read_msr(MSR_IA32_DEBUGCTL) & 0xffffffff);
    vmwrite(VMCS_GUEST_IA32_DEBUGCTL_HIGH, read_msr(MSR_IA32_DEBUGCTL) >> 32);

    vmwrite(VMCS_GUEST_SYSENTER_CS, read_cs());
    vmwrite(VMCS_GUEST_SYSENTER_EIP, CANONICAL_ADDRESS);
    vmwrite(VMCS_GUEST_SYSENTER_ESP, CANONICAL_ADDRESS);

    vmwrite(VMCS_GUEST_CS_SELECTOR, read_cs());
    vmwrite(VMCS_GUEST_CS_BASE, 0ULL);
    vmwrite(VMCS_GUEST_CS_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_CS_AR_BYTES, GUEST_CS_ACCESS_RIGHTS);

    vmwrite(VMCS_GUEST_SS_SELECTOR, read_ss());
    vmwrite(VMCS_GUEST_SS_BASE, 0ULL);
    vmwrite(VMCS_GUEST_SS_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_SS_AR_BYTES, GUEST_DS_ACCESS_RIGHTS);

    vmwrite(VMCS_GUEST_DS_SELECTOR, read_ds());
    vmwrite(VMCS_GUEST_DS_BASE, 0ULL);
    vmwrite(VMCS_GUEST_DS_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_DS_AR_BYTES, GUEST_DS_ACCESS_RIGHTS);
    
    vmwrite(VMCS_GUEST_ES_SELECTOR, read_es());
    vmwrite(VMCS_GUEST_ES_BASE, 0ULL);
    vmwrite(VMCS_GUEST_ES_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_ES_AR_BYTES, GUEST_DS_ACCESS_RIGHTS);

    vmwrite(VMCS_GUEST_FS_SELECTOR, read_fs());
    vmwrite(VMCS_GUEST_FS_BASE, 0ULL);
    vmwrite(VMCS_GUEST_FS_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_FS_AR_BYTES, GUEST_DS_ACCESS_RIGHTS);

    vmwrite(VMCS_GUEST_GS_SELECTOR, read_gs());
    vmwrite(VMCS_GUEST_GS_BASE, 0ULL);
    vmwrite(VMCS_GUEST_GS_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_GS_AR_BYTES, GUEST_DS_ACCESS_RIGHTS);

    vmwrite(VMCS_GUEST_TR_SELECTOR, read_ds());
    vmwrite(VMCS_GUEST_TR_BASE, 0ULL);
    vmwrite(VMCS_GUEST_TR_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_TR_AR_BYTES, GUEST_TR_ACCESS_RIGHTS);

    vmwrite(VMCS_GUEST_LDTR_SELECTOR, 0); // null selector to mark unusable
    vmwrite(VMCS_GUEST_LDTR_BASE, 0);
    vmwrite(VMCS_GUEST_LDTR_LIMIT, 0xff);
    vmwrite(VMCS_GUEST_LDTR_AR_BYTES, VMCS_SELECTOR_UNUSABLE);
    // Descriptor tables
    vmwrite(VMCS_GUEST_GDTR_BASE, gdtr.base);
    vmwrite(VMCS_GUEST_IDTR_LIMIT, gdtr.limit);
    vmwrite(VMCS_GUEST_IDTR_BASE, idtr.base);
    vmwrite(VMCS_GUEST_IDTR_LIMIT, idtr.limit);
    
    vmwrite(VMCS_GUEST_ACTIVITY_STATE, CPU_STATE_ACTIVE);
    vmwrite(VMCS_GUEST_INTERRUPTIBILITY_INFO, DEFAULT_INTERRUPTIBILITY_STATE);
    vmwrite(VMCS_VMCS_LINK_POINTER, -1ULL);

    vmwrite(VMCS_MSR_BITMAP, (qword_t)cpu_data->msr_bitmaps);
    monitor_rdmsr(cpu_data->msr_bitmaps, EFER_MSR);
    vmwrite(VMCS_EPT_POINTER, initialize_extended_page_tables(&cpu_data->epts).qword_value);
}

void vmexit_handler() {
    exit_reason_t exit_reason = { 0 };
    handler_status_t status = HANDLER_FAILURE;
    guest_cpu_state_t *guest_state = (guest_cpu_state_t *)vmread(VMCS_HOST_FS_BASE);

    vmread_with_ptr(VMCS_VM_EXIT_REASON, (qword_t *)&exit_reason);
    LOG_INFO("VM-exit reason: %s", EXIT_REASON_STRINGS[exit_reason.basic_exit_reason]);

    switch (exit_reason.basic_exit_reason) {
        case EXIT_REASON_HLT:
            status = halt_handler(guest_state);
            break;

        case EXIT_REASON_MSR_READ:
            status = rdmsr_handler(guest_state);
            break;

        case EXIT_REASON_MSR_WRITE:
            status = wrmsr_handler(guest_state);
            break;

        case EXIT_REASON_EPT_VIOLATION:
            status = ept_violation_handler(guest_state);
            break;
            
        default:
            PANIC("unsupported exit reason");
    }

    if (status == HANDLER_FAILURE) {
        PANIC("Faild to handle VM-exit");
    }
}

void vmentry_handler() {
    LOG_INFO("VM-entry occurred");
    read_msr(EFER_MSR);
    
    LOG_DEBUG("After handling rdmsr");
    asm volatile("hlt");
    while (1) {}
}

void launch_vm() {
    LOG_INFO("Launching the VM");
    vmlaunch();
    PANIC("VM launch faild, VM-instruction error: %s", VM_INSTRUCTION_ERROR_STRINGS[check_vm_instruction_error()]);
}

dword_t get_default_bits(dword_t default_bits_msr, dword_t true_default_bits_msr) {
    dword_t default1 = read_msr(default_bits_msr) & 0xffffffffUL;
    dword_t default0 = read_msr(default_bits_msr) >> 32;

    if (BIT_N(read_msr(MSR_IA32_VMX_BASIC), 55)) {
        default1 = read_msr(true_default_bits_msr) & 0xffffffffUL;
        default0 = read_msr(true_default_bits_msr) >> 32;    
    }

    return default1 & default0;
}

vm_instruction_error_t check_vm_instruction_error() {
    qword_t error_code = UNDEFINE_VM_INSTRUCTION_ERROR;
    error_code = vmread(VMCS_VM_INSTRUCTION_ERROR);

    return (vm_instruction_error_t)error_code;
}