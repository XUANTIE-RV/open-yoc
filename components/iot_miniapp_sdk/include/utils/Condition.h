/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_BASE_CONDITION_H___
#define ___JQUICK_BASE_CONDITION_H___

#include "port/jquick_condition.h"

#include "Mutex.h"

namespace JQuick
{
/*
 * Condition variable class.  The implementation is system-dependent.
 *
 * Condition variables are paired up with mutexes.  Lock the mutex,
 * call wait(), then either re-wait() if things aren't quite what you want,
 * or unlock the mutex and continue.  All threads calling wait() must
 * use the same mutex for a given Condition.
 */
class Condition
{
public:
    Condition();

    ~Condition();

    // Wait on the condition variable.  Lock the mutex before calling.
    //
    int32_t wait(Mutex& mutex);

    // same with relative timeout
    //
    int32_t waitRelative(Mutex& mutex, int millisecond);

    // Signal the condition variable, allowing one thread to continue.
    //
    void signal();

    // Signal the condition variable, allowing all threads to continue.
    //
    void broadcast();

private:
    JQuick_Condition mCond;
};

inline Condition::Condition()
{
    mCond = jquick_condition_create();
}

inline Condition::~Condition()
{
    jquick_condition_destroy(mCond);
}

inline int32_t Condition::wait(Mutex& mutex)
{
    return jquick_condition_wait(mCond, mutex.mMutex);
}

inline int32_t Condition::waitRelative(Mutex& mutex, int millisecond)
{
    if (millisecond <= 0) {
        return jquick_condition_wait(mCond, mutex.mMutex);
    } else {
        return jquick_condition_wait_with_timeout(mCond, mutex.mMutex, millisecond);
    }
}

inline void Condition::signal()
{
    jquick_condition_signal(mCond);
}
inline void Condition::broadcast()
{
    jquick_condition_broadcast(mCond);
}

}  // namespace JQuick

#endif  // ___GREENUI_CONDITION_H___
