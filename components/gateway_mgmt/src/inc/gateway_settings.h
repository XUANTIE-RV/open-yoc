#ifndef _GW_SETTINGS_H_
#define _GW_SETTINGS_H_

int gateway_subdev_ctx_store(gw_subdev_ctx_t *subdev_ctx);
int gateway_subdev_ctx_load(gw_subdev_ctx_t *subdev_ctx);
int gateway_subdev_ctx_delete(gw_subdev_ctx_t *subdev_ctx);
int gateway_subdev_name_store(gw_subdev_ctx_t *subdev_ctx);
int gateway_subdev_name_load(gw_subdev_ctx_t *subdev_ctx);
int gateway_subdev_name_delete(gw_subdev_ctx_t *subdev_ctx);
int gateway_subgrp_ctx_store(gw_subgrp_ctx_t *subgrp_ctx);
int gateway_subgrp_ctx_load(gw_subgrp_ctx_t *subgrp_ctx);
int gateway_subgrp_ctx_delete(gw_subgrp_ctx_t *subgrp_ctx);
int gateway_subgrp_name_store(gw_subgrp_ctx_t *subgrp_ctx);
int gateway_subgrp_name_load(gw_subgrp_ctx_t *subgrp_ctx);
int gateway_subgrp_name_delete(gw_subgrp_ctx_t *subgrp_ctx);

#endif
