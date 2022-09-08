
# Intel VT

## VMX

**VMM**, Virtual Machine Monitor, is the hypervisor. it and the guest software run in the **VMX operation**, with gives them new registers and/or instructions.

Every instruction the VMM does called **VMX root**, and the guest software inscturctions called **VMX non-root**.

You can move between the guest and the VMM using **VMX trasitions**. going into the guest software mode called **VM entries**, and getting out **VM exists**

**VMCS** is a configuration structure for VMs processors.

## APIC virtualization & Virtual interrupts

## EPT - Extended Page Table

## VPID - virtual processor ID
