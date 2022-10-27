/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <time.h>

extern int clock_gettime(clockid_t clockid, struct timespec *tp);

/*
SVr4, 4.3BSD, C89, C99, POSIX.1-2001.  POSIX does not specify any
error conditions.
*/
time_t time(time_t *tloc)
{
  struct timespec ts;
  int ret;

  /* Get the current time from the system */

  ret = clock_gettime(CLOCK_REALTIME, &ts);
  if (ret == 0)
    {
      /* Return the seconds since the epoch */

      if (tloc)
        {
          *tloc = ts.tv_sec;
        }

      return ts.tv_sec;
    }

  return (time_t)-1;
}
