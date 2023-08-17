/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FCE_SIM_H
#define TM_FCE_SIM_H

#include <cstdint>
#include <vector>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/aiengine/fce.h>

class FCESim : public FCE
{
public:
    FCESim();
    ~FCESim();

    int Open(int idx);
    int Close();

    int SetDIM(FCEDim_e dim);
    int SetDataWidth(FCEDataWidth_e width);
    int SetSignType(FCESignType_e sign);
    int SetEndianType(FCEEndianType_e endian);
    int SetRetTopN(uint32_t n);

    int LoadBaseLib(const FCEBaseLib &lib);
    int UnLoadBaseLib();
    int DumpBaseLibFile(const std::string &lib_file);
    int AddItem(const FCETargetVec_t &target_vec);

    int DoCompare(const FCETargetVec_t &target_vec, std::vector<FCERet_t> &fce_rets, int32_t timeout_ms);

private:
    bool mOpened;
    int mDevIdx;
    FCEDim_e mDim;
    FCEDataWidth_e mWidth;
    FCESignType_e mSign;
    FCEEndianType_e mEndian;
    uint32_t mTopN;
    std::vector<float> mBaseLib;
};

#endif  /* TM_FCE_SIM_H */
