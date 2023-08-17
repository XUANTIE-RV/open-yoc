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
	$(CPRE) touch app/app_main.c
	$(CPRE) scons $(VERB) --board=$(BOARD) -j4
	@echo YoC SDK Done

	@echo [INFO] Create bin files
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
	$(CPRE) product image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x

.PHONY:clean
clean:
	$(CPRE) rm -rf yoc_sdk binary out yoc.*  generated
	$(CPRE) rm -fr gdbinitflash .gdbinit gdbinit

