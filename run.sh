make
qemu-system-x86_64 -cdrom hypervisor.iso \
-chardev stdio,mux=on,id=char0 \
-mon chardev=char0,mode=readline \
-serial chardev:char0 \
-m 2G \
#-s -S