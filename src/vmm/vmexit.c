#include "vmm/vmexit.h"
#include "hardware/vmcs.h"
#include "hardware/vmx.h"
#include "lib/log.h"

handler_status_t halt_handler(registers_t *state) {
    int opcode_size = vmread(VMCS_VM_EXIT_INSTRUCTION_LEN);
    LOG_DEBUG("Handling halt, opcode size is: %d", opcode_size);
    state->rip += opcode_size;
    return HANDLER_SUCCESS;
}