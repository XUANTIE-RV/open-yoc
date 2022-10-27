/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __AT_MESH_H
#define __AT_MESH_H

#ifdef CONFIG_UNITTEST_MODE
#define _STATIC
#else
#define _STATIC static
#endif

#define ABS(errno)         ((errno) < (0) ? (-errno) : (errno))
#define MESH_BASE_ERR      0x00000000
#define MESH_AT_ERR        0x000007D0
#define ERR_MESH(errno)    (-(MESH_BASE_ERR + ABS(errno)))
#define ERR_MESH_AT(errno) (-(MESH_AT_ERR + ABS(errno)))

#define AT_STATUS_OK                              (0)
#define AT_ERR_MESH_AT_INIT_FAILED                 (-11)
#define AT_ERR_MESH_AT_NOT_INIT                   (-12)
#define AT_ERR_MESH_RST_FAILED                     (-13)
#define AT_ERR_MESH_KV_UNLOAD                     (-14)
#define AT_ERR_MESH_CMD_NOT_SUPPORTED             (-15)
#define AT_ERR_MESH_INIT_COMP_FAILED               (-16)
#define AT_ERR_MESH_INIT_NODE_FAILED               (-17)
#define AT_ERR_MESH_NOT_FOUND_MODEL               (-18)
#define AT_ERR_MESH_PROVISIONER_NOT_ENABLED       (-19)
#define AT_ERR_MESH_SET_UUID_FILTER_FAILED         (-20)
#define AT_ERR_MESH_SHOW_DEV_FAILED                (-21)
#define AT_ERR_MESH_ADD_NODE_FAILED                (-22)
#define AT_ERR_MESH_ADD_DEV_FAILED                 (-23)
#define AT_ERR_MESH_DEL_DEV_FAILED                 (-24)
#define AT_ERR_MESH_INPUT_OOB_FAILED               (-25)
#define AT_ERR_MESH_OOB_NOT_NEED                  (-26)
#define AT_ERR_MESH_GET_NODE_INFO_FAILED           (-27)
#define AT_ERR_MESH_GET_NODE_COMP_FAILED           (-28)
#define AT_ERR_MESH_GET_NETKEY_FAILED              (-29)
#define AT_ERR_MESH_GET_APPKEY_FAILED              (-30)
#define AT_ERR_MESH_ADD_NETKEY_FAILED              (-31)
#define AT_ERR_MESH_ADD_APPKEY_FAILED              (-32)
#define AT_ERR_MESH_BIND_APPKEY_FAILED             (-33)
#define AT_ERR_MESH_GET_SUB_ADDR_FAILED            (-34)
#define AT_ERR_MESH_ADD_SUB_ADDR_FAILED            (-35)
#define AT_ERR_MESH_DEL_SUB_ADDR_FAILED            (-36)
#define AT_ERR_MESH_GET_PUB_ADDR_FAILED            (-37)
#define AT_ERR_MESH_ADD_PUB_ADDR_FAILED            (-38)
#define AT_ERR_MESH_AUTO_CONFIG_FAILED             (-39)
#define AT_ERR_MESH_TX_FAILED                     (-40)
#define AT_ERR_MESH_ENABLE_MAC_FILTER_FAILED       (-41)
#define AT_ERR_MESH_DISABLE_MAC_FILTER_FAILED      (-42)
#define AT_ERR_MESH_ADD_MAC_FILTER_DEV_FAILED      (-43)
#define AT_ERR_MESH_DEL_MAC_FILTER_DEV_FAILED      (-44)
#define AT_ERR_MESH_CLR_MAC_FILTER_DEV_FAILED      (-45)
#define AT_ERR_MESH_GET_DEVKEY_FAILED              (-46)
#define AT_ERR_MESH_PROVISIONER_CONFIG_FAILED      (-47)
#define AT_ERR_MESH_PROVISIONER_GET_CONFIG_FAILED  (-48)
#define AT_ERR_MESH_PROVISIONER_AUTO_PROV_FAILED   (-49)
#define AT_ERR_MESH_PROVISIONER_ENABLE_FAILED      (-50)
#define AT_ERR_MESH_PROVISIONER_DISABLE_FAILED     (-51)
#define AT_ERR_MESH_PROVISIONER_SET_LPM_FLAG_FAIL (-52)
#define AT_ERR_MESH_GET_VERSION_FAIL              (-53)
#define AT_ERR_MESH_OVERWRITE_SUB_LIST_FAIL       (-54)
#define AT_ERR_MESH_ACTIVE_CHECK_FAIL             (-55)

#define DEF_BEARER                         0X03
#define DEF_PROVISIONER_UNICAST_ADDR_LOCAL 0x0001
#define DEF_PROVISIONER_UNICAST_ADDR_START 0x0002
#define DEV_UUID                                                                                                       \
    {                                                                                                                  \
        0xcf, 0xa0, 0xe3, 0x7e, 0x17, 0xd9, 0x11, 0xe8, 0x86, 0xd1, 0x5f, 0x1c, 0xe2, 0x8a, 0xde, 0x02                 \
    }
#define DEV_NAME "AT_MESH_PROVISIONER"

#define DEF_AUTO_ADD_APPKEY_ONOFF (1)
#define DEF_FOUND_DEV_TIMEOUT     (0) // s

#define DEF_AUTO_CONFIG_TIMEOUT_MIN (5) // s
#define DEF_AUTO_CONFIG_TIMEOUT     DEF_AUTO_CONFIG_TIMEOUT_MIN

#define DEF_ATTENTION_TIMEOUT  (15) // s
#define DEF_PUB_SET_TTL        (3)
#define DEF_PUB_SET_PERIOD     (0)
#define DEF_PUB_SET_COUNT      (0)
#define DEF_PUB_SET_INTERVAlL  (0)
#define DEF_PUB_SET_APPKEY_IDX (0)
#define DEF_HB_PUB_SET_TTL     (3)
#define DEF_HB_PUB_SET_PERIOD  (0)
#define DEF_HB_PUB_SET_COUNT   (0xFF)
#define DEF_HB_PUB_SET_FEAT    (0x0F) // RELAY/PROXY/FRIEND/LPN

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
#define DEF_OCC_AUTH_NODE_TIMEOUT (10000) // 10s
#endif

#define DEF_SEND_APPKEY_IDX (0)
#if defined(CONFIG_BT_MESH_EXT_ADV) && CONFIG_BT_MESH_EXT_ADV > 0
#define DEF_TRS_MAX_LENGTH 410
#else
#define DEF_TRS_MAX_LENGTH 88
#endif

#define AUTOCONFIG_DST_SUB_ADDR CONFIG_BT_MESH_AUTOCONFIG_DST_SUB_ADDR
#define AUTOCONFIG_DST_PUB_ADDR CONFIG_BT_MESH_AUTOCONFIG_DST_PUB_ADDR

typedef enum
{
    OOB_NULL,
    OOB_NUM,
    OOB_STR,
    OOB_STATIC,
} oob_type_en;

#define DEF_NODE_ROLE (NODE | PROVISIONER)

typedef void (*at_mesh_cb)(uint32_t event, void *p_arg);

int at_mesh_init(at_mesh_cb app_cb);

#endif
