/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _IOT_ONENET_API_
#define _IOT_ONENET_API_
#include <stdint.h>
#include <aos/list.h>
#include "cis_api.h"

#define S(x) #x
#define STR(x) S(x)

#define STR_VALUE_LEN         161
#define OPAQUE_VALUE_LEN      161
#define ON_WRITE_LEN          70
#define ONENET_STACK_SIZE     2048
#define MAX_ONE_NET_AT_SIZE   120
#define MAX_SET_PARAM_SIZE    80
#define MAX_SET_PARAM_AT_SIZE 150
#define MAX_INS_MAP_SIZE      30
#define CIS_PARAM_ERROR       601
#define CIS_STATUS_ERROR      602
#define CIS_UNKNOWN_ERROR     100

struct onenet_conf {
    int total_size;
    char *cur_config;
    int index;
    int cur_size;
    int flag;
};

struct onenet_addobj {
    unsigned int ref;
    unsigned int obj_id;
    unsigned int ins_count;
    char ins_map[MAX_INS_MAP_SIZE];
    unsigned int attr_count;
    unsigned int action_count;
};

struct onenet_update {
    unsigned int ref;
    unsigned int lifetime;
    unsigned int with_obj_flag;
};

struct onenet_discover_rsp {
    unsigned int ref;
    unsigned int msg_id;
    unsigned int result;
    unsigned int length;
    char         value[MAX_SET_PARAM_SIZE];
};

struct result_code_map {
    unsigned char at_result_code;
    unsigned char coap_result_code;
};

struct onenet_parameter_rsp {
    unsigned int ref;
    unsigned int msg_id;
    unsigned int result;
};

struct onenet_delobj {
    unsigned int ref;
    unsigned int obj_id;
};

struct onenet_write_exe {
    unsigned int ref;
    unsigned int msg_id;
    unsigned int result;
};

struct onenet_notify_read {
    unsigned int ref;
    unsigned int msg_id;
    unsigned int obj_id;
    unsigned int ins_id;
    unsigned int res_id;
    unsigned int value_type;
    unsigned int len;
    char *value;
    unsigned int ackid;
    unsigned int last;
};

struct onenet_obs_node {
    slist_t node;
    uint16_t ref;
    uint32_t msg_id;
    uint16_t obj_id;
    uint16_t ins_id;
    uint16_t res_id;
};

enum onenet_rsp_type {
    RSP_READ = 0,
    RSP_WRITE,
    RSP_EXECUTE,
    RSP_OBSERVE,
    RSP_SETPARAMS,
    RSP_DISCOVER,
};

int iot_onenet_miplcreate(void);
int iot_onenet_mipladdobj(struct onenet_addobj *param);
int iot_onenet_mipldelobj(unsigned int obj_id);
int iot_onenet_miplopen(unsigned int liftime);
int iot_onenet_miplclose(void);
int iot_onenet_mipldel(void);
int iot_onenet_miplnotify(struct onenet_notify_read *param);
int iot_onenet_miplreadrsp(struct onenet_notify_read *param);

/* support write rsp & execute rsp */
int iot_onenet_miplchangedrsp(uint32_t mid);

int iot_onenet_miplobserveresp(struct onenet_write_exe param);
int iot_onenet_miplparameterresp(struct onenet_parameter_rsp param);
int iot_onenet_mipldiscoverresp(int mid, uint8_t result, int *atts, int count);
int iot_onenet_miplupdate(struct onenet_update param);
char *iot_onenet_miplver(void);
int iot_onenet_get_notify_mid(uint16_t obj_id, uint16_t ins_id, uint32_t *ptr_mid);

#endif
