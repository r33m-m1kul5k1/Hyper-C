file build/iso/boot/hypervisor
target remote localhost:1235
set disassembly-flavor intel
b _start
c
define hook-stop
x/1i $rip
end


define real_mode_breakpoint
    b *($arg0 - real_mode_start + 0x7e00)