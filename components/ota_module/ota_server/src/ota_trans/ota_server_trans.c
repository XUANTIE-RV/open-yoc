/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include "stdint.h"
#include "ota_trans/ota_server_trans.h"
#include "ota_module.h"
#include "errno.h"
#include "ulog/ulog.h"

#if defined(CONFIG_OTA_SERVER_TRANS_GATT) && CONFIG_OTA_SERVER_TRANS_GATT
#include "ota_trans/ota_server_trans_gatt.h"
#endif

#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART
#include "ota_trans/ota_server_trans_hci_uart.h"
#endif

#define TAG "OTA_SERVER_TRANS"

int ota_server_trans_init(uint8_t channel, ota_server_event_cb cb)
{
    int ret = 0;
    if (!cb) {
        return -1;
    }

    if (channel == OTA_CHANNEL_ON_GATT) {
#if defined(CONFIG_OTA_SERVER_TRANS_GATT) && CONFIG_OTA_SERVER_TRANS_GATT
        ret = ota_server_trans_gatt_init(cb);
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "init ota trans gatt failed\r\n");
            return ret;
        }
#else
        return -ENOTSUP;
#endif

    } else if (channel == OTA_CHANNEL_ON_HCI_UART) {
#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART
        ret = ota_server_trans_hci_uart_init(cb);

        if (ret && ret != -EALREADY) {
            LOGE(TAG, "init ota trans uart failed\r\n");
            return ret;
        }
#else
        return -ENOTSUP;
#endif

    } else {
        LOGE(TAG, "server init failed,unsupport trans channel %02x", channel);
        return -EINVAL;
    }
    (void)ret;
    return 0;
}

int ota_server_trans_prepare(uint8_t channel, slist_t *dev_list)
{
    int ret = 0;
    if (channel == OTA_CHANNEL_ON_GATT) {
#if defined(CONFIG_OTA_SERVER_TRANS_GATT) && CONFIG_OTA_SERVER_TRANS_GATT
        ret = ota_server_trans_gatt_prepare(dev_list);
        if (ret) {
            LOGE(TAG, "ota gatt prepare failed %d", ret);
            return ret;
        }
#else
        return -ENOTSUP;
#endif

    } else if (channel == OTA_CHANNEL_ON_HCI_UART) {
#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART
        ret = ota_server_trans_hci_uart_prepare(dev_list);
        if (ret) {
            LOGE(TAG, "ota uart prepare failed %d", ret);
            return ret;
        }
#else
        return -ENOTSUP;
#endif

    } else {
        LOGE(TAG, "server prepare failed,unsupport trans channel %02x", channel);
        return -EINVAL;
    }
    (void)ret;
    return 0;
}

int ota_server_trans_send(uint8_t channel, uint8_t *data, uint8_t len, uint8_t ack)
{
    if (channel == OTA_CHANNEL_ON_GATT) {
#if defined(CONFIG_OTA_SERVER_TRANS_GATT) && CONFIG_OTA_SERVER_TRANS_GATT
        return ota_server_trans_gatt_send(data, len, ack);
#else
        return -ENOTSUP;
#endif

    } else if (channel == OTA_CHANNEL_ON_HCI_UART) {
#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART
        return ota_server_trans_hci_uart_send(data, len, ack);
#else
        return -ENOTSUP;
#endif

    } else {
        return -1;
    }
}

int ota_server_trans_disconnect(uint8_t channel)
{
    if (channel == OTA_CHANNEL_ON_GATT) {
#if defined(CONFIG_OTA_SERVER_TRANS_GATT) && CONFIG_OTA_SERVER_TRANS_GATT
        return ota_server_trans_gatt_disconnect();
#else
        return -ENOTSUP;
#endif

    } else if (channel == OTA_CHANNEL_ON_HCI_UART) {
#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART
        return ota_server_trans_hci_uart_disconnect();
#else
        return -ENOTSUP;
#endif
    }
    return 0;
}

int ota_server_trans_unregister(uint8_t channel)
{
    int ret = 0;

    if (channel == OTA_CHANNEL_ON_GATT) {
#if defined(CONFIG_OTA_SERVER_TRANS_GATT) && CONFIG_OTA_SERVER_TRANS_GATT
        ret = ota_server_trans_gatt_unregister();
        if (ret) {
            LOGE(TAG, "ota gatt unregister failed %d", ret);
            return ret;
        }
#else
        return -ENOTSUP;
#endif
    } else if (channel == OTA_CHANNEL_ON_HCI_UART) {
#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART
        ret = ota_server_trans_hci_uart_unregister();
        if (ret) {
            LOGE(TAG, "ota uart unregister failed", ret);
            return ret;
        }
#else
        return -ENOTSUP;
#endif
    }
    (void)ret;
    return -EINVAL;
}

int ota_server_trans_reset(uint8_t channel)
{
    int ret = 0;

    if (channel == OTA_CHANNEL_ON_HCI_UART) {
#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART
        ret = ota_server_trans_hci_uart_reset();
        if (ret) {
            LOGE(TAG, "ota HCI uart reset failed %d", ret);
            return ret;
        }
#else
        return -ENOTSUP;
#endif
    }
    (void)ret;
    return 0;
}
