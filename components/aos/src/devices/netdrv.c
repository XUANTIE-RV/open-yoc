/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <devices/hal/net_impl.h>

#define NET_DRIVER(dev)  ((net_ops_t *)(((netdev_driver_t *)dev->drv)->net_ops))


int hal_net_get_mac_addr(aos_dev_t *dev, uint8_t *mac)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}

int hal_net_set_mac_addr(aos_dev_t *dev, const uint8_t *mac)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}

int hal_net_get_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_dns_server(dev, ipaddr, num);
    device_unlock(dev);

    return ret;
}

int hal_net_set_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_dns_server(dev, ipaddr, num);
    device_unlock(dev);

    return ret;
}

int hal_net_set_hostname(aos_dev_t *dev, const char *name)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_hostname(dev, name);
    device_unlock(dev);

    return ret;
}

const char *hal_net_get_hostname(aos_dev_t *dev)
{
    const char* ret;

    if (dev == NULL) {
        return NULL;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_hostname(dev);
    device_unlock(dev);

    return ret;
}

int hal_net_set_link_up(aos_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_link_up(dev);
    device_unlock(dev);

    return ret;
}

int hal_net_set_link_down(aos_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    if (NET_DRIVER(dev)->set_link_down)
        ret = NET_DRIVER(dev)->set_link_down(dev);
    else
        ret = -1;
    device_unlock(dev);

    return ret;
}

int hal_net_start_dhcp(aos_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    if (NET_DRIVER(dev)->start_dhcp)
        ret = NET_DRIVER(dev)->start_dhcp(dev);
    else
        ret = -1;
    device_unlock(dev);

    return ret;

}
int hal_net_stop_dhcp(aos_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->stop_dhcp(dev);
    device_unlock(dev);

    return ret;
}

int hal_net_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{

    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_ipaddr(dev, ipaddr, netmask, gw);
    device_unlock(dev);

    return ret;

}


int hal_net_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_ipaddr(dev, ipaddr, netmask, gw);
    device_unlock(dev);

    return ret;
}



int hal_net_ping(aos_dev_t *dev, int type, char *remote_ip)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->ping(dev, type, remote_ip);
    device_unlock(dev);

    return ret;
}


int hal_net_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->subscribe(dev, event, cb, param);
    device_unlock(dev);

    return ret;
}

int hal_net_unsubscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->unsubscribe(dev, event, cb, param);
    device_unlock(dev);

    return ret;
}
