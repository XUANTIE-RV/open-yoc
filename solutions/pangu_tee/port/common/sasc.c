/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <tee_addr_map.h>
#include <sasc.h>

#if (CONFIG_SECURE_SPACE > 0)
#if ((CONFIG_PLATFORM_PHOBOS > 0) || (CONFIG_PLATFORM_HOBBIT1_2 > 0) || (CONFIG_PLATFORM_HOBBIT3 > 0))

static inline void eflash_region_security_config(uint32_t attr)
{
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CAR) = attr;
}

static inline void eflash_region_security_active(uint32_t act)
{
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CR) = act;
}

static inline int eflash_region_address_config(uint8_t region_idx, uint32_t addr, uint32_t size)
{
#if ((CONFIG_PLATFORM_PHOBOS > 0) || (CONFIG_PLATFORM_HOBBIT1_2 > 0))
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_RG0 + region_idx * 4) = (size | ((addr >> 9) << 23));
#elif (CONFIG_PLATFORM_HOBBIT3 > 0)
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_RG0 + region_idx * 4) = (size | (addr >> 9) << 8);
#endif
    return 0;
}

static inline void eflash_regin_cd_config(uint32_t cd)
{
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG0) = cd << 16;
}

static inline void eflash_regin_ap_config(uint32_t uap, uint32_t sap)
{
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG0) = uap;
    *(volatile uint32_t *)(EFLASH_REG_BASE + EFLASH_CFG1) = sap;
}

static inline void sram_region_security_config(uint32_t attr)
{
    *(volatile uint32_t *)(SRAM_REG_BASE + SRAM_CAR) = attr;
}

static inline void sram_region_security_active(uint32_t act)
{
    *(volatile uint32_t *)(SRAM_REG_BASE + SRAM_CR) = act;
}

static inline int sram_region_address_config(uint8_t region_idx, uint32_t addr, uint32_t size)
{
#if ((CONFIG_PLATFORM_PHOBOS > 0) || (CONFIG_PLATFORM_HOBBIT1_2 > 0))
    *(volatile uint32_t *)(SRAM_REG_BASE + SRAM_RG0 + region_idx * 4) =  addr << 21 | size;
#elif (CONFIG_PLATFORM_HOBBIT3 > 0)
    *(volatile uint32_t *)(SRAM_REG_BASE + SRAM_RG0 + region_idx * 4) =  addr << 8 | size;
#endif

    return 0;
}

static inline void sram_regin_cd_config(uint32_t cd)
{
    *(volatile uint32_t *)(SRAM_REG_BASE + SRAM_CFG0) = cd << 16;
}

static inline void sram_regin_ap_config(uint32_t uap, uint32_t sap)
{
    *(volatile uint32_t *)(SRAM_REG_BASE + SRAM_CFG0) |= uap;
    *(volatile uint32_t *)(SRAM_REG_BASE + SRAM_CFG1) = sap;
}

void sram_secure_config(void)
{
    sram_region_security_config(SRAM_REG_CAR_CAR0);
    // TW_RW_SIZE = 4K
    sram_region_address_config(0, TW_RW_ADDR - SRAM_ADDR_START, SRAM_REG_RG_4KB);
    sram_regin_cd_config(CD_DN);
    sram_regin_ap_config(AP_DN, AP_DN);
    sram_region_security_active(SRAM_REG_CA_CA0);
}

void eflash_secure_config(void)
{
    eflash_region_security_config(EFLASH_REG_CAR_CAR0);
    // TW_RO_SIZE = 20K, RG0 = 16K, RG1 = 4K
    eflash_region_address_config(0, TW_RO_ADDR - EFLASH_ADDR_START, EFLASH_REG_RG_32S);
    eflash_region_address_config(1, TW_RO_ADDR - EFLASH_ADDR_START + 16 * 1024, EFLASH_REG_RG_4S);
    eflash_regin_cd_config(CD_DN);
    eflash_regin_ap_config(AP_DN, AP_DN);
    eflash_region_security_active(EFLASH_REG_CA_CA0);
}

#elif (CONFIG_PLATFORM_ZX297100 > 0)

#if 0
#define LOG(format, ...)    printf(format, ##__VA_ARGS__)
#define LOG_HEX(msg,data,len)                           \
    do {                                                \
        int hex_i;                                      \
        hex_i = (len);                              \
        printf("%s\n",(msg));                       \
        while(hex_i--)                                  \
        {                                               \
            printf("0x%02x ",(data)[(len) - hex_i - 1]);    \
            if (((len) - hex_i) % 16 == 0)          \
            {                                           \
                printf("\n");                           \
            }                                           \
        }                                               \
        printf("\n");                                   \
    } while(0)
#else
#define LOG(format, ...)
#define LOG_HEX(msg,data,len)
#endif

static struct asc_si_efc_dev asc_si_efc_data[MAX_EFLASHDEV_NUM] = {{NULL}, {NULL}};

/**
 *
 * set the attr of  security region setting registers of eflash 1/2
 *
 *
 * @param[in]   efc_id          eflash 1/2
 * @param[in]   sec_reg         sec region register type
 * @param[in]   is_security `   1:is secure, 0:is not sec   ure
 *
 * @return  0 on success; -1 on failure
 *
 */
static int _asc_efc_set_regs_secure(enum asc_si_efc_id efc_id,
                                    enum asc_si_efc_secregn_reg sec_reg, uint8_t is_security)
{
    volatile struct asc_si_efc_regs *efc_reg = NULL;

    if (efc_id >= MAX_EFLASHDEV_NUM) {
        LOG("set_reg_secure efc_id is invalid.\n");
        return -1;
    }

    if (sec_reg >= MAX_EFC_SECUREG) {
        LOG("set_reg_secure sec_reg is invalid.\n");
        return -1;
    }

    efc_reg =  asc_si_efc_data[efc_id].reg;

    if (is_security) {
        efc_reg->sec_flag |= (1 << sec_reg);
    } else {
        efc_reg->sec_flag &= ~(1 << sec_reg);
    }

    return 0;
}


/**
 *
 * set the eflash region area is secure or not
 *
 *
 * @param[in]   region_reg_base         the addr base of sec region regs
 * @param[in]   sec_reg                 sec region register type
 * @param[in]   is_securearea               1:is secure, 0: not secure
 *
 * @return  void
 *
 */
static void _asc_efc_set_region_secure(uint32_t region_reg_base,
                                       enum asc_si_efc_secregn_reg sec_reg,
                                       uint8_t is_securearea)
{
    if (is_securearea) {
        REG32(region_reg_base + sec_reg * 8) |= (1 << EFC_REGION_IS_SECURE);
    } else {
        REG32(region_reg_base + sec_reg * 8) &= ~(1 << EFC_REGION_IS_SECURE);
    }
}


/**
 *
 * set the eflash regions
 *
 *
 * @param[in]   region_reg_base         the addr base of sec region regs
 * @param[in]   sec_reg                 sec region register type
 * @param[in]   start_addr              the start addr of region
 * @param[in]   end_addr                    the end addr of region
 *
 * @return  0 on success; -1 on failure
 *
 */
int _asc_efc_set_regions(uint32_t region_reg_base, enum asc_si_efc_secregn_reg sec_reg,
                         uint32_t start_addr, uint32_t end_addr)
{
    uint32_t start_allign_addr;
    uint32_t end_allign_addr;
    uint32_t start_page_addr;
    uint32_t end_page_addr;

    if (region_reg_base == EFLASH1_REGION_BASE) {
        start_allign_addr = ROUNDDOWN(start_addr, EFC1_SEC_REGION_ALLIG_SIZE);
        end_allign_addr = ROUNDDOWN(end_addr, EFC1_SEC_REGION_ALLIG_SIZE);
    } else {
        start_allign_addr = ROUNDDOWN(start_addr, EFC2_SEC_REGION_ALLIG_SIZE);
        end_allign_addr = ROUNDDOWN(end_addr, EFC2_SEC_REGION_ALLIG_SIZE);
    }

    if ((start_addr != start_allign_addr) || (end_addr != end_allign_addr)
        || (start_allign_addr >= end_allign_addr)) {
        LOG("efc_set_sec_region: start_addr ,end_addr is invalid.\n");
        return -1;
    }

    start_page_addr = start_addr >> (REGION_ADDR_ALIG_SHIFT);
    end_page_addr = (end_addr - 1) >> (REGION_ADDR_ALIG_SHIFT);

    LOG("efc_set_sec_region: start_addr=%x ,end_addr=%x.\n", start_page_addr, end_page_addr);

    REG32(region_reg_base + sec_reg * 8) |= (start_page_addr << EFC_REGION_ADDR);
    REG32(region_reg_base + sec_reg * 8 + 4) |= (end_page_addr << EFC_REGION_ADDR);

    return 0;

}
/**
 *
 * set the eflash region nx type
 *
 *
 * @param[in]   region_reg_base         the addr base of sec region regs
 * @param[in]   sec_reg                 sec region register type
 * @param[in]   nx                      the nx attr
 *
 * @return  void
 *
 */
static void _asc_efc_set_region_nx(uint32_t region_reg_base,
                                   enum asc_si_efc_secregn_reg sec_reg, enum asc_si_efc_nx_attr nx)
{
    REG32(region_reg_base + sec_reg * 8) &= ~(3 << EFC_REGION_NX);
    REG32(region_reg_base + sec_reg * 8) |= (nx << EFC_REGION_NX);
}

/**
 *
 * set the eflash region area
 *
 *
 * @param[in]   region_reg_base         the addr base of sec region regs
 * @param[in]   sec_reg                 sec region register type
 * @param[in]   ap_prio                 ap attr
 *
 * @return  void
 *
 */
static void _asc_efc_set_region_ap_prio(uint32_t region_reg_base, enum asc_si_efc_secregn_reg sec_reg,
                                        enum asc_si_efc_ap_attr ap_prio)
{
    REG32(region_reg_base + sec_reg * 8) &= ~(3 << EFC_REGION_AP_ACCESS_PRI);
    REG32(region_reg_base + sec_reg * 8) |= (ap_prio << EFC_REGION_AP_ACCESS_PRI);
}

/**
 *
 * set the security registers setting work or not
 *
 *
 * @param[in]   region_reg_base         the addr base of sec region regs
 * @param[in]   sec_reg                 sec region register type
 * @param[in]   is_securework               1: the security work, 0:the security does not work
 *
 * @return  void
 *
 */
static void _asc_efc_region_secure_wrok(uint32_t region_reg_base,
                                        enum asc_si_efc_secregn_reg sec_reg, uint8_t is_securework)
{
    if (is_securework) {
        REG32(region_reg_base + sec_reg * 8) |= (1 << EFC_REGION_SECURE_EN);
    } else {
        REG32(region_reg_base + sec_reg * 8) &= ~(1 << EFC_REGION_SECURE_EN);
    }
}

/**
 *
 * lock the security registers of eflash1/2
 *
 *
 * @param[in]   region_reg_base         the addr base of sec region regs
 * @param[in]   sec_reg                 sec region register type
 *
 *
 * @return  void
 *
 */
static void _asc_efc_region_lock(uint32_t region_reg_base,
                                 enum asc_si_efc_secregn_reg sec_reg)
{
    REG32(region_reg_base + sec_reg * 8) |= (1 << EFC_REGION_LOCK);
}

/**
 *
 * eflash 1/2 mpu enable or disable
 *
 *
 * @param[in]   efc_id          eflash 1/2
 * @param[in]   is_enblempu     1:mpu enable,0:mpu disable
 *
 *
 * @return  0 on success; -1 on failure
 *
 */
static int _asc_efc_mpu_ctrl(enum asc_si_efc_id efc_id, uint8_t is_enblempu)
{
    uint32_t mpu_sec_addr;

    if (efc_id == EFLASH_DEV1) {
        mpu_sec_addr = EFLASH1_MPU_SEC_ENABLE_CFG;
    } else if (efc_id == EFLASH_DEV2) {
        mpu_sec_addr = EFLASH2_MPU_SEC_ENABLE_CFG;
    } else {
        LOG("efc_mpu_ctrl: efc_id  is invalid.\n");
        return -1;
    }

    if (is_enblempu) {
        REG32(mpu_sec_addr) |= (1 << EFC_MPU_SEC_ENABLE); //soc_sys ,Eflash1/2 mpu_enable(thisi is about region)
    } else {
        REG32(mpu_sec_addr) &= ~(1 << EFC_MPU_SEC_ENABLE); //soc_sys ,Eflash1/2 mpu_enable(thisi is about region)
    }

    return 0;
}

/**
 *
 * find idle region setting register
 *
 *
 * @param[in]   efc_id                  eflash1/2
 * @param[in]   regn_idx                    the pointer to stored region setting regs type
 *
 *
 * @return  0 on success; -1 on failure
 *
 */
static int32_t _asc_efc_find_idle_region(enum asc_si_efc_id efc_id, enum asc_si_efc_secregn_reg *regn_idx)
{
    uint32_t region_reg_base;
    uint32_t i;
    unsigned char is_locked = 0;

    if (efc_id == EFLASH_DEV1) {
        region_reg_base = EFLASH1_REGION_BASE;
    } else if (efc_id == EFLASH_DEV2) {
        region_reg_base = EFLASH2_REGION_BASE;
    } else {
        LOG("_asc_efc_find_idle_region: efc_id is invalid.\n");
        return -1;
    }

    for (i = 1; i <= EFLASH_REGN_NUM; i++) {
        is_locked = (REG32(region_reg_base + i * 8) >> EFC_REGION_LOCK) & 0x1;

        if (is_locked == 1) {
            continue;
        }

        *regn_idx = i;
        return 0;
    }

    return -1;
}


/**
 *
 * set eflash secure area
 *
 *
 * @param[in]   efc_id                  eflash1/2
 * @param[in]   regn_idx                    the idle region setting regs type
 * @param[in]   start_addr              the start addr of region
 * @param[in]   end_addr                    the end addr of region
 * @param[in]   is_secure                   1:is secure, 0: not secure
 *
 *
 * @return  0 on success; -1 on failure
 *
 */
static int _asc_set_eflash_security(enum asc_si_efc_id efc_id, enum asc_si_efc_secregn_reg regn_idx,
                                    uint32_t start_addr, uint32_t end_addr, uint8_t is_secure)
{
    int ret = 0;
    uint32_t region_reg_base;

    if ((regn_idx > EFC_REGION4_REG) || (regn_idx < EFC_REGION0_REG)) {
        LOG("_asc_set_eflash_security: regn_idx  is invalid.\n");
        return -1;
    }

    if (efc_id == EFLASH_DEV1) {
        region_reg_base = EFLASH1_REGION_BASE;
    } else if (efc_id == EFLASH_DEV2) {
        region_reg_base = EFLASH2_REGION_BASE;
    } else {
        LOG("_asc_efc_set_region_ap_prio: efc_id is invalid.\n");
        return -1;
    }

    ret = _asc_efc_set_regions(region_reg_base, regn_idx, start_addr, end_addr);

    if (ret != 0) {
        LOG("_asc_efc_set_regions failed\n");
        return -1;
    }

    _asc_efc_set_region_secure(region_reg_base, regn_idx, is_secure);

    _asc_efc_set_region_nx(region_reg_base, regn_idx, ZTE_AND_EXE);

    _asc_efc_set_region_ap_prio(region_reg_base, regn_idx, RW_AND_RW);

    _asc_efc_region_secure_wrok(region_reg_base, regn_idx, 1);

    _asc_efc_region_lock(region_reg_base, regn_idx);

    return 0;
}

/**
 *
 * asc_efc_sec_init
 *
 *
 * @param[in]void
 *
 *
 * @return  void
 *
 */
static void _asc_efc_sec_init(void)
{
    uint32_t i;

    asc_si_efc_data[EFLASH_DEV1].reg = (volatile struct asc_si_efc_regs *)(EFLASH1_CTRL_BASE);
    asc_si_efc_data[EFLASH_DEV2].reg = (volatile struct asc_si_efc_regs *)(EFLASH2_CTRL_BASE);

    for (i = 0; i <= EFC_REGION4_REG; i++) {
        _asc_efc_set_regs_secure(EFLASH_DEV1, i, 1);
    }

    _asc_efc_set_regs_secure(EFLASH_DEV1, EFC_DEFUT_REG, 0);

    for (i = 0; i <= EFC_REGION4_REG; i++) {
        _asc_efc_set_regs_secure(EFLASH_DEV2, i, 1);
    }

    _asc_efc_set_regs_secure(EFLASH_DEV2, EFC_DEFUT_REG, 0);

    _asc_efc_set_region_secure(EFLASH1_REGION_BASE, EFC_REGION0_REG, 0);
    _asc_efc_set_region_secure(EFLASH2_REGION_BASE, EFC_REGION0_REG, 0);

    _asc_efc_set_region_nx(EFLASH1_REGION_BASE, EFC_REGION0_REG, ZTE_AND_EXE);
    _asc_efc_set_region_nx(EFLASH2_REGION_BASE, EFC_REGION0_REG, ZTE_AND_EXE);

    _asc_efc_set_region_ap_prio(EFLASH1_REGION_BASE, EFC_REGION0_REG, RW_AND_RW);
    _asc_efc_set_region_ap_prio(EFLASH2_REGION_BASE, EFC_REGION0_REG, RW_AND_RW);

    _asc_efc_mpu_ctrl(EFLASH_DEV1, 1);
    _asc_efc_mpu_ctrl(EFLASH_DEV2, 1);
}

/**
 *
 * set the sram sec region area
 *
 *
 * @param[in]   ap_ram                  the ap iram type
 * @param[in]   start_addr              the start addr of region
 * @param[in]   end_addr                    the end addr of region
 *
 * @return  0 on success; -1 on failure
 *
 */
static int _asc_ram_set_sec_region(enum asc_si_ap_ram ap_ram, uint32_t start_addr, uint32_t size)
{
    uint32_t sec_addr_align = 0;
    uint32_t ap_iram1_addr = 0;
    uint32_t ap_iram1_up_addr = 0;
    uint32_t matrix_iram_addr = 0;

    sec_addr_align = ROUNDDOWN(start_addr + size, IRAM_SEC_REGION_ALLIG_SIZE);

    if (sec_addr_align != (start_addr + size)) {
        LOG("ram_region_addr_set: start_addr+size is not align 1024\n");
        return -1;
    }

    matrix_iram_addr = CKTOMATRIX(start_addr + size);

    if (ap_ram == AP_IRAM1) {
        ap_iram1_addr = AP_IRAM1_ROSZ_CFG;
        ap_iram1_up_addr = AP_IRAM1_UP_ROSZ_CFG;
    } else if (ap_ram == AP_IRAM2) {
        ap_iram1_addr = AP_IRAM2_ROSZ_CFG;
        ap_iram1_up_addr = AP_IRAM2_UP_ROSZ_CFG;
    } else {
        LOG("ram_region_addr_set: ap_ram is invalid.\n");
        return -1;
    }

    REG32(ap_iram1_addr) |= (matrix_iram_addr >> REGION_ADDR_ALIG_SHIFT);
    REG32(ap_iram1_up_addr) |= 1;

    return 0;
}

/**
 *
 * set eflash/AP IRAM  secure area
 *
 * @param[in]   paddr                   the start addr of sec area
 * @param[in]size                       the size of sec area
 * @param[in]   perm                    the addr of region
 *
 * when set ap iram and eflash2 secure area, paddr and size need 1K aligned
 * when set eflash1 secure area, paddr and size need 2K aligned
 *
 * @return  0 on success; -1 on failure
 *
 */
static int32_t _asc_set_mem_perm(uint32_t paddr, uint32_t size, uint32_t is_sec)
{
    int32_t ret;
    enum asc_si_efc_secregn_reg  regn_idx;


    if (paddr >= SRAM1_START && paddr < SRAM1_END) {
        ret = _asc_ram_set_sec_region(AP_IRAM1, paddr, size);

        if (ret != 0) {
            LOG("set_sec ap_iram1 region failed\n");
            return -1;
        }
    } else if (paddr >= SRAM2_START && paddr < SRAM2_END) {
        ret = _asc_ram_set_sec_region(AP_IRAM2, paddr, size);

        if (ret != 0) {
            LOG("set ap_iram2 region failed\n");
            return -1;
        }
    } else if (paddr >= FLASH1_START && paddr < FLASH1_END) {
        ret = _asc_efc_find_idle_region(EFLASH_DEV1, &regn_idx);

        if (ret != 0) {
            LOG("can not find free eflash1 region\n");
            return -1;
        }

        LOG("_set eflash1 region: paddr = %x, endaddr =%x \n",  paddr,  paddr + size);

        ret = _asc_set_eflash_security(EFLASH_DEV1, regn_idx, paddr, paddr + size, is_sec);

        if (ret != 0) {
            LOG("_set eflash1 region failed\n");
            return -1;
        }

    } else if (paddr >= FLASH2_START && paddr < FLASH2_END) {
        ret = _asc_efc_find_idle_region(EFLASH_DEV2, &regn_idx);

        if (ret != 0) {
            LOG("can not find free eflash2 region\n");
            return -1;
        }

        ret = _asc_set_eflash_security(EFLASH_DEV2, regn_idx, paddr, paddr + size, is_sec);

        if (ret != 0) {
            LOG("_set eflash2 region failed\n");
            return -1;
        }
    } else {
        LOG("set_mem_perm: invalid paddr\n");
        return -1;
    }

    return 0;
}

/**
 *
 * set AP IRAM  secure area
 *
 * when set ap iram secure area, paddr and size need 1K aligned
 *
 * @return  null
 *
 */
void sram_secure_config(void)
{
    if (_asc_set_mem_perm(TW_RW_ADDR , TW_RW_SIZE , 1)  !=  0) {
        LOG("set_iram1 mem failed addr = 0x%x \n", TW_RW_ADDR);
    }

}
/**
 *
 * set eflash  secure area
 *
 * when set eflash2 secure area, paddr and size need 1K aligned
 * when set eflash1 secure area, paddr and size need 2K aligned
 *
 * @return  null
 *
 */
void eflash_secure_config(void)
{
    _asc_efc_sec_init();

    if (_asc_set_mem_perm(TW_RO_ADDR, TW_RO_SIZE , 1)  !=  0) {
        LOG("set_elfash1 mem failed addr = 0x%x \n", TW_RO_ADDR);
    }
}
#endif
#endif

