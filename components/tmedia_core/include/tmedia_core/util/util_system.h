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
    static const string GetCurExePath();
    static const string GetCurExeName();
    static int CreateDirectory(const char *path);

    static void PrintBackTrace();

private:
    static const string Demangle(const char* func_name);
    static void PrintBackTraceByPC(const void *pc);
};

#endif  /* TM_UTIL_SYSTEM_H */
