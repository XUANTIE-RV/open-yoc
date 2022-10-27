/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#ifndef __OTA_AB_H__
#define __OTA_AB_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get current active A/B slot
 *
 * @returns "a" if current slot is A, or "b" if current slot is B.
 */
const char* otaab_get_current_ab(void);

/**
 * @brief Get next active A/B slot
 *
 * @returns "a" if current slot is A, or "b" if current slot is B.
 */
const char* otaab_get_next_ab(void);

/**
 * @brief To start ota process
 * It called by sdk to indicate ota will start soon, vendor may just do nothing.
 */
void otaab_start(void);

/**
 * @brief To upgrade one slice of a partition from file_name or buffer.
 *  It should write-read-verify the slice data to make sure it is wrote correctly,
 *  And also should retry 3~5 times if write failed.
 * @param[in] image_name: The image name extract from ota package, like prim/boot.
 *            vendor should know which partition to write via the image_name.
 * @param[in] file_name: if buffer==NULL, get slice from the file_name, or get slice direct from buffer
 * @param[in] offset: from which position this slice starts
 * @param[in] slice_size: the length of this slice in Byte
 * @param[in] is_last_slice: 1, this slice is the last one of current iamge_name, otherwise 0.
 * @param[in] buffer: slice data is in buffer if it's not NULL, in this case, just ignore file_name
 * @return: - 0: success; - -1: fail
 */
int otaab_upgrade_slice(const char *image_name,
        const char *file_name,
        uint32_t offset,
        uint32_t slice_size,
        uint32_t is_last_slice,
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
int otaab_upgrade_partition_slice(const char *partition_name,
        const char *file_name,
        uint32_t offset,
        uint32_t slice_size,
        uint32_t is_last_slice,
        char *buffer);

/**
 * @brief To mark that the non-active slot is upgraded successfully.
 * It's time to set A/B related flag to let system switch to new system until next reboot.
 * It MAY do two actions:
 * 1. To set flag to indicate system boot from non-active slot next time.
 * 2. To enable A/B fallback to old system if new system boot failed
 * @param[in] img_version: the version of images
 * @return: - 0: success; - -1: fail
 */
int otaab_upgrade_end(const char *img_version);

/**
 * @brief To mark the ota process is finished and present slot is workable,
 * It called by sdk after main boot flow done
 * @param[in] ab_fallback: 1: enable ab fallback, if it is not enabled; 0, disable ab_fallback.
 */
void otaab_finish(int ab_fallback);

#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_OTA_H_ */
