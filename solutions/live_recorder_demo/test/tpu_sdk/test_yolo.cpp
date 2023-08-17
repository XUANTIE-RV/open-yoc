#include <stdio.h>

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "vfs.h"
#include "fatfs_vfs.h"
#include "aos/cli.h"

#include "cviruntime.h"
#include "cvi_vb.h"
#include "cvi_sys.h"
#include "cvi_buffer.h"
#include "cvi_vpss.h"
#include "cvi_tpu_interface.h"

#include "ScopTime.hpp"

using namespace std;

#define USE_VPSS 1

typedef struct {
  float x, y, w, h;
} box;

typedef struct {
  box bbox;
  int cls;
  float score;
  int batch_idx;
} detection;

typedef struct {
  CVI_TENSOR *output;
  int num_anchors, h, w, bbox_len, batch = 1, layer_idx;
} detectLayer;

static const char *coco_names[] = {
    "person",        "bicycle",       "car",           "motorbike",
    "aeroplane",     "bus",           "train",         "truck",
    "boat",          "traffic light", "fire hydrant",  "stop sign",
    "parking meter", "bench",         "bird",          "cat",
    "dog",           "horse",         "sheep",         "cow",
    "elephant",      "bear",          "zebra",         "giraffe",
    "backpack",      "umbrella",      "handbag",       "tie",
    "suitcase",      "frisbee",       "skis",          "snowboard",
    "sports ball",   "kite",          "baseball bat",  "baseball glove",
    "skateboard",    "surfboard",     "tennis racket", "bottle",
    "wine glass",    "cup",           "fork",          "knife",
    "spoon",         "bowl",          "banana",        "apple",
    "sandwich",      "orange",        "broccoli",      "carrot",
    "hot dog",       "pizza",         "donut",         "cake",
    "chair",         "sofa",          "pottedplant",   "bed",
    "diningtable",   "toilet",        "tvmonitor",     "laptop",
    "mouse",         "remote",        "keyboard",      "cell phone",
    "microwave",     "oven",          "toaster",       "sink",
    "refrigerator",  "book",          "clock",         "vase",
    "scissors",      "teddy bear",    "hair drier",    "toothbrush"};

static float anchors_[3][3][2] = {{{10, 13}, {16, 30}, {33, 23}},
                                  {{30, 61}, {62, 45}, {59, 119}},
                                  {{116, 90}, {156, 198}, {373, 326}}};

template <typename T>
int argmax(const T *data,
          size_t len,
          size_t stride = 1)
{
	int maxIndex = 0;
	for (size_t i = 1; i < len; i++)
	{
		int idx = i * stride;
		if (data[maxIndex * stride] < data[idx])
		{
			maxIndex = i;
		}
	}
	return maxIndex;
}

static float sigmoid(float x)
{
  return 1.0 / (1 + std::exp(-x));
}

float calIou(box a, box b)
{
  float area1 = a.w * a.h;
  float area2 = b.w * b.h;
  float wi = std::min((a.x + a.w / 2), (b.x + b.w / 2)) - std::max((a.x - a.w / 2), (b.x - b.w / 2));
  float hi = std::min((a.y + a.h / 2), (b.y + b.h / 2)) - std::max((a.y - a.h / 2), (b.y - b.h / 2));
  float area_i = std::max(wi, 0.0f) * std::max(hi, 0.0f);
  return area_i / (area1 + area2 - area_i);
}

void correctYoloBoxes(std::vector<detection> dets,
                      int det_num,
                      int image_h,
                      int image_w,
                      int input_height,
                      int input_width) {
    int restored_w;
    int restored_h;
    float resize_ratio;
    if (((float)input_width / image_w) < ((float)input_height / image_h)) {
        restored_w = input_width;
        restored_h = (image_h * input_width) / image_w;
    } else {
        restored_h = input_height;
        restored_w = (image_w * input_height) / image_h;
    }
    resize_ratio = ((float)image_w / restored_w);

    for (int i = 0; i < det_num; ++i) {
        box bbox = dets[i].bbox;
        //int b    = dets[i].batch_idx;
        bbox.x   = (bbox.x - (input_width - restored_w) / 2.) * resize_ratio;
        bbox.y   = (bbox.y - (input_height - restored_h) / 2.) * resize_ratio;
        bbox.w *= resize_ratio;
        bbox.h *= resize_ratio;
        dets[i].bbox = bbox;
    }
}

void NMS(std::vector<detection> &dets, int *total, float thresh)
{
  if (*total){
    std::sort(dets.begin(), dets.end(), [](detection &a, detection &b)
              { return b.score < a.score; });
    int new_count = *total;
    for (int i = 0; i < *total; ++i)
    {
      detection &a = dets[i];
      if (a.score == 0)
        continue;
      for (int j = i + 1; j < *total; ++j)
      {
        detection &b = dets[j];
        if (dets[i].batch_idx == dets[j].batch_idx &&
            b.score != 0 && dets[i].cls == dets[j].cls &&
            calIou(a.bbox, b.bbox) > thresh)
        {
          b.score = 0;
          new_count--;
        }
      }
    }
    for (int i = 0, j = 0 ; i < *total && j < new_count; ++i) {
      detection &a = dets[i];
      if (a.score == 0)
        continue;
      dets[j] = dets[i];
      ++j;
    }
    *total = new_count;
  }
}

/**
 * @brief
 *
 * @note work as long as output shape [n, a, h, w, cls + 5]
 * @param layer
 * @param input_height 
 * @param input_width
 * @param classes_num
 * @param conf_thresh
 * @param dets
 * @return int
 */
int getDetections(detectLayer *layer,
                  int32_t input_height,
                  int32_t input_width,
                  int classes_num,
                  float conf_thresh,
                  std::vector<detection> &dets) {
    CVI_TENSOR *output = layer->output;
    float *output_ptr  = (float *)CVI_NN_TensorPtr(output);
    int count          = 0;
    int w_stride       = layer->bbox_len;
    int h_stride       = layer->w * w_stride;
    int a_stride       = layer->h * h_stride;
    int b_stride       = layer->num_anchors * a_stride;
    float down_stride  = input_width / layer->w;
    for (int b = 0; b < layer->batch; b++) {
        for (int a = 0; a < layer->num_anchors; ++a) {
            for (int i = 0; i < layer->w * layer->h; ++i) {
                int col          = i % layer->w;
                int row          = i / layer->w;
                float *obj       = output_ptr + b * b_stride + a * a_stride + row * h_stride + col * w_stride + 4;
                float objectness = sigmoid(obj[0]);
                float *scores    = obj + 1;
                int category     = argmax(scores, classes_num);
                objectness *= sigmoid(scores[category]);

                if (objectness <= conf_thresh) {
                    continue;
                }

                float x               = *(obj - 4);
                float y               = *(obj - 3);
                float w               = *(obj - 2);
                float h               = *(obj - 1);
                dets.emplace_back();
                detection &det = dets.back();
                det.score     = objectness;
                det.cls       = category;
                det.batch_idx = b;

                det.bbox.x = (sigmoid(x) * 2 + col - 0.5) * down_stride;
                det.bbox.y = (sigmoid(y) * 2 + row - 0.5) * down_stride;
                det.bbox.w = pow(sigmoid(w) * 2, 2) * anchors_[layer->layer_idx][a][0];
                det.bbox.h = pow(sigmoid(h) * 2, 2) * anchors_[layer->layer_idx][a][1];

                ++count;
            }
        }
    }
    return count;
}

#ifndef USE_VPSS
static int BGRPacked2RBGPlanar(uint8_t * packed, uint8_t* planar, int height, int width) {
  uint8_t *p_img = (uint8_t*)packed;
  uint8_t *p_r = planar + height*width * 2;
  uint8_t *p_g = planar + height*width;
  uint8_t *p_b = planar;

  for (int i = 0; i<height*width; i++) {
    *p_r++ = *p_img++;
    *p_g++ = *p_img++;
    *p_b++ = *p_img++;
  }
  return 0;
}

static void  load_file(const char * file_name, void **ptr, size_t *size) {
  int file_fd = aos_open(file_name, O_RDONLY);
  if (file_fd < 0) {
    printf("open label file failed %s\n", file_name);
    exit(1);
  }
  struct aos_stat label_stat;
  int ret = aos_stat(file_name, &label_stat);
  if (ret < 0) {
    printf("get file stat failed\n");
    exit(1);
  }
  if (*size <= 0) {
    *ptr = malloc(label_stat.st_size);
    *size = label_stat.st_size;
  }
  size_t r_size = aos_read(file_fd, *ptr, *size);
  if (r_size != *size) {
    printf("load file error\n");
    assert(0);
  }
  aos_close(file_fd);
  return;
}
#else

static int vpss_init() {
  //VPSS_GRP_ATTR_S stVpssGrpAttr;
  VPSS_CHN_ATTR_S stVpssChnAttr;
  VPSS_GRP VpssGrp = 0;
  VPSS_CHN VpssChn = 0;
  //MMF_CHN_S stSrcChn;
  //MMF_CHN_S stDestChn;
  //CVI_S32 s32Ret;

#if 0
  stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
  stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
  stVpssGrpAttr.enPixelFormat               = PIXEL_FORMAT_NV21;
  stVpssGrpAttr.u32MaxW                     = 1920;
  stVpssGrpAttr.u32MaxH                     = 1080;
  stVpssGrpAttr.u8VpssDev                   = 0;
#endif

  stVpssChnAttr.u32Width                    = 640;
  stVpssChnAttr.u32Height                   = 640;
  stVpssChnAttr.enVideoFormat               = VIDEO_FORMAT_LINEAR;
  stVpssChnAttr.enPixelFormat               = PIXEL_FORMAT_RGB_888_PLANAR;
  stVpssChnAttr.stFrameRate.s32SrcFrameRate = -1;
  stVpssChnAttr.stFrameRate.s32DstFrameRate = -1;
  stVpssChnAttr.u32Depth                    = 0;
  stVpssChnAttr.bMirror                     = CVI_FALSE;
  stVpssChnAttr.bFlip                       = CVI_FALSE;
  stVpssChnAttr.stNormalize.bEnable         = CVI_FALSE;
  stVpssChnAttr.stAspectRatio.enMode        = ASPECT_RATIO_AUTO;
  stVpssChnAttr.stAspectRatio.bEnableBgColor = CVI_TRUE;
  stVpssChnAttr.stAspectRatio.u32BgColor    = 0x00000000;

  //CVI_VPSS_CreateGrp(VpssGrp, &stVpssGrpAttr);
  CVI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
  CVI_VPSS_EnableChn(VpssGrp, VpssChn);
  //CVI_VPSS_StartGrp(VpssGrp);

#if 0
  stSrcChn.enModId  = CVI_ID_VI;
  stSrcChn.s32DevId = 0;
  stSrcChn.s32ChnId = 0;

  stDestChn.enModId  = CVI_ID_VPSS;
  stDestChn.s32DevId = VpssGrp;
  stDestChn.s32ChnId = 0;

  s32Ret = CVI_SYS_Bind(&stSrcChn, &stDestChn);
  if (s32Ret == 0) {
      printf("*********VI bind VPSS Sucessful******** \n");
  } else {
      printf("*********VI bind VPSS Failed************\n");
  }
#endif
  return CVI_SUCCESS;
}

#define FILENAME_MAX_LEN (128)
static void dump_vpss_frame(VIDEO_FRAME_INFO_S *frm, const char *name) {
  char filename[FILENAME_MAX_LEN] = {0};
  const char *extension           = NULL;
  if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) {
    extension = "yuv";
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_NV12) {
    extension = "nv12";
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_NV21) {
    extension = "nv21";
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_RGB_888_PLANAR || frm->stVFrame.enPixelFormat == PIXEL_FORMAT_BGR_888_PLANAR) {
    extension = "chw";
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_RGB_888) {
    extension = "rgb";
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_BGR_888) {
    extension = "bgr";
  } else {
    printf("Invalid frm pixel format %d\n",
           frm->stVFrame.enPixelFormat);
    return;
  }
  snprintf(filename, FILENAME_MAX_LEN, SD_FATFS_MOUNTPOINT"/%s_%dX%d.%s", name,
           frm->stVFrame.u32Width,
           frm->stVFrame.u32Height,
           extension);

  int file_fd = aos_open(filename, O_CREAT | O_RDWR | O_TRUNC);
  if (file_fd < 0) {
    printf("open file failed! file:%s\n", filename);
    return;
  }

  printf("Save %s, w*h(%d*%d)\n",
         filename,
         frm->stVFrame.u32Width,
         frm->stVFrame.u32Height);

  if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) {
    for (int i = 0; i < 3; ++i) {
      printf("  plane(%d): paddr(0x%lx) vaddr(%p) stride(%d) length(%d)\n",
               i,
               frm->stVFrame.u64PhyAddr[i],
               frm->stVFrame.pu8VirAddr[i],
               frm->stVFrame.u32Stride[i],
               frm->stVFrame.u32Length[i]);
      uint32_t length = (i == 0 ? frm->stVFrame.u32Height : frm->stVFrame.u32Height / 2);
      uint32_t step   = (i == 0 ? frm->stVFrame.u32Width : frm->stVFrame.u32Width / 2);
      uint8_t *ptr    = (uint8_t *)frm->stVFrame.u64PhyAddr[i];
      for (unsigned j = 0; j < length; ++j) {
        aos_write(file_fd, ptr, step);
        ptr += frm->stVFrame.u32Stride[i];
      }
    }
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_NV12 || frm->stVFrame.enPixelFormat == PIXEL_FORMAT_NV21) {
    for (int i = 0; i < 2; ++i) {
      printf("  plane(%d): paddr(0x%lx) vaddr(%p) stride(%d) length(%d)\n",
               i,
               frm->stVFrame.u64PhyAddr[i],
               frm->stVFrame.pu8VirAddr[i],
               frm->stVFrame.u32Stride[i],
               frm->stVFrame.u32Length[i]);
      uint32_t length = (i == 0 ? frm->stVFrame.u32Height : frm->stVFrame.u32Height / 2);
      uint32_t step   = frm->stVFrame.u32Width;
      uint8_t *ptr    = (uint8_t *)frm->stVFrame.u64PhyAddr[i];
      for (unsigned j = 0; j < length; ++j) {
        aos_write(file_fd, ptr, step);
        ptr += frm->stVFrame.u32Stride[i];
      }
    }
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_RGB_888_PLANAR || frm->stVFrame.enPixelFormat == PIXEL_FORMAT_BGR_888_PLANAR) {
    for (int i = 0; i < 3; i++) {
      printf("  plane(%d): paddr(0x%lx) vaddr(%p) stride(%d) length(%d)\n",
               i,
               frm->stVFrame.u64PhyAddr[i],
               frm->stVFrame.pu8VirAddr[i],
               frm->stVFrame.u32Stride[i],
               frm->stVFrame.u32Length[i]);
      uint8_t *ptr = (uint8_t *)frm->stVFrame.u64PhyAddr[i];
      for (unsigned j = 0; j < frm->stVFrame.u32Height; ++j) {
        aos_write(file_fd, ptr, frm->stVFrame.u32Width);
        ptr += frm->stVFrame.u32Stride[i];
      }
    }
  } else if (frm->stVFrame.enPixelFormat == PIXEL_FORMAT_RGB_888 || frm->stVFrame.enPixelFormat == PIXEL_FORMAT_BGR_888) {
    printf("  packed: paddr(0x%lx) vaddr(%p) stride(%d) length(%d)\n",
             frm->stVFrame.u64PhyAddr[0],
             frm->stVFrame.pu8VirAddr[0],
             frm->stVFrame.u32Stride[0],
             frm->stVFrame.u32Length[0]);
    uint8_t *ptr = (uint8_t *)frm->stVFrame.u64PhyAddr[0];
    for (unsigned j = 0; j < frm->stVFrame.u32Height; ++j) {
      aos_write(file_fd, ptr, frm->stVFrame.u32Width * 3);
      ptr += frm->stVFrame.u32Stride[0];
    }
  } else {
    printf("Invalid frm pixel format %d\n", frm->stVFrame.enPixelFormat);
  }

  aos_sync(file_fd);
  aos_close(file_fd);
}
#endif

int test_yolo(int argc, char **argv) {
  int ret = 0;
  CVI_MODEL_HANDLE model;

  CVI_TENSOR *input;
  CVI_TENSOR *output;
  CVI_TENSOR *input_tensors;
  CVI_TENSOR *output_tensors;
  int32_t input_num;
  int32_t output_num;
  CVI_SHAPE input_shape;
  std::vector<CVI_SHAPE> output_shape;
  int32_t height;
  int32_t width;
  int bbox_len = 85; // classes num + 5
  int classes_num = 80;
  float conf_thresh = 0.5;
  float iou_thresh = 0.5;

  if (argc != 2) {
    printf("invailed param\n usage: %s [count]\n", argv[0]);
    return -1;
  }

  cvi_tpu_init();
  int count = std::max(atoi(argv[1]), 1);
#ifdef USE_VPSS
  if (CVI_SUCCESS != vpss_init()) {
    printf("init vpss failed!\n");
  }
  /*********************************************************************************************
   * If use vpss frame as model's input, you need add "--fuse_preprocess --align_input true"
   * when call model_deploy.py
   * model_deploy.py --model_name yolo_v5_s 
   *                 --mlir yolo_v5_s_fp32.mlir 
   *                 --calibration_table yolo_v5_s_calibration_table
   *                 --image dog.jpg 
   *                 --chip cv181x
   *                 --tolerance 0.97,0.97,0.76 
   *                 --fuse_preprocess 
   *                 --pixel_format RGB_PLANAR 
   *                 --aligned_input 1 
   *                 --correctness 0.99,0.99,0.99 
   *                 --cvimodel yolov5s_fused_preprocess_aligned_input.cvimodel
   *********************************************************************************************/
  const char *model_file = SD_FATFS_MOUNTPOINT"/yolov5s_fused_preprocess_aligned_input.cvimodel";
#else
  /*********************************************************************************************
   * model_deploy.py --model_name yolo_v5_s 
   *                 --mlir yolo_v5_s_fp32.mlir 
   *                 --calibration_table yolo_v5_s_calibration_table
   *                 --chip cv181x
   *                 --image dog.jpg 
   *                 --tolerance 0.97,0.97,0.76 
   *                 --fuse_preprocess 
   *                 --pixel_format RGB_PLANAR 
   *                 --correctness 0.99,0.99,0.99 
   *                 --cvimodel yolov5s_fused_preprocess.cvimodel
   *********************************************************************************************/
  const char *model_file = SD_FATFS_MOUNTPOINT"/yolov5s_fused_preprocess.cvimodel";
#endif

  ret = CVI_NN_RegisterModel(model_file, &model);
  if (ret != CVI_RC_SUCCESS) {
    printf("CVI_NN_RegisterModel failed, err %d\n", ret);
    return -1;
  }
  printf("CVI_NN_RegisterModel succeeded! model file:%s\n", model_file);

  // get input output tensors
  CVI_NN_GetInputOutputTensors(model, &input_tensors, &input_num, &output_tensors,
                               &output_num);

  input = CVI_NN_GetTensorByName(CVI_NN_DEFAULT_TENSOR, input_tensors, input_num);
  assert(input);
  output = output_tensors;
  
  // nchw
  input_shape = CVI_NN_TensorShape(input);
  height = input_shape.dim[2];
  width = input_shape.dim[3];

  for (int i = 0; i < output_num; i++) {
    output_shape.emplace_back(CVI_NN_TensorShape(&output[i]));
  }

  while (count--) {
#ifdef USE_VPSS
    int Grp = 0;
    int Chn = 0;
    VIDEO_FRAME_INFO_S stFrameInfo;
    CVI_S32 s32MilliSec = 1000;
    if (CVI_VPSS_GetChnFrame(Grp, Chn, &stFrameInfo, s32MilliSec) != CVI_SUCCESS) {
      printf("Get frame fail \n");
      return -1;
    }
    printf("get frame success! pixel_format:%d width:%d height:%d\n",
           stFrameInfo.stVFrame.enPixelFormat,
           stFrameInfo.stVFrame.u32Width,
           stFrameInfo.stVFrame.u32Height);
    for (int i = 0; i < 3; ++i) {
      printf("plane(%d): paddr(%lx) vaddr(%p) stride(%d)\n",
             i, stFrameInfo.stVFrame.u64PhyAddr[i],
             stFrameInfo.stVFrame.pu8VirAddr[i],
             stFrameInfo.stVFrame.u32Stride[i]);
    }
    if (count == 0) {
      dump_vpss_frame(&stFrameInfo, "frame");
    }
    CVI_NN_SetTensorPhysicalAddr(input, stFrameInfo.stVFrame.u64PhyAddr[0]);
#else
    // read image
    const char *img_name = SD_FATFS_MOUNTPOINT"/dog_640x640.bgr";
    uint8_t *ptr = NULL;
    size_t img_size = 0;
    load_file(img_name, (void **)&ptr, &img_size);
    assert(img_size == (size_t)(3 * width * height));
    printf("read img file success\n");

    uint8_t *ptr_planar = (uint8_t *)malloc(img_size);
    BGRPacked2RBGPlanar(ptr, ptr_planar, height, width);
    memcpy(CVI_NN_TensorPtr(input), ptr_planar, CVI_NN_TensorSize(input));
    free(ptr);
    free(ptr_planar);
#endif

  // run inference
    {
      ScopeTimer st("forward");
      ret = CVI_NN_Forward(model, input_tensors, input_num, output_tensors, output_num);
      if (ret != CVI_RC_SUCCESS) {
        printf("cvimodel forward failed! ret:%d\n", ret);
        return -1;
      }
    }

#ifdef USE_VPSS
    if (CVI_VPSS_ReleaseChnFrame(Grp, Chn, &stFrameInfo) != CVI_SUCCESS)
      printf("CVI_VPSS_ReleaseChnFrame fail\n");
#endif

    // do post proprocess
    int det_num = 0;
    int count = 0;
    std::vector<detection> dets;
    std::vector<detectLayer> layers;

    //int stride[3] = {8, 16, 32};
    // for each detect layer
    for (int i = 0; i < output_num; i++)
    {
      // layer init
      detectLayer layer;
      layer.output = &output[i];
      layer.bbox_len = bbox_len;
      layer.num_anchors = output_shape[i].dim[1];
      layer.h = output_shape[i].dim[2];
      layer.w = (int)(output_shape[i].dim[3] / bbox_len);
      layer.layer_idx = i;
      layers.push_back(layer);

      count = getDetections(&layer, height, width,
                            classes_num, conf_thresh, dets);
      det_num += count;
    }
    // correct box with origin image size
    printf("raw detection num: %d\n", det_num);
    NMS(dets, &det_num, iou_thresh);
    //correctYoloBoxes(dets, det_num, height, width, height, width);
    printf("get detection num: %d\n", det_num);

    // print bbox
    for (int i = 0; i < det_num; i++) {
      box b = dets[i].bbox;
      // xywh2xyxy
      int x1 = (b.x - b.w / 2);
      int y1 = (b.y - b.h / 2);
      int x2 = (b.x + b.w / 2);
      int y2 = (b.y + b.h / 2);
      printf("det object:[xywh:%d, %d, %d, %d] name:%s  confidence:%f\n", x1, y1, x2 - x1, y2 - y1, coco_names[dets[i].cls], dets[i].score);
    }

    printf("------\n");
    printf("%d objects are detected\n", det_num);
    printf("------\n");
  }

  CVI_NN_CleanupModel(model);
  printf("CVI_NN_CleanupModel succeeded\n");
  return 0;
} 

ALIOS_CLI_CMD_REGISTER(test_yolo, test_yolo, test tpu sdk);
