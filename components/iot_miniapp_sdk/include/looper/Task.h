/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */
#ifndef ___JQUICK_BASE_TASK_H___
#define ___JQUICK_BASE_TASK_H___

#include <algorithm>
#include "utils/Condition.h"
#include "utils/Functional.h"

namespace JQuick
{
class Task
{
public:
    const bool cleanup;

    Task(bool shouldCleanup = true) :
            cleanup(shouldCleanup)
    {
    }

    virtual void run() = 0;
    virtual ~Task() {}

    virtual void tryCleanup()
    {
        if (cleanup) {
            delete this;
        }
    }
};

class FunctionalTask : public Task
{
public:
    FunctionalTask(Closure func) :
            _closure(func)
    {
    }
    virtual ~FunctionalTask() {}
    virtual void run()
    {
        if (!_closure.isNull()) {
            _closure();
        }
    }

private:
    Closure _closure;
};

class BlockingTask : public Task
{
public:
    bool done;
    JQuick::Mutex* mutex;
    JQuick::Condition* cond;

    BlockingTask(Task* task, JQuick::Mutex* m, JQuick::Condition* c) :
            Task(false), done(false), mutex(m), cond(c), real_task(task)
    {
    }
    virtual ~BlockingTask()
    {
        if (real_task) {
            real_task->tryCleanup();
            real_task = NULL;
        }
    };

    void run()
    {
        if (real_task) {
            real_task->run();
        }
    }

    virtual void tryCleanup()
    {
        Task::tryCleanup();
        if (!done) {
            mutex->lock();
            done = true;
            cond->signal();
            mutex->unlock();
        }
    }

private:
    Task* real_task;
};

}  // namespace JQuick

#endif
