echo qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -vga none -kernel yoc.elf -gdb tcp::1234

qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -vga none -kernel yoc.elf -gdb tcp::1234


# help
# ubuntu18 env prepare:
# sudo apt install qemu-system-arm

# how to debug:
# terminal 1 run:     qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -vga none -kernel yoc.elf -gdb tcp::1234 -S
# terminal 2 run:     arm-none-eabi-gdb yoc.elf -x gdbinit
# gdbinit:            target remote localhost:1234

