#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#ifndef CONFIG_KERNEL_NONE
#include <devices/hal/hci_impl.h>
#include "ulog/ulog.h"
#include "co_list.h"
#include "log.h"
#define TAG "HCI_ADAPTER"
uint8_t *ll_hci_read_buf;
void (*ll_hci_read_callback)(void *,uint8_t);
void *ll_hci_read_param;
uint16_t ll_hci_read_size;

hci_event_cb_t host_event_cb;
void *host_priv;
uint8_t *ll_hci_write_buf;
void (*ll_hci_write_callback)(void *,uint8_t);
void *ll_hci_write_param;
uint16_t ll_hci_write_size;

struct downstream_data
{
    struct co_list_hdr hdr;
    uint16_t remain_size;
    uint8_t data[];
};

struct co_list host_sent_data_list;
uint8_t *send_ptr;

void ble_ll_sched(void);

void host_hci_read_event_set()
{
    if(host_event_cb)
    {
        host_event_cb(HCI_EVENT_READ,ll_hci_write_size,host_priv);
    }
}

static void hci_copy_from_send_buf()
{
    if(ll_hci_read_size == 0)
    {
        return;
    }
    struct downstream_data *buf = (struct downstream_data *)co_list_pick(&host_sent_data_list);
    if(buf)
    {
        if(send_ptr==NULL)
        {
            send_ptr = buf->data;
        }
        memcpy(ll_hci_read_buf,send_ptr,ll_hci_read_size);
        send_ptr += ll_hci_read_size;
        buf->remain_size -= ll_hci_read_size;
        if(buf->remain_size==0)
        {
            send_ptr = NULL;
            co_list_pop_front(&host_sent_data_list);
            aos_free(buf);
        }
        ll_hci_read_size = 0;
        ll_hci_read_callback(ll_hci_read_param,0);
    }
}

static int host_hci_send(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    struct downstream_data *buf = aos_malloc(sizeof(struct downstream_data) + size);
    buf->remain_size = size;
    memcpy(buf->data,data,size);
    co_list_push_back(&host_sent_data_list,&buf->hdr);
    //printf("send");
    //LOG_HEX(data,size);
    hci_copy_from_send_buf();
    return size;
}

void ll_hci_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *,uint8_t), void* dummy)
{
    ll_hci_read_buf = bufptr;
    ll_hci_read_size = size;
    ll_hci_read_callback = callback;
    ll_hci_read_param = dummy;
    hci_copy_from_send_buf();
}

static int host_set_event(aos_dev_t *dev, hci_event_cb_t event, void *priv)
{
    host_event_cb = event;
    host_priv = priv;
    return 0;
}

static int host_hci_recv(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    if(ll_hci_write_size)
    {
        uint16_t copy_size = ll_hci_write_size > size ? size : ll_hci_write_size;
        memcpy(data,ll_hci_write_buf,copy_size);
        ll_hci_write_buf += copy_size;
        ll_hci_write_size -= copy_size;
        if(ll_hci_write_size==0)
        {
            ll_hci_write_callback(ll_hci_write_param,0);
        }
        return copy_size;
    }else
    {
//        LOGD(TAG,"ll sched");
        ble_ll_sched();
        return 0;
    }
}


void ll_hci_write(uint8_t *bufptr, uint32_t size, void (*callback)(void *,uint8_t), void* dummy)
{
    ll_hci_write_buf = bufptr;
    ll_hci_write_size = size;
    ll_hci_write_callback = callback;
    ll_hci_write_param = dummy;
    host_hci_read_event_set();
}

void ll_hci_flow_on(void)
{


}

bool ll_hci_flow_off(void)
{
    return true;
}

static int host_hci_start(aos_dev_t *dev, hci_driver_send_cmd_t send_cmd)
{
    return 0;
}

static aos_dev_t *hal_init(driver_t *drv, void *g_uart_config, int id)
{
    hci_driver_t *dev = (hci_driver_t *)device_new(drv, sizeof(hci_driver_t), id);
    return (aos_dev_t *)dev;
}

#define hal_uninit device_free

static int hal_open(aos_dev_t *dev)
{
    return 0;
}

static int hal_close(aos_dev_t *dev)
{
    return 0;
}

static hci_driver_t hci_driver = {
    .drv = {
        .name   = "hci",
        .init   = hal_init,
        .uninit = hal_uninit,
        .lpm    = NULL,
        .open   = hal_open,
        .close  = hal_close,
    },
    .set_event = host_set_event,
    .send      = host_hci_send,
    .recv      = host_hci_recv,
    .start     = host_hci_start,
};

void ble_le5010_register()
{
    driver_register(&hci_driver.drv, NULL, 0);
}
#endif
