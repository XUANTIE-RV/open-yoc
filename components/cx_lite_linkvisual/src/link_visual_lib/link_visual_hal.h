#ifndef LINK_VISUAL_HAL_H
#define LINK_VISUAL_HAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//<fcntl.h>所需要的定义
static const int LV_HAL_ENUM_F_GETFL = 3;               /* get file status flags */
static const int LV_HAL_ENUM_F_SETFL = 4;               /* set file status flags */
static const int LV_HAL_ENUM_O_NONBLOCK = 2048;         /* no delay */ /* 原定义是八进制数04000 */

//<socket.h>所需要的定义
static const int LV_HAL_ENUM_SOL_SOCKET = 1;       /* options for socket level */
static const int LV_HAL_ENUM_SO_ERROR = 4;         /* get error status and clear */

static const int LV_HAL_ENUM_AF_UNIX = 1;               /* local to host (pipes) */
static const int LV_HAL_ENUM_AF_INET = 2;               /* internetwork: UDP, TCP, etc. */
static const int LV_HAL_ENUM_SOCK_STREAM = 1;           /* stream socket */
static const int LV_HAL_ENUM_SOCK_DGRAM = 2;            /* datagram socket */
static const int LV_HAL_ENUM_SOCK_RAW = 3;              /* raw-protocol interface */

//<socket.h>所需要的定义
static const int LV_HAL_ENUM_IPPROTO_IP = 0;            /* dummy for IP */
static const int LV_HAL_ENUM_IPPROTO_ICMP = 1;          /* control message protocol */
static const int LV_HAL_ENUM_IPPROTO_TCP = 6;           /* tcp */
static const int LV_HAL_ENUM_IPPROTO_UDP = 17;          /* user datagram protocol */


//线程操作函数，一般在头文件<pthread.h>
extern int lv_hal_thread_create(void **thread, void *(*function)(void *arg), void *data);
extern int lv_hal_thread_destroy(void *thread);


//锁操作函数，一般在头文件<pthread.h>
extern void lv_hal_mutex_init(void **mutex);
extern void lv_hal_mutex_lock(void *mutex);
extern void lv_hal_mutex_unlock(void *mutex);
extern void lv_hal_mutex_destroy(void *mutex);


//socket操作函数，一般在头文件<sys/socket.h>
/*
 *
 * @NOTICE:下述会说明每个参数的具体定义，这些定义参考了遵守POSIX的系统(UBUNTU/CENTOS)，
 *          如果你的系统头文件与以下定义不一致，请自行做一层转换。
 *          如果你的系统头文件不支持以下定义功能，请联系系统厂商来给予支持。
 *
 * address_families的值：
 * static const int LV_HAL_ENUM_AF_INET = 2;
 *
 * types的值：
 * static const int LV_HAL_ENUM_SOCK_STREAM = 1;
 * static const int LV_HAL_ENUM_SOCK_DGRAM = 2;
 * static const int LV_HAL_ENUM_SOCK_RAW = 3;
 *
 * protocols的值：
 * static const int LV_HAL_ENUM_IPPROTO_ICMP = 1;
 * static const int LV_HAL_ENUM_IPPROTO_TCP = 6;
 * static const int LV_HAL_ENUM_IPPROTO_UDP = 17;
 * */
extern int lv_hal_socket(int address_families, int types, int protocols);

/*
 * @NOTICE:下述会说明每个参数的具体定义，这些定义参考了遵守POSIX的系统(UBUNTU/CENTOS)，
 *          如果你的系统头文件与以下定义不一致，请自行做一层转换。
 *          如果你的系统头文件不支持以下定义功能，请联系系统厂商来给予支持。
 *
 * family的值：
 * static const int LV_HAL_ENUM_AF_UNIX = 1;
 *
 * types的值：
 * static const int LV_HAL_ENUM_SOCK_DGRAM = 2;
 *
 * protocols的值：
 * static const int LV_HAL_ENUM_IPPROTO_IP = 0;
 * */
extern int lv_hal_socketpair(int address_families, int type, int protocol, int fd[2]);

/*
 * @NOTICE:下述会说明每个参数的具体定义，这些定义参考了遵守POSIX的系统(UBUNTU/CENTOS)，
 *          如果你的系统头文件与以下定义不一致，请自行做一层转换。
 *          如果你的系统头文件不支持以下定义功能，请联系系统厂商来给予支持。
 *
 * address_families的值：
 * static const int LV_HAL_ENUM_AF_INET = 2;
 * */
extern int lv_hal_connect(int fd, int address_families, const char *addr, uint16_t port);

/*
 * @NOTICE:下述会说明每个参数的具体定义，这些定义参考了遵守POSIX的系统(UBUNTU/CENTOS)，
 *          如果你的系统头文件与以下定义不一致，请自行做一层转换。
 *          如果你的系统头文件不支持以下定义功能，请联系系统厂商来给予支持。
 *
 * level_number的值：
 * static const int LV_HAL_ENUM_SOL_SOCKET = 1;
 *
 * addition_optional的值：
 * static const int LV_HAL_ENUM_SO_ERROR = 4;
 * */
extern int lv_hal_getsockopt(int fd, int level_number, int addition_optional, int *error);


//socket操作函数，一般在头文件<unistd.h>
extern int64_t lv_hal_read(int fd, void *buf, uint64_t byte);
extern int64_t	lv_hal_write(int fd, const void *buf, uint64_t byte);
extern int	lv_hal_close(int fd);


//socket操作函数，一般在头文件<fcntl.h>
/*
 * @NOTICE:下述会说明每个参数的具体定义，这些定义参考了遵守POSIX的系统(UBUNTU/CENTOS)，
 *          如果你的系统头文件与以下定义不一致，请自行做一层转换。
 *          如果你的系统头文件不支持以下定义功能，请联系系统厂商来给予支持。
 *
 * command_values的值：
 * static const int LV_HAL_ENUM_F_GETFL = 3;
 * static const int LV_HAL_ENUM_F_SETFL = 4;
 *
 * types的值：
 * static const int LV_HAL_ENUM_O_NONBLOCK = 2048;
 * */
extern int lv_hal_fcntl(int fd, int command_values, int flags);


//socket操作函数，一般在头文件<sys/select.h>
extern void lv_hal_fd_set_create(void **fd_set);
extern void lv_hal_fd_set_destroy(void *fd_set);
extern int lv_hal_fd_isset(int, void *fd_set);
extern void lv_hal_fd_set(int, void *fd_set);
extern void lv_hal_fd_zero(void *fd_set);
extern int lv_hal_select(int, void *fd_set_read, void *fd_set_write, void *fd_set_event, uint64_t time_out);

typedef struct 
{
    /* data */
    long tv_sec;
    long tv_usec;
}lv_hal_timeval_t;

typedef struct 
{
    /* data */
    uint64_t tv_sec;
    uint64_t tv_nsec;
}lv_hal_timespec_t;

typedef struct 
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/
}lv_hal_tm_t;


#define LV_HAL_CLOCK_MONOTONIC      (1)

extern int lv_hal_gettimeofday(lv_hal_timeval_t *tv ,void *zone);
extern int lv_hal_gmtime(uint64_t utc,lv_hal_tm_t *tm);
extern int lv_hal_get_date_str(char *date,int date_len,const char *date_fmt);
extern int lv_hal_clock_gettime(int clock_id,lv_hal_timespec_t *ts);
extern uint64_t lv_hal_time(void *tloc);

#define LV_HAL_W_OK         (0)
#define LV_HAL_F_OK         (2)
extern int lv_hal_access(const char *pathname,int mode);

extern char *lv_hal_strtok_r(char *str, const char *delim, char **saveptr);
extern int lv_hal_strcasecmp(const char *s1, const char *s2);
extern double lv_hal_strtod(const char *nptr, char **endptr);

#ifdef __cplusplus
}
#endif

#endif //LINK_VISUAL_HAL_H
