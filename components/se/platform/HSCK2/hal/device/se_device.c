/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include <se_device.h>
#include <spi_hal.h>
#include <se_dev_internal.h>
#include "cmd.h"

static se_dev_t se_dev;
static se_dev_t *p_se_dev;

void se_dev_lock(void)
{
	aos_mutex_lock(&p_se_dev->mutex, AOS_WAIT_FOREVER);
}

void se_dev_unlock(void)
{
	aos_mutex_unlock(&p_se_dev->mutex);
}

/**
  \brief       Initialize SE Interface. 1. Initializes the resources needed for the RSA interface 2.registers event callback function
  \param[out]  se_dev  	rsa handle to operate.
  \param[in]   idx      device id
  \param[in]   type		the type of se interface
  \param[in]   config	a pointer to the configuraion of se interface
  \return      \ref 0 success, Non-zero failure
*/
int se_init(se_dev_t *dev, uint32_t idx, se_if_type_t type, void *config)
{
	if (p_se_dev)
		return 0;

	if (type == SE_IF_TYPE_SPI) {
    	se_dev.dev = SPI_Init_Config(idx, config);
		if (!se_dev.dev)
			return -2;
	} else {
		return -1;
	}

	p_se_dev = &se_dev;
	dev = p_se_dev;

    return 0;
}

/**
  \brief       De-initialize SE Interface. stops operation and releases the software resources used by the interface
  \param[in]   aes    handle to operate
  \return      None
*/
void se_uninit(se_dev_t *se_dev)
{
	SPI_unInit_Config(se_dev->dev);
	se_dev->dev = NULL;
	p_se_dev = NULL;

    return ;
}

/**
  \brief       Get SE firmware version
  \param[in]   se_dev     handle to operate
  \param[out]  version    Pointer to the version buf
  \return      \ref 0 success, Non-zero failure
*/
int se_get_firmware_version(se_dev_t *se_dev, uint8_t *version)
{
    uint32_t pulVersiondLen = 0;
    uint32_t ret = 0;

    se_dev_lock();
    ret = hs_GetAppVersion( version, &pulVersiondLen);
    se_dev_unlock();
    if (ret != 0) {
        return -1;
    }
    #ifdef SE_DEVICE_LOG
    printf("app version:0x%x0%x\n", version[0], version[1]);
    #endif
    return 0;
}

/**
  \brief       Get se system function test
  \param[in]   se_dev     handle to operate
  \return      \ref 0 success, Non-zero failure
*/
int se_system_selftest(se_dev_t *se_dev)
{
    return 0;
}

/**
  \brief       Start SE firmware upgrade
  \return      0 success, Non-zero failured
*/
int se_start_update_firmware(void)
{
    uint32_t ret = 0;

    ret = hs_Initialization_before_downloading_firmware();

	return (int)ret;
}

/**
  \brief       SE firmware upgrade
  \param[in]   src        Pointer to the firmware buf
  \param[in]   size       The length of firmware buf
  \param[in]   pos        The position of firmware buf
  \return      \ref 0 success, Non-zero failure
*/
int se_update_firmware(const void *src, uint32_t size, uint32_t pos)
{
    CHECK_RET_WITH_RET(src, -1);
    uint32_t ret = 0;

    ret = hs_downloading_firmware((const uint8_t *)src, size);
    CHECK_RET_WITH_RET(ret == 0, -1);

    return 0;
}

/**
  \brief       Stop SE firmware upgrade
  \return      0 success, Non-zero failured
*/
int se_stop_update_firmware(void)
{
    uint32_t ret = 0;

    ret = hs_Activate_the_app();

	return (int)ret;
}