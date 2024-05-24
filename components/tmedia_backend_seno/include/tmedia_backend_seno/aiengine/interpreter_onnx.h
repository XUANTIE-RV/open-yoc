/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_INTERPRETER_ONNX_H
#define TM_INTERPRETER_ONNX_H

#include <cstdint>
#include <vector>
#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/aiengine/interpreter.h>

extern "C"
{
#include <onnxruntime/core/session/onnxruntime_c_api.h>
}

class TMInterpreterONNX: public TMInterpreter
{
public:
    TMInterpreterONNX();
    ~TMInterpreterONNX();

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
    const std::string mEnvName;
    std::string mModelPath;
    OrtApi* mOrt;
    OrtEnv* mEnv;
    OrtSessionOptions* mSessionOptions;
    OrtSession* mSession;
    OrtMemoryInfo* mMemoryInfo;
    OrtAllocator* mAllocator;
    size_t mInputNums;
    ONNXTensorElementDataType mInputDtype;
    std::vector<OrtValue *> mInputTensors;
    std::vector<char *> mInputNames;
    size_t mOutputNums;
    std::vector<OrtValue *> mOutputTensors;
    std::vector<char*> mOutputName;
    std::vector<size_t> mOutputDataSize;
    std::vector<std::vector<int64_t>> mOutputNodeDims;
    Tensor_t *mOTensor;
};

#endif  /* TM_INTERPRETER_ONNX_H */
