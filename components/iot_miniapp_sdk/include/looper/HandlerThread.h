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
#ifndef ___JQUICK_BASE_HANDLERTHREAD_H___
#define ___JQUICK_BASE_HANDLERTHREAD_H___

#include "looper/Handler.h"
#include "looper/Looper.h"
#include "looper/Thread.h"

namespace JQuick
{
/**
 * Used to create an MessageQueue based thread.
 * Exp: JQuick::HandlerThread* t = new JQuick::HandlerThread("myjob");
 *      t->start();
 *      JQuick::sp<JQuick::Handler> h = new JQuick::Handler(t->getLooper());
 *      h->post(new JQuick::FunctionalTask(JQuick::bind(&func)));
 */
class HandlerThread : public Thread
{
public:
    HandlerThread(const std::string& name, ReleaseThreadFunction release = 0);
    HandlerThread(const std::string& name, int priority, ReleaseThreadFunction release = 0);
    virtual ~HandlerThread();

    virtual void start();

    size_t getStackSize() const;

    void stop();

    virtual void run();
    JQuick::sp< Looper > getLooper();

private:
    JQuick::sp< Looper > mLooper;
};

}  // namespace JQuick

#endif /* ___GREENUI_HANDLERTHREAD_H___ */
