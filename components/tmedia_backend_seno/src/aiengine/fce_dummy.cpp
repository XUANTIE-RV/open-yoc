/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <tmedia_backend_seno/aiengine/fce_dummy.h>

FCEDummy::FCEDummy()
{

}

FCEDummy::~FCEDummy()
{
}

int FCEDummy::Open(int idx)
{
    return TMResult::TM_OK;
}

int FCEDummy::Close()
{
    return TMResult::TM_OK;
}

int FCEDummy::SetDIM(FCEDim_e dim)
{
    return TMResult::TM_OK;
}

int FCEDummy::SetDataWidth(FCEDataWidth_e width)
{
    return TMResult::TM_OK;
}

int FCEDummy::SetSignType(FCESignType_e sign)
{
    return TMResult::TM_OK;
}

int FCEDummy::SetEndianType(FCEEndianType_e endian)
{
    return TMResult::TM_OK;
}

int FCEDummy::SetRetTopN(uint32_t n)
{
    return TMResult::TM_OK;
}

int FCEDummy::LoadBaseLib(const FCEBaseLib &lib)
{
    return TMResult::TM_OK;
}

int FCEDummy::UnLoadBaseLib()
{
    return TMResult::TM_OK;
}

int FCEDummy::DumpBaseLibFile(const std::string &lib_file)
{
    return TMResult::TM_OK;
}

int FCEDummy::AddItem(const FCETargetVec_t &target_vec)
{
    return TMResult::TM_OK;
}

int FCEDummy::DoCompare(const FCETargetVec_t &target_vec, std::vector<FCERet_t> &fce_rets, int32_t timeout_ms)
{
    return TMResult::TM_OK;
}