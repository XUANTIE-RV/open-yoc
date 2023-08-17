/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>
#include <mmio.h>

static void cvi_ephy_id_init(void)
{
	// set rg_ephy_apb_rw_sel 0x0804@[0]=1/APB by using APB interface
	mmio_write_32(0x03009804, 0x0001);

	// Release 0x0800[0]=0/shutdown
	mmio_write_32(0x03009800, 0x0900);

	// Release 0x0800[2]=1/dig_rst_n, Let mii_reg can be accessabile
	mmio_write_32(0x03009800, 0x0904);

	// PHY_ID
	mmio_write_32(0x03009008, 0x0043);
	mmio_write_32(0x0300900c, 0x5649);

	// switch to MDIO control by ETH_MAC
	mmio_write_32(0x03009804, 0x0000);
}

void board_eth_init(void)
{

    cvi_ephy_id_init();
extern void drv_eth_register(void);
    drv_eth_register();
}
