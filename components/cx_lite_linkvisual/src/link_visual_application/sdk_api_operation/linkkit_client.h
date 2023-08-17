/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef PROJECT_LINKKIT_DEMO_H
#define PROJECT_LINKKIT_DEMO_H

#include "link_visual_struct.h"
#include "iot_export_linkkit.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef int (*iot_msg_cb)(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg);
typedef int (*connect_handle)(void);

/**
 * 文件用于描述linkkit的启动、结束过程，以及linkkit生命周期过程中对命令的收发处理
 */

/* linkkit支持一型一密模式，即通过product_key/product_secret/device_name获取到device_secret */
//#define LINKKIT_DYNAMIC_REGISTER

/* linkkit支持实例化 */
//#define LINKKIT_INSTANCE

/* ali-smartliving-sdk-c功能 */
//#define SMARTLIVING

/* linkkit资源初始化，并连接服务器 */
int linkkit_client_start(const iotx_linkkit_dev_meta_info_t *main,
                            unsigned int sub_num, const iotx_linkkit_dev_meta_info_t *sub);

/* linkkit与服务器断开连接，并释放资源 */
void linkkit_client_destroy();

int linkkit_message_publish_cb(const lv_message_publish_param_s *param);

/* 属性设置回调 */
void linkkit_client_set_property_handler(const lv_device_auth_s *auth, const char *value);

void linkkit_get_auth(int dev_id, lv_device_auth_s *auth, iotx_linkkit_dev_meta_info_t *sub_dev);

/* 注册linkkit消息用户接口 */
void iot_msg_callback_register(iot_msg_cb  cb);

#if defined(__cplusplus)
}
#endif
#endif //PROJECT_LINKKIT_DEMO_H
