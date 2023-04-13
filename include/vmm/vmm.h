#pragma once

#include "hardware/types.h"
#include "hardware/error_codes.h"
#include "vmm/ept.h"

#define PAGE_FRAME_SIZE 0x1000

struct __attribute__((packed, aligned(4096))) {
    qword_t rax;
    qword_t rbx;
    qword_t rcx;
    qword_t rdx;
    qword_t rsi;
    qword_t rdi;
    qword_t rbp;
    qword_t r8;
    qword_t r9;
    qword_t r10;
    qword_t r11;
    qword_t r12;
    qword_t r13;
    qword_t r14;
    qword_t r15;    
    qword_t rsp;    
    qword_t rip;    
} typedef registers_t;

// Note: allocation for this struct should be 4KiB aligned
// Moreover note that each item in the following struct is 4KiB aligned

struct {
    registers_t guest_registers;
    byte_t guest_stack_top[8 * PAGE_FRAME_SIZE];
} typedef cpu_state_t;

struct {
    byte_t vmxon_region[PAGE_FRAME_SIZE];
    byte_t vmcs[PAGE_FRAME_SIZE];
    cpu_state_t cpu_state;    
} typedef cpu_data_t;

struct {
    cpu_data_t cpu_data;
    extended_paging_tables_t epts;
    byte_t msr_bitmaps[PAGE_FRAME_SIZE];
} typedef vmm_data_t;

void enter_vmx_root(vmm_data_t *vmm_data);
void configure_vmcs(vmm_data_t *vmm_data);
void launch_vm();