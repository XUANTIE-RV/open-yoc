/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */
#ifndef ___JQUICK_THREADPOOL_H___
#define ___JQUICK_THREADPOOL_H___

#include "looper/Thread.h"
#include "looper/Task.h"
#include "utils/Mutex.h"
#include "utils/Condition.h"
#include <list>
#include <string>

namespace JQuick
{
class Worker;
class ThreadPoolTask : public Task

{
public:
    ThreadPoolTask(const std::string& groupId, const std::string& taskName, Closure func);
    ThreadPoolTask(const std::string& groupId, const std::string& taskName, Task* realTask);
    ThreadPoolTask(Task* realTask);
    ThreadPoolTask();
    virtual ~ThreadPoolTask();
    void run();

protected:
    virtual void runTask();

public:
    std::string _groupId;
    std::string _taskName;
    Task* _realTask;
};

/**
 * Used to create a thread pool, and run some job.
 * Exp: ThreadPool* t = new ThreadPool("myjob");
 *      t->execute(new ThreadPoolTask);
 */
class ThreadPool
{
public:
    /**
     * create pool instance with {new ThreadPool}
     * destroy with {shutdown()}, will delete self automatic when all worker thread exit.
     */
    ThreadPool(const std::string& poolName, int32_t corePoolSize = 1, int32_t dynamicPoolSize = 0, size_t stackSize = 0);
    ~ThreadPool();
    void execute(ThreadPoolTask* task);
    void execute(Closure func);
    void execute(const std::string& groupId, const std::string& taskName, JQuick::Closure func);
    bool removeTask(ThreadPoolTask* task);
    int32_t removeTaskGroup(const std::string& groupId);

    void shutdown();

private:
    void addCoreWorker();
    void addDynamicWorker();
    void removeWorker(Worker*);
    void tryCleanup();

    ThreadPool(const ThreadPool& o);
    ThreadPool& operator=(const ThreadPool&);

private:
    // Task lock
    mutable Mutex _taskLock;
    mutable Condition _condition;
    // Worker lock
    mutable Mutex _lock;

    std::string _poolName;
    bool _active;
    int32_t _corePoolSize;
    int32_t _dynamicPoolSize;
    int32_t _workerIdCounter;
    size_t _stackSize;

    std::list< ThreadPoolTask* > _tasks;
    // 核心线程，常驻直到线程池退出
    std::list< Worker* > _coreWorkers;
    // 动态线程，空闲10秒后退出
    std::list< Worker* > _dynamicWorkers;

    int32_t _idleWorkers;

    friend class Worker;
};
}  // namespace JQuick
#endif