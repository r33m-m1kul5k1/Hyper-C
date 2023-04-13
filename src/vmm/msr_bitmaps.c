#include "vmm/msr_bitmaps.h"
#include "lib/log.h"

#define MAX_LOW_MSR 0x00001FFF
#define MIN_LOW_MSR 0x0
#define MAX_HIGH_MSR 0xC0001FFF
#define MIN_HIGH_MSR 0xC0000000
#define BITMAP_FOR_HIGH_MSRS 1024
#define BITMAP_FOR_WRMSR 2048

// The given msr is the bit's index inside the msr_bitmaps
void monitor_rdmsr(byte_t *msr_bitmaps, dword_t msr) {
    int msr_index = 0;

    if (!((MIN_LOW_MSR <= msr && msr <= MAX_LOW_MSR) || (MIN_HIGH_MSR <= msr && msr <=MAX_HIGH_MSR))) {
        PANIC("invalid MSR id");
    }

    msr_index = (MIN_LOW_MSR <= msr && msr <= MAX_LOW_MSR) ? msr / 8  : (msr - 0xC0000000) / 8 + BITMAP_FOR_HIGH_MSRS;
    
    LOG_DEBUG("msr byte: %x", msr_index);
    msr_bitmaps[msr_index] |= 1 << (msr & 0b0111);
}

void monitor_wrmsr(byte_t *msr_bitmaps, dword_t msr) {
    int msr_index = 0;

    if (!((MIN_LOW_MSR <= msr && msr <= MAX_LOW_MSR) || (MIN_HIGH_MSR <= msr && msr <=MAX_HIGH_MSR))) {
        PANIC("invalid MSR id");
    }

    msr_index = (MIN_LOW_MSR <= msr && msr <= MAX_LOW_MSR) ? 
            msr / 8 + BITMAP_FOR_WRMSR :
            (msr - 0xC0000000) / 8 + BITMAP_FOR_WRMSR + BITMAP_FOR_HIGH_MSRS;
    
    LOG_DEBUG("msr byte: %x", msr_index);
    msr_bitmaps[msr_index] |= 1 << (msr & 0b0111);
}