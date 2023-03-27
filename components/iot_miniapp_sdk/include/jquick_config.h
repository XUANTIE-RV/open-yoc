/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_BASE_CONFIG_H___
#define ___JQUICK_BASE_CONFIG_H___

#if defined(__linux__)
#define JQUICK_EXPORT __attribute__((visibility("default")))
#else
#define JQUICK_EXPORT
#endif

#endif  //___JQUICK_BASE_CONFIG_H___
