/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include "board.h"
#if  defined (CONFIG_BOARD_OTA_SUPPORT) && (CONFIG_BOARD_OTA_SUPPORT)
#include <aos/aos.h>
#include <errno.h>
#include <dfu_port.h>
#include <ota_server.h>
#include <ulog/ulog.h>
#include <yoc/partition.h>
#include <platform/wm_fwup.h>

#define TAG "board_ota"

static int          board_ota_bt_controller_idx   = -1;
static void        *board_ota_main_controller_image = NULL;
static uint32_t     board_ota_main_controller_size  = 0;

static void board_ota_firmware_event_cb(ota_firmware_state_en event, void *data)
{
    ota_firmware_state_data *event_data = (ota_firmware_state_data *)data;

    if (event_data->firmware_index != board_ota_bt_controller_idx) {
        return;
    }

    switch (event) {
    case FIRMWARE_STATE_IN_USE: {
        LOGD(TAG, "OTA firmware index :%02x in use", event_data->firmware_index);
    }
    break;

    case FIRMWARE_STATE_END_USE: {
        LOGD(TAG, "OTA firmware index :%02x end use", event_data->firmware_index);
    }
    break;

    case FIRMWARE_STATE_REMOVED: {
        LOGD(TAG, "OTA firmware index :%02x removed", event_data->firmware_index);
    }
    break;

    default:
        break;
    }
}

#if defined(CONFIG_GW_USE_YOC_BOOTLOADER) && (CONFIG_GW_USE_YOC_BOOTLOADER)
static int board_ota_yoc_image_upgrade(const void *image, uint32_t size)
{
    LOGI(TAG, "Gateway upgrade start\n");
    partition_t handle = partition_open("misc");

    if (handle >= 0) {
        partition_info_t *lp = hal_flash_get_info(handle);
        aos_assert(lp);
        if (partition_erase(handle, lp->sector_size << 1, (size + lp->sector_size - 1) / lp->sector_size) < 0) {
            LOGE(TAG,"erase misc fail:%x\n", size);
            return -1;
        }

        if (partition_write(handle, lp->sector_size << 1, (void *)image, size) < 0) {
            LOGE(TAG, "write misc fail:%x\n", size);
            return -1;
        }

        LOGI(TAG, "OTA Reboot!!!");
        aos_reboot();
        return 0;
    }
    return 0;
}
#endif

#if defined(CONFIG_GW_USE_YOC_BOOTLOADER) && (CONFIG_GW_USE_YOC_BOOTLOADER)
#define BT_PARTIION_NAME        "bt"
typedef struct {
    uint32_t magic;
    uint8_t ver;
    uint8_t image_sum;
    uint8_t digest_type;
    uint8_t signature_type;
    uint32_t mnft_off;
    uint8_t head_len;
    uint8_t pad_type;
    uint8_t rsvd[2];
} fota_head_info_t;

typedef struct {
    uint8_t img_name[8];
    uint32_t img_off;
    uint8_t img_type;
    uint8_t rsv_d[3];
} fota_img_info_t;

static int board_ota_occ_fota_bt_image_parse(const char *image, uint32_t size, const char **bt_image_addr, uint32_t *bt_image_size)
{
    fota_head_info_t *fota_head = (fota_head_info_t *)image;

    if (!image || !size) {
        LOGE(TAG, "image param is NULL");
        return -1;
    }

    fota_img_info_t *img_info = (fota_img_info_t *)(image + fota_head->head_len);

    //prase BT image from fota image
    for (int i = 0; i < fota_head->image_sum; i++) {
        if (strlen(BT_PARTIION_NAME) == strlen((const char *)img_info[i].img_name) && !strncmp(BT_PARTIION_NAME, (const char *)img_info[i].img_name, strlen(BT_PARTIION_NAME))) {
            uint32_t img_size = (i + 1 == fota_head->image_sum) ? fota_head->mnft_off - img_info[i].img_off : img_info[i + 1].img_off - img_info[i].img_off;
            const char *pscn = image + img_info[i].img_off;
            uint32_t scn_size = pscn[4] | (pscn[5] << 8) | (pscn[6] << 16) | (pscn[7] << 24);

            if (scn_size > img_size) {
                LOGE(TAG, "scn size err %d, %d", scn_size, img_size);
                return -1;
            }

            //flash sector align
            uint32_t img_real_offet = (img_info[i].img_off + scn_size + 0x1000 - 1) / 0x1000 * 0x1000;
            uint32_t img_real_size = img_size - (img_real_offet - img_info[i].img_off);
            *bt_image_addr = image + img_real_offet;
            *bt_image_size = img_real_size;
            return 0;
        }
    }

    return -1;
}

#else

static int board_ota_get_main_controller_image_len(const char *image, uint32_t size)
{
    if (size <= sizeof(IMAGE_HEADER_PARAM_ST)) {
        LOGE(TAG, "invaild image size");
        return -1;
    }

    IMAGE_HEADER_PARAM_ST image_header;

    memcpy((char *)&image_header, image, sizeof(image_header));
    return image_header.img_len + sizeof(image_header);
}

static int board_ota_wm_image_upgrade(const char *image, uint32_t size)
{
    IMAGE_HEADER_PARAM_ST image_header;
    char *image_offset = NULL;

    image_offset = (char *)image;

    memcpy((char *)&image_header, (char *)image, sizeof(image_header));
    int total_len = image_header.img_len + sizeof(image_header);
    int remain_len = total_len;
    int sync_len = 0;

    LOGI(TAG, "upgrade start, %p, %d", image, remain_len);

    unsigned int session_id = tls_fwup_enter(TLS_FWUP_IMAGE_SRC_WEB, size);

    while (remain_len > 0) {
        sync_len = (remain_len > 512) ? 512 : remain_len;
        tls_fwup_request_sync(session_id, image_offset, sync_len);
        image_offset += sync_len;
        remain_len -= sync_len;
    }

    tls_fwup_exit(session_id);

    LOGI(TAG, "upgrade Reboot!!!");
    aos_reboot();

    return 0;
}
#endif

/* gateway main controler chip upgrade */
static int board_ota_main_controler_upgrade(const void *image, uint32_t size)
{
    if (!image || !size) {
        LOGE(TAG, "image param is NULL");
        return -1;
    }

    int ret = 0;

#if defined(CONFIG_BT_MESH) && (CONFIG_BT_MESH)
    extern int gateway_mesh_suspend();
    gateway_mesh_suspend();
#endif

#if defined(CONFIG_GW_USE_YOC_BOOTLOADER) && (CONFIG_GW_USE_YOC_BOOTLOADER)
    ret = board_ota_yoc_image_upgrade(image, size);
#else
    ret = board_ota_wm_image_upgrade(image, size);
#endif
    if(ret) {
        LOGE(TAG, "main controler update failed %d", ret);
    }

    aos_free(board_ota_main_controller_image);

    return 0;
}

static void board_ota_dev_event_cb(ota_device_state_en event_id, void *param)
{

    switch (event_id) {
    case OTA_STATE_ONGOING: {
        ota_state_ongoing *status = (ota_state_ongoing *)param;

        if (status->firmware_index != board_ota_bt_controller_idx) {
            return;
        }

        LOGI(TAG, "ota ready bt controller");
    }
    break;

    case OTA_STATE_SUCCESS: {
        ota_state_success *status = (ota_state_success *)param;

        if (status->firmware_index != board_ota_bt_controller_idx) {
            return;
        }

        LOGI(TAG, "ota success for bt controller old_version:0x%04x new_verison:0x%04x cost %d ms", \
             status->old_ver, status->new_ver, status->cost_time);

        board_ota_main_controler_upgrade(board_ota_main_controller_image, board_ota_main_controller_size);
    }
    break;

    case OTA_STATE_FAILED: {
        ota_state_fail *status = (ota_state_fail *)param;

        if (status->firmware_index != board_ota_bt_controller_idx) {
            return;
        }

        LOGE(TAG, "BT Controller Upgrade fail, Reboot!!!",  \
             status->firmware_index, status->dev_info.unicast_addr, status->dev_info.addr.val[0], status->dev_info.addr.val[1], status->dev_info.addr.val[2], status->dev_info.addr.val[3], status->dev_info.addr.val[4], status->dev_info.addr.val[5], \
             status->dev_info.addr.type, ota_fail_str(status->reason), status->channel);
        aos_reboot();
    }
    break;

    default:
        LOGW(TAG, "Unkonw ota event %02x", event_id);
        return;
    }
}

static struct ota_server_cb board_ota_cb = {
    .device_event_cb   = board_ota_dev_event_cb,
};

/* gateway bt controler chip upgrade */
static int board_ota_bt_controler_upgrade(const char *image, uint32_t size)
{
    int ret = 0;
    firmware_info info = {0};

    device_info devices = {0};

    if (!image || !size) {
        LOGE(TAG, "image param is NULL");
        return -1;
    }

    LOGD(TAG, "bt image addr %p img size %d", image, size);

    ret = ota_server_init();

    if (ret && ret != -EALREADY) {
        LOGE(TAG, "ota server init failed %d", ret);
        return ret;
    }

    ota_server_cb_register(&board_ota_cb);

    info.address = (uint8_t *)image;
    info.image_type = 0x00;
    info.image_pos  = IMAGE_POS_RAM;
    info.size    = size;
    info.version = dfu_get_fota_file_app_version(info.address, info.image_pos);
    info.ota_chanel = OTA_CHANNEL_ON_HCI_UART;
    info.protocol = OTA_PROTOCOL_AIS;
    info.cb = board_ota_firmware_event_cb;
    int index  =  ota_server_upgrade_firmware_add(info);

    if (index < 0) {
        LOGE(TAG, "Add OTA firmware failed %d", index);
        return -1;
    }

    board_ota_bt_controller_idx = index;

    ret = ota_server_upgrade_device_add(index, 1, &devices);

    if (ret) {
        LOGE(TAG, "add ota dev failed, %d", ret);
        return ret;
    }

    return 0;
}

/* user api */

/* if ota for the gateway, handle 6220 image firstly, after the image ota success, handle image then */
int board_ota_gateway_upgrade(const void *image, uint32_t size)
{
    void    *bt_controller_image_addr = NULL;
    uint32_t bt_controller_image_size = 0;

    if (!image || !size) {
        LOGE(TAG, "invaild param");
        return -1;
    }

    int ret;

    void* temp_save_buffer = (void *)aos_zalloc(size);
    if(!temp_save_buffer) {
        LOGE(TAG,"Alloc buffer for image saving failed");
        return -1;
    }

    memcpy(temp_save_buffer, image, size);

#if defined(CONFIG_GW_USE_YOC_BOOTLOADER) && (CONFIG_GW_USE_YOC_BOOTLOADER)

    board_ota_main_controller_image = temp_save_buffer;
    board_ota_main_controller_size = size;

    ret = board_ota_occ_fota_bt_image_parse(image, size, (const char **)&bt_controller_image_addr, &bt_controller_image_size);
    if (ret) {
        bt_controller_image_size  = 0;
    }

#else
    board_ota_main_controller_image = temp_save_buffer;
    board_ota_main_controller_size = board_ota_get_main_controller_image_len(image, size);;

    /* the bt controller iamge is in the tail of the total image */
    if (size > board_ota_main_controller_size) {
        bt_controller_image_addr = (const char *)board_ota_main_controller_image + board_ota_main_controller_size;
        bt_controller_image_size = dfu_get_fota_file_size(bt_controller_image_addr, IMAGE_POS_RAM);
    }

#endif

    if (bt_controller_image_addr && bt_controller_image_size) {
        return board_ota_bt_controler_upgrade(bt_controller_image_addr, bt_controller_image_size);
    } else {
        return board_ota_main_controler_upgrade(board_ota_main_controller_image, board_ota_main_controller_size);
    }

    return 0;
}
#endif
