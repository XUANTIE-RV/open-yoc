#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>


#include "usbd_core.h"
#include "usbd_video.h"

#include "media_video.h"

#define VIDEO_IN_EP 0x81

#ifdef CONFIG_USB_HS
// #define TRANSACTION_PER_MICROFRAME 2
// #define MAX_PAYLOAD_SIZE  1024 // for high speed with one transcations every one micro frame
// #define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))

// #define MAX_PAYLOAD_SIZE  2048 // for high speed with two transcations every one micro frame
// #define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 2)) | (0x01 << 11))

#define MAX_PAYLOAD_SIZE  3072 // for high speed with three transcations every one micro frame
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 3)) | (0x02 << 11))

#else
#define MAX_PAYLOAD_SIZE  1020
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))
#endif

#define WIDTH  (unsigned int)(1920)
#define HEIGHT (unsigned int)(1080)

#define CAM_FPS        (30)
#define INTERVAL       (unsigned long)(10000000 / CAM_FPS)
#define MIN_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS) //16 bit
#define MAX_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS)
#define MAX_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 2)
#define DEFAULT_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 3 / 2)


#define VC_TERMINAL_SIZ (unsigned int)(13 + 18 + 12 + 9)
#define VS_HEADER_SIZ   (unsigned int)(13 + 1 + 11 + 30)
#define VS_H264_HEADER_SIZ  (unsigned int)(13 + 1 + 28 + 30)
#define VS_H264_MJPEG_HEADER_SIZ  (unsigned int)(13 + 2 + 11 + 30 + 28 + 30)

#define USB_VIDEO_DESC_SIZ (unsigned long)(9 +  \
                                           8 +  \
                                           9 +  \
                                           13 + \
                                           18 + \
                                           9 +  \
                                           12 + \
                                           9 +  \
                                           VS_H264_MJPEG_HEADER_SIZ + \
                                           9 +  \
                                           7)

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

const uint8_t video_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    VIDEO_VC_DESCRIPTOR_INIT(0x00, 0, 0x0100, VC_TERMINAL_SIZ, 48000000, 0x02),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
    #if 0
    VIDEO_VS_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),
    VIDEO_VS_FORMAT_MJPEG_DESCRIPTOR_INIT(0x01, 0x01),
    VIDEO_VS_FRAME_MJPEG_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, MAX_FRAME_SIZE, DBVAL(INTERVAL), 0x01, DBVAL(INTERVAL)),
    #elif 0
    VIDEO_VS_HEADER_DESCRIPTOR_INIT(0x01, VS_H264_HEADER_SIZ, VIDEO_IN_EP, 0x00),
    VIDEO_VS_FORMAT_H264_DESCRIPTOR_INIT(0x01, 0x01),
    VIDEO_VS_FRAME_H264_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, DBVAL(INTERVAL), 0x01),
    #else
    VIDEO_VS_HEADER_DESCRIPTOR_INIT(0x02, VS_H264_MJPEG_HEADER_SIZ, VIDEO_IN_EP, 0x00, 0x00),
    VIDEO_VS_FORMAT_MJPEG_DESCRIPTOR_INIT(0x01, 0x01),
    VIDEO_VS_FRAME_MJPEG_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, MAX_FRAME_SIZE, DBVAL(INTERVAL), 0x01, DBVAL(INTERVAL)),
    VIDEO_VS_FORMAT_H264_DESCRIPTOR_INIT(0x02, 0x01),
    VIDEO_VS_FRAME_H264_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, DBVAL(INTERVAL), 0x01),
    #endif
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x01, 0x01),
    /* 1.2.2.2 Standard VideoStream Isochronous Video Data Endpoint Descriptor */
    0x07,                         /* bLength */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: ENDPOINT */
    0x81,                         /* bEndpointAddress: IN endpoint 2 */
    0x01,                         /* bmAttributes: Isochronous transfer type. Asynchronous synchronization type. */
    WBVAL(VIDEO_PACKET_SIZE),     /* wMaxPacketSize */
    0x01,                         /* bInterval: One frame interval */

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
    'V', 0x00,                  /* wcChar11 */
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
    '0', 0x00,                  /* wcChar9 */
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

// void usbd_configure_done_callback(void)
// {
//     /* no out ep, so do nothing */
// }

volatile bool tx_flag = 0;
volatile bool iso_tx_busy = false;
static int uvc_session_init_flag = 0;

void uvc_streaming_on(int is_on) {
    USB_LOG_RAW("streaming %s\n", is_on ? "on" : "off");
    tx_flag = is_on;
    iso_tx_busy = false;
}

void usbd_configure_done_callback(void)
{

}

void uvc_setup_class_control(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    printf("%s:%d\n", __FUNCTION__, __LINE__);
}

void uvc_setup_class_streaming(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    printf("%s:%d\n", __FUNCTION__, __LINE__);
}

void uvc_data_out(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    printf("%s:%d\n", __FUNCTION__, __LINE__);
}

void uvc_tx_complete(uint8_t ep, uint32_t nbytes)
{
    // printf("%d bytes of data sent at ep(%d)\n", nbytes, ep);
    iso_tx_busy = false;
}

void usbd_video_commit_set_cur(struct video_probe_and_commit_controls *commit)
{
    printf("commit format idx:%d, frame idx:%d\n", commit->bFormatIndex, commit->bFrameIndex);
}

static uvc_event_callbacks_t uvc_evt_callbks = {
    .uvc_event_stream_on = uvc_streaming_on,
    .uvc_event_setup_class_control = uvc_setup_class_control,
    .uvc_event_setup_class_streaming = uvc_setup_class_streaming,
    .uvc_event_data_out = uvc_data_out,
    .uvc_event_tx_complete = uvc_tx_complete,
};

static void * media_video_mjp()
{
    uint32_t out_len,i = 0,ret = 0;
	
	uint32_t buf_len = 0;
    uint32_t packets = 0;
	uint8_t *packet_buffer_mjp = (uint8_t *)usb_malloc(DEFAULT_FRAME_SIZE);
	uint8_t *packet_buffer_uvc = (uint8_t *)usb_malloc(DEFAULT_FRAME_SIZE);
    memset(packet_buffer_mjp, 0, DEFAULT_FRAME_SIZE);
    memset(packet_buffer_uvc, 0, DEFAULT_FRAME_SIZE);
	extern volatile bool tx_flag;
	extern volatile bool iso_tx_busy;
    VENC_STREAM_S stStream = {0},*pstStream= &stStream;
	
	VENC_PACK_S *ppack;


    while (uvc_session_init_flag) {
        if (tx_flag) {

	        if(MEDIA_VIDEO_VencGetStream(0,pstStream,2000) == 0) {
				
				for (i = 0; i < pstStream->u32PackCount; ++i)
				{
					if(buf_len < DEFAULT_FRAME_SIZE){
						ppack = &pstStream->pstPack[i];
						memcpy(packet_buffer_mjp + buf_len, ppack->pu8Addr + ppack->u32Offset, ppack->u32Len - ppack->u32Offset);
						buf_len += (ppack->u32Len - ppack->u32Offset);
					}
					else{
							printf("venc buf_len oversize\n");
							MEDIA_VIDEO_VencReleaseStream(0,pstStream);
							continue;
					}
				}
				ret = MEDIA_VIDEO_VencReleaseStream(0,pstStream);
				if(ret !=0)
					printf("MEDIA_VIDEO_VencReleaseStream failed\n");
				
				packets = usbd_video_mjpeg_payload_fill(packet_buffer_mjp, buf_len, packet_buffer_uvc, &out_len);
				buf_len = 0;
				
	            
		        } else {
		        	//printf("MEDIA_VIDEO_VencGetStream failed\n");
					aos_msleep(1);
					continue;
	        }

            /* dwc2 must use this method */
            for (i = 0; i < packets; i++) {
                if (i == (packets - 1)) {
                    iso_tx_busy = true;
					usbd_ep_start_write(VIDEO_IN_EP, &packet_buffer_uvc[i * MAX_PAYLOAD_SIZE], out_len - i * MAX_PAYLOAD_SIZE);
                    while (iso_tx_busy) {
                        if (tx_flag == 0) {
                            break;
                        }
                    }
                } else {
                    iso_tx_busy = true;
                    usbd_ep_start_write(VIDEO_IN_EP, &packet_buffer_uvc[i * MAX_PAYLOAD_SIZE], MAX_PAYLOAD_SIZE);
                    while (iso_tx_busy) {
                        if (tx_flag == 0) {
                            break;
                        }
                    }
                }
            }
        }else 
        	aos_msleep(1);
    }
	
    return 0;
}


static struct usbd_endpoint video_in_ep;

static struct usbd_interface intf0;
static struct usbd_interface intf1;

void video_init()
{
    usbd_desc_register(video_descriptor);
    usbd_add_interface(usbd_video_init_intf(&intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));

    usbd_add_endpoint(usbd_video_init_ep(&video_in_ep, VIDEO_IN_EP, NULL));

    usbd_video_register_uvc_callbacks(&uvc_evt_callbks);

    usbd_initialize();
}

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t packet_buffer[200 * 1024];

int MEDIA_UVC_Init()
{
    char threadname[64] = {0};
	struct sched_param param;
	pthread_attr_t pthread_attr;
	pthread_t pthreadId = 0;

	video_init();

	// Wait until configured
	while (!usb_device_is_configured()) {
		aos_msleep(100);
	}
	
	uvc_session_init_flag = 1;

	param.sched_priority = 31;
	pthread_attr_init(&pthread_attr);
	pthread_attr_setschedpolicy(&pthread_attr, SCHED_RR);
	pthread_attr_setschedparam(&pthread_attr, &param);
	pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_create(&pthreadId,&pthread_attr,media_video_mjp,NULL);
	snprintf(threadname,sizeof(threadname),"uvc_send%d",0);
	pthread_setname_np(pthreadId, threadname);
	
	return 0;
}

int MEDIA_UVC_DeInit()
{
	uvc_session_init_flag = 0;
	aos_msleep(100);
	usbd_deinitialize();

	return 0;
}

void test_uvc_init()
{
	MEDIA_UVC_Init();
}

void test_uvc_Deinit()
{
	MEDIA_UVC_DeInit();
}

ALIOS_CLI_CMD_REGISTER(test_uvc_init, test_uvc_init, test_uvc_init);
ALIOS_CLI_CMD_REGISTER(test_uvc_Deinit, test_uvc_Deinit, test_uvc_Deinit);
