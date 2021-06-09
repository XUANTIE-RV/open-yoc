#ifndef PRF_FOTAS_H_
#define PRF_FOTAS_H_
#include <stdint.h>

enum fotas_evt_type
{
    FOTAS_START_REQ_EVT,
    FOTAS_FINISH_EVT,
};

enum fota_start_cfm_status
{
    FOTA_REQ_ACCEPTED,
    FOTA_REQ_REJECTED,
};

struct fota_signature
{
    uint8_t data[64];
};

struct fw_digest
{
    uint8_t data[32];
};

struct fota_image_info
{
    uint32_t base;
    uint32_t size;
};

struct fotas_start_req_evt
{
    struct fota_signature *signature;
    struct fw_digest *digest;
};

struct fotas_finish_evt
{
    struct fota_image_info *new_image;
    bool integrity_checking_result;
};

union fotas_evt_u
{
    struct fotas_start_req_evt fotas_start_req;
    struct fotas_finish_evt fotas_finish;
};


void prf_fota_server_callback_init(void (*evt_cb)(enum fotas_evt_type,union fotas_evt_u *,uint8_t));

void dev_manager_prf_fota_server_add(uint8_t sec_lvl);

void prf_fotas_start_confirm(uint8_t con_idx,enum fota_start_cfm_status status);
#endif

