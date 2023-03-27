#ifndef __CVI_MIPI_TX_H__
#define __CVI_MIPI_TX_H__

#include "mipi_tx_uapi.h"
#include "board_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MIPI_TX_RT_PRIO 30

struct panel_desc_s {
	char *panel_name;
	struct combo_dev_cfg_s *dev_cfg;
	const struct hs_settle_s *hs_timing_cfg;
	const struct dsc_instr *dsi_init_cmds;
	int dsi_init_cmds_size;
};

int mipi_tx_cfg(int fd, const struct combo_dev_cfg_s *dev_cfg);
int mipi_tx_send_cmd(int fd, struct cmd_info_s *cmd_info);
int mipi_tx_recv_cmd(int fd, struct get_cmd_info_s *cmd_info);
int mipi_tx_enable(int fd);
int mipi_tx_disable(int fd);
int mipi_tx_set_hs_settle(int fd, const struct hs_settle_s *hs_cfg);
int mipi_tx_get_hs_settle(int fd, struct hs_settle_s *hs_cfg);
int mipi_tx_poweroff(int fd);
int mipi_tx_init(struct panel_desc_s *panel_desc);
int mipi_tx_suspend(void);
int mipi_tx_resume(struct panel_desc_s *panel_desc);
int mipi_tx_rstpin(int devno);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // __CVI_MIPI_TX_H__
