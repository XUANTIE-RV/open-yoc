#ifndef PRF_BASS_H_
#define PRF_BASS_H_
#include <stdint.h>

///Maximal number of BAS that can be added in the DB
#define BASS_NB_BAS_INSTANCES_MAX         (2)
#define PRF_BASS_BATTERY_LVL_MAX 100

enum bass_evt_type
{
    BASS_READ_BATT,
};

union bass_evt_u
{
    uint8_t temp;
};

struct bas_db_cfg
{
    uint8_t ins_num;
    uint8_t ntf_enable[BASS_NB_BAS_INSTANCES_MAX];
};

void prf_bass_server_callback_init(void (*evt_cb)(enum bass_evt_type, union bass_evt_u *,uint8_t));

void dev_manager_prf_bass_server_add(uint8_t sec_lvl,struct bas_db_cfg*cfg,uint16_t len);

void bas_batt_lvl_update(uint8_t ins_idx, uint8_t batt_lvl);

#endif
