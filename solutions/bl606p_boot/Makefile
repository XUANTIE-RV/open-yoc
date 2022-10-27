CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated
BOARD_PATH:=../../boards/bl606p_evb_e907

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j8
	@echo YoC SDK Done

.PHONY:install
install:
	@echo install boot
	$(CPRE) cp boot.bin ${BOARD_PATH}/bootimgs/boot.bin
	$(CPRE) cp boot.elf ${BOARD_PATH}/bootimgs/boot.elf

.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk boot.* out -rf
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm -rf generated
