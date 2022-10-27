/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include "stdio.h"
#include "errno.h"
#include <stdint.h>
#include "ulog/ulog.h"
#include "ota_process.h"
#include "ota_server.h"
#include "aos/kernel.h"
#include "ais_ota/ais_ota_server.h"
#include "mesh_ota/mesh_ota_server.h"
#include "timer_port.h"

#define TAG "OTA_PROCESS"

#ifndef CONFIG_OTA_QUEUE_MAX_MSG_SIZE
#define CONFIG_OTA_QUEUE_MAX_MSG_SIZE 80
#endif

#ifndef CONFIG_OTA_VERSION_REPORT_TIMEOUT
#define CONFIG_OTA_VERSION_REPORT_TIMEOUT (300000) // ms
#endif

#ifndef CONFIG_OTA_TIMEOUT
#define CONFIG_OTA_TIMEOUT (180000) // ms
#endif

#ifndef CONFIG_BT_MESH_OTA_TIMEOUT
#define CONFIG_BT_MESH_OTA_TIMEOUT (1800000) // ms
#endif

#ifndef CONFIG_OTA_PROCESS_TIMEOUT
#define CONFIG_OTA_PROCESS_TIMEOUT (180000) // ms
#endif

#define OTA_CHECK_TIMOUT (1000) // ms

static ota_process_cmd_message g_ota_process_queue_message[CONFIG_OTA_QUEUE_MAX_MSG_SIZE] = { 0 };

static aos_queue_t g_ota_event_queue;
static ota_timer_t g_ota_check_timer;

static uint8_t  ota_process_unint_flag = 0;
static uint16_t firmware_index         = 0;

extern struct ota_server_cb *callback_list;

extern void device_list_mutex_lock();
extern void device_list_mutex_unlock();

#if defined(CONFIG_DEBUG) && CONFIG_DEBUG > 0
static char *ota_process_cmd_str(uint8_t process_cmd)
{
    switch (process_cmd) {
        case OTA_EVENT_ONGOING:
            return "ONGOING";
        case OTA_EVENT_SUCCESS:
            return "SUCCESS";
        case OTA_EVENT_FAILED:
            return "FAILED";
        case OTA_EVENT_SEND_CMD:
            return "SEND";
        case OTA_EVENT_STOP_CMD:
            return "STOP";
        case OTA_EVENT_EXIST_CMD:
            return "EXIST";
        default:
            return "UNKNOW";
    }
}
#endif

static int _ota_process_set_dev_ota_failed(ota_firmware *firmware_info, upgrade_device *failed_device,
                                           uint8_t failed_reason)
{
    int ret = 0;
    LOGE(TAG, "The ota device Unicast_addr:%04x,%02x:%02x:%02x:%02x:%02x:%02x type:%02x process timeout,reason %02x",
         failed_device->device.unicast_addr, failed_device->device.addr.val[5], failed_device->device.addr.val[4],
         failed_device->device.addr.val[3], failed_device->device.addr.val[2], failed_device->device.addr.val[1],
         failed_device->device.addr.val[0], failed_device->device.addr.type, failed_reason);
    ota_process_cmd_message message;
    message.event = OTA_EVENT_FAILED;
    memcpy(&message.data.success_data.dev_info, &failed_device->device, sizeof(device_info));
    message.data.fail_data.reason         = failed_reason;
    message.data.fail_data.old_ver        = failed_device->old_version;
    message.data.fail_data.new_ver        = OTA_VERSION_UNKNOW;
    message.data.fail_data.cost_time      = failed_device->download_finish_time - failed_device->start_time;
    message.data.fail_data.channel        = firmware_info->firmware.info.ota_chanel;
    message.data.fail_data.firmware_index = firmware_index;
    failed_device->ota_state              = OTA_STATE_FAILED;
    ret                                   = ota_process_cmd_set(&message);
    if (ret) {
        LOGE(TAG, "ais ota server send fail state fail");
        return ret;
    }
    return ret;
}

static void ota_check_timeout_cb(void *timer, void *arg)
{
    uint8_t         device_in_ota_flag           = 0;
    uint8_t         device_in_load_complete_flag = 0;
    ota_firmware *  firmware_info                = (ota_firmware *)arg;
    long long       time_now                     = aos_now_ms();
    upgrade_device *up_device                    = NULL;

    up_device = dev_list_dev_get(&firmware_info->firmware.dev_list_head);
    if (!up_device) {
        ota_timer_stop(&g_ota_check_timer);
        ota_timer_start(&g_ota_check_timer, OTA_CHECK_TIMOUT);
        return;
    }

    do {
        if (up_device->ota_state == OTA_STATE_ONGOING) {
            device_in_ota_flag = 1;
            break;
        } else if (up_device->ota_state == OTA_STATE_LOAD_COMPLETE) {
            device_in_load_complete_flag = 1;
        }
        if (device_in_ota_flag && device_in_load_complete_flag) {
            break;
        }
        up_device = dev_list_dev_get(&up_device->list);
    } while (up_device);

    up_device = dev_list_dev_get(&firmware_info->firmware.dev_list_head);

    do {
        if (up_device->ota_state == OTA_STATE_LOAD_COMPLETE) {
            long long compare_time = firmware_info->process_idle_time >= up_device->download_finish_time
                                         ? firmware_info->process_idle_time
                                         : up_device->download_finish_time;
            if (!device_in_ota_flag && (time_now - compare_time) >= CONFIG_OTA_VERSION_REPORT_TIMEOUT) {
                _ota_process_set_dev_ota_failed(firmware_info, up_device, OTA_FAIL_REPORT_VERSION_TIMEOUT);
                break;
            }
        } else if (up_device->ota_state == OTA_STATE_ONGOING) {
            if (((firmware_info->firmware.info.protocol == OTA_PROTOCOL_MESH)
                 && ((time_now - firmware_info->process_idle_time) >= CONFIG_BT_MESH_OTA_TIMEOUT))
                || ((firmware_info->firmware.info.protocol == OTA_PROTOCOL_AIS)
                    && ((time_now - firmware_info->process_idle_time) >= CONFIG_OTA_TIMEOUT)))
            {
                _ota_process_set_dev_ota_failed(firmware_info, up_device, OTA_FAIL_TIMEOUT);
                break;
            }
        } else if (up_device->ota_state == OTA_STATE_IDLE
                   && (time_now - firmware_info->process_idle_time) >= CONFIG_OTA_PROCESS_TIMEOUT)
        {
            if (!device_in_ota_flag && !device_in_load_complete_flag) {
                _ota_process_set_dev_ota_failed(firmware_info, up_device, OTA_FAIL_TIMEOUT);
                break;
            }
        }
        up_device = dev_list_dev_get(&up_device->list);
    } while (up_device);

    ota_timer_stop(&g_ota_check_timer);
    ota_timer_start(&g_ota_check_timer, OTA_CHECK_TIMOUT);
}

static int ota_process_init(ota_firmware *firmware)
{
    if (!firmware) {
        return -EINVAL;
    }

    int ret = 0;

    memset(g_ota_process_queue_message, 0, CONFIG_OTA_QUEUE_MAX_MSG_SIZE * sizeof(ota_process_cmd_message));
    ret = aos_queue_new(&g_ota_event_queue, (void *)g_ota_process_queue_message, sizeof(g_ota_process_queue_message),
                        sizeof(ota_process_cmd_message));
    if (ret != 0) {
        return ret;
    }
    ret = ota_timer_init(&g_ota_check_timer, ota_check_timeout_cb, firmware);
    if (ret != 0) {
        return ret;
    }
    ret = ota_timer_start(&g_ota_check_timer, OTA_CHECK_TIMOUT);
    if (ret != 0) {
        return ret;
    }
    ota_process_unint_flag = 0;

    return 0;
}

static void ota_process_uninit()
{
    if (!ota_process_unint_flag) {
        aos_queue_free(&g_ota_event_queue);
        ota_timer_free(&g_ota_check_timer);
        ota_process_unint_flag = 1;
    }
}

static int ota_process_inform_firmware_states(uint16_t firmware_index, uint8_t event)
{
    int                    ret = 0;
    ota_server_cmd_message cmd;
    cmd.event          = event;
    cmd.firmware_index = firmware_index;
    ret                = ota_server_cmd_set(&cmd);
    if (ret < 0) {
        LOGE(TAG, "Ota server cmd set failed %d", ret);
        return -1;
    }
    return 0;
}

static int ota_process_cmd_get(ota_process_cmd_message *cmd, unsigned int timeout)
{
    int ret = 0;
    if (ota_process_unint_flag) {
        LOGE(TAG, "Ota process has been rst");
        return -1;
    }
    unsigned int read_size = 0;
    if (!cmd || !aos_queue_is_valid(&g_ota_event_queue)) {
        return -EINVAL;
    }

    ret = aos_queue_recv(&g_ota_event_queue, timeout, (void *)cmd, (size_t *)&read_size);
    if (ret == 0 && read_size > 0) {
        return 0;
    } else {
        LOGE(TAG, "get queue message fail:%d", ret);
        return -1;
    }
}

static void ota_process_exit_process(uint8_t firmware_index, ota_firmware *new_firmware)
{
    LOGD(TAG, "Ota process exist process");
    upgrade_device *ota_device = NULL;
    ota_state_fail  fail_status;
    uint8_t         dev_info_type = DEV_LIST_FOUND_BY_MAC;

    if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_MESH) {
        dev_info_type = DEV_LIST_FOUND_BY_UNICAST_ADDR;
    }

    ota_device = dev_list_dev_get(&new_firmware->firmware.dev_list_head);
    while (ota_device) {
        if (ota_device->ota_state != OTA_STATE_SUCCESS) { // remider the USER
            fail_status.reason = OTA_FAIL_FIRMWARE_DEL;
            fail_status.old_ver
                = (ota_device->device.old_version == 0) ? OTA_VERSION_UNKNOW : ota_device->device.old_version;
            fail_status.new_ver        = OTA_VERSION_UNKNOW;
            fail_status.cost_time      = 0;
            fail_status.channel        = new_firmware->firmware.info.ota_chanel;
            fail_status.firmware_index = firmware_index;
            memcpy(&fail_status.dev_info, &ota_device->device, sizeof(device_info));
            ota_server_device_event_process(OTA_STATE_FAILED, &fail_status);
        }

        if (dev_list_rm_and_free_dev(&new_firmware->firmware.dev_list_head, ota_device->device, dev_info_type)) {
            LOGE(TAG, "Ota process exist dev rm failed");
        }
        ota_device = dev_list_dev_get(&new_firmware->firmware.dev_list_head);
    }
}

static void ota_process_thread(void *args)
{
    int           ret           = 0;
    ota_firmware *new_firmware  = NULL;
    uint8_t       dev_info_type = DEV_LIST_FOUND_BY_MAC;
    LOGD(TAG, "enter process thread");
    firmware_index = *(uint16_t *)args;

    new_firmware = ota_server_upgrade_firmware_get(firmware_index);
    if (!new_firmware) {
        LOGE(TAG, "Get firmware %d failed\r\n", firmware_index);
        goto EXIST;
    }

    if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_MESH) {
        dev_info_type = DEV_LIST_FOUND_BY_UNICAST_ADDR;
    }
    ota_process_cmd_message process_cmd;

    if (!new_firmware) {
        LOGE(TAG, "no firmware in ota process thread");
        goto EXIST;
    }

    ret = ota_process_init(new_firmware);
    if (ret < 0) {
        LOGE(TAG, "ota process init failed");
        goto EXIST;
    }

    LOGD(TAG, "Start Pocess firmware version: %04x  size: %dB by channel:%02x use protocal:%02x",
         new_firmware->firmware.info.version, new_firmware->firmware.info.size, new_firmware->firmware.info.ota_chanel,
         new_firmware->firmware.info.protocol);

    if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_AIS) {
        ret = ais_ota_server_init(new_firmware->firmware.info.ota_chanel);
        if (ret < 0 && ret != -EALREADY) {
            LOGE(TAG, "init ota server failed");
            goto EXIST;
        }

        ret = ais_ota_server_prepare(firmware_index, &new_firmware->firmware);
        if (ret) {
            LOGE(TAG, "ota server prepare failed");
            goto EXIST;
        } // TODO no dev ack at all
#if defined(CONFIG_BT_MESH_MODEL_BLOB_CLI) && CONFIG_BT_MESH_MODEL_BLOB_CLI > 0
    } else if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_MESH) {
        ret = mesh_ota_server_init(firmware_index, new_firmware->firmware.info.ota_chanel, &new_firmware->firmware);
        if (ret) {
            LOGE(TAG, "Mesh ota server prepare failed %d", ret);
            goto EXIST;
        } // TODO no dev ack at all
#endif
    } else {
        LOGE(TAG, "unsupport ota protocal");
        goto EXIST;
    }

    new_firmware->process_idle_time = aos_now_ms();

    while (1) {
        ret = ota_process_cmd_get(&process_cmd, AOS_WAIT_FOREVER);
        if (ret < 0) {
            LOGE(TAG, "ota get event status failed");
            goto EXIST;
        }

        new_firmware->process_idle_time = aos_now_ms();

        LOGD(TAG, "Ota process cmd:%s", ota_process_cmd_str(process_cmd.event));
        switch (process_cmd.event) {
            case OTA_EVENT_ONGOING: {
                ota_server_device_event_process(OTA_STATE_ONGOING, &process_cmd.data);
                LOGD(TAG, "Start OTA the firmware for unicast_addr:%04x dev %02x:%02x:%02x:%02x:%02x:%02x %02x",
                     process_cmd.data.ongoing_data.dev_info.unicast_addr,
                     process_cmd.data.ongoing_data.dev_info.addr.val[5],
                     process_cmd.data.ongoing_data.dev_info.addr.val[4],
                     process_cmd.data.ongoing_data.dev_info.addr.val[3],
                     process_cmd.data.ongoing_data.dev_info.addr.val[2],
                     process_cmd.data.ongoing_data.dev_info.addr.val[1],
                     process_cmd.data.ongoing_data.dev_info.addr.val[0],
                     process_cmd.data.ongoing_data.dev_info.addr.type);
            } break;
            case OTA_EVENT_SUCCESS: {
                ota_server_device_event_process(OTA_STATE_SUCCESS, &process_cmd.data);
                if (dev_list_rm_and_free_dev(&new_firmware->firmware.dev_list_head,
                                             process_cmd.data.success_data.dev_info, dev_info_type))
                {
                    LOGE(TAG, "remove dev failed\r\n");
                }
                if (!dev_list_dev_get(&new_firmware->firmware.dev_list_head)) {
                    goto EXIST;
                }
            } break;
            case OTA_EVENT_FAILED: {
                if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_AIS) {
                    ais_ota_server_channel_disconnect();
                }
                ota_server_device_event_process(OTA_STATE_FAILED, &process_cmd.data);
                if (!dev_list_rm_and_free_dev(&new_firmware->firmware.dev_list_head,
                                              process_cmd.data.fail_data.dev_info, dev_info_type))
                {
                    LOGE(TAG, "remove dev failed");
                }
                if (!dev_list_dev_get(&new_firmware->firmware.dev_list_head)) {
                    goto EXIST;
                }
            } break;
            case OTA_EVENT_SEND_CMD: {
                if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_AIS) {
                    ais_ota_server_send_start();
                }
            } break;
            case OTA_EVENT_STOP_CMD: {
                if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_AIS) {
                    ais_ota_server_ota_stop();
                }
#if defined(CONFIG_BT_MESH_MODEL_BLOB_CLI) && CONFIG_BT_MESH_MODEL_BLOB_CLI > 0
                else if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_MESH)
                {
                    mesh_ota_server_ota_stop();
                }
#endif
            } break;
            case OTA_EVENT_EXIST_CMD: {
                ota_process_exit_process(firmware_index, new_firmware);
                goto EXIST;
            } break;
            default:
                break;
        }
    }
EXIST:
    if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_AIS) {
        ais_ota_server_reset();
    }
#if defined(CONFIG_BT_MESH_MODEL_BLOB_CLI) && CONFIG_BT_MESH_MODEL_BLOB_CLI > 0
    else if (new_firmware->firmware.info.protocol == OTA_PROTOCOL_MESH)
    {
        mesh_ota_server_reset();
    }
#endif
    ota_process_uninit();
    ota_process_inform_firmware_states(firmware_index, FIRMWARE_STATE_IDLE);
    LOGI(TAG, "firware can free now");
    aos_task_exit(-1);
}

int32_t ota_process_task_create(ktask_t *task_handle, void *arg, uint32_t stack, uint32_t priority)
{
    static uint32_t firmware_index       = 0;
    static char     ota_process_name[21] = { 0x00 };
    snprintf(ota_process_name, sizeof(ota_process_name) - 1, "ota_process_%d", firmware_index++);
    return krhino_task_dyn_create(&task_handle, ota_process_name, arg, priority, 0, stack / sizeof(cpu_stack_t),
                                  ota_process_thread, 1u);
}

int ota_process_cmd_set(ota_process_cmd_message *cmd)
{
    int ret = 0;

    if (ota_process_unint_flag) {
        LOGE(TAG, "Ota process has been rst");
        return -1;
    }

    if (!cmd || !aos_queue_is_valid(&g_ota_event_queue)) {
        return -EINVAL;
    }
    ret = aos_queue_send(&g_ota_event_queue, (void *)cmd, sizeof(ota_process_cmd_message));
    if (0 != ret) {
        LOGE(TAG, "send queue message fail:%d", ret);
        return -1;
    }
    return 0;
}
