CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) "
	$(CPRE) scons $(VERB) --board=$(BOARD) -j4
	@echo YoC SDK Done

	@echo [INFO] Create bin files
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -v "v1.0" -p
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x

	@echo [INFO] Create wm bin files
	$(CPRE) csky-elfabiv2-objcopy -O binary yoc.elf yoc.bin
	$(CPRE) gcc ../../components/chip_w800/wmsdk/tools/w800/wm_tool.c -Wall -lpthread -O2 -o wm_tool
	$(CPRE) ./wm_tool -b yoc.bin -fc 0 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0 -vs yoc-7.3.0
	$(CPRE) cat ../../components/chip_w800/wmsdk/tools/w800/w800_secboot.img yoc.img > yoc.fls
	$(CPRE) ./wm_tool -b yoc.img -fc 1 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0 -vs yoc-7.3.0
	$(CPRE) mv yoc_gz.img yoc_ota.img

run:all
	$(CPRE) ./wm_tool -c $(DL_PORT) -rs at -ds 2000000 -dl yoc.fls -sl str -ws 115200

.PHONY:flash
flash:
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim -f script/w800_flash.elf

.PHONY:flashall
flashall:
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -a -f script/w800_flash.elf

sdk:
	$(CPRE) yoc sdk

.PHONY:clean
clean:
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk yoc.* generated out -rf
