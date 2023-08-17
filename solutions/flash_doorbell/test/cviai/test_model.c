#include <aos/cli.h>
#include <cvi_sys.h>
#include <cvi_math.h>

#include <stdio.h>
// #include <ulog/ulog.h>
#include <aos/kernel.h>
#include "cviruntime.h"
#include "fatfs_vfs.h"

#define DEFAULT_INFERENCE_NUM 10

static void cvimodel_inference(const char *model_path, int inference_num) {
  printf("test model inference (path: %s)\n", model_path);
  CVI_MODEL_HANDLE model = NULL;
  int ret = CVI_NN_RegisterModel(model_path, &model);
  if (CVI_RC_SUCCESS != ret) {
  	printf("CVI_NN_RegisterModel failed, err %d\n", ret);
    return;
  }
  printf("CVI_NN_RegisterModel succeeded\n");

  // get input output tensors
  CVI_TENSOR *input_tensors;
  CVI_TENSOR *output_tensors;
  int32_t input_num;
  int32_t output_num;
  CVI_NN_GetInputOutputTensors(model, &input_tensors, &input_num, &output_tensors,
  &output_num);
  CVI_TENSOR *input = CVI_NN_GetTensorByName(CVI_NN_DEFAULT_TENSOR, input_tensors, input_num);
  assert(input);
  printf("input, name:%s\n", input->name);

  float qscale = CVI_NN_TensorQuantScale(input);
  printf("qscale:%f\n", qscale);
  CVI_SHAPE shape = CVI_NN_TensorShape(input);

  // nchw
  int32_t height = shape.dim[2];
  int32_t width = shape.dim[3];

  uint8_t *ptr = aos_malloc(3 * height * ALIGN(width, 64));

  // fill to input tensor
  CVI_VIDEO_FRAME_INFO frame;
  frame.type = CVI_FRAME_PLANAR;
  frame.shape.dim_size = 4;
  frame.shape.dim[0] = 1;
  frame.shape.dim[1] = 3;
  frame.shape.dim[2] = height;
  frame.shape.dim[3] = width;
  frame.fmt = CVI_FMT_INT8;
  for (int i = 0; i < 3; ++i) {
    frame.stride[i] = ALIGN(width, 64);
    frame.pyaddr[i] = (uint64_t)(ptr + height * frame.stride[i] * i);
  }

  int count = inference_num;
  while (--count > 0) {
    CVI_NN_FeedTensorWithFrames(model, input, CVI_FRAME_PLANAR,
    CVI_FMT_INT8, 3, frame.pyaddr, height, width, frame.stride[0]);

    CVI_NN_Forward(model, input_tensors, input_num, output_tensors, output_num);
    printf("CVI_NN_Forward succeeded!\n");
    sleep(1);
  }
  aos_free(ptr);
  CVI_NN_CleanupModel(model);
  printf("CVI_NN_CleanupModel succeeded\n");

  return;
}

void cviai_test_model(int32_t argc, char **argv) {
  if (argc != 3) {
    printf("usage: test_model <model_path> <inference_num>\n");
    return;
  }
  const char *model_name = argv[1];
  int inference_num = atoi(argv[2]);

  char model_path[128];
  sprintf(model_path, "%s/%s", SD_FATFS_MOUNTPOINT, model_name);

  cvimodel_inference(model_path, inference_num);
}

void cviai_test_fd_model(int32_t argc, char **argv) {
  const char *model_path = SD_FATFS_MOUNTPOINT"/retinaface_mnet0.25_608.cvimodel";
  cvimodel_inference(model_path, DEFAULT_INFERENCE_NUM);
}

void cviai_test_od_model(int32_t argc, char **argv) {
  const char *model_path = SD_FATFS_MOUNTPOINT"/yolox_tiny.cvimodel";
  cvimodel_inference(model_path, DEFAULT_INFERENCE_NUM);
}

void cviai_test_fr_model(int32_t argc, char **argv) {
  const char *model_path = SD_FATFS_MOUNTPOINT"/cviface-v5-s.cvimodel";
  cvimodel_inference(model_path, DEFAULT_INFERENCE_NUM);
}

ALIOS_CLI_CMD_REGISTER(cviai_test_model, cviai_test_model, test cvimodel inference);
ALIOS_CLI_CMD_REGISTER(cviai_test_fd_model, cviai_test_fd_model, test face detection model);
ALIOS_CLI_CMD_REGISTER(cviai_test_od_model, cviai_test_od_model, test object detection model);
ALIOS_CLI_CMD_REGISTER(cviai_test_fr_model, cviai_test_fr_model, test face recognition detection model);