/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FCE_DUMMY_H
#define TM_FCE_DUMMY_H

#include <cstdint>
#include <vector>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/aiengine/fce.h>

class FCEDummy : public FCE
{
public:
    FCEDummy();
    ~FCEDummy();

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
};

#endif  /* TM_FCE_DUMMY_H */
