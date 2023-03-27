#ifndef _GATEWAY_PORT_H_
#define _GATEWAY_PORT_H_


int gateway_porting_subgrp_set_brightness(gw_subdev_protocol_e protocal,gw_subgrp_t subgrp, uint16_t brightness);
int gateway_porting_subgrp_set_onoff(gw_subdev_protocol_e protocal,gw_subgrp_t subgrp, uint8_t onoff);
int gateway_porting_gw_hb_recv_enable(uint8_t enable);
int gateway_porting_gw_discover(uint8_t enable, uint32_t timeout);
int gateway_porting_subdev_add(gw_discovered_dev_t subdev);
int gateway_porting_subdev_del(gw_subdev_ctx_t *subdev_ctx);
int gateway_porting_dev_simple_rst(uint8_t protocol, void *arg);
int gateway_porting_subdev_sub_del(gw_subdev_ctx_t *subdev_ctx, gw_subgrp_t subgrp);
int gateway_porting_subdev_get_triples(gw_subdev_ctx_t *subdev_ctx, uint8_t triples_flag);
int gateway_porting_subdev_sub_add(gw_subdev_ctx_t *subdev_ctx, gw_subgrp_t subgrp);
int gateway_porting_subdev_set_onoff(gw_subdev_ctx_t *subdev_ctx, uint8_t onoff);
int gateway_porting_subdev_set_brightness(gw_subdev_ctx_t *subdev_ctx, uint16_t brightness);
int gateway_porting_subdev_send_rawdata(gw_subdev_ctx_t *subdev_ctx, uint8_t *raw_data, int data_len);
int gateway_porting_subdev_get_onoff(gw_subdev_ctx_t *subdev_ctx);
int gateway_porting_subdev_get_brightness(gw_subdev_ctx_t *subdev_ctx);
int gateway_porting_subdev_heartbeat_period_set(gw_subdev_ctx_t *subdev_ctx, uint8_t enable, uint16_t period_s);

#endif