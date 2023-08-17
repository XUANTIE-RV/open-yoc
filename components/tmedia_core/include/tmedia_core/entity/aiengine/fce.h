/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */
#ifndef FCE_H
#define FCE_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

typedef enum
{
    FCE_DIM_128     = 128,
    FCE_DIM_160     = 160,
    FCE_DIM_192     = 192,
    FCE_DIM_224     = 224,
    FCE_DIM_256     = 256,
    FCE_DIM_512     = 512,
    FCE_DIM_UNKNOWN = 0,
} FCEDim_e;

typedef enum
{
    FCE_DATA_INT4       = 4,
    FCE_DATA_INT8       = 8,
    FCE_DATA_F32        = 32,
    FCE_DATA_UNKNOWN    = 0,
} FCEDataWidth_e;

typedef enum
{
    FCE_UNSIGN  = 1 << 0,
    FCE_SIGN    = 1 << 1,
} FCESignType_e;

typedef enum
{
    FCE_ENDIAN_LITTLE   = 1 << 0,
    FCE_ENDIAN_BIG      = 1 << 1,
} FCEEndianType_e;

typedef struct
{
    uint64_t    pts;
    void        *data;
    int32_t     size;
} FCETargetVec_t;

typedef struct
{
    uint64_t    pts;
    uint32_t    idx;
    float       val;
} FCERet_t;

class FCEBaseLib
{
public:
    FCEBaseLib(std::string file)
    {
        mFile = file;
    }
    FCEBaseLib(const uint8_t *data, uint32_t size)
    {
        mData = std::make_pair(data, size);
    }

public:
    // file
    std::string mFile;
    // the <pointer,size> to file data
    std::pair<const uint8_t *, uint32_t> mData;
};

typedef struct
{
    uint32_t max_channels;
    uint32_t dims;
    uint32_t widths;
    uint32_t signs;
    uint32_t endians;
} FCECap_t;

class FCE
{
public:
    virtual ~FCE() {};

    // device control
    virtual int Open(int idx) = 0;
    virtual int Close() = 0;

    // parameter setting
    virtual int SetDIM(FCEDim_e dim) = 0;
    virtual int SetDataWidth(FCEDataWidth_e width) = 0;
    virtual int SetSignType(FCESignType_e sign) = 0;
    virtual int SetEndianType(FCEEndianType_e endian) = 0;
    virtual int SetRetTopN(uint32_t n) = 0;

    // base lib operation
    virtual int LoadBaseLib(const FCEBaseLib &lib) = 0;
    virtual int UnLoadBaseLib() = 0;
    virtual int DumpBaseLibFile(const std::string &lib_file) = 0;
    virtual int AddItem(const FCETargetVec_t &target_vec) = 0;

    // data operation
    virtual int DoCompare(const FCETargetVec_t &target_vec, std::vector<FCERet_t> &fce_rets, int32_t timeout_ms) = 0;
};

#endif  // FCE_H