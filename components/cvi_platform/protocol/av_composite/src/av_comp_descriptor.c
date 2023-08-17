#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "av_comp_descriptor.h"
#include "uac_descriptor.h"
#include "io.h"

/* --------------------------------------------------------------------------
 * Device descriptor
 */

#define CVITEK_VENDOR_ID        0x3346    /* cvitek vendor id */
#define CVITEK_PRODUCT_ID       0x0001    /* Webcam A/V gadget */
#define CVITEK_DEVICE_BCD       0x0001    /* 0.01 */
#define VIDEO_IN_EP             0x81

#define USBD_LANGID_STRING 1033

static struct usb_device_descriptor uvc_device_descriptor = {
    .bLength            = USB_DT_DEVICE_SIZE,
    .bDescriptorType    = USB_DT_DEVICE,
    .bcdUSB             = USB_2_0,
    .bDeviceClass       = USB_CLASS_MISC,
    .bDeviceSubClass    = 0x02,
    .bDeviceProtocol    = 0x01,
    .bMaxPacketSize0    = 0x40,
    .idVendor           = cpu_to_le16(CVITEK_VENDOR_ID),
    .idProduct          = cpu_to_le16(CVITEK_PRODUCT_ID),
    .bcdDevice          = cpu_to_le16(CVITEK_DEVICE_BCD),
    .iManufacturer      = USB_STRING_MFC_INDEX,
    .iProduct           = USB_STRING_PRODUCT_INDEX,
    .iSerialNumber      = USB_STRING_SERIAL_INDEX,
    .bNumConfigurations = 1,
};

struct usb_config_descriptor uvc_config_descriptor = {
    .bLength                = USB_DT_CONFIG_SIZE,
    .bDescriptorType        = USB_DT_CONFIG,
    .wTotalLength           = 0, /* dynamic */
    .bNumInterfaces         = 5,
    .bConfigurationValue    = 1,
    .iConfiguration         = 0,
    .bmAttributes           = USB_CONFIG_ATT_ONE,
    .bMaxPower              = 50,
};

DECLARE_UVC_HEADER_DESCRIPTOR(1);

static const struct UVC_HEADER_DESCRIPTOR(1) uvc_control_header = {
    .bLength        = UVC_DT_HEADER_SIZE(1),
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VC_HEADER,
    .bcdUVC            = cpu_to_le16(0x0100),
    .wTotalLength        = 0, /* dynamic */
    .dwClockFrequency    = cpu_to_le32(48000000),
    .bInCollection        = 0, /* dynamic */
    .baInterfaceNr[0]    = 0, /* dynamic */
};

static const struct uvc_camera_terminal_descriptor uvc_camera_terminal = {
    .bLength        = UVC_DT_CAMERA_TERMINAL_SIZE(3),
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VC_INPUT_TERMINAL,
    .bTerminalID        = 1,
    .wTerminalType        = cpu_to_le16(0x0201),
    .bAssocTerminal        = 0,
    .iTerminal        = 0,
    .wObjectiveFocalLengthMin    = cpu_to_le16(0),
    .wObjectiveFocalLengthMax    = cpu_to_le16(0),
    .wOcularFocalLength        = cpu_to_le16(0),
    .bControlSize        = 3,
    .bmControls[0]        = 0,
    .bmControls[1]        = 0,
    .bmControls[2]        = 0,
};

static const struct uvc_processing_unit_descriptor uvc_processing = {
    .bLength        = UVC_DT_PROCESSING_UNIT_SIZE(2),
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VC_PROCESSING_UNIT,
    .bUnitID        = 2,
    .bSourceID        = 1,
    .wMaxMultiplier        = cpu_to_le16(0),
    .bControlSize        = 2,
    .bmControls[0]        = 0,
    .bmControls[1]        = 0,
    .iProcessing        = 0,
    .bmVideoStandards   = 0,
};

static const struct uvc_output_terminal_descriptor uvc_output_terminal = {
    .bLength        = UVC_DT_OUTPUT_TERMINAL_SIZE,
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VC_OUTPUT_TERMINAL,
    .bTerminalID        = 3,
    .wTerminalType        = cpu_to_le16(0x0101),
    .bAssocTerminal        = 0,
    .bSourceID        = 2,
    .iTerminal        = 0,
};

DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 4);

static const struct UVC_INPUT_HEADER_DESCRIPTOR(1, 4) uvc_input_header = {
    .bLength        = UVC_DT_INPUT_HEADER_SIZE(1, 4),
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VS_INPUT_HEADER,
    .bNumFormats        = 0,  /* dynamic */
    .wTotalLength        = 0, /* dynamic */
    .bEndpointAddress    = 0, /* dynamic */
    .bmInfo            = 0,
    .bTerminalLink        = 3,
    .bStillCaptureMethod    = 0,
    .bTriggerSupport    = 0,
    .bTriggerUsage        = 0,
    .bControlSize        = 1,
    .bmaControls[0][0]    = 0,
    .bmaControls[1][0]    = 0,
    .bmaControls[2][0]    = 0,
    .bmaControls[3][0]    = 0,
};

static const struct uvc_format_uncompressed uvc_format_yuy2 = {
    .bLength        = UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VS_FORMAT_UNCOMPRESSED,
    .bFormatIndex           = 1, /* dynamic */
    .bNumFrameDescriptors   = 0, /* dynamic */
    .guidFormat        = { VIDEO_GUID_YUY2 },
    .bBitsPerPixel        = 16,
    .bDefaultFrameIndex    = 1,
    .bAspectRatioX        = 0,
    .bAspectRatioY        = 0,
    .bmInterfaceFlags    = 0,
    .bCopyProtect        = 0,
};

static const struct uvc_format_uncompressed uvc_format_nv21 = {
    .bLength        = UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VS_FORMAT_UNCOMPRESSED,
    .bFormatIndex           = 1, /* dynamic */
    .bNumFrameDescriptors   = 0, /* dynamic */
    .guidFormat        = { VIDEO_GUID_NV21 },
    .bBitsPerPixel        = 16,
    .bDefaultFrameIndex    = 1,
    .bAspectRatioX        = 0,
    .bAspectRatioY        = 0,
    .bmInterfaceFlags    = 0,
    .bCopyProtect        = 0,
};

DECLARE_UVC_FRAME_UNCOMPRESSED(1);

static const struct UVC_FRAME_UNCOMPRESSED(1) uvc_frame_yuy2_default = {
    .bLength                    = UVC_DT_FRAME_UNCOMPRESSED_SIZE(1),
    .bDescriptorType            = USB_DT_CS_INTERFACE,
    .bDescriptorSubType         = UVC_VS_FRAME_UNCOMPRESSED,
    .bFrameIndex                = 0, /* dynamic */
    .bmCapabilities             = 0,
    .wWidth                     = cpu_to_le16(1280),
    .wHeight                    = cpu_to_le16(720),
    .dwMinBitRate               = cpu_to_le32(29491200),
    .dwMaxBitRate               = cpu_to_le32(29491200),
    .dwMaxVideoFrameBufferSize  = cpu_to_le32(1843200),
    .dwDefaultFrameInterval     = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
    .bFrameIntervalType         = 1,
    .dwFrameInterval[0]         = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
};

static const struct UVC_FRAME_UNCOMPRESSED(1) uvc_frame_nv21_default = {
    .bLength                    = UVC_DT_FRAME_UNCOMPRESSED_SIZE(1),
    .bDescriptorType            = USB_DT_CS_INTERFACE,
    .bDescriptorSubType         = UVC_VS_FRAME_UNCOMPRESSED,
    .bFrameIndex                = 0, /* dynamic */
    .bmCapabilities             = 0,
    .wWidth                     = cpu_to_le16(1280),
    .wHeight                    = cpu_to_le16(720),
    .dwMinBitRate               = cpu_to_le32(29491200),
    .dwMaxBitRate               = cpu_to_le32(29491200),
    .dwMaxVideoFrameBufferSize  = cpu_to_le32(1843200),
    .dwDefaultFrameInterval     = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
    .bFrameIntervalType         = 1,
    .dwFrameInterval[0]         = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
};

static const struct uvc_format_mjpeg uvc_format_mjpg = {
    .bLength        = UVC_DT_FORMAT_MJPEG_SIZE,
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VS_FORMAT_MJPEG,
    .bFormatIndex           = 1, /* dynamic */
    .bNumFrameDescriptors   = 2, /* dynamic */
    .bmFlags        = 0,
    .bDefaultFrameIndex    = 1,
    .bAspectRatioX        = 0,
    .bAspectRatioY        = 0,
    .bmInterfaceFlags    = 0,
    .bCopyProtect        = 0,
};

DECLARE_UVC_FRAME_MJPEG(1);

static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_default = {
    .bLength                = UVC_DT_FRAME_MJPEG_SIZE(1),
    .bDescriptorType        = USB_DT_CS_INTERFACE,
    .bDescriptorSubType     = UVC_VS_FRAME_MJPEG,
    .bFrameIndex            = 0, /* dynamic */
    .bmCapabilities         = 0,
    .wWidth                 = cpu_to_le16(1920),
    .wHeight                = cpu_to_le16(1080),
    .dwMinBitRate           = cpu_to_le32(995328000),
    .dwMaxBitRate           = cpu_to_le32(995328000),
    .dwMaxVideoFrameBufferSize  = cpu_to_le32(4147200),
    .dwDefaultFrameInterval     = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
    .bFrameIntervalType     = 1,
    .dwFrameInterval[0]     = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
};

struct uvc_format_framebased uvc_format_h264 = {
    .bLength            = UVC_DT_FORMAT_FRAMEBASED_SIZE,
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType = UVC_VS_FORMAT_FRAME_BASED,
    .bFormatIndex           = 0, /* dynamic */
    .bNumFrameDescriptors   = 0,/* dynamic */
    .guidFormat         = { VIDEO_GUID_H264 },
    .bBitsPerPixel      = 16,
    .bDefaultFrameIndex = 1,
    .bAspectRatioX      = 0,
    .bAspectRatioY      = 0,
    .bmInterfaceFlags   = 0,
    .bCopyProtect       = 0,
    .bVariableSize      = 1,
};

DECLARE_UVC_FRAME_FRAMEBASED(1);

static const struct UVC_FRAME_FRAMEBASED(1) uvc_frame_h264_default = {
    .bLength                = UVC_DT_FRAME_FRAMEBASED_SIZE(1),
    .bDescriptorType        = USB_DT_CS_INTERFACE,
    .bDescriptorSubType     = UVC_VS_FRAME_FRAME_BASED,
    .bFrameIndex            = 0, /* dynamic */
    .bmCapabilities         = 0,
    .wWidth                 = cpu_to_le16(1920),
    .wHeight                = cpu_to_le16(1080),
    .dwMinBitRate           = cpu_to_le32(200000000),
    .dwMaxBitRate           = cpu_to_le32(995328000),
    .dwDefaultFrameInterval = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
    .bFrameIntervalType     = 1,
    .dwBytesPerLine         = 0,
    .dwFrameInterval[0]     = cpu_to_le32(FRAME_INTERVAL_FPS(30)),
};

static const struct uvc_color_matching_descriptor uvc_color_matching = {
    .bLength        = UVC_DT_COLOR_MATCHING_SIZE,
    .bDescriptorType    = USB_DT_CS_INTERFACE,
    .bDescriptorSubType    = UVC_VS_COLORFORMAT,
    .bColorPrimaries    = 1,
    .bTransferCharacteristics    = 1,
    .bMatrixCoefficients    = 4,
};

static const struct uvc_descriptor_header * const uvc_fs_control_cls[] = {
    (const struct uvc_descriptor_header *) &uvc_control_header,
    (const struct uvc_descriptor_header *) &uvc_camera_terminal,
    (const struct uvc_descriptor_header *) &uvc_processing,
    (const struct uvc_descriptor_header *) &uvc_output_terminal,
    NULL,
};

#define UVC_INTF_VIDEO_CONTROL          0
#define UVC_INTF_VIDEO_STREAMING        1

#define UVC_STATUS_MAX_PACKET_SIZE        16    /* 16 bytes status */

static struct usb_interface_assoc_descriptor uvc_iad = {
    .bLength        = sizeof(uvc_iad),
    .bDescriptorType    = USB_DT_INTERFACE_ASSOCIATION,
    .bFirstInterface    = 0,
    .bInterfaceCount    = 2,
    .bFunctionClass        = USB_CLASS_VIDEO,
    .bFunctionSubClass    = UVC_SC_VIDEO_INTERFACE_COLLECTION,
    .bFunctionProtocol    = 0x00,
    .iFunction        = 0,
};

static struct usb_interface_descriptor uvc_control_intf = {
    .bLength        = USB_DT_INTERFACE_SIZE,
    .bDescriptorType    = USB_DT_INTERFACE,
    .bInterfaceNumber    = UVC_INTF_VIDEO_CONTROL,
    .bAlternateSetting    = 0,
    .bNumEndpoints        = 0,
    .bInterfaceClass    = USB_CLASS_VIDEO,
    .bInterfaceSubClass    = UVC_SC_VIDEOCONTROL,
    .bInterfaceProtocol    = 0x00,
    .iInterface        = 0x02,
};

// static struct usb_endpoint_descriptor uvc_control_ep = {
//     .bLength        = USB_DT_ENDPOINT_SIZE,
//     .bDescriptorType    = USB_DT_ENDPOINT,
//     .bEndpointAddress    = USB_DIR_IN,
//     .bmAttributes        = USB_ENDPOINT_XFER_INT,
//     .wMaxPacketSize        = cpu_to_le16(UVC_STATUS_MAX_PACKET_SIZE),
//     .bInterval        = 8,
// };

// static struct uvc_control_endpoint_descriptor uvc_control_cs_ep = {
//     .bLength        = UVC_DT_CONTROL_ENDPOINT_SIZE,
//     .bDescriptorType    = USB_DT_CS_ENDPOINT,
//     .bDescriptorSubType    = UVC_EP_INTERRUPT,
//     .wMaxTransferSize    = cpu_to_le16(UVC_STATUS_MAX_PACKET_SIZE),
// };

static struct usb_interface_descriptor uvc_streaming_intf_alt0 = {
    .bLength        = USB_DT_INTERFACE_SIZE,
    .bDescriptorType    = USB_DT_INTERFACE,
    .bInterfaceNumber    = UVC_INTF_VIDEO_STREAMING,
    .bAlternateSetting    = 0,
    .bNumEndpoints        = 0,
    .bInterfaceClass    = USB_CLASS_VIDEO,
    .bInterfaceSubClass    = UVC_SC_VIDEOSTREAMING,
    .bInterfaceProtocol    = 0x00,
    .iInterface        = 0,
};

static struct usb_interface_descriptor uvc_streaming_intf_alt1 = {
    .bLength        = USB_DT_INTERFACE_SIZE,
    .bDescriptorType    = USB_DT_INTERFACE,
    .bInterfaceNumber    = UVC_INTF_VIDEO_STREAMING,
    .bAlternateSetting    = 1,
    .bNumEndpoints        = 1,
    .bInterfaceClass    = USB_CLASS_VIDEO,
    .bInterfaceSubClass    = UVC_SC_VIDEOSTREAMING,
    .bInterfaceProtocol    = 0x00,
    .iInterface        = 0,
};

static struct usb_endpoint_descriptor uvc_hs_streaming_ep = {
    .bLength            = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType    = USB_DT_ENDPOINT,
    .bEndpointAddress   = VIDEO_IN_EP,
    .bmAttributes       = USB_ENDPOINT_XFER_ISOC,
    .wMaxPacketSize     = cpu_to_le16(VIDEO_PACKET_SIZE),
    .bInterval          = 1,
};

static const struct usb_descriptor_header * const uvc_hs_streaming[] = {
    (struct usb_descriptor_header *) &uvc_streaming_intf_alt1,
    (struct usb_descriptor_header *) &uvc_hs_streaming_ep,
    NULL,
};

DECLARE_UVC_STRING_DESCRIPTOR(1);

static const struct UVC_STRING_DESCRIPTOR(1) uvc_string_descriptor_zero = {
    .bLength            = UVC_STRING_DESCRIPTOR_SIZE(1),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_STRING,
    .wData[0]           = cpu_to_le16(USBD_LANGID_STRING),
};


DECLARE_UVC_STRING_DESCRIPTOR(6);

static const struct UVC_STRING_DESCRIPTOR(6) uvc_string_descriptor_manufacturer = {
    .bLength            = UVC_STRING_DESCRIPTOR_SIZE(6),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_STRING,
    .wData              = {
        cpu_to_le16('C'),
        cpu_to_le16('V'),
        cpu_to_le16('I'),
        cpu_to_le16('T'),
        cpu_to_le16('E'),
        cpu_to_le16('K'),
    },
};

DECLARE_UVC_STRING_DESCRIPTOR(13);

static const struct UVC_STRING_DESCRIPTOR(13) uvc_string_descriptor_product = {
    .bLength            = UVC_STRING_DESCRIPTOR_SIZE(13),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_STRING,
    .wData              = {
        cpu_to_le16('C'),
        cpu_to_le16('V'),
        cpu_to_le16('I'),
        cpu_to_le16('T'),
        cpu_to_le16('E'),
        cpu_to_le16('K'),
        cpu_to_le16(' '),
        cpu_to_le16('C'),
        cpu_to_le16('a'),
        cpu_to_le16('m'),
        cpu_to_le16('e'),
        cpu_to_le16('r'),
        cpu_to_le16('a'),
    },
};

DECLARE_UVC_STRING_DESCRIPTOR(12);

static const struct UVC_STRING_DESCRIPTOR(12) uvc_string_descriptor_speaker = {
    .bLength            = UVC_STRING_DESCRIPTOR_SIZE(12),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_STRING,
    .wData              = {
        cpu_to_le16('C'),
        cpu_to_le16('V'),
        cpu_to_le16('I'),
        cpu_to_le16('T'),
        cpu_to_le16('E'),
        cpu_to_le16('K'),
        cpu_to_le16(' '),
        cpu_to_le16('A'),
        cpu_to_le16('u'),
        cpu_to_le16('d'),
        cpu_to_le16('i'),
        cpu_to_le16('o'),
    },
};

DECLARE_UVC_STRING_DESCRIPTOR(10);

static const struct UVC_STRING_DESCRIPTOR(10) uvc_string_descriptor_mic = {
    .bLength            = UVC_STRING_DESCRIPTOR_SIZE(10),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_STRING,
    .wData              = {
        cpu_to_le16('C'),
        cpu_to_le16('V'),
        cpu_to_le16('I'),
        cpu_to_le16('T'),
        cpu_to_le16('E'),
        cpu_to_le16('K'),
        cpu_to_le16(' '),
        cpu_to_le16('M'),
        cpu_to_le16('I'),
        cpu_to_le16('C'),
    },
};

static const struct UVC_STRING_DESCRIPTOR(10) uvc_string_descriptor_serial = {
    .bLength            = UVC_STRING_DESCRIPTOR_SIZE(10),
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_STRING,
    .wData              = {
        cpu_to_le16('2'),
        cpu_to_le16('0'),
        cpu_to_le16('2'),
        cpu_to_le16('3'),
        cpu_to_le16('0'),
        cpu_to_le16('2'),
        cpu_to_le16('1'),
        cpu_to_le16('3'),
        cpu_to_le16('0'),
        cpu_to_le16('0'),
    },
};

static const struct usb_descriptor_header * const uvc_string_descriptors[] = {
    (struct usb_descriptor_header *) &uvc_string_descriptor_zero,
    (struct usb_descriptor_header *) &uvc_string_descriptor_manufacturer,
    (struct usb_descriptor_header *) &uvc_string_descriptor_product,
    (struct usb_descriptor_header *) &uvc_string_descriptor_serial,
    (struct usb_descriptor_header *) &uvc_string_descriptor_speaker,
    (struct usb_descriptor_header *) &uvc_string_descriptor_mic,
    NULL,
};

#ifdef CONFIG_USB_HS
struct usb_qualifier_descriptor uvc_qual_descriptor = {
    .bLength             = USB_DT_QUALIFIER_SIZE,
    .bDescriptorType     = USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    .bcdUSB              = cpu_to_le16(USB_2_0),
    .bDeviceClass        = 0,
    .bDeviceSubClass     = 0,
    .bDeviceProtocol     = 0,
    .bMaxPacketSize0     = 0x40,
    .bNumConfigurations  = 1,
    .bRESERVED           = 0,
};
#endif

#define UVC_COPY_DESCRIPTOR(mem, dst, desc) \
    do { \
        memcpy(mem, desc, (desc)->bLength); \
        *(dst)++ = mem; \
        mem += (desc)->bLength; \
    } while (0);

#define UVC_COPY_DESCRIPTORS(mem, dst, src) \
    do { \
        const struct usb_descriptor_header * const *__src; \
        for (__src = src; *__src; ++__src) { \
            memcpy(mem, *__src, (*__src)->bLength); \
            *dst++ = mem; \
            mem += (*__src)->bLength; \
        } \
    } while (0)

static struct uvc_descriptor_header *
uvc_alloc_format_descriptor(uint32_t format_type, uint32_t format_index, uint32_t frame_cnt)
{
    struct uvc_format_descriptor_header *uvc_format = NULL;
    struct uvc_format_descriptor_header *template_header = NULL;

    switch (format_type) {
    case UVC_FORMAT_YUY2:
        template_header = (struct uvc_format_descriptor_header *)&uvc_format_yuy2;
        break;
    case UVC_FORMAT_MJPEG:
        template_header = (struct uvc_format_descriptor_header *)&uvc_format_mjpg;
        break;
    case UVC_FORMAT_H264:
        template_header = (struct uvc_format_descriptor_header *)&uvc_format_h264;
        break;
    case UVC_FORMAT_NV21:
        template_header = (struct uvc_format_descriptor_header *)&uvc_format_nv21;
        break;
    default:
        break;
    }

    if (template_header == NULL) {
        return NULL;
    }

    uvc_format = (struct uvc_format_descriptor_header *)malloc(template_header->bLength);
    if (uvc_format != NULL) {
        memcpy(uvc_format, template_header, template_header->bLength);
        uvc_format->bFormatIndex = format_index;
        uvc_format->bNumFrameDescriptors = frame_cnt;
    }

    return (struct uvc_descriptor_header *)uvc_format;
}

static struct uvc_descriptor_header *
uvc_alloc_frame_descriptor(uint32_t format_type,
                           uint32_t frame_index,
                           struct uvc_frame_info_st *uvc_frame_info)
{
    struct uvc_descriptor_header *uvc_frame = NULL;
    uint16_t width = 0;
    uint16_t height = 0;
    uint32_t fps = 0;

    if (uvc_frame_info == NULL) {
        return NULL;
    }

    width = uvc_frame_info->width;
    height = uvc_frame_info->height;
    fps = uvc_frame_info->fps;

    switch (format_type) {
    case UVC_FORMAT_YUY2:
    {
        uvc_frame = (struct uvc_descriptor_header *)malloc(uvc_frame_yuy2_default.bLength);
        if (uvc_frame) {
            memcpy(uvc_frame, &uvc_frame_yuy2_default, uvc_frame_yuy2_default.bLength);
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->bFrameIndex = frame_index;
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->wWidth
                                                        = cpu_to_le16(width);
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->wHeight
                                                        = cpu_to_le16(height);
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->dwDefaultFrameInterval
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->dwFrameInterval[0]
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
        }
        break;
    }
    case UVC_FORMAT_NV21:
    {
        uvc_frame = (struct uvc_descriptor_header *)malloc(uvc_frame_nv21_default.bLength);
        if (uvc_frame) {
            memcpy(uvc_frame, &uvc_frame_nv21_default, uvc_frame_nv21_default.bLength);
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->bFrameIndex = frame_index;
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->wWidth
                                                        = cpu_to_le16(width);
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->wHeight
                                                        = cpu_to_le16(height);
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->dwDefaultFrameInterval
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
            ((struct UVC_FRAME_UNCOMPRESSED(1) *)uvc_frame)->dwFrameInterval[0]
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
        }
        break;
    }
    case UVC_FORMAT_MJPEG:
    {
        uvc_frame = (struct uvc_descriptor_header *)malloc(uvc_frame_mjpg_default.bLength);
        if (uvc_frame) {
            memcpy(uvc_frame, &uvc_frame_mjpg_default, uvc_frame_mjpg_default.bLength);
            ((struct UVC_FRAME_MJPEG(1) *)uvc_frame)->bFrameIndex = frame_index;
            ((struct UVC_FRAME_MJPEG(1) *)uvc_frame)->wWidth
                                                        = cpu_to_le16(width);
            ((struct UVC_FRAME_MJPEG(1) *)uvc_frame)->wHeight
                                                        = cpu_to_le16(height);
            ((struct UVC_FRAME_MJPEG(1) *)uvc_frame)->dwDefaultFrameInterval
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
            ((struct UVC_FRAME_MJPEG(1) *)uvc_frame)->dwFrameInterval[0]
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
        }
        break;
    }
    case UVC_FORMAT_H264:
    {
        uvc_frame = (struct uvc_descriptor_header *)malloc(uvc_frame_h264_default.bLength);
        if (uvc_frame) {
            memcpy(uvc_frame, &uvc_frame_h264_default, uvc_frame_h264_default.bLength);
            ((struct UVC_FRAME_FRAMEBASED(1) *)uvc_frame)->bFrameIndex = frame_index;
            ((struct UVC_FRAME_FRAMEBASED(1) *)uvc_frame)->wWidth
                                                        = cpu_to_le16(width);
            ((struct UVC_FRAME_FRAMEBASED(1) *)uvc_frame)->wHeight
                                                        = cpu_to_le16(height);
            ((struct UVC_FRAME_FRAMEBASED(1) *)uvc_frame)->dwDefaultFrameInterval
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
            ((struct UVC_FRAME_FRAMEBASED(1) *)uvc_frame)->dwFrameInterval[0]
                                                        = cpu_to_le32(FRAME_INTERVAL_FPS(fps));
        }
        break;
    }
    default:
        break;
    }

    return (struct uvc_descriptor_header *)uvc_frame;
}

static struct uvc_descriptor_header **
uvc_create_streaming_class(struct uvc_format_info_st *format_info,
                                                uint32_t num_formats)
{
    struct uvc_descriptor_header **uvc_streaming_cls = NULL;
    uint32_t num_descriptors = 0;
    uint32_t pos = 0;
    uint32_t format_type = 0;
    uint32_t frame_cnt = 0;

    if (num_formats == 0
        || format_info == NULL) {
        return NULL;
    }

    // Number 3 is that includes "input header", "color matching" and "NULL"
    num_descriptors = 3 + num_formats;
    for (uint32_t i = 0; i < num_formats; i++) {
        num_descriptors += format_info[i].frame_cnt;
    }

    uvc_streaming_cls = malloc(sizeof(*uvc_streaming_cls) * num_descriptors);
    if (uvc_streaming_cls == NULL) {
        return NULL;
    }

    // "input header"
    uvc_streaming_cls[0] = (struct uvc_descriptor_header *)&uvc_input_header;
    ((struct UVC_INPUT_HEADER_DESCRIPTOR(1, 4) *)uvc_streaming_cls[0])->bNumFormats
                                                                    = num_formats;
    ((struct UVC_INPUT_HEADER_DESCRIPTOR(1, 4) *)uvc_streaming_cls[0])->bLength
                                        = UVC_DT_INPUT_HEADER_SIZE(1, num_formats);
    pos++;

    for (uint32_t i = 0; i < num_formats; i++) {
        format_type = format_info[i].format_type;
        frame_cnt = format_info[i].frame_cnt;
        uvc_streaming_cls[pos++] =
            uvc_alloc_format_descriptor(format_type, i + 1, frame_cnt);
        for(uint32_t j = 0; j < frame_cnt; j++) {
            uvc_streaming_cls[pos++] = 
                uvc_alloc_frame_descriptor(format_type, j + 1, &format_info[i].frames[j]);
        }
    }

    // tail "color matching" and "NULL"
    uvc_streaming_cls[num_descriptors - 2]
        = (struct uvc_descriptor_header *)&uvc_color_matching;
    uvc_streaming_cls[num_descriptors - 1] = NULL;

    return uvc_streaming_cls;
}

uint8_t *
uvc_build_descriptor(struct uvc_format_info_st *format_info,
                     uint32_t num_formats,
                     uint32_t *len)
{
    struct uvc_input_header_descriptor *uvc_streaming_header;
    struct uvc_header_descriptor *uvc_control_header;
    const struct uvc_descriptor_header * const *uvc_control_desc;
    struct uvc_descriptor_header **uvc_streaming_cls;
    const struct usb_descriptor_header * const *uvc_streaming_std;
    const struct usb_descriptor_header * const *src;
    struct usb_descriptor_header **dst;
    uint8_t *hdr;
    unsigned int control_size;
    unsigned int streaming_size;
    unsigned int n_desc;
    unsigned int bytes;
    void *mem;

    uvc_control_desc = uvc_fs_control_cls;
    uvc_streaming_cls = uvc_create_streaming_class(format_info, num_formats);
    uvc_streaming_std = uvc_hs_streaming;

    if (!uvc_control_desc || !uvc_streaming_cls)
        return NULL;

    /* Descriptors layout
     *
     * uvc_iad
     * uvc_control_intf
     * Class-specific UVC control descriptors
     * uvc_control_ep
     * uvc_control_cs_ep
     * uvc_ss_control_comp (for SS only)
     * uvc_streaming_intf_alt0
     * Class-specific UVC streaming descriptors
     * uvc_{fs|hs}_streaming
     */

    /* Count descriptors and compute their size. */
    control_size = 0;
    streaming_size = 0;
    bytes = uvc_iad.bLength + uvc_control_intf.bLength
          + uvc_streaming_intf_alt0.bLength;
    n_desc = 20;

    for (src = (const struct usb_descriptor_header **)uvc_control_desc;
         *src; ++src) {
        control_size += (*src)->bLength;
        bytes += (*src)->bLength;
        n_desc++;
    }
    for (src = (const struct usb_descriptor_header **)uvc_streaming_cls;
         *src; ++src) {
        streaming_size += (*src)->bLength;
        bytes += (*src)->bLength;
        n_desc++;
    }
    for (src = uvc_streaming_std; *src; ++src) {
        bytes += (*src)->bLength;
        n_desc++;
    }

    if (len) {
        *len = bytes;
    }

    bytes++;

    mem = malloc((n_desc + 1) * sizeof(*src) + bytes);
    if (mem == NULL)
        return NULL;

    hdr = mem;
    dst = mem + bytes;

    UVC_COPY_DESCRIPTOR(mem, dst, &uvc_iad);
    UVC_COPY_DESCRIPTOR(mem, dst, &uvc_control_intf);

    uvc_control_header = mem;
    UVC_COPY_DESCRIPTORS(mem, dst,
        (const struct usb_descriptor_header **)uvc_control_desc);
    uvc_control_header->wTotalLength = cpu_to_le16(control_size);
    uvc_control_header->bInCollection = 1;
    uvc_control_header->baInterfaceNr[0] = 1;

    UVC_COPY_DESCRIPTOR(mem, dst, &uvc_streaming_intf_alt0);

    uvc_streaming_header = mem;
    UVC_COPY_DESCRIPTORS(mem, dst,
        (const struct usb_descriptor_header**)uvc_streaming_cls);
    uvc_streaming_header->wTotalLength = cpu_to_le16(streaming_size);
    uvc_streaming_header->bEndpointAddress = VIDEO_IN_EP;

    UVC_COPY_DESCRIPTORS(mem, dst, uvc_streaming_std);

    ((uint8_t *)mem)[0] = 0x00;

    *dst = NULL;
    return hdr;
}

void uvc_destroy_descriptor(uint8_t *hdr)
{
    if (hdr != NULL) {
        free (hdr);
    }
}

static struct uvc_frame_info_st uvc_frame_info;
static struct uvc_format_info_st uvc_format_info;

void uvc_set_video_format_info(const struct uvc_format_info_st *video_format_info){
	memcpy(&uvc_format_info, video_format_info, sizeof(struct uvc_format_info_st));
}

void uvc_get_video_format_info(struct uvc_format_info_st *video_format_info){
	memcpy(video_format_info, &uvc_format_info, sizeof(struct uvc_format_info_st));
}

void uvc_set_video_frame_info(const struct uvc_frame_info_st *video_frame_info){
	memcpy(&uvc_frame_info, video_frame_info, sizeof(struct uvc_frame_info_st));
}

void uvc_get_video_frame_info(struct uvc_frame_info_st *video_frame_info){
	memcpy(video_frame_info, &uvc_frame_info, sizeof(struct uvc_frame_info_st));
}

uint8_t *av_comp_build_descriptors(struct uvc_format_info_st *format_info, uint32_t num_formats)
{
    uint8_t *av_desc = NULL;
    uint8_t *video_desc = NULL;
    uint8_t *audio_desc = NULL;
    uint32_t av_desc_len = 0;
    uint32_t video_desc_len = 0;
    uint32_t audio_desc_len = 0;
    uint32_t bytes = 0;
    uint32_t n_desc = 15;
    void *mem = NULL;
    const struct usb_descriptor_header * const *src;
    struct usb_descriptor_header **dst;
    struct usb_config_descriptor *uvc_config_header;

    audio_desc = uac_build_descriptor(&audio_desc_len);
    if (audio_desc != NULL
        && audio_desc_len > 0) {
    }

    video_desc = uvc_build_descriptor(format_info, num_formats, &video_desc_len);
    if (video_desc != NULL
        && video_desc_len > 0) {
    }
    av_desc_len = video_desc_len + audio_desc_len + uvc_config_descriptor.bLength;
    bytes += av_desc_len;
    bytes += uvc_device_descriptor.bLength;
    for (src = uvc_string_descriptors; *src; ++src) {
        bytes += (*src)->bLength;
    }
#ifdef CONFIG_USB_HS
    bytes += uvc_qual_descriptor.bLength;
#endif

    av_desc = malloc(bytes + 1 + n_desc * sizeof(*src));
    mem = av_desc;
    dst = mem + bytes + 1;
    /* Copy the descriptors. */
    UVC_COPY_DESCRIPTOR(mem, dst, &uvc_device_descriptor);

    uvc_config_header = mem;
    UVC_COPY_DESCRIPTOR(mem, dst, &uvc_config_descriptor);
    uvc_config_header->wTotalLength = cpu_to_le16(av_desc_len);

    // Copy uvc iad
    memcpy(mem, video_desc, video_desc_len);
    mem += video_desc_len;
    // Copy uac iad
    memcpy(mem, audio_desc, audio_desc_len);
    mem += audio_desc_len;

    UVC_COPY_DESCRIPTORS(mem, dst, uvc_string_descriptors);
#ifdef CONFIG_USB_HS
    UVC_COPY_DESCRIPTOR(mem, dst, &uvc_qual_descriptor);
#endif
    ((uint32_t *)mem)[0] = 0x00;


    uac_destroy_descriptor(audio_desc);
    uvc_destroy_descriptor(video_desc);

    return av_desc;
}

void av_comp_destroy_descriptors(uint8_t *descriptors)
{
    if (descriptors) {
        free(descriptors);
    }
    return ;
}


