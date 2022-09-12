
# Intel VT

## VMX

**VMM**, Virtual Machine Monitor, is the hypervisor. it and the guest software run in the **VMX operation**, with gives them new registers and/or instructions.

Every instruction the VMM does called **VMX root**, and the guest software instructions called **VMX non-root**.

You can move between the guest and the VMM using **VMX transitions**. going into the guest software mode called **VM entries**, and getting out **VM exists**

**VMCS** is a configuration structure for VMs processors.

## Instructions

### VMXON

1. check CPU support with `cpuid`
2. set the 13 bit in the CR4 register

**MSR**\
***0 the lock bit*** \
clear -> no vmx \
set -> the MSR cannot be modify and vmx is on. \
***1 enables VMXON in SMX operation*** \
clear -> no vmxon in SMX operation mode \
set -> meaning the processor supports vmx & smx. \
***2 enables VMXON outside SMX operation*** \
clear -> no vmx \
set -> the MSR cannot be modify and vmx is on. 

## SMX operation mode

creates a measured environment on a trust level.

## APIC virtualization & Virtual interrupts

## EPT - Extended Page Table

## VPID - virtual processor ID