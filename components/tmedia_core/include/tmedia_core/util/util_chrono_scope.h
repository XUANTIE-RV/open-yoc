/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_UTIL_CHRONO_SCOPE_H
#define TM_UTIL_CHRONO_SCOPE_H

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <chrono>

using namespace std;

class TMUtilChronoScope
{
public:
    TMUtilChronoScope()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }

    void reset()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }
    double delta_ms() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count() / 1000.0;
    }
    double delta_sec() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count() / 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
};

#endif  /* TM_UTIL_CHRONO_SCOPE_H */
