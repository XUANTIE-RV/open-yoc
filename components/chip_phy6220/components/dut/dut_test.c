/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <drv/pmu.h>
#include <drv/spiflash.h>
#include "rf_phy_driver.h"
#include "dut/hal/common.h"
#include "dut/hal/ble.h"
#include "pm.h"
#include <ll_sleep.h>
#include <ll_debug.h>

/* Must be aligned with 4 Bytes */
typedef struct {
    uint8_t mac[6];
    uint8_t reserved1[2];
    uint32_t xtalcap;
    uint8_t reserved[CONFIG_USER_FACTORY_DATA_SIZE];
} dut_factory_data_t;

#define CONFIG_FCDS_STARTADDR  0x11004000

static spiflash_handle_t g_flash_handle = NULL;
static int nvram_write(const uint8_t *buffer, int length)
{
    int ret = -1;
    if (buffer == NULL || length <= 0) {
        return -1;
    }

    if (g_flash_handle == NULL) {
        g_flash_handle = csi_spiflash_initialize(0, NULL);
        if (g_flash_handle == NULL) {
            return -1;
        }
    }

    ret = csi_spiflash_erase_sector(g_flash_handle, CONFIG_FCDS_STARTADDR);
    if (ret < 0) {
        return -1;
    }

    ret = csi_spiflash_program(g_flash_handle, CONFIG_FCDS_STARTADDR, (void *)buffer, length);
    if (ret != length) {
        return -1;
    }

    return 0;
}

static int nvram_read(uint32_t offset, const uint8_t *buffer, int length)
{
    int ret = -1;
    if (buffer == NULL || length <= 0) {
        return -1;
    }

    if (g_flash_handle == NULL) {
        g_flash_handle = csi_spiflash_initialize(0, NULL);
        if (g_flash_handle == NULL) {
            return -1;
        }
    }

    ret = csi_spiflash_read(g_flash_handle, CONFIG_FCDS_STARTADDR + offset, (void *)buffer, length);
    if (ret <= 0) {
        return -1;
    }

    return 0;
}

int dut_hal_factorydata_read(uint32_t offset, uint8_t *buffer, uint32_t length)
{
    int ret = -1;
    dut_factory_data_t *dut_factory_data = NULL;

    if (buffer == NULL || (offset + length) > CONFIG_USER_FACTORY_DATA_SIZE) {
        return -1;
    }

    dut_factory_data = (dut_factory_data_t *)aos_malloc(sizeof(dut_factory_data_t));
    if (dut_factory_data == NULL) {
        return -1;
    }

    int real_off = sizeof(dut_factory_data->mac) + sizeof(dut_factory_data->reserved1) + sizeof(dut_factory_data->xtalcap) + offset;

    ret = nvram_read(real_off, buffer, length);
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    aos_free(dut_factory_data);

    return 0;
}

int dut_hal_factorydata_store(uint32_t offset, uint8_t *buffer, uint32_t length)
{
    int ret = -1;
    dut_factory_data_t *dut_factory_data = NULL;

    if (buffer == NULL || (offset + length) > CONFIG_USER_FACTORY_DATA_SIZE) {
        return -1;
    }

    dut_factory_data = (dut_factory_data_t *)aos_malloc(sizeof(dut_factory_data_t));
    if (dut_factory_data == NULL) {
        return -1;
    }

    ret = nvram_read(0, (uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    memcpy(&dut_factory_data->reserved[offset], buffer, length);

    ret = nvram_write((uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    aos_free(dut_factory_data);

    return 0;
}

int dut_hal_sleep(uint8_t mode)
{
    drv_pm_sleep_enable();

    subWriteReg(&(AP_AON->PMCTL2_0), 6, 6, 0x00); //disable software control

    ll_debug_output(0);

    set_sleep_flag(1);
    enter_sleep_off_mode(1);
    while (1);
    return 0;
}

int dut_hal_mac_get(uint8_t addr[6])
{
    uint8_t temp_addr[6];
    int ret = -1;
    int real_off = 0;

    ret = nvram_read(real_off, temp_addr, sizeof(temp_addr));
    if (ret < 0) {
        return -1;
    }

    addr[2] = temp_addr[0];
    addr[3] = temp_addr[1];
    addr[4] = temp_addr[2];
    addr[5] = temp_addr[3];
    addr[0] = temp_addr[4];
    addr[1] = temp_addr[5];

    return 0;
}

int dut_hal_mac_store(uint8_t addr[6])
{
    dut_factory_data_t *dut_factory_data = NULL;
    int ret = -1;
    int real_off = 0;

    dut_factory_data = (dut_factory_data_t *)aos_malloc(sizeof(dut_factory_data_t));
    if (dut_factory_data == NULL) {
        return -1;
    }

    ret = nvram_read(real_off, (uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    dut_factory_data->mac[0] = addr[2];
    dut_factory_data->mac[1] = addr[3];
    dut_factory_data->mac[2] = addr[4];
    dut_factory_data->mac[3] = addr[5];
    dut_factory_data->mac[4] = addr[0];
    dut_factory_data->mac[5] = addr[1];

    ret = nvram_write((uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    aos_free(dut_factory_data);

    return 0;
}

/* Attetion: This function will be called by system, should not printf here */
int dut_hal_xtalcap_get(uint32_t *xtalcap)
{
    dut_factory_data_t *dut_factory_data = NULL;
    int ret = -1;
    int real_off = 0;

    if (xtalcap == NULL) {
        return -1;
    }

    dut_factory_data = (dut_factory_data_t *)aos_malloc(sizeof(dut_factory_data_t));
    if (dut_factory_data == NULL) {
        return -1;
    }

    ret = nvram_read(real_off, (uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    if (dut_factory_data->xtalcap != 0xff) {
        *xtalcap = dut_factory_data->xtalcap;
    } else {
        *xtalcap = 0x10;
    }

    aos_free(dut_factory_data);

    return 0;
}

int dut_hal_xtalcap_store(uint32_t xtalcap)
{
    dut_factory_data_t *dut_factory_data = NULL;
    int ret = -1;
    int real_off = 0;

    dut_factory_data = (dut_factory_data_t *)aos_malloc(sizeof(dut_factory_data_t));
    if (dut_factory_data == NULL) {
        return -1;
    }

    ret = nvram_read(real_off, (uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    dut_factory_data->xtalcap = xtalcap;

    ret = nvram_write((uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    aos_free(dut_factory_data);

    return 0;
}

/* API for ble test */
/**************************************************************************************
 * @fn          dut_hal_ble_tx_single_tone
 *
 * @brief       This function process for rf phy direct test
 *
 * input parameters
 *
 * @param 
 *              phy_fmt      :   PHY Format
 *              rf_chn_idx    :   rf channel = 2402+(rf_chn_idx<<1)
 *              xtalcap     :   xtal cap
 *              txpower     :   rf tx power
 *              testTime    :   test loop active time(unit: s)
 *
 * output parameters
 *
 * @param       none
 *
 * @return      Success:0 ; Fail: < 0
 */
int dut_hal_ble_tx_single_tone(uint8_t phy_fmt, uint8_t rf_chn_idx, uint32_t xtalcap, uint8_t txpower)
{
    uint32 testTimeUs = 1000;

    if (phy_fmt != PKT_FMT_BLE1M) {
        return -2;
    }

    if (txpower > 0x3f) {
        return -3;
    }

    if (rf_chn_idx > 39) {
        return -4;
    }

    rf_phy_dtm_ext_tx_singleTone(txpower, rf_chn_idx, xtalcap, 0, testTimeUs);

    return 0;
}

/**************************************************************************************
 * @fn          dut_hal_ble_tx_mod_burst
 *
 * @brief       This function process for rf phy direct test
 *
 * input parameters
 *
 * @param
 *              phy_fmt      :   PHY Format
 *              rf_chn_idx    :   rf channel = 2402+(rf_chn_idx<<1)
 *              xtalcap     :   xtal cap
 *              txpower     :   rf tx power
 *              pkt_type     :   modulaiton data type, 0: prbs9, 1: 1111000: 2 10101010
 *              pktLength   :   pkt length(Byte)
 *              txPktNum    :   burst pkt tx number
 *              txPktIntv   :   txPkt intv,0 txPkt intv is pkt interval =  ceil((L+249)/625) * 625
 *
 * output parameters
 *
 * @param       none
 *
 * @return      Success:0 ; Fail: < 0
 */
int dut_hal_ble_tx_mod_burst(uint8_t phy_fmt, uint8_t rf_chn_idx,uint32_t xtalcap, uint8_t txpower, uint8_t pkt_type)
{
    uint32 txPktIntv = 0;
    uint8_t pktLength = 37; //max pkt length in 625us
    uint32_t txPktNum = 1000; //send 1000 pkt

    if (phy_fmt != PKT_FMT_BLE1M) {
        return -2;
    }

    if (txpower > 0x3f) {
        return -3;
    }

    if (rf_chn_idx > 39) {
        return -4;
    }

    if (pkt_type > 3) {
        return -5;
    }

    rf_phy_dtm_ext_tx_mod_burst(txpower, rf_chn_idx, xtalcap, 0, pkt_type, pktLength, txPktNum, txPktIntv);

    return 0;
}

/**************************************************************************************
 * @fn          dut_hal_ble_rx_demod_burst
 *
 * @brief       This function process for rf phy direct test
 *
 * input parameters
 *
 * @param
 *              phy_fmt          :   PHY Format
 *              rf_chn_idx        :   rf channel = 2402+(rf_chn_idx<<1)
 *              xtalcap         :   xtal cap
 *              pktLength       :   pkt length(Byte)
 *              rxWindow        :   rx demod window length(us)
 *              rxTimeOut       :   rx on time (ms)
 *
 * output parameters
 *
 * @param       rx_freq_off       :   rx demod estimated frequency offset
 *              rx_rssi       :   rx demod estimated rssi
 *              rx_carr_sens   :   rx demod estimated carrier sense
 *              rx_pkt_num        :   rx demod received pkt number
 *
 * @return      Success:0 ; Fail: < 0
 */
int dut_hal_ble_rx_demod_burst(uint8_t phy_fmt, uint8_t rf_chn_idx, uint32_t xtalcap, int16_t *rx_freq_off, uint8_t *rx_rssi, uint8_t *rx_carr_sens, uint16_t *rx_pkt_num)
{
    uint8_t pktLength = 37; //max pkt length in 625us
    uint32 rxWindow = 625; //pkt interval unit:us
    uint32 rxTimeOut = 625; //1000 pkt cost 625ms unit:ms

    if (phy_fmt != PKT_FMT_BLE1M) {
        return -2;
    }

    if (rf_chn_idx > 39) {
        return -4;
    }

    rf_phy_dtm_ext_rx_demod_burst(rf_chn_idx, 0, xtalcap, pktLength, rxTimeOut, rxWindow, rx_freq_off, rx_rssi, rx_carr_sens, rx_pkt_num);

    return 0;
}

int dut_hal_ble_transmit_stop(void)
{
    rf_phy_dtm_stop();
    return 0;
}
