#include <spiflash_test.h>
#include <string.h>

#define SPIFLASH_ADDR_BASE  CSKY_QSPIMEM_BASE

int spiflash_erase_read(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;

    uint8_t idx;
    uint32_t erase_addr_start;
    uint32_t erase_size;

    idx = *((uint32_t *)args);
    erase_addr_start = *((uint32_t *)args+1);
    erase_size = *((uint32_t *)args+2);

    TEST_CASE_READY();

    hd = csi_spiflash_initialize(idx, NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"spiflash %d initialize failed.", idx);

    ret = csi_spiflash_erase_sector(hd, erase_addr_start);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d rease sector failed.", idx);

    uint32_t i;
    for (i=0;i<erase_size;i++) {
        if (*((uint8_t *)erase_addr_start+i) != 0xff) {
            TEST_CASE_ASSERT(1 == 0, "spiflash erase failed");
            break;
        }
    }

    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);

    return 0;
}

int spiflash_rease_write_erase_read(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;

    uint8_t idx;
    uint32_t erase_addr_start;
    uint32_t erase_size;

    void *data = NULL, *r_data = NULL;

    idx = *((uint32_t *)args);
    erase_addr_start = *((uint32_t *)args+1);
    erase_size = *((uint32_t *)args+2);


    TEST_CASE_READY();

    data = malloc(erase_size);
    r_data = malloc(erase_size);
    if (!data || !r_data) {
        free(data);
        free(r_data);
        TEST_CASE_ASSERT_QUIT(1 == 0,"malloc failed.");
    }


    hd = csi_spiflash_initialize(idx, NULL);
    if (hd == NULL) {
        free(data);
        free(r_data);
        TEST_CASE_ASSERT_QUIT(1 == 0,"spiflash %d initialize failed.", idx);
    }
    ret = csi_spiflash_erase_sector(hd, erase_addr_start);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d rease sector failed.", idx);

    memset(data, 0x55, erase_size);

    ret = csi_spiflash_program(hd, erase_addr_start, data, erase_size);
    TEST_CASE_ASSERT(ret == erase_size,"spiflash %d program failed.", idx);

    ret = csi_spiflash_erase_sector(hd, erase_addr_start);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d rease sector failed.", idx);

    ret = csi_spiflash_read(hd, erase_addr_start, r_data, erase_size);
    TEST_CASE_ASSERT(ret == erase_size,"spiflash %d read failed.", idx);


    uint32_t i;
    for (i=0;i<erase_size;i++) {
        if (*((uint8_t *)r_data+i) != 0xff) {
            TEST_CASE_ASSERT(1 == 0, "spiflash erase failed");
            break;
        }
    }


    free(data);
    free(r_data);
    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);


    return 0;
}

int spiflash_erase_overflow(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;

    uint8_t idx;
    uint32_t erase_addr_start;

    TEST_CASE_READY();

    idx = *((uint32_t *)args);
    erase_addr_start = *((uint32_t *)args+1);

    hd = csi_spiflash_initialize(idx, NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"spiflash %d initialize failed.", idx);


    ret = csi_spiflash_erase_sector(hd, erase_addr_start);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPIFLASH_BASE | DRV_ERROR_PARAMETER),"spiflash %d rease sector overflow failed.", idx);


    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);

    return 0;
}

int spiflash_erase_chip(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;
    spiflash_info_t *info=NULL;

    uint8_t idx;

    TEST_CASE_READY();

    hd = csi_spiflash_initialize(idx, NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"spiflash %d initialize failed.", idx);

    ret = csi_spiflash_erase_chip(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d rease chip failed.", idx);

    info = csi_spiflash_get_info(hd);

    uint32_t i;
    for (i=0;i<(info->end-info->start);i++) {
        if (*((uint8_t *)info->start+i) != 0xff) {
            TEST_CASE_ASSERT(1 == 0, "spiflash erase chip failed");
            break;
        }
    }


    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);

    return 0;
}

int spiflash_erase_write_read(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;
    int ret_cmp;

    uint8_t idx;
    uint32_t erase_addr_start;
    uint32_t erase_size;

    void *data = NULL, *r_data = NULL;

    idx = *((uint32_t *)args);
    erase_addr_start = *((uint32_t *)args+1);
    erase_size = *((uint32_t *)args+2);

    TEST_CASE_READY();

    data = malloc(erase_size);
    r_data = malloc(erase_size);
    if (!data || !r_data) {
        free(data);
        free(r_data);
        TEST_CASE_ASSERT_QUIT(1 == 0,"malloc failed.");
    }


    hd = csi_spiflash_initialize(idx, NULL);
    if (hd == NULL) {
        free(data);
        free(r_data);
        TEST_CASE_ASSERT_QUIT(1 == 0,"spiflash %d initialize failed.", idx);
    }

    ret = csi_spiflash_erase_sector(hd, erase_addr_start);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d rease sector failed.", idx);

    memset(data, 0x55, erase_size);

    ret = csi_spiflash_program(hd, erase_addr_start, data, erase_size);
    TEST_CASE_ASSERT(ret == erase_size,"spiflash %d program failed.", idx);

    ret = csi_spiflash_read(hd, erase_addr_start, r_data, erase_size);
    TEST_CASE_ASSERT(ret == erase_size,"spiflash %d read failed.", idx);

    ret_cmp = memcmp(data, r_data, erase_size);
    TEST_CASE_ASSERT(ret_cmp == 0,"spiflash write datas not equal read datas.", idx);


    free(data);
    free(r_data);
    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);

    return 0;
}

int spiflash_write_overflow(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;

    uint8_t idx;
    uint32_t erase_addr_start;
    uint32_t erase_size;

    void *data = NULL;

    idx = *((uint32_t *)args);
    erase_addr_start = *((uint32_t *)args+1);
    erase_size = *((uint32_t *)args+2);

    TEST_CASE_READY();

    data = malloc(erase_size);
    TEST_CASE_ASSERT_QUIT(data != NULL,"malloc failed.");

    hd = csi_spiflash_initialize(idx, NULL);
    if (!hd) {
        free(data);
        TEST_CASE_ASSERT_QUIT(1 == 0,"spiflash %d initialize failed.", idx);
    }

    ret = csi_spiflash_program(hd, erase_addr_start, data, erase_size);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPIFLASH_BASE | DRV_ERROR_PARAMETER),"spiflash %d program overflow failed.", idx);


    free(data);
    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);


    return 0;

}


int spiflash_read_overflow(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;

    uint8_t idx;
    uint32_t erase_addr_start;
    uint32_t erase_size;

    void *r_data = NULL;

    idx = *((uint32_t *)args);
    erase_addr_start = *((uint32_t *)args+1);
    erase_size = *((uint32_t *)args+2);

    TEST_CASE_READY();

    r_data = malloc(erase_size);
    TEST_CASE_ASSERT_QUIT(r_data != NULL,"malloc failed.");

    hd = csi_spiflash_initialize(idx, NULL);
    if (!hd) {
        free(r_data);
        TEST_CASE_ASSERT_QUIT(1 == 0,"spiflash %d initialize failed.", idx);
    }

    ret = csi_spiflash_read(hd, erase_addr_start, r_data, erase_size);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPIFLASH_BASE | DRV_ERROR_PARAMETER),"spiflash %d read overflow failed.", idx);


    free(r_data);
    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);


    return 0;
    
}

/* 稳定性测试 */
int spiflash_stability(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;

    uint8_t idx;
    uint32_t erase_addr_start;
    uint32_t erase_size;

    TEST_CASE_READY();

    hd = csi_spiflash_initialize(idx, NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"spiflash %d initialize failed.", idx);

    //TODO

    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.", idx);


    return 0;
}

/* 获取状态测试*/
int spiflash_get_status(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;
    spiflash_status_t sta;

    uint8_t idx;
    
    idx = *((uint32_t *)args);

    TEST_CASE_READY();

    hd = csi_spiflash_initialize(idx, NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"spiflash %d initialize failed.\n", idx);

    sta = csi_spiflash_get_status(hd);
    TEST_CASE_TIPS("SPIFLASH is %d staatus", sta.busy);

    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.\n", idx);


    return 0;
}

/* 获取SPIF 信息*/
int spiflash_get_info(void *args)
{
    spiflash_handle_t hd;
    int32_t ret;
    spiflash_info_t *info=NULL;

    uint8_t idx;
    uint32_t spiflash_start_addr;
    uint32_t spiflash_size;
    uint32_t spiflash_sector_count;
    uint32_t spiflash_sector_size;
    uint32_t spiflash_page_size;
    uint32_t spiflash_program_unit;
    uint8_t spiflash_erased_value;

    uint8_t info_flag;

    
    idx = *((uint32_t *)args);
    spiflash_start_addr = *((uint32_t *)args+1);
    spiflash_size = *((uint32_t *)args+2);
    spiflash_sector_count = *((uint32_t *)args+3);
    spiflash_sector_size = *((uint32_t *)args+4);
    spiflash_page_size = *((uint32_t *)args+5);
    spiflash_program_unit = *((uint32_t *)args+6);
    spiflash_erased_value = *((uint32_t *)args+7);

    info_flag = 0;

    TEST_CASE_READY();

    hd = csi_spiflash_initialize(idx, NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL,"spiflash %d initialize failed.\n", idx);

    info = csi_spiflash_get_info(hd);

    if (info->start != spiflash_start_addr) {
        info_flag = 1;
    }
    if (info->end != (spiflash_start_addr+spiflash_size)){
        info_flag = 1;
    }
    if (info->sector_count != spiflash_sector_count) {
        info_flag = 1;
    }
    if (info->sector_size != spiflash_sector_size) {
        info_flag = 1;
    }
    if (info->page_size != spiflash_page_size) {
        info_flag = 1;
    }
    if (info->program_unit != spiflash_program_unit) {
        info_flag = 1;
    }
    if (info->erased_value != spiflash_erased_value) {
        info_flag = 1;
    }

    TEST_CASE_ASSERT(info_flag == 0,"spiflash %d get spiflash info failed.\n", idx);

    ret = csi_spiflash_uninitialize(hd);
    TEST_CASE_ASSERT(ret == 0,"spiflash %d uninitialize failed.\n", idx);


    return 0;
}

