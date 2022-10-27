/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include "soc.h"
#include "drv/wdt.h"
#include "aos/hal/wdg.h"

/**
 * This function will initialize the on board CPU hardware watch dog
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    int32_t ret;

    if (wdg == NULL) {
        return -1;
    }

    wdt_handle_t* wdt_handle = csi_wdt_initialize(wdg->port, NULL);
    if (wdt_handle == NULL) {
        return -1;
    }

    wdg->priv = (wdt_handle_t*)wdt_handle;
    ret = csi_wdt_set_timeout(wdt_handle, wdg->config.timeout);
    if (ret < 0) {
        return -1;
    }

    ret = csi_wdt_start((wdt_handle_t)wdg->priv);

    if (ret < 0) {
        return -1;
    }

    return 0;
}
/**
 * Reload watchdog counter.
 *
 * @param[in]  wdg  the watch dog device
 */
void hal_wdg_reload(wdg_dev_t *wdg)
{
    wdt_handle_t* wdt_handle = (wdt_handle_t*)wdg->priv;

    if (wdg == NULL) {
        return;
    }

    csi_wdt_restart(wdt_handle);
}
/**
 * This function performs any platform-specific cleanup needed for hardware watch dog.
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    int32_t ret;
    wdt_handle_t* wdt_handle = (wdt_handle_t*)wdg->priv;

    if (wdg == NULL || wdt_handle == NULL) {
        return -1;
    }

    ret = csi_wdt_uninitialize(wdt_handle);

    if (ret < 0) {
        return -1;
    }

    return 0;
}
