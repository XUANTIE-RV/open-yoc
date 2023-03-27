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
#ifndef ___JQUICK_BASE_MESSAGEQUEUE_H___
#define ___JQUICK_BASE_MESSAGEQUEUE_H___

#include "looper/Message.h"
#include "utils/Mutex.h"
#include "utils/Condition.h"

#include <vector>
#include <map>

typedef void (*MessageNotifier)(JQuick::Message*);

namespace JQuick
{
class Handler;

class MessageQueue
{
public:
    MessageQueue(bool quitAllowed);
    ~MessageQueue();

    /**
     * return next message to be handled,
     * NULL if queue has already quit.
     * Blocked if there is no next message or time has not reached.
     * @return next message.
     */
    Message* next();
    Message* poll(long long* nextWhen = NULL);
    void setMessageNotifier(MessageNotifier notifier);

    /**
     * Quits the MessageQueue.
     * Remove all messages if safe is false.
     * If safe is true, terminate the queue as soon as all remaining messages
     * in the message queue that are already due to be delivered have been handled.
     * However pending delayed messages with due times in the future will not be
     * delivered
     * @param safe
     */
    void quit(bool safe);

    /**
     * Remove any pending posts of messages with target 'h' and code 'what'
     * that are in the message queue.
     */
    void removeMessage(JQuick::sp< Handler > h, int what);
    /**
     * Remove any pending posts of messages with target 'h' and task 'task'
     * that are in the message queue.
     */
    void removeTask(JQuick::sp< Handler > h, Task* task);
    /**
     * Remove any pending posts of messages with target 'h' that are
     * in the message queue.
     */
    void removeMessages(JQuick::sp< Handler > h);

    /**
     * Check if there are any pending posts of messages with target 'h' and
     * code 'what' in the message queue.
     */
    bool hasMessages(JQuick::sp< Handler > h, int what, void* obj = NULL);
    /**
     * Check if there are any pending posts of messages with target 'h' and
     * task 'task' in the message queue.
     */
    bool hasTasks(JQuick::sp< Handler > h, Task* task, void* obj = NULL);

    /**
     * Enqueue a message at the front of the message queue, to be processed on
     * the next iteration of the message loop.  You will receive it in
     * {@link Handler#handleMessage}, in the thread attached to this handler.
     *
     * @return Returns true if the message was successfully placed in to the
     *         message queue.  Returns false on failure, usually because the
     *         looper processing the message queue is exiting.
     */
    bool enqueueMessage(Message* msg, long long when);

    bool isIdling() const;

private:
    mutable JQuick::Mutex mLock;
    mutable JQuick::Condition mCondition;

    void pollOnce(long long timeoutMillis);
    void wake(bool debug = false);

    Message* mMessages;
    bool mQuitAllowed;
    bool mBlocked;
    bool mQuitting;

    bool mIdling;
    JQuick::Mutex mMessageLock;

    MessageNotifier mNotifier;

    friend class Handler;
};
}  // namespace JQuick

#endif /* ___JQUICK_BASE_MESSAGEQUEUE_H___  */
