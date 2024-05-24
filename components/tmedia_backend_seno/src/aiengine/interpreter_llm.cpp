/*
 * Copyright (C) 2024 Alibaba Group Holding Limited
 */
#ifdef __linux__
#include <tmedia_config.h>
#endif
#ifdef CONFIG_TMEDIA_AIENGINE_INTERPRETER_CSINN
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <tmedia_core/entity/aiengine/aiengine_inc.h>
#include <tmedia_backend_seno/aiengine/interpreter_llm.h>

TMInterpreterLLM::TMInterpreterLLM()
{
    lib_handle = NULL;
}

TMInterpreterLLM::~TMInterpreterLLM()
{
}

int TMInterpreterLLM::Open(int dev)
{
    mDev = dev;
    return TMResult::TM_OK;
}

int TMInterpreterLLM::Close()
{
    return TMResult::TM_OK;
}

int TMInterpreterLLM::LoadNet(const InterpreterNet *net)
{
    if (net == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    lib_handle = dlopen(net->model.c_str(), RTLD_NOW);
    if (!lib_handle)
    {
        LOG_E("dlopen failed:%s, lib path:%s\n", dlerror(), net->model.c_str());
        return TMResult::TM_ENOENT;
    }

    llm_conf_init = (decltype(llm_conf_init))dlsym(lib_handle, "llm_conf_init");
    if (!llm_conf_init) {
        LOG_E("Error loading llm_conf_init: %s\n", dlerror());
        return TMResult::TM_ENOENT;
    }
    llm_build = (decltype(llm_build))dlsym(lib_handle, "llm_build");
    if (!llm_build) {
        LOG_E("Error loading llm_build: %s\n", dlerror());
        return TMResult::TM_ENOENT;
    }
    llm_run = (decltype(llm_run))dlsym(lib_handle, "llm_run");
    if (!llm_run) {
        LOG_E("Error loading llm_run: %s\n", dlerror());
        return TMResult::TM_ENOENT;
    }
    llm_reset_cache = (decltype(llm_reset_cache))dlsym(lib_handle, "llm_reset_cache");
    if (!llm_reset_cache) {
        LOG_E("Error loading llm_reset_cache: %s\n", dlerror());
        return TMResult::TM_ENOENT;
    }
    llm_deinit = (decltype(llm_deinit))dlsym(lib_handle, "llm_deinit");
    if (!llm_deinit) {
        LOG_E("Error loading llm_deinit: %s\n", dlerror());
        return TMResult::TM_ENOENT;
    }

    reset_flag = false;
    pos_start = 0;
    conf = llm_conf_init(net->weights.c_str());
    ctx = llm_build(conf);
    output_tensor = csinn_alloc_tensor(NULL);
    output_tensor->data = NULL;
    output_image_tensor = csinn_alloc_tensor(NULL);
    output_image_tensor->data = NULL;

    llm_reset_cache(conf, ctx);
    embd = (struct shl_llm_input *)shl_mem_alloc(sizeof(struct shl_llm_input));

    return TMResult::TM_OK;
}

int TMInterpreterLLM::UnLoadNet()
{
    if (lib_handle)
    {
        llm_deinit(conf, ctx);
        dlclose(lib_handle);
    }

    if(!embd->pos) {
        free(embd->pos);
    }
    free(embd);

    if (!output_tensor->is_const)
    {
        shl_mem_free(output_tensor->data);
    }
    csinn_free_tensor(output_tensor);

    return TMResult::TM_OK;
}

int TMInterpreterLLM::SetNetConfig(const NetConfig_t *cfg)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterLLM::GetNetConfig(NetConfig_t *cfg)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterLLM::GetPerfProfile(std::vector<std::string> &names, std::vector<uint32_t> &timings_us)
{
    return TMResult::TM_EBUSY;
}

int TMInterpreterLLM::GetInputTensor(Tensor_t **input)
{
    *input = new Tensor_t();
    Tensor_t *input_p = *input;

    input_p->mtx_num = 10;
    input_p->mtx = new Matrix_t[input_p->mtx_num];

    return TMResult::TM_OK;
}

int TMInterpreterLLM::GetOutputTensor(Tensor_t **output)
{
    *output = new Tensor_t();
    output_p = *output;
    output_p->mtx_num = 2;
    output_p->mtx = new Matrix_t[output_p->mtx_num];

    return TMResult::TM_OK;
}

int TMInterpreterLLM::SetInputTensor(const Tensor_t *input)
{
    if (input == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    reset_flag = *(bool *)(input->mtx[0].mdata.data);
    if (reset_flag) {
        return TMResult::TM_OK;
    }

    pos_start = *(int *)(input->mtx[1].mdata.data);
    std::vector<int>* ids = (std::vector<int>*)(input->mtx[2].mdata.data);
    embd->n_tokens = ids->size();
    embd->token = ids->data();
    int32_t *embd_pos = (int32_t *)malloc(embd->n_tokens * sizeof(int32_t));
    for (int i = 0; i < embd->n_tokens; i++)
    {
        embd_pos[i] = i + pos_start;
    }
    embd->pos = embd_pos;

    // No image data.
    if (input->mtx[3].mdata.data == NULL) {
        return TMResult::TM_OK;
    }
    embd->image_h = *(int32_t*)(input->mtx[3].mdata.data);
    embd->image_w = *(int32_t*)(input->mtx[4].mdata.data);
    embd->image_c = *(int32_t*)(input->mtx[5].mdata.data);
    embd->image = (void*)(input->mtx[6].mdata.data);
    embd->only_run_vision = *(bool *)(input->mtx[7].mdata.data);
    embd->image_embd = (void*)(input->mtx[8].mdata.data);
    embd->image_input_mask = (int32_t*)(input->mtx[9].mdata.data);

    return TMResult::TM_OK;
}

int TMInterpreterLLM::SetOutputTensor(const Tensor_t *output)
{
    return TMResult::TM_OK;
}

int TMInterpreterLLM::Run(int32_t timeout)
{
    if (!output_tensor->is_const && (output_tensor->data != NULL))
    {
        if (ctx->output_session->output[0]->data == output_tensor->data) {
            ctx->output_session->output[0]->data = NULL;
        }
        shl_mem_free(output_tensor->data);
        output_tensor->data = NULL;
    }

    if (!output_image_tensor->is_const && (output_image_tensor->data != NULL))
    {
        if (ctx->i2t_session->output[0]->data == output_image_tensor->data) {
            ctx->i2t_session->output[0]->data = NULL;
        }
        shl_mem_free(output_image_tensor->data);
        output_image_tensor->data = NULL;
    }

    if (reset_flag) {
        llm_reset_cache(conf, ctx);
        reset_flag = false;
        return TMResult::TM_OK;
    }

    llm_run(ctx, embd);

    csinn_get_output(0, output_tensor, ctx->output_session);
    output_p->mtx[0].mdata.data = &(output_tensor->data);
    free(embd->pos);
    embd->pos = NULL;

    if (ctx->i2t_session != NULL) {
        csinn_get_output(0, output_image_tensor, ctx->i2t_session);
        output_p->mtx[1].mdata.data = &(output_image_tensor->data);
    }

    return TMResult::TM_OK;
}

int TMInterpreterLLM::ReleaseTensor(Tensor_t *tensor)
{
    if (tensor == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    delete[] tensor->mtx;
    delete tensor;

    return TMResult::TM_OK;
}

REGISTER_INTERPRETER_CLASS(TMInterpreterLLM);

#endif