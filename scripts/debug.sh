#!/bin/bash
SCRIPT_DIR=$(cd -- $(dirname ${BASH_SOURCE[0]}) &> /dev/null && pwd)
PROJECT_DIR=$SCRIPT_DIR/..
BUILD_DIR=$PROJECT_DIR/build

make clean
make -C $PROJECT_DIR

qemu-system-x86_64 -cdrom $BUILD_DIR/hypervisor.iso \
-chardev stdio,mux=on,id=char0 \
-mon chardev=char0,mode=readline \
-serial chardev:char0 \
-m 4G \
-s -S \
-cpu host -enable-kvm \
-drive file=$BUILD_DIR/test.txt,format=raw \
&
gdb
