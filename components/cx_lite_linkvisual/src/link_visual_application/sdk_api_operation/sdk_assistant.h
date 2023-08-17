/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef PROJECT_SDK_ASSISTANT_H
#define PROJECT_SDK_ASSISTANT_H

#include "link_visual_struct.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define string_safe_copy(target, source, max_len) \
    do {        \
    if (target && source && max_len) \
            memcpy(target, source, (strlen(source) > max_len)?max_len:strlen(source));\
} while(0)


lv_device_auth_s *lvDeviceAuthCopy(const lv_device_auth_s *auth, unsigned int num);

void lvDeviceAuthDelete(lv_device_auth_s *auth, unsigned int num);

unsigned int lvDeviceAuthCompares(const lv_device_auth_s *auth_1, unsigned int num, const lv_device_auth_s *auth_2);

#if defined(__cplusplus)
}
#endif

#endif //PROJECT_SDK_ASSISTANT_H
