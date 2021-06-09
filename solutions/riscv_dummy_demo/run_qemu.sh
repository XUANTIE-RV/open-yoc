qemu-system-riscv32 -machine smartl -cpu e906fd -nographic -kernel yoc.elf -gdb tcp::1234



# help
# ubuntu18 env prepare:
# sudo apt install libvdeplug-dev libaio-dev libsnappy-dev
# sudo ln -s /snap/core/7917/lib/x86_64-linux-gnu/libpng12.so.0.54.0 /lib/x86_64-linux-gnu/libpng12.so.0

# how to debug:
# terminal 1 run:     qemu-system-riscv32 -machine smartl -cpu e906fd -nographic -kernel yoc.elf -gdb tcp::1234 -S
# terminal 2 run:     riscv64-unknown-elf-gdb yoc.elf -x gdbinit
# gdbinit:            target remote localhost:1234

