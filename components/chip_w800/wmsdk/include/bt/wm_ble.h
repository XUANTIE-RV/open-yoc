/**
 * @file    wm_ble.h
 *
 * @brief   Bluetooth API
 *
 * @author  WinnerMicro
 *
 * Copyright (c) 2020 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_BLE_H
#define WM_BLE_H

#include "wm_bt_def.h"

/**
 * @defgroup BT_APIs Bluetooth APIs
 * @brief Bluetooth related APIs
 */

/**
 * @addtogroup BT_APIs
 * @{
 */

/**
 * @defgroup BLE_APIs BLE APIs
 * @brief BLE APIs
 */

/**
 * @addtogroup BLE_APIs
 * @{
 */

/**
 * @brief          initialize the application callback function
 *
 * @param[in]      *p_callback      pointer on callback function
 *
 * @return         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_ble_dm_init(tls_ble_dm_callback_t callback);

/**
 * @brief          start/stop ble advertisement
 *
 * @param[in]      start      TRUE enable; FALSE disable
 *
 * @return         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_ble_adv(bool start);

/**
 * @brief          configure the advertisment content
 *
 * @param[in]      *data        @ref btif_dm_adv_data_t
 *
 * @retval         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_ble_set_adv_data(tls_ble_dm_adv_data_t *data);

/**
 * @brief          configure the advertisment parameters
 *
 * @param[in]      *param        @ref btif_dm_adv_param_t
 *
 * @retval         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_ble_set_adv_param(tls_ble_dm_adv_param_t *param);

/**
 * @brief          start/stop ble scan
 *
 * @param[in]      start        TRUE enable; FALSE disable
 *
 * @retval         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_ble_scan(bool start);

/**
 * @brief          configure the scan parameters
 *
 * @param[in]      window        scan window size
 * @param[in]      interval      scan interval length
 *
 * @retval         @ref tls_bt_status_t
 *
 * @note           interval should greater or equals to windows,
 *                 both range should be within (0x0004, 0x4000)
 */
tls_bt_status_t tls_ble_set_scan_param(int window, int interval);

/**
 * @brief          enable a async process evt
 *
 * @param[in]      id               user specific definition
 * @param[in]      *p_callback      callback function
 *
 * @return         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_dm_evt_triger(int id, tls_ble_dm_triger_callback_t callback);

/**
 * @brief          configure the max transmit unit
 *
 * @param[in]      *bd_addr     the remote device address
 * @param[in]      length       range [27 - 251]
 *
 * @return         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_dm_set_data_length(tls_bt_addr_t *bd_addr, uint16_t length);

/**
 * @brief          configure the ble privacy
 *
 * @param[in]      enable   TRUE:  using rpa/random address, updated every 15 mins
 **                         FALSE: public address
 *
 * @return         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_dm_set_privacy(uint8_t enable);

/**
 * @brief          update the connection parameters
 *
 * @param[in]      *bd_addr         remote device address
 * @param[in]      min_interval
 * @param[in]      max_interval
 * @param[in]      latency
 * @param[in]      timeout
 *
 * @return         @ref tls_bt_status_t
 *
 * @note           None
 */
tls_bt_status_t tls_ble_conn_parameter_update(const tls_bt_addr_t *bd_addr, 
                                             int min_interval,
                                             int max_interval, 
                                             int latency, 
                                             int timeout);

/**
 * @brief          read the remote device signal strength connected
 *
 * @param[in]      *bd_addr         remote device address
 *
 * @return         dbm
 *
 * @note           None
 */
int8_t tls_dm_read_remote_rssi(const tls_bt_addr_t *bd_addr);



/**
 * @}
 */

/**
 * @}
 */
 
#endif /* WM_BLE_H */

