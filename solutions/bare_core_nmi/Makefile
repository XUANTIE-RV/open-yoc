# sdk root dir
TOP_DIR := $(realpath $(shell pwd)/../..)

ifeq ($(V), 1)
CPRE =
else
CPRE = @
endif

ARCH := riscv

# target
TARGET      := yoc
BUILD_DIR   := out
OUTPUT_ELF  := $(TARGET).elf
OUTPUT_ASM	:= $(TARGET).asm
OUTPUT_BIN	:= $(TARGET).bin

CLEANING := $(filter clean, $(MAKECMDGOALS))

ifneq ($(CLEANING),clean)
# update .defconfig and get make arguments from .defconfig
$(shell bash $(TOP_DIR)/tools/core/check_defconfig.sh update_config cpu=$(cpu) board=$(board) rtos=$(rtos) toolchain=$(toolchain))
cpu 		:= $(shell bash $(TOP_DIR)/tools/core/check_defconfig.sh read_config cpu)
board 		:= $(shell bash $(TOP_DIR)/tools/core/check_defconfig.sh read_config board)
rtos 		:= $(shell bash $(TOP_DIR)/tools/core/check_defconfig.sh read_config rtos)
toolchain 	:= $(shell bash $(TOP_DIR)/tools/core/check_defconfig.sh read_config toolchain)

$(info [info]: make cpu=$(cpu) board=$(board) rtos=$(rtos) toolchain=$(toolchain))

# toolchain
include $(TOP_DIR)/tools/core/toolchain.mk
# import funcs
include $(TOP_DIR)/tools/core/build_funcs.mk
# check arguments
include $(TOP_DIR)/tools/core/build_check_args.mk

SOLUTION_PATH := $(shell pwd)
ifeq ($(board), xiaohui)
LINKER_SCRIPT := $(TOP_DIR)/components/chip_riscv_dummy/gcc_flash_xiaohui.ld
BOARD_COMP_NAME := board_riscv_dummy
CHIP_COMP_NAME := chip_riscv_dummy
BOARD_PATH := $(TOP_DIR)/boards/board_riscv_dummy
CHIP_PATH := $(TOP_DIR)/components/chip_riscv_dummy
endif
ifeq ($(board), smartl)
ifneq ($(filter $(CPU_FULL_NAME), e901bm-cp e902mt),)
LINKER_SCRIPT := $(TOP_DIR)/components/chip_riscv_dummy/gcc_flash_smartl_lite.ld
else ifeq ($(CPU_FULL_NAME), e901minibzm-cp)
LINKER_SCRIPT := $(TOP_DIR)/components/chip_riscv_dummy/gcc_flash_smartl_e901mini.ld
else
LINKER_SCRIPT := $(TOP_DIR)/components/chip_riscv_dummy/gcc_flash_smartl.ld
endif
BOARD_COMP_NAME := board_riscv_dummy
CHIP_COMP_NAME := chip_riscv_dummy
BOARD_PATH := $(TOP_DIR)/boards/board_riscv_dummy
CHIP_PATH := $(TOP_DIR)/components/chip_riscv_dummy
endif
ifeq ($(board), wujian300)
LINKER_SCRIPT :=
BOARD_COMP_NAME := board_wujian300_evb
CHIP_COMP_NAME := chip_wujian300
BOARD_PATH := $(TOP_DIR)/boards/board_wujian300_evb
CHIP_PATH := $(TOP_DIR)/components/chip_wujian300
endif

COLLECT_BUILD_FILES=0
# depend components
# TODO: the common components for solution
COMPONENTS := csi libc_bare mm
COMPONENTS += $(CHIP_COMP_NAME)
COMPONENTS_NORMAL := $(COMPONENTS)
# include xx.mk in componnets dir first
$(foreach mod,$(COMPONENTS_NORMAL),$(eval $(call module_inlcude_template_func,$(mod))))
# board component
COMPONENTS += $(BOARD_COMP_NAME)
include $(BOARD_PATH)/sub.mk
# solution component
COMPONENTS += $(notdir $(patsubst %/,%,$(shell pwd)))
include $(SOLUTION_PATH)/sub.mk

CFLAGS += -D$(CPU_MACRO)=1
# TODO: solution defines
CFLAGS += -DCONFIG_KERNEL_NONE=1 -DCONFIG_DEBUG=1 -DCONFIG_SUPPORT_NMI_DEMO=1


# remove duplicate defines for cflags
CFLAGS := $(call remove_duplicate_defines_func, $(CFLAGS))

# include sub.mk again for collect files
COLLECT_BUILD_FILES=1
$(foreach mod,$(COMPONENTS_NORMAL),$(eval $(call module_inlcude_template_func,$(mod))))
include $(BOARD_PATH)/sub.mk
include $(SOLUTION_PATH)/sub.mk
# import sources/includes/lpath
include $(TOP_DIR)/tools/core/build_source.mk

LD_LIBS := $(addprefix -l, $(COMPONENTS))
LD_PATH += $(BUILD_DIR)/libs $(ALL_L_PATH)
LIBS := $(foreach mod,$(COMPONENTS),$(BUILD_DIR)/libs/lib$(mod).a)

CPU_FLAGS := -mcpu=$(cpu) -mcmodel=medany

CFLAGS += $(CPU_FLAGS)
CFLAGS += -MP -MMD -Wno-main
CFLAGS += -g -Os \
          -Wpointer-arith -Wno-undef -ffunction-sections \
          -fdata-sections -fno-inline-functions -fno-builtin -fno-strict-aliasing
CFLAGS += $(addprefix -I, $(ALL_INCS))
CXXFLAGS += $(CFLAGS)
ASFLAGS += -D__ASSEMBLY__ $(CFLAGS)
LDFLAGS := $(CPU_FLAGS) -nostartfiles -Wl,--gc-sections
LDFLAGS += -Wl,--whole-archive $(LD_LIBS) -Wl,--no-whole-archive
LDFLAGS += $(addprefix -L, $(LD_PATH))
LDFLAGS += -T$(LINKER_SCRIPT) -Wl,-zmax-page-size=1024 -Wl,-Map=$(TARGET).map

endif # CLEANING

.PHONY: all
all: $(BUILD_DIR) $(OUTPUT_ELF)

VPATH := $(TOP_DIR)

$(BUILD_DIR):
	$(CPRE) mkdir -p $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo [CC] $@
	$(CPRE) $(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo [CXX] $@
	$(CPRE) $(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	@echo [CXX] $@
	$(CPRE) $(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo [AS] $@
	$(CPRE) $(AS) $(ASFLAGS) -c $< -o $@

# static lib
$(foreach mod,$(COMPONENTS),$(eval $(call lib_rule_template_func,$(mod))))

$(OUTPUT_ELF): $(LIBS) $(LINKER_SCRIPT)
	@echo "Linking $@"
	$(CPRE) $(CXX) -o $@ $(LDFLAGS)
	$(CPRE) $(OBJDUMP) -d $(OUTPUT_ELF) > $(OUTPUT_ASM)
	$(CPRE) $(OBJCOPY) -O binary $(OUTPUT_ELF) $(OUTPUT_BIN)
	$(CPRE) sh script/after_build.sh $(SOLUTION_PATH) $(BOARD_PATH) $(CHIP_PATH) $(board) $(cpu) $(OBJCOPY) $(OBJDUMP)
	@echo "done building targets."

.PHONY: clean
clean:
	$(CPRE) rm -rf out yoc.* generated
	$(CPRE) rm -rf gdbinitflash .gdbinit gdbinit.remote
	@echo "done cleaning."
