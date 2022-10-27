#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>

void btble_controller_init(uint8_t task_priority);
void ble_controller_deinit(void);
void btble_set_default_addr(uint8_t addr[6]);

// return sleep duration, in unit of 1/32768s
// if 0, means not allow sleep
// if -1, means allow sleep, but there is no end of sleep interrupt (ble core deep sleep is not enabled)
int32_t ble_controller_sleep(void);
bool ble_controller_sleep_is_ongoing(void);

void ble_controller_set_tx_pwr(int ble_tx_power);

char *btble_controller_get_lib_ver(void);
#endif
