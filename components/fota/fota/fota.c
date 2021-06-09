/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/list.h>
#include <aos/kernel.h>
#include <aos/kv.h>
#include <yoc/netio.h>
#include <yoc/fota.h>
#include <yoc/partition.h>
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
    fota_cls_node_t *node = aos_malloc(sizeof(fota_cls_node_t));

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

    aos_assert(fota_name);
    aos_assert(dst);

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
    fota->timeoutms = 3000;
    return fota;
}

static int fota_version_check(fota_t *fota, fota_info_t *info) {
    if (fota->cls->version_check)
        return fota->cls->version_check(info);

    return -1;
}

static void fota_finish(fota_t *fota)
{
    if (fota->cls->finish)
        fota->cls->finish();
}

static void fota_fail(fota_t *fota)
{
    if (fota->cls->fail)
        fota->cls->fail();
}

static int fota_prepare(fota_t *fota)
{
    fota->buffer = aos_malloc(CONFIG_FOTA_BUFFER_SIZE);
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
    fota->total_size = fota->from->size;
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

static void fota_release(fota_t *fota)
{
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

static void fota_task(void *arg)
{
    fota_t *fota = (fota_t *)arg;
    int retry = fota->retry_count;
    unsigned int flag;

    // LOGD(TAG, "fota_task start: %s", fota->to_path);
    while (!fota->quit) {
        if (fota->status == FOTA_INIT) {
            //LOGD(TAG, "fota_task FOTA_INIT! wait");
            aos_event_get(&fota->do_check_event, 0x01, AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
            if (fota->quit) {
                break;
            }
            if (fota->event_cb)
                fota->event_cb(arg, FOTA_EVENT_START);

            if (fota_version_check(fota, &fota_info) == 0) {
                if (fota->event_cb) {
                    fota->event_cb(arg, FOTA_EVENT_VERSION);
                }
                if (fota_prepare(fota) < 0) {
                    LOGE(TAG, "fota_prepare failed");
                }
                continue;
            } else {
                if (fota->event_cb) {
                    fota->error_code = FOTA_ERROR_VERSION_CHECK;
                    fota->event_cb(arg, FOTA_EVENT_FAIL);
                }
            }
        } else if (fota->status == FOTA_DOWNLOAD) {
            //LOGD(TAG, "fota_task FOTA_DOWNLOAD! f:%d t:%d", fota->from->size, fota->to->offset);
            if (fota->from->size > 0) {
                static int fota_download_per = 101;
                int percent =  fota->to->offset * 100 / fota->from->size;
                if (fota_download_per != percent) {
                    LOGD(TAG, "FOTA:%d%%", percent);
                    fota_download_per = percent;
                }
            }

            int size = netio_read(fota->from, fota->buffer, CONFIG_FOTA_BUFFER_SIZE, fota->timeoutms);
            LOGD(TAG, "##read: %d", size);
            if (size < 0) {
                // LOGD(TAG, "read size < 0 %d", size);
                if (size == -2) {
                    LOGW(TAG, "reconnect again");
                    continue;
                }
                if (fota->event_cb) {
                    fota->error_code = FOTA_ERROR_NET_READ;
                    fota->event_cb(arg, FOTA_EVENT_FAIL);
                }
                fota->status = FOTA_ABORT;
                LOGD(TAG, "fota abort");
                continue;
            } else if (size == 0) {
                // finish
                fota->status = FOTA_FINISH;
                if (fota->event_cb)
                    fota->event_cb(arg, FOTA_EVENT_VERIFY);
                int verify = fota_data_verify();
                fota_finish(fota);
                fota_release(fota);
                aos_kv_del("fota_offset");
                if (fota->event_cb) {
                    if (verify != 0) {
                        LOGE(TAG, "fota data verify failed.");
                        fota->error_code = FOTA_ERROR_VERIFY;
                        fota->event_cb(arg, FOTA_EVENT_FAIL);
                    } else {
                        fota->event_cb(arg, FOTA_EVENT_FINISH);
                    }
                }
                fota->status = FOTA_INIT;
                continue;
            }

            size = netio_write(fota->to, fota->buffer, size, -1);
            // LOGI(TAG, "write: %d", size);
            if (size > 0) {
                fota->offset += size;
                aos_kv_setint("fota_offset", fota->offset);
                if (fota->event_cb) {
                    fota->event_cb(arg, FOTA_EVENT_PROGRESS);
                }
            } else {
                // flash write error
                if (fota->event_cb) {
                    fota->error_code = FOTA_ERROR_WRITE;
                    fota->event_cb(arg, FOTA_EVENT_FAIL);
                }
                fota->status = FOTA_ABORT;
            }
        } else if (fota->status == FOTA_ABORT) {
            LOGD(TAG, "fota_task FOTA_ABORT!");
            if (retry != 0) {
                LOGW(TAG, "fota retry: %d!", retry);
                retry--;
                fota->status = FOTA_DOWNLOAD;
                aos_msleep(fota->sleep_time);
            } else {
                retry = fota->retry_count;
                fota->status = FOTA_INIT;
                fota_fail(fota);
                fota_release(fota);
            }
        } else {
            // do not excute
            LOGE(TAG, "fota status:%d", fota->status);
            aos_msleep(fota->sleep_time);
        }
    }

    LOGD(TAG, "force quit need release source");
    fota->status = 0; // need reset for next restart
    fota_release(fota);
    aos_sem_signal(&fota->sem);

    if (fota->event_cb)
        fota->event_cb(arg, FOTA_EVENT_QUIT);
}

static void fota_check_task(void *arg)
{
    fota_t *fota = (fota_t *)arg;
    while(!fota->quit) {
        if (fota->status == FOTA_INIT && fota->auto_check_en) {
            LOGD(TAG, "fota check signal......");
            aos_event_set(&fota->do_check_event, 0x01, AOS_EVENT_OR);
        }
        aos_msleep(fota->sleep_time);
    }
    aos_task_exit(0);
}

void fota_do_check(fota_t *fota)
{
    aos_assert(fota);

    LOGD(TAG, "fota do check signal........");
    if (fota->status == FOTA_INIT)
        aos_event_set(&fota->do_check_event, 0x01, AOS_EVENT_OR);
}

int fota_start(fota_t *fota)
{
    aos_task_t task;

    aos_assert(fota);

    if (fota->status == 0) {
        fota->status = FOTA_INIT;
        fota->quit = 0;
        if (aos_task_new_ext(&task, "fota-check", fota_check_task, fota, 1024, AOS_DEFAULT_APP_PRI + 13) != 0) {
            return -1;
        }
        if (aos_task_new_ext(&fota->task, "fota", fota_task, fota, CONFIG_FOTA_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI + 13) != 0) {
            fota->quit = 1;
            fota->status = 0;
            return -1;
        }
    }

    return 0;
}

int fota_stop(fota_t *fota)
{
    aos_assert(fota);

    fota->quit = 1;
    aos_event_set(&fota->do_check_event, 0x01, AOS_EVENT_OR);

    return 0;
}

int fota_close(fota_t *fota)
{
    aos_assert(fota);

    fota->quit = 1;
    aos_event_set(&fota->do_check_event, 0x01, AOS_EVENT_OR);
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
static int fota_event_cb(void *arg, fota_event_e event)
{
    fota_t *fota = (fota_t *)arg;
    switch (event) {
        case FOTA_EVENT_START:
            LOGD(TAG, "FOTA START :%x", fota->status);
            break;
        case FOTA_EVENT_VERSION:
            LOGD(TAG, "FOTA VERSION CHECK OK :%x", fota->status);
            break;
        case FOTA_EVENT_PROGRESS:
            LOGD(TAG, "FOTA PROGRESS :%x, %d, %d", fota->status, fota->offset, fota->total_size);
            break;
        case FOTA_EVENT_FAIL:
            LOGD(TAG, "FOTA FAIL :%x, %d", fota->status, fota->error_code);
            break;
        case FOTA_EVENT_VERIFY:
            LOGD(TAG, "FOTA VERIFY :%x", fota->status);
            break;
        case FOTA_EVENT_FINISH:
            LOGD(TAG, "FOTA FINISH :%x", fota->status);
            break;
        case FOTA_EVENT_QUIT:
            LOGD(TAG, "FOTA QUIT :%x", fota->status);
            break;
        default:
            break;
    }
    return 0;
}
#endif