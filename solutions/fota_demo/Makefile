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
	$(CPRE) rm $(MK_GENERATED_IMGS_PATH) -rf
	@echo "Build Solution by $(BOARD) $(SDK) "
	$(CPRE) scons $(VERB) --board=$(BOARD) --sdk=$(SDK) -j4
	@echo YoC SDK Done

	@echo [INFO] Create bin files
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x

.PHONY:flash
flash:
	$(CPRE) scons --flash=prim --board=$(BOARD) --sdk=$(SDK)

.PHONY:flashall
flashall:
	$(CPRE) scons --flash=all --board=$(BOARD) --sdk=$(SDK)

sdk:
	$(CPRE) yoc sdk
.PHONY:clean
clean:
	$(CPRE) rm $(MK_GENERATED_IMGS_PATH) -rf
	$(CPRE) rm yoc_sdk yoc.* generated out -rf
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) scons -c
