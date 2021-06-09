/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __UPDATE_H__
#define __UPDATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_RAM_MAX_USE
#define CONFIG_RAM_MAX_USE 50*1024 //BYTE
#endif

int update_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __UPDATE_H__ */
