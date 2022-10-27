CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done

.PHONY:flash
flash:
	$(CPRE) product flashbin boot.bin 0x18000000 -f ch2601_flash.elf -x .gdbinit -v 

.PHONY:install
install:
	@echo install boot
	$(CPRE) cp boot.bin ../../boards/ch2601_evb/bootimgs/boot
	$(CPRE) cp boot.elf ../../boards/ch2601_evb/bootimgs/boot.elf

.PHONY:clean
clean:
	$(CPRE) rm -rf yoc_sdk binary out boot.* generated
	$(CPRE) rm -fr gdbinitflash .gdbinit gdbinit mkflash.sh
