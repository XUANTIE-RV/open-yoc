CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) $(SDK) "
	$(CPRE) scons $(VERB) --board=$(BOARD) --sdk=$(SDK) -j8
	@echo YoC SDK Done

.PHONY:clean
clean:
	$(CPRE) rm -rf yoc_sdk binary out yoc.* generated
	$(CPRE) rm -fr gdbinitflash .gdbinit gdbinit.remote
