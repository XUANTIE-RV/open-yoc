/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef __GATEWAY_MGMT_OFFLINE_H_
#define __GATEWAY_MGMT_OFFLINE_H_

#define GW_ACTIVE_CHECK_NODE_MAX_NUM      6
#define GW_ACTIVE_CHECK_MAX_RETRY_NUM     5
#define GW_ACTIVE_CHECK_INTERVAL          2000
#define GW_HB_CHECK_MAX_LOSS              4
#define GW_HB_CHECK_INTERVAL              2000
#define GW_HB_PERIOD_NOT_CHECK            0xFFFF
#define GW_DEF_HB_SUBDEV_CHECK_PERIOD     (4)  // 4S
#define GW_DEF_HB_LPM_SUBDEV_CHECK_PERIOD (10) // 10S

typedef struct {
    gw_subdev_t subdev;
    uint16_t    period;
} gateway_check_subdev_msg_t;

int  gateway_mgmt_offline_check_init();
int  gateway_default_offline_check_timeout_get(uint8_t lpm_type);
void gateway_subdev_hb_recv(gw_subdev_ctx_t *subdev_ctx);
void gateway_subdev_active_status_recv(gw_subdev_ctx_t *subdev_ctx, gw_active_check_status_t active_status);
int  gateway_subdev_hb_pub_status_update(gw_subdev_ctx_t *subdev_ctx, gw_subdev_hb_set_status_t hb_set_status);

#endif
