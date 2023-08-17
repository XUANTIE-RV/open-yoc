/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/wifi.h>
#include <string.h>

static aos_status_t _devfs_wifi_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case WIFI_IOC_INIT:
    {
        if (rvm_hal_wifi_init(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_DEINIT:
    {
        if (rvm_hal_wifi_deinit(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_RESET:
    {
        if (rvm_hal_wifi_reset(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SET_MODE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_mode_t mode;
        memcpy(&mode, (void *)arg, sizeof(rvm_hal_wifi_mode_t));
        if (rvm_hal_wifi_set_mode(dev, mode)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_GET_MODE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_mode_t mode;
        if (rvm_hal_wifi_get_mode(dev, &mode)) {
            return -EIO;
        }
        memcpy((void *)arg, &mode, sizeof(rvm_hal_wifi_mode_t));
        break;
    }
    case WIFI_IOC_INSTALL_EVENT_CB:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_event_func evt_cb;
        memcpy(&evt_cb, (void *)arg, sizeof(rvm_hal_wifi_event_func));
        if (rvm_hal_wifi_install_event_cb(dev, &evt_cb)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SET_PROTOCOL:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        uint8_t protocol_bitmap;
        memcpy(&protocol_bitmap, (void *)arg, sizeof(uint8_t));
        if (rvm_hal_wifi_set_protocol(dev, protocol_bitmap)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_GET_PROTOCOL:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_get_protocol(dev, msg.protocol_bitmap)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SET_COUNTRY:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_country_t country;
        memcpy(&country, (void *)arg, sizeof(rvm_hal_wifi_country_t));
        if (rvm_hal_wifi_set_country(dev, country)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_GET_COUNTRY:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_country_t country;
        if (rvm_hal_wifi_get_country(dev, &country)) {
            return -EIO;
        }
        memcpy((void *)arg, &country, sizeof(rvm_hal_wifi_country_t));
        break;
    }
    case WIFI_IOC_SET_MAC_ADDR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_set_mac_addr(dev, msg.mac)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_GET_MAC_ADDR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_get_mac_addr(dev, msg.mac)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SET_AUTO_RECONNECT:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        bool en;
        memcpy(&en, (const void *)arg, sizeof(bool));
        if (rvm_hal_wifi_set_auto_reconnect(dev, en)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_GET_AUTO_RECONNECT:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_get_auto_reconnect(dev, msg.en)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SET_LPM:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_lpm_mode_t mode;
        memcpy(&mode, (void *)arg, sizeof(rvm_hal_wifi_lpm_mode_t));
        if (rvm_hal_wifi_set_lpm(dev, mode)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_GET_LPM:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_lpm_mode_t mode;
        if (rvm_hal_wifi_get_lpm(dev, &mode)) {
            return -EIO;
        }
        memcpy((void *)arg, &mode, sizeof(rvm_hal_wifi_lpm_mode_t));
        break;
    }
    case WIFI_IOC_POWER_ON:
    {
        if (rvm_hal_wifi_power_on(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_POWER_OFF:
    {
        if (rvm_hal_wifi_power_off(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_START_SCAN:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_start_scan(dev, msg.config, msg.block)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_START:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_config_t config;
        memcpy(&config, (void *)arg, sizeof(rvm_hal_wifi_config_t));
        if (rvm_hal_wifi_start(dev, &config)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_STOP:
    {
        if (rvm_hal_wifi_stop(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_STA_GET_LINK_STATUS:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_ap_record_t ap_info;
        if (rvm_hal_wifi_sta_get_link_status(dev, &ap_info)) {
            return -EIO;
        }
        memcpy((void *)arg, &ap_info, sizeof(rvm_hal_wifi_ap_record_t));
        break;
    }
    case WIFI_IOC_AP_GET_STA_LIST:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_sta_list_t sta;
        if (rvm_hal_wifi_ap_get_sta_list(dev, &sta)) {
            return -EIO;
        }
        memcpy((void *)arg, &sta, sizeof(rvm_hal_wifi_sta_list_t));
        break;
    }
    case WIFI_IOC_START_MONITOR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_promiscuous_cb_t cb;
        memcpy(&cb, (void *)arg, sizeof(rvm_hal_wifi_promiscuous_cb_t));
        if (rvm_hal_wifi_start_monitor(dev, cb)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_STOP_MONITOR:
    {
        if (rvm_hal_wifi_stop_monitor(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_START_MGNT_MONITOR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_wifi_mgnt_cb_t cb;
        memcpy(&cb, (void *)arg, sizeof(rvm_hal_wifi_mgnt_cb_t));
        if (rvm_hal_wifi_start_mgnt_monitor(dev, cb)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_STOP_MGNT_MONITOR:
    {
        if (rvm_hal_wifi_stop_mgnt_monitor(dev)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SEND_80211_RAW_FRAME:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_send_80211_raw_frame(dev, msg.buffer, msg.len)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SET_CHANNEL:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_set_channel(dev, *msg.primary, *msg.second)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_GET_CHANNEL:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_wifi_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wifi_dev_msg_t));
        if (rvm_hal_wifi_get_channel(dev, msg.primary, msg.second)) {
            return -EIO;
        }
        break;
    }
    case WIFI_IOC_SET_SMARTCFG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        int enable;
        memcpy(&enable, (const void *)arg, sizeof(int));
        if (rvm_hal_wifi_set_smartcfg(dev, enable)) {
            return -EIO;
        }
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    if ((cmd & DEV_NETDRV_IOCTL_CMD_BASE) == DEV_NETDRV_IOCTL_CMD_BASE) {
        extern aos_status_t _devfs_netdrv_ioctl(devfs_file_t *file, int cmd, uintptr_t arg);
        // netdrv的逻辑
        ret = _devfs_netdrv_ioctl(file, cmd, arg);
    } else if (cmd > DEV_WIFI_IOCTL_CMD_BASE) {
        // 上面switch的逻辑
        return ret;
    } else {
        // 错误
        ret = -EINVAL;
    }

    return ret;
}

static const devfs_file_ops_t devfs_wifi_ops = {
    .ioctl      = _devfs_wifi_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t wifi_ops_node = {
    .name = "wifi",
    .ops = (devfs_file_ops_t *)&devfs_wifi_ops,
};

void wifi_devfs_init(void)
{
    devices_add_devfs_ops_node(&wifi_ops_node);
}
VFS_DEV_DRIVER_ENTRY(wifi_devfs_init);

#endif

#endif

