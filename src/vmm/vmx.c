#include "hardware/instructions.h"
#include "lib/log.h"
#include "lib/utils.h"


#define CR4_VMX_ENABLE (1 << 13)
#define CR0_NE_ENABLE (1 << 5)
#define IA32_VMX_CR0_FIXED0 0x486
#define IA32_VMX_CR0_FIXED1 0x487
#define IA32_VMX_CR4_FIXED0 0x488
#define IA32_VMX_CR4_FIXED1 0x489
#define IA32_VMX_BASIC 0x480
#define IA32_FEATURE_CONTROL 0x3a

void enter_vmx_root() {
    
    qword_t value = read_msr(IA32_VMX_CR4_FIXED1);
    char test[] = "sup";
    memset((test+1), 0, 2);
    log_info("test: %s", test);
    log_info("IA32_VMX_CR4_FIXED1 value: %x", value);
}
