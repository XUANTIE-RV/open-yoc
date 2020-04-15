/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __INCLUDE_SYS_TIME_H
#define __INCLUDE_SYS_TIME_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/
/* struct timeval represents time as seconds plus microseconds */

struct timeval
{
  time_t tv_sec;         /* Seconds */
  long tv_usec;          /* Microseconds */
};

/* The use of the struct timezone  is obsolete; the tz argument should
 * normally be specified as NULL (and is ignored in any event).
 */

struct timezone
{
  int tz_minuteswest;     /* Minutes west of Greenwich */
  int tz_dsttime;         /* Type of DST correction */
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: gettimeofday
 *
 * Description:
 *   Get the current time
 *
 *   Conforming to SVr4, 4.3BSD. POSIX.1-2001 describes gettimeofday().
 *   POSIX.1-2008 marks gettimeofday() as obsolete, recommending the use of
 *   clock_gettime(2) instead.
 *
 *   NuttX implements gettimeofday() as a thin layer around clock_gettime();
 *
 * Input Parameters:
 *   tv - The location to return the current time
 *   tz - Ignored
 *
 * Returned value:
 *   Zero (OK) on success;  -1 is returned on failure with the errno variable
 *   set appropriately.
 *
 ****************************************************************************/

int gettimeofday(  struct timeval *tv,   struct timezone *tz);

/****************************************************************************
 * Name: settimeofday
 *
 * Description:
 *   Set the current time
 *
 *   Conforming to SVr4, 4.3BSD. POSIX.1-2001 describes gettimeofday() but
 *   not settimeofday().
 *
 *   NuttX implements settimeofday() as a thin layer around clock_settime();
 *
 * Input Parameters:
 *   tv - The net to time to be set
 *   tz - Ignored
 *
 * Returned value:
 *   Zero (OK) on success;  -1 is returned on failure with the errno variable
 *   set appropriately.
 *
 ****************************************************************************/

int settimeofday(  const struct timeval *tv,   const struct timezone *tz);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_SYS_TIME_H */
