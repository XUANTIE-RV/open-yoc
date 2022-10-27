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
#include <dut/hal/common.h>
#include "dut/hal/ble.h"
#include "pm.h"
#include <ll_sleep.h>
#include <ll_debug.h>
#include "drv/gpio.h"
#include "pinmux.h"
#include "pin_name.h"

/* Must be aligned with 4 Bytes */
typedef struct {
    uint8_t mac[6];
    uint8_t reserved1[2];
#ifdef CONFIG_CALIBRATE_WITH_FREQOFF
    uint32_t freqoff;
#else
    uint32_t xtalcap;
#endif
    uint8_t reserved[CONFIG_USER_FACTORY_DATA_SIZE];
} dut_factory_data_t;

#define CONFIG_FCDS_STARTADDR  0x11004000
#define CONFIG_FCDS_XTALCAP    0x11004008

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

    int real_off = sizeof(dut_factory_data->mac) + sizeof(dut_factory_data->reserved1) + sizeof(uint32_t) + offset;

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
    if (mode == 0) {
        //Sleep
        drv_pm_sleep_enable();

        subWriteReg(&(AP_AON->PMCTL2_0), 6, 6, 0x00); //disable software control

        ll_debug_output(0);

        set_sleep_flag(1);
        enter_sleep_off_mode(SYSTEM_SLEEP_MODE);
    } else {
        //Standby
        *((unsigned int *)0x4000f0c0) = 0x2;
        subWriteReg(0x4000f01c, 6, 6, 0x00); //disable software control
        enter_sleep_off_mode(SYSTEM_OFF_MODE);
    }

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

#ifdef CONFIG_CALIBRATE_WITH_FREQOFF
int dut_hal_freqoff_get(int32_t *freqoff)
{
    uint32_t freqoff_xip = (*(volatile unsigned int  *)(CONFIG_FCDS_XTALCAP));

    if (freqoff == NULL) {
        return -1;
    }

    if ((RF_PHY_FREQ_FOFF_N200KHZ > ((int32_t)freqoff_xip) ||
        ((int32_t)freqoff_xip) > RF_PHY_FREQ_FOFF_200KHZ) ||
        (freqoff_xip == 0xffffffff)) {
        *freqoff = RF_PHY_DEFAULT_FREQOFF << 2;
    } else {
        *freqoff = (int32_t)freqoff_xip << 2;
    }

    return 0;
}

int dut_hal_freqoff_store(int32_t freqoff)
{
    dut_factory_data_t *dut_factory_data = NULL;
    int ret = -1;
    int real_off = 0;
    int real_freqoff = 0;

    real_freqoff = (freqoff - freqoff % 20)/4;

    if (real_freqoff < RF_PHY_FREQ_FOFF_N200KHZ || real_freqoff > RF_PHY_FREQ_FOFF_200KHZ) {
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

    dut_factory_data->freqoff = (uint32_t)real_freqoff;

    ret = nvram_write((uint8_t *)dut_factory_data, sizeof(dut_factory_data_t));
    if (ret < 0) {
        aos_free(dut_factory_data);
        return -1;
    }

    aos_free(dut_factory_data);

    g_rfPhyFreqOffSet = real_freqoff;

    return 0;
}
#else
/* Attetion: This function will be called by system, should not printf here */
int dut_hal_xtalcap_get(uint32_t *xtalcap)
{
    uint32_t xtalcap_xip = (*(volatile unsigned int  *)(CONFIG_FCDS_XTALCAP));

    if (xtalcap == NULL ||
        xtalcap_xip == 0xffffffff) {
        return -1;
    }

    *xtalcap = xtalcap_xip;

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
#endif

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

    if (!(phy_fmt == PKT_FMT_BLE1M||phy_fmt ==PKT_FMT_BLE2M||phy_fmt ==PKT_FMT_BLR500K||phy_fmt ==PKT_FMT_BLR125K)) {
        return -2;
    }

    if (txpower > 0x3f) {
        return -3;
    }

    uint32_t cap = RF_PHY_DEFAULT_XTAL_CAP;
    /*
        singletone test, if flash have no data, we test with 0;
        else we need use value from flash.
    */
    int32_t freqoff = RF_PHY_FREQ_FOFF_00KHZ;
    int ret = dut_hal_freqoff_get(&freqoff);
    if (ret == 0 && freqoff != 0) {
        g_rfPhyFreqOffSet = freqoff >> 2;
        freqoff = g_rfPhyFreqOffSet;
    }
    //printf("%s, g_freqoff = %d,freqoff =  %d\n", __func__, g_rfPhyFreqOffSet, freqoff);

    rf_phy_dtm_tx_singleTone(phy_fmt, txpower, rf_chn_idx, cap, freqoff, testTimeUs);
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

    if (!(phy_fmt == PKT_FMT_BLE1M||phy_fmt ==PKT_FMT_BLE2M||phy_fmt ==PKT_FMT_BLR500K||phy_fmt ==PKT_FMT_BLR125K)) {
        return -2;
    }

    if (txpower > 0x3f) {
        return -3;
    }

    if (pkt_type > 5) {
        return -5;
    }
    uint32_t cap = RF_PHY_DEFAULT_XTAL_CAP;
    int32_t freqoff = RF_PHY_FREQ_FOFF_00KHZ;
    int ret = dut_hal_freqoff_get(&freqoff);
    if (ret == 0 && freqoff != 0) {
        g_rfPhyFreqOffSet = freqoff >> 2;
        freqoff = g_rfPhyFreqOffSet;
    }
    //printf("%s, g_freqoff = %d,freqoff =  %d\n", __func__, g_rfPhyFreqOffSet, freqoff);

    rf_phy_dtm_tx_mod_burst(phy_fmt,txpower, rf_chn_idx, cap, freqoff, pkt_type, pktLength, txPktNum, txPktIntv);
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

    if (!(phy_fmt == PKT_FMT_BLE1M||phy_fmt ==PKT_FMT_BLE2M||phy_fmt ==PKT_FMT_BLR500K||phy_fmt ==PKT_FMT_BLR125K)) {
        return -2;
    }

    uint32_t cap = RF_PHY_DEFAULT_XTAL_CAP;
    int32_t freqoff = RF_PHY_FREQ_FOFF_00KHZ;
    int ret = dut_hal_freqoff_get(&freqoff);
    if (ret == 0 && freqoff != 0) {
        g_rfPhyFreqOffSet = freqoff >> 2;
        freqoff = g_rfPhyFreqOffSet;
    }
    //printf("%s, g_freqoff = %d,freqoff =  %d\n", __func__, g_rfPhyFreqOffSet, freqoff);

    rf_phy_dtm_rx_demod_burst(phy_fmt,rf_chn_idx, freqoff, cap, pktLength, rxTimeOut, rxWindow, rx_freq_off, rx_rssi, rx_carr_sens, rx_pkt_num);
    return 0;
}

int dut_hal_ble_transmit_stop(void)
{
    rf_phy_dtm_stop();
    return 0;
}

int dut_hal_rx_current_test(uint32_t sleep_time)
{
    int16_t rxEstFoff = 0;
    uint8_t rx_est_rssi = 0;
    uint8_t rx_est_carrsens = 0;
    uint16_t rx_pkt_num = 0;
    uint8_t rfChnIdx = 0;
    uint8_t pktLength = 37; //max pkt length in 625us
    uint32 rxWindow = 625; //pkt interval unit:us

    if (sleep_time < 10) {
        sleep_time = 10;
    }

    uint32_t cap = RF_PHY_DEFAULT_XTAL_CAP;
    int32_t freqoff = RF_PHY_FREQ_FOFF_00KHZ;
    int ret = dut_hal_freqoff_get(&freqoff);
    if (ret == 0 && freqoff != 0) {
        g_rfPhyFreqOffSet = freqoff >> 2;
        freqoff = g_rfPhyFreqOffSet;
    }
    //printf("%s, g_freqoff = %d,freqoff =  %d\n", __func__, g_rfPhyFreqOffSet, freqoff);

    drv_pm_sleep_enable();

    rf_phy_dtm_rx_demod_burst(2, rfChnIdx, freqoff, cap, pktLength, sleep_time, rxWindow,
                                &rxEstFoff, &rx_est_rssi, &rx_est_carrsens, &rx_pkt_num);

    rf_phy_dtm_stop();
    drv_pm_sleep_disable();

    return OK;
}


int dut_hal_test_gpio_write(uint8_t wrgpio, uint8_t value)
{
    int ret;
    gpio_pin_handle_t *pin = NULL;

    if (wrgpio >= GPIO_NUM) {
        return -1;
    }

    drv_pinmux_config(wrgpio, PIN_FUNC_GPIO);
    pin = csi_gpio_pin_initialize(wrgpio, NULL);

    ret = csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_OUTPUT);
    if(ret < 0){
        return ret;
    }
    csi_gpio_pin_write(pin, value);
    return 0;
}

int dut_hal_test_gpio_read(uint8_t wrgpio, uint8_t *value)
{
    int ret;
    bool *bool_value = (bool *)value;
    gpio_pin_handle_t *pin = NULL;

    if (wrgpio >= GPIO_NUM) {
        return -1;
    }

    drv_pinmux_config(wrgpio, PIN_FUNC_GPIO);
    pin = csi_gpio_pin_initialize(wrgpio, NULL);

    ret = csi_gpio_pin_config_direction(pin,GPIO_DIRECTION_INPUT);
    if(ret < 0){
        return ret;
    }
    csi_gpio_pin_read(pin,bool_value);
    return 0;
}

