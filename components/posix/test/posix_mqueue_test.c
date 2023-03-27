/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "k_api.h"
#include "pthread.h"
#include "mqueue.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"

#define TAG "mqueue_test"

#define TOTAL_MSG_NUM 5

static mqd_t mqueue;

static char recv_buf[1024];
static int  succeed_flag = 1;

static char *test_msg[TOTAL_MSG_NUM] = {"os=AliOS Things", "version=v2.1", "component=osal.posix",
                                        "author=Jason","email=jason_sophia@aliyun.com"
                                       };

static void posix_mqueue_case1(void);
static void posix_mqueue_case2(void);
static void posix_mqueue_case3(void);

void posix_mqueue_test_case(void)
{
    posix_mqueue_case1();
    posix_mqueue_case2();
    posix_mqueue_case3();
}

static void *demo_task1(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = 0;
    int   len    = 0;

    while (1) {
        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_send(mqueue, test_msg[count], len, 0);
            if (ret != 0) {
                LOGE(TAG, "mq send failed %d\n", ret);
                succeed_flag = 0;
            }
        }

        aos_msleep(1000);
        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task2(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = 0;
    int   len    = 0;

    while (1) {
        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_receive(mqueue, recv_buf, sizeof(recv_buf), NULL);
            if ((ret != len) || (strncmp(recv_buf, test_msg[count], len) != 0)) {
                LOGE(TAG, "mq recv failed %d\n", ret);
                succeed_flag = 0;
            } else {
                LOGI(TAG, "mq recv msg: %s", recv_buf);
            }
        }

        aos_msleep(1000);
        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task3(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = 0;
    int   len    = 0;

    while (1) {
        if (count < 3) {
            len = strlen(test_msg[count]);
            ret = mq_send(mqueue, test_msg[count], len, 0);
            if (ret != 0) {
                succeed_flag = 0;
                LOGE(TAG, "mq send failed %d\n", ret);
            }
        }

        aos_msleep(1000);
        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task4(void *arg)
{
    int   count = 0;
    int   ret   = 0;
    int   len   = 0;
    struct timespec ts;

    while (1) {
        ts.tv_sec = 2;
        ts.tv_nsec = 0;
        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_timedreceive(mqueue, recv_buf, sizeof(recv_buf), NULL, &ts);
            if (count < 3) {
                if ((ret != len)||(strncmp(recv_buf, test_msg[count], len) != 0)) {
                    succeed_flag = 0;
                    LOGE(TAG, "mq recv failed %d\n", ret);
                } else {
                    LOGI(TAG, "mq recv msg: %s", recv_buf);
                }
            } else {
                if (ret != -1) {
                    succeed_flag = 0;
                    LOGE(TAG, "mq recv failed %d\n", ret);
                }
                break;
            }
        }

        aos_msleep(1000);
        count++;
    }

    return NULL;
}

static void *demo_task5(void *arg)
{
    int   count  = 0;
    int   ret    = 0;
    int   len    = 0;
    struct timespec ts;
    mqd_t mq;

    mq = mq_open("/mqueue", O_RDWR);
    if (mq == -1) {
        succeed_flag = 0;
    }

    while (1) {
        ts.tv_sec = 2;
        ts.tv_nsec = 0;
        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_timedreceive(mq, recv_buf, sizeof(recv_buf), NULL, &ts);
            if ((ret == len)||(strncmp(recv_buf, test_msg[count], len) == 0)) {
                LOGI(TAG, "mq recv msg: %s", recv_buf);
            } else {
                succeed_flag = 0;
                LOGI(TAG, "mq recv fail: %d", ret);
            }
        }

        aos_msleep(1000);
        if (count == TOTAL_MSG_NUM) {
            break;
        }
        count++;
    }
    mq_unlink("/mqueue");

    return NULL;
}

static void *demo_task6(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = 0;
    int   len    = 0;
    struct timespec ts;

    while (1) {
        ts.tv_sec = 2;
        ts.tv_nsec = 0;
        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_timedsend(mqueue, test_msg[count], len, 0, &ts);
            if (ret != 0) {
                succeed_flag = 0;
                LOGE(TAG, "mq send failed %d\n", ret);
            }
        }

        aos_msleep(1000);
        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }
        count++;
    }

    return NULL;
}

void posix_mqueue_case1(void)
{
    int   ret    = -1;
    void *status = NULL;
    pthread_t thread1;
    pthread_t thread2;
    struct mq_attr mqa = {0};

    LOGI(TAG, "posix_mqueue_case1 start !\n");
    mqa.mq_maxmsg  = 20;
    mqa.mq_msgsize = 96;
    succeed_flag = 1;
    memset(recv_buf, 0, sizeof(recv_buf));

    mqueue = mq_open("/mqueue", O_CREAT | O_RDWR, 0, &mqa);
    if (mqueue == -1) {
        succeed_flag = 0;
    }

    ret = pthread_create(&thread1, NULL, demo_task1, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task2, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    mq_close(mqueue);

    if (succeed_flag == 1) {
        printf("posix_mqueue_case1 test OK !\n");
    } else {
        printf("posix_mqueue_case1 test failed !\n");
    }

    LOGI(TAG, "posix_mqueue_case1 end !\n");
}

void posix_mqueue_case2(void)
{
    int   ret    = -1;
    void *status = NULL;

    pthread_t thread1;
    pthread_t thread2;
    struct mq_attr mqa = {0};

    LOGI(TAG, "posix_mqueue_case2 start !\n");
    mqa.mq_maxmsg  = 20;
    mqa.mq_msgsize = 96;
    succeed_flag = 1;
    memset(recv_buf, 0, sizeof(recv_buf));

    mqueue = mq_open("/mqueue", O_CREAT | O_RDWR, 0, &mqa);
    if (mqueue == -1) {
        succeed_flag = 0;
    }

    ret = pthread_create(&thread1, NULL, demo_task3, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task4, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    mq_close(mqueue);

    if (succeed_flag == 1) {
        printf("posix_mqueue_case2 test OK !\n");
    } else {
        printf("posix_mqueue_case2 test failed !\n");
    }

    LOGI(TAG, "posix_mqueue_case2 end !\n");
}

void posix_mqueue_case3(void)
{
    int   ret    = -1;
    void *status = NULL;

    pthread_t thread1;
    pthread_t thread2;
    struct mq_attr mqa = {0};

    LOGI(TAG, "posix_mqueue_case3 start !\n");
    mqa.mq_maxmsg  = 20;
    mqa.mq_msgsize = 96;
    succeed_flag = 1;
    memset(recv_buf, 0, sizeof(recv_buf));

    mqueue = mq_open("/mqueue", O_CREAT | O_RDWR, 0, &mqa);
    if (mqueue == -1) {
        succeed_flag = 0;
    }

    ret = pthread_create(&thread1, NULL, demo_task5, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task6, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    mq_close(mqueue);

    if (succeed_flag == 1) {
        printf("posix_mqueue_case3 test OK !\n");
    } else {
        printf("posix_mqueue_case3 test failed !\n");
    }

    LOGI(TAG, "posix_mqueue_case3 end !\n");
}
