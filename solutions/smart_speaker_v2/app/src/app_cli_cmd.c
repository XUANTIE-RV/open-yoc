/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#include <aos/cli.h>

extern void cli_reg_cmd_kvtool(void);
extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_iperf(void);
extern void cli_reg_cmd_ifconfig(void);
extern void cli_reg_cmd_aui(void);
extern void cli_reg_cmd_record(void);
extern void cli_reg_cmd_pcminput(void);
extern void cli_reg_cmd_appsys(void);
extern void cli_reg_cmd_voice(void);
extern void cli_reg_cmd_keymsg(void);
extern void cli_reg_cmd_display(void);
extern void cli_reg_cmd_status_event(void);
extern void cli_reg_cmd_player(void);
extern void cli_reg_cmd_eqset(void);
extern void cli_reg_cmd_gpio(void);
extern void cli_reg_cmd_pwm(void);
extern void cli_reg_cmd_gadc(void);
extern void cli_reg_cmd_logipc(void);
extern void cli_reg_cmd_adb_config(void);
extern void cli_reg_cmd_fstst(void);
extern void cli_reg_cmd_nvram(void);
extern void cli_reg_cmd_factory(void);
extern void cli_reg_cmd_bt(void);
extern void cli_reg_cmd_clock(void);
extern void cli_reg_cmd_kwstest(void);
extern void cli_reg_cmd_free(void);

void app_cli_init(void)
{
    cli_reg_cmd_kvtool();
    cli_reg_cmd_ping();
    cli_reg_cmd_iperf();
    cli_reg_cmd_ifconfig();
    cli_reg_cmd_aui();
    cli_reg_cmd_record();
    cli_reg_cmd_pcminput();
    cli_reg_cmd_appsys();
    cli_reg_cmd_voice();
    cli_reg_cmd_keymsg();
    cli_reg_cmd_display();
    cli_reg_cmd_status_event();
    cli_reg_cmd_free();

#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0
    cli_reg_cmd_player();
    cli_reg_cmd_eqset();
#endif

    cli_reg_cmd_gpio();
    cli_reg_cmd_pwm();

#ifndef CONFIG_HAL_ADC_DISABLED
    cli_reg_cmd_gadc();
#endif

#if defined(CONFIG_COMP_LOG_IPC) && CONFIG_COMP_LOG_IPC
    cli_reg_cmd_logipc();
#endif

    cli_reg_cmd_adb_config();
    cli_reg_cmd_fstst();

#ifdef CONFIG_STANDALONE_NVRAM
    cli_reg_cmd_nvram();
#endif
    cli_reg_cmd_factory();

#if (defined(CONFIG_BT_A2DP) && (CONFIG_BT_A2DP == 1)) || (defined(CONFIG_BT_HFP) && (CONFIG_BT_HFP == 1))
    cli_reg_cmd_bt();
#endif

    cli_reg_cmd_clock();

#ifdef CONFIG_KWS_TEST
    cli_reg_cmd_kwstest();
#endif
}
