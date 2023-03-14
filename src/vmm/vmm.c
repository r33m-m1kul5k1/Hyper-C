#include "vmm/vmm.h"
#include "hardware/registers.h"
#include "hardware/vmcs.h"
#include "hardware/types.h"
#include "hardware/vmx.h"
#include "hardware/error_codes.h"
#include "hardware/msr.h"
#include "lib/log.h"
#include "lib/utils.h"

#define CR4_VMX_ENABLE (1 << 13)
#define CR0_NE_ENABLE (1 << 5)

#define IA32_FEATURE_CONTROL_LOCK_BIT (1 << 0)
#define IA32_FEATURE_CONTROL_ENABLE_VMXON_INSIDE_SMX (1 << 1)
#define IA32_FEATURE_CONTROL_ENABLE_VMXON_OUTSIDE_SMX (1 << 2)

#define VMXON_REGION_ADDRESS 0x10000
#define VMCS_REGION_ADDRESS 0x11000
#define PAGE_FRAME_SIZE 0x1000
#define CANONICAL_ADDRESS 0xffffffff
#define CANONICAL_SELECTOR 0xff

#define STACK_TOP 0x7BFF
#define DESCRIPTOR_MAX_LIMIT 0xfffff
#define GUEST_DS_ACCESS_RIGHTS 0xc093
#define GUEST_CS_ACCESS_RIGHTS 0xa09b

vm_instruction_error_t check_vm_instruction_error();
dword_t get_default_bits(dword_t defualt_bits_msr, dword_t true_defualt_bits_msr);
void vm_exit_handler();


void enter_vmx_root() {    
    // Note that PE & PG should be 0 with "unrestricted guest".
    write_cr0((read_cr0() | CR0_NE_ENABLE | read_msr(MSR_IA32_VMX_CR0_FIXED0)) & read_msr(MSR_IA32_VMX_CR0_FIXED1));
    write_cr4((read_cr4() | CR4_VMX_ENABLE | read_msr(MSR_IA32_VMX_CR4_FIXED0)) & read_msr(MSR_IA32_VMX_CR4_FIXED1));
    // dr7 ?
    int feature_control_msr = read_msr(MSR_IA32_FEATURE_CONTROL);
    LOG_DEBUG("IA32_FEATURE_CONTROL: %b", feature_control_msr);

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
    

    initialize_vmx_regions((char *)VMXON_REGION_ADDRESS, (char *)VMCS_REGION_ADDRESS);
    
    ASSERT(vmxon((void *)VMXON_REGION_ADDRESS) == VM_SUCCESS);
    ASSERT(vmclear((void *)VMCS_REGION_ADDRESS) == VM_SUCCESS);
    ASSERT(vmptrld((void *)VMCS_REGION_ADDRESS) == VM_SUCCESS);
    
    LOG_INFO("entered VMX-root operation");
}

void initialize_vmx_regions(char* vmxon_region, char* vmcs_region) {

    memset((void *)vmxon_region, 0, PAGE_FRAME_SIZE);
    memset((void *)vmcs_region, 0, PAGE_FRAME_SIZE);

    *(dword_t *)vmxon_region = read_msr(MSR_IA32_VMX_BASIC);
    *(dword_t *)vmcs_region = read_msr(MSR_IA32_VMX_BASIC);
}

void configure_vmcs() {
    dword_t default_bits;
    // pin base VM-execution control fields
    default_bits = get_default_bits(MSR_IA32_VMX_PINBASED_CTLS, MSR_IA32_VMX_TRUE_PINBASED_CTLS);
    // bits 1, 2, 4 must contain inside default1
    ASSERT(BIT_N(default_bits, 1) && BIT_N(default_bits, 2) && BIT_N(default_bits, 4));
    vmwrite(VMCS_PIN_BASED_VM_EXEC_CONTROL, default_bits);

    // primary-processor based VM-execution control fields
    default_bits = get_default_bits(MSR_IA32_VMX_PROCBASED_CTLS, MSR_IA32_VMX_TRUE_PROCBASED_CTLS);
    vmwrite(VMCS_PIN_BASED_VM_EXEC_CONTROL, default_bits);
    // CR3-target controls 24.6.7, `mov cr3` will cause VM exit at default

    // VM-exit control fields
    default_bits = get_default_bits(MSR_IA32_VMX_EXIT_CTLS, MSR_IA32_VMX_TRUE_EXIT_CTLS);
    vmwrite(VMCS_VM_EXIT_CONTROLS, default_bits | VM_EXIT_IA32E_MODE); // load & save efer?

    // VM-entry control fields
    default_bits = get_default_bits(MSR_IA32_VMX_ENTRY_CTLS, MSR_IA32_VMX_TRUE_ENTRY_CTLS);
    vmwrite(VMCS_VM_ENTRY_CONTROLS, default_bits | VM_ENTRY_IA32E_MODE);

    // Host state area
    vmwrite(VMCS_HOST_CR0, read_cr0());
    vmwrite(VMCS_HOST_CR4, read_cr4());
    vmwrite(VMCS_HOST_CR3, read_cr3());
    vmwrite(VMCS_HOST_RIP, (qword_t)vm_exit_handler);
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
    // not used by the hv
    vmwrite(VMCS_HOST_FS_BASE, 0);
    vmwrite(VMCS_HOST_GS_BASE, 0);
    // https://www.felixcloutier.com/x86/ltr
    vmwrite(VMCS_HOST_TR_BASE, read_ds());
    vmwrite(VMCS_HOST_GDTR_BASE, read_gdtr_base());
    vmwrite(VMCS_HOST_IDTR_BASE, 0);

    // Guest state area
    vmwrite(VMCS_GUEST_CR0, read_cr0());
    vmwrite(VMCS_GUEST_CR3, read_cr3());
    vmwrite(VMCS_GUEST_CR4, read_cr4());
    vmwrite(VMCS_HOST_SYSENTER_EIP, CANONICAL_ADDRESS);
    vmwrite(VMCS_HOST_SYSENTER_ESP, CANONICAL_ADDRESS);

    vmwrite(VMCS_GUEST_CS_SELECTOR, read_cs());
    vmwrite(VMCS_GUEST_CS_BASE, 0);
    vmwrite(VMCS_GUEST_CS_LIMIT, DESCRIPTOR_MAX_LIMIT);
    vmwrite(VMCS_GUEST_CS_AR_BYTES, GUEST_CS_ACCESS_RIGHTS);

    vmwrite(VMCS_GUEST_SS_SELECTOR, read_ss());
    vmwrite(VMCS_GUEST_DS_SELECTOR, read_ds());
    vmwrite(VMCS_GUEST_ES_SELECTOR, read_es());
    vmwrite(VMCS_GUEST_FS_SELECTOR, read_fs());
    vmwrite(VMCS_GUEST_GS_SELECTOR, read_gs());
    vmwrite(VMCS_GUEST_TR_SELECTOR, read_ds());
    vmwrite(VMCS_GUEST_LDTR_SELECTOR, read_ds());
    vmwrite(VMCS_GUEST_TR_SELECTOR, read_ds());
    LOG_DEBUG("current VM-instruction error: %s", VM_INSTRUCTION_ERROR_STRINGS[check_vm_instruction_error()]);

}

dword_t get_default_bits(dword_t defualt_bits_msr, dword_t true_defualt_bits_msr) {
    dword_t default1 = read_msr(defualt_bits_msr);
    dword_t default0 = read_msr(defualt_bits_msr) >> 32;

    if (BIT_N(read_msr(MSR_IA32_VMX_BASIC), 55)) {
        default1 = read_msr(true_defualt_bits_msr);
        default0 = read_msr(true_defualt_bits_msr) >> 32;    
    }
    return default1 & default0;
}

vm_instruction_error_t check_vm_instruction_error() {
    qword_t error_code = UNDEFINE_VM_INSTRUCTION_ERROR;
    vmread(VMCS_VM_INSTRUCTION_ERROR, &error_code);

    return (vm_instruction_error_t)error_code;
}

void vm_exit_handler() {
    LOG_INFO("VM-exit occurred");
}