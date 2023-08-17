/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include "dfu_port.h"
#include "ota_server.h"
#include "ota_process.h"
#include "errno.h"
#include "ulog/ulog.h"
#include "crc/crc16.h"
#include "dfu_port.h"
#include "aos/kernel.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include <provisioner_main.h>
#endif

#define TAG "OTA SERVICE"

#ifndef CONFIG_OTA_SERVER_QUEUE_MAX_MSG_SIZE
#define CONFIG_OTA_SERVER_QUEUE_MAX_MSG_SIZE 20
#endif

enum
{
    FIRMWARE_REMOVED                  = 0x00,
    FIRMWARE_ADDED                    = 0x01,
    FIRMWARE_WAITING_REMOVE           = 0x02,
    FIRMWARE_ADDED_DEV_NOT_IN_PROCESS = 0x03,
    FIRMWARE_ADDED_DEV_IN_PROCESS     = 0x04,
} firmware_flag_en;

enum
{
    STATE_IDLE,
    STATE_ONGOING,
};

enum
{
    TASK_NOT_BUSY = 0x00,
    TASK_BUSY     = 0x01,
};

typedef struct {
    ktask_t *task_handle;
    uint8_t  task_state;
    uint8_t  firmware_index;
} ota_process_thread_t;

typedef struct _ota_server_ctx {
    uint8_t init_flag;
    // uint8_t states;
    ota_firmware         firmwares[DEF_MAX_OTA_IMAGE];
    ota_process_thread_t threads[DEF_MAX_OTA_PROCESS_THREAD];
    aos_queue_t          ota_server_queue;
    aos_mutex_t          device_list_mutex;
    aos_task_t           task;
} ota_server_ctx;

static ota_server_cmd_message queue_message[CONFIG_OTA_SERVER_QUEUE_MAX_MSG_SIZE];

static ota_server_ctx g_ota_server_ctx;

struct ota_server_cb *ota_callback_list;

void dev_list_init(slist_t *list)
{
    slist_init(list);
}

upgrade_device *dev_list_dev_search(slist_t *list_head, device_info dev_info, uint8_t type)
{
    if (!list_head) {
        return NULL;
    }
    upgrade_device *device_iter_temp = NULL;
    slist_t *       tmp;
    aos_mutex_lock(&g_ota_server_ctx.device_list_mutex, AOS_WAIT_FOREVER);
    slist_for_each_entry_safe(list_head, tmp, device_iter_temp, upgrade_device, list)
    {
        if (!device_iter_temp) {
            aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);
            return NULL;
        } else {
            if (type == DEV_LIST_FOUND_BY_MAC_UNICAST_ADDR
                && !memcmp(&device_iter_temp->device.addr, &dev_info.addr, sizeof(mac_t))
                && device_iter_temp->device.unicast_addr == dev_info.unicast_addr)
            {
                aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);
                return device_iter_temp;
            } else if (type == DEV_LIST_FOUND_BY_MAC
                       && !memcmp(&device_iter_temp->device.addr, &dev_info.addr, sizeof(mac_t))) {
                aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);
                return device_iter_temp;
            } else if (type == DEV_LIST_FOUND_BY_UNICAST_ADDR
                       && device_iter_temp->device.unicast_addr == dev_info.unicast_addr) {
                aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);
                return device_iter_temp;
            }
        }
    }
    aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);
    return NULL;
}

upgrade_device *dev_list_dev_get(slist_t *list_head)
{
    if (!list_head) {
        return NULL;
    }
    upgrade_device *device = NULL;
    aos_mutex_lock(&g_ota_server_ctx.device_list_mutex, AOS_WAIT_FOREVER);
    device = slist_first_entry(list_head, upgrade_device, list);
    aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);
    return device;
}

int dev_list_add_dev(slist_t *list_head, upgrade_device *dev, uint8_t type)
{
    if (!list_head) {
        return -1;
    }
    if (dev_list_dev_search(list_head, dev->device, type)) {
        LOGE(TAG, "device already exist");
        return -EALREADY;
    }
    aos_mutex_lock(&g_ota_server_ctx.device_list_mutex, AOS_WAIT_FOREVER);
    slist_add_tail(&dev->list, list_head);
    aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);
    return 0;
}

upgrade_device *dev_list_rm_dev(slist_t *list_head, device_info dev_info, uint8_t type)
{
    if (!list_head) {
        LOGE(TAG, "list head null");
        return NULL;
    }
    upgrade_device *dev = dev_list_dev_search(list_head, dev_info, type);
    if (!dev) {
        LOGE(TAG, "no dev found");
        return NULL;
    }
    aos_mutex_lock(&g_ota_server_ctx.device_list_mutex, AOS_WAIT_FOREVER);
    slist_del(&dev->list, list_head);
    aos_mutex_unlock(&g_ota_server_ctx.device_list_mutex);

    return dev;
}

int dev_list_rm_and_free_dev(slist_t *list_head, device_info dev_info, uint8_t type)
{
    upgrade_device *dev = dev_list_rm_dev(list_head, dev_info, type);
    if (!dev) {
        LOGE(TAG, "Device not found in the list");
        return -1;
    }
    aos_free(dev);
    return 0;
}

void ota_server_firmware_event_process(firmware_info *firmware, ota_firmware_state_en event, void *data)
{
    if (firmware && firmware->cb) {
        firmware->cb(event, data);
    }
}

static uint16_t index_temp = 0;

static int ota_server_process_firmware(uint16_t firmware_index)
{
    int ret    = 0;
    index_temp = firmware_index;
    for (int i = 0; i < DEF_MAX_OTA_PROCESS_THREAD; i++) {
        if (!g_ota_server_ctx.threads[i].task_handle) {
            ret = ota_process_task_create(g_ota_server_ctx.threads[i].task_handle, (void *)&index_temp,
                                          CONFIG_OTA_PROCESS_STACK_SIZE, CONFIG_OTA_PROCESS_PRIO);
            if (ret) {
                LOGE(TAG, "create task fail");
                return ret;
            } else {
                LOGD(TAG, "Start ota process id: %02x for firmware: %02x", i, firmware_index);
                g_ota_server_ctx.threads[i].firmware_index                   = firmware_index;
                g_ota_server_ctx.threads[i].task_state                       = TASK_BUSY;
                g_ota_server_ctx.firmwares[firmware_index].flags             = FIRMWARE_ADDED_DEV_IN_PROCESS;
                g_ota_server_ctx.firmwares[firmware_index].process_idle_time = aos_now_ms();
                return 0;
            }
        }
    }
    LOGE(TAG, "No OTA process thread for the firmware %d", firmware_index);
    // g_ota_server_ctx.states = TASK_BUSY;
    return -1;
}

static int ota_server_get_unprocessed_firmware()
{
    for (int i = 0; i < DEF_MAX_OTA_IMAGE; i++) {
        if (g_ota_server_ctx.firmwares[i].flags == FIRMWARE_ADDED_DEV_NOT_IN_PROCESS) {
            return i;
        }
    }
    return -1;
}

static int ota_server_firmware_idle_event_process(uint16_t firmware_index)
{
    int           ret              = 0;
    ota_firmware *ota_fir          = &g_ota_server_ctx.firmwares[firmware_index];
    uint16_t      to_process_index = 0;
    LOGD(TAG, "Process firmware index %02x state %02x", firmware_index, ota_fir->flags);

    if (ota_fir->flags == FIRMWARE_ADDED_DEV_NOT_IN_PROCESS) { // create a task process it
        to_process_index = firmware_index;
    } else if (ota_fir->flags == FIRMWARE_ADDED_DEV_IN_PROCESS || ota_fir->flags == FIRMWARE_WAITING_REMOVE) { //
        if (ota_fir->flags == FIRMWARE_ADDED_DEV_IN_PROCESS) {
            ota_fir->flags = FIRMWARE_ADDED;
            ota_firmware_state_data data;
            data.firmware_index = firmware_index;
            ota_server_firmware_event_process(&ota_fir->firmware.info, FIRMWARE_STATE_END_USE, &data);
        } else if (ota_fir->flags == FIRMWARE_WAITING_REMOVE) {
            ota_firmware_state_data data;
            data.firmware_index = firmware_index;
            ota_fir->flags      = FIRMWARE_REMOVED;
            ota_server_firmware_event_process(&ota_fir->firmware.info, FIRMWARE_STATE_REMOVED, &data);
            memset(&ota_fir->firmware.info, 0, sizeof(ota_fir->firmware.info));
        }

        int the_next_index = ota_server_get_unprocessed_firmware();
        if (the_next_index < 0) {
            LOGE(TAG, "No firmware need process");
            return 0;
        }
        to_process_index = the_next_index;
    }
    ret = ota_server_process_firmware(to_process_index);
    if (ret) {
        LOGE(TAG, "OTA process firmware failed");
        return -1;
    }
#if 1
    ota_firmware_state_data data;
    data.firmware_index = to_process_index;
    ota_server_firmware_event_process(&ota_fir->firmware.info, FIRMWARE_STATE_IN_USE, &data);
#endif
    return 0;
}

static int search_and_put_firmware(firmware_info *firmware)
{
    int unused_index = -1;
    for (int i = 0; i < DEF_MAX_OTA_IMAGE; i++) {
        if (g_ota_server_ctx.firmwares[i].flags != FIRMWARE_REMOVED) { // 0 1
            if (firmware->address == g_ota_server_ctx.firmwares[i].firmware.info.address
                && firmware->size == g_ota_server_ctx.firmwares[i].firmware.info.size
                && firmware->version == g_ota_server_ctx.firmwares[i].firmware.info.version
                && firmware->protocol == g_ota_server_ctx.firmwares[i].firmware.info.protocol)
            {
                return i;
            } else if (firmware->address >= g_ota_server_ctx.firmwares[i].firmware.info.address
                       && firmware->address <= g_ota_server_ctx.firmwares[i].firmware.info.address
                                                   + g_ota_server_ctx.firmwares[i].firmware.info.size)
            {
                LOGE(TAG, "The firmware address is inside the existed firmware %d,may be invalid", i);
                return -1;
            }
        } else {
            if (unused_index == -1) {
                unused_index = i;
            }
        }
    }

    if (unused_index == -1) {
        LOGE(TAG, "No vaild space left for new firmware");
        return -1;
    }

    memcpy(&g_ota_server_ctx.firmwares[unused_index].firmware.info, firmware, sizeof(firmware_info));
    dev_list_init(&g_ota_server_ctx.firmwares[unused_index].firmware.dev_list_head);
    dev_list_init(&g_ota_server_ctx.firmwares[unused_index].firmware.cancel_list_head);
    return unused_index;
}

static inline int _upgrade_firemware_read(firmware_info *firmware, uint32_t offset, uint32_t length, uint8_t *buffer)
{
    if (firmware->image_pos == IMAGE_POS_FLASH) {
        return dfu_image_read(firmware->address, offset, length, buffer);
    } else { // RAM
        memcpy(buffer, firmware->address + offset, length);
    }
    return 0;
}

static int ota_server_upgrade_firmware_crc(uint16_t index, uint16_t *crc16)
{
    uint8_t        read_buffer[256];
    uint32_t       offset    = 0;
    int            ret       = 0;
    uint32_t       read_size = sizeof(read_buffer);
    firmware_info *firmware  = &g_ota_server_ctx.firmwares[index].firmware.info;

    while (read_size == sizeof(read_buffer)) {
        read_size = (firmware->size - offset) < sizeof(read_buffer) ? firmware->size - offset : sizeof(read_buffer);
        ret       = _upgrade_firemware_read(firmware, offset, read_size, read_buffer);
        if (ret < 0) {
            return -1;
        }
        if (!offset) {
            *crc16 = util_crc16_ccitt(read_buffer, read_size, NULL);
        } else {
            *crc16 = util_crc16_ccitt(read_buffer, read_size, crc16);
        }
        offset += read_size;
    }
    LOGD(TAG, "ota image crc calu success %02x", *crc16);
    return 0;
}

static int ota_server_cmd_get(ota_server_cmd_message *cmd, unsigned int timeout)
{
    int          ret       = 0;
    unsigned int read_size = 0;
    if (!cmd || !aos_queue_is_valid(&g_ota_server_ctx.ota_server_queue)) {
        return -EINVAL;
    }

    ret = aos_queue_recv(&g_ota_server_ctx.ota_server_queue, timeout, (void *)cmd, (size_t *)&read_size);
    if (ret == 0 && read_size > 0) {
        return 0;
    } else {
        LOGE(TAG, "get queue message fail:%d", ret);
        return -1;
    }
}

static void ota_server_task(void *arg)
{
    int                    ret = 0;
    ota_server_cmd_message cmd;
    while (1) {
        ret = ota_server_cmd_get(&cmd, AOS_WAIT_FOREVER);
        if (ret < 0) {
            LOGE(TAG, "Get ota server cmd get failed");
        }
        LOGD(TAG, "Ota server event:%4x firmware index:%04x ", cmd.event, cmd.firmware_index);
        switch (cmd.event) {
            case FIRMWARE_STATE_IDLE: {
                ota_server_firmware_idle_event_process(cmd.firmware_index);
            } break;
            case FIRMWARE_STATE_IN_USE: {
                // Do Nothing

            } break;
            default:
                break;
        }
    }
}

// API FOR OTA MODULE
void ota_server_device_event_process(ota_device_state_en event, void *data)
{
    struct ota_server_cb *cb;

    for (cb = ota_callback_list; cb; cb = cb->_next) {
        if (cb->device_event_cb) {
            cb->device_event_cb(event, data);
        }
    }
}

int ota_server_cmd_set(ota_server_cmd_message *cmd)
{
    int ret = 0;
    if (!cmd || !aos_queue_is_valid(&g_ota_server_ctx.ota_server_queue)) {
        return -EINVAL;
    }
    ret = aos_queue_send(&g_ota_server_ctx.ota_server_queue, (void *)cmd, sizeof(ota_server_cmd_message));
    if (0 != ret) {
        LOGE(TAG, "send queue message fail:%d", ret);
        return -1;
    }
    return 0;
}

// API FOR USER
int ota_server_init()
{
    if (g_ota_server_ctx.init_flag) {
        return -EALREADY;
    }

    int ret = 0;
    memset(&g_ota_server_ctx, 0x00, sizeof(ota_server_ctx));

    memset(queue_message, 0, CONFIG_OTA_SERVER_QUEUE_MAX_MSG_SIZE * sizeof(ota_server_cmd_message));

    ret = aos_queue_new(&g_ota_server_ctx.ota_server_queue, (void *)queue_message, sizeof(queue_message),
                        sizeof(ota_server_cmd_message));
    if (ret != 0) {
        return ret;
    }

    ret = aos_mutex_new(&g_ota_server_ctx.device_list_mutex);
    if (ret != 0) {
        aos_queue_free(&g_ota_server_ctx.ota_server_queue);
        return ret;
    }

    ret = aos_task_new_ext(&g_ota_server_ctx.task, "ota_server_task", ota_server_task, NULL,
                           CONFIG_OTA_SERVER_STACK_SIZE, CONFIG_OTA_SERVER_PRIO);
    if (ret < 0) {
        LOGE(TAG, "Ota server task create failed");
        aos_mutex_free(&g_ota_server_ctx.device_list_mutex);
        aos_queue_free(&g_ota_server_ctx.ota_server_queue);
        return -1;
    }

    // g_ota_server_ctx.states  = OTA_SERVER_AVAILABLE;
    g_ota_server_ctx.init_flag = 1;
    return 0;
}

void ota_server_cb_register(struct ota_server_cb *cb)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return;
    }
    if (ota_callback_list) {
        if (ota_callback_list == cb) {
            return;
        }
        for (struct ota_server_cb *temp = ota_callback_list; temp->_next != NULL; temp = temp->_next) {
            if (temp->_next == cb) {
                return;
            }
        }
    }
    cb->_next         = ota_callback_list;
    ota_callback_list = cb;
}

void ota_server_cb_unregister(struct ota_server_cb *cb)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return;
    }

    struct ota_server_cb *temp = ota_callback_list;

    if (!ota_callback_list) {
        return;
    }

    if (cb == ota_callback_list) {
        ota_callback_list = cb->_next;
        return;
    }

    for (; temp->_next != NULL; temp = temp->_next) {
        if (temp->_next == cb) {
            temp->_next = cb->_next;
            cb->_next   = NULL;
        }
    }
}

int ota_server_upgrade_firmware_add(firmware_info firware)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return -1;
    }

    int index = 0;
    int ret   = 0;
    index     = search_and_put_firmware(&firware);
    if (index < 0) {
        LOGE(TAG, "add firmware failed");
        return -1;
    }

    ret = ota_server_upgrade_firmware_crc(index, &g_ota_server_ctx.firmwares[index].firmware.info.crc16);
    if (ret < 0) {
        LOGE(TAG, "Crc calu failed for firmware");
        return -1;
    }

    if (g_ota_server_ctx.firmwares[index].firmware.info.protocol == OTA_PROTOCOL_MESH) {
        if (firware.multicast_addr < 0xD000 || firware.multicast_addr > 0xff00) {
            int addr_random = rand();
            g_ota_server_ctx.firmwares[index].firmware.info.multicast_addr
                = 0xD000 | (g_ota_server_ctx.firmwares[index].firmware.info.crc16 & 0x0FF0) | (addr_random & 0x000F);
            LOGD(TAG, "Set mesh ota random multiaddr %04x",
                 g_ota_server_ctx.firmwares[index].firmware.info.multicast_addr);
        }
    }

    g_ota_server_ctx.firmwares[index].flags = FIRMWARE_ADDED;

    LOGI(TAG, "firmware %d add", index);
    return index;
}

int ota_server_upgrade_firmware_rm(uint16_t index)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return -1;
    }

    // int ret = 0;

    if (index >= DEF_MAX_OTA_IMAGE) {
        LOGE(TAG, "Valid firmware index");
        return -EINVAL;
    }

    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_REMOVED) {
        return -EALREADY;
    }

    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_ADDED_DEV_IN_PROCESS) {
        LOGD(TAG, "firmware is being used,inform the ota process to stop");
        g_ota_server_ctx.firmwares[index].flags = FIRMWARE_WAITING_REMOVE;
        ota_process_cmd_message cmd;
        cmd.event = OTA_EVENT_EXIST_CMD;
        ota_process_cmd_set(&cmd);
        LOGD(TAG, "OTA firmware %d state now %d", index, g_ota_server_ctx.firmwares[index].flags);
    } else {
        ota_firmware_state_data data;
        data.firmware_index = index;
        ota_server_firmware_event_process(&g_ota_server_ctx.firmwares[index].firmware.info, FIRMWARE_STATE_REMOVED,
                                          &data);
        g_ota_server_ctx.firmwares[index].flags = FIRMWARE_REMOVED;
        LOGD(TAG, "Firmware is idle,remove it from firmawre queue directly");
    }
    return 0;
}

int ota_server_upgrade_firmware_read(uint16_t index, uint32_t offset, uint32_t length, uint8_t *buffer)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return -1;
    }
    if (index >= DEF_MAX_OTA_IMAGE) {
        LOGE(TAG, "Valid firmware index %d", index);
        return -EINVAL;
    }
    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_REMOVED) {
        LOGE(TAG, "The firmware not exist");
        return -1;
    }
    firmware_info *firmware = &g_ota_server_ctx.firmwares[index].firmware.info;

    return _upgrade_firemware_read(firmware, offset, length, buffer);
}

ota_firmware *ota_server_upgrade_firmware_get(uint16_t index)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return NULL;
    }

    if (index >= DEF_MAX_OTA_IMAGE) {
        LOGE(TAG, "Valid firmware index %d", index);
        return NULL;
    }

    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_REMOVED) {
        LOGE(TAG, "The firmware index %d not exist", index);
        return NULL;
    }
    return &g_ota_server_ctx.firmwares[index];
}

int ota_server_upgrade_device_add(uint16_t index, uint16_t device_list_size, device_info *devices_list)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return -1;
    }

    if (index >= DEF_MAX_OTA_IMAGE) {
        LOGE(TAG, "Valid firmware index %d", index);
        return -EINVAL;
    }

    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_REMOVED) {
        LOGE(TAG, "The firmware index %d not exist", index);
        return -1;
    }

    if (!devices_list || !device_list_size) {
        return -EINVAL;
    }

    int             ret           = 0;
    uint8_t         dev_info_type = DEV_LIST_FOUND_BY_MAC;
    ota_firmware *  ota_fir       = &g_ota_server_ctx.firmwares[index];
    upgrade_device *new_dev       = NULL;

    if (ota_fir->firmware.info.protocol == OTA_PROTOCOL_MESH) {
        dev_info_type = DEV_LIST_FOUND_BY_UNICAST_ADDR;
        if (ota_fir->flags == FIRMWARE_ADDED_DEV_IN_PROCESS) {
            LOGE(TAG, "Firmawre Already in process,not allowed add new device when use mesh ota protocal");
            return -ENOTSUP;
        }
    }

    for (int i = 0; i < device_list_size; i++) {
        new_dev = (upgrade_device *)aos_zalloc(sizeof(upgrade_device)); // TODO release it
        if (!new_dev) {
            LOGE(TAG, "malloc new dev failed");
            return -ENOMEM;
        }

        /*0 - public addr; 1 - random addr*/
        if (devices_list[i].addr.type > 1) {
            LOGE(TAG, "Invalid addr type %02x", devices_list[i].addr.type);
            return -EINVAL;
        }

        memcpy(&new_dev->device, &devices_list[i], sizeof(device_info));

        new_dev->device.old_version = devices_list[i].old_version;

#ifdef CONFIG_BT_MESH_PROVISIONER /* if the added device's mac address is invaild in mesh ota, then try get mac adress \
                                     by unicast address */
        uint8_t addr_comp[6] = { 0x0 };
        if (ota_fir->firmware.info.protocol == OTA_PROTOCOL_MESH && !memcmp(new_dev->device.addr.val, addr_comp, 6)) {
            struct bt_mesh_node_t *node = bt_mesh_provisioner_get_node_info(devices_list[i].unicast_addr);
            if (!node) {
                LOGE(TAG, "Node %04 not find", devices_list[i].unicast_addr);
                return -1;
            }
            memcpy(new_dev->device.addr.val, node->addr_val, 6);
        }
#endif
        ret = dev_list_add_dev(&ota_fir->firmware.dev_list_head, new_dev, dev_info_type);
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "add device failed");
            aos_free(new_dev);
            return -1;
        } else if (ret == -EALREADY) {
            aos_free(new_dev);
        }
    }

    if (ota_fir->flags == FIRMWARE_ADDED) {
        ota_fir->flags = FIRMWARE_ADDED_DEV_NOT_IN_PROCESS;
    }

    if (ota_fir->flags == FIRMWARE_ADDED_DEV_NOT_IN_PROCESS && ota_fir->firmware.info.protocol != OTA_PROTOCOL_MESH) {
        ota_server_cmd_message cmd = {
            .event          = FIRMWARE_STATE_IDLE,
            .firmware_index = index,
        };
        ota_server_cmd_set(&cmd);
    }

    return 0;
}

int ota_server_upgrade_start(uint16_t index)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return -1;
    }

    if (index >= DEF_MAX_OTA_IMAGE) {
        LOGE(TAG, "Valid firmware index %d", index);
        return -EINVAL;
    }

    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_REMOVED) {
        LOGE(TAG, "The firmware index %d not exist", index);
        return -1;
    }

    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_ADDED_DEV_IN_PROCESS) {
        LOGE(TAG, "Firmawre Already in process");
        return -EALREADY;
    }

    if (!dev_list_dev_get(&g_ota_server_ctx.firmwares[index].firmware.dev_list_head)) {
        LOGE(TAG, "No dev need ota for the firmware");
        return -EALREADY;
    }

    ota_firmware *ota_fir = &g_ota_server_ctx.firmwares[index];
    if (ota_fir->firmware.info.protocol == OTA_PROTOCOL_MESH) {
        if (ota_fir->flags == FIRMWARE_ADDED_DEV_IN_PROCESS) {
            LOGE(TAG, "Firmawre Already in process");
            return -ENOTSUP;
        }
    }

    ota_server_cmd_message cmd = {
        .event          = FIRMWARE_STATE_IDLE,
        .firmware_index = index,
    };
    ota_server_cmd_set(&cmd);

    return 0;
}

int ota_server_upgrade_device_rm(uint16_t index, uint16_t device_list_size, device_info *devices_list)
{
    if (!g_ota_server_ctx.init_flag) {
        LOGE(TAG, "ota server not init");
        return -1;
    }

    if (index >= DEF_MAX_OTA_IMAGE) {
        LOGE(TAG, "Valid firmware index %d", index);
        return -EINVAL;
    }

    if (g_ota_server_ctx.firmwares[index].flags == FIRMWARE_REMOVED) {
        LOGE(TAG, "The firmware index %d not exist", index);
        return -1;
    }

    if (!devices_list || !device_list_size) {
        return -EINVAL;
    }

    int             i              = 0;
    uint8_t         dev_info_type  = DEV_LIST_FOUND_BY_MAC;
    upgrade_device *dev            = NULL;
    uint8_t         send_stop_flag = 0;

    ota_firmware *ota_fir = &g_ota_server_ctx.firmwares[index];
    if (ota_fir->firmware.info.protocol == OTA_PROTOCOL_MESH) {
        dev_info_type = DEV_LIST_FOUND_BY_UNICAST_ADDR;
    }

    for (i = 0; i < device_list_size; i++) {
        dev = dev_list_dev_search(&ota_fir->firmware.dev_list_head, devices_list[i], dev_info_type);
        if (dev) {
            if (dev->ota_state == OTA_STATE_IDLE) {
                dev_list_rm_and_free_dev(&ota_fir->firmware.dev_list_head, dev->device, dev_info_type);
                LOGD(TAG, "device is idle,remove it from ota dev list directly");
            } else {
                dev_list_rm_dev(&ota_fir->firmware.dev_list_head, dev->device, dev_info_type);
                dev_list_add_dev(&ota_fir->firmware.cancel_list_head, dev, dev_info_type);
                send_stop_flag = 1;
                LOGD(TAG, "device %4x is being ota,inform the ota process to stop", dev->device.addr); //
            }
        }
    }

    if (send_stop_flag) {
        ota_process_cmd_message cmd;
        cmd.event = OTA_EVENT_STOP_CMD;
        ota_process_cmd_set(&cmd);
    }

    return 0;
}
