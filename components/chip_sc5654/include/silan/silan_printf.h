/*
 * silan_printf.h
 */
#include <stdio.h>

#ifndef __VSPRINTF_H__
#define __VSPRINTF_H__

void assert_report(const char *file_name, const char *function_name, unsigned int line_no);
void sl_printf(const char *fmt, ...);
extern int sprintf(char *buf, const char *fmt, ...);

#define ASSERT_REPORT(condition)    \
    do{ \
        if(condition)   \
            NULL;    \
        else    \
            assert_report(__FILE__, __func__, __LINE__);    \
    }while(0)

#define _SL_LOG(fmt, ...)   do { \
        /*while (silan_cxc_mutex_lock(SILAN_CXC_MUTEX_LOG));*/ \
        printf(fmt "\n", ##__VA_ARGS__); \
        /*silan_cxc_mutex_unlock(SILAN_CXC_MUTEX_LOG);*/ \
    } while(0)

#if defined(__CC_ARM)
#define LOG_CORE_HEAD          "[M0]"
#elif defined(__CSKY__)
#define LOG_CORE_HEAD          "[CK]"
#elif defined(__XCC__)
#define LOG_CORE_HEAD          "[DSP]"
#else
#error error.
#endif

#ifndef MODULE_NAME
#define LOG_HEAD               LOG_CORE_HEAD
#define DBGLOG_HEAD            LOG_CORE_HEAD
#else
#define LOG_HEAD               LOG_CORE_HEAD"["MODULE_NAME"]"
#define DBGLOG_HEAD            LOG_CORE_HEAD"["MODULE_NAME"]"
#endif

#define SL_LOG(fmt, ...)       _SL_LOG(LOG_HEAD fmt, ##__VA_ARGS__)

#ifndef __DEBUG__
#define SL_DBGLOG(fmt, ...)
#define SL_DBGLOG_IRQ(fmt, ...)
#define SL_TRACE()
#define SL_TRACE_IRQ()
#define SL_ASSERT(x)
#else
#define SL_DBGLOG(fmt, ...)      _SL_LOG(DBGLOG_HEAD fmt, ##__VA_ARGS__)
#define SL_DBGLOG_IRQ(fmt, ...)  do { \
        printf(fmt "\n", ##__VA_ARGS__); \
    } while(0)
#define SL_TRACE()               SL_DBGLOG("<%s> %s,%d", __func__, __FILE__, __LINE__)
#define SL_TRACE_IRQ()           SL_DBGLOG_IRQ("<%s> %s,%d", __func__, __FILE__, __LINE__)
#define SL_ASSERT(x)             do { \
        if (!(x)) { \
            printf("ASSERT at <%s> %s,%d\n\n\n\n", \
                      __func__, __FILE__, __LINE__); \
            while (1); \
        } \
    } while(0)
#endif

#endif  //__VSPRINTF_H__
