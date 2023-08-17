/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#ifndef PROJECT_PICTURE_PARSER_H
#define PROJECT_PICTURE_PARSER_H

#include <string>
#include <stdint.h>

#include "thread_entry.h"

typedef void (*PictureBufferHandler)(void *arg, unsigned char *buffer, unsigned int buffer_len, const char *id);

class PictureParser {
public:
    PictureParser(PictureBufferHandler handler, void *arg);
    ~PictureParser();

    int Start(unsigned int picture_interval, const std::string &file_name);

    void Stop();

    unsigned int GetPictureLen() const {return buffer_len_;};

    unsigned char *GetPictureData() const {return buffer_;};

private:
    static bool pictureThread(void *arg);

private:
    unsigned int buffer_len_;
    unsigned int picture_interval_;
    unsigned int count_;
    unsigned char* buffer_;
    ThreadEntry *thread_handler_;
    PictureBufferHandler handler_;
    void *arg_;
    uint64_t system_start_time_;
private:
    static const unsigned int kMaxBufferLen = 64 *1024;
    static const unsigned int kMinInterval = 1;
};


#endif //PROJECT_PICTURE_PARSER_H
