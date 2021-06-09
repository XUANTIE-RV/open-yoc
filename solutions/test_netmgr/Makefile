CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated
CONFIG_FILE:= generated/data/config.yaml

ifeq ($(CONFIG_FILE), $(wildcard $(CONFIG_FILE)))
CHIP_NAME:=$(shell cat generated/data/config.yaml | grep chip | sed 's/[[:space:]\"]//g' | awk -F":" '{print $$2}')

ifeq ($(CHIP_NAME), pangu)
PRODUCT_ELF := -f PanGu_CK804EF_QSPIFlash_CDK.elf
else
PRODUCT_ELF :=
endif
endif
.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) --board=$(BOARD) -j4
	@echo YoC SDK Done
	#riscv64-unknown-elf-objcopy -O binary yoc.elf yoc.bin
	@echo [INFO] Create bin files
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x

.PHONY:flash
flash:
	@echo chip name:$(CHIP_NAME)
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim $(PRODUCT_ELF)

.PHONY:flashall
flashall:
	@echo chip name:$(CHIP_NAME)
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -a $(PRODUCT_ELF)

sdk:
	$(CPRE) yoc sdk
.PHONY:clean
clean:
	$(CPRE) rm $(MK_GENERATED_IMGS_PATH) -rf
	$(CPRE) rm yoc_sdk -rf
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk yoc.* generated out -rf
	$(CPRE) scons -c
