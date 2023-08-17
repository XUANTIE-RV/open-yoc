/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <utility>
#include <vector>

typedef enum
{
    DEV_CPU = 1 << 0,
    DEV_NPU = 1 << 1,
} DevType_e;

typedef enum
{
    DATA_8U     = 1 << 0,
    DATA_8S     = 1 << 1,
    DATA_16U    = 1 << 2,
    DATA_16S    = 1 << 3,
    DATA_32U    = 1 << 4,
    DATA_32S    = 1 << 5,
    DATA_16F    = 1 << 6,
    DATA_32F    = 1 << 7,
    DATA_64F    = 1 << 8,
} DataType_e;

typedef enum
{
    ALIGN_BYTE_16   = 1 << 0,
    ALIGN_BYTE_32   = 1 << 1,
    ALIGN_BYTE_64   = 1 << 2,
    ALIGN_BYTE_128  = 1 << 3,
    ALIGN_BYTE_256  = 1 << 4,
    ALIGN_BYTE_512  = 1 << 5,
    ALIGN_BYTE_1024 = 1 << 6,
    ALIGN_BYTE_2048 = 1 << 7,
    ALIGN_BYTE_4096 = 1 << 8,
} AlignByteType_e;

typedef struct
{
    int size;
    DataType_e type;
    void **data; //data[batch][size] = type
} MatrixData_t;

typedef struct
{
    std::string name;
    std::vector<int> dims;
} MatrixSpec_t;

typedef struct
{
    int zero_point;
    float scale;
    float min;
    float max;
} MatrixQuant_t;

typedef struct
{
    MatrixSpec_t mspec;
    MatrixData_t mdata;
    MatrixQuant_t mquant;
} Matrix_t;

typedef struct
{
    Matrix_t *mtx;
    int mtx_num;
} Tensor_t;

typedef struct
{
    int batch;
    int sched_mode;
} NetConfig_t;

class InterpreterNet
{
public:
    InterpreterNet(std::string model)
    {
        this->model = model;
    }
    InterpreterNet(std::string model, std::string weights)
    {
        this->model = model;
        this->weights = weights;
    }
    InterpreterNet(const uint8_t *pmodel, uint32_t model_size)
    {
        this->pmodel = std::make_pair(pmodel, model_size);
    }
    InterpreterNet(const uint8_t *pmodel, uint32_t model_size, const uint8_t *pweights,
                   uint32_t weights_size)
    {
        this->pmodel = std::make_pair(pmodel, model_size);
        this->pweights = std::make_pair(pweights, weights_size);
    }

public:
    // file
    std::string model;
    std::string weights;
    // the pointer to file data
    std::pair<const uint8_t *, uint32_t> pmodel;
    std::pair<const uint8_t *, uint32_t> pweights;
};

typedef struct
{
    uint32_t dev_types;
    uint32_t data_types;
    uint32_t align_type;
} InterpreterCap_t;

class Interpreter
{
public:
    virtual ~Interpreter() {}

    virtual int Open(int idx) = 0;
    virtual int Close() = 0;

    virtual int LoadNet(const InterpreterNet *net) = 0;
    virtual int UnLoadNet() = 0;
    virtual int SetNetConfig(const NetConfig_t *cfg) = 0;
    virtual int GetNetConfig(NetConfig_t *cfg) = 0;
    virtual int GetPerfProfile(std::vector<std::string> &names, std::vector<uint32_t> &timings_us) = 0;

    virtual int GetInputTensor(Tensor_t **input) = 0;
    virtual int GetOutputTensor(Tensor_t **output) = 0;
    virtual int SetInputTensor(const Tensor_t *input) = 0;
    virtual int SetOutputTensor(const Tensor_t *output) = 0;
    virtual int Run(int32_t timeout_ms) = 0;
    virtual int ReleaseTensor(Tensor_t *tensor) = 0;
};

#endif // INTERPRETER_H