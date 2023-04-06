/* SDM intel volume 3 chapter 24.6.9 and 25.1.3 */
#pragma once

#include "hardware/types.h"

void monitor_rdmsr(byte_t *msr_bitmap, dword_t msr);
void monitor_wrmsr(byte_t *msr_bitmap, dword_t msr);