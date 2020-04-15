/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_pmu_tx216.h
 * @brief    head file for tx216 pmu
 * @version  V1.2
 * @date     15. April 2019
 ******************************************************************************/
#ifndef _CK_PMU_TX216_H_
#define _CK_PMU_TX216_H_

#include <drv/errno.h>

/// definition for pmu handle.
typedef void *pmu_handle_t;

typedef enum {
    PMU_ERROR_POWER_DOMAIN_OFF_FAILED = (DRV_ERROR_SPECIFIC + 1),
} dpu_errno_e;


#define PMU_CPUFREQ_SPEED_MAX IHS_VALUE

/*----- PMU Control Codes: CPU frequency, unit:Hz -----*/
typedef enum {
    PMU_CPUFREQ_STOP       = 0,                             ///< CPU Clock Gate
    PMU_CPUFREQ_MAX_1IN512 = (PMU_CPUFREQ_SPEED_MAX / 512), ///< 1/512 max speed
    PMU_CPUFREQ_MAX_1IN64  = (PMU_CPUFREQ_SPEED_MAX / 64),  ///< 1/64 max speed
    PMU_CPUFREQ_MAX_1IN8   = (PMU_CPUFREQ_SPEED_MAX / 8),   ///< 1/8 max speed
    PMU_CPUFREQ_MAX        = (PMU_CPUFREQ_SPEED_MAX),       ///< max speed
} pmu_cpufreq_e;

/*----- PMU Control Codes: Wakeup polarity -----*/
typedef enum {
    PMU_WAKEUP_POL_LOW      = 0,       ///< Low or negedge
    PMU_WAKEUP_POL_HIGH                ///< High or posedge
} pmu_wakeup_pol_e;

typedef enum {
    WAKEUP_SRC_GPIO_PAD0,
    WAKEUP_SRC_GPIO_PAD1,
    WAKEUP_SRC_GPIO_PAD2,
    WAKEUP_SRC_GPIO_PAD3,
    WAKEUP_SRC_GPIO_PAD4,
    WAKEUP_SRC_GPIO_PAD5,
    WAKEUP_SRC_GPIO_PAD6,
    WAKEUP_SRC_GPIO_PAD7,

    WAKEUP_SRC_GPIO_COUNT
} wakeup_src_gpio_e;

typedef enum {
    WAKEUP_SRC_IRQ_RTC = 0,

    WAKEUP_SRC_IRQ_COUNT
} wakeup_src_irq_e;

typedef enum {
    WAKEUP_SRC_TYPE_GPIO,
    WAKEUP_SRC_TYPE_IRQ
} wakeup_src_type_e;

typedef struct {
    uint8_t enable;
    wakeup_src_gpio_e num;
    pmu_wakeup_pol_e  pol;
    uint32_t          level_cnt; /* Be careful, this is global config for all gpio */
} wakeup_src_cfg_gpio_t;

typedef struct {
    uint8_t enable;
    wakeup_src_irq_e num;
} wakeup_src_cfg_irq_t;

typedef struct {
    wakeup_src_type_e src_type;
    union {
        wakeup_src_cfg_gpio_t gpio_cfg;
        wakeup_src_cfg_irq_t  irq_cfg;
    };
} wakeup_src_cfg_t;

/****** PMU Event *****/
typedef enum {
    PMU_EVENT_STOP_MODE_PREPARE,
    PMU_EVENT_STOP_MODE_DONE,
    PMU_EVENT_RETENTION_MODE_PREPARE,
    PMU_EVENT_EXT_MODE_PREPARE,

    PMU_EVENT_CPUFREQ_CHANGE_PREPARE,
    PMU_EVENT_CPUFREQ_CHANGE_DONE,

    PMU_EVENT_POWER_DOMAIN_OFF_PREPARE,
    PMU_EVENT_POWER_DOMAIN_OFF_DONE,
    PMU_EVENT_POWER_DOMAIN_ON_PREPARE,
    PMU_EVENT_POWER_DOMAIN_ON_DONE,
} pmu_event_e;

/****** SoC power domain *****/
typedef enum  {
    PMU_PD_CV_SYS = 0,
    PMU_PD_BT_SYS,
    PMU_PD_VI_SYS,
    PMU_PD_ENC_SYS,
    PMU_PD_DDR_SYS,
    PMU_PD_AP_SYS,

    PMU_PD_COUNT
} pmu_pd_e;

/***** AP_SYS mode *****/
typedef enum {
    PMU_LPM_MODE_STOP,
    PMU_LPM_MODE_RETENTION,
    PMU_LPM_MODE_EXT
} pmu_lpm_mode_e;

/***** AP_SYS power on dsram size in retention mode *****/
typedef enum {
    PMU_DSRAM_SIZE_4K  = 0,
    PMU_DSRAM_SIZE_8K  = 1,
    PMU_DSRAM_SIZE_16K = 2
} pmu_dsram_size_e;

/***** PMU mode configuration *****/
typedef struct {
} pmu_lpm_stop_mode_cfg_t;

typedef struct {
    pmu_dsram_size_e dsram_size_in_retition;
} pmu_lpm_retention_mode_cfg_t;

typedef struct {
    pmu_lpm_mode_e lpm_mode;
    union {
        pmu_lpm_stop_mode_cfg_t      stop_mode_cfg;
        pmu_lpm_retention_mode_cfg_t retention_mode_cfg;
    };
} pmu_lpm_mode_cfg_t;

///< Pointer to \ref pmu_event_cb_t : PMU Event call back.
typedef void (*pmu_event_cb_t)(int32_t idx, pmu_event_e event, void *data);

/**
  \brief       Initialize PMU Interface. 1. Initializes the resources needed for the PMU interface 2.registers event callback function
  \param[in]   idx the id of the pmu
  \return      return pmu handle if success
*/
pmu_handle_t drv_pmu_initialize(int32_t idx, pmu_event_cb_t cb_events);

/**
  \brief       De-initialize PMU Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  pmu handle to operate.
  \return      error code
*/
int32_t drv_pmu_uninitialize(pmu_handle_t handle);

/**
  \brief       Get the cpu frequency
  \param[in]   handle  pmu handle to operate.
  \param[out]  cpufreq    \ref pmu_cpufreq_e
  \return      error code
*/
int32_t drv_pmu_get_cpufreq(pmu_handle_t handle, uint32_t *cpufreq);

/**
  \brief       Set the cpu frequency
  \param[in]   handle  pmu handle to operate.
  \param[in]   cpufreq    \ref pmu_cpufreq_e
  \return      error code
*/
int32_t drv_pmu_set_cpufreq(pmu_handle_t handle, pmu_cpufreq_e cpufreq);

/**
  \brief       Config the wakeup source.
  \param[in]   handle  pmu handle to operate
  \param[in]   cfg     wakeup source configuration
  \return      error code
*/
int32_t drv_pmu_config_wakeup_source(pmu_handle_t handle, wakeup_src_cfg_t *cfg);

/**
  \brief       Config the wakeup source.
  \param[in]   handle  pmu handle to operate
  \param[in]   wakeup_num wakeup source num
  \param[in]   type    \ref pmu_wakeup_type
  \param[in]   pol     \ref pmu_wakeup_pol
  \param[in]   enable  flag control the wakeup source is enable or not
  \return      error code
*/
int32_t drv_pmu_power_domain_control(pmu_handle_t handle, pmu_pd_e pd, uint8_t enable);

/**
  \brief       Config the wakeup source.
  \param[in]   handle  pmu handle to operate
  \param[in]   cfg     AP_SYS configuration
*/
int32_t drv_pmu_enter_lpm_mode(pmu_handle_t handle, pmu_lpm_mode_cfg_t *cfg);


/* PMU controller, defined refer to regcode_regdoc_gen_TX216_PMU.xlsm v0.2 (2019/3/11) */
typedef struct {
    __IOM uint32_t PMU_CFG;           /* offset: 0x00, refer to pmu_cfg_t */
    __IM  uint32_t RESERVED0[2];      /* offset: 0x04 */
    __IOM uint32_t INT_MASK_CFG;      /* offset: 0x0C, refer to pmu_int_mask_cfg_t */
    __IOM uint32_t INT_POLARITY_CFG;  /* offset: 0x10, refer to pmu_int_polarity_cfg_t */
    __IOM uint32_t INT_CFG;           /* offset: 0x14, refer to pmu_int_cfg_t */
    __IOM uint32_t INT_STS;           /* offset: 0x18, refer to pmu_int_sts_t */
    __IOM uint32_t INT_LEVEL_CNT_CFG; /* offset: 0x1C, refer to pmu_int_level_cnt_cfg_t */
    __IM  uint32_t PMU_STATUS_L;      /* offset: 0x20, refer to pmu_status_l_t */
    __IM  uint32_t PMU_STATUS_H;      /* offset: 0x24, refer to pmu_status_h_t */
    __IM  uint32_t RESERVED1;         /* offset: 0x28 */
    __IOM uint32_t WAIT_CNT;          /* offset: 0x2C, refer to pmu_wait_cnt_t */
    __IOM uint32_t PD_CPU_SW_CNT;     /* offset: 0x30, refer to pmu_pd_cpu_sw_cnt_t */
    __IOM uint32_t PD_CPU_INTV_CNT;   /* offset: 0x34, refer to pmu_pd_cpu_intv_cnt_t */
    __IM  uint32_t RESERVED2[2];      /* offset: 0x38 */
    __IOM uint32_t USER_DEFINED_REG0; /* offset: 0x40 */
    __IOM uint32_t USER_DEFINED_REG1; /* offset: 0x44 */
    __IOM uint32_t USER_DEFINED_REG2; /* offset: 0x48 */
    __IOM uint32_t USER_DEFINED_REG3; /* offset: 0x4C */
    /**/
    __IOM uint32_t PD_REQ;            /* offset: 0x50 */
} pmu_reg_t;

/* PMU_CFG, 0x00 */
typedef union {
    struct {
        uint32_t exit              : 1; /* WC [0] low power exit, auto cleared by hardware */
        uint32_t req               : 1; /* WC [1] low power request, auto cleared by hardware */
        uint32_t reserved0         : 1; /* -- [2] */
        uint32_t mode              : 2; /* RW [3:4] 0:stop; 1:retention; 2:ext */
        uint32_t reserved1         : 2; /* -- [5:6] */
        uint32_t bus_rst_en        : 1; /* RW [7] cpu bus reset enable */
        uint32_t ret_dsram_size_sel: 2; /* RW [8:9] retention dsram size select */
        uint32_t reserved2         : 3; /* -- [10:12] */
        uint32_t pd_sw_wait_cnt_en : 1; /* RW [13]  Power down/up ack wait count enable */
        uint32_t reserved3         : 1; /* -- [14] */
        uint32_t bus_clken         : 1; /* RW [15] CPU bus clock enable */
        uint32_t pd_flag           : 1; /* RW [16] when chip cold power on, the flag is reset to 0 */
        uint32_t reserved4        : 15; /* -- [17:31] */
    } b;
    uint32_t w;
} pmu_cfg_t;

/* INT_MASK_CFG, 0x0C */
typedef union {
    struct {
        uint32_t gpio_int_mask : 8; /* RW [0:7] bitmap mask for 8 pad gpio wakeup interrupt source */
        uint32_t intc_int_mask : 1; /* RW [8]   mask for intc wakeup interrupt source. 0:enable, 1:disable */
        uint32_t reserved1    : 23; /* -- [9:31] */
    } b;
    uint32_t w;
} pmu_int_mask_cfg_t;

/* INT_POLARITY_CFG, 0x10 */
typedef union {
    struct {
        uint32_t int_pol   : 3; /* RC [0] bitmap polarity for 8 pad gpio wakeup interrupt source */
        uint32_t reserved : 29; /* -- [1:31] */
    } b;
    uint32_t w;
} pmu_int_polarity_cfg_t;

/* INT_CFG, 0x14 */
typedef union {
    struct {
        uint32_t int_clr   : 3; /* RW [0:2] bitmap polarity for 8 pad gpio wakeup interrupt source. 1:active high */
        uint32_t reserved : 29; /* -- [3:31] */
    } b;
    uint32_t w;
} pmu_int_cfg_t;

/* INT_STS, 0x18 */
typedef union {
    struct {
        uint32_t gpio_int  : 3; /* RO [0:2] bitmap for 3 pad gpio wakeup interrupt status */
        uint32_t reserved0 : 5; /* -- [3:7] */
        uint32_t intc_int  : 1; /* RO [8]   intc wakeup interrupt status */
        uint32_t reserved1: 23; /* -- [9:31] */
    } b;
    uint32_t w;
} pmu_int_sts_t;

/* INT_LEVEL_CNT_CFG, 0x1C */
typedef union {
    struct {
        uint32_t level_cnt : 8; /* RW [0:7] external pad gpio interrupt pulse length count, used for filtering unwanted glitch */
        uint32_t reserved : 24; /* -- [8:31] */
    } b;
    uint32_t w;
} pmu_int_level_cnt_cfg_t;

/* PMU_STATUS_L, 0x20 */
typedef union {
    struct {
        uint32_t reserved0 : 4; /* -- [0:3] */
        uint32_t clkon     : 1; /* RO [4] cpu core clk on */
        uint32_t bus_clkon : 1; /* RO [5] cpu bus clk on */
        uint32_t reserved1 : 2; /* -- [6:7] */
        uint32_t fsm       : 4; /* RO [8:11] cpu pmu fsm state */
        uint32_t cnt       : 4; /* RO [12:27] cpu pmu internal counter */
        uint32_t reserved2 : 4; /* -- [28:31] */
    } b;
    uint32_t w;
} pmu_status_l_t;

/* PMU_STATUS_H, 0x24 */
typedef union {
    struct {
        uint32_t biu_pad_wakeup_b : 1; /* RO [0] cpu wake up when it is 0 */
        uint32_t reserved0        : 3; /* -- [1:3] */
        uint32_t biu_pad_lpmd_b   : 2; /* RO [4:5] cpu wake up when it is 0 */
        uint32_t reserved1       : 26; /* -- [6:31] */
    } b;
    uint32_t w;
} pmu_status_h_t;

/* WAIT_CNT, 0x2C */
typedef union {
    struct {
        uint32_t gate_cnt   : 8; /* RW [0:7]   clock gate wait counter */
        uint32_t reserved  : 16; /* -- [8:23] */
        uint32_t pd_rst_cnt : 8; /* RW [24:31] power down/up reset wait counter */
    } b;
    uint32_t w;
} pmu_wait_cnt_t;

/* PD_CPU_SW_CNT, 0x30 */
typedef union {
    struct {
        uint32_t pd_cpu_sw_cnt : 16; /* RW [0:15] cpu power down/up ack wait counter. used when PMU_CFG[13] is set */
        uint32_t reserved      : 16; /* -- [16:31] */
    } b;
    uint32_t w;
} pmu_pd_cpu_sw_cnt_t;

/* PD_CPU_INTV_CNT, 0x34 */
typedef union {
    struct {
        uint32_t iso_sw_wait_cnt : 16; /* RW [0:15] wait counter between isolation state and power up/down state */
        uint32_t iso_wait_cnt    : 16; /* RW [16:31] wait counter between isolation state and clock on state for power up sequencer. */
    } b;
    uint32_t w;
} pmu_pd_cpu_intv_cnt_t;

/* PD_REQ, 0x50 */
typedef union {
    struct {
        uint32_t pd_req: 9; /* bitmap block power down request, auto cleared by hardware. Used for hardware block pmu fsm */
    } b;
    uint32_t w;
} pmu_pd_req_t;

typedef struct {
    uint8_t idx;
    uint32_t base;
    uint32_t irq;
    pmu_event_cb_t cb;
} pmu_priv_t;

extern int32_t target_get_pmu(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

#endif /* _CK_PMU_TX216_H_ */
