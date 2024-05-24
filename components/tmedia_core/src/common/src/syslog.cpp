/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>

#define LOG_WITH_COLOR

#define LOG_COLOR_RED_YELLO_BACK "\033[1;31;43m"
#define LOG_COLOR_RED            "\033[2;31;49m"
#define LOG_COLOR_YELLOW         "\033[2;33;49m"
#define LOG_COLOR_GREEN          "\033[2;32;49m"
#define LOG_COLOR_BLUE           "\033[2;34;49m"
#define LOG_COLOR_GRAY           "\033[1;30m"
#define LOG_COLOR_WHITE          "\033[1;47;49m"
#define LOG_COLOR_RESET          "\033[0m"

#ifdef LOG_WITH_COLOR
const char *TM_PFORMAT_D    = LOG_COLOR_GRAY   "[%sD][%s:%s():%d] " LOG_COLOR_RESET;
const char *TM_PFORMAT_I    = LOG_COLOR_BLUE   "[%sI][%s:%s():%d] " LOG_COLOR_RESET;
const char *TM_PFORMAT_W    = LOG_COLOR_YELLOW "[%sW][%s:%s():%d] " LOG_COLOR_RESET;
const char *TM_PFORMAT_E    = LOG_COLOR_RED    "[%sE][%s:%s():%d] " LOG_COLOR_RESET;
const char *TM_PFORMAT_O    = LOG_COLOR_GREEN  "[%sO][%s:%s():%d] " LOG_COLOR_RESET;
#else
const char *TM_PFORMAT_D    = "[%sD][%s:%s():%d] ";
const char *TM_PFORMAT_I    = "[%sI][%s:%s():%d] ";
const char *TM_PFORMAT_W    = "[%sW][%s:%s():%d] ";
const char *TM_PFORMAT_E    = "[%sE][%s:%s():%d] ";
const char *TM_PFORMAT_O    = "[%sO][%s:%s():%d] ";
#endif
