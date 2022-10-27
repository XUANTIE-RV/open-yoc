/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#ifndef __BOOTAB_H__
#define __BOOTAB_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 触发回滚的最大有效重启次数
#ifndef CONFIG_BOOT_LIMIT
#define BOOT_LIMIT 5
#else
#define BOOT_LIMIT CONFIG_BOOT_LIMIT
#endif

// 回读校验次数配置
#ifndef CONFIG_AB_WRITE_CHECK_COUNT
#define AB_WRITE_CHECK_COUNT 1
#else
#define AB_WRITE_CHECK_COUNT CONFIG_AB_WRITE_CHECK_COUNT
#endif

// 回读校验的静态BUFFER大小
#ifndef CONFIG_AB_WRITE_CHECK_BUF_SIZE
#define AB_WRITE_CHECK_BUF_SIZE 8192
#else
#define AB_WRITE_CHECK_BUF_SIZE CONFIG_AB_WRITE_CHECK_BUF_SIZE
#endif

#define B_ENVAB_NAME    "envab"
#define BOOTAB_INIT_VER "yoc.init.v"

/**
 * @brief bootab分区升级初始化，需要在系统启动时调用
 * @return 0 : success; other : fail
 */
int bootab_init(void);

/**
 * @brief 获取当前有效分区
 * @return "a"或者"b" : success; NULL: fail
 */
const char *bootab_get_current_ab(void);

/**
 * @brief 获取当前有效分区的bootcount个数
 * @param[in] ab "a"或者"b"
 * @return 0 : fail; other: bootcount
 */
int bootab_get_bootcount(const char *ab);

/**
 * @brief 设置当前有效分区的bootcount个数
 * @param[in] ab "a"或者"b"
 * @param[in] old_cnt 该分区的原来的bootcount
 * @return 0 : success; other: fail
 */
int bootab_set_bootcount(const char *ab, int old_cnt);

/**
 * @brief 获取当前有效分区的bootlimit个数
 * @param[in] ab "a"或者"b"
 * @return bootlimit个数
 */
int bootab_get_bootlimit(const char *ab);

/**
 * @brief 获取当前有效分区fallback功能是否使能
 * @param[in] ab "a"或者"b"
 * @return true : enable; false: disable
 */
bool bootab_get_fallback_is_enable(const char *ab);

/**
 * @brief 清除FLAG区域
 * @param[in] ab a或者b分区
 * @return int 0 : success; other: fail
 */
int bootab_clear_flags(const char *ab);

/**
 * @brief 设置a或者b分区有效, 并打开回滚功能
 * @param[in] ab "a" 或者 "b"
 * @param[in] img_version 固件的版本
 * @return int 0 : success; other: fail
 */
int bootab_set_valid(const char *ab, const char *img_version);

/**
 * @brief 设置分区为无效
 * @param[in] ab "a"或者"b"
 * @return 0 : success; other: fail
 */
int bootab_set_invalid(const char *ab);

/**
 * @brief enable fallback function
 * @param[in] ab "a" 或者 "b"
 * @return 0 : success; other: fail
 */
int bootab_set_fallback_enable(const char *ab);

/**
 * @brief disable fallback function
 * @param[in] ab "a" 或者 "b"
 * @return 0 : success; other: fail
 */
int bootab_set_fallback_disable(const char *ab);

/**
 * @brief 回滚功能执行
 * @param[in] ab "a"或者"b"
 * @return NULL : 不需要回滚; other: 回滚至分区的名字
 */
const char * bootab_fallback(const char *ab);

/**
 * @brief 获取a/b镜像版本
 * @param[in] ab "a"或者"b"
 * @return "" : 空版本; other: 镜像版本
 */
const char *bootab_get_imgs_version(const char *ab);

/**
 * @brief 获取当前slot镜像版本
 * @return "" : 空版本; other: 镜像版本
 */
const char *bootab_get_cur_imgs_version(void);

/**
 * @brief To upgrade one slice of a partition from file_name or buffer.
 *  It should write-read-verify the slice data to make sure it is wrote correctly,
 *  And also should retry 3~5 times if write failed.
 * @param[in] image_name: The image name extract from ota package, like prim/boot.
 *            vendor should know which partition to write via the image_name.
 * @param[in] ab: which slot to write, "a" or "b"
 * @param[in] file_name: if buffer==NULL, get slice from the file_name, or get slice direct from buffer
 * @param[in] offset: from which position this slice starts
 * @param[in] slice_size: the length of this slice in Byte
 * @param[in] is_last_slice: 1, this slice is the last one of current iamge_name, otherwise 0.
 * @param[in] buffer: slice data is in buffer if it's not NULL, in this case, just ignore file_name
 * @return: - 0: success; - -1: fail
 */
int bootab_upgrade_slice(const char *image_name, const char *ab, const char *file_name,
                         uint32_t offset, uint32_t slice_size, uint32_t is_last_slice,
                         char *buffer);
/**
 * @brief To upgrade one slice of a partition from file_name or buffer.
 *  It should write-read-verify the slice data to make sure it is wrote correctly,
 *  And also should retry 3~5 times if write failed.
 * @param[in] partition_name: the partition name which we want to write fota data to
 * @param[in] file_name: if buffer==NULL, get slice from the file_name, or get slice direct from buffer
 * @param[in] offset: from which position this slice starts
 * @param[in] slice_size: the length of this slice in Byte
 * @param[in] is_last_slice: 1, this slice is the last one of current iamge_name, otherwise 0.
 * @param[in] buffer: slice data is in buffer if it's not NULL, in this case, just ignore file_name
 * @return: - 0: success; - -1: fail
 */
int bootab_upgrade_partition_slice(const char *partition_name, const char *file_name,
                         uint32_t offset, uint32_t slice_size, uint32_t is_last_slice, char *buffer);
#ifdef __cplusplus
}
#endif

#endif