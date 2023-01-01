
# https://stackoverflow.com/questions/59895/how-do-i-get-the-directory-where-a-bash-script-is-located-from-within-the-script

SCRIPT_DIR=$(cd -- $(dirname ${BASH_SOURCE[0]}) &> /dev/null && pwd)
PROJECT_DIR=$SCRIPT_DIR/..

echo $PROJECT_DIR
make -C $PROJECT_DIR

qemu-system-x86_64 -cdrom $PROJECT_DIR/hypervisor.iso \
-chardev stdio,mux=on,id=char0 \
-mon chardev=char0,mode=readline \
-serial chardev:char0 \
-m 2G \
-drive file=$PROJECT_DIR/bootimage-CrabOS.bin,format=raw,media=cdrom \
