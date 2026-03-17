CORES=4
qemu-system-x86_64 -drive format=raw,file=$1 -m 512M -nic none -smp sockets=1,cores=$CORES