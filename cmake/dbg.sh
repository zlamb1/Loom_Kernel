ELF=$1
IMG=$2
CONF=$(dirname $(realpath "$0"))/gdb

qemu-system-x86_64 -drive format=raw,file=$IMG -m 512M -nic none -s -S &
gdb $ELF -q -x $CONF