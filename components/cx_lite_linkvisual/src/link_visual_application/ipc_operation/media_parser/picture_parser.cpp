/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#include "picture_parser.h"

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "local_time.h"
#include "posix/timer.h"

PictureParser::PictureParser(PictureBufferHandler handler, void *arg) {
    assert(handler != NULL);
    system_start_time_ = GetLocalTime() / 1000;
    buffer_len_ = 0;
    picture_interval_ = kMinInterval;
    count_ = 0;
    handler_ = handler;
    arg_ = arg;
    buffer_ = new unsigned char[kMaxBufferLen];
    thread_handler_ = new ThreadEntry();
}

PictureParser::~PictureParser() {
    Stop();
    delete [] buffer_;
    delete thread_handler_;
}

int PictureParser::Start(unsigned int picture_interval, const std::string &file_name) {
    FILE *fp = fopen(file_name.c_str(), "r");
    if (!fp) {
        printf("Open file failed: %s\n", file_name.c_str());
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    unsigned int file_size = ftell(fp);
    if (file_size > kMaxBufferLen) {
        printf("File is too large: %s\n", file_name.c_str());
        fclose(fp);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    buffer_len_ = fread(buffer_, 1, file_size, fp);
    if(buffer_len_ != file_size) {
        printf("File read error: %s\n", file_name.c_str());
        fclose(fp);
        return -1;
    }
    fclose(fp);
    picture_interval_ = picture_interval < kMinInterval?kMinInterval:picture_interval;
    char name[] = "lv_pic";
    thread_handler_->Start(pictureThread, this, name, 30);

    return 0;
}

bool PictureParser::pictureThread(void *arg) {
    assert(arg != NULL);
    PictureParser *picture_parser = static_cast<PictureParser *>(arg);

    usleep(1000000);
    picture_parser->count_++;
    if (picture_parser->count_ >= picture_parser->picture_interval_) {
        picture_parser->count_ = 0;
        picture_parser->handler_(picture_parser->arg_, picture_parser->buffer_, picture_parser->buffer_len_, NULL);
    }
    return true;
}

void PictureParser::Stop() {
    thread_handler_->Stop();
}
