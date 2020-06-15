/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/list.h>
#include <aos/kernel.h>
#include <aos/kv.h>
#include <yoc/netio.h>
#include <yoc/fota.h>
#include <aos/log.h>
#include <aos/debug.h>

#define TAG "fota"

typedef struct fota_netio_list {
    slist_t next;
    const fota_cls_t *cls;
} fota_cls_node_t;

static AOS_SLIST_HEAD(fota_cls_list);
static fota_info_t fota_info;

int fota_register(const fota_cls_t *cls)
{
    fota_cls_node_t *node = malloc(sizeof(fota_cls_node_t));

    if (node) {
        node->cls = cls;
        slist_add_tail(&node->next, &fota_cls_list);
        return 0;
    }

    return -1;
}

fota_t *fota_open(const char *fota_name, const char *dst, fota_event_cb_t event_cb)
{
    fota_cls_node_t *node;
    fota_t *fota = NULL;

    slist_for_each_entry(&fota_cls_list, node, fota_cls_node_t, next) {
        if (strcmp(node->cls->name, fota_name) == 0) {
            fota = aos_zalloc_check(sizeof(fota_t));
            fota->to_path = strdup(dst);
            fota->cls = node->cls;
            fota->event_cb = event_cb;
            if (fota->cls->init)
                fota->cls->init();
            
            // LOGD(TAG, "fota: %x path:%s", fota, fota->to_path);
            break;
        }
    }
    aos_sem_new(&fota->sem, 0);
    aos_event_new(&fota->do_check_event, 0);
    fota->auto_check_en = 1;
    return fota;
}

static int fota_version_check(fota_t *fota, fota_info_t *info) {
    if (fota->cls->version_check)
        return fota->cls->version_check(info);

    return -1;
}

int fota_upgrade(fota_t *fota)
{
    if (fota->running || fota->status == FOTA_DOWNLOAD) //?
        return -1;

    fota->quit = 0;


    fota->buffer = aos_malloc(512);
    fota->from = netio_open(fota_info.fota_url);
    fota->to = netio_open(fota->to_path);

    if (fota->buffer == NULL || fota->from == NULL || fota->to == NULL) {
        if (fota->buffer == NULL) {
            LOGD(TAG, "fota->buffer e");
        } else if (fota->from == NULL) {
            LOGD(TAG, "fota->from e");
        } else if (fota->to == NULL) {
            LOGD(TAG, "fota->to e");
        }
        goto error;
    }

    if (aos_kv_getint("fota_offset", &fota->offset) < 0) {
        aos_kv_setint("fota_offset", 0);
        fota->offset = 0;
    }

    LOGI(TAG, "FOTA seek %d", fota->offset);

    if (netio_seek(fota->from, fota->offset, SEEK_SET) != 0) {
        LOGD(TAG, "from seek error");
        goto error;
    }

    if (netio_seek(fota->to, fota->offset, SEEK_SET) != 0) {
        LOGD(TAG, "to seek error");
        goto error;
    }

    fota->status = FOTA_DOWNLOAD;
    return 0;

error:
    if (fota->buffer) {
        aos_free(fota->buffer);
        fota->buffer = NULL;
    }
    if (fota->from) {
        netio_close(fota->from);
        fota->from = NULL;
    }
    if (fota->to) {
        netio_close(fota->to);
        fota->to = NULL;
    }

    return -1;
}

static void fota_task(void *arg)
{
    fota_t *fota = (fota_t *)arg;
    int retry = fota->retry_count;
    unsigned int flag;

    fota->quit = 0;
    // LOGD(TAG, "fota_task start: %s", fota->to_path);
    while (!fota->quit) {
        if (fota->status == FOTA_INIT) {
            LOGD(TAG, "fota_task FOTA_INIT! wait");
            aos_event_get(&fota->do_check_event, 0x01, AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
            if (fota_version_check(fota, &fota_info) == 0) {
                if (fota->event_cb && fota->event_cb(arg, FOTA_EVENT_VERSION) != 0) {
                    LOGD(TAG, "fota_upgrade 1!");  
                    //aos_sem_signal(&fota->do_check_sem);
                } else {
                    LOGI(TAG, "fota_upgrade 2!");
                    if (fota_upgrade(fota) < 0) {
                        LOGE(TAG, "fota_upgrade fail");
                    }
                }
                continue;
            }
        } else if (fota->status == FOTA_DOWNLOAD) {
            LOGD(TAG, "fota_task FOTA_DOWNLOAD! f:%d t:%d", fota->from->size, fota->to->offset);
            int size = netio_read(fota->from, fota->buffer, 512, 3000);
            LOGD(TAG, "read: %d", size);
            if (size < 0) {
                // LOGD(TAG, "read size < 0 %d", size);
                if (size == -2) {
                    LOGW(TAG, "reconnect again");
                    continue;
                }
                if (fota->event_cb && fota->event_cb(arg, FOTA_EVENT_FAIL) != 0) {
                    LOGD(TAG, "FOTA_STOP 1!");
                    // fota->status = FOTA_STOP;
                } else {
                    LOGD(TAG, "FOTA_STOP 2!");
                    fota->status = FOTA_STOP;
                    // fota_fail(fota);
                }
                continue;
            } else if (size == 0) { // finish
                if (fota->event_cb && fota->event_cb(arg, FOTA_EVENT_FINISH) != 0) {
                    LOGD(TAG, "FOTA_FINISH 1!");
                } else {
                    LOGD(TAG, "FOTA_FINISH 2!");
                    fota->status = FOTA_FINISH;
                    fota_finish(fota);
                    continue;
                }
            }

            size = netio_write(fota->to, fota->buffer, size, -1);
            // TODO: verify data.
            // ...
            // LOGI(TAG, "write: %d", size);
            if (size > 0) {
                fota->offset += size;
                aos_kv_setint("fota_offset", fota->offset);
            } else { // flash write error
                if (fota->event_cb && fota->event_cb(arg, FOTA_EVENT_FAIL) != 0) {
                    LOGD(TAG, "FOTA_STOP 3!");
                } else {
                    LOGD(TAG, "write size < 0 %d", size);
                    LOGD(TAG, "FOTA_STOP 4!");
                    fota->status = FOTA_STOP;
                    // fota_fail(fota);
                }
            }
        } else if (fota->status == FOTA_STOP) {
            LOGD(TAG, "fota_task FOTA_STOP!");
            aos_msleep(fota->sleep_time);

            if (retry != 0) {
                LOGW(TAG, "fota retry: %x!", retry);
                retry--;
                fota->status = FOTA_DOWNLOAD;
            } else {
                retry = fota->retry_count;
                // aos_kv_setint("fota_offset", 0);
                fota->status = FOTA_INIT;
                fota_fail(fota);
            }
        }
    }

    fota->running = 0;

    aos_sem_signal(&fota->sem);

    if (fota->event_cb)
        fota->event_cb(arg, FOTA_EVENT_FINISH); //?
}

static void fota_check_task(void *arg)
{
    fota_t *fota = (fota_t *)arg;
    while(1) {
        if (fota->status == FOTA_INIT && fota->auto_check_en) {
            // LOGD(TAG, "fota check signal......"); 
            aos_event_set(&fota->do_check_event, 0x01, AOS_EVENT_OR);
        }
        aos_msleep(fota->sleep_time);
    }
}

void fota_do_check(fota_t *fota)
{
    // LOGD(TAG, "fota do check signal........");
    aos_event_set(&fota->do_check_event, 0x01, AOS_EVENT_OR);
}

int fota_start(fota_t *fota)
{
    aos_task_t task;
    if (fota->status == 0) {
        fota->status = FOTA_INIT;
        aos_task_new_ext(&task, "fota-check", fota_check_task, fota, 1024, AOS_DEFAULT_APP_PRI + 13);
        return aos_task_new_ext(&fota->task, "fota", fota_task, fota, 2048, AOS_DEFAULT_APP_PRI + 13);
    }

    return 0;
}

int fota_stop(fota_t *fota)
{
    fota->quit = 1;
    aos_event_set(&fota->do_check_event, 0x01, AOS_EVENT_OR);

    return 0;
}

void fota_finish(fota_t *fota)
{
    if (fota->cls->finish)
        fota->cls->finish();
}

void fota_fail(fota_t *fota)
{
    if (fota->cls->fail)
        fota->cls->fail();

    if (fota->from_path) {
        aos_free(fota->from_path);
        fota->from_path = NULL;
    }

    if (fota->buffer) {
        aos_free(fota->buffer);
        fota->buffer = NULL;
    }

    if (fota->private) {
        aos_free(fota->private);
        fota->private = NULL;
    }

    if (fota->from) {
        netio_close(fota->from);
        fota->from = NULL;
    }

    if (fota->to) {
        netio_close(fota->to);
        fota->to = NULL;
    }
}

int fota_close(fota_t *fota) {
    aos_sem_wait(&fota->sem, -1);
    aos_sem_free(&fota->sem);
    aos_event_free(&fota->do_check_event);

    if (fota->from_path) aos_free(fota->from_path);
    if (fota->to_path) aos_free(fota->to_path);
    if (fota->buffer) aos_free(fota->buffer);
    if (fota->private) aos_free(fota->private);
    if (fota->from) netio_close(fota->from);
    if (fota->to) netio_close(fota->to);

    aos_free(fota);

    return 0;
}

void fota_config(fota_t *fota, fota_config_t *config)
{
    aos_assert(fota);
    aos_assert(config);

    fota->auto_check_en = config->auto_check_en;
    fota->retry_count   = config->retry_count;
    fota->timeoutms     = config->timeoutms;
    fota->sleep_time    = config->sleep_time;
}

void fota_set_auto_check(fota_t *fota, int enable)
{
    aos_assert(fota);

    fota->auto_check_en = enable;
}

int fota_get_auto_check(fota_t *fota)
{
    aos_assert(fota);

    return fota->auto_check_en;
}

fota_status_e fota_get_status(fota_t *fota)
{
    aos_assert(fota);

    return fota->status;
}

// demo

#if 0
static int fota_event_cb(void *arg, fota_event_e event) //return 0: still do the default handle      not zero: only do the user handle
{
    fota_t *fota = (fota_t *)arg;
    switch (event) {
        case FOTA_EVENT_START:
            LOGD(TAG, "FOTA START :%x", fota->status);
            break;

        case FOTA_EVENT_FAIL:
            LOGD(TAG, "FOTA FAIL :%x", fota->status);
            break;

        case FOTA_EVENT_FINISH:
            LOGD(TAG, "FOTA FINISH :%x", fota->status);
            break;

        default:
            break;
    }
    return 0;
}

int event_cb(void *arg, fota_event_e event){
    fota_t *fota = (fota_t *)arg;
    switch (event) {
        case FOTA_EVENT_START:
            fota_upgrade(fota);
            fota_stop(fota);

            return 0;

        case FOTA_EVENT_FAIL:
            aos_msleep(3000);
            fota_start(fota);
            break;

        case FOTA_EVENT_FINISH:
            aos_reboot();

        default:
            return 0;
    }
}

#endif