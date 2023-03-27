/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _GATEWAY_MESH_H_
#define _GATEWAY_MESH_H_

#include <api/mesh.h>
#include <mesh_model/mesh_model.h>
#include "gateway_mgmt.h"

#define GET_NODE_VER_TIMEOUT     4 * 1000
#define GET_NODE_VER_INTERVAL    2 * 1000
#define GET_NODE_VER_CNT_MAX     3
#define GATEWAY_DEFAULT_MOD_ID   0x1000
#define GATEWAY_DEFAULT_SUB_ADDR 0xC000

#define DEF_GW_AUTO_ADD_APPKEY_ONOFF 1

#define GW_MESH_DEV_NAME             "GW_MESH_PROVISIONER"
#define GW_DEF_ATTENTION_TIMEOUT     (15) // s
#define GW_DEF_FOUND_DEV_TIMEOUT     (0)  // s
#define GW_DEF_AUTO_ADD_APPKEY_ONOFF (1)
#define GW_AUTOCONFIG_DST_SUB_ADDR   CONFIG_BT_MESH_AUTOCONFIG_DST_SUB_ADDR

#ifndef CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_SRC_ADDR
#define CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_SRC_ADDR 0x0000 // Not assigned,for all nodes
#endif

#ifndef CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_DST_ADDR
#define CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_DST_ADDR 0xF003
#endif

#ifndef CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_PERIOD_LOG
#define CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_PERIOD_LOG 0x3 // 4s
#endif

#ifndef CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_NET_IDX
#define CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_NET_IDX 0x0
#endif

#ifndef CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_TTL
#define CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_TTL 0x7
#endif

#ifndef CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_FEAD
#define CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_FEAD 0x0F // RELAY/PROXY/FRIEND/LPN
#endif

#ifndef CONFIG_GW_MESH_GW_START_ADDR
#define CONFIG_GW_MESH_GW_START_ADDR 0x0001
#endif

#ifndef CONFIG_GW_MESH_GW_END_ADDR
#define CONFIG_GW_MESH_GW_END_ADDR (CONFIG_GW_MESH_GW_START_ADDR + CONFIG_BT_MESH_MAX_PROV_NODES)
#endif

typedef struct {
    char *                     dev_name;
    const struct bt_mesh_comp *comp;
    model_event_cb             user_model_cb;
} gateway_mesh_config_t;

typedef struct add_config_s {
    uint8_t  addr_type;
    uint8_t  uuid[16];
    uint8_t  bearer;
    uint16_t oob_info;
} add_config_t;

typedef struct prov_addr_config_s {
    uint32_t start_addr;
    uint32_t end_addr;
} prov_addr_config_t;

typedef struct room_and_device_type_s {
    char *room_type;
    char *device_type;
} room_and_device_type_t;

typedef int (*gateway_mgmt_update_status_t)(gw_subdev_status_update_dev_info *dev_info, void *status, int status_len,
                                            enum_subdev_status_type_e status_type);
typedef int (*gateway_mgmt_update_event_t)(void *msg, int msg_len, enum_mgmt_msg_type_e msg_type);

typedef struct _gateway_mgmt_callback_s {
    gateway_mgmt_update_status_t mgmt_update_status_cb;
    gateway_mgmt_update_event_t  mgmt_update_event_cb;
} gateway_mgmt_callback_t;

int      gateway_mesh_init();
void     gateway_mesh_mgmt_cb_register(gateway_mgmt_callback_t gw_cb);
char *   gateway_mesh_node_cid_get(uint16_t unicast_addr);
int      gateway_mesh_set_onoff(uint16_t unicast_addr, uint8_t onoff);
int      gateway_mesh_set_brightness(uint16_t unicast_addr, uint16_t bri);
int      gateway_mesh_set_cct(uint16_t unicast_addr, uint16_t cct);
int      gateway_mesh_sub_add(uint16_t unicast_addr, uint16_t sub_addr);
int      gateway_mesh_control_sub_onoff(uint16_t sub_addr, uint8_t onoff);
int      gateway_mesh_auth_input(uint8_t action, void *data, u8_t size);
int      gateway_mesh_gw_set_onoff(uint8_t onoff);
int      gateway_mesh_prov_enable(uint8_t enable);
void     gateway_mesh_add_mac_filter(uint16_t unicast_addr_str);
void     gateway_mesh_del_mac_filter(uint16_t unicast_addr_str);
int      gateway_mesh_suspend(void);
int      gateway_mesh_resume(void);
int      gateway_mesh_del_node(uint16_t unicast_addr);
int      gateway_mesh_prov_add_dev(gw_ble_mesh_info_t ctx);
int      gateway_mesh_prov_showdev(uint8_t show_dev, uint32_t timeout);
uint16_t gateway_mesh_get_subgrp_subaddr(gw_subgrp_t subgrp);
int      gateway_mesh_sub_del(uint16_t unicast_addr, uint16_t sub_addr);
int      gateway_mesh_send_rawdata(uint16_t unicast_addr, uint8_t *rawdata, int data_len);
int      gateway_mesh_composition_init(void);
int      gateway_btmesh_prov_config(uint32_t start_addr, uint32_t end_addr);
int      gateway_btmesh_prov_autoconfig(void);
int      gateway_btmesh_prov_enable(uint8_t enable);
void     gateway_mesh_mgmt_cb_register(gateway_mgmt_callback_t gw_cb);
int      gateway_mesh_get_onoff(uint16_t unicast_addr);
int      gateway_mesh_get_brightness(uint16_t unicast_addr);
int      gateway_mesh_node_triples_get(uint16_t unicast_addr, uint8_t triples_flag);
int      gateway_mesh_hb_config_check(uint16_t unicast_addr, uint16_t period);
int      gateway_mesh_node_version_get(uint16_t unicast_addr);
int      gateway_mesh_hb_recv_enable(uint8_t enable);
int      gateway_mesh_hb_pub_set(uint16_t unicast_addr, uint8_t enable, uint16_t period_s);
int      gateway_mesh_prov_node_auto_config(uint16_t unicast_addr);

struct bt_mesh_node_t *gateway_mesh_get_node_info_by_mac(uint16_t unicast_addr);

#endif
