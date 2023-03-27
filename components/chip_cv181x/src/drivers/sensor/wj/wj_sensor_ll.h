/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_sensor_ll.h.h
 * @brief    header file for sensor driver
 * @version  V1.0
 * @date     15. June 2020
 ******************************************************************************/

#ifndef _WJ_SENSOR_LL_H_
#define _WJ_SENSOR_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/************************************VD  Sensor*******************************/
/* VD_CTRL, offset: 0x00 */
#define WJ_VD_CTRL_VHS_Pos                              (0U)
#define WJ_VD_CTRL_VHS_Msk                              (0x3U << WJ_VD_CTRL_VHS_Pos)

#define WJ_VD_CTRL_VLS_Pos                              (2U)
#define WJ_VD_CTRL_VLS_Msk                              (0x3U << WJ_VD_CTRL_VLS_Pos)

#define WJ_VD_CTRL_VDS_Pos                              (4U)
#define WJ_VD_CTRL_VDS_Msk                              (0xFU << WJ_VD_CTRL_VDS_Pos)

#define WJ_VD_CTRL_VD_TRIM_Pos                          (8U)
#define WJ_VD_CTRL_VD_TRIM_Msk                          (0x1FU << WJ_VD_CTRL_VD_TRIM_Pos)

#define WJ_VD_CTRL_INT_H_Pos                            (16U)
#define WJ_VD_CTRL_INT_H_Msk                            (0x1U << WJ_VD_CTRL_INT_H_Pos)
#define WJ_VD_CTRL_INT_H_EN                             WJ_VD_CTRL_INT_H_Msk

#define WJ_VD_CTRL_INT_L_Pos                            (17U)
#define WJ_VD_CTRL_INT_L_Msk                            (0x1U << WJ_VD_CTRL_INT_L_Pos)
#define WJ_VD_CTRL_INT_L_EN                             WJ_VD_CTRL_INT_L_Msk

#define WJ_VD_CTRL_INT_CLR_H_Pos                        (18U)
#define WJ_VD_CTRL_INT_CLR_H_Msk                        (0x1U << WJ_VD_CTRL_INT_CLR_H_Pos)
#define WJ_VD_CTRL_INT_CLR_H                            WJ_VD_CTRL_INT_CLR_H_Msk

#define WJ_VD_CTRL_INT_CLR_L_Pos                        (19U)
#define WJ_VD_CTRL_INT_CLR_L_Msk                        (0x1U << WJ_VD_CTRL_INT_CLR_L_Pos)
#define WJ_VD_CTRL_INT_CLR_L                            WJ_VD_CTRL_INT_CLR_L_Msk

#define WJ_VD_CTRL_ERROR_Pos                            (20U)
#define WJ_VD_CTRL_ERROR_Msk                            (0x1U << WJ_VD_CTRL_ERROR_Pos)
#define WJ_VD_CTRL_ERROR_EN                             WJ_VD_CTRL_ERROR_Msk

#define WJ_VD_CTRL_CLR_ERROR_Pos                        (21U)
#define WJ_VD_CTRL_CLR_ERROR_Msk                        (0x1U << WJ_VD_CTRL_CLR_ERROR_Pos)
#define WJ_VD_CTRL_CLR_ERROR                            WJ_VD_CTRL_CLR_ERROR_Msk

#define WJ_VD_CTRL_RESET_Pos                            (22U)
#define WJ_VD_CTRL_RESET_Msk                            (0x1U << WJ_VD_CTRL_RESET_Pos)
#define WJ_VD_CTRL_RESET                                WJ_VD_CTRL_RESET_Msk

/* VD_START, offset: 0x04 */
#define WJ_VD_START_ANA_Pos                             (0U)
#define WJ_VD_START_ANA_Msk                             (0x1U << WJ_VD_START_ANA_Pos)
#define WJ_VD_START_ANA_EN                              WJ_VD_START_ANA_Msk

#define WJ_VD_START_Pos                                 (1U)
#define WJ_VD_START_Msk                                 (0x1U << WJ_VD_START_Pos)
#define WJ_VD_START_EN                                  WJ_VD_START_Msk

/* VD_STATUS, offset: 0x08 */
#define WJ_VD_STATUS_V_RET_Pos                          (0U)
#define WJ_VD_STATUS_V_RET_Msk                          (0x1U << WJ_VD_STATUS_V_RET_Pos)
#define WJ_VD_STATUS_V_RET                              WJ_VD_STATUS_V_RET_Msk

#define WJ_VD_STATUS_VL_Pos                             (1U)
#define WJ_VD_STATUS_VL_Msk                             (0x1U << WJ_VD_STATUS_VL_Pos)
#define WJ_VD_STATUS_VL                                 WJ_VD_STATUS_VL_Msk

#define WJ_VD_STATUS_VH_Pos                             (2U)
#define WJ_VD_STATUS_VH_Msk                             (0x1U << WJ_VD_STATUS_VH_Pos)
#define WJ_VD_STATUS_VH                                 WJ_VD_STATUS_VH_Msk

#define WJ_VD_STATUS_INT_H_Pos                          (4U)
#define WJ_VD_STATUS_INT_H_Msk                          (0x1U << WJ_VD_STATUS_INT_H_Pos)
#define WJ_VD_STATUS_INT_H                               WJ_VD_STATUS_INT_H_Msk

#define WJ_VD_STATUS_INT_L_Pos                          (5U)
#define WJ_VD_STATUS_INT_L_Msk                          (0x1U << WJ_VD_STATUS_INT_L_Pos)
#define WJ_VD_STATUS_INT_L                              WJ_VD_STATUS_INT_L_Msk

#define WJ_VD_STATUS_INT_ERROR_Pos                      (6U)
#define WJ_VD_STATUS_INT_ERROR_Msk                      (0x1U << WJ_VD_STATUS_INT_ERROR_Pos)
#define WJ_VD_STATUS_INT_ERROR                          WJ_VD_STATUS_INT_ERROR_Msk

/************************************TD  Sensor*******************************/
/* TD_CTRL, offset: 0x00 */
#define WJ_TD_CTRL_BG_TC_Pos                            (0U)
#define WJ_TD_CTRL_BG_TC_Msk                            (0xFU << WJ_TD_CTRL_BG_TC_Pos)

#define WJ_TD_CTRL_TH_TRIM_Pos                          (4U)
#define WJ_TD_CTRL_TH_TRIM_Msk                          (0xFU << WJ_TD_CTRL_TH_TRIM_Pos)

#define WJ_TD_CTRL_ADJUST_Pos                           (8U)
#define WJ_TD_CTRL_ADJUST_Msk                           (0xFU << WJ_TD_CTRL_ADJUST_Pos)

#define WJ_TD_CTRL_TDLS_Pos                             (12U)
#define WJ_TD_CTRL_TDLS_Msk                             (0xFU << WJ_TD_CTRL_TDLS_Pos)

#define WJ_TD_CTRL_TDHS_Pos                             (16U)
#define WJ_TD_CTRL_TDHS_Msk                             (0x3FU << WJ_TD_CTRL_TDHS_Pos)

#define WJ_TD_CTRL_INT_H_Pos                            (24U)
#define WJ_TD_CTRL_INT_H_Msk                            (0x1U << WJ_TD_CTRL_INT_H_Pos)
#define WJ_TD_CTRL_INT_H_EN                             WJ_TD_CTRL_INT_H_Msk

#define WJ_TD_CTRL_INT_L_Pos                            (25U)
#define WJ_TD_CTRL_INT_L_Msk                            (0x1U << WJ_TD_CTRL_INT_L_Pos)
#define WJ_TD_CTRL_INT_L_EN                             WJ_TD_CTRL_INT_L_Msk

#define WJ_TD_CTRL_INT_CLR_H_Pos                        (26U)
#define WJ_TD_CTRL_INT_CLR_H_Msk                        (0x1U << WJ_TD_CTRL_INT_CLR_H_Pos)
#define WJ_TD_CTRL_INT_CLR_H                            WJ_TD_CTRL_INT_CLR_H_Msk

#define WJ_TD_CTRL_INT_CLR_L_Pos                        (27U)
#define WJ_TD_CTRL_INT_CLR_L_Msk                        (0x1U << WJ_TD_CTRL_INT_CLR_L_Pos)
#define WJ_TD_CTRL_INT_CLR_L                            WJ_TD_CTRL_INT_CLR_L_Msk

#define WJ_TD_CTRL_INT_ERROR_Pos                        (28U)
#define WJ_TD_CTRL_INT_ERROR_Msk                        (0x1U << WJ_TD_CTRL_INT_ERROR_Pos)
#define WJ_TD_CTRL_INT_ERROR_EN                         WJ_TD_CTRL_INT_ERROR_Msk

#define WJ_TD_CTRL_CLR_INT_ERROR_Pos                    (29U)
#define WJ_TD_CTRL_CLR_INT_ERROR_Msk                    (0x1U << WJ_TD_CTRL_CLR_INT_ERROR_Pos)
#define WJ_TD_CTRL_CLR_INT_ERROR                        WJ_TD_CTRL_CLR_INT_ERROR_Msk

#define WJ_TD_CTRL_RESET_Pos                            (30U)
#define WJ_TD_CTRL_RESET_Msk                            (0x1U << WJ_TD_CTRL_RESET_Pos)
#define WJ_TD_CTRL_RESET                                WJ_TD_CTRL_RESET_Msk

/* TD_START, offset: 0x04 */
#define WJ_TD_START_ANA_Pos                             (0U)
#define WJ_TD_START_ANA_Msk                             (0x1U << WJ_TD_START_ANA_Pos)
#define WJ_TD_START_ANA_EN                              WJ_TD_START_ANA_Msk

#define WJ_TD_START_Pos                                 (1U)
#define WJ_TD_START_Msk                                 (0x1U << WJ_TD_START_Pos)
#define WJ_TD_START_EN                                  WJ_TD_START_Msk

/* TD_STATUS, offset: 0x08 */
#define WJ_TD_STATUS_RESET_Pos                          (0U)
#define WJ_TD_STATUS_RESET_Msk                          (0x1U << WJ_TD_STATUS_RESET_Pos)
#define WJ_TD_STATUS_RESET                              WJ_TD_STATUS_RESET_Msk

#define WJ_TD_STATUS_LTD_Pos                            (1U)
#define WJ_TD_STATUS_LTD_Msk                            (0x1U << WJ_TD_STATUS_LTD_Pos)
#define WJ_TD_STATUS_LTD                                WJ_TD_STATUS_LTD_Msk

#define WJ_TD_STATUS_HTD_Pos                            (2U)
#define WJ_TD_STATUS_HTD_Msk                            (0x1U << WJ_TD_STATUS_HTD_Pos)
#define WJ_TD_STATUS_HTD                                WJ_TD_STATUS_HTD_Msk

#define WJ_TD_STATUS_INT_H_Pos                          (4U)
#define WJ_TD_STATUS_INT_H_Msk                          (0x1U << WJ_TD_STATUS_INT_H_Pos)
#define WJ_TD_STATUS_INT_H                              WJ_TD_STATUS_INT_H_Msk

#define WJ_TD_STATUS_INT_L_Pos                          (5U)
#define WJ_TD_STATUS_INT_L_Msk                          (0x1U << WJ_TD_STATUS_INT_L_Pos)
#define WJ_TD_STATUS_INT_L                              WJ_TD_STATUS_INT_L_Msk

#define WJ_TD_STATUS_INT_ERROR_Pos                      (6U)
#define WJ_TD_STATUS_INT_ERROR_Msk                      (0x1U << WJ_TD_STATUS_INT_ERROR_Pos)
#define WJ_TD_STATUS_INT_ERROR                          WJ_TD_STATUS_INT_ERROR_Msk

/************************************FD  Sensor*******************************/
/* FD_CTRL, offset: 0x00 */
#define WJ_FD_CTRL_FHS_Pos                              (0U)
#define WJ_FD_CTRL_FHS_Msk                              (0x3U << WJ_FD_CTRL_FHS_Pos)

#define WJ_FD_CTRL_FLS_Pos                              (2U)
#define WJ_FD_CTRL_FLS_Msk                              (0x3U << WJ_FD_CTRL_FLS_Pos)

#define WJ_FD_CTRL_RFDTM_Pos                            (4U)
#define WJ_FD_CTRL_RFDTM_Msk                            (0x3U << WJ_FD_CTRL_RFDTM_Pos)

#define WJ_FD_CTRL_TRI_Pos                              (6U)
#define WJ_FD_CTRL_TRI_Msk                              (0x1U << WJ_FD_CTRL_TRI_Pos)
#define WJ_FD_CTRL_TRI_En                               WJ_FD_CTRL_TRI_Msk

#define WJ_FD_CTRL_TM_Pos                               (8U)
#define WJ_FD_CTRL_TM_Msk                               (0x1FU << WJ_FD_CTRL_TM_Pos)

#define WJ_FD_CTRL_INT_H_Pos                            (16U)
#define WJ_FD_CTRL_INT_H_Msk                            (0x1U << WJ_FD_CTRL_INT_H_Pos)
#define WJ_FD_CTRL_INT_H_En                             WJ_FD_CTRL_INT_H_Msk

#define WJ_FD_CTRL_INT_L_Pos                            (17U)
#define WJ_FD_CTRL_INT_L_Msk                            (0x1U << WJ_FD_CTRL_INT_L_Pos)
#define WJ_FD_CTRL_INT_L_En                             WJ_FD_CTRL_INT_L_Msk

#define WJ_FD_CTRL_INT_CLR_H_Pos                        (18U)
#define WJ_FD_CTRL_INT_CLR_H_Msk                        (0x1U << WJ_FD_CTRL_INT_CLR_H_Pos)
#define WJ_FD_CTRL_INT_CLR_H                             WJ_FD_CTRL_INT_CLR_H_Msk

#define WJ_FD_CTRL_INT_CLR_L_Pos                        (19U)
#define WJ_FD_CTRL_INT_CLR_L_Msk                        (0x1U << WJ_FD_CTRL_INT_CLR_L_Pos)
#define WJ_FD_CTRL_INT_CLR_L                            WJ_FD_CTRL_INT_CLR_L_Msk

#define WJ_FD_CTRL_INT_ERROR_Pos                        (20U)
#define WJ_FD_CTRL_INT_ERROR_Msk                        (0x1U << WJ_FD_CTRL_INT_ERROR_Pos)
#define WJ_FD_CTRL_INT_ERROR_En                         WJ_FD_CTRL_INT_ERROR_Msk

#define WJ_FD_CTRL_CLR_INT_ERROR_Pos                    (21U)
#define WJ_FD_CTRL_CLR_INT_ERROR_Msk                    (0x1U << WJ_FD_CTRL_CLR_INT_ERROR_Pos)
#define WJ_FD_CTRL_CLRINT_ERROR_En                      WJ_FD_CTRL_CLR_INT_ERROR_Msk

#define WJ_FD_CTRL_RESET_Pos                            (22U)
#define WJ_FD_CTRL_RESET_Msk                            (0x1U << WJ_FD_CTRL_RESET_Pos)
#define WJ_FD_CTRL_RESETS                               WJ_FD_CTRL_RESET_Msk

/* FD_START, offset: 0x04 */
#define WJ_FD_START_ANA_Pos                             (0U)
#define WJ_FD_START_ANA_Msk                             (0x1U << WJ_FD_START_ANA_Pos)
#define WJ_FD_START_ANA_EN                              WJ_FD_START_ANA_Msk

#define WJ_FD_START_Pos                                 (1U)
#define WJ_FD_START_Msk                                 (0x1U << WJ_FD_START_Pos)
#define WJ_FD_START_EN                                  WJ_FD_START_Msk

/* FD_STATUS, offset: 0x08 */
#define WJ_FD_STATUS_RESET_Pos                          (0U)
#define WJ_FD_STATUS_RESET_Msk                          (0x1U << WJ_FD_STATUS_RESET_Pos)
#define WJ_FD_STATUS_RESET                              WJ_FD_STATUS_RESET_Msk

#define WJ_FD_STATUS_LFD_Pos                            (1U)
#define WJ_FD_STATUS_LFD_Msk                            (0x1U << WJ_FD_STATUS_LFD_Pos)
#define WJ_FD_STATUS_LFD                                WJ_FD_STATUS_LFD_Msk

#define WJ_FD_STATUS_HFD_Pos                            (2U)
#define WJ_FD_STATUS_HFD_Msk                            (0x1U << WJ_FD_STATUS_HFD_Pos)
#define WJ_FD_STATUS_HFD                                WJ_FD_STATUS_HFD_Msk

#define WJ_FD_STATUS_INT_H_Pos                          (4U)
#define WJ_FD_STATUS_INT_H_Msk                          (0x1U << WJ_FD_STATUS_INT_H_Pos)
#define WJ_FD_STATUS_INT_H                              WJ_FD_STATUS_INT_H_Msk

#define WJ_FD_STATUS_INT_L_Pos                          (5U)
#define WJ_FD_STATUS_INT_L_Msk                          (0x1U << WJ_FD_STATUS_INT_L_Pos)
#define WJ_FD_STATUS_INT_L                              WJ_FD_STATUS_INT_L_Msk

#define WJ_FD_STATUS_INT_ERROR_Pos                      (6U)
#define WJ_FD_STATUS_INT_ERROR_Msk                      (0x1U << WJ_FD_STATUS_INT_ERROR_Pos)
#define WJ_FD_STATUS_INT_ERROR                          WJ_FD_STATUS_INT_ERROR_Msk



typedef struct {
    __IOM uint32_t  CTRL;                 /* Offset: 0x000 (W/R)  sensor control register */
    __IOM uint32_t  START;                /* Offset: 0x004 (W/R)  sensor start register   */
    __IOM uint32_t  STATUS;               /* Offset: 0x008 (W/R)  sensor start register   */
} wj_sensor_regs_t;

/************************************VD  Sensor*******************************/
static inline void wj_sensor_vd_set_vds(wj_sensor_regs_t *sensor, uint8_t vds)
{
    sensor->CTRL &= ~WJ_VD_CTRL_VDS_Msk;
    sensor->CTRL |= vds << WJ_VD_CTRL_VDS_Pos;
}

static inline void wj_sensor_vd_set_vhs(wj_sensor_regs_t *sensor, uint8_t vhs)
{
    sensor->CTRL &= ~WJ_VD_CTRL_VHS_Msk;
    sensor->CTRL |= vhs << WJ_VD_CTRL_VHS_Pos;
}

static inline void wj_sensor_vd_set_vls(wj_sensor_regs_t *sensor, uint8_t vls)
{
    sensor->CTRL &= ~WJ_VD_CTRL_VLS_Msk;
    sensor->CTRL |= vls << WJ_VD_CTRL_VLS_Pos;
}

static inline void wj_sensor_vd_set_vtm(wj_sensor_regs_t *sensor, uint8_t vtm)
{
    sensor->CTRL &= ~WJ_VD_CTRL_VD_TRIM_Msk;
    sensor->CTRL |= vtm << WJ_VD_CTRL_VD_TRIM_Pos;
}


static inline void wj_sensor_vd_enble_vh(wj_sensor_regs_t *sensor)
{
    sensor->CTRL |= WJ_VD_CTRL_INT_H_EN;
}

static inline void wj_sensor_vd_enble_vl(wj_sensor_regs_t *sensor)
{
    sensor->CTRL |= WJ_VD_CTRL_INT_L_EN;
}

static inline void wj_sensor_vd_enble_ana(wj_sensor_regs_t *sensor)
{
    sensor->START |= WJ_VD_START_ANA_EN;
}

static inline void wj_sensor_vd_enble(wj_sensor_regs_t *sensor)
{
    sensor->START |= WJ_VD_START_EN;
}


static inline int32_t wj_sensor_vd_get_vh_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_VD_STATUS_VH_Msk) >> WJ_VD_STATUS_VH_Pos;
}

static inline int32_t wj_sensor_vd_get_vl_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_VD_STATUS_VL_Msk) >> WJ_VD_STATUS_VL_Pos;
}

static inline int32_t wj_sensor_vd_get_vrst_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_VD_STATUS_V_RET_Msk) >> WJ_VD_STATUS_V_RET_Pos;
}

/************************************FD  Sensor*******************************/
static inline void wj_sensor_fd_set_rfdtm(wj_sensor_regs_t *sensor, uint8_t rfdtm)
{
    sensor->CTRL &= ~WJ_FD_CTRL_RFDTM_Msk;
    sensor->CTRL |= rfdtm << WJ_FD_CTRL_RFDTM_Pos;
}

static inline void wj_sensor_fd_set_enble_tri(wj_sensor_regs_t *sensor)
{
    sensor->CTRL |= WJ_FD_CTRL_TRI_En;
}

static inline void wj_sensor_fd_set_disenble_tri(wj_sensor_regs_t *sensor)
{
    sensor->CTRL &= ~WJ_FD_CTRL_TRI_Msk;
}

static inline void wj_sensor_fd_set_fhs(wj_sensor_regs_t *sensor, uint8_t fhs)
{
    sensor->CTRL &= ~WJ_FD_CTRL_FHS_Msk;
    sensor->CTRL |= fhs << WJ_FD_CTRL_FHS_Pos;
}

static inline void wj_sensor_fd_set_fls(wj_sensor_regs_t *sensor, uint8_t fhs)
{
    sensor->CTRL &= ~WJ_FD_CTRL_FLS_Msk;
    sensor->CTRL |= fhs << WJ_FD_CTRL_FLS_Pos;
}

static inline void wj_sensor_fd_enble_ana(wj_sensor_regs_t *sensor)
{
    sensor->START |= WJ_FD_START_ANA_EN;
}

static inline void wj_sensor_fd_enble(wj_sensor_regs_t *sensor)
{
    sensor->START |= WJ_FD_START_EN;
}

static inline uint32_t wj_sensor_fd_get_htd_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_FD_STATUS_HFD_Msk) >> WJ_FD_STATUS_HFD_Pos;
}

static inline uint32_t wj_sensor_fd_get_ltd_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_FD_STATUS_LFD_Msk) >> WJ_FD_STATUS_LFD_Pos;
}

static inline uint32_t wj_sensor_fd_get_reset_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_FD_STATUS_RESET_Msk) >> WJ_FD_STATUS_RESET_Pos;
}
/************************************TD  Sensor*******************************/
static inline void wj_sensor_td_set_bgtc(wj_sensor_regs_t *sensor, uint8_t bgtc)
{
    sensor->CTRL &= ~WJ_TD_CTRL_BG_TC_Msk;
    sensor->CTRL |= bgtc << WJ_TD_CTRL_BG_TC_Pos;
}

static inline void wj_sensor_td_set_trim(wj_sensor_regs_t *sensor, uint8_t trim)
{
    sensor->CTRL &= ~WJ_TD_CTRL_TH_TRIM_Msk;
    sensor->CTRL |= trim << WJ_TD_CTRL_TH_TRIM_Pos;
}

static inline void wj_sensor_td_set_adjst(wj_sensor_regs_t *sensor, uint8_t adjst)
{
    sensor->CTRL &= ~WJ_TD_CTRL_ADJUST_Msk;
    sensor->CTRL |= adjst << WJ_TD_CTRL_ADJUST_Pos;
}

static inline void wj_sensor_td_set_hs(wj_sensor_regs_t *sensor, uint8_t hs)
{
    sensor->CTRL &= ~WJ_TD_CTRL_TDHS_Msk;
    sensor->CTRL |= hs << WJ_TD_CTRL_TDHS_Pos;
}

static inline void wj_sensor_td_set_ls(wj_sensor_regs_t *sensor, uint8_t ls)
{
    sensor->CTRL &= ~WJ_TD_CTRL_TDLS_Msk;
    sensor->CTRL |= ls << WJ_TD_CTRL_TDLS_Pos;
}

static inline void wj_sensor_td_clrl_h_irq(wj_sensor_regs_t *sensor)
{
    sensor->CTRL &= ~WJ_TD_CTRL_INT_CLR_H;
}

static inline void wj_sensor_td_clrl_l_irq(wj_sensor_regs_t *sensor)
{
    sensor->CTRL &= ~WJ_TD_CTRL_INT_CLR_L;
}

static inline void wj_sensor_td_clrl_error_irq(wj_sensor_regs_t *sensor)
{
    sensor->CTRL &= ~WJ_TD_CTRL_CLR_INT_ERROR;
}

static inline void wj_sensor_td_enble_vh(wj_sensor_regs_t *sensor)
{
    sensor->CTRL |= WJ_TD_CTRL_INT_H_EN;
}

static inline void wj_sensor_td_enble_vl(wj_sensor_regs_t *sensor)
{
    sensor->CTRL |= WJ_TD_CTRL_INT_L_EN;
}

static inline void wj_sensor_td_enble_error(wj_sensor_regs_t *sensor)
{
    sensor->CTRL |= WJ_TD_CTRL_INT_ERROR_EN;
}

static inline void wj_sensor_td_reset(wj_sensor_regs_t *sensor)
{
    sensor->CTRL |= WJ_TD_CTRL_RESET;
}

static inline void wj_sensor_td_enble_ana(wj_sensor_regs_t *sensor)
{
    sensor->START |= WJ_TD_START_ANA_EN;
}

static inline void wj_sensor_td_enble(wj_sensor_regs_t *sensor)
{
    sensor->START |= WJ_TD_START_EN;
}

static inline uint32_t wj_sensor_td_get_htd_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_TD_STATUS_HTD_Msk) >> WJ_TD_STATUS_HTD_Pos;
}

static inline uint32_t wj_sensor_td_get_ltd_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_TD_STATUS_LTD_Msk) >> WJ_TD_STATUS_LTD_Pos;
}

static inline uint32_t wj_sensor_td_get_trst_status(wj_sensor_regs_t *sensor)
{
    return (sensor->STATUS & WJ_TD_STATUS_RESET_Msk) >> WJ_TD_STATUS_RESET_Pos;
}
#endif  /* _WJ_SENSOR_LL_H_*/
