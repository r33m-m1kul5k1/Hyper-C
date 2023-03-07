
static inline void vmxon(void *vmxon_region) {
    asm volatile("vmxon %0"::"m" (*(char*)vmxon_region));
}