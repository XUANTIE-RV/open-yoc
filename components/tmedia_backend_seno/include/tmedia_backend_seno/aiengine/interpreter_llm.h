/*
 * Copyright (C) 2024 Alibaba Group Holding Limited
 */

#ifndef TM_INTERPRETER_LLM_H
#define TM_INTERPRETER_LLM_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_set>
#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/aiengine/interpreter.h>


extern "C"
{
#include "llm/shl_llm.h"
}

class TMInterpreterLLM: public TMInterpreter
{
public:
    TMInterpreterLLM();
    ~TMInterpreterLLM();

    int Open(int idx);
    int Close();

    int LoadNet(const InterpreterNet *net);
    int UnLoadNet();
    int SetNetConfig(const NetConfig_t *cfg);
    int GetNetConfig(NetConfig_t *cfg);
    int GetPerfProfile(std::vector<std::string> &names, std::vector<uint32_t> &timings_us);

    int GetInputTensor(Tensor_t **input);
    int GetOutputTensor(Tensor_t **output);
    int SetInputTensor(const Tensor_t *input);
    int SetOutputTensor(const Tensor_t *output);
    int Run(int32_t timeout_ms);
    int ReleaseTensor(Tensor_t *tensor);

private:
    void *lib_handle;
    bool reset_flag;
    int mDev;
    int pos_start;
    Tensor_t *output_p;
    struct csinn_tensor *output_tensor;
    struct csinn_tensor *output_image_tensor;
    struct llm_config *conf;
    struct shl_llm_ctx *ctx;
    struct shl_llm_input *embd;

    struct llm_config *(*llm_conf_init)(const char *);
    struct shl_llm_ctx *(*llm_build)(struct llm_config *);
    int (*llm_run)(struct shl_llm_ctx *, struct shl_llm_input *);
    void (*llm_reset_cache)(struct llm_config *, struct shl_llm_ctx *);
    void (*llm_deinit)(struct llm_config *, struct shl_llm_ctx *);
};

#endif  /* TM_INTERPRETER_LLM_H */
