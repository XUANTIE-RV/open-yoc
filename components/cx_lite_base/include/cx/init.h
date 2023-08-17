/**
 * @file init.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_INIT_H
#define CX_INIT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* chixiao memory loading callback type */
typedef int (*cx_mem_load_cb)(const char *partition_name, uint64_t flash_addr, uint64_t ram_addr, uint32_t size);

typedef struct {
    const char      *load_config;   /* memory loading configuration */
    cx_mem_load_cb  load_cb;        /* memory loading callback */
} cx_mem_config_t;

/**
 * @description: init chixiao hardware related, including board infomation and memory loading config
 * @param [in] board_config:    board configuration in json format
 * @param [in] mem_config:      memory loading configuration
 * @return                      0: Success, < 0: Error code.
 */
int cx_hardware_init(const char *board_config, const cx_mem_config_t *mem_config);

/**
 * @description: config chixiao hardware
 * @param [in] config:          board configuration in json format
 * @return                      0: Success, < 0: Error code.
 */
int cx_hardware_config(const char *config);

/**
 * @description: init chixiao services
 * @param [in] service_config:  service configuration in json format
 * @param [in] input_config:  video input configuration in json format
 * @return                      0: Success, < 0: Error code.
 */
int cx_service_init(const char *service_config, const char *input_config);

/**
 * @description: deinit chixiao framework and all services
 * @return       0: Success, < 0: Error code.
 */
int cx_deinit(void);

/**
 * @description: get chixiao sdk version
 * @return       version char string
 */
const char *cx_get_version(void);

/**
 * @description: check if this is a preview edition
 * @return       true: preview edition, false: release edition
 */
bool cx_is_preview_edition(void);

/**
 * @description: get preview expire time, format is YYYYMM
 * @return       >= 0: Expire time, < 0: No expire time
 */
int cx_get_preview_expire_time(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CX_INIT_H */

