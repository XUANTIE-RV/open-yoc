CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

ALGORITHMS_PATH:=../../components/chip_bl606p/bl606p_flasher.elf

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done

.PHONY:flash
flash:
	$(CPRE) product flashbin -w yoc.bin 0x011000 -f ${ALGORITHMS_PATH} -x gdbinitflash -v

.PHONY:clean
clean:
	$(CPRE) rm -rf yoc_sdk binary out yoc.* generated
	$(CPRE) rm -fr .gdbinit mkflash.sh
