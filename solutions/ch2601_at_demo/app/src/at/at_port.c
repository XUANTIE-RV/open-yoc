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

#include "main.h"

/* SPI CMD define */
#define SPI_REG_INT_STTS		0x06
#define SPI_REG_RX_DAT_LEN		0x02
#define SPI_CMD_TX_DATA			0x90
#define SPI_CMD_RX_DATA			0x10

#define SPI_RX_BUFFER_LEN       (1024)

#define TAG "w800_at"

static csi_gpio_pin_t         spi_int_pin;
static csi_gpio_pin_t         spi_cs_pin;
static csi_gpio_pin_t         spi_wakeup_pin;
static csi_spi_t              spi_handle;
static aos_sem_t              spi_recv_sem;
static dev_ringbuf_t          spi_ringbuffer;
static char                  *spi_recv_buffer;
static channel_event_t        spi_channel_cb;
static void                  *spi_channel_priv;
static atparser_uservice_t   *spi_at;
static aos_mutex_t g_cmd_mutex;
static int g_net_status = 0;

typedef enum {
    NET_TYPE_NULL = 0,
    NET_TYPE_TCP_SERVER,
    NET_TYPE_TCP_CLIENT,
    NET_TYPE_UDP_UNICAST,
    NET_TYPE_MAX
} net_conn_e;

typedef enum {
    NET_STATUS_UNKNOW = 0,
    NET_STATUS_READY,
    NET_STATUS_LINKDOWN,
    NET_STATUS_CONFIGING,
    NET_STATUS_LINKUP,
    NET_STATUS_GOTIP,
} net_status_t;

#define CS_HIGH csi_gpio_pin_write(&spi_cs_pin, GPIO_PIN_HIGH);
#define CS_LOW  csi_gpio_pin_write(&spi_cs_pin, GPIO_PIN_LOW);

extern int _connect_net_event_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data);
extern int _connect_recv_event_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data);
extern int _connect_ready_event_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data);

static void spi_in_int_cb(csi_gpio_pin_t *pin, void *arg)
{
    aos_sem_signal(&spi_recv_sem);
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

            printf("recv len:%d\r\n", recv_len);
            break;
        }
        aos_msleep(100);
    }

    return recv_len;
}

static int spi_recv(uint8_t *buf, int len)
{
    uint8_t cmd = SPI_CMD_RX_DATA;

    CS_LOW;
    csi_spi_send(&spi_handle, &cmd, 1, AOS_WAIT_FOREVER);		//Check if data is ready
    int ret = csi_spi_receive(&spi_handle, buf, len, AOS_WAIT_FOREVER);
    CS_HIGH;
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

        int w_len = ringbuffer_write(&spi_ringbuffer, recv, len-1);
        if (w_len != (len-1)) {
            printf("11111111111\r\n");
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

    csi_pin_set_mux(PA16, PA16_SPI0_SCK);
    csi_pin_set_mux(PA17, PA17_SPI0_MOSI);
    csi_pin_set_mux(PA18, PA18_SPI0_MISO);
    // csi_pin_set_mux(PA15, PA15_SPI0_CS); // CS
    csi_pin_set_mux(PA15, PIN_FUNC_GPIO); // CS
    csi_pin_set_mux(PA22, PIN_FUNC_GPIO); // INT

    csi_gpio_pin_init(&spi_int_pin, PA22);
    csi_gpio_pin_dir(&spi_int_pin,GPIO_DIRECTION_INPUT);
    csi_gpio_pin_mode(&spi_int_pin,GPIO_MODE_PULLNONE);
    csi_gpio_pin_debounce(&spi_int_pin, true);
    csi_gpio_pin_attach_callback(&spi_int_pin, spi_in_int_cb, NULL);
    csi_gpio_pin_irq_mode(&spi_int_pin,GPIO_IRQ_MODE_FALLING_EDGE);
    csi_gpio_pin_irq_enable(&spi_int_pin, 1);

    csi_gpio_pin_init(&spi_cs_pin, PA15);
    csi_gpio_pin_mode(&spi_cs_pin,GPIO_MODE_PULLUP);
    csi_gpio_pin_dir(&spi_cs_pin,GPIO_DIRECTION_OUTPUT);
    CS_HIGH;

    csi_gpio_pin_init(&spi_wakeup_pin, PA25);
    csi_gpio_pin_mode(&spi_wakeup_pin,GPIO_MODE_PULLUP);
    csi_gpio_pin_dir(&spi_wakeup_pin,GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(&spi_wakeup_pin, GPIO_PIN_HIGH);

    ret = csi_spi_init(&spi_handle, 0);
    if (ret < 0) {
        printf("csi spi init failed\r\n");
        return NULL;
    }

    csi_spi_mode(&spi_handle, SPI_MASTER);
    csi_spi_baud(&spi_handle, 100000);
    csi_spi_cp_format(&spi_handle, SPI_FORMAT_CPOL0_CPHA0);
    csi_spi_frame_len(&spi_handle, SPI_FRAME_LEN_8);
    csi_spi_select_slave(&spi_handle, 0);

    aos_task_t task;

    ret = aos_sem_new(&spi_recv_sem, 0);
    // aos_check(ret, NULL);
    
    ret = aos_task_new_ext(&task, "spi_recv", at_spi_recv_task, NULL, 2048, 9);
    // aos_check(ret, NULL);

    spi_recv_buffer = (char *)aos_malloc_check(SPI_RX_BUFFER_LEN);

    ringbuffer_create(&spi_ringbuffer, spi_recv_buffer, SPI_RX_BUFFER_LEN);

    aos_mutex_new(&g_cmd_mutex);

    return 1;
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
    int count = 4-(size+1)%4;

    CS_LOW;
    csi_spi_send(&spi_handle, &cmd, 1, AOS_WAIT_FOREVER);
    csi_spi_send(&spi_handle, data, size, AOS_WAIT_FOREVER);

    csi_spi_send(&spi_handle, &tail, 1, AOS_WAIT_FOREVER);
    for (int i = 0; i < count; i++) {
        csi_spi_send(&spi_handle, &tail, 1, AOS_WAIT_FOREVER);
    }
    CS_HIGH;

    return 0;
}

static int at_spi_recv(void *hdl, const char *data, int size, int timeout)
{
    int ret = ringbuffer_read(&spi_ringbuffer, (uint8_t *)data, size);

    return ret;
}

static at_channel_t spi_channel = {
    .init       = at_spi_init,
    .set_event  = at_spi_set_event,
    .send       = at_spi_send,
    .recv       = at_spi_recv,
};

void at_channel_init(utask_t *task)
{
    if (task == NULL) {
        task = utask_new("w800", 1 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);
    }

    if (task == NULL) {
        return;
    }

    spi_at = atparser_channel_init(task, NULL, NULL, &spi_channel);

    atparser_debug_control(spi_at, 1);
    atparser_oob_create(spi_at, "+EVENT=NET,", _connect_net_event_handler, NULL);
    atparser_oob_create(spi_at, "+EVENT=RECV,", _connect_recv_event_handler, NULL);
    atparser_oob_create(spi_at, "+EVENT=READY", _connect_ready_event_handler, NULL);
}

static void at_test(void)
{
    atparser_clr_buf(spi_at);

    if (atparser_send(spi_at, "AT") == 0) {
        if (atparser_recv(spi_at, "OK") == 0) {
            printf("at test ok\r\n");
        }
    }

    atparser_cmd_exit(spi_at);
}

static void systime_test(void)
{
    int systime, systime1;
    atparser_clr_buf(spi_at);

    if (atparser_send(spi_at, "AT+SYSTIME") == 0) {
        if (atparser_recv(spi_at, "+SYSTIME:%d.%d", &systime, &systime1) == 0) {
            printf("systime: %d.%d\r\n", systime, systime1);
        }
    }

    atparser_cmd_exit(spi_at);    
}

static void wjap_test(void)
{
    atparser_clr_buf(spi_at);

    if (atparser_send(spi_at, "AT+WJAP=%s,%s", "Alibaba-test-247264", "@asdfghjkl88") == 0) {
        if (atparser_recv(spi_at, "OK") == 0) {
            printf("wjap test ok\r\n");
        }
    }

    atparser_cmd_exit(spi_at);
}

int w800_ping(const char *ip, uint8_t seq_num)
{
    int ret = -1;
    int ping_time;
    
    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);

    if (atparser_send(spi_at, "AT+PING=%s", ip) == 0) {
        if ((atparser_recv(spi_at, "+PING:%d", &ping_time) == 0) \
            && (atparser_recv(spi_at, "OK\n") == 0)) {
            ret = 0;
        } else {
            printf("\t from %s: Destination Host Unreachable\r\n", ip);
        }
    }

    atparser_cmd_exit(spi_at);

    if (ret == 0) {
        printf("\t from %s: icmp_seq=%d time=%dms\r\n", ip, seq_num, ping_time);
    }

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_connect_remote(int id, net_conn_e type, char *srvname, uint16_t port)
{
    int ret = -1;
    int ret_id;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);

    switch (type) {
        case NET_TYPE_TCP_SERVER:
            /* TCP Server can NOT ignore lport */
            break;

        case NET_TYPE_UDP_UNICAST:
        case NET_TYPE_TCP_CLIENT:
            ret = atparser_send(spi_at, "AT+CIPSTART=%d,%s,%s,%d", id, type == NET_TYPE_TCP_CLIENT ? "tcp_client" : "udp_client", srvname, port);
            break;

        default:
            LOGE(TAG, "type=%d err!", type);
            return -1;

    }

    if (ret == 0) {
        ret = -1;

        if ((atparser_recv(spi_at, "%d,CONNECT\n", &ret_id) == 0) \
            && (atparser_recv(spi_at, "OK\n") == 0)) {
            if (ret_id == id) {
                ret = 0;
            }
        }
    }

    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_send_data(int id, const uint8_t *pdata, int len,int timeout)
{
    int ret = -1;
  

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);
    atparser_set_timeout(spi_at, timeout);
    
    atparser_send(spi_at, "AT+CIPSEND=%d,%s", len, pdata);

    atparser_set_timeout(spi_at,8*1000);
    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);
    return ret;;
}

#if 0
int w800_link_info(void)
{
    int ret = -1;
    char ip[20] = {0};

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);
    
    atparser_send(spi_at, "AT+WGIP");
    if ((atparser_recv(spi_at, "+WGIP:%s", ip) == 0) \
        && (atparser_recv(spi_at, "OK\n") == 0)) {
        ret = 0;
    } else {
        printf("fail to get ip\r\n");
    }

    LOGE(TAG, "ip %s", ip);

    atparser_set_timeout(spi_at,8*1000);
    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);
    return ret;;
}
#else
int w800_link_info(void)
{
    int ret = -1;
    char ip[30] = {0};
    char gw[30] = {0};
    char mask[30] = {0};

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);

    if (atparser_send(spi_at, "AT+CIPSTA") == 0) {
        if ((atparser_recv(spi_at, "+CIPSTA:ip:%s", ip) == 0) \
            && (atparser_recv(spi_at, "+CIPSTA:gateway:%s", gw) == 0) \
            && (atparser_recv(spi_at, "+CIPSTA:netmask:%s", mask) == 0) \
            && (atparser_recv(spi_at, "OK\n") == 0)) {
            ret = 0;
        }
    }

    LOGE(TAG, "\tinet addr:%s\n", ip);
    LOGE(TAG, "\tGWaddr:%s\n", gw);
    LOGE(TAG, "\tMask:%s\n", mask);

    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;

}
#endif

#if 0
int w800_get_mac(void)
{
    int ret = -1;
    char mac[30] = {0};

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);
    
    atparser_send(spi_at, "AT+CIPSTAMAC");
    if ((atparser_recv(spi_at, "+CIPSTAMAC:%s", mac) == 0) \
        && (atparser_recv(spi_at, "OK\n") == 0)) {
        ret = 0;
    } else {
        printf("fail to get mac\r\n");
    }

    LOGE(TAG, "mac %s", mac);

    atparser_set_timeout(spi_at,8*1000);
    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);
    return ret;;
}
#else
int w800_get_mac(void)
{
    int ret = -1;
    int mac[6];

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);

    if (atparser_send(spi_at, "AT+CIPSTAMAC") == 0) {
        if ((atparser_recv(spi_at, "+CIPSTAMAC:%x:%x:%x:%x:%x:%x\n", 
                &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 0)) {
            printf("mac: %02x:%02x:%02x:%02x:%02x:%02x\r\n", 
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            ret = 0;
        }
    }

    

    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}
#endif

int w800_domain_to_ip(char *domain, char ip[16])
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);
    atparser_set_timeout(spi_at,30*1000);

    if (atparser_send(spi_at, "AT+CIPDOMAIN=%s", domain) == 0) {
        if ((atparser_recv(spi_at, "+CIPDOMAIN:%s\n", ip) == 0) \
            && (atparser_recv(spi_at, "OK\n") == 0)) {
            ret = 0;
            LOGE(TAG, "domain to ip: %s", ip);
        } else {
            LOGE(TAG, "domain resp fail\n");
        }
    }

    atparser_set_timeout(spi_at,8*1000);
    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

static void w800_reset(void)
{
    atparser_clr_buf(spi_at);

    if (atparser_send(spi_at, "AT+RST") == 0) {
        if (atparser_recv(spi_at, "OK") == 0) {
            printf("rst ok\r\n");
        }
    }

    atparser_cmd_exit(spi_at);
}

int w800_get_status()
{
    int ret = -1;
    int stat = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(spi_at);
    atparser_set_timeout(spi_at,8*1000);

    if (atparser_send(spi_at, "AT+CIPGETSTATUS") == 0) {
        if ((atparser_recv(spi_at, "+CIPGETSTATUS:%d\n", &stat) == 0) \
            && (atparser_recv(spi_at, "OK\n") == 0)) {
            ret = 0;
            printf("link status %d\r\n", stat);
        }
    }

    atparser_cmd_exit(spi_at);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int _connect_net_event_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    printf("%s, %d, %s\r\n", __FUNCTION__, __LINE__, oob_data->buf);
    LOGE(TAG, "+connect oob %s", oob_data->buf);

    if (strcmp("LINK_UP", oob_data->buf) == 0) {
        g_net_status = NET_STATUS_LINKUP;
    } else if (strcmp("LINK_DOWN", oob_data->buf)) {
        g_net_status = NET_STATUS_LINKDOWN;
    }
    return 0;
}

int _connect_ready_event_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    LOGE(TAG, "W800 is ready");

    return 0;
}

#define TIME_OUT 180 //ms
typedef struct ipd {
    int linkid;
    int rlen;
    char remote_ip[16];
    uint16_t remote_port;
    char *data;
    size_t data_size;
} ipd_t;

static int ipd_put(ipd_t *ipd, char *data, int len)
{
    if(ipd->data == NULL) {
        ipd->data = aos_malloc(ipd->rlen);
        if(ipd->data == NULL) {
            LOGW(TAG, "IPD mem err\n");
        }
    }
    int copy_len;
    if ((ipd->data_size + len) > ipd->rlen) {
        copy_len = ipd->rlen - ipd->data_size;
    } else {
        copy_len = len;
    }
    memcpy(&ipd->data[ipd->data_size], data, copy_len);
    ipd->data_size += copy_len;

    return copy_len;
}

static void ipd_clear(ipd_t *ipd)
{
    free(ipd->data);
    memset(ipd, 0, sizeof(ipd_t));
}

int _connect_recv_event_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    //+IPD,0,512,106.15.213.199,5684:
    static ipd_t ipd;

    // oob timeout
    if (aos_now_ms() > (oob_data->time_stamp + TIME_OUT)) {
        ipd_clear(&ipd);
        return 0;
    }

    if (ipd.rlen == 0) { // first data
        char *str = strchr(oob_data->buf, ':');
        if (str  != NULL) {
            sscanf(oob_data->buf, "%d,%d,%[^,],%d:%*s", &ipd.linkid, &ipd.rlen, ipd.remote_ip, &ipd.remote_port);
            oob_data->used_len = ipd_put(&ipd, str+1, oob_data->len - (str - oob_data->buf+1));
            //LOGI(TAG,"%s(%d)",oob_data->buf,oob_data->used_len);
            oob_data->used_len += (str - oob_data->buf+1);
        }
    } else {
        oob_data->used_len = ipd_put(&ipd, oob_data->buf, oob_data->len);

        if (ipd.data_size == ipd.rlen ) {
            // if (g_net_data_input_cb) {
            //     g_net_data_input_cb(ipd.linkid, ipd.data, ipd.rlen, ipd.remote_ip, ipd.remote_port);
            // }
            ipd_clear(&ipd);
            return 0;
        }
    }
    /* recv data is not enough need recv more */
    return -1;

}

void at_tests_case(void)
{
    uint8_t ip[16] = {0};
    at_test();
    //systime_test();
    //wjap_test();
    w800_link_info();
    w800_get_mac();
    w800_domain_to_ip("www.baidu.com", ip);
    w800_get_status();
    //w800_reset();
    //w800_ping("192.168.1.1", 1);
    w800_connect_remote(0, NET_TYPE_TCP_CLIENT, "192.168.1.19", 8080);
    //w800_send_data(0, "helloworld", 10, 1000);
    //w800_recv_data();
}