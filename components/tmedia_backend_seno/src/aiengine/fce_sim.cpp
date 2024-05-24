/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */
#ifdef __linux__
#include <tmedia_config.h>
#endif
#ifdef CONFIG_TMEDIA_AIENGINE_FCE_SIM
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <tmedia_core/entity/aiengine/aiengine_inc.h>
#include <tmedia_backend_seno/aiengine/fce_sim.h>

inline float hypot(const uint32_t len, const float *v)
{
    float hypots = 0;
    for(uint32_t i = 0; i < len; i++)
        hypots += pow(v[i], 2);
    hypots = sqrt(hypots);
    return hypots;
}

inline void normalize(const uint32_t len, float *v)
{
    float hypots = hypot(len, v);

    for(uint32_t i = 0; i < len; i++)
        v[i] = v[i]/hypots;
}

inline float dot(const uint32_t len, const float *v1, const float *v2)
{
    float sum = 0;
    for (uint32_t i = 0; i < len; i++)
    {
        sum += v1[i] * v2[i];
    }
    return sum;
}

inline float cos_sim(const uint32_t len, const float *v1, const float *v2)
{
    float dots = dot(len, v1, v2);
    float hypots_v1 = hypot(len, v1);
    float hypots_v2 = hypot(len, v2);

    return dots/(hypots_v1*hypots_v2);
}

FCESim::FCESim()
{
    mOpened = false;
    mDevIdx = -1;
    mTopN = 1;
}

FCESim::~FCESim()
{
}

int FCESim::Open(int idx)
{
    mOpened = true;
    mDevIdx = idx;
    return TMResult::TM_OK;
}

int FCESim::Close()
{
    mOpened = false;
    mDevIdx = -1;
    return TMResult::TM_OK;
}

int FCESim::SetDIM(FCEDim_e dim)
{
    if (!mOpened)
    {
        return TMResult::TM_EPERM;
    }

    mDim = dim;
    return TMResult::TM_OK;
}

int FCESim::SetDataWidth(FCEDataWidth_e width)
{
    if (!mOpened)
    {
        return TMResult::TM_EPERM;
    }

    if ((width != FCE_DATA_INT8) && (width != FCE_DATA_F32))
    {
        return TMResult::TM_EINVAL;
    }

    mWidth = width;
    return TMResult::TM_OK;
}

int FCESim::SetSignType(FCESignType_e sign)
{
    if (!mOpened)
    {
        return TMResult::TM_EPERM;
    }

    mSign = sign;
    return TMResult::TM_OK;
}

int FCESim::SetEndianType(FCEEndianType_e endian)
{
    if (!mOpened)
    {
        return TMResult::TM_EPERM;
    }

    mEndian = endian;
    return TMResult::TM_OK;
}

int FCESim::SetRetTopN(uint32_t n)
{
    if (!mOpened)
    {
        return TMResult::TM_EPERM;
    }

    if (n > 3)
    {
        return TMResult::TM_EINVAL;
    }

    mTopN = n;
    return TMResult::TM_OK;
}

int FCESim::LoadBaseLib(const FCEBaseLib &lib)
{
    if (!lib.mFile.empty())
    {
        printf("baselib=%s\n", lib.mFile.c_str());
        FILE *pFile = fopen(lib.mFile.c_str(), "r");
        if (pFile == NULL)
        {
            printf("fopen file=%s failed\n", lib.mFile.c_str());
            return TMResult::TM_EINVAL;
        }

        int ret = 0;
        do
        {
            float feature_i;
            ret = fscanf(pFile, "%f", &feature_i);
            if (ret == 1)
            {
                // printf("feature_i=%f\n", feature_i);
                mBaseLib.push_back(feature_i);
            }
        }
        while (ret != EOF);

        fclose(pFile);
        printf("baselib.size=%ld\n", mBaseLib.size());
    }
    else
    {
        return TMResult::TM_EINVAL;
    }

    return TMResult::TM_OK;
}

int FCESim::UnLoadBaseLib()
{
    if (mBaseLib.empty())
    {
        return TMResult::TM_EPERM;
    }

    mBaseLib.clear();
    return TMResult::TM_OK;
}

int FCESim::DumpBaseLibFile(const std::string &lib_file)
{
    if (!mOpened)
    {
        return TMResult::TM_EPERM;
    }
    if (mBaseLib.empty())
    {
        return TMResult::TM_EPERM;
    }
    if ((mWidth != FCE_DATA_INT8) && (mWidth != FCE_DATA_F32))
    {
        return TMResult::TM_EINVAL;
    }

    FILE *pFile = fopen(lib_file.c_str(), "w");
    if (pFile == NULL)
    {
        printf("fopen file=%s failed\n", lib_file.c_str());
        return TMResult::TM_EINVAL;
    }

    for (size_t i = 0; i < mBaseLib.size(); i++)
    {
        if (mWidth == FCE_DATA_INT8)
        {
            fprintf(pFile, "%d\n", (int)(mBaseLib[i]));
        }
        else if (mWidth == FCE_DATA_F32)
        {
            fprintf(pFile, "%f\n", mBaseLib[i]);
        }
        else
        {
            ;
        }
    }

    fclose(pFile);

    return TMResult::TM_OK;
}

int FCESim::AddItem(const FCETargetVec_t &target_vec)
{
    if ((target_vec.data == NULL) || (target_vec.size <= 0))
    {
        return TMResult::TM_EINVAL;
    }

    if (mWidth == FCE_DATA_INT8)
    {
        uint8_t *pdata = static_cast<uint8_t *>(target_vec.data);
        float *normalize_datas = new float[target_vec.size];
        for (int32_t i = 0; i < target_vec.size; i++)
        {
            normalize_datas[i] = pdata[i] * 1.0;
        }
        normalize(target_vec.size, normalize_datas);
        for (int32_t i = 0; i < target_vec.size; i++)
        {
            mBaseLib.push_back(normalize_datas[i]);
        }
        delete [] normalize_datas;
    }
    else if (mWidth == FCE_DATA_F32)
    {
        float *pdata = static_cast<float *>(target_vec.data);
        normalize(target_vec.size, pdata);
        for (int32_t i = 0; i < target_vec.size; i++)
        {
            mBaseLib.push_back(pdata[i]);
        }
    }
    else
    {
        return TMResult::TM_NOT_SUPPORT;
    }

    return TMResult::TM_OK;
}

int FCESim::DoCompare(const FCETargetVec_t &target_vec, std::vector<FCERet_t> &fce_rets, int32_t timeout_ms)
{
    if (mBaseLib.empty())
    {
        return TMResult::TM_EPERM;
    }
    if (target_vec.size != mDim)
    {
        return TMResult::TM_EINVAL;
    }

    size_t lib_vecs_size = mBaseLib.size() / mDim;
    float *target_buf = NULL;
    float *lib_buf = mBaseLib.data();
    if (mWidth == FCE_DATA_INT8)
    {
        target_buf = new float[target_vec.size];
        uint8_t *target_buf_fact = static_cast<uint8_t *>(target_vec.data);
        for (int32_t i = 0; i < target_vec.size; i++)
        {
            target_buf[i] = target_buf_fact[i] * 1.0;
        }
    }
    else if (mWidth == FCE_DATA_F32)
    {
        target_buf = static_cast<float *>(target_vec.data);
        normalize(target_vec.size, target_buf);
    }
    else
    {
        return TMResult::TM_NOT_SUPPORT;
    }
    //printf("baselib size=%ld dim=%d width=%d vecs_size=%ld\n", mBaseLib.size(), mDim, mWidth, lib_vecs_size);
    std::vector<float> cos_sims;
    for (size_t i = 0; i < lib_vecs_size; i++)
    {
        //After normalization, the dot product used directly to reduce the calculation
        //float cos_sim_i = cos_sim(target_vec.size, target_buf, lib_buf + target_vec.size * i);
        float cos_sim_i = dot(target_vec.size, target_buf, lib_buf + target_vec.size * i);
        // printf("  vec[%ld] cos_sim_i=%f\n", i, cos_sim_i);
        cos_sims.push_back(cos_sim_i);
    }
    if (mWidth == FCE_DATA_INT8)
    {
        delete [] target_buf;
    }

    fce_rets.clear();
    for (size_t i = 0; i < mTopN; i++)
    {
        std::vector<float>::iterator biggest = std::max_element(cos_sims.begin(), cos_sims.end());
        FCERet_t fce_ret;
        fce_ret.pts = target_vec.pts;
        fce_ret.idx = std::distance(cos_sims.begin(), biggest);
        fce_ret.val = (*biggest);
        fce_rets.push_back(fce_ret);

        cos_sims[fce_ret.idx] = 0;
    }

    return TMResult::TM_OK;
}

REGISTER_FCE_CLASS(FCESim);

#endif
