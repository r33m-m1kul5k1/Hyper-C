#pragma once


void enter_vmx_root();
void initialize_vmx_regions(char* vmxon_region, char* vmcs_region);
void configure_vmcs();
