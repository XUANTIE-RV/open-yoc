/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_BASE_THREAD_H___
#define ___JQUICK_BASE_THREAD_H___

#include <string>
#include "utils/Condition.h"
#include "utils/Mutex.h"
#include "port/jquick_thread.h"

namespace JQuick
{
class Thread;

typedef void (*ReleaseThreadFunction)(Thread*);

/**
 * Used to create an async thread, and run some job.
 * Exp: SomeThread* t = new SomeThread("myjob");
 *      t->start();
 */
class Thread
{
public:
    Thread(const std::string& name, ReleaseThreadFunction release = 0);
    Thread(const std::string& name, int priority, ReleaseThreadFunction release = 0);
    virtual ~Thread();
    virtual size_t getStackSize() const;
    virtual int getPriority() const;

    virtual void start();
    /**
     * 执行线程任务
     * 可继承之后执行真正的任务
     */
    virtual void run();

    bool isActive();

    /* internal use */
    void threadRun();
    void tryRelease();
    /* internal use */
private:
    void setName();

protected:
    bool mActive;
    JQuick_Thread mId;

    int mPriority;

    ReleaseThreadFunction _releaseFunc;

    std::string _threadName;

protected:
    JQuick::Mutex mutex;
    JQuick::Condition cond;
};

static void* _threadRun(void* arg)
{
#if defined(__linux__)
    // Disable pthread_cancel. We need Manage the thread lifecycle ourselves.
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif
    Thread* _this = static_cast< Thread* >(arg);
    if (_this) {
        _this->threadRun();
    }
    _this->tryRelease();
    return (void*)0;
}

inline Thread::Thread(const std::string& name, ReleaseThreadFunction release) :
        mActive(false),
        mId(NULL),
        mPriority(0),
        _releaseFunc(release),
        _threadName(name)
{
}

inline Thread::Thread(const std::string& name, int priority, ReleaseThreadFunction release) :
        mActive(false),
        mId(NULL),
        mPriority(priority),
        _releaseFunc(release),
        _threadName(name)
{
}

inline Thread::~Thread()
{
    if (mId) {
        jquick_thread_destroy(mId);
        mId = NULL;
    }
}

inline size_t Thread::getStackSize() const
{
#if defined(__ALIOS__)
    return 8192;
#else
    // 默认0，使用系统默认值
    return 0;
#endif
}

inline int Thread::getPriority() const
{
    return mPriority;
}

inline void Thread::start()
{
    mutex.lock();

    size_t stackSize = getStackSize();
    if (stackSize > 0) {
        mId = jquick_thread_create_with_stack_size(_threadName.c_str(), _threadRun, this, stackSize);
    } else {
        mId = jquick_thread_create(_threadName.c_str(), _threadRun, this);
    }
    if (mId) {
        int priority = getPriority();
        if (priority > 0 && priority < 200) {
            jquick_thread_set_priority(mId, priority);
        }

        mActive = true;
    } else {
        fprintf(stderr, "Can not create thread(%s)\n", _threadName.c_str());
    }
    mutex.unlock();
}

inline void Thread::threadRun()
{
    setName();
    run();
    mutex.lock();
    mActive = false;
    mutex.unlock();
    if (mId) {
        jquick_thread_destroy(mId);
        mId = NULL;
    }
}
inline void Thread::run()
{
}

inline bool Thread::isActive()
{
    return mActive;
}

inline void Thread::tryRelease()
{
    if (_releaseFunc) {
        _releaseFunc(this);
    } else {
        delete this;
    }
}

inline void Thread::setName()
{
    if (!_threadName.empty()) {
        jquick_thread_set_current_name(_threadName.c_str());
        jquick_thread_set_name(mId, _threadName.c_str());
    }
}

}  // namespace JQuick

#endif /* ___JQUICK_BASE_THREAD_H___ */
