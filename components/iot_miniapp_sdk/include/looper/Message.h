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
#ifndef ___JQUICK_BASE_MESSAGE_H___
#define ___JQUICK_BASE_MESSAGE_H___

#include "looper/Task.h"
#include <memory>

namespace JQuick
{
class Handler;

class Message
{
public:
    Message(JQuick::sp< Handler > h, Task* t, int priority = -1) :
            what(0), arg1(0), arg2(0), obj(NULL), task(t), when(0), priority(priority), target(h), next(NULL)
    {
    }
    Message(JQuick::sp< Handler > h, int w = 0, int a1 = 0, int a2 = 0, void* o = NULL, int priority = -1) :
            what(w), arg1(a1), arg2(a2), obj(o), task(NULL), when(0), priority(priority), target(h), next(NULL)
    {
    }
    ~Message()
    {
        if (task) {
            task->tryCleanup();
        }
        task = NULL;
        next = NULL;
        target = NULL;
    }
    int what;
    int arg1;
    int arg2;
    void* obj;
    Task* task;

    long long when;
    int priority;
    JQuick::sp< Handler > target;

    Message* next;
};
}  // namespace JQuick

#endif /* ___JQUICK_BASE_MESSAGE_H___ */
