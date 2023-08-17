/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_INTERPRETER_CSINN_H
#define TM_INTERPRETER_CSINN_H

#include <cstdint>
#include <vector>
#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/aiengine/interpreter.h>

extern "C"
{
#include "csinn/csi_nn.h"
#include "csinn/csinn_runtime.h"
#include "csinn/shl_utils.h"
}

class InterpreterCSINN: public Interpreter
{
public:
    InterpreterCSINN();
    ~InterpreterCSINN();

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
    int mDev;
    struct csinn_session *mSess;
    const Tensor_t *mOutput;
    std::vector<char> mData;
    void *lib_handle;
};

#endif  /* TM_INTERPRETER_CSINN_H */
