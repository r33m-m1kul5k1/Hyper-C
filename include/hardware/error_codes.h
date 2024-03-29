#pragma once

enum {
    // rflags == 0
    VM_SUCCESS,
    // carry flag is set
    VM_FAIL_INVALID,
    // zero flag is set, the vmcs pointer is valid
    VM_FAIL_VALID,
} typedef vmx_error_codes_t;

// Intel manual appendix B.3.2 and volume 3 chapter 30.4
enum {
    UNDEFINE_VM_INSTRUCTION_ERROR,
    VMCALL_EXECUTED_IN_VMX_ROOT_OPERATION,
    VMCLEAR_WITH_INVALID_PHYSICAL_ADDRESS,
    VMCLEAR_WITH_VMXON_POINTER,
    VMLAUNCH_WITH_NON_CLEAR_VMCS,
    VMRESUME_WITH_NON_LAUNCHED_VMCS,
    VMRESUME_AFTER_VMXOFF,
    VMENTRY_WITH_INVALID_CONTROL_FIELDS,
    VM_ENTRY_WITH_INVALID_HOST_STATE_FIELDS,
    VMPTRLD_WITH_INVALID_PHYSICAL_ADDRESS,
    VMPTRLD_WITH_VMXON_POINTER,
    VMPTRLD_WITH_INCORRECT_VMCS_REVISION_IDENTIFIER,
    VMREAD_OR_VMWRITE_TO_UNSUPPORTED_VMCS_COMPONENT,
    VMWRITE_TO_READ_ONLY_VMCS_COMPONENT,
    VMXON_EXECUTED_IN_VMX_ROOT_OPERATION,
    VM_ENTRY_WITH_INVALID_EXECUTIVE_VMCS_POINTER,
    VM_ENTRY_WITH_NON_LAUNCHED_EXECUTIVE_VMCS,
    VM_ENTRY_WITH_WXECUTIVE_VMCS_POINTER_NOT_VMXON_POINTER,
    VMCALL_WITH_NON_CLEAR_VMCS,
    VMCALL_WITH_INVALID_VM_EXIT_CONTROL_FIELDS,
    VMCALL_WITH_INCORRECT_MSEG_REVISION_IDENTIFIER,
    VMXOFF_UNSER_DUAL_MONITOR_TREATMENT,
    VMCALL_WITH_INVALID_SMM_MONITOR,
    VM_ENTRY_WITH_INVALID_VM_EXECUTION_CONTROL_FIELDS,
    VM_ENTRY_WITH_EVENTS_BLOCKED_BY_MOV_SS,
    INVALID_OPERAND_TO_INVEPT_OR_IVVPID,
} typedef vm_instruction_error_t;



