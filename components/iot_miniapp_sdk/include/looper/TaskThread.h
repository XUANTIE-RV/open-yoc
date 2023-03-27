/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_BASE_TASK_THREAD_H___
#define ___JQUICK_BASE_TASK_THREAD_H___

#include <string>
#include "looper/Thread.h"
#include "looper/Task.h"

namespace JQuick
{
class TaskThread : public Thread
{
public:
    TaskThread(const std::string& name, Task* task, ReleaseThreadFunction release = 0);
    ~TaskThread();
    void run();
private:
    Task* _task;
};
inline TaskThread::TaskThread(const std::string& name, Task* task, ReleaseThreadFunction release) :
        Thread(name, release),
        _task(task)
{
}
inline TaskThread::~TaskThread()
{
    if (_task != NULL) {
        _task->tryCleanup();
        _task = NULL;
    }
}

inline void TaskThread::run()
{
    if (_task != NULL) {
        _task->run();
        _task->tryCleanup();
        _task = NULL;
    }
    Thread::run();
}
}

#endif  // ___JQUICK_BASE_TASK_THREAD_H___
