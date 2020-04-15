/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <aos/log.h>
#include <aos/debug.h>

#include <sal.h>
#include <sal2lwip.h>

#define TAG "salm"
#define SAL_LOGE(format, ...)  //LOGE(TAG, format, ##__VA_ARGS__)

static sal_op_t *g_sal_module = NULL;

int sal_module_register(sal_op_t *module)
{
    aos_check_return_einval(module && (g_sal_module==NULL));

    // if (NULL == module) {
    //     SAL_LOGE(TAG, "sal module register invalid input\n");
    //     return -1;
    // }

    // if (NULL != g_sal_module) {
    //     SAL_LOGE(TAG, "sal module have already registered\n");
    //     return -1;
    // }

    g_sal_module = module;

    return 0;
}

int sal_module_init(void)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->init);

    // if (NULL == g_sal_module) {
    //     LOGI(TAG, "sal module init fail for there is no sal module registered yet \n");
    //     return 0;
    // }

    // if (NULL == g_sal_module->init) {
    //     SAL_LOGE(TAG, "init function in sal module is null \n");
    //     return -1;
    // }

    /*show we deinit module at first ?*/
    err = g_sal_module->init();

    if (err) {
        SAL_LOGE(TAG, "init %d", err);
    }

    return err;
}

int sal_module_deinit(void)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->init);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module deinit fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->deinit) {
    //     SAL_LOGE(TAG, "deinit function in sal module is null \n");
    //     return -1;
    // }

    err = g_sal_module->deinit();

    if (err) {
        SAL_LOGE(TAG, "deinit", err);
    }

    return err;
}


int sal_module_start(sal_conn_t *conn)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->start && conn && conn->addr);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module start fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->start) {
    //     SAL_LOGE(TAG, "start function in sal module is null \n");
    //     return -1;
    // }

    // if (NULL == conn) {
    //     SAL_LOGE(TAG, "invalid input\n");
    //     return -1;
    // }

    err = g_sal_module->start(conn);

    if (err) {
        SAL_LOGE(TAG, "start %d", err);
    }

    return err;
}

int sal_module_close(int fd, int32_t remote_port)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->close);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module close fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->close) {
    //     SAL_LOGE(TAG, "close function in sal module is null \n");
    //     return -1;
    // }

    err = g_sal_module->close(fd, remote_port);

    if (err) {
         SAL_LOGE(TAG, "close %d", err);
    }

    return err;
}

int sal_module_send(int fd, uint8_t *data, uint32_t len,
                    char remote_ip[16], int32_t remote_port, int32_t timeout)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->send && data);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module send fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->send) {
    //     SAL_LOGE(TAG, "send function in sal module is null \n");
    //     return -1;
    // }

    // if (NULL == data) {
    //     SAL_LOGE(TAG, "invalid input\n");
    //     return -1;
    // }

    err = g_sal_module->send(fd, data, len, remote_ip, remote_port, timeout);

    if (err) {
        SAL_LOGE(TAG, "send %d", err);
    }

    return err;
}

int sal_module_domain_to_ip(char *domain, char ip[16])
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->domain_to_ip && domain && ip);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module domain_to_ip fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->domain_to_ip) {
    //     SAL_LOGE(TAG, "domai_to_ip function in sal module is null \n");
    //     return -1;
    // }

    // if (NULL == domain) {
    //     SAL_LOGE(TAG, "invalid input\n");
    //     return -1;
    // }

    err = g_sal_module->domain_to_ip(domain, ip);

    if (err) {
        SAL_LOGE(TAG, "domain2ip %d\n", err);
    }

    return err;
}

int sal_module_register_netconn_data_input_cb(netconn_data_input_cb_t cb)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->register_netconn_data_input_cb);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module recv fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->register_netconn_data_input_cb) {
    //     SAL_LOGE(TAG, "recv function in sal module is null \n");
    //     return -1;
    // }

    err = g_sal_module->register_netconn_data_input_cb(cb);

    if (err) {
        SAL_LOGE(TAG, "recv %d", err);
    }

    return err;
}

int sal_module_register_netconn_close_cb(netconn_close_cb_t cb)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->register_netconn_close_cb);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module close fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->register_netconn_close_cb) {
    //     SAL_LOGE(TAG, "close function in sal module is null \n");
    //     return -1;
    // }

    err = g_sal_module->register_netconn_close_cb(cb);

    if (err) {
        SAL_LOGE(TAG, "nc colse %d\n", err);
    }

    return err;
}

#ifdef SAL_SERVER
int sal_module_register_client_status_notify_cb(netconn_client_status_notify_t cb)
{
    int err = 0;

    aos_check_return_einval(g_sal_module && g_sal_module->register_netconn_client_status_notify);

    // if (NULL == g_sal_module) {
    //     SAL_LOGE(TAG, "sal module client status fail for there is no sal module registered yet \n");
    //     return -1;
    // }

    // if (NULL == g_sal_module->register_netconn_client_status_notify) {
    //     SAL_LOGE(TAG, "client status function in sal module is null \n");
    //     return -1;
    // }

    err = g_sal_module->register_netconn_client_status_notify(cb);

    if (err) {
        SAL_LOGE(TAG, "module recv fail err=%d\n", err);
    }

    return err;
}
#endif
