#!/bin/bash
SCRIPT_DIR=$(cd -- $(dirname ${BASH_SOURCE[0]}) &> /dev/null && pwd)
PROJECT_DIR=$SCRIPT_DIR/..
BUILD_DIR=$PROJECT_DIR/build

make -C $PROJECT_DIR

qemu-system-x86_64 -cdrom $BUILD_DIR/hypervisor.iso \
-chardev stdio,mux=on,id=char0 \
-mon chardev=char0,mode=readline \
-serial chardev:char0 \
-m 2G \
-drive file=$BUILD_DIR/test.txt,format=raw,media=cdrom \
-s -S

#-drive file=$BUILD_DIR/bootimage-CrabOS.bin,format=raw,media=cdrom \