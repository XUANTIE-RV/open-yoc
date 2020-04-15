CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated
YOC_ROOT=../../
BOARD_PATH:=$(shell yoc variable BOARD_PATH)
ELF_PATH:=out/$(shell cat package.yaml | grep name -w| awk -F" " '{printf "%s", $$2}')/
ELF_NAME:=yoc
.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done
	@echo [INFO] Create bin files
	$(CPRE) rm -fr $(MK_GENERATED_IMGS_PATH)
	$(CPRE) mkdir $(MK_GENERATED_IMGS_PATH) $(MK_GENERATED_IMGS_PATH)/data -p
	$(CPRE) if [ -f "$(BOARD_PATH)/bootimgs/boot" ]; then cp $(BOARD_PATH)/bootimgs/boot $(MK_GENERATED_IMGS_PATH)/data/; fi
	$(CPRE) cp $(BOARD_PATH)/configs/config.yaml $(MK_GENERATED_IMGS_PATH)/data/
	$(CPRE) cp $(ELF_PATH)${ELF_NAME}.elf ${ELF_NAME}.elf
	$(CPRE) csky-abiv2-elf-objcopy -O binary ${ELF_NAME}.elf yoc.bin
	$(CPRE) csky-abiv2-elf-objdump -d ${ELF_NAME}.elf > yoc.asm
	$(CPRE) cp -f yoc.bin $(MK_GENERATED_IMGS_PATH)/data/prim
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x
	$(CPRE) rm -fr $(MK_GENERATED_IMGS_PATH)/data

.PHONY:flash
flash:
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim

.PHONY:flashall
flashall:
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -a

sdk:
	$(CPRE) yoc sdk
.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk -rf
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
