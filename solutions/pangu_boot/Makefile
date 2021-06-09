CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done

.PHONY:flash
flash:
	$(CPRE) product flashbin boot.bin 0x8001000 -f PanGu_CK804EF_QSPIFlash_CDK.elf -v

.PHONY:install
install:
	@echo install boot
	$(CPRE) cp boot.bin ../../boards/pangu_cpu0_evb/bootimgs/boot
	$(CPRE) cp boot.elf ../../boards/pangu_cpu0_evb/bootimgs/boot.elf

.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk boot.* out -rf
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm -rf generated
