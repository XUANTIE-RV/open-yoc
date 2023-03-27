CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

cpunum = $(shell cat /proc/cpuinfo| grep "processor"| wc -l)

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) $(SDK) "
	$(CPRE) scons $(VERB) --board=$(BOARD) --sdk=$(SDK) -j$(cpunum)
	@echo YoC SDK Done

.PHONY:flash
flash:
	$(CPRE) scons --flash=prim --board=$(BOARD) --sdk=$(SDK)


.PHONY:flashall
flashall:
	$(CPRE) scons --flash=all --board=$(BOARD) --sdk=$(SDK)

.PHONY:erasechip
erasechip:
	$(CPRE) scons --flash=erasechip --board=$(BOARD) --sdk=$(SDK)
sdk:
	$(CPRE) yoc sdk

.PHONY:clean
clean:
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk yoc.* generated out temp .gdbinit  -rf
	$(CPRE) rm -fr gdbinitflash .gdbinit gdbinit mkflash.sh
