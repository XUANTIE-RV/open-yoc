CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated
BOARD_NAME:=$(shell cat package.yaml | grep board_name | sed 's/[[:space:]\"]//g' | awk -F":" '{print $$2}')
BOARD_PATH:=../../boards/silan/$(BOARD_NAME)
PRODUCT_BIN:=product

CKBOOT_BIN:=${BOARD_PATH}/bootimgs/boot.bin
DSPFW_BIN:=firmware/dsp.bin
AUDIO_BIN:=firmware/audio.bin
M0BOOT_BIN:=${BOARD_PATH}/bootimgs/m0app.bin
BOOT_PARAM:=${BOARD_PATH}/bootimgs/para_only.bin
CONFIG_YAML:= ${BOARD_PATH}/configs/config.yaml

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done
	@echo [INFO] Create bin files
	
	$(CPRE) cp $(CKBOOT_BIN) $(MK_GENERATED_IMGS_PATH)/data/boot
	$(CPRE) cp $(M0BOOT_BIN) $(MK_GENERATED_IMGS_PATH)/data/m0app
	$(CPRE) cp $(BOOT_PARAM) $(MK_GENERATED_IMGS_PATH)/data/para_only
	$(CPRE) cp $(DSPFW_BIN) $(MK_GENERATED_IMGS_PATH)/data/dsp
	$(CPRE) cp $(AUDIO_BIN) $(MK_GENERATED_IMGS_PATH)/data/audio


	$(CPRE) cp $(CONFIG_YAML) $(MK_GENERATED_IMGS_PATH)/data/
	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x
	$(CPRE) rm -fr $(MK_GENERATED_IMGS_PATH)/data

#	$(CPRE) csky-abiv2-elf-objdump -D yoc.elf > yoc.asm
	$(CPRE) ls .gdbinit >/dev/null 2>&1 || (cp $(BOARD_PATH)/configs/gdbinit ./.gdbinit)
.PHONY:flash
flash:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim

.PHONY:flashall
flashall:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -a

.PHONY:clean
clean:
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk out generated yoc.* -rf

%:
	$(CPRE) scons --component="$@" -j4
