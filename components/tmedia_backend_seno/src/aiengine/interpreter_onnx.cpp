/*
 * Copyright (C) 2024 Alibaba Group Holding Limited
 */
#ifdef __linux__
#include <tmedia_config.h>
#endif
#ifdef CONFIG_TMEDIA_AIENGINE_INTERPRETER_ONNXRUNTIME  

#define DYNAMIC_OUT 0
#define SHOW_RUN_TIME 1

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <numeric>
#include <math.h>
#include <tmedia_core/entity/aiengine/aiengine_inc.h>
#include <tmedia_backend_seno/aiengine/interpreter_onnx.h>

#define ORT_LOG_ON_ERROR(expr)                             \
  do {                                                       \
    OrtStatus* onnx_status = (expr);                         \
    if (onnx_status != NULL) {                               \
      const char* msg = mOrt->GetErrorMessage(onnx_status); \
      LOG_E("%s\n", msg);                                    \
      mOrt->ReleaseStatus(onnx_status);                     \
    }                                                        \
  } while (0);

static inline uint32_t ElementSize(ONNXTensorElementDataType onnx_dtype)
{
    switch (onnx_dtype)
    {
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
        return 4;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:
        return 1;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
        return 2;
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED:
        LOG_E("Error: undifined onnx dtype\n");
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @brief 对onnxruntime 后端的配置。
 *        理论上支持 默认cpu后端，TH1520后端，C920后端,
 *        如果为空，则默认使用cpu后端。
 *        如果格式匹配失败，则返回空字符串，使用默认后端。
 * 
 * @param api_config 类似于"base_api:CSINN_C920, thread_num:2"
 * @return std::string 
 */
static inline std::string parse_api_config(std::string api_config)
{
   std::set<std::string> allowed_base_apis = {"CSINN_C920", "TH1520"};

    if (api_config.empty()) return std::string("");

    std::stringstream ss(api_config);
    std::string item;
    std::map<std::string, std::string> params;

    // 分割字符串，按逗号分割
    while (std::getline(ss, item, ','))
    {
        std::stringstream key_value_stream(item);
        std::string key, value;
        // 按冒号分割键值对
        if (std::getline(key_value_stream, key, ':') && std::getline(key_value_stream, value))
        {
            // 删除键值对中的空白字符
            key.erase(std::remove_if(key.begin(), key.end(), [](unsigned char c)
            {
                return std::isspace(c);
            }), key.end());
            value.erase(remove_if(value.begin(), value.end(), [](unsigned char c)
            {
                return std::isspace(c);
            }), value.end());
            params[key] = value;
        }
        else
        {
            // 分割失败，字符串格式不正确
            return std::string("");
        }
    }

    if (params.find("base_api") != params.end() && allowed_base_apis.find(params["base_api"]) == allowed_base_apis.end())
    {
        return std::string("");  // base_api的值不在允许的范围内
    }

    // 验证thread_num字段
    if (params.find("thread_num") != params.end())
    {
        int thread_num = std::stoi(params["thread_num"]); // 将字符串转换为整数
        if (thread_num < 1 || thread_num >= 8)
        {
            LOG_W("Warning: wrong thread num, use default config\n");
            return std::string("");  // thread_num的值不在允许的范围内
        }
    }

    return std::string(api_config);
}

TMInterpreterONNX::TMInterpreterONNX():mEnvName("onnx_env")
{

}

TMInterpreterONNX::~TMInterpreterONNX()
{

}

int TMInterpreterONNX::Open(int dev)
{
    // ASSERT_STATE(TMEntity::State::RESET);
    return TMResult::TM_OK;
}

int TMInterpreterONNX::Close()
{
    return TMResult::TM_OK;
}

int TMInterpreterONNX::LoadNet(const InterpreterNet *net)
{
    // ASSERT_STATE(TMEntity::State::RESET);
    if (net == NULL)
    {
        return TMResult::TM_EINVAL;
    }
    mOrt = const_cast<OrtApi *>(OrtGetApiBase()->GetApi(ORT_API_VERSION));
    if(mOrt == nullptr) {
        LOG_E("Error: can not get onnx api ptr\n");
        return TMResult::TM_EINVAL;
    }
    ORT_LOG_ON_ERROR(mOrt->CreateEnv(ORT_LOGGING_LEVEL_WARNING, mEnvName.c_str(), &mEnv));
    if(mEnv == nullptr)
    {
        LOG_E("Error: can not create onnx env\n");
        return TMResult::TM_EINVAL;
    }
    ORT_LOG_ON_ERROR(mOrt->CreateSessionOptions(&mSessionOptions));
    if(mSessionOptions == nullptr) {
        LOG_E("Error: can not create session options\n");
        return TMResult::TM_EINVAL;
    }
    mModelPath = net->model;

    return TMResult::TM_OK;
}

int TMInterpreterONNX::UnLoadNet()
{
    // ASSERT_STATE(TMEntity::State::READY);
    for (auto &name : mInputNames)
    {
        mAllocator->Free(mAllocator, name);
    }
    for (auto &name : mOutputName)
    {
        mAllocator->Free(mAllocator, name);
    }
    mOrt->ReleaseMemoryInfo(mMemoryInfo);
    mOrt->ReleaseSessionOptions(mSessionOptions);
    mOrt->ReleaseSession(mSession);
    mOrt->ReleaseEnv(mEnv);

    mOrt            = nullptr;
    mEnv            = nullptr;
    mSessionOptions = nullptr;
    mSession        = nullptr;
    mMemoryInfo     = nullptr;
    mAllocator      = nullptr;
    // SET_STATE(TMEntity::State::RESET);

    return TMResult::TM_OK;
}

int TMInterpreterONNX::SetNetConfig(const NetConfig_t *cfg)
{
    // ASSERT_STATE(TMEntity::State::RESET);
    if(cfg == nullptr) {
        LOG_E("Error: net config is nullptr\n");
        return TMResult::TM_EINVAL;        
    }

#ifdef PLATFORM_LIGHT
    std::string api_config = parse_api_config(cfg->base_api);
    if(api_config.empty())
    {
        // todo 由于默认cpu后端暂时存在bug，因此暂统一使用如下配置
        const char *api = "base_api:CSINN_C920, thread_num:3";
        LOG_E("ONNXRUNTIME default cpu backend is not supported temporaly, choose CSINN_C920, %s\n", api);
        ORT_LOG_ON_ERROR(mOrt->OrtSessionOptionsAppendExecutionProvider_Shl(mSessionOptions, api));
    }
    else
    {
        LOG_O("ONNXRUNTIME use %s\n", api_config.c_str());
        ORT_LOG_ON_ERROR(mOrt->OrtSessionOptionsAppendExecutionProvider_Shl(mSessionOptions, api_config.c_str()));
    }
#else
    LOG_O("Onnxruntime only support default cpu backend\n");
#endif

    ORT_LOG_ON_ERROR(mOrt->CreateSession(mEnv, mModelPath.c_str(), mSessionOptions, &mSession));
    ORT_LOG_ON_ERROR(mOrt->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &mMemoryInfo));
    ORT_LOG_ON_ERROR(mOrt->GetAllocatorWithDefaultOptions(&mAllocator));

    // Input
    ORT_LOG_ON_ERROR(mOrt->SessionGetInputCount(mSession, &mInputNums));
    LOG_O("input nums %ld\n", mInputNums);
    mInputTensors.resize(mInputNums, nullptr);
    mInputNames.resize(mInputNums, nullptr);
    for(size_t i = 0; i < mInputNums; i ++)
    {
        char *in_name;
        ORT_LOG_ON_ERROR(mOrt->SessionGetInputName(mSession, i, mAllocator, &in_name));
        mInputNames[i] = in_name;
    }
    size_t input_index = 0;
    OrtTypeInfo *typeinfo;
    ORT_LOG_ON_ERROR(mOrt->SessionGetInputTypeInfo(mSession, input_index, &typeinfo));
    const OrtTensorTypeAndShapeInfo *tensor_info;
    ORT_LOG_ON_ERROR(mOrt->CastTypeInfoToTensorInfo(typeinfo, &tensor_info));
    ORT_LOG_ON_ERROR(mOrt->GetTensorElementType(tensor_info, &mInputDtype));
    mOrt->ReleaseTypeInfo(typeinfo);

    // output
    ORT_LOG_ON_ERROR(mOrt->SessionGetOutputCount(mSession, &mOutputNums));
    mOutputName.resize(mOutputNums);
    mOutputDataSize.resize(mOutputNums);
    mOutputNodeDims.resize(mOutputNums);
    mOutputTensors.resize(mOutputNums);
    for (size_t i = 0; i < mOutputNums; i++)
    {
        // Get output node names
        char *output_name;
        ORT_LOG_ON_ERROR(mOrt->SessionGetOutputName(mSession, i, mAllocator, &output_name));
        mOutputName[i] = output_name;
#if DYNAMIC_OUT == 0
        ORT_LOG_ON_ERROR(mOrt->SessionGetOutputTypeInfo(mSession, i, &typeinfo));
        const OrtTensorTypeAndShapeInfo *tensor_info;
        ORT_LOG_ON_ERROR(mOrt->CastTypeInfoToTensorInfo(typeinfo, &tensor_info));

        // Get output shapes/dims
        size_t num_dims;
        ORT_LOG_ON_ERROR(mOrt->GetDimensionsCount(tensor_info, &num_dims));
        mOutputNodeDims[i].resize(num_dims);
        ORT_LOG_ON_ERROR(mOrt->GetDimensions(tensor_info, (int64_t *)mOutputNodeDims[i].data(), num_dims));

        size_t tensor_size;
        ORT_LOG_ON_ERROR(mOrt->GetTensorShapeElementCount(tensor_info, &tensor_size));
        mOutputDataSize[i] = tensor_size;
        mOrt->ReleaseTypeInfo(typeinfo);
#endif
    }
    // SET_STATE(TMEntity::State::READY);

    return TMResult::TM_OK;
}

int TMInterpreterONNX::GetNetConfig(NetConfig_t *cfg)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterONNX::GetPerfProfile(std::vector<std::string> &names, std::vector<uint32_t> &timings_us)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterONNX::GetInputTensor(Tensor_t **input)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterONNX::GetOutputTensor(Tensor_t **output)
{
    *output = new Tensor_t();
    mOTensor = *output;
    mOTensor->mtx_num = mOutputNums;
    mOTensor->mtx = new Matrix_t[mOTensor->mtx_num];
    for(int i = 0; i < mOTensor->mtx_num; i ++)
    {
        mOTensor->mtx[i].mdata.data = new void *[1]; // !only batch = 1
    }

    return TMResult::TM_OK;
}

int TMInterpreterONNX::SetInputTensor(const Tensor_t *input)
{
    if (input == nullptr || input->mtx_num <= 0 || input->mtx == nullptr)
    {
        LOG_E("Error: wrong input\n");
        return TMResult::TM_EINVAL;
    }
    if(input->mtx_num != (int)mInputNums)
    {
        LOG_E("Error: wrong input nums\n");
        return TMResult::TM_EINVAL;
    }
    for (int i = 0; i < input->mtx_num; ++i)
    {
        Matrix_t &matrix = input->mtx[i];
        // 计算输入的元素个数
        size_t total_elements = std::accumulate(matrix.mspec.dims.begin(), matrix.mspec.dims.end(), 1, std::multiplies<int>());
        // 确定数据类型和每个元素的大小
        size_t element_size = ElementSize(mInputDtype);
        if(element_size == 0) {
            LOG_E("Error: wrong modle input element size\n");
            return TMResult::TM_EINVAL;            
        }
        size_t total_size = total_elements * element_size;
        OrtValue **input_tensor = &mInputTensors[i];
        std::vector<int64_t> dims_int64(matrix.mspec.dims.size());
        std::copy(matrix.mspec.dims.begin(), matrix.mspec.dims.end(), dims_int64.begin());

        ORT_LOG_ON_ERROR(mOrt->CreateTensorWithDataAsOrtValue(
                             mMemoryInfo,
                             matrix.mdata.data[0],
                             total_size,
                             dims_int64.data(),
                             dims_int64.size(),
                             mInputDtype,
                             input_tensor));
        int is_tensor;
        ORT_LOG_ON_ERROR(mOrt->IsTensor(*input_tensor, &is_tensor));
    }

    return TMResult::TM_OK;
}

int TMInterpreterONNX::SetOutputTensor(const Tensor_t *output)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterONNX::Run(int32_t timeout)
{
    // ASSERT_STATE(TMEntity::State::READY);

    if(mOTensor == nullptr) {
        LOG_E("Error: output tensor need to be initialized by GetOutputTensor\n");
        return TMResult::TM_EINVAL;        
    }
    if(mInputTensors.size() == 0) {
        LOG_E("Error: output tensor need to be set by SetInputTensor\n");
        return TMResult::TM_EINVAL;        
    }
    for(size_t i = 0; i < mInputTensors.size(); i ++) {
        if(mInputTensors[i] == nullptr) {
            LOG_E("Error: output tensor need to be set by SetInputTensor\n");
            return TMResult::TM_EINVAL;  
        }
    }

#if SHOW_RUN_TIME == 1
    auto start = std::chrono::high_resolution_clock::now();
#endif

    ORT_LOG_ON_ERROR(mOrt->Run(mSession, NULL, mInputNames.data(), mInputTensors.data(),
                               mInputNames.size(), mOutputName.data(), mOutputName.size(), mOutputTensors.data()));

#if SHOW_RUN_TIME == 1
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    LOG_O("ONNX Run time is %ld ms\n", duration);
#endif

#if DYNAMIC_OUT == 0
    for (size_t i = 0; i < mOutputNums; i++) {
        auto output_tensor = mOutputTensors[i];
        void* output_tensor_data = NULL;
        ORT_LOG_ON_ERROR(mOrt->GetTensorMutableData(output_tensor, (void**)&output_tensor_data));

        Matrix_t& matrix = mOTensor->mtx[i];
        matrix.mdata.data[0] = output_tensor_data;
    }
#else
    for (size_t i = 0; i < mOutputNums; ++i) {
        OrtValue* output_tensor = mOutputTensors[i];
        OrtTensorTypeAndShapeInfo* tensor_info;
        ORT_LOG_ON_ERROR(mOrt->GetTensorTypeAndShape(output_tensor, &tensor_info));
        size_t num_dims;
        ORT_LOG_ON_ERROR(mOrt->GetDimensionsCount(tensor_info, &num_dims));
        // 获取动态张量形状
        std::vector<int64_t> output_dims(num_dims);
        ORT_LOG_ON_ERROR(mOrt->GetDimensions(tensor_info, output_dims.data(), num_dims));
        mOrt->ReleaseTensorTypeAndShapeInfo(tensor_info);
        Matrix_t &matrix = mOTensor->mtx[i];
        for(size_t d = 0; d < num_dims; d ++)
        {
            matrix.mspec.dims[d] = (int)output_dims[d];
        }
        // 获取输出张量数据
        void *output_tensor_data = nullptr;
        ORT_LOG_ON_ERROR(mOrt->GetTensorMutableData(output_tensor, (void **)&output_tensor_data));
        matrix.mdata.data[0] = output_tensor_data;
    }
#endif

    for(size_t i = 0; i < mInputNums; i ++)
    {
        mOrt->ReleaseValue(mInputTensors[i]);
        mInputTensors[i] = nullptr;
    }
    // SET_STATE(TMEntity::State::RUNNING);

    return TMResult::TM_OK;
}

// ! only used for output
int TMInterpreterONNX::ReleaseTensor(Tensor_t *tensor)
{
    // NO_HANDLE_STATE(TMEntity::State::READY);
    // ASSERT_STATE(TMEntity::State::RUNNING);
    for(size_t i = 0; i < mOutputNums; i ++)
    {
        if(mOutputTensors[i] != nullptr)
        {
            mOrt->ReleaseValue(mOutputTensors[i]);
            mOutputTensors[i] = nullptr;
        }
    }
    if (tensor == NULL)
    {
        LOG_O("Info: released output tensor is nullptr\n");
        return TMResult::TM_OK;
    }
    for(int i = 0; i < tensor->mtx_num; i ++)
    {
        delete[] tensor->mtx[i].mdata.data;
    }
    delete[] tensor->mtx;
    delete tensor;
    // SET_STATE(TMEntity::State::READY);

    return TMResult::TM_OK;
}

REGISTER_INTERPRETER_CLASS(TMInterpreterONNX);

#endif