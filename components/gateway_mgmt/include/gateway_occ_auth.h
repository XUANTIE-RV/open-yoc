/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _GATEWAY_OCC_AUTH_H_
#define _GATEWAY_OCC_AUTH_H_

#define PRODUCT_KEY_MAXLEN       (20 + 1)
#define DEVICE_NAME_MAXLEN       (32 + 1)
#define DEVICE_SECRET_MAXLEN     (64 + 1)
#define PRODUCT_SECRET_MAXLEN    (64 + 1)
#define OCC_PID_MAXLEN           (32 + 1)
#define GW_OCC_AUTH_WAIT_FOREVER 0xFFFFFFFF
#define TRIGET_FLAG              "triget:"

#ifndef CONFIG_OCC_AUTH_TASK_PRIO
#define CONFIG_OCC_AUTH_TASK_PRIO (16)
#endif

#ifndef CONFIG_OCC_AUTH_STACK_SIZE
#define CONFIG_OCC_AUTH_STACK_SIZE (4096)
#endif

#ifndef CONFIG_OCC_AUTH_NODE_MAX_MSG_SIZE
#define CONFIG_OCC_AUTH_NODE_MAX_MSG_SIZE 20
#endif

typedef struct {
    char device_name[DEVICE_NAME_MAXLEN];
    char product_key[PRODUCT_KEY_MAXLEN];
    char product_secret[PRODUCT_SECRET_MAXLEN];
    char occ_pid[OCC_PID_MAXLEN];
} gw_subdev_triples_t;

typedef enum
{
    TRIPLE_TYPE_DN = 0x1,
    TRIPLE_TYPE_PK = 0x2,
    TRIPLE_TYPE_PS = 0x4,
} triple_type_en;

typedef enum
{
    OCC_AUTH_IDLE = 0x00,
    OCC_AUTH_IN,
    OCC_AUTH_FAILED,
    OCC_AUTH_SUCCESS,
} gw_occ_auth_status_en;

typedef enum
{
    GW_OCC_AUTH_TYPE_MESH = 0x0,
} gw_occ_auth_node_type_e;

typedef struct {
    uint8_t  addr[6];
    uint8_t  addr_type;
    uint8_t  uuid[16];
    uint8_t  bearer;
    uint16_t oob_info;
} mesh_occ_auth_in_t;

typedef struct {
    uint8_t  addr[6];
    uint8_t  addr_type;
    uint8_t  uuid[16];
    uint8_t  bearer;
    uint16_t oob_info;
    uint32_t short_oob;
    char     CID[33];
} mesh_occ_auth_out_t;

typedef union
{
    mesh_occ_auth_in_t mesh_occ_in_data;
} occ_protocol_info_in_t;

typedef union
{
    mesh_occ_auth_out_t mesh_occ_out_data;
} occ_protocol_info_out_t;

typedef struct {
    uint8_t                dev_protocal;
    occ_protocol_info_in_t in_data;
} gw_auth_in_t;

typedef struct {
    uint8_t                 dev_protocal;
    uint8_t                 auth_status;
    occ_protocol_info_out_t out_data;
} gw_auth_out_t;

typedef void (*occ_auth_cb)(gw_auth_out_t *out_info);
typedef void (*occ_js_get_cb)(void *p);

typedef struct gw_occ_auth_cb_s {
    occ_auth_cb              auth_cb;
    struct gw_occ_auth_cb_s *_next;
} gw_occ_auth_cb_t;

int   gateway_occ_auth_init();
int   gateway_occ_auth_enable(uint8_t enable);
int   gateway_occ_auth_is_enabled(void);
int   gateway_occ_auth_cb_register(gw_occ_auth_cb_t *cb);
int   gateway_occ_auth_cb_unregister(gw_occ_auth_cb_t *cb);
int   gateway_occ_node_auth(gw_auth_in_t auth_in_info, uint8_t *auth_status);
int   gateway_occ_node_auth_cache_remove(gw_auth_in_t auth_in_info);
int   gateway_occ_node_auth_cache_clear();
int   gateway_occ_node_auth_result_get(gw_auth_in_t auth_in_info, gw_auth_out_t *auth_out, uint32_t timeout);

#endif
