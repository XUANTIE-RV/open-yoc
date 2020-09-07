CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated
BOARD_NAME:=$(shell cat package.yaml | grep board_name | sed 's/[[:space:]\"]//g' | awk -F":" '{print $$2}')
BOARD_PATH:=../../boards/silan/$(BOARD_NAME)
PRODUCT_BIN:=product

CKBOOT_BIN:=${BOARD_PATH}/bootimgs/boot_0278.bin
DSPFW_BIN:=firmware/t-head/dsp.bin
M0BOOT_BIN:=${BOARD_PATH}/bootimgs/m0flash_0278_dbs.bin
CONFIG_YAML:= ${BOARD_PATH}/configs/config.yaml

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done
	@echo [INFO] Create bin files
	$(CPRE) cp $(CKBOOT_BIN) $(MK_GENERATED_IMGS_PATH)/data/boot
	$(CPRE) cp $(M0BOOT_BIN) $(MK_GENERATED_IMGS_PATH)/data/m0flash
#	$(CPRE) cp $(DSPFW_BIN) $(MK_GENERATED_IMGS_PATH)/data/dsp
	$(CPRE) cp $(CONFIG_YAML) $(MK_GENERATED_IMGS_PATH)/data/

	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -b -p -v "v1.0"
	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x
	$(CPRE) rm -fr $(MK_GENERATED_IMGS_PATH)/data

#	$(CPRE) csky-abiv2-elf-objdump -D yoc.elf > yoc.asm
	$(CPRE) ls .gdbinit >/dev/null 2>&1 || (cp $(BOARD_PATH)/configs/gdbinit ./.gdbinit)
.PHONY:flash
flash:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim

.PHONY:flashdsp
flashdsp:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w dsp

.PHONY:flashboot
flashboot:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w m0flash
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w boot

.PHONY:flashall
flashall:
#	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w dsp
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim
#	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -a

.PHONY:clean
clean:
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk out generated yoc.* -rf

%:
	$(CPRE) scons --component="$@" -j4
