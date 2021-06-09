READELF_INFO=`csky-abiv2-elf-readelf -A yoc.elf`
CPU_NAME=`echo $READELF_INFO | awk -F 'Tag_CSKY_CPU_NAME: "' '{print $NF}' | awk -F '" Tag_CSKY_ISA_FLAGS' '{print $1}'`

echo qemu-system-cskyv2 -machine smartl -cpu $CPU_NAME -nographic -kernel yoc.elf -gdb tcp::1234

qemu-system-cskyv2 -machine smartl -cpu $CPU_NAME -nographic -kernel yoc.elf -gdb tcp::1234



# help
# ubuntu18 env prepare:
# sudo apt install libvdeplug-dev libaio-dev libsnappy-dev
# sudo ln -s /snap/core/7917/lib/x86_64-linux-gnu/libpng12.so.0.54.0 /lib/x86_64-linux-gnu/libpng12.so.0

# how to debug:
# terminal 1 run:     qemu-system-cskyv2 -machine smartl -cpu $CPU_NAME -nographic -kernel yoc.elf -gdb tcp::1234 -S
# terminal 2 run:     csky-abiv2-elf-gdb yoc.elf -x gdbinit
# gdbinit:            target remote localhost:1234

