/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifdef __linux__
#include <tmedia_config.h>
#endif
#ifdef CONFIG_TMEDIA_AIENGINE_INTERPRETER_CSINN
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <tmedia_core/entity/aiengine/aiengine_inc.h>
#include <tmedia_backend_seno/aiengine/interpreter_csinn.h>

static inline TMInterpreter::DataType_e DataType_CSI2AIE(int32_t csi_dtype)
{
    switch (csi_dtype)
    {
    case CSINN_DTYPE_INT8:
        return TMInterpreter::DATA_8S;
    case CSINN_DTYPE_UINT8:
        return TMInterpreter::DATA_8U;
    case CSINN_DTYPE_INT16:
        return TMInterpreter::DATA_16S;
    case CSINN_DTYPE_UINT16:
        return TMInterpreter::DATA_16U;
    case CSINN_DTYPE_INT32:
        return TMInterpreter::DATA_32S;
    case CSINN_DTYPE_UINT32:
        return TMInterpreter::DATA_32U;
    case CSINN_DTYPE_FLOAT16:
        return TMInterpreter::DATA_16F;
    case CSINN_DTYPE_FLOAT32:
        return TMInterpreter::DATA_32F;
    case CSINN_DTYPE_FLOAT64:
        return TMInterpreter::DATA_64F;
    case CSINN_DTYPE_INT64:
        return TMInterpreter::DATA_64S;
    default:
        break;
    }
    return TMInterpreter::DATA_8U;
}

static inline csinn_mem_type_enum MemType_AIE2CSI(TMInterpreter::MemType_e aie_mtype)
{
    switch (aie_mtype)
    {
    case TMInterpreter::MEM_NORMAL:
        return CSINN_MEM_TYPE_CPU_NOT_ALIGNED;
    case TMInterpreter::MEM_DMABUF:
        return CSINN_MEM_TYPE_DMABUF;
    default:
        break;
    }
    return CSINN_MEM_TYPE_CPU_NOT_ALIGNED;
}

static inline uint32_t DataTypeElemSize(int32_t csi_dtype)
{
switch (csi_dtype)
    {
    case CSINN_DTYPE_INT8:
    case CSINN_DTYPE_UINT8:
        return 1;
    case CSINN_DTYPE_INT16:
    case CSINN_DTYPE_UINT16:
    case CSINN_DTYPE_FLOAT16:
        return 2;
    case CSINN_DTYPE_INT32:
    case CSINN_DTYPE_UINT32:
    case CSINN_DTYPE_FLOAT32:
        return 4;
    case CSINN_DTYPE_FLOAT64:
    case CSINN_DTYPE_INT64:
        return 8;
    default:
        break;
    }
    return 1;
}

template <typename T>
T ProductVector(std::vector<T> &vec)
{
    T res = 1;
    for (size_t i = 0; i < vec.size(); i++)
    {
        res *= vec[i];
    }
    return res;
}

TMInterpreterCSINN::TMInterpreterCSINN()
{
    mDev = -1;
    mSess = NULL;
    mOutput = NULL;
    lib_handle = NULL;
}

TMInterpreterCSINN::~TMInterpreterCSINN()
{
}

int TMInterpreterCSINN::Open(int dev)
{
    mDev = dev;
    return TMResult::TM_OK;
}

int TMInterpreterCSINN::Close()
{
    return TMResult::TM_OK;
}

int TMInterpreterCSINN::LoadNet(const InterpreterNet *net)
{
    if (net == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    std::string suffix_bm = net->weights.substr(net->weights.length() - 3); // .bm

    lib_handle = dlopen(net->model.c_str(), RTLD_NOW);
    if (!lib_handle)
    {
        printf("dlopen failed:%s lib:%s\n", dlerror(), net->model.c_str());
        return TMResult::TM_ENOENT;
    }

    // create network
    std::ifstream file(net->weights.data(), std::ios::binary);
    file.seekg(0, std::ios::end);
    mData.resize(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char *>(mData.data()), mData.size());
    file.close();
    char *params = reinterpret_cast<char *>(mData.data());
    if (params == NULL)
    {
        printf("fail to read file:%s\n", net->weights.c_str());
        return TMResult::TM_ENOENT;
    }

    if (suffix_bm == ".bm")
    {
        struct shl_bm_sections *section = (struct shl_bm_sections *)(params + 4128);
        if (section->graph_offset)
        {
            // run model for inferencing with binary graph
            struct csinn_session *(*csinn_import_binary_model_c)(char *bm_addr);
            csinn_import_binary_model_c = (decltype(csinn_import_binary_model_c))dlsym(lib_handle, "csinn_import_binary_model");
            if (csinn_import_binary_model_c == NULL)
            {
                mSess = csinn_import_binary_model(params);
                if (!mSess)
                    printf("csinn_import_binary_model failed.\n");
            }
            else
            {
                mSess = csinn_import_binary_model_c(params);
                if (!mSess)
                    printf("csinn_import_binary_model_c failed.\n");
            }
        }
        else
        {
            // generate binary graph by .bm
            void *(*csinn_)(char *params_base);
            csinn_ = (decltype(csinn_))dlsym(lib_handle, "csinn_");
            mSess = (struct csinn_session *)csinn_(params + section->params_offset * 4096);
            if (!mSess)
                printf("csinn_ failed.\n");
        }
    }
    else
    {
        printf("invalid weight file:%s\n", net->weights.c_str());
        return TMResult::TM_ENOENT;
    }

    return TMResult::TM_OK;
}

int TMInterpreterCSINN::UnLoadNet()
{
    if (lib_handle)
    {
        dlclose(lib_handle);
    }

    csinn_session_deinit(mSess);
    csinn_free_session(mSess);

    return TMResult::TM_OK;
}

int TMInterpreterCSINN::SetNetConfig(const NetConfig_t *cfg)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterCSINN::GetNetConfig(NetConfig_t *cfg)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterCSINN::GetPerfProfile(std::vector<std::string> &names, std::vector<uint32_t> &timings_us)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterCSINN::GetInputTensor(Tensor_t **input)
{
    *input = new Tensor_t();
    Tensor_t *input_p = *input;

    struct csinn_tensor *input_tensor;

    input_p->mtx_num = csinn_get_input_number(mSess);
    input_p->mtx = new Matrix_t[input_p->mtx_num];
    for (int i = 0; i < input_p->mtx_num; i++)
    {
        input_tensor = mSess->input[i];
        // print_tensor_info(input_tensor);
        struct csinn_quant_info *qinfo = input_tensor->qinfo;
        input_p->mtx[i].mquant.scale = qinfo->scale;
        input_p->mtx[i].mquant.zero_point = qinfo->zero_point;
        input_p->mtx[i].mquant.min = qinfo->min;
        input_p->mtx[i].mquant.max = qinfo->max;

        input_p->mtx[i].mspec.name = input_tensor->name;
        input_p->mtx[i].mspec.dims.resize(input_tensor->dim_count);
        for (int j = 0; j < input_tensor->dim_count; j++)
        {
            input_p->mtx[i].mspec.dims[j] = input_tensor->dim[j];
        }

        input_p->mtx[i].mdata.dtype = DataType_CSI2AIE(input_tensor->dtype);
        input_p->mtx[i].mdata.size = ProductVector(input_p->mtx[i].mspec.dims) * DataTypeElemSize(input_tensor->dtype);
    }

    return TMResult::TM_OK;
}

int TMInterpreterCSINN::GetOutputTensor(Tensor_t **output)
{
    *output = new Tensor_t();
    Tensor_t *output_p = *output;

    struct csinn_tensor *output_tensor;

    output_p->mtx_num = csinn_get_output_number(mSess);
    output_p->mtx = new Matrix_t[output_p->mtx_num];
    for (int i = 0; i < output_p->mtx_num; i++)
    {
        output_tensor = mSess->output[i];
        // print_tensor_info(output_tensor);
        struct csinn_quant_info *qinfo = output_tensor->qinfo;
        output_p->mtx[i].mquant.scale = qinfo->scale;
        output_p->mtx[i].mquant.zero_point = qinfo->zero_point;
        output_p->mtx[i].mquant.min = qinfo->min;
        output_p->mtx[i].mquant.max = qinfo->max;

        output_p->mtx[i].mspec.name = output_tensor->name;
        output_p->mtx[i].mspec.dims.resize(output_tensor->dim_count);
        for (int j = 0; j < output_tensor->dim_count; j++)
        {
            output_p->mtx[i].mspec.dims[j] = output_tensor->dim[j];
        }

        output_p->mtx[i].mdata.dtype = DataType_CSI2AIE(output_tensor->dtype);
        output_p->mtx[i].mdata.size = ProductVector(output_p->mtx[i].mspec.dims) * DataTypeElemSize(output_tensor->dtype);
    }

    return TMResult::TM_OK;
}

int TMInterpreterCSINN::SetInputTensor(const Tensor_t *input)
{
    if (input == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    struct csinn_tensor input_tensor;
    for (int i = 0; i < input->mtx_num; i++)
    {
        input_tensor.data = *(input->mtx[i].mdata.data);
        input_tensor.mtype = MemType_AIE2CSI(input->mtx[i].mdata.mtype);
        csinn_update_input(i, &input_tensor, mSess);
    }

    return TMResult::TM_OK;
}

int TMInterpreterCSINN::SetOutputTensor(const Tensor_t *output)
{
    if (output == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    struct csinn_tensor output_tensor;
    for (int i = 0; i < output->mtx_num; i++)
    {
        output_tensor.data = *(output->mtx[i].mdata.data);
        output_tensor.mtype = MemType_AIE2CSI(output->mtx[i].mdata.mtype);
        csinn_update_output(i, &output_tensor, mSess);
    }

    return TMResult::TM_OK;
}

int TMInterpreterCSINN::Run(int32_t timeout)
{
    int ret = csinn_session_run(mSess);
    if (ret == CSINN_TRUE)
    {
        return TMResult::TM_OK;
    }
    else
    {
        printf("csinn_session_run failed. ret=%d\n", ret);
    }

    return TMResult::TM_EBUSY;
}

int TMInterpreterCSINN::ReleaseTensor(Tensor_t *tensor)
{
    if (tensor == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    delete[] tensor->mtx;
    delete tensor;

    return TMResult::TM_OK;
}

REGISTER_INTERPRETER_CLASS(TMInterpreterCSINN);

#endif
