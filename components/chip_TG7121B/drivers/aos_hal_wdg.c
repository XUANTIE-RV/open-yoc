#include "aos/hal/wdg.h"
#include <stddef.h>

#include "field_manipulate.h"
#include "reg_rcc_type.h"
#include "reg_lsiwdt_type.h"
#include "reg_lsiwdt.h"
#include "lsiwdt.h"
#include "reg_syscfg.h"

/**
 * This function will initialize the on board CPU hardware watch dog
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    if (NULL == wdg) 
    {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    REG_FIELD_WR(SYSCFG->CFG, SYSCFG_IWDG_DEBUG, 1);
    REG_FIELD_WR(RCC->CK, RCC_HSE_EN, 1);
    REG_FIELD_WR(RCC->CK, RCC_LSI_EN, 1);
    REG_FIELD_WR(RCC->AHBEN, RCC_IWDT, 1);

    IWDG_DeInit();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetClockCmd(IWDG_lrc_select);
    IWDG_SetLoad(wdg->config.timeout << 4); //period time 5s  5000000/((1/32)*1000)us  //32khz
    IWDG_ClearITFlag();
    IWDG_ResetEnable(ENABLE);
    IWDG_Enable(ENABLE);

    return 0;
}

/**
 * Reload watchdog counter.
 *
 * @param[in]  wdg  the watch dog device
 */
void hal_wdg_reload(wdg_dev_t *wdg)
{
    if (NULL == wdg) 
    {
        printf("input param invalid in %s \r\n", __func__);
        return ;
    }

    IWDG_SetLoad(wdg->config.timeout << 4);
    IWDG_ClearITFlag();

}

/**
 * This function performs any platform-specific cleanup needed for hardware watch dog.
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    if (NULL == wdg) 
    {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    REG_FIELD_WR(RCC->AHBRST, RCC_IWDT, 1);
    REG_FIELD_WR(RCC->AHBRST, RCC_IWDT, 0);
    REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_EN, 0);
    REG_FIELD_WR(RCC->AHBEN, RCC_IWDT, 1);

    return 0;
}