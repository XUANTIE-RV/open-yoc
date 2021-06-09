CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif
FLASH_SCRIPT=script/flash.sh
BOARD := phy6220_evb
AFTER_BUILD_SCRIPT=script/aft_build.sh
SOLUTION_NAME:=$(shell cat package.yaml | grep -w name | sed 's/[[:space:]\"]//g' | awk -F":" '{print $$2}')

MK_GENERATED_IMGS_PATH:=generated
PRODUCT_BIN:=product

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) "
	$(CPRE) scons $(VERB) --board=$(BOARD) -j4
	@echo YoC SDK Done

	@echo [INFO] Create bin files
	$(CPRE) if [ -f ./$(AFTER_BUILD_SCRIPT) ]; then \
		chmod +x ./$(AFTER_BUILD_SCRIPT);\
		./$(AFTER_BUILD_SCRIPT) $(SOLUTION_NAME); \
	else \
		$(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p;  \
		$(PRODUCT_BIN) image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x; \
	fi;

.PHONY:flash
flash:
	$(CPRE) if [ -f ./$(FLASH_SCRIPT) ]; then \
		chmod +x ./$(FLASH_SCRIPT); \
		./$(FLASH_SCRIPT) flash; \
	else \
		$(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim;\
	fi;

.PHONY:flashall
flashall:
	$(CPRE) if [ -f ./$(FLASH_SCRIPT) ]; then \
		chmod +x ./$(FLASH_SCRIPT); \
		./$(FLASH_SCRIPT) flashall; \
	else \
		$(PRODUCT_BIN) flash $(MK_GENERATED_IMGS_PATH)/images.zip -a;\
	fi;

sdk:
	$(CPRE) yoc sdk

.PHONY:clean
clean:
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm yoc_sdk yoc.* generated out script misc -rf .gdbinit
