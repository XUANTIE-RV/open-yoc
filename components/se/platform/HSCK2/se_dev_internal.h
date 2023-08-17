/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_rng.h
 * @brief    Header File for RNG
 * @version  V1.0
 * @date     22. May 2023
 * @model    rng
 ******************************************************************************/
#ifndef _SE_DEV_INTERNAL_H_
#define _SE_DEV_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief       Lock the resource of se device
  \return      None
*/
void se_dev_lock(void);

/**
  \brief       Unlock the resource of se device
  \return      None
*/
void se_dev_unlock(void);

#ifdef __cplusplus
}
#endif

#endif /* _SE_DEV_INTERNAL_H_ */