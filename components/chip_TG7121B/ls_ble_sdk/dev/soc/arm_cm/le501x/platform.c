#define LOG_TAG "PLF"
#include <string.h>
#include <stdlib.h>
#include "platform.h"
#include "calc_div.h"
#include "cpu.h"
#include "tinyfs_config.h"
#include "tinyfs.h"
#include "sleep.h"
#include "le501x.h"
#include "log.h"
#include "lsqspi_msp.h"
#include "spi_flash.h"
#include "compile_flag.h"
#include "lscache.h"
#include "reg_rcc.h"
#include "modem_rf_le501x.h"
#include "calc_acc.h"
#include "builtin_timer.h"
#include "reg_syscfg.h"
#include "lsecc.h"
#include "lstrng.h"
#include "lscrypt.h"
#include "field_manipulate.h"
#include "io_config.h"
#include "ls_dbg.h"
#include "systick.h"
#define ISR_VECTOR_ADDR ((uint32_t *)(0x0))
#define APP_IMAGE_BASE_OFFSET (0x24)
#define FOTA_IMAGE_BASE_OFFSET (0x28)
#define DATA_STORAGE_BASE_OFFSET (0x2c)
#define BASEBAND_MEMORY_ADDR   (0x50004000)
#define IRQ_NVIC_PRIO(IRQn,priority) (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn))

DEF_BUILTIN_TIMER_ENV(SDK_SW_TIMER_MAX);

void stack_var_ptr_init(void);

void main_task_app_init(void);

void main_task_itf_init(void);


static void bb_mem_clr(void)
{
    //memset((void *)BASEBAND_MEMORY_ADDR,0,16384);
}

static void irq_priority()
{
    __NVIC_SetPriority(SVCall_IRQn,1);
    NVIC->IP[0] = IRQ_NVIC_PRIO(EXTI_IRQn,2) | IRQ_NVIC_PRIO(WWDT_IRQn,2) | IRQ_NVIC_PRIO(LPWKUP_IRQn,2) | IRQ_NVIC_PRIO(BLE_IRQn,0);
    NVIC->IP[1] = IRQ_NVIC_PRIO(RTC_IRQn,2) | IRQ_NVIC_PRIO(DMA_IRQn,2) | IRQ_NVIC_PRIO(QSPI_IRQn,2) | IRQ_NVIC_PRIO(ECC_IRQn,2);
    NVIC->IP[2] = IRQ_NVIC_PRIO(CACHE_IRQn,2) | IRQ_NVIC_PRIO(TRNG_IRQn,2) | IRQ_NVIC_PRIO(IWDT_IRQn,2) | IRQ_NVIC_PRIO(CRYPT_IRQn,2);
    NVIC->IP[3] = IRQ_NVIC_PRIO(PDM_IRQn,2) | IRQ_NVIC_PRIO(BLE_WKUP_IRQn,0) | IRQ_NVIC_PRIO(ADC_IRQn,2) | IRQ_NVIC_PRIO(ADTIM1_IRQn,2);
    NVIC->IP[4] = IRQ_NVIC_PRIO(BSTIM1_IRQn,2) | IRQ_NVIC_PRIO(GPTIMA1_IRQn,2) | IRQ_NVIC_PRIO(GPTIMB1_IRQn,2) | IRQ_NVIC_PRIO(BLE_ERR_IRQn,0);
    NVIC->IP[5] = IRQ_NVIC_PRIO(LVD33_IRQn,2) | IRQ_NVIC_PRIO(GPTIMC1_IRQn,2) | IRQ_NVIC_PRIO(LPTIM_IRQn,2) | IRQ_NVIC_PRIO(I2C1_IRQn,2);
    NVIC->IP[6] = IRQ_NVIC_PRIO(I2C2_IRQn,2) | IRQ_NVIC_PRIO(SPI1_IRQn,2) | IRQ_NVIC_PRIO(SPI2_IRQn,2) | IRQ_NVIC_PRIO(UART1_IRQn,2);
    NVIC->IP[7] = IRQ_NVIC_PRIO(UART2_IRQn,2) | IRQ_NVIC_PRIO(UART3_IRQn,2) | IRQ_NVIC_PRIO(BLE_FIFO_IRQn,0) | IRQ_NVIC_PRIO(BLE_CRYPT_IRQn,0);
}

static void stack_data_bss_init()
{
    extern uint32_t __stack_bss_start__;
    extern uint32_t __stack_bss_size__;
    memset(&__stack_bss_start__,0,(uint32_t)&__stack_bss_size__);
    extern uint32_t __stack_data_lma__;
    extern uint32_t __stack_data_start__;
    extern uint32_t __stack_data_size__;
    memcpy(&__stack_data_start__,&__stack_data_lma__,(uint32_t)&__stack_data_size__);
}

static void ble_irq_clr()
{
    //clear ble irq
    NVIC->ICPR[0] = 1<<BLE_IRQn | 1<<BLE_ERR_IRQn | 1<<BLE_FIFO_IRQn | (unsigned int)1<<BLE_CRYPT_IRQn;
}

static void ble_irq_enable()
{
    //enable ble irq
    NVIC->ISER[0] = 1<<BLE_IRQn | 1<<BLE_ERR_IRQn | 1<<BLE_FIFO_IRQn | (unsigned int)1<<BLE_CRYPT_IRQn;
}

void ble_irq_clr_and_enable()
{
    ble_irq_clr();
    ble_irq_enable();
}

uint32_t config_word_get(uint32_t offset)
{
    uint32_t data;
    spi_flash_quad_io_read(offset,(uint8_t *)&data,sizeof(data));
    return data;
}

static uint32_t flash_data_storage_base_offset()
{
    return config_word_get(DATA_STORAGE_BASE_OFFSET);
}

void irq_reinit()
{
    irq_priority();
    NVIC->ISER[0] = 1<<LPWKUP_IRQn|1<<EXTI_IRQn;
}

static void irq_init()
{
    irq_reinit();
    ble_irq_clr_and_enable();
}

static void mac_init()
{
    uint8_t mac_clk;
    uint32_t clk_cfg = RCC->CFG;
    if(REG_FIELD_RD(clk_cfg, RCC_SYSCLK_SW) == 0x4)
    {
        if(REG_FIELD_RD(clk_cfg, RCC_HCLK_SCAL) == 0x8 )
        {
            mac_clk = 1; //AHB 32M
        }else
        {
            mac_clk = 2; //AHB 64M
        }
    }else
    {
        mac_clk = 0; //AHB 16M
    }
//    RCC->CK |= RCC_LSE_EN_MASK;
//    while(REG_FIELD_RD(RCC->CK,RCC_LSE_RDY)==0);
    RCC->BLECFG = mac_clk<<RCC_BLE_CK_SEL_POS| 1<<RCC_BLE_MRST_POS | 1<<RCC_BLE_CRYPT_RST_POS | 1<<RCC_BLE_LCK_RST_POS | 1<<RCC_BLE_AHB_RST_POS | 1<<RCC_BLE_WKUP_RST_POS
        | 1<<RCC_BLE_LPWR_CKEN_POS | 1<<RCC_BLE_AHBEN_POS | 1<<RCC_BLE_MDM_REFCLK_CKEN_POS;// | 1<<RCC_BLE_LCK_SEL_POS;
    RCC->BLECFG &= ~(1<<RCC_BLE_MRST_POS | 1<<RCC_BLE_CRYPT_RST_POS | 1<<RCC_BLE_LCK_RST_POS | 1<<RCC_BLE_AHB_RST_POS | 1<<RCC_BLE_WKUP_RST_POS);
}

void rco_calib_mode_set(uint8_t mode)
{
    REG_FIELD_WR(SYSCFG->ANACFG1, SYSCFG_RCO_MODE_SEL, mode);
}

void rco_calibration_start()
{
    REG_FIELD_WR(SYSCFG->ANACFG1, SYSCFG_EN_RCO_DIG_PWR, 1);
    REG_FIELD_WR(SYSCFG->ANACFG1, SYSCFG_RCO_CAL_START, 1);
    uint16_t cal_code;
    while(REG_FIELD_RD(SYSCFG->ANACFG1, SYSCFG_RCO_CAL_DONE)==0)
    {
        cal_code = REG_FIELD_RD(SYSCFG->ANACFG1, SYSCFG_RCO_CAL_CODE);
        SYSCFG->PMU_ANALOG = cal_code;
    }
    REG_FIELD_WR(SYSCFG->ANACFG1, SYSCFG_RCO_CAL_START, 0);
    REG_FIELD_WR(SYSCFG->ANACFG1, SYSCFG_EN_RCO_DIG_PWR, 0);
}

static void check_wkup_state(void)
{
    struct reset_retain_struct *reset_retain_ptr = (struct reset_retain_struct*)RESET_RETAIN_BASE;
    uint8_t wkup_stat = REG_FIELD_RD(SYSCFG->PMU_WKUP,SYSCFG_WKUP_STAT);
    if (wkup_stat)
    {
        REG_FIELD_WR(SYSCFG->PMU_WKUP, SYSCFG_LP_WKUP_CLR,1);
        reset_retain_ptr->wakeup_source = wkup_stat;
    }
}

uint8_t get_reset_source()
{
    uint8_t rst_stat = SYSCFG->RSTST;
    SYSCFG->RSTST = 0xff;
    return rst_stat;
}

uint8_t get_wakeup_source()
{
    struct reset_retain_struct *reset_retain_ptr = (struct reset_retain_struct*)RESET_RETAIN_BASE;
    return reset_retain_ptr->wakeup_source;
}

void arm_cm_set_int_isr(uint8_t type,void (*isr)())
{
    ISR_VECTOR_ADDR[type + 16] = (uint32_t)isr;
}

void cpu_recover_asm(void);

void cpu_sleep_recover_init()
{
    ISR_VECTOR_ADDR[1] = (uint32_t)cpu_recover_asm;
    arm_cm_set_int_isr(LPWKUP_IRQn,LPWKUP_Handler);
}

static void module_init()
{
    check_wkup_state();
    io_init();
    LOG_INIT();
    LOG_I("sys init");
    INIT_BUILTIN_TIMER_ENV();
    lsecc_init();
    lstrng_init();
    lscrypt_init();
    srand(lstrng_random());
    calc_acc_init();
    cpu_sleep_recover_init();
    uint32_t base_offset = flash_data_storage_base_offset();
    tinyfs_init(base_offset);
    tinyfs_print_dir_tree();
    mac_init();
    rco_calib_mode_set(0);
    rco_calibration_start();
    modem_rf_init();
    low_power_mode_init();
    irq_init();
    systick_start();
}

static void analog_init()
{
    dcdc_on();
    if(clk_check()==false)
    {
        clk_switch();
    }

    REG_FIELD_WR(SYSCFG->ANACFG1, SYSCFG_OSCRC_DIG_PWR_EN,0);
    //REG_FIELD_WR(SYSCFG->ANACFG1, SYSCFG_ADC12B_DIG_PWR_EN, 0);
    REG_FIELD_WR(SYSCFG->PMU_TRIM, SYSCFG_XTAL_STBTIME, XTAL_STB_VAL);

}

static void var_init()
{
    stack_data_bss_init();
    bb_mem_clr();
    stack_var_ptr_init();
    spi_flash_drv_var_init(true,false);
}

void sys_init_itf()
{
    analog_init();
    var_init();
    main_task_itf_init();
    module_init();
}

void sys_init_app()
{
    analog_init();
    var_init();
    main_task_app_init();
    module_init();
}

void ll_stack_var_ptr_init(void);

static void ll_var_init()
{
    stack_data_bss_init();
    bb_mem_clr();
    ll_stack_var_ptr_init();
    spi_flash_drv_var_init(true,false);
}

void sys_init_ll()
{
    analog_init();
    ll_var_init();
    io_init();
    lsecc_init();
    calc_acc_init();
    cpu_sleep_recover_init();
    mac_init();
    rco_calib_mode_set(0);
    rco_calibration_start();
    modem_rf_init();
    low_power_mode_init();
    irq_init();
    systick_start();
}

void platform_reset(uint32_t error)
{
    __disable_irq();
    struct reset_retain_struct *reset_retain_ptr = (struct reset_retain_struct*)RESET_RETAIN_BASE;
    reset_retain_ptr->reset_reason = error;
    switch_to_hse();
    __NVIC_SystemReset();
}

uint64_t idiv_acc(uint32_t dividend,uint32_t divisor,bool signed_int)
{
    uint64_t retval;
    struct {
        uint32_t r0;
        uint32_t r1;
    }*ret_ptr = (void *)&retval;
    enter_critical();
    calc_div(dividend, divisor, signed_int,&ret_ptr->r0,&ret_ptr->r1);
    exit_critical();
    return retval;
}

__attribute__((weak)) void SystemInit(){}

uint32_t plf_get_reset_error()
{
    struct reset_retain_struct *reset_retain_ptr = (struct reset_retain_struct*)RESET_RETAIN_BASE;
    return reset_retain_ptr->reset_reason;
}

XIP_BANNED void flash_prog_erase_suspend_delay()
{
    DELAY_US(30);
}

__attribute__((weak)) void ble_isr(){}

XIP_BANNED void BLE_Handler()
{
    bool flash_writing_status = spi_flash_writing_busy();
    bool xip = spi_flash_xip_status_get();
    if(flash_writing_status)
    {
        LS_RAM_ASSERT(xip == false);
        spi_flash_prog_erase_suspend();
        flash_prog_erase_suspend_delay();
    }
    if(xip == false)
    {
        spi_flash_xip_start();
    }
    ble_isr();
    if(xip == false)
    {
        spi_flash_xip_stop();
    }
    if(flash_writing_status)
    {
        spi_flash_prog_erase_resume();
    }
}

XIP_BANNED void switch_to_rc32k()
{
    MODIFY_REG(RCC->CFG, RCC_SYSCLK_SW_MASK| RCC_HCLK_SCAL_MASK| RCC_CKCFG_MASK, (uint32_t)2<<RCC_SYSCLK_SW_POS | (uint32_t)1<<RCC_CKCFG_POS);
}

XIP_BANNED void switch_to_xo16m()
{
    MODIFY_REG(RCC->CFG, RCC_HCLK_SCAL_MASK | RCC_CKCFG_MASK | RCC_SYSCLK_SW_MASK, (uint32_t)1<<RCC_CKCFG_POS | (uint32_t)1<<RCC_SYSCLK_SW_POS);
}

XIP_BANNED void switch_to_hse(void)
{
    MODIFY_REG(RCC->CFG, RCC_SYSCLK_SW_MASK| RCC_HCLK_SCAL_MASK| RCC_CKCFG_MASK, (uint32_t)1 <<RCC_SYSCLK_SW_POS | (uint32_t)1<<RCC_CKCFG_POS | (uint32_t)0 << RCC_HCLK_SCAL_POS);
}

#if (SDK_HCLK_MHZ==16)
XIP_BANNED bool clk_check()
{
    uint32_t rcc_cfg = RCC->CFG;
    return REG_FIELD_RD(rcc_cfg, RCC_SYSCLK_SW) == 1 && REG_FIELD_RD(rcc_cfg, RCC_HCLK_SCAL) == 0;
}

XIP_BANNED void clk_switch()
{
    switch_to_xo16m();
}

#else
XIP_BANNED static void switch_to_pll(uint8_t hclk_scal)
{
    MODIFY_REG(RCC->CFG, RCC_SYSCLK_SW_MASK| RCC_HCLK_SCAL_MASK| RCC_CKCFG_MASK, (uint32_t)4 <<RCC_SYSCLK_SW_POS | (uint32_t)1<<RCC_CKCFG_POS | (uint32_t)hclk_scal << RCC_HCLK_SCAL_POS);
}

#if (SDK_HCLK_MHZ==32)

XIP_BANNED bool clk_check()
{
    uint32_t rcc_cfg = RCC->CFG;
    return REG_FIELD_RD(rcc_cfg, RCC_SYSCLK_SW) == 4 && REG_FIELD_RD(rcc_cfg, RCC_HCLK_SCAL) == 0x8;
}

XIP_BANNED void clk_switch()
{
    switch_to_pll(0x8);
}
#elif(SDK_HCLK_MHZ==64)

XIP_BANNED bool clk_check()
{
    uint32_t rcc_cfg = RCC->CFG;
    return REG_FIELD_RD(rcc_cfg, RCC_SYSCLK_SW) == 4 && REG_FIELD_RD(rcc_cfg, RCC_HCLK_SCAL) == 0;
}

XIP_BANNED void clk_switch()
{
    switch_to_pll(0);
}
#else
#error HCLK not supported
#endif

#endif

uint32_t get_ota_info_offset()
{
    uint8_t manufacturer_id;
    uint8_t mem_type_id;
    uint8_t capacity_id;
    spi_flash_read_id(&manufacturer_id,&mem_type_id,&capacity_id);
    uint32_t flash_size = 1<<capacity_id;
    return flash_size - FLASH_PAGE_SIZE;
}

void ota_settings_erase(void)
{
    SYSCFG->BKD[7] = 0;
    spi_flash_sector_erase(get_ota_info_offset());
}

void ota_settings_write(uint32_t ota_settings_type)
{
    LS_ASSERT(ota_settings_type < OTA_SETTINGS_TYPE_MAX); 
    spi_flash_quad_page_program(get_ota_info_offset(),(uint8_t *)&ota_settings_type,sizeof(ota_settings_type));
}

uint32_t ota_settings_read(void)
{
    uint32_t ota_settings;
    spi_flash_quad_io_read(get_ota_info_offset(),(uint8_t *)&ota_settings,sizeof(ota_settings));
    return ota_settings;
}

// only for foreground OTA
void request_ota_reboot()
{
    SYSCFG->BKD[7] = 0x5A5A3C3C;
    platform_reset(RESET_OTA_REQ);
}

uint32_t get_ota_status_offset()
{
    return get_ota_info_offset() + 0x10;    
}

bool ota_copy_info_get(struct fota_image_info *ptr)
{
    spi_flash_quad_io_read(get_ota_status_offset(),(uint8_t *)ptr, sizeof(struct fota_image_info));
    if(ptr->base==0xffffffff && ptr->size ==0xffffffff)
    {
        return false;
    }else
    {
        return true;
    }
}

void ota_copy_info_set(struct fota_image_info *ptr)
{
    spi_flash_quad_page_program(get_ota_status_offset(), (uint8_t *)ptr, sizeof(struct fota_image_info));
}

uint32_t get_app_image_base()
{
    return config_word_get(APP_IMAGE_BASE_OFFSET);
}

uint32_t get_fota_image_base()
{
    return config_word_get(FOTA_IMAGE_BASE_OFFSET);
}
