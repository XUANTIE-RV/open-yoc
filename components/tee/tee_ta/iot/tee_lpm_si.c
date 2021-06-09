/* ****************************************************************************
 *                                                                          *
 * C-Sky Microsystems Confidential                                          *
 * -------------------------------                                          *
 * This file and all its contents are properties of C-Sky Microsystems. The *
 * information contained herein is confidential and proprietary and is not  *
 * to be disclosed outside of C-Sky Microsystems except under a             *
 * Non-Disclosured Agreement (NDA).                                         *
 *                                                                          *
 ****************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define PMU_IRAM_AON_BASE     (0x20110000)
#define WAKEUP_CODE_SIZE      (100)
#define WAKE_UP_CODE_ADDRESS  (PMU_IRAM_AON_BASE + 0x100)

#if (CONFIG_ALGO_LPM > 0)

extern void eflash_powon(void);
extern unsigned int arch_tw_do_cpu_save(void);
extern unsigned int arch_tw_do_cpu_resume(void);
extern int tee_efc_sleep(uint32_t *efc_lpm_save_ptr);
extern int32_t tee_efc_resume(uint32_t *efc_lpm_save_ptr);
unsigned int tee_efc_save[37];
static void wake_code_init(void)
{
    memset((void *)(WAKE_UP_CODE_ADDRESS),0x0,WAKEUP_CODE_SIZE);
    memcpy((void *)WAKE_UP_CODE_ADDRESS,(void *)eflash_powon,WAKEUP_CODE_SIZE);//(eflash_shutdown_recovery_end-eflash_shutdown_recovery)
    //outl((unsigned int)(WAKE_UP_CODE_ADDRESS), 0x0);
    *(volatile unsigned int *)(0x0) = 0x100;
}

static void do_cpu_sleep(void)
{
    __asm__("stop");

}

void _tw_to_stop(void)
{
    //tee_efc_sleep(tee_efc_save);
    wake_code_init();
    //while(cc);
    if (arch_tw_do_cpu_save() == 0) {
            do_cpu_sleep();
    }

    arch_tw_do_cpu_resume();
    //tee_efc_resume(tee_efc_save);
}
#endif
