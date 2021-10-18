/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __VFS_CLI_H__
#define __VFS_CLI_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief command for cat
 * @return
 */
void cli_reg_cmd_cat(void);

/**
 * @brief command for ls
 * @return
 */
void cli_reg_cmd_ls(void);

/**
 * @brief command for mkdir
 * @return
 */
void cli_reg_cmd_mkdir(void);

/**
 * @brief command for mv
 * @return
 */
void cli_reg_cmd_mv(void);

/**
 * @brief command for rm
 * @return
 */
void cli_reg_cmd_rm(void);

#ifdef __cplusplus
}
#endif

#endif /* __VFS_CLI_H__ */

