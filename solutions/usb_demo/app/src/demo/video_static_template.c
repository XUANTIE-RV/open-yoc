#include "usbd_core.h"
#include "usbd_video.h"
#include "pic_data.h"
#include "h264_data.h"

#define VIDEO_IN_EP 0x81

#ifdef CONFIG_USB_HS
#define MAX_PAYLOAD_SIZE  1024 // for high speed with one transcations every one micro frame
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))
#else
#define MAX_PAYLOAD_SIZE  1020
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))
#endif

#define WIDTH  (unsigned int)(640)
#define HEIGHT (unsigned int)(480)

#define CAM_FPS        (30)
#define INTERVAL       (unsigned long)(10000000 / CAM_FPS)
#define MIN_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS) // 16 bit
#define MAX_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS)
#define MAX_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 2)

#define VC_TERMINAL_SIZ (unsigned int)(13 + 18 + 12 + 9)
#if defined(CONFIG_VUC_H264) && CONFIG_VUC_H264
#define USB_VIDEO_DESC_SIZ (unsigned long)(9 + 8 + 9 + 13 + 18 + 9 + 12 + 9 + 14 + 28 + 30 + 9 + 7)
#define VS_HEADER_SIZ      (unsigned int)(13 + 1 + 28 + 30)
#else
#define VS_HEADER_SIZ      (unsigned int)(13 + 1 + 11 + 30)
#define USB_VIDEO_DESC_SIZ (unsigned long)(9 + 8 + 9 + 13 + 18 + 9 + 12 + 9 + 14 + 11 + 30 + 9 + 7)
#endif

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

const uint8_t video_descriptor[]
    = { USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
        USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
        VIDEO_VC_DESCRIPTOR_INIT(0x00, 0, 0x0100, VC_TERMINAL_SIZ, 48000000, 0x02),
        VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
        VIDEO_VS_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),
#if defined(CONFIG_VUC_H264) && CONFIG_VUC_H264
        VIDEO_VS_FORMAT_H264_DESCRIPTOR_INIT(0x01, 0x01),
        VIDEO_VS_FRAME_H264_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, DBVAL(INTERVAL), 0x01),
#else
        VIDEO_VS_FORMAT_MJPEG_DESCRIPTOR_INIT(0x01, 0x01),
        VIDEO_VS_FRAME_MJPEG_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, MAX_FRAME_SIZE,
                                             DBVAL(INTERVAL), 0x01, DBVAL(INTERVAL)),
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
        'C',
        0x00, /* wcChar0 */
        'h',
        0x00, /* wcChar1 */
        'e',
        0x00, /* wcChar2 */
        'r',
        0x00, /* wcChar3 */
        'r',
        0x00, /* wcChar4 */
        'y',
        0x00, /* wcChar5 */
        'U',
        0x00, /* wcChar6 */
        'S',
        0x00, /* wcChar7 */
        'B',
        0x00, /* wcChar8 */
        ///////////////////////////////////////
        /// string2 descriptor
        ///////////////////////////////////////
        0x26,                       /* bLength */
        USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
        'C',
        0x00, /* wcChar0 */
        'h',
        0x00, /* wcChar1 */
        'e',
        0x00, /* wcChar2 */
        'r',
        0x00, /* wcChar3 */
        'r',
        0x00, /* wcChar4 */
        'y',
        0x00, /* wcChar5 */
        'U',
        0x00, /* wcChar6 */
        'S',
        0x00, /* wcChar7 */
        'B',
        0x00, /* wcChar8 */
        ' ',
        0x00, /* wcChar9 */
        'U',
        0x00, /* wcChar10 */
        'V',
        0x00, /* wcChar11 */
        'C',
        0x00, /* wcChar12 */
        ' ',
        0x00, /* wcChar13 */
        'D',
        0x00, /* wcChar14 */
        'E',
        0x00, /* wcChar15 */
        'M',
        0x00, /* wcChar16 */
        'O',
        0x00, /* wcChar17 */
        ///////////////////////////////////////
        /// string3 descriptor
        ///////////////////////////////////////
        0x16,                       /* bLength */
        USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
        '2',
        0x00, /* wcChar0 */
        '0',
        0x00, /* wcChar1 */
        '2',
        0x00, /* wcChar2 */
        '1',
        0x00, /* wcChar3 */
        '0',
        0x00, /* wcChar4 */
        '3',
        0x00, /* wcChar5 */
        '1',
        0x00, /* wcChar6 */
        '0',
        0x00, /* wcChar7 */
        '0',
        0x00, /* wcChar8 */
        '0',
        0x00, /* wcChar9 */
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
        0x00 };

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static volatile bool         tx_flag     = 0;
static volatile bool         iso_tx_busy = false;

static void uvc_streaming_on(int is_on)
{
    USB_LOG_RAW("streaming %s\n", is_on ? "on" : "off");
    tx_flag = is_on;
}

static void uvc_setup_class_control(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USB_LOG_RAW("%s:%d\n", __FUNCTION__, __LINE__);
}

static void uvc_setup_class_streaming(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USB_LOG_RAW("%s:%d\n", __FUNCTION__, __LINE__);
}

static void uvc_data_out(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USB_LOG_RAW("%s:%d\n", __FUNCTION__, __LINE__);
}

static void uvc_tx_complete(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("%d bytes of data sent at ep(%d)\n", nbytes, ep);
}

static void usbd_video_iso_callback(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    iso_tx_busy = false;
}

static struct usbd_endpoint  video_in_ep     = { .ep_cb = usbd_video_iso_callback, .ep_addr = VIDEO_IN_EP };
static uvc_event_callbacks_t uvc_evt_callbks = {
    .uvc_event_stream_on             = uvc_streaming_on,
    .uvc_event_setup_class_control   = uvc_setup_class_control,
    .uvc_event_setup_class_streaming = uvc_setup_class_streaming,
    .uvc_event_data_out              = uvc_data_out,
    .uvc_event_tx_complete           = uvc_tx_complete,
};

void video_init()
{
    usbd_desc_register(video_descriptor);
    usbd_add_interface(usbd_video_init_intf(&intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_endpoint(&video_in_ep);
    usbd_video_register_uvc_callbacks(&uvc_evt_callbks);

    usbd_initialize();
}

void usbd_video_commit_set_cur(struct video_probe_and_commit_controls *commit)
{
    // uvc_parse_media_info(commit->bFormatIndex, commit->bFrameIndex);
    USB_LOG_RAW("commit format idx:%d, frame idx:%d\n", commit->bFormatIndex, commit->bFrameIndex);
}

uint8_t packet_buffer[100 * 1024];

void video_test()
{
    uint32_t out_len = sizeof(packet_buffer);
    uint32_t packets;
    memset(packet_buffer, 0, out_len);
    while (1) {
        if (tx_flag) {
#if defined(CONFIG_VUC_H264) && CONFIG_VUC_H264
            packets = usbd_video_payload_fill((uint8_t *)h264_data, 0xddec, packet_buffer, &out_len);
#else
            packets = usbd_video_payload_fill((uint8_t *)jpeg_data, sizeof(jpeg_data), packet_buffer, &out_len);
#endif

            /* dwc2 must use this method */
            for (uint32_t i = 0; i < packets; i++) {
                if (i == (packets - 1)) {
                    iso_tx_busy = true;
                    usbd_ep_start_write(
                        VIDEO_IN_EP, &packet_buffer[i * MAX_PAYLOAD_SIZE], out_len - (packets - 1) * MAX_PAYLOAD_SIZE);
                    while (iso_tx_busy) {
                        if (tx_flag == 0) {
                            break;
                        }
                    }
                } else {
                    iso_tx_busy = true;
                    usbd_ep_start_write(VIDEO_IN_EP, &packet_buffer[i * MAX_PAYLOAD_SIZE], MAX_PAYLOAD_SIZE);
                    while (iso_tx_busy) {
                        if (tx_flag == 0) {
                            break;
                        }
                    }
                }
            }
        }
    }
}