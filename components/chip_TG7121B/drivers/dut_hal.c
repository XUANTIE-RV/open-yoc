#include <stdint.h>
#ifndef CONFIG_KERNEL_NONE
#include "dut/hal/common.h"
#include "aos/hal/flash.h"


int dut_hal_factorydata_read(uint32_t offset, uint8_t *buffer, uint32_t length)
{
    return hal_flash_read(HAL_PARTITION_BT_FIRMWARE,&offset,buffer,length);
}

int dut_hal_factorydata_store(uint32_t offset, uint8_t *buffer, uint32_t length)
{
    return hal_flash_write(HAL_PARTITION_BT_FIRMWARE,&offset,buffer,length);
}

int dut_hal_ble_tx_single_tone(uint8_t phyFmt, uint8_t rfChnIdx, uint8_t xtalCap, uint8_t txPower)
{
    return 0;
}

int dut_hal_ble_tx_mod_burst(uint8_t phyFmt, uint8_t rfChnIdx,uint8_t xtalCap, uint8_t txPower, uint8_t pktType)
{
    return 0;
}

int dut_hal_ble_rx_demod_burst(uint8_t phyFmt, uint8_t rfChnIdx, uint8_t xtalCap, int16_t *rxCurFoff, uint8_t *rxCurRssi, uint8_t *rxCurCarrSens, uint16_t *rxPktNum)
{
    return 0;
}

int dut_hal_mac_get(uint8_t addr[6])
{
    uint32_t offset = 0x100;
    return hal_flash_read(HAL_PARTITION_BT_FIRMWARE,&offset,addr,6);
}

int dut_hal_mac_store(uint8_t addr[6])
{
    uint32_t offset = 0x100;
    return hal_flash_write(HAL_PARTITION_BT_FIRMWARE,&offset,addr,6);
}

int dut_hal_sleep(uint8_t mode)
{
    return 0;
}

int dut_hal_xtalcap_get(uint32_t *xtalCap)
{
    return 0;
}

int dut_hal_xtalcap_store(uint32_t xtalCap)
{
    return 0;
}

int dut_hal_ble_transmit_stop(void)
{
    return 0;
}

#endif