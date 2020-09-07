CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated

# define tee client library target
LIBCSITEECA:=libcsiteeca.a

.PHONY:startup
startup: all $(LIBCSITEECA)

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done

	@echo [INFO] Create bin files

# include tee.mk to generate tee client library
include tee.mk

.PHONY:flash
flash:
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -w prim

.PHONY:flashall
flashall:
	$(CPRE) product flash $(MK_GENERATED_IMGS_PATH)/images.zip -a

sdk:
	$(CPRE) yoc sdk
.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk out -rf
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[oda.]" -delete
	$(CPRE) find . -name "*.asm" -delete
