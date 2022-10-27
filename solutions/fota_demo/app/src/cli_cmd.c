#include <aos/cli.h>

extern void cli_reg_cmd_kvtool(void);
extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_ifconfig(void);

void board_cli_init()
{
    aos_cli_init();

    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();
    cli_reg_cmd_kvtool();
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    extern void cli_reg_cmd_otaab(void);
    cli_reg_cmd_otaab();
#endif
}
