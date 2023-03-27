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

#ifndef ___JQUICK_BASE_MUTEX_H___
#define ___JQUICK_BASE_MUTEX_H___

#include "port/jquick_mutex.h"
#include <sys/types.h>

#if defined(_WIN32)
#include <malloc.h>
#include <stdint.h>
#endif

namespace JQuick
{
class Condition;

/*
 * Simple mutex class.  The implementation is system-dependent.
 *
 * The mutex must be unlocked by the thread that locked it.  They are not
 * recursive, i.e. the same thread can't lock it multiple times.
 */
class Mutex
{
public:
    Mutex();

    ~Mutex();

    // lock or unlock the mutex
    int32_t lock();

    void unlock();

    // Manages the mutex automatically. It'll be locked when Autolock is
    // constructed and released when Autolock goes out of scope.
    class Autolock
    {
    public:
        inline Autolock(Mutex& mutex) :
                mLock(mutex)
        {
            mLock.lock();
        }
        inline Autolock(Mutex* mutex) :
                mLock(*mutex)
        {
            mLock.lock();
        }
        inline ~Autolock()
        {
            mLock.unlock();
        }

    private:
        Mutex& mLock;
    };

private:
    friend class Condition;

    // A mutex cannot be copied
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

    JQuick_Mutex mMutex;
};

// ---------------------------------------------------------------------------

inline Mutex::Mutex()
{
    mMutex = jquick_mutex_create();
}

inline Mutex::~Mutex()
{
    jquick_mutex_destroy(mMutex);
}

inline int32_t Mutex::lock()
{
    return jquick_mutex_lock(mMutex);
}

inline void Mutex::unlock()
{
    jquick_mutex_unlock(mMutex);
}

// ---------------------------------------------------------------------------

/*
 * Automatic mutex.  Declare one of these at the top of a function.
 * When the function returns, it will go out of scope, and release the
 * mutex.
 */
typedef Mutex::Autolock AutoMutex;

};  // namespace JQuick

#endif  // ___GREENUI_MUTEX_H___
