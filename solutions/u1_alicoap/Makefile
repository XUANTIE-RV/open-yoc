CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

PKG_NAME:=$(shell cat package.yaml | grep "^name:" | awk -F" " '{print $$2}')


.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done
	@echo [INFO] Create bin files
	$(CPRE) cp out/${PKG_NAME}/yoc.elf ./
	$(CPRE) csky-abiv2-elf-objcopy -O binary yoc.elf ck.bin
	$(CPRE) csky-abiv2-elf-objdump -D yoc.elf > yoc.asm

.PHONY:clean
clean:
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk yoc.* generated out -rf
