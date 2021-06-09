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
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH)/data -tb

	@echo [INFO] Create wm bin files
	$(CPRE) csky-elfabiv2-objcopy -O binary ./out/helloworld_w800/yoc.elf ./out/helloworld_w800/yoc.bin
	$(CPRE) gcc ../../components/chip_w800/wmsdk/tools/w800/wm_tool.c -Wall -lpthread -O2 -o ./out/helloworld_w800/wm_tool
	$(CPRE) ./out/helloworld_w800/wm_tool -b ./out/helloworld_w800/yoc.bin -fc 0 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0 -vs yoc-7.3.0
	$(CPRE) cat ../../components/chip_w800/wmsdk/tools/w800/w800_secboot.img ./out/helloworld_w800/yoc.img > ./out/helloworld_w800/yoc.fls
	$(CPRE) ./out/helloworld_w800/wm_tool -b ./out/helloworld_w800/yoc.img -fc 1 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0 -vs yoc-7.3.0
	$(CPRE) mv ./out/helloworld_w800/yoc_gz.img ./out/helloworld_w800/yoc_ota.bin
	@echo [INFO] Create fls file
	$(CPRE) cp $(MK_GENERATED_IMGS_PATH)/data/boot boot.bin
	$(CPRE) cp $(MK_GENERATED_IMGS_PATH)/data/prim prim.bin
	$(CPRE) cp $(MK_GENERATED_IMGS_PATH)/data/imtb imtb.bin
	$(CPRE) ./out/helloworld_w800/wm_tool -b boot.bin -fc 0 -it 512 -ih 8002000 -ra 8002400 -ua 8010000 -nh 80D0000 -un 0
	$(CPRE) ./out/helloworld_w800/wm_tool -b prim.bin -fc 0 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0
	$(CPRE) ./out/helloworld_w800/wm_tool -b imtb.bin -fc 0 -it 1 -ih 20047400 -ra 81C0000 -ua 0 -nh 0 -un 0
	$(CPRE) cat boot.img prim.img imtb.img > ./out/helloworld_w800/yoc_total.fls
	$(CPRE) rm -f boot.bin prim.bin imtb.bin boot.img prim.img imtb.img

run:all
	$(CPRE) ./out/helloworld_w800/wm_tool -c $(DL_PORT) -rs at -ds 2000000 -dl ./out/helloworld_w800/yoc.fls -sl str -ws 115200

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
