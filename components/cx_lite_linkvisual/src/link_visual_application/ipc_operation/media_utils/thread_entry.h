/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#ifndef PROJECT_THREAD_ENTRY_H
#define PROJECT_THREAD_ENTRY_H


typedef bool (*ThreadFunction)(void*);

/*
 * 通用线程类
 * */
class ThreadEntry {
public:
    ThreadEntry();
    ~ThreadEntry();

    void Start(ThreadFunction function, void *arg, const char *name, int prio);

    void Stop();

    bool GetStatus() const { return running_;};

private:
    static void *thread(void *parse);

private:
    bool running_;
    bool thread_not_quit_;
    ThreadFunction function_;
    void *arg_;
};

#endif //PROJECT_THREAD_ENTRY_H
