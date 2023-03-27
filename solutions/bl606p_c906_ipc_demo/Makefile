CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) $(SDK) "
	$(CPRE) scons $(VERB) --board=$(BOARD) --sdk=$(SDK) -j8
	@echo YoC SDK Done

# copy c906 firmware
.PHONY:install
install:
	$(CPRE) cp yoc.bin ../../boards/bl606p_evb_e907/bootimgs/c906.bin -arf
	$(CPRE) head -n 20 yoc.asm > ../../boards/bl606p_evb_e907/bootimgs/c906.asm

.PHONY:clean
clean:
	$(CPRE) rm -rf yoc_sdk binary out yoc.* generated
	$(CPRE) rm -fr gdbinitflash .gdbinit gdbinit mkflash.sh
