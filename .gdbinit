file build/iso/boot/hypervisor
target remote localhost:1234
b _start
c
define hook-stop
x/2i $rip
end