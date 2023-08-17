/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/netdrv.h>
#include <string.h>

aos_status_t _devfs_netdrv_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case NETDRV_IOC_GET_MAC_ADDR:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }

        uint8_t *mac = (uint8_t *)arg;
        if (rvm_hal_net_get_mac_addr(dev, mac)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_SET_MAC_ADDR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        uint8_t *mac = (uint8_t *)arg;
        if (rvm_hal_net_set_mac_addr(dev, mac)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_GET_DNS_SERVER:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_netdrv_dev_dns_server_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_netdrv_dev_dns_server_msg_t));
        if (rvm_hal_net_get_dns_server(dev, msg.ipaddr, msg.num)) {
            return -EIO;
        }
        memcpy((void *)arg, &msg, sizeof(msg.num) + sizeof(ip_addr_t) * msg.num);
        break;
    }
    case NETDRV_IOC_SET_DNS_SERVER:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_netdrv_dev_dns_server_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_netdrv_dev_dns_server_msg_t));
        if (rvm_hal_net_set_dns_server(dev, msg.ipaddr, msg.num)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_SET_LINK_UP:
    {
        if (rvm_hal_net_set_link_up(dev)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_SET_LINK_DOWN:
    {
        if (rvm_hal_net_set_link_down(dev)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_SET_HOSTNAME:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_net_set_hostname(dev, (const char *)arg)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_GET_HOSTNAME:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        const char *hostname = rvm_hal_net_get_hostname(dev);
        if (hostname)
            strcpy((char *)arg, hostname);
        break;
    }
    case NETDRV_IOC_START_DHCP:
    {
        if (rvm_hal_net_start_dhcp(dev)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_STOP_DHCP:
    {
        if (rvm_hal_net_stop_dhcp(dev)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_SET_IPADDR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_netdrv_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_netdrv_dev_msg_t));
        if (rvm_hal_net_set_ipaddr(dev, &msg.ipaddr, &msg.netmask, &msg.gw)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_GET_IPADDR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_netdrv_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_netdrv_dev_msg_t));
        if (rvm_hal_net_get_ipaddr(dev, &msg.ipaddr, &msg.netmask, &msg.gw)) {
            return -EIO;
        }
        memcpy((void *)arg, &msg, sizeof(rvm_netdrv_dev_msg_t));
        break;
    }
    case NETDRV_IOC_PING:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_netdrv_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_netdrv_dev_msg_t));
        if (rvm_hal_net_ping(dev, msg.type, msg.remote_ip)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_SUBSCRIBE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_netdrv_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_netdrv_dev_msg_t));
        if (rvm_hal_net_subscribe(dev, msg.event, msg.cb, msg.param)) {
            return -EIO;
        }
        break;
    }
    case NETDRV_IOC_UNSUBSCRIBE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_netdrv_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_netdrv_dev_msg_t));
        if (rvm_hal_net_unsubscribe(dev, msg.event, msg.cb, msg.param)) {
            return -EIO;
        }
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

#endif

#endif
