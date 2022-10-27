CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

ALGORITHMS_PATH:=../../components/chip_d1/d1_flash.elf
BOARD_PATH:=../../boards/d1_dockpro_evb

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done

.PHONY:flash
flash:
	$(CPRE) product flashbin -w boot.bin 0x10000 -f ${ALGORITHMS_PATH} -x gdbinit -v 

.PHONY:install
install:
	@echo install boot to ${BOARD_PATH}/bootimgs
	$(CPRE) cp boot.bin ${BOARD_PATH}/bootimgs/boot
	$(CPRE) cp boot.elf ${BOARD_PATH}/bootimgs/boot.elf

.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk boot.* out -rf
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm -rf generated
