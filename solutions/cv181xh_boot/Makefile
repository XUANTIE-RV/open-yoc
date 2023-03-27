CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

BOARD_PATH:=../../boards/cv181xh_evb

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j8
	@echo YoC SDK Done

.PHONY:install
install:
	@echo install boot
	$(CPRE) cp boot.bin ${BOARD_PATH}/bootimgs/boot
	$(CPRE) cp boot.elf ${BOARD_PATH}/bootimgs/boot.elf

.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk boot.* out -rf
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm -rf generated
