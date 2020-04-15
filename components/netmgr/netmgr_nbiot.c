/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc_config.h>
#include <aos/aos.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <devices/netdrv.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>


//static const char *TAG = "netmgr_nbiot";

int netmgr_nbiot_provision(netmgr_dev_t *node)
{
    netmgr_subscribe(EVENT_NBIOT_LINK_UP);
    netmgr_subscribe(EVENT_NBIOT_LINK_DOWN);

    return 0;

}

int netmgr_nbiot_unprovision(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;

    hal_net_set_link_down(dev);
    device_close(dev);

    return 0;

}


int netmgr_nbiot_info(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;
    netdev_driver_t *drv = dev->drv;
    eth_driver_t *eth_drv = (eth_driver_t *)drv->link_ops;
    eth_drv->ifconfig(dev);
    return 0;
}

int netmgr_nbiot_service(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    int ret = 0;

    switch (rpc->cmd_id) {

        case EVENT_NBIOT_LINK_UP: {
            netmgr_dev_t *node = netmgr_find_dev(&netmgr->dev_list, "nbiot");
            aos_dev_t *dev = node->dev;
            netdev_driver_t *drv = dev->drv;
            eth_driver_t *nbiot_drv = (eth_driver_t *)drv->link_ops;

            nbiot_drv->start(dev);
            //TODO
            event_publish(EVENT_NET_GOT_IP, NULL);

            break;
        }

        case EVENT_NBIOT_LINK_DOWN:
            event_publish(EVENT_NETMGR_NET_DISCON, NULL);

            break;
    }

    return ret;
}

static netmgr_dev_t * netmgr_nbiot_init(struct netmgr_uservice *netmgr)
{
    netmgr_dev_t *node = NULL;

    int ival = 0;

    // get eth configuration
#ifdef CONFIG_KV_SMART
    ival = netmgr_kv_getint(KV_NBIOT_EN);
#else
    ival = 1;
#endif

    if (ival == 1) {
        node = (netmgr_dev_t *)aos_zalloc(sizeof(netmgr_dev_t));

        if (node) {
            node->dev = device_open_id("nbiot", 0);
            aos_assert(node->dev);
            node->provision = netmgr_nbiot_provision;
            node->unprovision = netmgr_nbiot_unprovision;
            node->info = netmgr_nbiot_info;
            node->enable = 1;
            strcpy(node->name, "nbiot");
            node->id = 0;

            slist_add_tail((slist_t *)node, &netmgr->dev_list);
        }

        netmgr_reg_srv_func(EVENT_NBIOT_LINK_UP, netmgr_nbiot_service);
        netmgr_reg_srv_func(EVENT_NBIOT_LINK_DOWN, netmgr_nbiot_service);

    }

    return node;
}


netmgr_hdl_t netmgr_dev_nbiot_init()
{
    return (netmgr_hdl_t)netmgr_nbiot_init(&netmgr_svc);
}

