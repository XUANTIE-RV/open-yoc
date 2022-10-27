#ifndef __APP_INIT_H
#define __APP_INIT_H

void board_cli_init(void);
void board_yoc_init(void);
int cli_reg_cmd_ft(void);
void led_pinmux_init(void);
int ft_btn_init(void);

#endif
