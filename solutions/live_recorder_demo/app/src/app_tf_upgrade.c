#include <stdio.h>
#include <yoc/partition.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include <ulog/ulog.h>
#include <aos/version.h>
#include <vfs.h>
#include "mbedtls/md5.h"

#define PARTITION_SECTOR_NUM (10)

#define TAG "app"

static int tf_firmware_version_check(const char *upgrade_file)
{
	int fd;
	char buf[48];
	ssize_t ret = 0;
	char *current_ver = aos_get_app_version();

    if (0 > (fd = aos_open(upgrade_file, O_RDONLY))) {
        LOGI(TAG, "no fota.bin file");
        return -1;
    }

	ret = aos_read(fd, buf, 48);
	if (ret != 48) {
        LOGW(TAG, "fota.bin header read failed");
		return -1;
	}

	if (strncmp(buf, "IMGE", 4) != 0) {
        LOGW(TAG, "fota.bin header read failed");
		return -1;
	}

	/* if version the same, don't upgrade*/
	if (strncmp(buf + 16, current_ver, 32) == 0) {
		LOGW(TAG, "fota.bin version equals current app version (%s)", current_ver);
		return -1;
	}

	aos_close(fd);
	return 0;
}

int program_fota_bin(char *fileName)
{
    uint32_t read_len, offset, buf_len;
    int fd;

    if (0 > (fd = aos_open(fileName, O_RDONLY))) {
        LOGW(TAG, "File read open failed");
        return -1;
    }

    partition_t part_hdl = partition_open("misc");
    if (part_hdl < 0) {
        LOGW(TAG, "misc partition open failed");
        goto FOTA_ERR;
    }

    partition_info_t *info = partition_info_get(part_hdl);
    buf_len = info->sector_size * PARTITION_SECTOR_NUM;
    char *buf = aos_malloc_check(buf_len);

    read_len    = buf_len;
    offset = 2 * info->sector_size;
    LOGD(TAG, "read begin :");

    while(1)
    {
        size_t ret = aos_read(fd, buf, read_len);
        if (ret <= 0) {
            LOGD(TAG, "ead end");
            break;
        }

        LOGW(TAG, "sdcard to flash %u bytes", ret);
        partition_erase(part_hdl, offset, (ret+info->sector_size-1)/info->sector_size);
        partition_write(part_hdl, offset, buf, ret);

        offset += ret;
    }

    LOGD(TAG, "sdcard to flash over !");
    aos_free(buf);
    aos_close(fd);
    partition_close(part_hdl);

    return 0;

FOTA_ERR:
    aos_close(fd);

    return -1;
}

int program_weight_bin(char *fileName)
{
#define MD5_MAX_LEN (17)

    unsigned char image_md5[MD5_MAX_LEN];
    unsigned char weight_md5[MD5_MAX_LEN];
    char weight_magic[4];
    uint32_t offset, buf_len;
    int fd = -1;
    size_t len;
    int ret;
    mbedtls_md5_context md5_ctx;

    partition_t part_hdl = partition_open("weight");
    if (part_hdl < 0) {
        LOGW(TAG, "weight partition open failed");
        return -1;
    }

    partition_info_t *info = partition_info_get(part_hdl);
    buf_len = info->sector_size * PARTITION_SECTOR_NUM;
    char *buf = aos_malloc_check(buf_len);

    /* check weight.bin md5 */
    fd = aos_open(fileName, O_RDONLY);
    CHECK_RET_TAG_WITH_GOTO(fd >= 0, WEIGHT_ERR);

    len = aos_read(fd, &weight_magic, 4);
    CHECK_RET_TAG_WITH_GOTO(len == 4 && strncmp(weight_magic, "WEGT", 4) == 0, WEIGHT_ERR);

    len = aos_read(fd, image_md5, MD5_MAX_LEN - 1);
    CHECK_RET_TAG_WITH_GOTO(len == MD5_MAX_LEN - 1, WEIGHT_ERR);

    int md5_len = MD5_MAX_LEN - 1;
    ret = aos_kv_get("weight_md5", weight_md5, &md5_len);
    if (ret < 0 || md5_len != MD5_MAX_LEN - 1) {
        LOGD(TAG, "no md5 stored\n");
    } else {
        if (memcmp(image_md5, weight_md5, MD5_MAX_LEN - 1) == 0) {
            LOGD(TAG, "weight, md5 the same, no upgrade");
            return -1;
        }
    }

    /* calculate the image md5, store in weight_md5 */
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);

    while (1) {
        len = aos_read(fd, buf, buf_len);
        if (len <= 0) {
            break;
        }

        mbedtls_md5_update(&md5_ctx, (unsigned char *)buf, len);
    }

    mbedtls_md5_finish(&md5_ctx, weight_md5);
    mbedtls_md5_free(&md5_ctx);

    /* check if image calculated md5 is the same as the one in image header*/
    if (memcmp(image_md5, weight_md5, MD5_MAX_LEN - 1) != 0) {
        LOGW(TAG, "image md5 check failed");
        goto WEIGHT_ERR;
    }

    LOGD(TAG, "start to upgrade weight");
    /* program weight to flash and update the kv */
    aos_kv_del("weight_md5");

    offset = 0;
    aos_lseek(fd, 0, SEEK_SET);   // skip off the md5 header

    while(1)
    {
        size_t len = aos_read(fd, buf, buf_len);
        if (len <= 0) {
            break;
        }

        LOGD(TAG, "sdcard[w] to flash %u bytes", len);
        ret = partition_erase(part_hdl, offset, (len+info->sector_size-1)/info->sector_size);
        CHECK_RET_TAG_WITH_GOTO(ret == 0, WEIGHT_ERR);

        ret = partition_write(part_hdl, offset, buf, len);
        CHECK_RET_TAG_WITH_GOTO(ret == 0, WEIGHT_ERR);

        offset += len;
    }

    aos_kv_set("weight_md5", weight_md5, MD5_MAX_LEN - 1, 1);

    LOGD(TAG, "weight upgrade finished!");
    aos_free(buf);
    aos_close(fd);
    partition_close(part_hdl);

    return 0;

WEIGHT_ERR:
    if (fd >= 0)
        aos_close(fd);

    if (part_hdl >= 0)
        partition_close(part_hdl);

    return -1;
}

int tf_firmware_upgrade_check(void)
{
    char *fotaFileName = "/mnt/sd/fota.bin";
    char *weightFileName = "/mnt/sd/weight.bin";
    int ret_fota = -1, ret_weight = -1;

	if (tf_firmware_version_check(fotaFileName) == 0) {
		// set_tf_ota_flag(1);
        ret_fota = program_fota_bin(fotaFileName);
	}
	// else
	// {
	// 	set_tf_ota_flag(0);
	// }

    if (aos_access(weightFileName, R_OK) == 0) {
        /* if there is weight.bin in the sdcard, do flash programming */
	    // set_tf_weight_flag(1);
        ret_weight = program_weight_bin(weightFileName);   
    } else {
        LOGI(TAG, "no weight.bin file");
		// set_tf_weight_flag(0);
	}

    if (ret_fota == 0 || ret_weight == 0) {
		// hcc_set_wifi_sleep_time(300);
        aos_reboot();
    }

    return 0;
}

// void tf_task(void *paras)
// {
// 	tf_firmware_upgrade_check();
// 	aos_task_exit(0);
// }

// void tf_task_start()
// {
// 	aos_task_t tf_task_thread;
// 	int ret = aos_task_new_ext(&tf_task_thread, "tf_task", tf_task, NULL, 5*1024, 32);
// 	if(ret <0)
// 	{
// 		printf("tf task start error \r\n");
// 	}
// }