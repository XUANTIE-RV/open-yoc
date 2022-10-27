CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif


.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) "
	$(CPRE) scons $(VERB) --board=$(BOARD) -j4
	@echo YoC SDK Done

.PHONY:clean
clean:
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk yoc.* generated out -rf
