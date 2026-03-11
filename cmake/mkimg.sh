OUTDIR=$1
CONF=$2
ELF=$3
LIMINE=$4
LIMINE_BIOS_SYS=$5
TMP=$(mktemp)
IMG=$OUTDIR/loom.img

rm -f $IMG

fallocate -l 64MiB $TMP
mkfs.fat -F 32 $TMP
mcopy -i $TMP $CONF ::/limine.conf
mcopy -i $TMP $LIMINE_BIOS_SYS ::/limine-bios.sys
mcopy -i $TMP $ELF ::/loom.elf

fallocate -l 64MiB $IMG
cat $TMP >> $IMG
parted -s -- $IMG mklabel msdos mkpart primary fat32 64MiB 100% \
            set 1 esp on 
$LIMINE bios-install $IMG

rm $TMP