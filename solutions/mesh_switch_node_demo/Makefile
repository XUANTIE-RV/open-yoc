CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif
MK_TEMPFILES_PATH=temp
AFTER_BUILD_SCRIPT=${MK_TEMPFILES_PATH}/script/aft_build.sh
SOLUTION_NAME:=$(shell cat package.yaml | grep -w name | sed 's/[[:space:]\"]//g' | awk -F":" '{print $$2}')

MK_GENERATED_IMGS_PATH:=generated
PRODUCT_BIN:=product

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) $(SDK) "
	$(CPRE) scons $(VERB) --board=$(BOARD) --sdk=$(SDK) -j4
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
	$(CPRE) rm yoc_sdk yoc.* generated out ${MK_TEMPFILES_PATH} -rf .gdbinit
