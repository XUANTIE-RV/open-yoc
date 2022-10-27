/**************************************************************************************************

  Phyplus Microelectronics Limited confidential and proprietary.
  All rights reserved.

  IMPORTANT: All rights of this software belong to Phyplus Microelectronics
  Limited ("Phyplus"). Your use of this Software is limited to those
  specific rights granted under  the terms of the business contract, the
  confidential agreement, the non-disclosure agreement and any other forms
  of agreements as a customer or a partner of Phyplus. You may not use this
  Software unless you agree to abide by the terms of these agreements.
  You acknowledge that the Software may not be modified, copied,
  distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy
  (BLE) integrated circuit, either as a product or is integrated into your
  products.  Other than for the aforementioned purposes, you may not use,
  reproduce, copy, prepare derivative works of, modify, distribute, perform,
  display or sell this Software and/or its documentation for any purposes.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

**************************************************************************************************/
#include "rom_sym_def.h"
#include "types.h"
#include "ll_sleep.h"
#include "bus_dev.h"
#include "string.h"

#include "pwrmgr.h"
#include "error.h"
#include "gpio.h"
//#include "log.h"
#include "clock.h"

#if(CFG_SLEEP_MODE == PWR_MODE_NO_SLEEP)
static uint8_t mPwrMode = PWR_MODE_NO_SLEEP;
#elif(CFG_SLEEP_MODE == PWR_MODE_SLEEP)
static uint8_t mPwrMode = PWR_MODE_SLEEP;
#elif(CFG_SLEEP_MODE == PWR_MODE_PWROFF_NO_SLEEP)
static uint8_t mPwrMode = PWR_MODE_PWROFF_NO_SLEEP;
#else
#error "CFG_SLEEP_MODE define incorrect"
#endif


typedef struct _pwrmgr_Context_t {
    MODULE_e     moudle_id;
    bool         lock;
    pwrmgr_Hdl_t sleep_handler;
    pwrmgr_Hdl_t wakeup_handler;
} pwrmgr_Ctx_t;

static pwrmgr_Ctx_t mCtx[HAL_PWRMGR_TASK_MAX_NUM];
static uint32_t sramRet_config;

int hal_pwrmgr_init(void)
{
    memset(&mCtx, 0, sizeof(mCtx));

    switch (mPwrMode) {
        case PWR_MODE_NO_SLEEP:
        case PWR_MODE_PWROFF_NO_SLEEP:
            disableSleep();
            break;

        case PWR_MODE_SLEEP:
            enableSleep();
            break;
    }

    return PPlus_SUCCESS;
}


bool hal_pwrmgr_is_lock(MODULE_e mod)
{
    int i;
    int ret = FALSE;

    if (mPwrMode == PWR_MODE_NO_SLEEP || mPwrMode == PWR_MODE_PWROFF_NO_SLEEP) {
        return TRUE;
    }

    _HAL_CS_ALLOC_();
    HAL_ENTER_CRITICAL_SECTION();

    for (i = 0; i < HAL_PWRMGR_TASK_MAX_NUM; i++) {
        if (mCtx[i].moudle_id == MOD_NONE) {
            break;
        }

        if (mCtx[i].moudle_id == mod) {
            if (mCtx[i].lock == TRUE) {
                ret = TRUE;
            }

            break;
        }
    }

    HAL_EXIT_CRITICAL_SECTION();
    return ret;
}


int hal_pwrmgr_lock(MODULE_e mod)
{
    int i;
    int ret = PPlus_ERR_NOT_REGISTED;

    if (mPwrMode == PWR_MODE_NO_SLEEP || mPwrMode == PWR_MODE_PWROFF_NO_SLEEP) {
        disableSleep();
        return PPlus_SUCCESS;
    }

    _HAL_CS_ALLOC_();
    HAL_ENTER_CRITICAL_SECTION();

    for (i = 0; i < HAL_PWRMGR_TASK_MAX_NUM; i++) {
        if (mCtx[i].moudle_id == MOD_NONE) {
            break;
        }

        if (mCtx[i].moudle_id == mod) {
            mCtx[i].lock = TRUE;
            disableSleep();
            //LOG("LOCK\n");
            ret = PPlus_SUCCESS;
            break;
        }
    }

    HAL_EXIT_CRITICAL_SECTION();
    return ret;
}

int hal_pwrmgr_unlock(MODULE_e mod)
{
    int i, cnt = 0;

    if (mPwrMode == PWR_MODE_NO_SLEEP || mPwrMode == PWR_MODE_PWROFF_NO_SLEEP) {
        disableSleep();
        return PPlus_SUCCESS;
    }

    _HAL_CS_ALLOC_();
    HAL_ENTER_CRITICAL_SECTION();

    for (i = 0; i < HAL_PWRMGR_TASK_MAX_NUM; i++) {
        if (mCtx[i].moudle_id == MOD_NONE) {
            break;
        }

        if (mCtx[i].moudle_id == mod) {
            mCtx[i].lock = FALSE;
        }

        if (mCtx[i].lock) {
            cnt ++;
        }
    }

    if (cnt == 0) {
        enableSleep();
    } else {
        disableSleep();
    }

    HAL_EXIT_CRITICAL_SECTION();

    //LOG("sleep mode:%d\n", isSleepAllow());

    return PPlus_SUCCESS;
}

int hal_pwrmgr_register(MODULE_e mod, pwrmgr_Hdl_t sleepHandle, pwrmgr_Hdl_t wakeupHandle)
{
    int i;
    pwrmgr_Ctx_t *pctx = NULL;

    for (i = 0; i < HAL_PWRMGR_TASK_MAX_NUM; i++) {
        if (mCtx[i].moudle_id == mod) {
            return PPlus_ERR_INVALID_STATE;
        }

        if (mCtx[i].moudle_id == MOD_NONE) {
            pctx = &mCtx[i];
            break;
        }
    }

    if (pctx == NULL) {
        return PPlus_ERR_NO_MEM;
    }

    pctx->lock = FALSE;
    pctx->moudle_id = mod;
    pctx->sleep_handler = sleepHandle;
    pctx->wakeup_handler = wakeupHandle;
    return PPlus_SUCCESS;
}

int hal_pwrmgr_unregister(MODULE_e mod)
{
    int i;
    pwrmgr_Ctx_t *pctx = NULL;

    for (i = 0; i < HAL_PWRMGR_TASK_MAX_NUM; i++) {
        if (mCtx[i].moudle_id == mod) {
            pctx = &mCtx[i];
            break;
        }

        if (mCtx[i].moudle_id == MOD_NONE) {
            return PPlus_ERR_NOT_REGISTED;
        }
    }

    if (pctx == NULL) {
        return PPlus_ERR_NOT_REGISTED;
    }

    _HAL_CS_ALLOC_();
    HAL_ENTER_CRITICAL_SECTION()
    memcpy(pctx, pctx + 1, sizeof(pwrmgr_Ctx_t) * (HAL_PWRMGR_TASK_MAX_NUM - i - 1));
    HAL_EXIT_CRITICAL_SECTION();
    return PPlus_SUCCESS;
}


int __attribute__((used)) hal_pwrmgr_wakeup_process(void)
{
    int i;

    //hal_system_init(SYS_CLK_XTAL_16M);//(SYS_CLK_DLL_32M);//SYS_CLK_XTAL_16M);   //system init
    for (i = 0; i < HAL_PWRMGR_TASK_MAX_NUM; i++) {
        if (mCtx[i].moudle_id == MOD_NONE) {
            return PPlus_ERR_NOT_REGISTED;
        }

        if (mCtx[i].wakeup_handler) {
            mCtx[i].wakeup_handler();
        }
    }

    return PPlus_SUCCESS;
}

int __attribute__((used)) hal_pwrmgr_sleep_process(void)
{
    int i;
    //20181013 ZQ :
    hal_pwrmgr_RAM_retention_set();

    //LOG("Sleep\n");
    for (i = 0; i < HAL_PWRMGR_TASK_MAX_NUM; i++) {
        if (mCtx[i].moudle_id == MOD_NONE) {
            return PPlus_ERR_NOT_REGISTED;
        }

        if (mCtx[i].sleep_handler) {
            mCtx[i].sleep_handler();
        }
    }



    return PPlus_SUCCESS;
}

/**************************************************************************************
 * @fn          hal_pwrmgr_RAM_retention
 *
 * @brief       This function process for enable retention sram
 *
 * input parameters
 *
 * @param       uint32_t sram: sram bit map
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      refer error.h.
 **************************************************************************************/
int hal_pwrmgr_RAM_retention(uint32_t sram)
{
    if (sram & 0xffffffe0) {
        sramRet_config = 0x00;
        return PPlus_ERR_INVALID_PARAM;
    }

    sramRet_config = sram;
    return PPlus_SUCCESS;
}

int hal_pwrmgr_RAM_retention_clr(void)
{
    subWriteReg(0x4000f01c, 21, 17, 0);
    return PPlus_SUCCESS;
}

int hal_pwrmgr_RAM_retention_set(void)
{
    subWriteReg(0x4000f01c, 21, 17, sramRet_config);
    return PPlus_SUCCESS;
}

int hal_pwrmgr_LowCurrentLdo_enable(void)
{
    subWriteReg(0x4000f014, 26, 26, 1);
    return PPlus_SUCCESS;
}

int hal_pwrmgr_LowCurrentLdo_disable(void)
{
    subWriteReg(0x4000f014, 26, 26, 0);
    return PPlus_SUCCESS;
}

int hal_pwrmgr_poweroff(pwroff_cfg_t *pcfg, uint8_t wakeup_pin_num)
{

    subWriteReg(0x4000f01c, 6, 6, 0x00); //disable software control

    uint8_t i = 0;

    for (i = 0; i < wakeup_pin_num; i++) {
        phy_gpio_wakeup_set(pcfg[i].pin, pcfg[i].type);
    }

    //system off
    write_reg(0x4000f000, 0x5a5aa5a5);
    return PPlus_SUCCESS;
}

void system_on_handler(GPIO_Pin_e pin, uint32_t timer)
{
    bool system_on_flag = FALSE;
    hal_rtc_clock_config(CLK_32K_RCOSC);
    uint32_t start_clock_tick;
    phy_gpio_pull_set(pin, PULL_DOWN);     //pull down the wake up pin
    pwroff_cfg_t pwr_wkp_cfg[] = {{pin, POSEDGE}};
    WaitMs(10);

    if (phy_gpio_read(pin) == 1) {
//      LOG("wkp");
        uint8_t tick_per_ms = 32;
        STOP_RTC;
        CLAER_RTC_COUNT;
        HAL_WAIT_CONDITION_TIMEOUT_WO_RETURN(FALSE, 500);
        RUN_RTC;
        start_clock_tick = clock_time_rtc();

//      HAL_WAIT_CONDITION(hal_gpio_read(P14)==0);
        while (phy_gpio_read(pin) != 0) {
            if ((clock_time_rtc() - start_clock_tick) >= (tick_per_ms * timer)) {
                system_on_flag = TRUE;
                break;
            }
        }

        if (system_on_flag == FALSE) {
            hal_pwrmgr_poweroff(pwr_wkp_cfg, sizeof(pwr_wkp_cfg) / sizeof(pwr_wkp_cfg[0]));

            while (1);
        }
    } else if (phy_gpio_read(pin) == 0) {
        hal_pwrmgr_poweroff(pwr_wkp_cfg, sizeof(pwr_wkp_cfg) / sizeof(pwr_wkp_cfg[0]));

        while (1);
    }


}




