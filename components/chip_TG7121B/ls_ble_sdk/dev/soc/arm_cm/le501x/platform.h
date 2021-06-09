#ifndef PLATFORM_H_
#define PLATFORM_H_
#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "compile_flag.h"
#include "prf_fotas.h"
#define RESET_OTA_SUCCEED      0xDBDBDBDB
#define RESET_OTA_FAILED       0xBDBDBDBD
#define RESET_OTA_REQ          0xDDDDDDDD

enum OTA_settings_type
{
    SINGLE_FOREGROUND = 0,
    DOUBLE_FOREGROUND = 1,
    DOUBLE_BACKGROUND = 2,
    OTA_SETTINGS_TYPE_MAX = DOUBLE_BACKGROUND + 1,
};

struct reset_retain_struct
{
    uint32_t reset_reason;
    uint8_t wakeup_source;
};

void switch_to_rc32k(void);

void switch_to_xo16m(void);

bool clk_check(void);

void clk_switch(void);

uint8_t get_reset_source(void);

uint8_t get_wakeup_source(void);

void sys_init_app(void);

void sys_init_itf(void);

void irq_reinit(void);

void uart_eif_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *,uint8_t), void* dummy);

void uart_eif_write(uint8_t *bufptr, uint32_t size, void (*callback)(void *,uint8_t), void* dummy);

void uart_eif_flow_on(void);

bool uart_eif_flow_off(void);

void stack_assert_asm(uint32_t,uint32_t,uint32_t);

void platform_reset(uint32_t error);

void ecc_calc_start(const uint8_t* secret_key,const uint8_t* pub_x,const uint8_t* pub_y,uint8_t* result_x,uint8_t* result_y,void (*cb)(void *),void *param);

uint64_t idiv_acc(uint32_t,uint32_t,bool);
void ls_ip_aes_encrypt_start(void (*cb)(void),const uint8_t* aes_key,const uint8_t* aes_val);
void ls_ip_aes_encrypt_complete(void (*cb)(uint32_t *),uint32_t* param);
uint32_t  lstrng_random(void);

void switch_to_hse(void);

void arm_cm_delay_asm(uint32_t);

void request_ota_reboot(void);

void power_up_hardware_modules(void);

void remove_hw_isolation(void);

uint32_t config_word_get(uint32_t offset);

void arm_cm_set_int_isr(uint8_t type,void (*isr)());


#define DELAY_US(a) arm_cm_delay_asm((a)*(SDK_HCLK_MHZ/5))

void ota_settings_erase(void);

void ota_settings_write(uint32_t ota_settings_type);

uint32_t ota_settings_read(void);

bool ota_copy_info_get(struct fota_image_info *ptr);

void ota_copy_info_set(struct fota_image_info *ptr);

uint32_t get_app_image_base(void);

uint32_t get_fota_image_base(void);

#define OSTICK_HS_INC(Hz) (2000*1000/((Hz)*625))
#define OSTICK_HUS_INC(Hz) (2000*1000/(Hz) - 625*OSTICK_HS_INC(Hz))
#endif
