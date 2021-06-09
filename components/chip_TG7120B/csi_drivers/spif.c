/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     spiflash_w25q64fv.c
 * @brief    CSI Source File for Embedded Flash Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <string.h>
#include <drv/spiflash.h>
#include "spif.h"
#include "rom_sym_def.h"
#include "bus_dev.h"
#include "error.h"

#define ATTRIBUTE_DATA

#define ERR_SPIFLASH(errno) (CSI_DRV_ERRNO_SPIFLASH_BASE | errno)
#define SPIFLASH_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_SPIFLASH(DRV_ERROR_PARAMETER))

static uint8_t spiflash_init_flag = 0;

typedef struct {
    spiflash_info_t spiflashinfo;
    spiflash_event_cb_t cb;
    spiflash_status_t status;
} ck_spiflash_priv_t;

static ck_spiflash_priv_t s_spiflash_handle[CONFIG_SPIFLASH_NUM];

static const spiflash_capabilities_t driver_capabilities = {
    1, /* event_ready */
    2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

extern int spif_erase_all(void);
extern int spif_erase_block64(unsigned int addr);
extern int spif_erase_sector(unsigned int addr);
extern int spif_write(uint32_t addr, uint8_t *data, uint32_t size);
extern void spif_set_deep_sleep(void);
extern void spif_release_deep_sleep(void);
extern void spif_cmd(uint8_t op, uint8_t addrlen, uint8_t rdlen, uint8_t wrlen, uint8_t mbit, uint8_t dummy);
extern void spif_rddata(uint8_t *data, uint8_t len);

#define SPIF_TIMEOUT       1000000

#define SFLG_WIP    1
#define SFLG_WEL    2
#define SFLG_WELWIP 3

#define SPIF_WAIT_IDLE_CYC                          (32)

#define SPIF_STATUS_WAIT_IDLE(n)                    \
    do                                              \
    {                                               \
        while((AP_SPIF->fcmd &0x02)==0x02);         \
        {                                           \
            volatile int delay_cycle = n;           \
            while (delay_cycle--){;}                \
        }                                           \
        while ((AP_SPIF->config & 0x80000000) == 0);\
    } while (0);

//static void SPIF_STATUS_WAIT_IDLE(uint8_t n)
//{
//    do
//    {
//        while((AP_SPIF->fcmd &0x02)==0x02);
//        {
//            volatile int delay_cycle = n;
//            while (delay_cycle--){;}
//        }
//        while ((AP_SPIF->config & 0x80000000) == 0);
//    } while (0);
//}


__attribute__((section(".__sram.code")))  static inline uint32_t spif_lock()
{
    _HAL_CS_ALLOC_();
    HAL_ENTER_CRITICAL_SECTION();
    uint32_t vic_iser = VIC->ISER[0];
    //mask all irq
    VIC->ICER[0] = 0xFFFFFFFF;
    //enable ll irq and tim1 irq
    VIC->ISER[0] = 0x100010;
    HAL_EXIT_CRITICAL_SECTION();
    return vic_iser;
}

__attribute__((section(".__sram.code")))  static inline void spif_unlock(uint32_t vic_iser)
{
    _HAL_CS_ALLOC_();
    HAL_ENTER_CRITICAL_SECTION();
    VIC->ISER[0] = vic_iser;
    HAL_EXIT_CRITICAL_SECTION();
}

#define HAL_CACHE_ENTER_BYPASS_SECTION()  do{ \
        _HAL_CS_ALLOC_();\
        HAL_ENTER_CRITICAL_SECTION();\
        AP_CACHE->CTRL0 = 0x02; \
        AP_PCR->CACHE_RST = 0X02; \
        AP_PCR->CACHE_BYPASS = 1;    \
        HAL_EXIT_CRITICAL_SECTION();\
    }while(0);


#define HAL_CACHE_EXIT_BYPASS_SECTION()  do{ \
        _HAL_CS_ALLOC_();\
        HAL_ENTER_CRITICAL_SECTION();\
        AP_CACHE->CTRL0 = 0x00;\
        AP_PCR->CACHE_RST = 0X03; \
        AP_PCR->CACHE_BYPASS = 0;\
        HAL_EXIT_CRITICAL_SECTION();\
    }while(0);

#define spif_wait_nobusy(flg, tout_ns, return_val)   {if(_spif_wait_nobusy_x(flg, tout_ns)){if(return_val){ return return_val;}}}

__attribute__((section(".__sram.code")))  static void hal_cache_tag_flush(void)
{
    _HAL_CS_ALLOC_();
    HAL_ENTER_CRITICAL_SECTION();
    uint32_t cb = AP_PCR->CACHE_BYPASS;
    volatile int dly = 8;

    if (cb == 0) {
        AP_CACHE->CTRL0 = 0x02;
        AP_PCR->CACHE_BYPASS = 1;
    }

    while (dly--) {
        ;
    };

    AP_CACHE->CTRL0 = 0x03;

    dly = 8;

    while (dly--) {
        ;
    };

    if (cb == 0) {
        AP_PCR->CACHE_BYPASS = 0;
        AP_CACHE->CTRL0 = 0;
    }

    HAL_EXIT_CRITICAL_SECTION();
}

__attribute__((section(".__sram.code")))  static uint8_t _spif_read_status_reg_x(void)
{
    uint8_t status;
    spif_cmd(FCMD_RDST, 0, 2, 0, 0, 0);
    SPIF_STATUS_WAIT_IDLE(SPIF_WAIT_IDLE_CYC);
    spif_rddata(&status, 1);
    return status;
}

__attribute__((section(".__sram.code")))  static int _spif_wait_nobusy_x(uint8_t flg, uint32_t tout_ns)
{
    uint8_t status;
    volatile int tout = (int)(tout_ns);

    for (; tout ; tout --) {
        status = _spif_read_status_reg_x();

        if ((status & flg) == 0) {
            return PPlus_SUCCESS;
        }
    }

    return PPlus_ERR_BUSY;
}

__attribute__((section(".__sram.code")))  static int hal_flash_write(uint32_t addr, uint8_t *data, uint32_t size)
{
    uint8_t retval;
    uint32_t cs = spif_lock();
    HAL_CACHE_ENTER_BYPASS_SECTION();
    SPIF_STATUS_WAIT_IDLE(SPIF_WAIT_IDLE_CYC);
    spif_wait_nobusy(SFLG_WIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);

    retval = spif_write(addr, data, size);
    SPIF_STATUS_WAIT_IDLE(SPIF_WAIT_IDLE_CYC);
    spif_wait_nobusy(SFLG_WIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);
    HAL_CACHE_EXIT_BYPASS_SECTION();
    spif_unlock(cs);
    return retval;
}

__attribute__((section(".__sram.code")))  static int hal_flash_read(uint32_t addr, uint8_t *data, uint32_t size)
{
    uint32_t cs = spif_lock();
    volatile uint8_t *u8_spif_addr = (volatile uint8_t *)((addr & 0x7ffff) | SPIF_ADDR_START);
    uint32_t cb = AP_PCR->CACHE_BYPASS;
//#if(SPIF_FLASH_SZIE == FLASH_SIZE_1MB)
#if 0
    uint32_t remap = addr & 0xf80000;

    if (remap) {
        AP_SPIF->remap = remap;
        AP_SPIF->config |= 0x10000;
    }

#endif

    //read flash addr direct access
    //bypass cache
    if (cb == 0) {
        HAL_CACHE_ENTER_BYPASS_SECTION();
    }

    for (int i = 0; i < size; i++) {
        data[i] = u8_spif_addr[i];
    }

    //bypass cache
    if (cb == 0) {
        HAL_CACHE_EXIT_BYPASS_SECTION();
    }


    //#if(SPIF_FLASH_SZIE == FLASH_SIZE_1MB)
#if 0

    if (remap) {
        AP_SPIF->remap = 0;
        AP_SPIF->config &= ~0x10000ul;
    }

#endif

    spif_unlock(cs);

    return PPlus_SUCCESS;
}

__attribute__((section(".__sram.code")))  int hal_flash_erase_sector(unsigned int addr)
{
    uint8_t retval;
    uint32_t cs = spif_lock();

    uint32_t cb = AP_PCR->CACHE_BYPASS;

    SPIF_STATUS_WAIT_IDLE(SPIF_WAIT_IDLE_CYC);
    spif_wait_nobusy(SFLG_WIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);

    retval = spif_erase_sector(addr);
    spif_wait_nobusy(SFLG_WELWIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);

    if (cb == 0) {
        hal_cache_tag_flush();
    }

    spif_unlock(cs);

    return retval;
}

__attribute__((section(".__sram.code")))  int hal_flash_erase_block64(unsigned int addr)
{
    uint32_t cs = spif_lock();
    uint8_t retval;
    uint32_t cb = AP_PCR->CACHE_BYPASS;
    SPIF_STATUS_WAIT_IDLE(SPIF_WAIT_IDLE_CYC);
    spif_wait_nobusy(SFLG_WIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);
    retval = spif_erase_block64(addr);
    spif_wait_nobusy(SFLG_WELWIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);

    if (cb == 0) {
        hal_cache_tag_flush();
    }

    spif_unlock(cs);

    return retval;
}

__attribute__((section(".__sram.code")))  int hal_flash_erase_all(void)
{
    uint32_t cs = spif_lock();
    uint8_t retval;
    uint32_t cb = AP_PCR->CACHE_BYPASS;
    SPIF_STATUS_WAIT_IDLE(SPIF_WAIT_IDLE_CYC);
    spif_wait_nobusy(SFLG_WIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);
    retval = spif_erase_all();
    spif_wait_nobusy(SFLG_WELWIP, SPIF_TIMEOUT, PPlus_ERR_BUSY);

    if (cb == 0) {
        hal_cache_tag_flush();
    }

    spif_unlock(cs);

    return retval;
}

/*
basic spif driver
*/

// erase total chip of flash
uint32_t phy_flash_chip_erase(void)
{
    return hal_flash_erase_all();
}

// write a Word to flash
__attribute__((section(".__sram.code")))  uint32_t phy_WriteFlash(uint32_t offset, uint32_t  value)
{
    uint32_t temp = value;
    offset &= 0x00ffffff;

    return hal_flash_write(offset, (uint8_t *)&temp, 4);

}

// program 64 bytes into flash
__attribute__((section(".__sram.code")))  int phy_ProgramPage64(unsigned long offset, const unsigned char *buf,  int size)    // size must be <=256
{
    offset &= 0x00ffffff;

    return hal_flash_write(offset, (uint8_t *)buf, size);
}

// set flash to deep sleep mode
void phy_set_flash_deep_sleep(void)
{
    spif_set_deep_sleep();
}

// wake up flash from sleep state
void phy_release_flash_deep_sleep(void)
{
    spif_release_deep_sleep();
}

// read one byte from flash
__attribute__((section(".__sram.code")))  uint32_t phy_ReadFlash(unsigned int addr, uint32_t *value)
{
    uint32_t temp;
    uint32_t ret;
    addr &= 0x00ffffff;

    ret = hal_flash_read(addr, (uint8_t *)&temp, 4);
    *value = temp;

    return ret;
}

// erase a sector of flash, size is 4KB
uint32_t phy_flash_sector_erase(unsigned int addr)
{
    return hal_flash_erase_sector(addr);
}

// erase a block whose size is 64KB
uint32_t phy_flash_block64_erase(unsigned int addr)
{
    return hal_flash_erase_block64(addr);
}

/*
cpi api
*/
int ATTRIBUTE_DATA spiflash_read_status_register(uint8_t *data)
{
    *data = _spif_read_status_reg_x();
    return 0;
}

/**
  \brief       Flash power down.
  \param[in]   handle  spiflash handle to operate.
  \return      error code
*/
int32_t csi_spiflash_power_down(spiflash_handle_t handle)
{
    return ERR_SPIFLASH(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Flash release power down.
  \param[in]   handle  spiflash handle to operate.
  \return      error code
*/
int32_t csi_spiflash_release_power_down(spiflash_handle_t handle)
{
    return ERR_SPIFLASH(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   spiflash handle to operate.
  \return      \ref spiflash_capabilities_t
*/
spiflash_capabilities_t csi_spiflash_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM) {
        spiflash_capabilities_t ret;
        memset(&ret, 0, sizeof(spiflash_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       Initialize SPIFLASH Interface. 1. Initializes the resources needed for the SPIFLASH interface 2.registers event callback function
  \param[in]   handle  spiflash handle to operate.
  \param[in]   cb_event  Pointer to \ref spiflash_event_cb_t
  \return      \ref execution_status
*/
spiflash_handle_t csi_spiflash_initialize(int32_t idx, spiflash_event_cb_t cb_event)
{
    ck_spiflash_priv_t *spiflash_priv = &s_spiflash_handle[idx];

    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM) {
        return NULL;
    }

    spiflash_priv->spiflashinfo.start = SPIF_ADDR_START;
    spiflash_priv->spiflashinfo.end = SPIF_ADDR_END;
    spiflash_priv->spiflashinfo.sector_count = SPIF_SECTOR_COUNT;
    spiflash_priv->spiflashinfo.sector_size = SPIF_SECTOR_SIZE;
    spiflash_priv->spiflashinfo.page_size = SPIF_PAGE_SIZE;
    spiflash_priv->spiflashinfo.program_unit = 1;
    spiflash_priv->spiflashinfo.erased_value = 0xff;

    spiflash_priv->cb = cb_event;

    spiflash_priv->status.busy = 0;
    spiflash_priv->status.error = 0U;

    spiflash_init_flag = 1;

    return (spiflash_handle_t)spiflash_priv;
}

/**
  \brief       De-initialize SPIFLASH Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spiflash handle to operate.
  \return      \ref execution_status
*/
int32_t csi_spiflash_uninitialize(spiflash_handle_t handle)
{

#if(DEBUG_EN == 1)
    SPIFLASH_NULL_PARAM_CHK(handle);
#endif

    ck_spiflash_priv_t *spiflash_priv = (ck_spiflash_priv_t *)handle;
    spiflash_init_flag = 0;
    spiflash_priv->cb = NULL;

    return 0;
}

/**
  \brief       Read data from Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or \ref execution_status
*/
int32_t csi_spiflash_read(spiflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    int i;
    uint32_t read_data = 0;
    uint32_t *p_rd = data;
    uint32_t len_rd = 0;
    ck_spiflash_priv_t *spiflash_priv = handle;

#if(DEBUG_EN == 1)
    SPIFLASH_NULL_PARAM_CHK(handle);
    SPIFLASH_NULL_PARAM_CHK(data);
    SPIFLASH_NULL_PARAM_CHK(cnt);

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr || spiflash_priv->spiflashinfo.start > (addr + cnt - 1) || spiflash_priv->spiflashinfo.end < (addr + cnt - 1)) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

#endif
    addr &= 0xffffff;
    spiflash_priv->status.error = 0U;

    len_rd  = cnt >> 2;

    if ((((uint32_t)data >> 2) << 2) != (uint32_t)data) {
        p_rd = &read_data;

        for (i = 0; i < len_rd; i++) {
            phy_ReadFlash(addr + (i << 2), (unsigned int *)(p_rd));
            memcpy((uint8_t *)data + (i << 2), p_rd, 4);
        }
    } else {
        for (i = 0; i < len_rd; i++) {
            phy_ReadFlash(addr + (i << 2), (unsigned int *)(p_rd + i));
        }
    }

    len_rd  = cnt - ((cnt >> 2) << 2);

    if (len_rd > 0) {
        p_rd = &read_data;
        phy_ReadFlash(addr + (i << 2), (unsigned int *)(p_rd));
        memcpy((uint8_t *)data + (i << 2), p_rd, len_rd);
    }

    return cnt;
}

//#include "log.h"
/**
  \brief       Program data to Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or \ref execution_status
*/
int32_t csi_spiflash_program(spiflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{

    ck_spiflash_priv_t *spiflash_priv = handle;

#if (DEBUG_EN == 1)
    SPIFLASH_NULL_PARAM_CHK(handle);
    SPIFLASH_NULL_PARAM_CHK(data);
    SPIFLASH_NULL_PARAM_CHK(cnt);

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr || spiflash_priv->spiflashinfo.start > (addr + cnt - 1) || spiflash_priv->spiflashinfo.end < (addr + cnt - 1)) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

#endif

    uint32_t len = 0;
    uint32_t num = cnt;
    uint8_t *p_wr;
    addr &= 0xffffff;

    spiflash_priv->status.busy = 1U;
    spiflash_priv->status.error = 0U;

    p_wr = (uint8_t *)data;
    len = 64 - (addr % 64);

    if (len >= cnt) {
        phy_ProgramPage64(addr, p_wr, cnt);
    } else {
        phy_ProgramPage64(addr, p_wr, len);

        addr += len;
        p_wr += len;
        cnt -= len;

        if (cnt <= 64) {
            phy_ProgramPage64(addr, p_wr, cnt);
        } else {
            while (cnt >= 64) {
                phy_ProgramPage64(addr, p_wr, 64);
                addr += 64;
                p_wr += 64;
                cnt -= 64;
            }

            if (cnt > 0) {
                phy_ProgramPage64(addr, p_wr, cnt);
            }
        }
    }

    spiflash_priv->status.busy = 0U;

    return num;
}

/**
  \brief       Erase Flash Sector.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Sector address
  \return      \ref execution_status
*/
int32_t csi_spiflash_erase_sector(spiflash_handle_t handle, uint32_t addr)
{
    ck_spiflash_priv_t *spiflash_priv = handle;

#if (DEBUG_EN == 1)

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end <= addr) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    SPIFLASH_NULL_PARAM_CHK(handle);

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

#endif

    spiflash_priv->status.busy = 1U;
    spiflash_priv->status.error = 0U;
    addr &= 0xffffff;

    phy_flash_sector_erase(addr);

    spiflash_priv->status.busy = 0U;

    if (spiflash_priv->cb) {
        spiflash_priv->cb(0, SPIFLASH_EVENT_READY);
    }

    return 0;
}

/**
  \brief       Erase complete Flash.
  \param[in]   handle  spiflash handle to operate.
  \return      \ref execution_status
*/
int32_t csi_spiflash_erase_chip(spiflash_handle_t handle)
{
    ck_spiflash_priv_t *spiflash_priv = handle;
#if (DEBUG_EN == 1)
    SPIFLASH_NULL_PARAM_CHK(handle);

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

#endif
    spiflash_priv->status.busy = 1U;

    phy_flash_chip_erase();

    spiflash_priv->status.busy = 0U;

    if (spiflash_priv->cb) {
        spiflash_priv->cb(0, SPIFLASH_EVENT_READY);
    }

    return 0;
}

/**
  \brief       Get Flash information.
  \param[in]   handle  spiflash handle to operate.
  \return      Pointer to Flash information \ref spiflash_info_t
*/
spiflash_info_t *csi_spiflash_get_info(spiflash_handle_t handle)//todo
{
    if (handle == NULL || (!spiflash_init_flag)) {
        return NULL;
    }

    ck_spiflash_priv_t *spiflash_priv = handle;
    spiflash_info_t *spiflash_info = &(spiflash_priv->spiflashinfo);

    return spiflash_info;
}

/**
  \brief       Get SPIFLASH status.
  \param[in]   handle  spiflash handle to operate.
  \return      SPIFLASH status \ref spiflash_status_t
*/
spiflash_status_t csi_spiflash_get_status(spiflash_handle_t handle)
{
    if (handle == NULL || (!spiflash_init_flag)) {
        spiflash_status_t ret;
        memset(&ret, 0, sizeof(spiflash_status_t));
        return ret;
    }

    ck_spiflash_priv_t *spiflash_priv = handle;

    uint8_t status = 0;
    spiflash_read_status_register(&status);
    spiflash_priv->status.busy = status;

    return spiflash_priv->status;
}

int32_t csi_spiflash_power_control(spiflash_handle_t handle, /*csi_power_stat_e*/ uint8_t state)
{
    switch (state) {
        case 0:
            phy_set_flash_deep_sleep();// set flash to deep sleep mode
            break;

        case 1:
            phy_release_flash_deep_sleep();// wake up flash from sleep state
            break;

        default:
            break;
    }

    return state;
}

