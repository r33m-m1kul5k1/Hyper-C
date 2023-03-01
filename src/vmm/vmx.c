#include "hardware/instructions.h"
#include "lib/log.h"
#include "lib/utils.h"


#define CR4_VMX_ENABLE (1 << 13)
#define CR0_NE_ENABLE (1 << 5)

// FIXED0 -> fixed ones
#define IA32_VMX_CR0_FIXED0 0x486
#define IA32_VMX_CR0_FIXED1 0x487
#define IA32_VMX_CR4_FIXED0 0x488
#define IA32_VMX_CR4_FIXED1 0x489

#define IA32_VMX_BASIC 0x480
#define IA32_FEATURE_CONTROL 0x3a

void enter_vmx_root() {

    
    // Note that PE & PG should be 0 with "unrestricted guest".
    write_cr0((read_cr0() | CR0_NE_ENABLE | read_msr(IA32_VMX_CR0_FIXED0)) & read_msr(IA32_VMX_CR0_FIXED1));
    write_cr4((read_cr4() | CR4_VMX_ENABLE | read_msr(IA32_VMX_CR4_FIXED0)) & read_msr(IA32_VMX_CR4_FIXED1));
}
