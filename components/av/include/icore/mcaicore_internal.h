/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MCAICORE_INTERNAL_H__
#define __MCAICORE_INTERNAL_H__

#include "icore/icore_msg.h"
#include "mca/mca.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define MCAICORE_AP_IDX          (0)
#define MCAICORE_CP_IDX          (2)
#else
#define MCAICORE_AP_IDX          (1)
#define MCAICORE_CP_IDX          (2)
#endif

#define IPC_CMD_MCAICORE         (61)
#define MCAICORE_IPC_SERIVCE_ID  (0x16)

enum {
    ICORE_CMD_MCA_INVALID,
    ICORE_CMD_MCA_NEW,
    ICORE_CMD_MCA_IIR_FXP32_COEFF32_CONFIG,
    ICORE_CMD_MCA_IIR_FXP32,
    ICORE_CMD_MCA_FREE,
};

typedef struct {
    int32_t           type;      // mca type
    void              *mca;      //
} mcaicore_new_t;

typedef struct {
    void              *mca;      //
    fxp32_t           coeff[5];
} mcaicore_iir_fxp32_coeff32_config_t;

typedef struct {
    void              *mca;      //
    fxp32_t           *input;
    size_t            input_size;
    fxp32_t           yn1;
    fxp32_t           yn2;
    fxp32_t           *output;
} mcaicore_iir_fxp32_t;

typedef struct {
    void      *mca;
} mcaicore_free_t;

__END_DECLS__

#endif /* __MCAICORE_INTERNAL_H__ */

