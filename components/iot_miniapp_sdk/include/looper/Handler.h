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
#ifndef ___JQUICK_BASE_HANDLER_H___
#define ___JQUICK_BASE_HANDLER_H___

#include "looper/Task.h"
#include "looper/Looper.h"

namespace JQuick
{
class Handler : public REF_BASE
{
public:
    Handler();
    Handler(JQuick::sp< Looper > looper);

    virtual ~Handler();

    JQuick::sp< Looper > getLooper() const;

    bool run(Task* task);
    bool post(Task* task, int priority = -1)
    {
        return postDelayed(task, 0, priority);
    }
    bool postDelayed(Task* task, long long delayMillis, int priority = -1);
    bool postAtTime(Task* task, long long when, int priority = -1);

    bool postAndWait(Task* task);

    bool sendMessage(int what, int arg1 = 0, int arg2 = 0, void* obj = NULL, int priority = -1)
    {
        return sendMessageDelayed(what, arg1, arg2, obj, 0, priority);
    }

    bool sendMessageDelayed(int what,
                            int arg1 = 0,
                            int arg2 = 0,
                            void* obj = NULL,
                            long long delayMillis = 0,
                            int priority = -1);

    bool sendMessageAtTime(int what,
                           int arg1 = 0,
                           int arg2 = 0,
                           void* obj = NULL,
                           long long when = 0,
                           int priority = -1);

    void removeTask(Task* task);
    void removeMessages(int what);
    virtual void removeTasksAndMessages();

    bool hasMessages(int what, void* obj = NULL);
    bool hasTasks(Task* task, void* obj = NULL);

    virtual void dispatchMessage(Message* msg);

    virtual void handleMessage(Message* msg);
    /**
	 * Called when recycle the Message.
	 */
    virtual void handleRecycleMessage(Message* msg);

private:
    JQuick::sp< Looper > mLooper;

    JQuick::Mutex mutex;
    JQuick::Condition cond;
};
}  // namespace JQuick

#endif /* ___GREENUI_HANDLER_H___ */
