##
## Auto Generated makefile by CDK
## Do not modify this file, and any manual changes will be erased!!!   
##
## BuildSet
ProjectName            :=ch2601_freertos_demo
ConfigurationName      :=BuildSet
WorkspacePath          :=./
ProjectPath            :=./
IntermediateDirectory  :=Obj
OutDir                 :=$(IntermediateDirectory)
User                   :=wc194771
Date                   :=18/10/2021
CDKPath                :=../../../../../../C-Sky/CDK
LinkerName             :=riscv64-unknown-elf-gcc
LinkerNameoption       :=
SIZE                   :=riscv64-unknown-elf-size
READELF                :=riscv64-unknown-elf-readelf
CHECKSUM               :=crc32
SharedObjectLinkerName :=
ObjectSuffix           :=.o
DependSuffix           :=.d
PreprocessSuffix       :=.i
DisassemSuffix         :=.asm
IHexSuffix             :=.ihex
BinSuffix              :=.bin
ExeSuffix              :=.elf
LibSuffix              :=.a
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
ElfInfoSwitch          :=-hlS
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
UnPreprocessorSwitch   :=-U
SourceSwitch           :=-c 
ObjdumpSwitch          :=-S
ObjcopySwitch          :=-O ihex
ObjcopyBinSwitch       :=-O binary
OutputFile             :=$(ProjectName)
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=ch2601_freertos_demo.txt
MakeDirCommand         :=mkdir
LinkOptions            :=   -nostartfiles  -Wl,--gc-sections  -T../../boards/ch2601_evb/gcc_flash.ld  -mabi=ilp32 -mtune=e906 -march=rv32imacxtheade 
LinkOtherFlagsOption   :=  -Wl,-Map="yoc.map" -Wl,-zmax-page-size=1024
IncludePackagePath     :=
IncludeCPath           :=$(IncludeSwitch)../../components/aos/include $(IncludeSwitch)../../components/aos/include/devices $(IncludeSwitch)../../components/aos_hal/include $(IncludeSwitch)../../boards/ch2601_evb/include $(IncludeSwitch)../../components/chip_ch2601/include $(IncludeSwitch)../../components/chip_ch2601/sys $(IncludeSwitch)../../components/csi/csi2/include $(IncludeSwitch)../../components/drivers/csi2/include $(IncludeSwitch)../../components/freertos/FreeRTOS/Source/include $(IncludeSwitch)../../components/freertos/FreeRTOS/Source/portable/GCC/riscv/rv32_32gpr/non_tspend/ $(IncludeSwitch)../../components/kv/include $(IncludeSwitch)../../components/newlib/include $(IncludeSwitch)../../components/partition/include $(IncludeSwitch)../../components/sec_crypto/include $(IncludeSwitch)../../components/sec_crypto/crypto/include $(IncludeSwitch)../../components/ulog/include $(IncludeSwitch)../../components/uservice/include  $(IncludeSwitch)app/include $(IncludeSwitch)app/src/oled  
IncludeAPath           :=$(IncludeSwitch)../../components/aos/include $(IncludeSwitch)../../components/aos/include/devices $(IncludeSwitch)../../components/aos_hal/include $(IncludeSwitch)../../boards/ch2601_evb/include $(IncludeSwitch)../../components/chip_ch2601/include $(IncludeSwitch)../../components/chip_ch2601/sys $(IncludeSwitch)../../components/csi/csi2/include $(IncludeSwitch)../../components/drivers/csi2/include $(IncludeSwitch)../../components/freertos/FreeRTOS/Source/include $(IncludeSwitch)../../components/freertos/FreeRTOS/Source/portable/GCC/riscv/rv32_32gpr/non_tspend/ $(IncludeSwitch)../../components/kv/include $(IncludeSwitch)../../components/newlib/include $(IncludeSwitch)../../components/partition/include $(IncludeSwitch)../../components/sec_crypto/include $(IncludeSwitch)../../components/sec_crypto/crypto/include $(IncludeSwitch)../../components/ulog/include $(IncludeSwitch)../../components/uservice/include  $(IncludeSwitch)app/include $(IncludeSwitch)app/src/oled  
Libs                   := -Wl,--whole-archive $(LibrarySwitch)aos $(LibrarySwitch)aos_hal $(LibrarySwitch)ch2601_evb $(LibrarySwitch)chip_ch2601 $(LibrarySwitch)csi $(LibrarySwitch)drivers $(LibrarySwitch)freertos $(LibrarySwitch)hal_csi $(LibrarySwitch)kv $(LibrarySwitch)newlib $(LibrarySwitch)partition $(LibrarySwitch)sec_crypto $(LibrarySwitch)ulog $(LibrarySwitch)uservice  -Wl,--no-whole-archive  
ArLibs                 := 
PackagesLibPath        :=$(LibraryPathSwitch)../../components/aos/lib $(LibraryPathSwitch)../../components/aos_hal/lib $(LibraryPathSwitch)../../boards/ch2601_evb/lib $(LibraryPathSwitch)../../components/chip_ch2601/lib $(LibraryPathSwitch)../../components/csi/lib $(LibraryPathSwitch)../../components/drivers/lib $(LibraryPathSwitch)../../components/freertos/lib $(LibraryPathSwitch)../../components/hal_csi/lib $(LibraryPathSwitch)../../components/kv/lib $(LibraryPathSwitch)../../components/newlib/lib $(LibraryPathSwitch)../../components/partition/lib $(LibraryPathSwitch)../../components/sec_crypto/lib $(LibraryPathSwitch)../../components/ulog/lib $(LibraryPathSwitch)../../components/uservice/lib 
LibPath                :=$(LibraryPathSwitch)Obj  $(PackagesLibPath) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       :=riscv64-unknown-elf-ar rcu
CXX      :=riscv64-unknown-elf-g++
CC       :=riscv64-unknown-elf-gcc
AS       :=riscv64-unknown-elf-gcc
OBJDUMP  :=riscv64-unknown-elf-objdump
OBJCOPY  :=riscv64-unknown-elf-objcopy
CXXFLAGS := $(PreprocessorSwitch)USE_AOS $(PreprocessorSwitch)USE_AOS_HAL $(PreprocessorSwitch)USE_CH2601_EVB -nostdlib -Wpointer-arith -Wall -Wl,-EL -ffunction-sections -fdata-sections -Os -g  $(PreprocessorSwitch)USE_CHIP_CH2601 -Wno-unused-function -Os -g -mno-ldr -mno-ext -mno-mula -mno-srri -mno-ldi -mno-addsl -mno-ldd -mno-rev -mno-condmv  $(PreprocessorSwitch)USE_CSI $(PreprocessorSwitch)USE_DRIVERS $(PreprocessorSwitch)USE_FREERTOS $(PreprocessorSwitch)USE_HAL_CSI $(PreprocessorSwitch)USE_KV $(PreprocessorSwitch)USE_NEWLIB $(PreprocessorSwitch)USE_PARTITION $(PreprocessorSwitch)USE_SEC_CRYPTO $(PreprocessorSwitch)USE_ULOG $(PreprocessorSwitch)USE_USERVICE $(PreprocessorSwitch)CONFIG_ARCH_INTERRUPTSTACK=6144 $(PreprocessorSwitch)CONFIG_CHIP_ALKAID=1 $(PreprocessorSwitch)CONFIG_CLI=1 $(PreprocessorSwitch)CONFIG_CPU_E906=1 $(PreprocessorSwitch)CONFIG_CSI=\"csi2\" $(PreprocessorSwitch)CONFIG_CSI_V2=1 $(PreprocessorSwitch)CONFIG_DEBUG=1 $(PreprocessorSwitch)CONFIG_INIT_TASK_STACK_SIZE=8192 $(PreprocessorSwitch)CONFIG_KERNEL_FREERTOS=1 $(PreprocessorSwitch)CONFIG_KERNEL_WORKQUEUE=1 $(PreprocessorSwitch)CONFIG_NON_ADDRESS_FLASH=1 $(PreprocessorSwitch)CONFIG_SYSTICK_HZ=100 $(PreprocessorSwitch)CONFIG_UART_RECV_BUF_SIZE=1024 $(PreprocessorSwitch)CONFIG_XIP=1   -mabi=ilp32 -mtune=e906 -march=rv32imacxtheade    
CFLAGS   := $(PreprocessorSwitch)USE_AOS $(PreprocessorSwitch)USE_AOS_HAL $(PreprocessorSwitch)USE_CH2601_EVB -nostdlib -Wpointer-arith -Wall -Wl,-EL -ffunction-sections -fdata-sections -Os -g  $(PreprocessorSwitch)USE_CHIP_CH2601 -Wno-unused-function -Os -g -mno-ldr -mno-ext -mno-mula -mno-srri -mno-ldi -mno-addsl -mno-ldd -mno-rev -mno-condmv  $(PreprocessorSwitch)USE_CSI $(PreprocessorSwitch)USE_DRIVERS $(PreprocessorSwitch)USE_FREERTOS $(PreprocessorSwitch)USE_HAL_CSI $(PreprocessorSwitch)USE_KV $(PreprocessorSwitch)USE_NEWLIB $(PreprocessorSwitch)USE_PARTITION $(PreprocessorSwitch)USE_SEC_CRYPTO $(PreprocessorSwitch)USE_ULOG $(PreprocessorSwitch)USE_USERVICE $(PreprocessorSwitch)CONFIG_ARCH_INTERRUPTSTACK=6144 $(PreprocessorSwitch)CONFIG_CHIP_ALKAID=1 $(PreprocessorSwitch)CONFIG_CLI=1 $(PreprocessorSwitch)CONFIG_CPU_E906=1 $(PreprocessorSwitch)CONFIG_CSI=\"csi2\" $(PreprocessorSwitch)CONFIG_CSI_V2=1 $(PreprocessorSwitch)CONFIG_DEBUG=1 $(PreprocessorSwitch)CONFIG_INIT_TASK_STACK_SIZE=8192 $(PreprocessorSwitch)CONFIG_KERNEL_FREERTOS=1 $(PreprocessorSwitch)CONFIG_KERNEL_WORKQUEUE=1 $(PreprocessorSwitch)CONFIG_NON_ADDRESS_FLASH=1 $(PreprocessorSwitch)CONFIG_SYSTICK_HZ=100 $(PreprocessorSwitch)CONFIG_UART_RECV_BUF_SIZE=1024 $(PreprocessorSwitch)CONFIG_XIP=1   -mabi=ilp32 -mtune=e906 -march=rv32imacxtheade   -mno-ldr -mno-ext -mno-mula -mno-srri -mno-ldi -mno-addsl -mno-ldd -mno-rev -mno-condmv 
ASFLAGS  := $(PreprocessorSwitch)USE_AOS $(PreprocessorSwitch)USE_AOS_HAL $(PreprocessorSwitch)USE_CH2601_EVB  $(PreprocessorSwitch)USE_CHIP_CH2601  $(PreprocessorSwitch)USE_CSI $(PreprocessorSwitch)USE_DRIVERS $(PreprocessorSwitch)USE_FREERTOS $(PreprocessorSwitch)USE_HAL_CSI $(PreprocessorSwitch)USE_KV $(PreprocessorSwitch)USE_NEWLIB $(PreprocessorSwitch)USE_PARTITION $(PreprocessorSwitch)USE_SEC_CRYPTO $(PreprocessorSwitch)USE_ULOG $(PreprocessorSwitch)USE_USERVICE $(PreprocessorSwitch)CONFIG_ARCH_INTERRUPTSTACK=6144 $(PreprocessorSwitch)CONFIG_CHIP_ALKAID=1 $(PreprocessorSwitch)CONFIG_CLI=1 $(PreprocessorSwitch)CONFIG_CPU_E906=1 $(PreprocessorSwitch)CONFIG_CSI=\"csi2\" $(PreprocessorSwitch)CONFIG_CSI_V2=1 $(PreprocessorSwitch)CONFIG_DEBUG=1 $(PreprocessorSwitch)CONFIG_INIT_TASK_STACK_SIZE=8192 $(PreprocessorSwitch)CONFIG_KERNEL_FREERTOS=1 $(PreprocessorSwitch)CONFIG_KERNEL_WORKQUEUE=1 $(PreprocessorSwitch)CONFIG_NON_ADDRESS_FLASH=1 $(PreprocessorSwitch)CONFIG_SYSTICK_HZ=100 $(PreprocessorSwitch)CONFIG_UART_RECV_BUF_SIZE=1024 $(PreprocessorSwitch)CONFIG_XIP=1   -mabi=ilp32 -mtune=e906 -march=rv32imacxtheade    


Objects0=$(IntermediateDirectory)/init_init$(ObjectSuffix) $(IntermediateDirectory)/src_main$(ObjectSuffix) $(IntermediateDirectory)/oled_bitmap_risc_v$(ObjectSuffix) $(IntermediateDirectory)/oled_oled$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all
all: $(IntermediateDirectory)/$(OutputFile)

$(IntermediateDirectory)/$(OutputFile):  $(Objects) Always_Link 
	$(LinkerName) $(OutputSwitch) $(IntermediateDirectory)/$(OutputFile)$(ExeSuffix) $(LinkerNameoption) $(LinkOtherFlagsOption)  @$(ObjectsFileList)  $(LinkOptions) $(LibPath) $(Libs)
	$(OBJCOPY) $(ObjcopySwitch) $(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)  $(ProjectPath)/Obj/$(OutputFile)$(IHexSuffix) 
	$(OBJDUMP) $(ObjdumpSwitch) $(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)  > $(ProjectPath)/Lst/$(OutputFile)$(DisassemSuffix) 
	@echo size of target:
	@$(SIZE) $(ProjectPath)$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix) 
	@echo -n checksum value of target:  
	@$(CHECKSUM) $(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix) 
	@$(ProjectName).modify.bat $(IntermediateDirectory) $(OutputFile)$(ExeSuffix) 

Always_Link:


##
## Objects
##
$(IntermediateDirectory)/init_init$(ObjectSuffix): app/src/init/init.c  
	$(CC) $(SourceSwitch) app/src/init/init.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/init_init$(ObjectSuffix) -MF$(IntermediateDirectory)/init_init$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/init_init$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/init_init$(PreprocessSuffix): app/src/init/init.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/init_init$(PreprocessSuffix) app/src/init/init.c

$(IntermediateDirectory)/src_main$(ObjectSuffix): app/src/main.c  
	$(CC) $(SourceSwitch) app/src/main.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/src_main$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/src_main$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/src_main$(PreprocessSuffix): app/src/main.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/src_main$(PreprocessSuffix) app/src/main.c

$(IntermediateDirectory)/oled_bitmap_risc_v$(ObjectSuffix): app/src/oled/bitmap_risc_v.c  
	$(CC) $(SourceSwitch) app/src/oled/bitmap_risc_v.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/oled_bitmap_risc_v$(ObjectSuffix) -MF$(IntermediateDirectory)/oled_bitmap_risc_v$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/oled_bitmap_risc_v$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/oled_bitmap_risc_v$(PreprocessSuffix): app/src/oled/bitmap_risc_v.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/oled_bitmap_risc_v$(PreprocessSuffix) app/src/oled/bitmap_risc_v.c

$(IntermediateDirectory)/oled_oled$(ObjectSuffix): app/src/oled/oled.c  
	$(CC) $(SourceSwitch) app/src/oled/oled.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/oled_oled$(ObjectSuffix) -MF$(IntermediateDirectory)/oled_oled$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/oled_oled$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/oled_oled$(PreprocessSuffix): app/src/oled/oled.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/oled_oled$(PreprocessSuffix) app/src/oled/oled.c


-include $(IntermediateDirectory)/*$(DependSuffix)
