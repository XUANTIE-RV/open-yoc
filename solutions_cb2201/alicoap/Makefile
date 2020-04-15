CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated
BOARD_NAME:=$(shell cat package.yaml | grep board_name | awk -F" " '{print $$2}')
BOARD_PATH:=../../boards/csky/$(BOARD_NAME)
PRODUCT_BIN:=../../tools/build/product
PKG_NAME:=$(shell cat package.yaml | grep "^name:" | awk -F" " '{print $$2}')
CKBOOT_BIN:=$(BOARD_PATH)/bootimgs/boot
CKMTB_BIN:=$(BOARD_PATH)/bootimgs/mtb
CKTEE_BIN:=$(BOARD_PATH)/bootimgs/tee

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done
	@echo [INFO] Create bin files
	$(CPRE) rm -fr $(MK_GENERATED_IMGS_PATH)
	$(CPRE) mkdir -p $(MK_GENERATED_IMGS_PATH) $(MK_GENERATED_IMGS_PATH)/data
	$(CPRE) cp $(CKMTB_BIN) $(MK_GENERATED_IMGS_PATH)/data/mtb
	$(CPRE) cp $(CKBOOT_BIN) $(MK_GENERATED_IMGS_PATH)/data/boot
	$(CPRE) cp $(CKTEE_BIN) $(MK_GENERATED_IMGS_PATH)/data/tee
	$(CPRE) cp $(BOARD_PATH)/configs/config.yaml $(MK_GENERATED_IMGS_PATH)/data/
	$(CPRE) cp out/${PKG_NAME}/yoc.elf ./
	$(CPRE) csky-abiv2-elf-objcopy -O binary yoc.elf yoc.bin
	$(CPRE) csky-abiv2-elf-objdump -D yoc.elf > yoc.asm
	$(CPRE) cp -f yoc.bin $(MK_GENERATED_IMGS_PATH)/data/prim
	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
	$(CPRE) $(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x
	$(CPRE) rm -fr $(MK_GENERATED_IMGS_PATH)/data

	$(CPRE) ls .gdbinit >/dev/null 2>&1 || (cp $(BOARD_PATH)/configs/gdbinit ./.gdbinit)
.PHONY:flash
flash:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim

.PHONY:flashall
flashall:
	$(CPRE) $(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -a

.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk -rf
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete

%:
	$(CPRE) scons --component="$@" -j4
