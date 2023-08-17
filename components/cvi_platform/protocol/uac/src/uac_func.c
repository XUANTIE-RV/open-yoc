#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <pthread.h>
#include "usbd_core.h"
#include "usbd_audio.h"
#include "cvi_type.h"
#include "ringfifo.h" 
#include "audio.h"

#ifdef DUMP_PCM_RAW_DATA
#include "fatfs_vfs.h"
#include "vfs.h"
#endif 

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

#define AUDIO_IN_EP  0x81
#define AUDIO_OUT_EP 0x02

/* AUDIO Class Config */
#define AUDIO_FREQ 16000U

#define AUDIO_SAMPLE_FREQ(frq) (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define AUDIO_OUT_PACKET ((uint32_t)((AUDIO_FREQ * 2 * 2) / 1000))
/* 16bit(2 Bytes) 双声道(Mono:2) */
#define AUDIO_IN_PACKET ((uint32_t)((AUDIO_FREQ * 2 * 2) / 1000))

#define USB_AUDIO_CONFIG_DESC_SIZ (unsigned long)(9 +                                       \
                                                  AUDIO_AC_DESCRIPTOR_INIT_LEN(2) +         \
                                                  AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                                                  AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                                                  AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +    \
                                                  AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                                                  AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                                                  AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +    \
                                                  AUDIO_AS_DESCRIPTOR_INIT_LEN(1) +         \
                                                  AUDIO_AS_DESCRIPTOR_INIT_LEN(1))

#define AUDIO_AC_SIZ (AUDIO_SIZEOF_AC_HEADER_DESC(2) +          \
                      AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                      AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                      AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC +    \
                      AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +     \
                      AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(2, 1) + \
                      AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

const uint8_t audio_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x03, AUDIO_AC_SIZ, 0x00, 0x01, 0x02),
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, 0x02, 0x0003),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x02, 0x01, 0x01, 0x03, 0x00),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, 0x02),
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_TERMINAL_STREAMING, 0x02, 0x0003),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(0x05, 0x04, 0x01, 0x03, 0x00),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_OUTTERM_SPEAKER, 0x05),
    AUDIO_AS_DESCRIPTOR_INIT(0x01, 0x04, 0x02, AUDIO_OUT_EP, AUDIO_OUT_PACKET, EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(AUDIO_FREQ)),
    AUDIO_AS_DESCRIPTOR_INIT(0x02, 0x03, 0x02, AUDIO_IN_EP, AUDIO_IN_PACKET, EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(AUDIO_FREQ)),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'U', 0x00,                  /* wcChar10 */
    'A', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '1', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

#define UAC_QUEUE_MIC_INDEX     0
#define UAC_QUEUE_SPEAKER_INDEX   1

static volatile bool tx_flag = 0;
static volatile bool rx_flag = 0;
static volatile bool ep_tx_busy_flag = false;
static volatile bool ep_rx_busy_flag = false;
static int uac_session_init_flag = CVI_FALSE;

//#define UAC_RINGFIFO_SIZE  (PERIOD_FRAMES_SIZE * 4 * 40) //default 50K
#define UAC_RINGFIFO_SIZE  65536
//static pthread_mutex_t play_count_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t *f_lock[2];
struct ring_buffer* g_ring_buf[2];
void * g_buffer[2];

struct usbd_interface intf0;
struct usbd_interface intf1;
struct usbd_interface intf2;

void usbd_audio_open(uint8_t intf)
{
    if (intf == 1) {
        rx_flag = 1;
        ep_rx_busy_flag = false;
        aos_debug_printf("OPEN1\r\n");
    } else {
        tx_flag = 1;
        ep_tx_busy_flag = false;
        aos_debug_printf("OPEN2\r\n");
    }
}
void usbd_audio_close(uint8_t intf)
{
    if (intf == 1) {
        rx_flag = 0;
        ep_rx_busy_flag = false;
        aos_debug_printf("CLOSE1\r\n");
    } else {
        tx_flag = 0;
        ep_tx_busy_flag = false;
        aos_debug_printf("CLOSE2\r\n");
    }
}

#ifdef CONFIG_USB_HS
#define AUDIO_OUT_EP_MPS 512
#else
#define AUDIO_OUT_EP_MPS 64
#endif

//USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t out_buffer[AUDIO_OUT_PACKET];


// static void dump_mem(const uint8_t *addr, uint32_t len) {

//     for (uint32_t i = 0; i < len; i++) {
//         aos_debug_printf("%02x ", addr[i]);
//         if (i != 0 && (i & 0x0f) == 0) {
//             aos_debug_printf("\n");
//         }
//     }
//     aos_debug_printf("\n");
// }

// void usbd_configure_done_callback(void)
// {
//     /* setup first out ep read transfer */
//     usbd_ep_start_read(AUDIO_OUT_EP, out_buffer, AUDIO_OUT_PACKET);
// }

void usbd_audio_out_callback(uint8_t ep, uint32_t nbytes)
{
    //aos_debug_printf("actual out len:%d\r\n", nbytes);

    // notify that reading is done
    ep_rx_busy_flag = false;
    // dump_mem(out_buffer, AUDIO_OUT_PACKET);
}

void usbd_audio_in_callback(uint8_t ep, uint32_t nbytes)
{
    //aos_debug_printf("actual in len:%d\r\n", nbytes);
    ep_tx_busy_flag = false;
}

static struct usbd_endpoint audio_in_ep = {
    .ep_cb = usbd_audio_in_callback,
    .ep_addr = AUDIO_IN_EP
};

static struct usbd_endpoint audio_out_ep = {
    .ep_cb = usbd_audio_out_callback,
    .ep_addr = AUDIO_OUT_EP
};

void uac_audio_init()
{
    usbd_desc_register(audio_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&intf0));
    usbd_add_interface(usbd_audio_init_intf(&intf1));
    usbd_add_interface(usbd_audio_init_intf(&intf2));
    usbd_add_endpoint(&audio_in_ep);
    usbd_add_endpoint(&audio_out_ep);

    usbd_audio_add_entity(0x02, AUDIO_CONTROL_FEATURE_UNIT);
    usbd_audio_add_entity(0x05, AUDIO_CONTROL_FEATURE_UNIT);

    usbd_initialize();
}



int uac_ringfifo_init(void) 
{
    for (int i = 0; i<2; i++) {
        f_lock[i] = (pthread_mutex_t *)aos_malloc(sizeof(pthread_mutex_t));
        if (pthread_mutex_init(f_lock[i], NULL) != 0) {
            aos_debug_printf("Failed init mutex\r\n");
            return -1;
        }
        g_buffer[i] = (void *)aos_malloc(UAC_RINGFIFO_SIZE);
        if (!g_buffer[i]) {
            aos_debug_printf("Failed to malloc memory.\r\n");
            return -1;
        }

        g_ring_buf[i] = ring_buffer_init(g_buffer[i], UAC_RINGFIFO_SIZE, f_lock[i]);
        if (!g_ring_buf[i]) {
            aos_debug_printf("Failed to init ring buffer\r\n");
            return -1;
        }
    }
    aos_debug_printf("uac_ringfifo_init init success\r\n");
    return 0;
}

int uac_ringfifo_deinit(void)
{
    for (int i = 0; i<2; i++) {
        ring_buffer_free(g_ring_buf[i]);
    }
    return 0;
}

/***************************************************************/
#if 0
static int uac_send_pcm_data2host(unsigned char *data,int len)
{
    int pack_len = AUDIO_IN_PACKET;
    if (tx_flag) {
        if (!ep_tx_busy_flag) {
            //aos_debug_printf("ca = %d\r\n",len/AUDIO_IN_PACKET);
            for(int x = 0; x < len/pack_len; x++) {
                ep_tx_busy_flag = true;
                usbd_ep_start_write(AUDIO_IN_EP, data+(x*pack_len), pack_len);
                while(ep_tx_busy_flag) {
                    //aos_msleep(1);
                }
            }
        }
    }
    return 0;
}

static int uac_get_host_pcmdata(unsigned char* data,int get_len)
{
    int count = 0;
    int pack_len = AUDIO_OUT_PACKET;

    if (rx_flag) {
        while(1) {
            if (!ep_rx_busy_flag) {
                // aos_debug_printf("start reading\n");
                ep_rx_busy_flag = true;
                //memset(out_buffer,0,AUDIO_OUT_PACKET);
                usbd_ep_start_read(AUDIO_OUT_EP, out_buffer, pack_len);
                memcpy(data+(count*pack_len),out_buffer,AUDIO_OUT_PACKET);
                if (count*pack_len >= get_len) {
                    //dump_mem(pstdata->buf,AUDIO_OUT_PACKET);
                    //aos_debug_printf("get pc pcm_data len %d\r\n",pstdata->len);
                    return 0;
                }
                count++;
            }
        }
    }
    return -1;
}
#endif

//USB 写线程
static void uac_send_to_host(void *arg)
{
    int ret = 0;
    unsigned char mic_index = UAC_QUEUE_MIC_INDEX;
    int len = audio_get_pcm_len(mic_index);
    int pack_len = AUDIO_IN_PACKET;
    unsigned char send_state = 0;

	unsigned char* buf = aos_malloc(CAPTURE_SIZE);
    if (buf == NULL) {
        aos_debug_printf("aos_malloc buf fail\n");
        return ;
    }

    aos_debug_printf("start uac_send_to_host thread\r\n");
    while (uac_session_init_flag) {
        //board mic --> PC
        if(tx_flag) {
            //aos_debug_printf("get ret %d\r\n",ret);
            ret = ring_buffer_get(g_ring_buf[mic_index], (void *)buf, len);
            if (ret == 0) {
                aos_msleep(1);
                continue;
            }

            if (!ep_tx_busy_flag) {
                //aos_debug_printf("ca = %d\r\n",len/AUDIO_IN_PACKET);
                for (int x = 0; x < len/pack_len; x++) {
                    ep_tx_busy_flag = true;
                    usbd_ep_start_write(AUDIO_IN_EP, buf+(x*pack_len), pack_len);
                    while (ep_tx_busy_flag) {
                        aos_msleep(1);  //temp,shoule use sem
                    }
                    if (!tx_flag)
                        break;
                }
            }

            if(!send_state)
                send_state = 1;
        } else {    //close
            if (send_state) {
                ring_buffer_reset(g_ring_buf[mic_index]);
                aos_debug_printf("ring_buffer_reset index[%d]\r\n",mic_index);
                send_state = 0;
            }
            aos_msleep(10);
        }

        // if ((!tx_flag) || (tx_flag && ep_tx_busy_flag)) {
        //     aos_msleep(1);
        // }
    }
    aos_free(buf);
    aos_debug_printf("exit uac_send_to_host thread\r\n");
    return ;
}

//USB 读线程
static void uac_recv_from_host(void *arg)
{
    unsigned char play_index = UAC_QUEUE_SPEAKER_INDEX;
    //int play_len = audio_get_pcm_len(play_index);

    unsigned char* buf = aos_malloc(CAPTURE_SIZE);
    if (buf == NULL) {
        aos_debug_printf("aos_malloc buf fail\n");
        return ;
    }

    int pack_len = AUDIO_OUT_PACKET;
    aos_debug_printf("start uac_recv_from_host thread\r\n");
    while (uac_session_init_flag) {
        //recv PC --> board speaker
        if (rx_flag) {
            if (!ep_rx_busy_flag) {
                // aos_debug_printf("start reading\n");
                ep_rx_busy_flag = true;
                usbd_ep_start_read(AUDIO_OUT_EP, out_buffer, pack_len);
                while (ep_rx_busy_flag) {
                    //aos_msleep(1);    //temp,shoule use sem
                }
                ring_buffer_put(g_ring_buf[play_index], (void *)out_buffer, pack_len);
            }
        } else {
            aos_msleep(10);
        }
    }
    aos_free(buf);
    aos_debug_printf("exit uac_recv_from_host thread\r\n");
    return ;
}

static void audio_read(void *arg)
{
    int ret = 0;
    unsigned char index = UAC_QUEUE_MIC_INDEX;

	unsigned char *buf = aos_malloc(CAPTURE_SIZE);
    if (buf == NULL) {
        aos_debug_printf("aos_malloc buf fail\n");
        return ;
    }
    aos_debug_printf("start audio_read thread\r\n");
    while (uac_session_init_flag) {
        if(tx_flag) {
            ret = audio_pcm_read(buf);
            if (ret > 0) {
                ring_buffer_put(g_ring_buf[index], (void *)buf, ret);
                // if (ring_buffer_len(g_ring_buf[index]) == UAC_RINGFIFO_SIZE) {
                //     aos_debug_printf("index[%d] is full\r\n",index);
                // }
            }
        }
        aos_msleep(10);
    }
    aos_free(buf);
    aos_debug_printf("exit audio_read thread\r\n");
}

//speaker thread
static void audio_write(void *arg)
{
    unsigned char index = UAC_QUEUE_SPEAKER_INDEX;
    int ret = 0;
    unsigned char player_state = 0;
	unsigned char *buf = aos_malloc(CAPTURE_SIZE);
    if (buf == NULL) {
        aos_debug_printf("aos_malloc buf fail\r\n");
        return ;
    }
    int play_len = audio_get_pcm_len(index);
    aos_debug_printf("start audio_write thread\r\n");

    while (uac_session_init_flag) {
        if (rx_flag) {
            ret = ring_buffer_get(g_ring_buf[index], (void *)buf, play_len);
            //aos_debug_printf("get ret %d\r\n",ret);
            if (ret == 0) {
                //aos_debug_printf("uac_dequeue speaker fail\r\n");
                aos_msleep(1);
                continue;            
            }
            audio_pcm_write(buf,ret);

            if(!player_state)
                player_state = 1;
            //aos_msleep(1);
        } else {    //close
            if (player_state) {
                ring_buffer_reset(g_ring_buf[index]);
                aos_debug_printf("ring_buffer_reset index[%d]\r\n",index);
                player_state = 0;
            }
            aos_msleep(10);
        }
    }
    aos_free(buf);
    aos_debug_printf("exit audio_write thread\r\n");

    return ;
}


int MEDIA_UAC_Init()
{
    aos_task_t read_handle,write_handle,us_handle,ur_handle;

    uac_audio_init();

	//Wait until configured
	while (!usb_device_is_configured()) {
		aos_msleep(100);
	}
    media_audio_init();
    uac_ringfifo_init();

	uac_session_init_flag = CVI_TRUE;


    if(0 != aos_task_new_ext(&read_handle,"audio_read"
                    ,audio_read,NULL,4*1024,30)) {
        aos_debug_printf("create audio_read thread fail\r\n");
        return -1;
    }

    if(0 != aos_task_new_ext(&us_handle,"uac_send_to_host"
            ,uac_send_to_host,NULL,4*1024,30)) {
        aos_debug_printf("create uac_send_to_host thread fail\r\n");
        return -1;
    }
    //return 0;
    if(0 != aos_task_new_ext(&write_handle,"audio_write"
                    ,audio_write,NULL,4*1024,28)) {
        aos_debug_printf("create audio_write thread fail\r\n");
        return -1;
    }

    if(0 != aos_task_new_ext(&ur_handle,"uac_recv_from_host"
            ,uac_recv_from_host,NULL,4*1024,34)) {
        aos_debug_printf("create uac_recv_from_host thread fail\r\n");
        return -1;
    }
	return 0;
}

int MEDIA_UAC_deInit()
{
	media_audio_deinit();
    uac_ringfifo_deinit();
    return 0;
}

void test_uac_init()
{
	MEDIA_UAC_Init();
}


void test_uac_deinit()
{
    MEDIA_UAC_deInit();
}

ALIOS_CLI_CMD_REGISTER(test_uac_init, test_uac_init, test_uac_init);
ALIOS_CLI_CMD_REGISTER(test_uac_deinit, test_uac_deinit, test_uac_deinit);
