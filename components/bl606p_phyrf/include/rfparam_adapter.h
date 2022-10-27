#ifndef __RFPARAM_ADAPTER_H__
#define __RFPARAM_ADAPTER_H__

#include "rfparam_rftlv.h"

#define RFPARAM_FLASH_ADDR_BASE                     0x58000000
#define RFPARAM_RF_PARAM_OFFSET                     0x1000

/* err code */
#define RFPARAM_SUSS                                0
#define RFPARAM_ERR_INVALID                         -1
#define RFPARAM_ERR_PWR_MODE_NULL                   -2
#define RFPARAM_ERR_PWR_OFFSET_NULL                 -3
#define RFPARAM_ERR_PWR_11B_NULL                    -4
#define RFPARAM_ERR_PWR_11G_NULL                    -5
#define RFPARAM_ERR_PWR_11N_NULL                    -6
#define RFPARAM_ERR_CAP_CODE_MODE_NULL              -7
#define RFPARAM_ERR_CAP_CODE_NULL                   -8
#define RFPARAM_ERR_TCAL_EN_NULL                    -9
#define RFPARAM_ERR_TCAL_LINEAR_OR_FOLLOW_NULL      -10
#define RFPARAM_ERR_TCAL_CHANNELS_NULL              -11
#define RFPARAM_ERR_TCAL_CHANNEL_OS_NULL            -12
#define RFPARAM_ERR_TCAL_CHANNEL_OS_LOW_NULL        -13
#define RFPARAM_ERR_TCAL_ROOM_OS_NULL               -14
#define RFPARAM_ERR_PWR_BLE                         -15

#define RFPARAM_ERR_PARAM_CHECK                    	-50

/* apply flag */
#define RFPARAM_APPLY_NONE          0x00000000
#define RFPARAM_APPLY_PWR_OFFSET    0x00000001
#define RFPARAM_APPLY_PWR_BLE       0x00000002
#define RFPARAM_APPLY_PWR_11B       0x00000004
#define RFPARAM_APPLY_PWR_11G       0x00000008
#define RFPARAM_APPLY_PWR_11N       0x00000010
#define RFPARAM_APPLY_CAP_CODE      0x00000020
#define RFPARAM_APPLY_TCAL          0x00000040
#define RFPARAM_APPLY_ALL           0xffffffff

/* type flag */
#define TYPE_UINT16  0
#define TYPE_INT16   1
#define TYPE_UINT8   2
#define TYPE_INT8    3

/* tlv store type */
#define RFPARAM_TLV_STORE_TYPE_RAM    0
#define RFPARAM_TLV_STORE_TYPE_FLASH  1

#define RFPARAM_BASE_ADDR_MASK     0xff000000
#define RFPARAM_XIP_BASE_ADDR      0x58000000
#define RFPARAM_RAM_BASE_ADDR      0x3E000000

typedef volatile struct{
    uint16_t Tchannels[5];
    int16_t Tchannel_os[5];
    int16_t Tchannel_os_low[5];
    int16_t Troom_os;
    uint8_t en_tcal;
    uint8_t linear_or_follow;
} tcal_param_struct;

typedef struct rfparam{
    /*tcal*/
    /* ToDo */
    tcal_param_struct tcal;

    /*pwr offset*/
    char pwr_offset_mode[4];
    int8_t pwr_offset[14];

    /*pwr:BLE*/
    int8_t pwr_ble;

    /*pwr:wifi*/
    int8_t pwr_11b[4];
    int8_t pwr_11g[8];
    int8_t pwr_11n[8];

    /*xtal*/
    char cap_code_mode[4];
    uint8_t cap_code[5];

}rfparam_t;

typedef struct rf_golden_param{
    uint64_t magic_code1;
    uint64_t magic_code2;

    uint16_t capcode_type;
    uint8_t capcode[5];

    uint16_t pwr_offset_type;
    char pwr_offset[14];

    uint16_t ble_pwr_offset_type;
    char ble_pwr_offset[20];

    uint32_t crc32;

} rf_golden_param_t;

extern struct tx_policy_tbl txl_frame_pol_24G;
extern void phy_powroffset_set(int8_t power_offset[14]);

#define rfparam_printf(...)   printf("rfparam>>"__VA_ARGS__)//printf("rfparam_adapter:"##__VA_ARGS__)

int32_t rfparam_init(uint32_t base_addr, rfparam_t *rf_para, uint32_t apply_flag);
int32_t rfparam_set_cap_code_with_option(uint32_t base_addr,uint32_t tlv_store_type);
int32_t rfparam_update_wifi_tx_pwr_table(uint32_t base_addr,int8_t pwr_dbm_user_define);
uint8_t rfparam_set_wifi_tx_pwr(void);
int32_t rfparam_set_wifi_pwroffset_with_option(uint32_t base_addr,uint32_t tlv_store_type);
int32_t rfparam_set_ble_tx_pwr(uint32_t base_addr,int8_t pwr_dbm_user_define,uint16_t tx_channel);
int32_t rfparam_write_golden_power_offset(int8_t power_offset[14]);
int32_t rfparam_write_golden_capcode(uint8_t capcode);
int32_t rfparam_read_golden_power_offset(int8_t power_offset[14]);
int32_t rfparam_read_golden_capcode(uint8_t *capcode);
int32_t rfparam_read_golden_ble_power_offset(int8_t power_offset[20]);
int32_t rfparam_write_golden_ble_power_offset(int8_t power_offset[20]);
int32_t rfparam_set_ble_pwroffset_with_option(uint32_t base_addr,uint32_t tlv_store_type);

#endif
