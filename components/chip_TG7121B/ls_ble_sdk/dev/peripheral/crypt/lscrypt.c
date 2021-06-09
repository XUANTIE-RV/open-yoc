
#include <string.h>
#include "le501x.h"
#include "ls_ble.h"
#include "platform.h"
#include "field_manipulate.h"
#include "reg_lscrypt.h"
#include "reg_rcc.h"
#include "sys_stat.h"

#define AES_KEY_LEN    (16)
#define AES_DATA_LEN   (16)

static void (*lscrypt_isr_cb)(void);

static void lscrypt_reset(void)
{
    REG_FIELD_WR(LSCRYPT->IVR0,CRYPT_IVR0,0);
    REG_FIELD_WR(LSCRYPT->IVR1,CRYPT_IVR1,0);
    REG_FIELD_WR(LSCRYPT->IVR2,CRYPT_IVR2,0);
    REG_FIELD_WR(LSCRYPT->IVR3,CRYPT_IVR3,0);

    REG_FIELD_WR(LSCRYPT->KEY0,CRYPT_KEY0,0);
    REG_FIELD_WR(LSCRYPT->KEY1,CRYPT_KEY1,0);
    REG_FIELD_WR(LSCRYPT->KEY2,CRYPT_KEY2,0);
    REG_FIELD_WR(LSCRYPT->KEY3,CRYPT_KEY3,0);
    REG_FIELD_WR(LSCRYPT->KEY4,CRYPT_KEY4,0);
    REG_FIELD_WR(LSCRYPT->KEY5,CRYPT_KEY5,0);
    REG_FIELD_WR(LSCRYPT->KEY6,CRYPT_KEY4,0);
    REG_FIELD_WR(LSCRYPT->KEY7,CRYPT_KEY5,0);

    REG_FIELD_WR(LSCRYPT->DATA0,CRYPT_DATA0,0);
    REG_FIELD_WR(LSCRYPT->DATA1,CRYPT_DATA1,0);
    REG_FIELD_WR(LSCRYPT->DATA2,CRYPT_DATA2,0);
    REG_FIELD_WR(LSCRYPT->DATA3,CRYPT_DATA3,0);

    REG_FIELD_WR(LSCRYPT->RES0,CRYPT_RES0,0);
    REG_FIELD_WR(LSCRYPT->RES1,CRYPT_RES1,0);
    REG_FIELD_WR(LSCRYPT->RES2,CRYPT_RES2,0);
    REG_FIELD_WR(LSCRYPT->RES3,CRYPT_RES3,0);
}
void CRYPT_Handler(void);
void lscrypt_init(void)
{
    __HAL_RCC_CRYPT_CLK_ENABLE();
    lscrypt_reset();

    REG_FIELD_WR(LSCRYPT->CR,CRYPT_AESKS,0);   //128bits
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_MODE,1);    //CBC  
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_TYPE,0);   //32bits no exchange 
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_FIFOEN,1);  //enable fifo  128bits data will auto trigger crypt
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_FIFOODR,0); //first DATA3/RES3
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_DMAEN,0);  //enable DMA
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_CRYSEL,0);  //enable AES  algorithm
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_IE,1);     //enable CRYPT interrupt	           

    arm_cm_set_int_isr(CRYPT_IRQn,CRYPT_Handler);             
    NVIC_EnableIRQ(CRYPT_IRQn);
}

void lscrypt_set_ivr_value(uint8_t const *ivr_val)
{
    uint32_t tmp_ivr_val[AES_KEY_LEN>>2]= {0};
    memcpy((uint8_t *)&tmp_ivr_val[0], ivr_val, AES_KEY_LEN);

    REG_FIELD_WR(LSCRYPT->IVR0,CRYPT_IVR0,tmp_ivr_val[0]);
    REG_FIELD_WR(LSCRYPT->IVR1,CRYPT_IVR1,tmp_ivr_val[1]);
    REG_FIELD_WR(LSCRYPT->IVR2,CRYPT_IVR2,tmp_ivr_val[2]);
    REG_FIELD_WR(LSCRYPT->IVR3,CRYPT_IVR3,tmp_ivr_val[3]);
}

void lscrypt_write_key(uint8_t const *key_val)
{
    uint32_t tmp_key_val[AES_KEY_LEN>>2]= {0};
    memcpy((uint8_t *)&tmp_key_val[0], key_val, AES_KEY_LEN);

    REG_FIELD_WR(LSCRYPT->KEY0,CRYPT_KEY0,tmp_key_val[0]);
    REG_FIELD_WR(LSCRYPT->KEY1,CRYPT_KEY1,tmp_key_val[1]);
    REG_FIELD_WR(LSCRYPT->KEY2,CRYPT_KEY2,tmp_key_val[2]);
    REG_FIELD_WR(LSCRYPT->KEY3,CRYPT_KEY3,tmp_key_val[3]);

}

void lscrypt_write_data(uint8_t const *data_val)
{
    uint32_t tmp_data_val[AES_DATA_LEN>>2]= {0};
    memcpy((uint8_t *)&tmp_data_val[0], data_val, AES_DATA_LEN);

    REG_FIELD_WR(LSCRYPT->CR,CRYPT_IVREN,1);  
    REG_FIELD_WR(LSCRYPT->FIFO,CRYPT_FIFO,tmp_data_val[0]);   
    REG_FIELD_WR(LSCRYPT->FIFO,CRYPT_FIFO,tmp_data_val[1]);
    REG_FIELD_WR(LSCRYPT->FIFO,CRYPT_FIFO,tmp_data_val[2]);   
    REG_FIELD_WR(LSCRYPT->FIFO,CRYPT_FIFO,tmp_data_val[3]);                
}

void ls_ip_aes_encrypt_start(void (*cb)(void),const uint8_t* aes_key,const uint8_t* aes_val)
{
    lscrypt_isr_cb = cb;
	NVIC_EnableIRQ(CRYPT_IRQn);
    crypt_status_set(true); 
    REG_FIELD_WR(LSCRYPT->CR,CRYPT_ENCS,1); 
    lscrypt_write_key(aes_key);
	lscrypt_write_data(aes_val);
}

void ls_ip_aes_encrypt_complete(void (*cb)(uint32_t *),uint32_t *param)
{
    param[0] = REG_FIELD_RD(LSCRYPT->RES0,CRYPT_RES0);
    param[1] = REG_FIELD_RD(LSCRYPT->RES1,CRYPT_RES1);
    param[2] = REG_FIELD_RD(LSCRYPT->RES2,CRYPT_RES2);
    param[3] = REG_FIELD_RD(LSCRYPT->RES3,CRYPT_RES3);
    
    crypt_status_set(false);

   cb(param);
}


void CRYPT_Handler(void)
{
    REG_FIELD_WR(LSCRYPT->ICRF, CRYPT_AESIF,1); 
    lscrypt_isr_cb();
}
