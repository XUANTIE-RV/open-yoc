.PHONY: clean All Project_Title Project_PreBuild aos aos_hal ch2601_evb chip_ch2601 csi drivers freertos hal_csi kv newlib partition sec_crypto ulog uservice Project_Build Project_PostBuild

All: Project_Title Project_PreBuild aos aos_hal ch2601_evb chip_ch2601 csi drivers freertos hal_csi kv newlib partition sec_crypto ulog uservice Project_Build Project_PostBuild

Project_Title:
	@echo "----------Building project:[ ch2601_freertos_demo - BuildSet ]----------"

Project_PreBuild:
	@echo Executing Pre Build commands ...
	@export BOARD_PATH="D:/ws/kehu/yoc/freertos_temp/boards/ch2601_evb" CDKPath="D:/C-Sky/CDK" CDK_VERSION="V2.10.1" CHIP_PATH="D:/ws/kehu/yoc/freertos_temp/components/chip_ch2601" ProjectPath="D:/ws/kehu/yoc/freertos_temp/solutions/ch2601_freertos_demo/" SOLUTION_PATH="D:/ws/kehu/yoc/freertos_temp/solutions/ch2601_freertos_demo/" aos="develop" aos_hal="v7.4.3" ch2601_evb="v7.4.y" chip_ch2601="v7.4.3" csi="v7.4.3" drivers="v7.4.5" freertos="v7.4.y" hal_csi="v7.4.y" kv="v7.4.3" newlib="v7.4.3" partition="v7.4.3" sec_crypto="v7.4.3" ulog="v7.4.3" uservice="v7.4.3" && "D:/ws/kehu/yoc/freertos_temp/solutions/ch2601_freertos_demo/script/pre_build.sh" 
	@echo Done

aos:
	@make -r -f Obj/BuildSet/Packages/aos/develop/Makefile -j 8 -C  ./ 

aos_hal:
	@make -r -f Obj/BuildSet/Packages/aos_hal/v7.4.3/Makefile -j 8 -C  ./ 

ch2601_evb:
	@make -r -f Obj/BuildSet/Packages/ch2601_evb/v7.4.y/Makefile -j 8 -C  ./ 

chip_ch2601:
	@make -r -f Obj/BuildSet/Packages/chip_ch2601/v7.4.3/Makefile -j 8 -C  ./ 

csi:
	@make -r -f Obj/BuildSet/Packages/csi/v7.4.3/Makefile -j 8 -C  ./ 

drivers:
	@make -r -f Obj/BuildSet/Packages/drivers/v7.4.5/Makefile -j 8 -C  ./ 

freertos:
	@make -r -f Obj/BuildSet/Packages/freertos/v7.4.y/Makefile -j 8 -C  ./ 

hal_csi:
	@make -r -f Obj/BuildSet/Packages/hal_csi/v7.4.y/Makefile -j 8 -C  ./ 

kv:
	@make -r -f Obj/BuildSet/Packages/kv/v7.4.3/Makefile -j 8 -C  ./ 

newlib:
	@make -r -f Obj/BuildSet/Packages/newlib/v7.4.3/Makefile -j 8 -C  ./ 

partition:
	@make -r -f Obj/BuildSet/Packages/partition/v7.4.3/Makefile -j 8 -C  ./ 

sec_crypto:
	@make -r -f Obj/BuildSet/Packages/sec_crypto/v7.4.3/Makefile -j 8 -C  ./ 

ulog:
	@make -r -f Obj/BuildSet/Packages/ulog/v7.4.3/Makefile -j 8 -C  ./ 

uservice:
	@make -r -f Obj/BuildSet/Packages/uservice/v7.4.3/Makefile -j 8 -C  ./ 


Project_Build:
	@make -r -f ch2601_freertos_demo.mk -j 8 -C  ./ 

Project_PostBuild:
	@echo Executing Post Build commands ...
	@export BOARD_PATH="D:/ws/kehu/yoc/freertos_temp/boards/ch2601_evb" CDKPath="D:/C-Sky/CDK" CDK_VERSION="V2.10.1" CHIP_PATH="D:/ws/kehu/yoc/freertos_temp/components/chip_ch2601" ProjectPath="D:/ws/kehu/yoc/freertos_temp/solutions/ch2601_freertos_demo/" SOLUTION_PATH="D:/ws/kehu/yoc/freertos_temp/solutions/ch2601_freertos_demo/" aos="develop" aos_hal="v7.4.3" ch2601_evb="v7.4.y" chip_ch2601="v7.4.3" csi="v7.4.3" drivers="v7.4.5" freertos="v7.4.y" hal_csi="v7.4.y" kv="v7.4.3" newlib="v7.4.3" partition="v7.4.3" sec_crypto="v7.4.3" ulog="v7.4.3" uservice="v7.4.3" && "D:/ws/kehu/yoc/freertos_temp/solutions/ch2601_freertos_demo/script/aft_build.sh" 
	@echo Done


clean:
	@echo "----------Cleaning project:[ ch2601_freertos_demo - BuildSet ]----------"

