/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_SYSLOG_H
#define TM_SYSLOG_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOG_ENABLE

#ifdef LOG_LEVEL
#if (LOG_LEVEL >= 3)
#define LOG_ENABLE_D
#endif

#if (LOG_LEVEL >= 2)
#define LOG_ENABLE_I
#endif

#if (LOG_LEVEL >= 1)
#define LOG_ENABLE_W
#endif

#if (LOG_LEVEL >= 0)
#define LOG_ENABLE_E
#endif
#else   /* #ifdef LOG_LEVEL */
#define LOG_ENABLE_E    /* Default level if LOG_LEVEL not defiend */
#endif  /* #ifdef LOG_LEVEL */

/* [LogLevel:FileName:Function:Line] */
extern const char *TM_PFORMAT_D;
extern const char *TM_PFORMAT_I;
extern const char *TM_PFORMAT_W;
extern const char *TM_PFORMAT_E;
extern const char *TM_PFORMAT_O;

#ifdef LOG_PREFIX
#define LOG_PREF LOG_PREFIX"-"
#else
#define LOG_PREF ""
#endif

#define __BARE_FILENAME__ strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__

#define LOG_E_BASE_ARGS LOG_PREF, __BARE_FILENAME__, __FUNCTION__, __LINE__
#define LOG_W_BASE_ARGS LOG_PREF, __BARE_FILENAME__, __FUNCTION__, __LINE__
#define LOG_I_BASE_ARGS LOG_PREF, __BARE_FILENAME__, __FUNCTION__, __LINE__
#define LOG_D_BASE_ARGS LOG_PREF, __BARE_FILENAME__, __FUNCTION__, __LINE__
#define LOG_O_BASE_ARGS LOG_PREF, __BARE_FILENAME__, __FUNCTION__, __LINE__

/* Log in freely format without prefix */
#ifdef LOG_ENABLE
#define LOG_F(fmt, args...) printf(fmt,##args)
#else
#define LOG_F(fmt, args...)
#endif

// OK, Green color
#define LOG_O(fmt, args...) \
    do {printf(TM_PFORMAT_O,LOG_O_BASE_ARGS); printf(fmt,##args);} while(0)

#define LOGPRINT_E printf
#define LOGPRINT_W printf
#define LOGPRINT_I printf
#define LOGPRINT_D printf

/* Log debug */
#if defined(LOG_ENABLE_D) && defined(LOG_ENABLE)
#define LOG_D(fmt, args...) \
    do {LOGPRINT_D(TM_PFORMAT_D,LOG_D_BASE_ARGS); LOGPRINT_D(fmt,##args);} while(0)
#else
#define LOG_D(fmt, args...)
#endif

/* Log information */
#if defined(LOG_ENABLE_I) && defined(LOG_ENABLE)
#define LOG_I(fmt, args...) \
    do {LOGPRINT_I(TM_PFORMAT_I,LOG_I_BASE_ARGS); LOGPRINT_I(fmt,##args);} while(0)
#else
#define LOG_I(fmt, args...)
#endif

/* Log warning */
#if defined(LOG_ENABLE_W) && defined(LOG_ENABLE)
#define LOG_W(fmt, args...) \
    do {LOGPRINT_W(TM_PFORMAT_W,LOG_W_BASE_ARGS); LOGPRINT_W(fmt,##args);} while(0)
#else
#define LOG_W(fmt, args...)
#endif

/* Log error */
#if defined(LOG_ENABLE_E) && defined(LOG_ENABLE)
#define LOG_E(fmt, args...) \
    do {LOGPRINT_E(TM_PFORMAT_E,LOG_E_BASE_ARGS); LOGPRINT_E(fmt,##args);} while(0)
#else
#define LOG_E(fmt, args...)
#endif

#define ENTER_VOID()    LOG_D("Enter\n")
#define EXIT_VOID()     do { LOG_D("Exit\n"); return;} while(0)
#define EXIT_INT(val)   do { LOG_D("Exit, return val=%d\n", (int)val); return val;} while(0)
#define EXIT_PTR(ptr)   do { LOG_D("Exit, return ptr=%p\n", (void*)ptr); return ptr;} while(0)

#endif  /* TM_SYSLOG_H */
