#ifndef _AT_CMD_PARSE_H_
#define _AT_CMD_PARSE_H_

#define ADDR_LEN 6
#define RECV_MAX_LEN 30

struct at_ctrl
{
    uint8_t transparent_start;
    uint8_t transparent_conidx;
    uint8_t one_slot_send_start;
    uint32_t one_slot_send_len;
} __attribute__((aligned(4)));

typedef struct _at_recv_cmd
{
    uint8_t recv_data[RECV_MAX_LEN];
    uint16_t recv_len;
}at_recv_cmd_t;

typedef struct at_defualt_info
{
    uint8_t auto_trans;
    uint8_t rfpower;
    uint8_t advint;
    uint8_t auto_sleep;
} default_info_t;

struct at_buff_env
{
    default_info_t default_info;

};
extern struct at_ctrl ls_at_ctl_env;
extern struct at_buff_env ls_at_buff_env;
extern const uint16_t adv_int_arr[6];
extern const uint16_t tx_power_arr[6];

void at_recv_cmd_handler(at_recv_cmd_t *recv_buf);
void trans_mode_enter(void);
void trans_mode_exit(void);

#endif
