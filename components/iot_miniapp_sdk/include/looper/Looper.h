/*
 * Copyright (C) 2006 The Android Open Source Project
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
#ifndef ___JQUICK_BASE_LOOPER_H___
#define ___JQUICK_BASE_LOOPER_H___

#include "looper/MessageQueue.h"
#include "port/jquick_thread.h"

namespace JQuick
{
class Looper;
typedef void (*MsgCallback)(JQuick::sp<JQuick::Looper> l, JQuick::Message* msg);
class Looper : public REF_BASE
{
public:
    ~Looper();

    static void prepare();
    static void prepare(JQuick_Thread tid, JQuick::sp< Looper >);
    static void prepareMainLooper();
    // 由平台初始化main线程(需在平台主线程调用)
    static void prepareMainLooper(JQuick::sp< Looper >);
    static JQuick::sp< Looper > getMainLooper();
    static JQuick::sp< Looper > myLooper();

    void loop();
    void loopOnce();

    void setCallback(MsgCallback callback);
    void setContext(void* context);
    void* getContext();

    void quit(bool safe);
    void setTimeoutThreshold(int32_t threshold);

    JQuick_Thread getThreadId() const;

    MessageQueue mQueue;

    Looper(bool quitAllowed);
private:
    friend class Handler;
    static void prepare(bool quitAllowed);
    JQuick_Thread _tid;
    MsgCallback _callback;
    void* _context;
    int32_t _timeoutThreshold;
};
}  // namespace JQuick

#endif /* ___JQUICK_BASE_LOOPER_H___ */
