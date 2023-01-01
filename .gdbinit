file build/iso/boot/hypervisor
target remote localhost:1234
set disassembly-flavor intel
b _start
c
define hook-stop
x/1i $rip
end