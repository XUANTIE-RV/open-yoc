#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include "usbd_core.h"
#include "usbd_video.h"
#include "media_video.h"
#include "video.h"
#include "cvi_venc.h"
#include "cvi_vpss.h"
#include "cvi_sys.h"

#define VIDEO_IN_EP 0x81

#define WIDTH  (unsigned int)(1600)
#define HEIGHT (unsigned int)(1200)

#define CAM_FPS        (30)
#define INTERVAL       (unsigned long)(10000000 / CAM_FPS)
#define MAX_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 2)
#define DEFAULT_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 3 / 2)

#define UVC_VENC_CHN   (0)
#define UVC_VPSS_CHN   (0)
#define UVC_VPSS_GRP   (0)

#define MJPEG_FORMAT_INDEX  (1)
#define H264_FORMAT_INDEX   (2)
#define YUYV_FORMAT_INDEX   (3)
#define NV21_FORMAT_INDEX   (4)

volatile bool tx_flag = CVI_FALSE;
volatile bool iso_tx_busy = CVI_FALSE;
volatile bool uvc_update = CVI_FALSE;
static int uvc_session_init_flag = CVI_FALSE;

CVI_S32 is_media_info_update();
void uvc_parse_media_info(uint8_t bFormatIndex, uint8_t bFrameIndex);
void uvc_media_update();


static struct uvc_frame_info_st yuy2_frame_info[] = {
    {1, 800, 600, 30, 0},
    {2, 640, 360, 30, 0},
    {3, 400, 300, 30, 0},
    {5, 480, 320, 30, 0},
    {6, 480, 360, 30, 0},
    {7, 1280, 720, 30, 0},
    {8, 1920, 1080, 30, 0},
    {9, 960, 640, 15, 0},
    {10, 320, 240, 15, 0},
};

static struct uvc_frame_info_st mjpeg_frame_info[] = {
    {1, 800, 600, 30, 0},
    {2, 640, 480, 30, 0},
    {3, 1280, 720, 30, 0},
    {4, 1920, 1080, 30, 0},
    {5, 480, 320, 30, 0},
    {6, 400, 300, 30, 0},
};

static struct uvc_frame_info_st h264_frame_info[] = {
    {1, 800, 600, 30, 0},
    {2, 1280, 720, 30, 0},
    {3, 640, 480, 30, 0},
    {4, 400, 300, 30, 0},
    {5, 1920, 1080, 30, 0},
};

static struct uvc_frame_info_st nv21_frame_info[] = {
    {1, 800, 600, 30, 0},
    {2, 1280, 720, 30, 0},
    {3, 640, 480, 30, 0},
};

static struct uvc_format_info_st uvc_format_info[] = {
    {MJPEG_FORMAT_INDEX, UVC_FORMAT_MJPEG, 1, ARRAY_SIZE(mjpeg_frame_info), mjpeg_frame_info},
    {H264_FORMAT_INDEX, UVC_FORMAT_H264, 1, ARRAY_SIZE(h264_frame_info), h264_frame_info},
    {YUYV_FORMAT_INDEX, UVC_FORMAT_YUY2, 1, ARRAY_SIZE(yuy2_frame_info), yuy2_frame_info},
    {NV21_FORMAT_INDEX, UVC_FORMAT_NV21, 1, ARRAY_SIZE(nv21_frame_info), nv21_frame_info},
};

CVI_S32 is_media_info_update(){
	PAYLOAD_TYPE_E enType;
	PIXEL_FORMAT_E enPixelFormat;
	VENC_CHN_ATTR_S stVencChnAttr,*pstVencChnAttr = &stVencChnAttr;
	VPSS_CHN_ATTR_S stVpssChnAttr, *pstVpssChnAttr = &stVpssChnAttr;
	PARAM_VENC_CFG_S *pstVencCfg = PARAM_getVencCtx();
	CVI_U8 u8VencInitStatus = pstVencCfg->pstVencChnCfg[UVC_VENC_CHN].stChnParam.u8InitStatus;


	struct uvc_format_info_st uvc_format_info;
	struct uvc_frame_info_st uvc_frame_info;

	uvc_get_video_format_info(&uvc_format_info);
	uvc_get_video_frame_info(&uvc_frame_info);

	CVI_VPSS_GetChnAttr(UVC_VPSS_GRP, UVC_VPSS_CHN, pstVpssChnAttr);

	switch(uvc_format_info.format_index){
	case YUYV_FORMAT_INDEX:
		enPixelFormat = PIXEL_FORMAT_YUYV;
		break;
	case NV21_FORMAT_INDEX:
		enPixelFormat = PIXEL_FORMAT_NV21;
		break;
	default:
		enPixelFormat = PIXEL_FORMAT_MAX;
		break;
	}

	if(u8VencInitStatus == 0 && enPixelFormat == PIXEL_FORMAT_MAX)
		return CVI_TRUE;

	if((pstVpssChnAttr->enPixelFormat != enPixelFormat) ||
		(pstVpssChnAttr->u32Width != uvc_frame_info.width) ||
		(pstVpssChnAttr->u32Height != uvc_frame_info.height))
		return CVI_TRUE;

	if(u8VencInitStatus == 0 && enPixelFormat != PIXEL_FORMAT_MAX)
		return CVI_FALSE;


	switch(uvc_format_info.format_index){
	case MJPEG_FORMAT_INDEX:
		enType = PT_MJPEG;
		break;
	case H264_FORMAT_INDEX:
		enType = PT_H264;
		break;
	default:
		enType = PT_BUTT;
		break;
	}

	if(u8VencInitStatus == 0 && enType != PT_BUTT)
		return CVI_TRUE;

	CVI_VENC_GetChnAttr(UVC_VENC_CHN, pstVencChnAttr);
	if((pstVencChnAttr->stVencAttr.enType != enType)||
		(pstVencChnAttr->stVencAttr.u32PicWidth != uvc_frame_info.width) ||
		(pstVencChnAttr->stVencAttr.u32PicHeight != uvc_frame_info.height))
		return CVI_TRUE;

	return CVI_FALSE;

}

void uvc_parse_media_info(uint8_t bFormatIndex, uint8_t bFrameIndex)
{
    const struct uvc_format_info_st *format_info;
    const int uvcout_format_cnt = ARRAY_SIZE(uvc_format_info);

    if (bFormatIndex < 0)
        bFormatIndex = uvcout_format_cnt + bFormatIndex;
    if (bFormatIndex < 0 || bFormatIndex > uvcout_format_cnt)
    {
        aos_debug_printf("format_cnt =%d, format %d error!\r\n", uvcout_format_cnt, bFormatIndex);
        return;
    }

    format_info = &uvc_format_info[bFormatIndex - 1];

    const int nframes = format_info->frame_cnt;
    if (bFrameIndex < 0)
        bFrameIndex = nframes + bFrameIndex;
    if (bFrameIndex < 0 || bFrameIndex > nframes)
    {
        aos_debug_printf("nframes = %d, frame %d error!\r\n", nframes, bFrameIndex);
        return;
    }

	uvc_set_video_format_info(format_info);
	uvc_set_video_frame_info(&format_info->frames[bFrameIndex - 1]);
}

void uvc_media_update(){
	PAYLOAD_TYPE_E enType;
	PIXEL_FORMAT_E enPixelFormat;
	PARAM_VENC_CFG_S *pstVencCfg = PARAM_getVencCtx();
	VPSS_CHN_ATTR_S stVpssChnAttr;
	CVI_U8 u8VencInitStatus = pstVencCfg->pstVencChnCfg[UVC_VENC_CHN].stChnParam.u8InitStatus;


	struct uvc_format_info_st uvc_format_info;
	struct uvc_frame_info_st uvc_frame_info;
	uvc_get_video_format_info(&uvc_format_info);
	uvc_get_video_frame_info(&uvc_frame_info);

	switch(uvc_format_info.format_index){
	case YUYV_FORMAT_INDEX:
		enPixelFormat = PIXEL_FORMAT_YUYV;
		break;
	case NV21_FORMAT_INDEX:
		enPixelFormat = PIXEL_FORMAT_NV21;
		break;
	default:
		enPixelFormat = PIXEL_FORMAT_NV21;
		break;
	}

	switch(uvc_format_info.format_index){
	case MJPEG_FORMAT_INDEX:
		enType = PT_MJPEG;
		break;
	case H264_FORMAT_INDEX:
		enType = PT_H264;
		break;
	default:
		enType = PT_MJPEG;
		break;
	}

	if(u8VencInitStatus == 1)
		MEDIA_VIDEO_VencDeInit(pstVencCfg);

	CVI_VPSS_GetChnAttr(UVC_VPSS_GRP,UVC_VPSS_CHN, &stVpssChnAttr);
	stVpssChnAttr.enPixelFormat = enPixelFormat;
	stVpssChnAttr.u32Width = uvc_frame_info.width;
	stVpssChnAttr.u32Height = uvc_frame_info.height;
	CVI_VPSS_SetChnAttr(UVC_VPSS_GRP,UVC_VPSS_CHN, &stVpssChnAttr);

	pstVencCfg->pstVencChnCfg[UVC_VENC_CHN].stChnParam.u16Width = uvc_frame_info.width;
	pstVencCfg->pstVencChnCfg[UVC_VENC_CHN].stChnParam.u16Height = uvc_frame_info.height;
	pstVencCfg->pstVencChnCfg[UVC_VENC_CHN].stChnParam.u16EnType = enType;
	pstVencCfg->pstVencChnCfg[UVC_VENC_CHN].stRcParam.u16BitRate = (enType == PT_MJPEG)?20480:2048;
	pstVencCfg->pstVencChnCfg[UVC_VENC_CHN].stRcParam.u16RcMode = (enType == PT_MJPEG)?VENC_RC_MODE_MJPEGCBR:VENC_RC_MODE_H264CBR;

	if(MJPEG_FORMAT_INDEX == uvc_format_info.format_index || H264_FORMAT_INDEX == uvc_format_info.format_index)
		MEDIA_VIDEO_VencInit(pstVencCfg);
}

void uvc_streaming_on(int is_on) {
//    aos_debug_printf("streaming %s\n", is_on ? "on" : "off");

    tx_flag = is_on;
    iso_tx_busy = false;

	if(is_on && is_media_info_update())
		uvc_update = 1;
}

void usbd_configure_done_callback(void)
{
    /* no out ep, so do nothing */
}

void uvc_setup_class_control(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    aos_debug_printf("%s:%d\n", __FUNCTION__, __LINE__);
}

void uvc_setup_class_streaming(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    aos_debug_printf("%s:%d\n", __FUNCTION__, __LINE__);
}

void uvc_data_out(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    aos_debug_printf("%s:%d\n", __FUNCTION__, __LINE__);
}

void uvc_tx_complete(uint8_t ep, uint32_t nbytes)
{
    // aos_debug_printf("%d bytes of data sent at ep(%d)\n", nbytes, ep);
    iso_tx_busy = false;
}

void usbd_video_commit_set_cur(struct video_probe_and_commit_controls *commit)
{
	uvc_parse_media_info(commit->bFormatIndex, commit->bFrameIndex);
//    aos_debug_printf("commit format idx:%d, frame idx:%d\n", commit->bFormatIndex, commit->bFrameIndex);
}

static uvc_event_callbacks_t uvc_evt_callbks = {
    .uvc_event_stream_on = uvc_streaming_on,
    .uvc_event_setup_class_control = uvc_setup_class_control,
    .uvc_event_setup_class_streaming = uvc_setup_class_streaming,
    .uvc_event_data_out = uvc_data_out,
    .uvc_event_tx_complete = uvc_tx_complete,
};

static void *send_to_uvc()
{
    uint32_t out_len, i = 0, ret = 0;
	uint32_t buf_len = 0,buf_len_stride = 0, packets = 0;
	uint8_t *packet_buffer_media = (uint8_t *)usb_malloc(DEFAULT_FRAME_SIZE);
	uint8_t *packet_buffer_uvc = (uint8_t *)usb_malloc(DEFAULT_FRAME_SIZE);
    memset(packet_buffer_media, 0, DEFAULT_FRAME_SIZE);
    memset(packet_buffer_uvc, 0, DEFAULT_FRAME_SIZE);
	extern volatile bool tx_flag;
	extern volatile bool iso_tx_busy;
    VENC_STREAM_S stStream = {0},*pstStream= &stStream;
	VENC_PACK_S *ppack;
	VIDEO_FRAME_INFO_S stVideoFrame, *pstVideoFrame=&stVideoFrame;
	VPSS_CHN_ATTR_S stChnAttr,*pstChnAttr = &stChnAttr;
	struct uvc_format_info_st uvc_format_info;

    while (uvc_session_init_flag) {
        if (tx_flag) {
			
			if(uvc_update){
				uvc_media_update();
				uvc_get_video_format_info(&uvc_format_info);
				uvc_update = 0;
			}

			
			if(H264_FORMAT_INDEX == uvc_format_info.format_index || 
				MJPEG_FORMAT_INDEX == uvc_format_info.format_index){
				
		        ret = MEDIA_VIDEO_VencGetStream(UVC_VENC_CHN,pstStream,2000);
				if(ret == CVI_FAILURE){
	//				printf("MEDIA_VIDEO_VencGetStream failed\n");
					aos_msleep(1);
					continue;
				}

				for (i = 0; i < pstStream->u32PackCount; ++i)
				{
					if(buf_len < DEFAULT_FRAME_SIZE){
						ppack = &pstStream->pstPack[i];
						memcpy(packet_buffer_media + buf_len, ppack->pu8Addr + ppack->u32Offset, ppack->u32Len - ppack->u32Offset);
						buf_len += (ppack->u32Len - ppack->u32Offset);
					}
					else{
							printf("venc buf_len oversize\n");
							MEDIA_VIDEO_VencReleaseStream(0,pstStream);
							continue;
					}
				}
				ret = MEDIA_VIDEO_VencReleaseStream(UVC_VENC_CHN,pstStream);
				if(ret != CVI_SUCCESS)
					printf("MEDIA_VIDEO_VencReleaseStream failed\n");

				}else 
			if(YUYV_FORMAT_INDEX == uvc_format_info.format_index){
				ret = CVI_VPSS_GetChnFrame(UVC_VPSS_GRP, UVC_VPSS_CHN, pstVideoFrame, -1);
				if(ret == CVI_FAILURE){
//					printf("CVI_VPSS_GetChnFrame failed\n");
					aos_msleep(1);
					continue;
				}
				CVI_VPSS_GetChnAttr(UVC_VPSS_GRP, UVC_VPSS_CHN, pstChnAttr);

				pstVideoFrame->stVFrame.pu8VirAddr[0] = (uint8_t *) pstVideoFrame->stVFrame.u64PhyAddr[0];
				int data_len = pstChnAttr->u32Width * 2;
				for (i = 0;i < (pstChnAttr->u32Height); ++i)
				{
					memcpy(packet_buffer_media + buf_len, pstVideoFrame->stVFrame.pu8VirAddr[0] + 
						buf_len_stride, data_len);

					buf_len += pstChnAttr->u32Width * 2;
					buf_len_stride += pstVideoFrame->stVFrame.u32Stride[0];
				}
				pstVideoFrame->stVFrame.pu8VirAddr[0] = NULL;

				ret = CVI_VPSS_ReleaseChnFrame(UVC_VPSS_GRP, UVC_VPSS_CHN, pstVideoFrame);
				if(ret != CVI_SUCCESS)
					printf("CVI_VPSS_ReleaseChnFrame failed\n");
			}else
			if(NV21_FORMAT_INDEX == uvc_format_info.format_index){
				ret = CVI_VPSS_GetChnFrame(UVC_VPSS_GRP, UVC_VPSS_CHN, pstVideoFrame, -1);
				if(ret == CVI_FAILURE){
//					printf("CVI_VPSS_GetChnFrame failed\n");
					aos_msleep(1);
					continue;
				}
				CVI_VPSS_GetChnAttr(UVC_VPSS_GRP, UVC_VPSS_CHN, pstChnAttr);

				pstVideoFrame->stVFrame.pu8VirAddr[0] = (uint8_t *) pstVideoFrame->stVFrame.u64PhyAddr[0];
				int data_len = pstChnAttr->u32Width;
				for (i = 0;i < ((pstChnAttr->u32Height * 3) >>1); ++i)
				{
					memcpy(packet_buffer_media + buf_len, pstVideoFrame->stVFrame.pu8VirAddr[0] + 
						buf_len_stride, data_len);
					buf_len += pstChnAttr->u32Width;
					buf_len_stride += pstVideoFrame->stVFrame.u32Stride[0];
				}
				pstVideoFrame->stVFrame.pu8VirAddr[0] = NULL;

				ret = CVI_VPSS_ReleaseChnFrame(UVC_VPSS_GRP, UVC_VPSS_CHN, pstVideoFrame);
				if(ret != CVI_SUCCESS)
					printf("CVI_VPSS_ReleaseChnFrame failed\n");
			}

			packets = usbd_video_payload_fill(packet_buffer_media, buf_len, packet_buffer_uvc, &out_len);

			buf_len = 0;
			buf_len_stride = 0;

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
        }else {
        	aos_msleep(1);
		}

    }
	
    return 0;
}

static struct usbd_endpoint video_in_ep;
static struct usbd_interface intf0;
static struct usbd_interface intf1;
static struct usb_descriptor_header **video_descriptor;

//static void mem_dump(void *address, uint32_t length) {
//    uint32_t width = 16;
//    uint32_t height = (length + width - 1) / width;
//    uint8_t *ptr = (uint8_t *)address;
//
//    printf("dump memory address(%p), length:%u.\r\n", address, length);
//    for (uint32_t i = 0; i < height; i++) {
//        for (uint32_t j = 0; j < width; j++) {
//            printf("%02x ", ptr[i * width + j]);
//        }
//        printf("\r\n");
//    }
//}

void uvc_video_init()
{
    uint8_t *real_video_dev_desc = NULL;

    video_descriptor = uvc_build_descriptors(uvc_format_info, ARRAY_SIZE(uvc_format_info));
    real_video_dev_desc = (uint8_t *)*video_descriptor;

    usbd_desc_register(real_video_dev_desc);
    usbd_add_interface(usbd_video_init_intf(&intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_endpoint(usbd_video_init_ep(&video_in_ep, VIDEO_IN_EP, NULL));
    usbd_video_register_uvc_callbacks(&uvc_evt_callbks);

    usbd_initialize();
}


int MEDIA_UVC_Init()
{
    char threadname[64] = {0};
	struct sched_param param;
	pthread_attr_t pthread_attr;
	pthread_t pthreadId = 0;

	uvc_video_init();

	// Wait until configured
	while (!usb_device_is_configured()) {
		aos_msleep(100);
	}
	
	uvc_session_init_flag = CVI_TRUE;

	param.sched_priority = 31;
	pthread_attr_init(&pthread_attr);
	pthread_attr_setschedpolicy(&pthread_attr, SCHED_RR);
	pthread_attr_setschedparam(&pthread_attr, &param);
	pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setstacksize(&pthread_attr, 6*1024);
	pthread_create(&pthreadId,&pthread_attr,send_to_uvc,NULL);
	snprintf(threadname,sizeof(threadname),"uvc_send%d",0);
	pthread_setname_np(pthreadId, threadname);

	return 0;
}

int MEDIA_UVC_DeInit()
{
	uvc_session_init_flag = CVI_FALSE;
	aos_msleep(100);
	usbd_deinitialize();

    uvc_destroy_descriptors(video_descriptor);
    video_descriptor = NULL;

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
