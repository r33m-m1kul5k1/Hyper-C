#!/bin/bash
# https://stackoverflow.com/questions/59895/how-do-i-get-the-directory-where-a-bash-script-is-located-from-within-the-script

SCRIPT_DIR=$(cd -- $(dirname ${BASH_SOURCE[0]}) &> /dev/null && pwd)
PROJECT_DIR=$SCRIPT_DIR/..
BUILD_DIR=$PROJECT_DIR/build

make clean -C $PROJECT_DIR
make -C $PROJECT_DIR

qemu-system-x86_64 -cdrom $BUILD_DIR/hypervisor.iso \
-chardev stdio,mux=on,id=char0 \
-mon chardev=char0,mode=readline \
-serial chardev:char0 \
-m 4G \
-drive file=$BUILD_DIR/test.txt,format=raw \
-cpu host -enable-kvm \

