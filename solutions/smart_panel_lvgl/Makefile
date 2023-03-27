CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

cpunum = $(shell cat /proc/cpuinfo| grep "processor"| wc -l)

SDK := $(shell cat .defsdk 2> /dev/null)

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) $(SDK) "
	@echo $(SDK) > .defsdk
	$(CPRE) touch app/src/app_main.c
	$(CPRE) scons $(VERB) --board=$(BOARD) --sdk=$(SDK) -j$(cpunum)
	@echo YoC SDK Done

.PHONY:flashall
flashall:
	$(CPRE) scons --flash=all --board=$(BOARD) --sdk=$(SDK)

.PHONY:erasechip
erasechip:
	$(CPRE) scons --flash=erasechip --board=$(BOARD) --sdk=$(SDK)

.PHONY:flash
flash:
	$(CPRE) scons --flash=prim --board=$(BOARD) --sdk=$(SDK)

.PHONY:flashfs
flashfs:
	$(CPRE) scons --flash=lfs --board=$(BOARD) --sdk=$(SDK)

.PHONY:flashboot
flashboot:
	$(CPRE) scons --flash=boot --board=$(BOARD) --sdk=$(SDK)

.PHONY:clean
clean:
	$(CPRE) rm -rf yoc_sdk binary out yoc.*  generated
	$(CPRE) rm -fr gdbinitflash .gdbinit gdbinit


