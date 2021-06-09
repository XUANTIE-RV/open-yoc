/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "spiflash_test.h"

#define NOT_AUTOTEST 0
#define TEST_LOCK_SIZE 128

static csi_gpio_t g_gpio;


#if NOT_AUTOTEST
static void pin_init(void)
{
    csi_pin_set_mux(EXAMPLE_PIN_SPI_MISO, EXAMPLE_PIN_SPI_MISO_FUNC);
    csi_pin_set_mux(EXAMPLE_PIN_SPI_MOSI, EXAMPLE_PIN_SPI_MOSI_FUNC);
    csi_pin_set_mux(EXAMPLE_PIN_SPI_SCK,  EXAMPLE_PIN_SPI_SCK_FUNC);

    csi_pin_set_mux(EXAMPLE_PIN_SPI_CS,   PIN_FUNC_GPIO);
}
#endif

static void spi_cs_callback(csi_gpio_pin_state_t value)
{
#ifdef EXAMPLE_PIN_SPI_CS_MSK
    csi_gpio_write(&g_gpio, EXAMPLE_PIN_SPI_CS_MSK, value);
#endif
}

int test_spiflash_lock(char *args)
{
    csi_error_t ret;
    csi_spiflash_t spiflash_handle;
    test_spiflash_args_t spiflash_args;
    csi_spiflash_info_t info;
    int lock_state;
    uint8_t tx_data[TEST_LOCK_SIZE];
    uint8_t rx_data[TEST_LOCK_SIZE];
    uint32_t gpio_pin_mask;
    uint32_t gpio_idx;
    uint8_t data[10];

    sscanf(args, "%d,%d,%d,%d,%d", &gpio_idx, &gpio_pin_mask, &spiflash_args.idx, &spiflash_args.offset, &spiflash_args.size);
    TEST_CASE_TIPS("gpio_idx: %d, gpio_pin_mask: %d", gpio_idx, gpio_pin_mask);
    TEST_CASE_TIPS("idx: %d, offset: %x, size %d", spiflash_args.idx, spiflash_args.offset, spiflash_args.size);

#if NOT_AUTOTEST
    pin_init();
#endif

    csi_gpio_init(&g_gpio, gpio_idx);
    csi_gpio_dir(&g_gpio, gpio_pin_mask, GPIO_DIRECTION_OUTPUT);


    ret = csi_spiflash_spi_init(&spiflash_handle, spiflash_args.idx, spi_cs_callback);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spiflash init error");

    ret = csi_spiflash_get_flash_info(&spiflash_handle, &info);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash_spi get flash info error");

    ret = csi_spiflash_erase(&spiflash_handle, 0, info.flash_size);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash_spi erase error");

    ret = csi_spiflash_erase(&spiflash_handle, spiflash_args.offset, info.sector_size);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash_spi erase error");

    ret = csi_spiflash_lock(&spiflash_handle, spiflash_args.offset, spiflash_args.size);
    TEST_CASE_ASSERT(ret == CSI_OK, "spiflash lock error");

    lock_state = csi_spiflash_is_locked(&spiflash_handle, spiflash_args.offset, spiflash_args.size);
    TEST_CASE_ASSERT(lock_state == 1, "check spiflash lock state error");

    ret = csi_spiflash_read_reg(&spiflash_handle, 0x05, data, 1);
    TEST_CASE_ASSERT(ret == CSI_OK, "spiflash read status");
    TEST_CASE_ASSERT((data[0] & 0x1c) > 0, "spiflash read status error");
    TEST_CASE_TIPS("spiflash lock, status:%d", data[0]);

    memset(tx_data, 0xa, sizeof(uint8_t) * TEST_LOCK_SIZE);
    ret = csi_spiflash_program(&spiflash_handle, spiflash_args.offset, tx_data, TEST_LOCK_SIZE);
    TEST_CASE_ASSERT(ret >= 0, "csi spiflash_spi program error");

    memset(rx_data, 0, sizeof(uint8_t) * TEST_LOCK_SIZE);
    ret = csi_spiflash_read(&spiflash_handle, spiflash_args.offset, rx_data, TEST_LOCK_SIZE);
    TEST_CASE_ASSERT(ret >= 0, "csi spiflash_spi read error");
    memset(tx_data, 0xff, sizeof(uint8_t) * TEST_LOCK_SIZE);
    ret = memcmp(tx_data, rx_data, TEST_LOCK_SIZE);
    TEST_CASE_ASSERT(ret == 0, "csi spiflash lock state program error");

    ret = csi_spiflash_unlock(&spiflash_handle, spiflash_args.offset, spiflash_args.size);
    TEST_CASE_ASSERT(ret == 0, "spiflash unlock error");

    ret = csi_spiflash_read_reg(&spiflash_handle, 0x05, data, 1);
    TEST_CASE_ASSERT(ret == CSI_OK, "spiflash read status");
    TEST_CASE_ASSERT((data[0] & 0x1c) == 0, "spiflash read status error");
    TEST_CASE_TIPS("spiflash unlock, status:%d", data[0]);

    lock_state = csi_spiflash_is_locked(&spiflash_handle, spiflash_args.offset, spiflash_args.size);
    TEST_CASE_ASSERT(lock_state == 0, "spiflash lock state error");

    memset(tx_data, 0xa, sizeof(uint8_t) * TEST_LOCK_SIZE);
    ret = csi_spiflash_program(&spiflash_handle, spiflash_args.offset, tx_data, TEST_LOCK_SIZE);
    TEST_CASE_ASSERT(ret >= 0, "csi spiflash_spi program error");
    memset(rx_data, 0, sizeof(uint8_t) * TEST_LOCK_SIZE);
    ret = csi_spiflash_read(&spiflash_handle, spiflash_args.offset, rx_data, TEST_LOCK_SIZE);
    TEST_CASE_ASSERT(ret >= 0, "csi spiflash_spi read error");
    ret = memcmp(tx_data, rx_data, TEST_LOCK_SIZE);
    TEST_CASE_ASSERT(ret == 0, "results of spiflash reading and programing are not equal");

    csi_spiflash_spi_uninit(&spiflash_handle);


    return 0;
}