#ifndef _DSI_PANELS_
#define _DSI_PANELS_

#include "cvi_vip.h"
#include "cvi_type.h"
#include "cvi_mipi_tx.h"
#include "cvi_comm_mipi_tx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#if CONFIG_PANEL_ILI9488
#include "dsi_ili9488.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "ILI9488-320x480",
	.dev_cfg = &dev_cfg_ili9488_320x480,
	.hs_timing_cfg = &hs_timing_cfg_ili9488_320x480,
	.dsi_init_cmds = dsi_init_cmds_ili9488_320x480,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ili9488_320x480)
};
#elif CONFIG_PANEL_HX8394
#include "dsi_hx8394_evb.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "HX8394-720x1280",
	.dev_cfg = &dev_cfg_hx8394_720x1280,
	.hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280,
	.dsi_init_cmds = dsi_init_cmds_hx8394_720x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280)
};
#elif CONFIG_PANEL_ST7701S
#include "dsi_st7701s.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "ST7701S-480x480",
	.dev_cfg = &dev_cfg_st7701s_480x480,
	.hs_timing_cfg = &hs_timing_cfg_st7701s_480x480,
	.dsi_init_cmds = dsi_init_cmds_st7701s_480x480,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_st7701s_480x480)
};
#elif CONFIG_PANEL_GC9503CV
#include "dsi_gc9503cv.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "GC9503CV-480x800",
	.dev_cfg = &dev_cfg_gc9503cv_480x800,
	.hs_timing_cfg = &hs_timing_cfg_gc9503cv_480x800,
	.dsi_init_cmds = dsi_init_cmds_gc9503cv_480x800,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_gc9503cv_480x800)
};
#else
#include "dsi_hx8394_evb.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "HX8394-720x1280",
	.dev_cfg = &dev_cfg_hx8394_720x1280,
	.hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280,
	.dsi_init_cmds = dsi_init_cmds_hx8394_720x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280)
};
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _DSI_PANELS_
