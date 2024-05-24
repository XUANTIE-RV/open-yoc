/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef TM_UTIL_SYSTEM_H
#define TM_UTIL_SYSTEM_H

#include <string>

using namespace std;

class TMUtilSystem
{
public:
    typedef struct {
        int major;
        int minor;
        int revision;
        bool latest;
        bool valid;
    } SDKVersion_t;

    static const string GetCurExePath();
    static const string GetCurExeName();
    static int CreateDirectory(const char *path);

    static void PrintBackTrace();
/*
@param in,
  SDKVersion_t &version
@param out,
 -1, Fail
  0, Success
*/
    static int GetSDKVersion(SDKVersion_t &version);

/*
@param in,
  SDKVersion_t version1, version2
@param out,
 -1, Fail
  0, version1 < version2
  1, version1 = version2
  2, version1 > version2
*/
    static int CompareGetSDKVersion(SDKVersion_t version1, SDKVersion_t version2);
    static SDKVersion_t CurrentSDKVersion;

private:
    static const string Demangle(const char* func_name);
    static void PrintBackTraceByPC(const void *pc);
};

#endif  /* TM_UTIL_SYSTEM_H */
