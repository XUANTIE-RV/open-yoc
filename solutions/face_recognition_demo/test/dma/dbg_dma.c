#include <cvi_dma.h>
#include <aos/cli.h>

void dma_dbg_show_dma(int argc, char **argv)
{
	dma_show_info();
}

void dma_dbg_show_all_ch(int argc, char **argv)
{
	dma_show_ch_info_all();
}

void dma_dbg_show_ch(int argc, char **argv)
{
	int ch_id = -1;

	if(argc != 2){
		dma_err("Usage: dma_show_ch [ch_id]\r\n");
		return;
	}

	if(argv[1][0] > '7' || argv[1][0] < '0'){
		dma_err("ch_id error. support 0~7");
		return;
	}

	ch_id = argv[1][0] - '0';
	dma_show_ch_info(ch_id);
}

ALIOS_CLI_CMD_REGISTER(dma_dbg_show_dma, dma_show_status, Show dma status);
ALIOS_CLI_CMD_REGISTER(dma_dbg_show_all_ch, dma_show_all_ch, Show all channel status);
ALIOS_CLI_CMD_REGISTER(dma_dbg_show_ch, dma_show_ch, Show channel status. Usage: dma_show_ch [ch_id]);
