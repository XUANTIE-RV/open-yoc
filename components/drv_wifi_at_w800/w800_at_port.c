#include <stdio.h>
#include <errno.h>

#include <aos/aos.h>
#include <yoc/at_port.h>
#include <yoc/atparser.h>
#include <aos/ringbuffer.h>

#include <drv/spi.h>
#include <drv/gpio.h>
#include <drv/gpio_pin.h>
#include <drv/pin.h>
#include "soc.h"
#include <devices/w800.h>


/* SPI CMD define */
#define SPI_REG_INT_STTS		0x06
#define SPI_REG_RX_DAT_LEN		0x02
#define SPI_CMD_TX_DATA			0x90
#define SPI_CMD_RX_DATA			0x10

#define SPI_EVENT_READ         (0X01)
#define SPI_EVENT_WRITE        (0X02)
#define SPI_EVENT_ERR          (0X04)
// #define param->buffer_size       (1024*4)

//#define SPI_USE_DMA

#define TAG "w800_at"

static csi_gpio_pin_t         spi_int_pin;
static csi_gpio_pin_t         spi_cs_pin;
static csi_gpio_pin_t         spi_wakeup_pin;
static csi_spi_t              spi_handle;
#ifdef SPI_USE_DMA
static csi_dma_ch_t           spi_send_dma;
static csi_dma_ch_t           spi_recv_dma;
#endif
static aos_sem_t              spi_recv_sem;
static dev_ringbuf_t          spi_ringbuffer;
static char                  *spi_recv_buffer;
static channel_event_t        spi_channel_cb;
static void                  *spi_channel_priv;
static aos_event_t            spi_event;

#define CS_HIGH csi_gpio_pin_write(&spi_cs_pin, GPIO_PIN_HIGH);
#define CS_LOW  csi_gpio_pin_write(&spi_cs_pin, GPIO_PIN_LOW);

static void spi_in_int_cb(csi_gpio_pin_t *pin, void *arg)
{
    aos_sem_signal(&spi_recv_sem);
}

volatile uint32_t g_spi_cb_count;
volatile uint32_t g_spi_flag;
static void spi_event_cb(csi_spi_t *spi, csi_spi_event_t event, void *arg)
{
    g_spi_cb_count++;
    if (event == SPI_EVENT_RECEIVE_COMPLETE) {
        g_spi_flag = 1;
        aos_event_set(&spi_event, SPI_EVENT_READ, AOS_EVENT_OR);
    } else if (event == SPI_EVENT_SEND_COMPLETE) {
        aos_event_set(&spi_event, SPI_EVENT_WRITE, AOS_EVENT_OR);
    } else {
        aos_event_set(&spi_event, SPI_EVENT_ERR, AOS_EVENT_OR);
    }
}

static int spi_resp_len(void)
{
    uint16_t temp = 0;
    uint8_t a,b;
    uint8_t cmd = SPI_REG_INT_STTS;
    int recv_len = 0;

    while (1) {
        CS_LOW;
        csi_spi_send(&spi_handle, &cmd, 1, AOS_WAIT_FOREVER);		//Check if data is ready
        csi_spi_receive(&spi_handle, &a, 1, AOS_WAIT_FOREVER);
        csi_spi_receive(&spi_handle, &b, 1, AOS_WAIT_FOREVER);
        CS_HIGH;

        temp = a | (b << 8);
        if((temp != 0xffff) && (temp & 0x01)) {
            cmd = SPI_REG_RX_DAT_LEN;
            CS_LOW;
            csi_spi_send(&spi_handle, &cmd, 1, AOS_WAIT_FOREVER);		//Check if data is ready
            csi_spi_receive(&spi_handle, &a, 1, AOS_WAIT_FOREVER);
            csi_spi_receive(&spi_handle, &b, 1, AOS_WAIT_FOREVER);
            CS_HIGH;
            recv_len = a | (b << 8);

            // printf("recv len:%d\r\n", recv_len);
            break;
        }
        aos_msleep(100);
    }

    return recv_len;
}

static int spi_recv(uint8_t *buf, int len)
{
    uint8_t cmd = SPI_CMD_RX_DATA;

    // uint32_t flags = csi_irq_save();
    aos_kernel_sched_suspend();
    CS_LOW;
    csi_spi_send(&spi_handle, &cmd, 1, AOS_WAIT_FOREVER);		//Check if data is ready
#ifndef SPI_USE_DMA 
    int ret = csi_spi_receive(&spi_handle, buf, len, AOS_WAIT_FOREVER);
#else 
    uint32_t actl_flags = 0;
    g_spi_flag = 0;
    csi_spi_receive_async(&spi_handle, buf, len);
    while(!g_spi_flag);
//    int ret = aos_event_get(&spi_event, SPI_EVENT_READ, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);
//    if (ret >= 0 && ((actl_flags & SPI_EVENT_READ) == SPI_EVENT_READ)) {
//        ret = len;
//    }
#endif    
    CS_HIGH;
    aos_kernel_sched_resume();
    // csi_irq_restore(flags);

    return ret;
}

static void at_spi_recv_task(void *priv)
{
    int      len  = 0;
    uint8_t *recv = NULL;

    while(1) {
        aos_sem_wait(&spi_recv_sem, AOS_WAIT_FOREVER);

        len = spi_resp_len();
        if (len)
            recv = aos_malloc_check(len);
        else
            continue;

        spi_recv(recv, len);

        while (ringbuffer_available_write_space(&spi_ringbuffer) < (len -1)) {
            aos_msleep(100);
        }

        int w_len = ringbuffer_write(&spi_ringbuffer, recv, len-1);
        if (w_len != (len-1)) {
            LOGD(TAG, "spi buffer is full\r\n");
        } else {
            spi_channel_cb(AT_CHANNEL_EVENT_READ, spi_channel_priv);
        }

        if (recv) {
            aos_free(recv);
            recv = NULL;
        }
    }
}

static void *at_spi_init(const char *name, void *config)
{
    int      ret  = 0;
    w800_wifi_param_t *param = (w800_wifi_param_t *)config;

    csi_pin_set_mux(PA16, PA16_SPI0_SCK);
    csi_pin_set_mux(PA17, PA17_SPI0_MOSI);
    csi_pin_set_mux(PA18, PA18_SPI0_MISO);
    // csi_pin_set_mux(PA15, PA15_SPI0_CS); // CS

    csi_pin_set_mux(param->cs_pin, PIN_FUNC_GPIO); // CS
    csi_pin_set_mux(param->int_pin, PIN_FUNC_GPIO); // INT

    csi_gpio_pin_init(&spi_int_pin, param->int_pin);
    csi_gpio_pin_dir(&spi_int_pin,GPIO_DIRECTION_INPUT);
    csi_gpio_pin_mode(&spi_int_pin,GPIO_MODE_PULLNONE);
    csi_gpio_pin_debounce(&spi_int_pin, true);
    csi_gpio_pin_attach_callback(&spi_int_pin, spi_in_int_cb, NULL);
    csi_gpio_pin_irq_mode(&spi_int_pin,GPIO_IRQ_MODE_FALLING_EDGE);
    csi_gpio_pin_irq_enable(&spi_int_pin, 1);

    csi_gpio_pin_init(&spi_cs_pin, param->cs_pin);
    csi_gpio_pin_mode(&spi_cs_pin,GPIO_MODE_PULLUP);
    csi_gpio_pin_dir(&spi_cs_pin,GPIO_DIRECTION_OUTPUT);
    CS_HIGH;

    csi_gpio_pin_init(&spi_wakeup_pin, param->wakeup_pin);
    csi_gpio_pin_mode(&spi_wakeup_pin,GPIO_MODE_PULLUP);
    csi_gpio_pin_dir(&spi_wakeup_pin,GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(&spi_wakeup_pin, GPIO_PIN_HIGH);

    ret = csi_spi_init(&spi_handle, 0);
    if (ret < 0) {
        printf("csi spi init failed\r\n");
        return NULL;
    }

    csi_spi_mode(&spi_handle, SPI_MASTER);
    ret = csi_spi_baud(&spi_handle, param->baud);

    LOGD(TAG, "#######################spi speed:%d\r\n", ret);
    csi_spi_cp_format(&spi_handle, SPI_FORMAT_CPOL0_CPHA0);
    csi_spi_frame_len(&spi_handle, SPI_FRAME_LEN_8);
    csi_spi_select_slave(&spi_handle, 0);
#ifdef SPI_USE_DMA
    csi_spi_attach_callback(&spi_handle, spi_event_cb, NULL);
    csi_spi_link_dma(&spi_handle, NULL, &spi_recv_dma);
#endif
    aos_task_t task;

    ret = aos_sem_new(&spi_recv_sem, 0);
    // aos_check(ret, NULL);
    ret = aos_event_new(&spi_event, 0);

    ret = aos_task_new_ext(&task, "spi_recv", at_spi_recv_task, NULL, 1536, 9);
    // aos_check(ret, NULL);

    spi_recv_buffer = (char *)aos_malloc_check(param->buffer_size);

    ringbuffer_create(&spi_ringbuffer, spi_recv_buffer, param->buffer_size);

    return (void*)1;
}

static int at_spi_set_event(void *hdl, channel_event_t evt_cb, void *priv)
{
    spi_channel_cb   = evt_cb;
    spi_channel_priv = priv;

    return 0;
}

static int at_spi_send(void *hdl, const char *data, int size)
{
    uint8_t cmd = SPI_CMD_TX_DATA;
    uint8_t tail = 0x00;
    int count = 4-(size+1+2)%4;
    int32_t ret = 0;

    CS_LOW;
    ret = csi_spi_send(&spi_handle, &cmd, 1, AOS_WAIT_FOREVER);
    if (ret < 0) {
        LOGD("at spi", "send cmd err");
    }

    ret = csi_spi_send(&spi_handle, &size, 2, AOS_WAIT_FOREVER);
    if (ret < 0) {
        LOGD("at spi", "send cmd err");
    }

    ret = csi_spi_send(&spi_handle, data, size, AOS_WAIT_FOREVER);
    if (ret < 0) {
        LOGD("at spi", "send data err");
    }

    ret = csi_spi_send(&spi_handle, &tail, 1, AOS_WAIT_FOREVER);
    if (ret < 0) {
        LOGD("at spi", "send tail err");
    }

    for (int i = 0; i < count; i++) {
        ret = csi_spi_send(&spi_handle, &tail, 1, AOS_WAIT_FOREVER);
        if (ret < 0) {
            LOGD("at spi", "send tail(%d) err", i);
        }
    }

    CS_HIGH;

    // LOGD("at spi", "send data(%d)", size);

    return 0;
}

static int at_spi_recv(void *hdl, const char *data, int size, int timeout)
{
    int ret = ringbuffer_read(&spi_ringbuffer, (uint8_t *)data, size);

    return ret < 0 ? 0 : ret;
}

at_channel_t spi_channel = {
    .init       = at_spi_init,
    .set_event  = at_spi_set_event,
    .send       = at_spi_send,
    .recv       = at_spi_recv,
};
