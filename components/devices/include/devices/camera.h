/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */


#ifndef _DEVICE_CAMERA_H_
#define _DEVICE_CAMERA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <devices/device.h>
#include <devices/driver.h>

/*
 * Camera logical parts are below, including: input sensor and output channel
 *  @------------------------------@
 *  |    Camera Settings      _____|______   Channel Settings
 *  |  ===================   |            | =====================
 *  |      Exposure          | channel[0] | configuration: width/height, pix_fmt
 *  |      R/G/B Gain        |_____ ______| ---> Channel Events
 *  |      HDR mode           _____|______
 *  |        ......          |            |
 *  |                        | channel[1] | configuration: width/height, pix_fmt
 *  |                        |_____ ______| ---> Channel Events
 *  |                         _____|______
 *  |                        |            |
 *  |                        | channel[n] | configuration: width/height, pix_fmt
 *  |                        |_____ ______| ---> Channel Events
 *  |        CAMERA                |
 *  @------------------------------@        ===> Camera Events (sensor, error, warning...)
 */


#define RVM_HAL_MAX_PLANE_COUNT     3

typedef struct camera_cfg {
    uint32_t iic_id;
} rvm_hal_camera_cfg_t;

typedef enum {
	RVM_HAL_PIX_FMT_INVALID = -1,
	RVM_HAL_PIX_FMT_I420,	// YYYYYYYY UU VV, alias: YUV420P/YU12/IYUV
	RVM_HAL_PIX_FMT_NV12,	// YYYYYYYY UV UV, alias: YUV420SP
	RVM_HAL_PIX_FMT_BGR,
	RVM_HAL_PIX_FMT_RAW_8BIT,/*default no align*/
	RVM_HAL_PIX_FMT_RAW_10BIT,
	RVM_HAL_PIX_FMT_RAW_12BIT,
	RVM_HAL_PIX_FMT_RAW_14BIT,
	RVM_HAL_PIX_FMT_RAW_16BIT,
	RVM_HAL_PIX_FMT_RGB_PLANAR_888,
	RVM_HAL_PIX_FMT_RGB_INTEVLEAVED_888,
	RVM_HAL_PIX_FMT_YUV_PLANAR_422,
	RVM_HAL_PIX_FMT_YUV_PLANAR_420,
	RVM_HAL_PIX_FMT_YUV_PLANAR_444,
	RVM_HAL_PIX_FMT_YUV_SEMIPLANAR_422,
	RVM_HAL_PIX_FMT_YUV_SEMIPLANAR_420,
	RVM_HAL_PIX_FMT_YUV_SEMIPLANAR_444,
	RVM_HAL_PIX_FMT_YUV_TEVLEAVED_422,
	RVM_HAL_PIX_FMT_YUV_TEVLEAVED_420,
	RVM_HAL_PIX_FMT_YUV_TEVLEAVED_444,
} rvm_hal_pixel_fmt_e;

typedef struct {
	uint32_t	width;
	uint32_t	height;
	rvm_hal_pixel_fmt_e	pix_fmt;
} rvm_hal_img_format_t;

typedef enum {
	RVM_HAL_COLORSPACE_ACES,   /* RGB color space ACES standardized as SMPTE ST 2065-1:2012. */
	RVM_HAL_COLORSPACE_ACESCG, /* RGB color space ACEScg standardized as Academy S-2014-004. */
	RVM_HAL_COLORSPACE_RGB,    /* RGB color space Adobe RGB (1998). */
	RVM_HAL_COLORSPACE_BT2020, /* RGB color space BT.2020 standardized as Rec. */
	RVM_HAL_COLORSPACE_BT709,   /* RGB color space BT.709 standardized as Rec. */
	RVM_HAL_COLORSPACE_CIE_LAB, /* Lab color space CIE L*a*b*. */
	RVM_HAL_COLORSPACE_CIE_XYZ, /* XYZ color space CIE XYZ.  */
	RVM_HAL_COLORSPACE_DCI_P3,  /* RGB color space DCI-P3 standardized as SMPTE RP 431-2-2007. */
	RVM_HAL_COLORSPACE_DISPLAY_p3, /* RGB color space Display P3 based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999.  */
	RVM_HAL_COLORSPACE_EXTENDED_SRGB, /* RGB color space scRGB-nl standardized as IEC 61966-2-2:2003. */
	RVM_HAL_COLORSPACE_LINEAR_EXTENDED_SRGB, /* RGB color space scRGB standardized as IEC 61966-2-2:2003.  */
	RVM_HAL_COLORSPACE_LINEAR_SRGB, /* RGB color space sRGB standardized as IEC 61966-2.1:1999.  */
	RVM_HAL_COLORSPACE_NTSC_1953, /* RGB color space NTSC, 1953 standard.  */
	RVM_HAL_COLORSPACE_PRO_PHOTO_RGB, /* RGB color space NTSC, 1953 standard. */
	RVM_HAL_COLORSPACE_SMPTE_C, /* RGB color space SMPTE C.  */
	RVM_HAL_COLORSPACE_SRGB  /* RGB color space sRGB standardized as IEC 61966-2.1:1999.  */
} rvm_hal_color_space_e;

typedef enum {
	RVM_HAL_CHROMA_LOCATION_LEFT,
	RVM_HAL_CHROMA_LOCATION_CENTER,
	RVM_HAL_CHROMA_LOCATION_TOPLEFT,
	RVM_HAL_CHROMA_LOCATION_TOP,
	RVM_HAL_CHROMA_LOCATION_BOTTOMLEFT,
	RVM_HAL_CHROMA_LOCATION_BOTTOM,
	RVM_HAL_CHROMA_LOCATION_DV420
} rvm_hal_chroma_location_e;

typedef struct {
	int32_t width;
	int32_t height;
	/* the region of interest of the frame */
	int32_t pixel_format;   //define in rvm_hal_pixel_fmt_e
	rvm_hal_color_space_e  color_space;
	rvm_hal_chroma_location_e chroma_location;
} rvm_hal_frame_info_t;

typedef struct {
	int64_t pts;
	int64_t dts;
	int64_t frame_num;
	int8_t is_key_frame;
	int8_t is_eos; /* last frame */

	int32_t offset[RVM_HAL_MAX_PLANE_COUNT];
	int32_t stride[RVM_HAL_MAX_PLANE_COUNT];
	int32_t num_plane;

	void *vir_addr[RVM_HAL_MAX_PLANE_COUNT];
	union {
		int fd[RVM_HAL_MAX_PLANE_COUNT];  /* -1 means invalid */
		int64_t phy_addr[RVM_HAL_MAX_PLANE_COUNT]; /* 0 means invalid */
	};
} rvm_hal_frame_data_t;

typedef enum {
	RVM_HAL_META_TYPE_SYSTEM,
	RVM_HAL_META_TYPE_CAMERA,
	RVM_HAL_META_TYPE_VDEC,
	RVM_HAL_META_TYPE_GPU,
	RVM_HAL_META_TYPE_G2D,
} rvm_hal_meta_type_e;

typedef struct {
	rvm_hal_meta_type_e  type;
	size_t           size;
	void            *data;
} rvm_hal_meta_t;

typedef struct {
	rvm_hal_frame_info_t frame_info;
	rvm_hal_frame_data_t frame_data;
    rvm_hal_meta_t frame_meta;
	void *opaque; /* for user private data, csi NOT use or modify it */
    void *csi_priv;/**for csi private data,user NOT use or modify it***************/
} rvm_hal_frame_ex_t;

typedef struct {
    int32_t stride_alignment;	// should >= to the width of the image according to IP design
    int32_t addr_alignment;	// physical begin address alignment according to IP design
    int32_t extra_size;	    // storaging platform private data according to IP design (meta data storage is suggested)
    int32_t min_buffers_count;  // minimum number of buffers required
} rvm_hal_frame_config_t;       // New defined structure

#define RVM_HAL_MAX_FRAME_COUNT 1024
typedef enum {
	RVM_HAL_FRAME_IDLE = 1,
	RVM_HAL_FRAME_WORKING = 2,
	RVM_HAL_FRAME_READY = 3,
	RVM_HAL_FRAME_DISPATCHED = 4,
} rvm_hal_frame_status_type_e;

typedef enum {
	RVM_HAL_IMG_TYPE_DMA_BUF,		// memory allocated via dma-buf
	RVM_HAL_IMG_TYPE_SYSTEM_CONTIG,	// memory allocated via kmalloc
	RVM_HAL_IMG_TYPE_CARVEOUT,		// memory allocated from reserved memory
	RVM_HAL_IMG_TYPE_UMALLOC,		// memory allocated from user mode malloc
	RVM_HAL_IMG_TYPE_SHM,		// memory allocated from share memory(<sys/shm.h>)
} rvm_hal_img_type_e;

typedef enum {
	RVM_HAL_CAMERA_CHANNEL_0 = 0,
	RVM_HAL_CAMERA_CHANNEL_1,
	RVM_HAL_CAMERA_CHANNEL_2,
	RVM_HAL_CAMERA_CHANNEL_3,
	RVM_HAL_CAMERA_CHANNEL_4,
	RVM_HAL_CAMERA_CHANNEL_5,
	RVM_HAL_CAMERA_CHANNEL_6,
	RVM_HAL_CAMERA_CHANNEL_7,
	RVM_HAL_CAMERA_CHANNEL_MAX_COUNT
} rvm_hal_camera_channel_id_e;

typedef enum {
	RVM_HAL_CAMERA_CHANNEL_INVALID = -1,	/* channel can't be openned */
    RVM_HAL_CAMERA_CHANNEL_CLOSED,
    RVM_HAL_CAMERA_CHANNEL_OPENED,
	RVM_HAL_CAMERA_CHANNEL_RUNNING,
    RVM_HAL_CAMERA_CHANNEL_EXCEPTION,
} rvm_hal_camera_channel_status_e;

typedef enum {
	RVM_HAL_CAMERA_CHANNEL_CAPTURE_VIDEO = (1 << 0),
	RVM_HAL_CAMERA_CHANNEL_CAPTURE_META  = (1 << 1),
} rvm_hal_camera_channel_capture_type_e;

typedef struct {
    int (*alloc)(rvm_hal_frame_ex_t * frame); /* base on the frame info,alloc fill the fd */
    int (*free)(rvm_hal_frame_ex_t * frame);
} rvm_hal_frame_alloctor_t;

typedef enum {
	RVM_HAL_CAMERA_META_ID_CAMERA_NAME = (1<<0), /* str_value */
	RVM_HAL_CAMERA_META_ID_CHANNEL_ID  = (1<<1), /* uint_value */
	RVM_HAL_CAMERA_META_ID_FRAME_ID    = (1<<2), /* uint_value: Re-Count from zero when start() */
	RVM_HAL_CAMERA_META_ID_TIMESTAMP   = (1<<3), /* time_value: Get from gettimeofday()  */
	RVM_HAL_CAMERA_META_ID_HDR         = (1<<4), /* bool_value */
} rvm_hal_camera_meta_id_e;

typedef struct {
    rvm_hal_camera_channel_id_e chn_id;
    unsigned int capture_type;              /* bitmask of: rvm_hal_camera_channel_capture_type_e */
    unsigned int frm_cnt;
    rvm_hal_img_format_t img_fmt;
    rvm_hal_img_type_e img_type;
    unsigned int meta_fields;               /* bitmask of: rvm_hal_camera_meta_id_e */
    rvm_hal_camera_channel_status_e status;
    rvm_hal_frame_alloctor_t alloctor;      /***external frame buffer allocater***/
} rvm_hal_camera_channel_cfg_t;

typedef enum {
	RVM_HAL_CAMERA_EVENT_TYPE_INVALID = -1,
	RVM_HAL_CAMERA_EVENT_TYPE_CAMERA,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL0,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL1,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL2,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL3,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL4,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL5,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL6,
	RVM_HAL_CAMERA_EVENT_TYPE_CHANNEL7,
} rvm_hal_camera_event_type_e;

typedef enum {
	RVM_HAL_CAMERA_EVENT_WARNING                   = 1 << 0,
	RVM_HAL_CAMERA_EVENT_ERROR                     = 1 << 1,
	RVM_HAL_CAMERA_EVENT_SENSOR_FIRST_IMAGE_ARRIVE = 1 << 2,
	RVM_HAL_CAMERA_EVENT_ISP_3A_ADJUST_READY       = 1 << 3,

	RVM_HAL_CAMERA_EVENT_MAX_COUNT = 32
} rvm_hal_camera_event_id_e;

typedef enum {
    RVM_HAL_CAMERA_3A_STATUS_AE_LOCKED      = 1 << 0,
    RVM_HAL_CAMERA_3A_STATUS_AWB_LOCKED     = 1 << 1,
    RVM_HAL_CAMERA_3A_STATUS_AF_FOCUSFOUND  = 1 << 2,
} rvm_hal_camera_3a_status_e;

typedef enum {
	RVM_HAL_CAMERA_CHANNEL_EVENT_FRAME_READY = 1 << 0,
	RVM_HAL_CAMERA_CHANNEL_EVENT_FRAME_PUT   = 1 << 1,
	RVM_HAL_CAMERA_CHANNEL_EVENT_OVERFLOW    = 1 << 2,

	RVM_HAL_CAMERA_CHANNEL_EVENT_MAX_COUNT = 32
} rvm_hal_camera_channel_event_id_e;

typedef struct {
	rvm_hal_camera_event_type_e type;
	unsigned int id;	/* bitmasks rvm_hal_camera_event_id_e */
} rvm_hal_camera_event_subscription_t;

typedef struct {
    rvm_dev_t *cam_handle;
	rvm_hal_camera_event_type_e	type;
	unsigned int		id;
	struct timespec		timestamp;
	union {
		char bin[128];
	};
} rvm_hal_camera_event_t;

typedef enum {
    RVM_HAL_CAMERA_RET_ERR_INVALID_DEV = -1001,/* device is not valid */
    RVM_HAL_CAMERA_RET_ERR_INVALID_PARA, /* input params is not valid */
    RVM_HAL_CAMERA_RET_ERR_NULL_PTR,
    RVM_HAL_CAMERA_RET_ERR_DEV_BUSY,  /* device is budy */
    RVM_HAL_CAMERA_RET_ERR_NOMEM,   /* malloc fail */
    RVM_HAL_CAMERA_RET_ERR_TIMEOUT, /* evet or frame wait timeout */
    RVM_HAL_CAMERA_RET_ERR_NOT_SUPPORT, /* setting or config is not supported */
    RVM_HAL_CAMERA_RET_ERR_NO_SENSOR_DETECT, /* sensor is not detected */
    RVM_HAL_CAMERA_CHANNEL_RET_ERR_INVALID_CHN = -501,/*channel is not valid */
    RVM_HAL_CAMERA_CHANNEL_RET_ERR_NOTOPEN,
    RVM_HAL_CAMERA_RET_ERR = -1, /*common sw error */
    RVM_HAL_CAMERA_RET_OK =0,
} rvm_hal_camera_erro_e;

#define rvm_hal_camera_open(name)           rvm_hal_device_open(name)
#define rvm_hal_camera_close(dev)           rvm_hal_device_close(dev)

/**
  \brief       Open and config the camera channel.
  \param[in]   dev - Pointer to device object.
  \param[in]   cfg - Pointer to camera channel configuration.
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_channel_open(rvm_dev_t *dev, rvm_hal_camera_channel_cfg_t *cfg);

/**
  \brief       Close the camera channel.
  \param[in]   dev      - Pointer to device object.
  \param[in]   chn_id   - The Channel ID
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_channel_close(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);

/**
  \brief       Get the configuration from a camera
  \param[in]   dev      - Pointer to device object.
  \param[in out]  cfg   - Pointer to camera channel configuration, need give channel id first.
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_channel_query(rvm_dev_t *dev, rvm_hal_camera_channel_cfg_t *cfg);

/**
  \brief       Start the camera with channel ID
  \param[in]   dev      - Pointer to device object.
  \param[in]   chn_id   - The Channel ID
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_channel_start(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);

/**
  \brief       Stop the camera with channel ID
  \param[in]   dev      - Pointer to device object.
  \param[in]   chn_id   - The Channel ID
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_channel_stop(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);

/**
  \brief       Create the event for the camera device
  \param[in]   dev      - Pointer to device object.
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_create_event(rvm_dev_t *dev);

/**
  \brief       Destory the event for the camera device
  \param[in]   dev      - Pointer to device object.
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_destory_event(rvm_dev_t *dev);

/**
  \brief       Subscribe the event for the camerea device
  \param[in]   dev          - Pointer to device object.
  \param[in]   subscribe    - Pointer to a event subscription.
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_subscribe_event(rvm_dev_t *dev, rvm_hal_camera_event_subscription_t *subscribe);

/**
  \brief       Unsubscribe the event for the camerea device
  \param[in]   dev          - Pointer to device object.
  \param[in]   subscribe    - Pointer to a event subscription.
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_unsubscribe_event(rvm_dev_t *dev, rvm_hal_camera_event_subscription_t *subscribe);

/**
  \brief       Get the event with a event handle
  \param[in]   event_handle - Pointer to a event handle.
  \param[in]   event        - Pointer to a event information
  \param[in]   timeout_ms   - The timeout, millisecond
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_get_event(rvm_dev_t *dev, rvm_hal_camera_event_t *event, int timeout_ms);

/**
  \brief       Get the frame count when event occur
  \param[in]   dev      - Pointer to device object.
  \param[in]   chn_id   - The Channel ID
  \return      The frame count, or negative if failed.
*/
int rvm_hal_camera_get_frame_count(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);

/**
  \brief       Get the frame when event occur
  \param[in]   dev          - Pointer to device object.
  \param[in]   chn_id       - The Channel ID
  \param[out]  frame        - Point to frame info
  \param[in]   timeout_ms   - The timeout, millisecond
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_camera_get_frame(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id, rvm_hal_frame_ex_t *frame, int timeout_ms);

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_camera.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
