#include <stdio.h>

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>

#include "aos/cli.h"
#include "fatfs_vfs.h"

#include "cviruntime.h"
#include "cvi_tpu_interface.h"

#include "ScopTime.hpp"

using namespace std;

int test_cvimodel(int argc, char **argv) {
  int ret = 0;
  CVI_MODEL_HANDLE model;

  CVI_TENSOR *input_tensors;
  CVI_TENSOR *output_tensors;
  int32_t input_num;
  int32_t output_num;

  if (argc != 2) {
    printf("invailed param\n usage: %s [model_file]\n", argv[0]);
    return -1;
  }

  std::stringstream model_file_name;
  model_file_name << SD_FATFS_MOUNTPOINT << "/" << argv[1];
  cvi_tpu_init();
  ret = CVI_NN_RegisterModel(model_file_name.str().c_str(), &model);
  if (ret != CVI_RC_SUCCESS) {
    printf("CVI_NN_RegisterModel failed, err %d\n", ret);
    return -1;
  }
  printf("CVI_NN_RegisterModel succeeded! model file:%s\n", model_file_name.str().c_str());

  // get input output tensors
  CVI_NN_GetInputOutputTensors(model, &input_tensors, &input_num, &output_tensors,
                               &output_num);

  // run inference
  {
    ScopeTimer st("forward");
    ret = CVI_NN_Forward(model, input_tensors, input_num, output_tensors, output_num);
    if (ret != CVI_RC_SUCCESS) {
      printf("cvimodel forward failed! ret:%d\n", ret);
      return -1;
    }
  }

  CVI_NN_CleanupModel(model);
  printf("CVI_NN_CleanupModel succeeded\n");
  return 0;
} 

ALIOS_CLI_CMD_REGISTER(test_cvimodel, test_cvimodel, test cvimodel);