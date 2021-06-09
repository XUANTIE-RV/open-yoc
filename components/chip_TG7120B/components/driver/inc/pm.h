#ifndef _PM_H
#define _PM_H

#define   RET_SRAM0         BIT(0)  /*32K, 0x1fff0000~0x1fff7fff*/
#define   RET_SRAM1         BIT(1)  /*16K, 0x1fff8000~0x1fffbfff*/
#define   RET_SRAM2         BIT(2)  /*16K, 0x1fffc000~0x1fffffff*/

typedef void (*pm_wakeup_by_io_cb)(void *arg);

void drv_pm_sleep_enable();


void drv_pm_sleep_disable();


int drv_pm_enter_standby();

int drv_pm_ram_retention(uint32_t sram);

void drv_pm_io_wakeup_handler_unregister();

void drv_pm_io_wakeup_handler_register(pm_wakeup_by_io_cb wakeup_cb);


#endif
