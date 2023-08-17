/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "link_visual_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>



int lv_hal_thread_create(void **thread, void *(*function)(void *arg), void *data) {
    static int count = 0;
    pthread_t tid;
    pthread_attr_t  attr;
    char task_name[128];
    int ret = pthread_attr_init(&attr);
    if (ret < 0) {
        printf("Create thread attr failed, ret = %d\n", ret);
        return -1;
    }
    attr.stacksize = 1024*40;
    ret = pthread_create(&tid, &attr, function, data);
    if (ret != 0) {
        printf("Create thread failed, ret = %d\n", ret);
        return -1;
    }
    ret = snprintf(task_name, sizeof(task_name), "%s%d", "lv_lib_task", count++);
    if(ret < 0) {
        printf("%s %d snprintf error\n", __func__, __LINE__);
    } 
    pthread_setname_np(tid, task_name);
    ret = pthread_detach(tid);
    if (ret != 0) {
        printf("Detach thread failed, ret = %d\n", ret);
        return -1;
    }

    return 0;
}

int lv_hal_thread_destroy(void *thread) {
    //demo默认使用的是分离式的线程，不需要去销毁
    //如果不使用分离式线程，可以使用pthread_join去销毁
    return 0;
}

void lv_hal_mutex_init(void **mutex) {
    *mutex =  (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (!*mutex) {
        printf("malloc mutex failed");
        return;
    }

    int ret = pthread_mutex_init((pthread_mutex_t *)*mutex, NULL);//存在失败的可能性
    if (ret != 0) {
        free((pthread_mutex_t *)*mutex);
        *mutex = NULL;
        printf("pthread_mutex_init failed, ret = %d \n", ret);
    }
}

void lv_hal_mutex_lock(void *mutex) {
    if (mutex) {
        pthread_mutex_lock((pthread_mutex_t *)mutex);
    }
}

void lv_hal_mutex_unlock(void *mutex) {
    if (mutex) {
        pthread_mutex_unlock((pthread_mutex_t *)mutex);
    }
}

void lv_hal_mutex_destroy(void *mutex) {
    if (mutex) {
        pthread_mutex_destroy((pthread_mutex_t *)mutex);
        free((pthread_mutex_t *)mutex);
    }
}

int lv_hal_socket(int address_families, int types, int protocols) {
    int ret;
    ret = socket(address_families, types, protocols);
    return ret;
}

int lv_hal_connect(int fd, int address_families, const char *addr, uint16_t port) {
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = address_families;
    sin.sin_addr.s_addr = inet_addr(addr);
    sin.sin_port = htons(port);

    return connect(fd, (struct sockaddr *)&sin, sizeof(sin));
}

int lv_hal_getsockopt(int fd, int level_number, int addition_optional, int *error) {
    socklen_t elen = sizeof(*error);
    if(addition_optional == LV_HAL_ENUM_SO_ERROR) {
        addition_optional = SO_ERROR;
    } 
    if(level_number == LV_HAL_ENUM_SOL_SOCKET) {
        level_number = SOL_SOCKET;
    }
    return getsockopt(fd, level_number, addition_optional, (void *)error, &elen);
}

int64_t lv_hal_read(int fd, void *buf, uint64_t byte) {
    return read(fd, buf, byte);
}

int64_t lv_hal_write(int fd, const void *buf, uint64_t byte) {
    return write(fd, buf, byte);
}

int lv_hal_close(int fd) {
    return close(fd);
}

int lv_hal_fcntl(int fd, int command_values, int flags) {
    if(command_values == LV_HAL_ENUM_F_SETFL) {
        if(flags & LV_HAL_ENUM_O_NONBLOCK) {
            flags &= (~LV_HAL_ENUM_O_NONBLOCK);
            flags |= O_NONBLOCK;
        }
    }
    return fcntl(fd, command_values, flags);
}

void lv_hal_fd_set_create(void **set) {
    *set =  (fd_set *)malloc(sizeof(fd_set));
}

void lv_hal_fd_set_destroy(void *set) {
    if (set) {
        free((fd_set*)set);
    }
}

int lv_hal_fd_isset(int fd, void *set) {
    return FD_ISSET(fd, (fd_set*)set);
}

void lv_hal_fd_set(int fd, void *set) {
    FD_SET(fd, (fd_set*)set);
}

void lv_hal_fd_zero(void *set) {
    FD_ZERO((fd_set*)set);
}

int lv_hal_select(int fd, void *set_read, void *set_write, void *set_event, uint64_t time_out) {
    struct timeval tv = {0};
    tv.tv_sec = time_out / 1000;
    tv.tv_usec = (time_out - tv.tv_sec *1000) * 1000;
    return select(fd, (fd_set *)set_read, (fd_set *)set_write, (fd_set *)set_event, &tv);
}

static int socketpair(int family, int type, int protocol, int fd[2])
{
    if (!fd) {
        fprintf(stderr, "EINVAL\n");
        return -1;
    }

    int mListener = -1;
    int mConnector = -1;
    int mAcceptor = -1;
    struct sockaddr_in mListenerAddr;
    struct sockaddr_in mConnectAddr;

    memset(&mListenerAddr, 0, sizeof(mListenerAddr));
    memset(&mConnectAddr, 0, sizeof(mConnectAddr));

    mListenerAddr.sin_family = AF_INET;
    mListenerAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    mListenerAddr.sin_port = 0;

    mListener = socket(AF_INET, type, 0);
    if (mListener < 0) {
        return -1;
    }

    mConnector = socket(AF_INET, type, 0);
    if (mConnector < 0) {
        goto err;
    }

    if (bind(mListener, (struct sockaddr *)&mListenerAddr, sizeof(mListenerAddr)) == -1) {
        goto err;
    }
    if (listen(mListener, 1) == -1) {
        goto err;
    }

    socklen_t size = sizeof(mConnectAddr);
    if (getsockname(mListener, (struct sockaddr *)&mConnectAddr, &size) == -1 || size != sizeof(mConnectAddr)) {
        goto err;
    }

    if (connect(mConnector, (struct sockaddr *)&mConnectAddr, sizeof(mConnectAddr)) == -1) {
        goto err;
    }

    size = sizeof(mListenerAddr);

    mAcceptor = accept(mListener, (struct sockaddr *)&mListenerAddr, &size);
    if (mAcceptor < 0 || size != sizeof(mListenerAddr)) {
        goto err;
    }
    close(mListener);

    if (getsockname(mConnector, (struct sockaddr *)&mConnectAddr, &size) == -1 || size != sizeof(mConnectAddr) \
        || mListenerAddr.sin_family != mConnectAddr.sin_family || mListenerAddr.sin_addr.s_addr != mConnectAddr.sin_addr.s_addr || mListenerAddr.sin_port != mConnectAddr.sin_port) {
        goto err;
    }

    fd[0] = mConnector;
    fd[1] = mAcceptor;

    return 0;

err:
    if (mListener != -1) {
        close(mListener);
    }
    if (mConnector != -1) {
        close(mConnector);
    }
    if (mAcceptor != -1) {
        close(mAcceptor);
    }

    return -1;
}
int lv_hal_socketpair(int address_families, int type, int protocol, int fd[2]) {
    return socketpair(AF_INET, SOCK_STREAM, 0, fd);
}

int lv_hal_gettimeofday(lv_hal_timeval_t *tv ,void *zone)
{
    int ret = 0;
    struct timeval tv_ = { 0 };
    
    ret = gettimeofday(&tv_,NULL);

    tv->tv_sec = tv_.tv_sec;
    tv->tv_usec = tv_.tv_usec;

    return ret;
}

int lv_hal_gmtime(uint64_t utc,lv_hal_tm_t *tm)
{
    struct tm *now_tm = NULL;

    now_tm =  gmtime((time_t*)&utc);

    tm->tm_year = now_tm->tm_year;
    tm->tm_mon = now_tm->tm_mon;
    tm->tm_yday = now_tm->tm_yday;

    tm->tm_hour = now_tm->tm_hour;
    tm->tm_min = now_tm->tm_min;
    tm->tm_sec = now_tm->tm_sec;

    tm->tm_isdst = now_tm->tm_isdst;
    tm->tm_wday = now_tm->tm_wday;
    tm->tm_mday = now_tm->tm_mday;

    return 0;
}

int lv_hal_get_date_str(char *date,int date_len,const char *date_fmt)
{
    time_t curr_time = time(NULL);
    
    strftime(date, date_len - 1, date_fmt, localtime(&curr_time));
    return 0;
}

int lv_hal_clock_gettime(int clock_id,lv_hal_timespec_t *ts)
{
    struct timespec ts_ = { 0 };

    clock_gettime(clock_id,&ts_);

    ts->tv_sec = ts_.tv_sec;
    ts->tv_nsec = ts_.tv_nsec;

    return 0;
}

uint64_t lv_hal_time(void *tloc)
{
    return time(tloc);
}

int lv_hal_access(const char *pathname,int mode)
{
    int tran_mode;
    if(mode == LV_HAL_W_OK) {
        tran_mode = W_OK;
    } else if(mode == LV_HAL_F_OK) {
        tran_mode = F_OK;
    } else {
        printf("unknown mode!\n");
        return -1;
    }
    return access(pathname, tran_mode);
}

char *lv_hal_strtok_r(char *str, const char *delim, char **saveptr)
{
    return strtok_r(str,delim,saveptr);
}

int lv_hal_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1,s2);
}

double lv_hal_strtod(const char *nptr, char **endptr)
{
    return strtod(nptr,endptr);
}


