#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <csi_core.h>
#include "aos/cli.h"

#include "drv/common.h"
#include "drv/efuse.h"

#include "cvi_efuse.h"
#include "cvi_misc.h"
#include "cvi_sys.h"

// get chip sn
void cvi_efuse_test_case_1(int32_t argc, char **argv)
{
    CVI_U8 pu8SN[8] = {0};

    CVI_SYS_GetChipSN(pu8SN, 8);

    printf("CVI_SYS_GetChipSN : %s\n", pu8SN);  
}

// EnableFastBoot
void cvi_efuse_test_case_2(int32_t argc, char **argv)
{   
    csi_efuse_t efuse = {0};
    uint32_t read_data[1] = {0};

    csi_efuse_init(&efuse, 0);
    csi_efuse_read(&efuse, 0xb, &read_data, sizeof(read_data));
    printf("value : %x \n", read_data[0]);

    int ret = CVI_EFUSE_EnableFastBoot();
    if (ret == 1) {
        printf("fast boot enable\n");   
    }
    
    ret = CVI_EFUSE_IsFastBootEnabled();
    if (ret == 1) {
        printf("fast boot enable\n");   
    }

    csi_efuse_read(&efuse, 0xb, &read_data, sizeof(read_data));
    printf("value : %x \n", read_data[0]);
    csi_efuse_uninit(&efuse); 
}

// efuse write
void cvi_efuse_test_case_3(int32_t argc, char **argv)
{
    csi_efuse_t efuse = {0};
    uint32_t data[17] = { 0x12345678, 0x87654321, 0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x87654321, 0x87654321,
        0x87654321, 0x87654321, 0x87654321, 0x87654321, 0x87654321, 0x87654321 };
    uint32_t read_data[17] = {0};

    csi_efuse_init(&efuse, 0);
    csi_efuse_read(&efuse, 0x0, &read_data, sizeof(read_data));
    for (int i = 0; i < 17; i++) {
        printf("value before [%d] : %x \n", i, read_data[i]);
    }

    csi_efuse_program(&efuse, 0x0, &data, sizeof(data));

    csi_efuse_read(&efuse, 0x0, &read_data, sizeof(read_data));
    for (int i = 0; i < 17; i++) {
        printf("value after [%d] : %x \n", i, read_data[i]);
    }
    csi_efuse_uninit(&efuse); 
}

void cvi_efuse_dump(int32_t argc, char **argv)
{
	uint32_t data;
	int i = 0;

	for (; i < 64; i++) {
		cvi_efuse_read_word_from_shadow(i, &data);
		printf("%08x ", data);
		if ((i & 0x3) == 0x3) {
			printf("\n");
		}
	}
}

ALIOS_CLI_CMD_REGISTER(cvi_efuse_dump, efused, efuse dump);
ALIOS_CLI_CMD_REGISTER(cvi_efuse_test_case_1, efuse_case1, efuse test);
ALIOS_CLI_CMD_REGISTER(cvi_efuse_test_case_2, fastboot_en, enable fastboot and print result);
ALIOS_CLI_CMD_REGISTER(cvi_efuse_test_case_3, efuse_case3, efuse write test);
