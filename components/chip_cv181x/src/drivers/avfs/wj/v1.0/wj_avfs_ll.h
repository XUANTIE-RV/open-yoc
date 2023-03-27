/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_avfs_ll.h
 * @brief
 * @version
 * @date     2020-12-01
 ******************************************************************************/

#ifndef _WJ_AVFS_LL_H_
#define _WJ_AVFS_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************** misc    *******************************************************************/

/* offset 0x000 MAIN_CTRL */
#define WJ_AVFS_MISC_MAINCTRL_HW_POS                                   (0U)
#define WJ_AVFS_MISC_MAINCTRL_HW_MASK                                  (1U << WJ_AVFS_MISC_MAINCTRL_HW_POS)         
#define WJ_AVFS_MISC_MAINCTRL_HW_EN                                    (WJ_AVFS_MISC_MAINCTRL_HW_MASK)

#define WJ_AVFS_MISC_MAINCTRL_FREQ_CHG_POS                             (1U)
#define WJ_AVFS_MISC_MAINCTRL_FREQ_CHG_MASK                            (1U << WJ_AVFS_MISC_MAINCTRL_FREQ_CHG_POS)         
#define WJ_AVFS_MISC_MAINCTRL_FREQ_CHG                                 (WJ_AVFS_MISC_MAINCTRL_FREQ_CHG_MASK)

/* offset 0x004 AVF STATUS */
#define WJ_AVFS_MISC_STATUS_MAINSTATE_POS                              (0U)
#define WJ_AFVS_MISC_STATUS_MAINSTATE_MASK                             (0xFU << WJ_AVFS_MISC_STATUS_MAINSTATE_POS)
#define WJ_AVFS_MISC_STATUS_MAINSTATE_IDLE                             (0U)
#define WJ_AVFS_MISC_STATUS_MAINSTATE_FREQ_CHK                         (1U)
#define WJ_AVFS_MISC_STATUS_MAINSTATE_MEM                              (2U)
#define WJ_AVFS_MISC_STATUS_MAINSTATE_FREQ_UP                          (3U)
#define WJ_AVFS_MISC_STATUS_MAINSTATE_FREQ_DOWN                        (4U)

#define WJ_AVFS_MISC_STATUS_FREQ_DN_POS                                (4U)
#define WJ_AVFS_MISC_STATUS_FREQ_DN_MASK                               (0xFU << WJ_AVFS_MISC_STATUS_FREQ_DN_POS)
#define WJ_AVFS_MISC_STATUS_FREQ_DN_IDLE                               (0U)
#define WJ_AVFS_MISC_STATUS_FREQ_DN_FREQ                               (1U)
#define WJ_AVFS_MISC_STATUS_FREQ_DN_VOL                                (2U)
#define WJ_AVFS_MISC_STATUS_FREQ_DN_VSESNSOR                           (3U)
#define WJ_AVFS_MISC_STATUS_FREQ_DN_PSENSOR                            (4U)

#define WJ_AVFS_MISC_STATUS_FREQ_UP_POS                                (8U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_MASK                               (0xFU << WJ_AVFS_MISC_STATUS_FREQ_UP_POS)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_IDLE                               (0U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_FREQ_DOWN                          (1U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_VOL                                (2U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_VSESNSOR                           (3U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_RECOVER                            (4U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_PSENSOR                            (5U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_MEM                                (6U)
#define WJ_AVFS_MISC_STATUS_FREQ_UP_FREQ_UP                            (7U)
#define WJ_AVFS_MISC_STATE_BUSY_MASK                                   (1U << 16U)

/* offset 0x008~ 0x018 FREQ_LVL VAL GRP */
/* offset 0x01c default freq val        */
/* offset 0x020 WORK_MODE_CTRL          */
#define WJ_AVFS_MISC_WKCTRL_VS_BP_EN                                   (1U << 0)
#define WJ_AVFS_MISC_WKCTRL_VL_BP_EN                                   (1U << 1)
#define WJ_AVFS_MISC_WKCTRL_PS_BP_EN                                   (1U << 2)
#define WJ_AVFS_MISC_WKCTRL_TMR_EN                                     (1U << 3)
#define WJ_AVFS_MISC_WKCTRL_FREQUP_BPFREQ_DN_EN                        (1U << 4)
#define WJ_AVFS_MISC_WKCTRL_ACLK_BP_EN                                 (1U << 5) 

#define WJ_AVFS_MISC_WKCTRL_ITERATE_NUM_POS                            (16U)
#define WJ_AVFS_MISC_WKCTRL_ITERATE_NUM_MASK                           (8U << WJ_AVFS_MISC_WKCTRL_ITERATE_NUM_POS)

/* offset 0x024 TIMER VAL UNIT = CPU CYCLES */

typedef enum {
	TAR_FREQ_LVL0_VAL,
	TAR_FREQ_LVL1_VAL,
	TAR_FREQ_LVL2_VAL,
	TAR_FREQ_LVL3_VAL,
	TAR_FREQ_LVL4_VAL,
	TAR_FREQ_LVL5_VAL,
	TAR_FREQ_LVL6_VAL,
	TAR_FREQ_LVL7_VAL,
	TAR_FREQ_LVL8_VAL,
	TAR_FREQ_LVL_MAX
}avfs_tar_freq_lvl_t;

/*************************************************** voltage ctrl *************************************/

/* offset 0x100 voltage control */
#define WJ_AVFS_VOL_CFG_MARGIN_POS                                     (0U)
#define WJ_AVFS_VOL_CFG_MARGIN_MASK                                    (0xff << WJ_AVFS_VOL_CFG_MARGIN_POS)

#define WJ_AVFS_VOL_CFG_STEP_POS                                       (8U)
#define WJ_AVFS_VOL_CFG_STEP_MASK                                      (0xff << WJ_AVFS_VOL_CFG_STEP_POS)

#define WJ_AVFS_VOL_CFG_PMIC_DVS_RATE_POS                              (16U)
#define WJ_AVFS_VOL_CFG_PMIC_DVS_RATE_MASK                             (0xff << WJ_AVFS_VOL_CFG_PMIC_DVS_RATE_POS)

#define WJ_AVFS_VOL_CFG_RG_DUAL_RAIL_MEM_OFFSET_POS                    (24U)
#define WJ_AVFS_VOL_CFG_RG_DUAL_RAIL_MEM_OFFSET_MASK                   (0xFF << WJ_AVFS_VOL_CFG_RG_DUAL_RAIL_MEM_OFFSET_POS)

/* offset 0x104 voltage status  */
#define WJ_AVFS_VOL_STATUS_PMIC_READY                                  (1U << 0)
#define WJ_AVFS_VOL_STATUS_PMIC_SPI_IDLE                               (1U << 1)

#define WJ_AVFS_VOL_STATUS_CUR_MEM_VOL_VAL_POS                         (16U)
#define WJ_AVFS_VOL_STATUS_CUR_MEM_VOL_VAL_MASK                        (0xFF << WJ_AVFS_VOL_STATUS_CUR_MEM_VOL_VAL_POS)

#define WJ_AVFS_VOL_STATUS_CUR_VOL_VAL_POS                             (24U)
#define WJ_AVFS_VOL_STATUS_CUR_VOL_VAL_MASK                            (0xFF << WJ_AVFS_VOL_STATUS_CUR_VOL_VAL_POS)
/* offset 0x108 select interface of pmic ic */
#define WJ_AVFS_VOL_IF_SEL_MASK                                        (1U)

/* offset 0x10c i2c base_addr    */

/* offset 0x110 cmd group select enable */
#define WJ_AVFS_VOL_CMD_GRP_SEL_ENA_MASK                               (1U)

/* offset 0x114~0x138 cmd a group (cmd_a0 ~ cmd_a9) */
/* offset 0x13c~0x160 cmd b group (cmd_b0 ~ cmd_b9) */

#define WJ_AVFS_VOL_CMD_GRP_PARAM_DATA_POS                             (0U)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_DATA_MASK                            (0xffff << WJ_AVFS_VOL_CMD_GRP_PARAM_DATA_POS)

#define WJ_AVFS_VOL_CMD_GRP_PARAM_ADDR_POS                             (16U)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_ADDR_MASK                            (0xfff << WJ_AVFS_VOL_CMD_GRP_PARAM_ADDR_POS)

#define WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG1_VLD_POS                         (29U)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG1_VLD_MASK                        (1U << WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG1_VLD_POS)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG1_VLD                             WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG1_VLD_MASK

#define WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG0_VLD_POS                         (30U)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG0_VLD_MASK                        (1U << WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG0_VLD_POS)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG0_VLD                             WJ_AVFS_VOL_CMD_GRP_PARAM_VLTG0_VLD_MASK


#define WJ_AVFS_VOL_CMD_GRP_PARAM_CMD_VLD_POS                         (31U)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_CMD_VLD_MASK                        (1U << WJ_AVFS_VOL_CMD_GRP_PARAM_CMD_VLD_POS)
#define WJ_AVFS_VOL_CMD_GRP_PARAM_CMD_VLD                             WJ_AVFS_VOL_CMD_GRP_PARAM_CMD_VLD_MASK 

/* offset 0x164  wait time for pmic-vol-adjust completion */

/* offset 0x168  spi_cmd0 of pmic */
#define WJ_AVFS_VOL_SPI_CMD0_DATA_POS                                 (0U)
#define WJ_AVFS_VOL_SPI_CMD0_DATA_MASK                                (0xff << WJ_AVFS_VOL_SPI_CMD0_DATA_POS)
#define WJ_AVFS_VOL_SPI_CMD0_CMD_POS                                  (8U)
#define WJ_AVFS_VOL_SPI_CMD0_CMD_RW_MASK                              (1U << WJ_AVFS_VOL_SPI_CMD0_CMD_POS)
#define WJ_AVFS_VOL_SPI_CMD0_ADDR_POS                                 (9U)
#define WJ_AVFS_VOL_SPI_CMD0_ADDR_MASK                                (0x7f << WJ_AVFS_VOL_SPI_CMD0_ADDR_POS)
#define WJ_AVFS_VOL_SPI_SW_SPI_EN_POS                                 (16U)
#define WJ_AVFS_VOL_SPI_SW_SPI_EN_EN                                  (1 << WJ_AVFS_VOL_SPI_SW_SPI_EN_POS)

/* offset 0x16c  spi data reg of pmic */

/* offset 0x170  spi baud reg         */

/* offset 0x174  spi rx sample delay  */
typedef enum
{
	AVFS_PMIC_IF_I2C,
	AVFS_PMIC_IF_SPI,
	AVFS_PMIC_IF_MAX
}pmic_if_dev_t;

/* offset 0x184 dual-rail memory voltage config */

#define WJ_AVFS_VOL_DUAL_RAIL_MEM_VOL_STEP_DOWN_POS                   (0U)
#define WJ_AVFS_VOL_DUAL_RAIL_MEM_VOL_STEP_DOWN_MASK                  (0xFF << WJ_AVFS_VOL_DUAL_RAIL_MEM_VOL_STEP_DOWN_POS)

#define WJ_AVFS_VOL_DUAL_RAIL_MEM_MARGIN_DOWN_POS                     (8U)
#define WJ_AVFS_VOL_DUAL_RAIL_MEM_MARGIN_DOWN_MASK                    (0xFF << WJ_AVFS_VOL_DUAL_RAIL_MEM_MARGIN_DOWN_POS)

#define WJ_AVFS_VOL_DUAL_RAIL_MEM_MARGIN_UP_POS                       (16U)
#define WJ_AVFS_VOL_DUAL_RAIL_MEM_MARGIN_UP_MASK                    (0xFF << WJ_AVFS_VOL_DUAL_RAIL_MEM_MARGIN_UP_POS)

#define WJ_AVFS_VOL_DUAL_RAIL_MEM_VOL_STEP_UP_POS                     (24U)
#define WJ_AVFS_VOL_DUAL_RAIL_MEM_VOL_STEP_UP_MASK                    (0xFF << WJ_AVFS_VOL_DUAL_RAIL_MEM_VOL_STEP_UP_POS)

/******************************************************** sensor ctrl **********************************/

/* offset 0x200~0x208 pvt-sensor sw config */
#define WJ_AVFS_SENSOR_PVT_REG_IDX_POS                                 (0U)
#define WJ_AVFS_SENSOR_PVT_REG_IDX_MASK                                (0xf << WJ_AVFS_SENSOR_PVT_REG_IDX_POS)

#define WJ_AVFS_SENSOR_PVT_SENSOR_IDX_P0S                              (4U)
#define WJ_AVFS_SENSOR_PVT_SENSOR_IDX_MASK                             (0x1f << WJ_AVFS_SENSOR_PVT_SENSOR_IDX_POS)

#define WJ_AVFS_SENSOR_PVT_CMD_TYPE_POS                                (12U)
#define WJ_AVFS_SENSOR_PVT_CMD_TYPE_MASK                               (0x7 << WJ_AVFS_SENSOR_PVT_CMD_TYPE_POS)
#define WJ_AVFS_SENSOR_PVT_CMD_SING_READ                               (0U)
#define WJ_AVFS_SENSOR_PVT_CMD_SING_WRITE                              (1U)
#define WJ_AVFS_SENSOR_PVT_CMD_SERIAL_READ                             (2U)
#define WJ_AVFS_SENSOR_PVT_CMD_SERIAL_WRITE                            (3U)

#define WJ_AVFS_SENSOR_PVT_WDATA_POS                                   (16U)
#define WJ_AVFS_SENSOR_PVT_WDATA_MASK                                  (0xff << WJ_AVFS_SENSOR_PVT_WDATA_POS)

#define WJ_AVFS_SENSOR_PVT_CMD_EN_POS                                  (24U)
#define WJ_AVFS_SENSOR_PVT_CMD_EN_MASK                                 (1U << WJ_AVFS_SENSOR_PVT_CMD_EN_POS)
#define WJ_AVFS_SENSOR_PVT_CMD_EN                                      WJ_AVFS_SENSOR_PVT_CMD_EN_MASK

#define WJ_AVFS_SENSOR_PVT_CMD_READY_POS                               (25U)
#define WJ_AVFS_SENSOR_PVT_CMD_READY_MASK                              (1U << WJ_AVFS_SENSOR_PVT_CMD_READY_POS)

/* offset 0x20c psensor margin */

/* offset 0x210 psensor config */
#define WJ_AVFS_SENSOR_P_TYPE_POS                                      (1U)
#define WJ_AVFS_SENSOR_P_TYPE_MASK                                     (1U << WJ_AVFS_SENSOR_P_TYPE_POS)
#define WJ_AVFS_SENSOR_P_TYPE_WEIGHT                                   (0U)
#define WJ_AVFS_SENSOR_P_TYPE_MIN                                      (1U)

#define WJ_AVFS_SENSOR_P_MGR_POS                                      (0U)
#define WJ_AVFS_SENSOR_P_MGR_MASK                                     (1U << WJ_AVFS_SENSOR_P_MGR_POS)

/* offset 0x214 vsensor thd val */
#define WJ_AVFS_VSENSOR_THD_L_POS                                     (0U)
#define WJ_AVFS_VSENSOR_THD_L_MASK                                    (0xfff << WJ_AVFS_VSENSOR_THD_L_POS)

#define WJ_AVFS_VSENSOR_THD_H_POS                                     (16U)
#define WJ_AVFS_VSENSOR_THD_H_MASK                                    (0xfff << WJ_AVFS_VSENSOR_THD_H_POS)

/*offset 0x218 vsensor config */
#define WJ_AVFS_VSENSOR_MGR_MASK                                      (1U)

/* offset 0x21c tsensor thd  val */
#define WJ_AVFS_TSENSOR_THD_L_POS                                     (0U)
#define WJ_AVFS_TSENSOR_THD_L_MASK                                    (0xffff << WJ_AVFS_TSENSOR_THD_L_POS)

#define WJ_AVFS_TSENSOR_THD_H_POS                                     (16U)
#define WJ_AVFS_TSENSOR_THD_H_MASK                                    (0xffff << WJ_AVFS_TSENSOR_THD_H_POS)

/* offset 0x220 tsensor warn value */

#define WJ_AVFS_TSENSOR_WARN_L_POS                                    (0U)
#define WJ_AVFS_TSENSOR_WARN_L_MASK                                   (0xfff << WJ_AVFS_TSENSOR_WARN_L_POS)

#define WJ_AVFS_TSENSOR_WARN_H_POS                                    (16U)
#define WJ_AVFS_TSENSOR_WARN_H_MASK                                   (0xfff << WJ_AVFS_TSENSOR_WARN_H_POS)

/* offset 0x224  tsensor config */

#define WJ_AVFS_TSENSOR_CFG_MGR_MASK                                  (1U)

#define WJ_AVFS_TSENSOR_CFG_TYPE_POS                                  (1U)
#define WJ_AVFS_TSENSOR_CFG_TYPE_MASK                                 (0x3 << WJ_AVFS_TSENSOR_CFG_TYPE_POS)
#define WJ_AVFS_TSENSOR_CFG_TYPE_AVG                                  (0U)
#define WJ_AVFS_TSENSOR_CFG_TYPE_MAX                                  (1U)
#define WJ_AVFS_TSENSOR_CFG_TYPE_MIN                                  (2U)

#define WJ_AVFS_TSENSOR_ERR_FREQ_CHG_POS                              (3U)
#define WJ_AVFS_TSENSOR_ERR_FREQ_CHG_EN                               (1 << WJ_AVFS_TSENSOR_ERR_FREQ_CHG_POS)

#define WJ_AVFS_TSENSOR_RNG_POS                                       (4U)
#define WJ_AVFS_TSENSOR_RNG_MASK                                      (0x3 << WJ_AVFS_TSENSOR_RNG_POS)
#define WJ_AVFS_TSENSOR_RNG_LOW                                       (0U)
#define WJ_AVFS_TSENSOR_RNG_NORMAL                                    (1U)
#define WJ_AVFS_TSENSOR_RNG_HIGH                                      (2U)

/* offset 0x228~0x234  tsensor data */

/* offset 0x238   psensor err status */
#define WJ_AVFS_PSENSOR_ERR_STATUS(ch)                                (1 << ch)


/******************************************************** freq  ctrl  **********************************/

/* offset 0x300  freq ctrl */
#define WJ_AVFS_FREQ_CTRL_DIRECT_UP_EN                                (1 << 0U)
#define WJ_AVFS_FREQ_CTRL_FREQ_UP_CGM_EN                              (1 << 1U)
#define WJ_AVFS_FREQ_CTRL_FREQ_DOWN_BF_VOL_EN                         (1 << 2U)

/* offset 0x304  cgm ctrl */
#define WJ_AVFS_CGM_INC_MOD_REQ_POS                                   (0U)
#define WJ_AVFS_CGM_INC_MOD_REQ_MASK                                  (1U << WJ_AVFS_CGM_INC_MOD_REQ_POS)
#define WJ_AVFS_CGM_INC_MOD_REQ_EN                                    (WJ_AVFS_CGM_INC_MOD_REQ_MASK)

#define WJ_AVFS_CGM_FIX_MOD_VLD_POS                                   (1U)
#define WJ_AVFS_CGM_FIX_MOD_VLD_MASK                                  (1U << WJ_AVFS_CGM_FIX_MOD_VLD_POS)
#define WJ_AVFS_CGM_FIX_MOD_VLD_EN                                    (WJ_AVFS_CGM_FIX_MOD_VLD_MASK)

#define WJ_AVFS_CGM_GAP_POS                                           (4U)
#define WJ_AVFS_CGM_GAP_MASK                                          (0xf << WJ_AVFS_CGM_GAP_POS)

#define WJ_AVFS_CGM_GAP_MARGIN_POS                                    (8U)
#define WJ_AVFS_CGM_GAP_MARGIN_MASK                                   (0xf << WJ_AVFS_CGM_GAP_MARGIN_POS)

/* offset 0x308  cgm inc period */
#define WJ_AVFS_CGM_INC_RAMP_RATE_POS                                 (0U)
#define WJ_AVFS_CGM_INC_RAMP_RATE_MASK                                (0xffff)

/* offset 0x30c  cgm fix pattern */
#define WJ_AVFS_CGM_FIX_PATTERN_MASK                                  (0xffff)

/*offset 0x310 ~ 0x32c inc pattern */

/*offset 0x390  pll-ctrl */
#define WJ_AVFS_CMU_FREQ_CHANGE_REQ_EN                                (1U << 0)
#define WJ_AVFS_CMU_PLL_SELECT_POS                                    (1U)
#define WJ_AVFS_CMU_PLL_SELECT_MASK                                   (1 << WJ_AVFS_CMU_PLL_SELECT_POS)

/*offset 0x394 clk-switch */
#define WJ_AVFS_CMU_CLK_SWITCH_PLL_SW_POS                             (0U)
#define WJ_AVFS_CMU_CLK_SWITCH_PLL_SW_MASK                            (1U << WJ_AVFS_CMU_CLK_SWITCH_PLL_SW_POS)

#define WJ_AVFS_CMU_CLK_SWITCH_PLL0_SW0_POS                           (1U)
#define WJ_AVFS_CMU_CLK_SWITCH_PLL0_SW0_MASK                          (3U << WJ_AVFS_CMU_CLK_SWITCH_PLL0_SW0_POS)

#define WJ_AVFS_CMU_CLK_SWITCH_PLL0_SW1_POS                           (3U)
#define WJ_AVFS_CMU_CLK_SWITCH_PLL0_SW1_MASK                          (1U << WJ_AVFS_CMU_CLK_SWITCH_PLL0_SW1_POS)


#define WJ_AVFS_CMU_CLK_SWITCH_PLL1_SW0_POS                           (4U)
#define WJ_AVFS_CMU_CLK_SWITCH_PLL1_SW0_MASK                          (3U << WJ_AVFS_CMU_CLK_SWITCH_PLL1_SW0_POS)

#define WJ_AVFS_CMU_CLK_SWITCH_PLL1_SW1_POS                           (6U)
#define WJ_AVFS_CMU_CLK_SWITCH_PLL1_SW1_MASK                          (1U << WJ_AVFS_CMU_CLK_SWITCH_PLL1_SW1_POS)

#define WJ_AVFS_CMU_INC_MODE_REQ_POS                                  (7U)
#define WJ_AVFS_CMU_INC_MODE_REQ_EN                                   (1U << 7U)
#define WJ_AVFS_CMU_DI_ACLK_CALI_POS                                  (8U)
#define WJ_AVFS_CMU_DI_ACLK_CALI_REQ                                  (1U << 8U)
#define WJ_AVFS_CMU_DI_ACLK_FREQ_DOWN_POS                             (9U)
#define WJ_AVFS_CMU_DI_ACLK_FREQ_DOWN                                 (1U << 9U)

/*offset 0x398 pll-status */
#define WJ_AVFS_PLL0_LOCK_READY_MASK                                  (1U)
#define WJ_AVFS_PLL1_LOCK_READY_MASK                                  (2U)
#define WJ_AVFS_FREQ_CHG_READY_MASK                                   (4U)
#define WJ_AVFS_PLL_SATUS_MASK                                        (7U)


/******************************************************** intr  ctrl  **********************************/

/* offset 0x400~0x404  avfs-intr flag */
#define WJ_AVFS_INTR_MASK                                             (0x3ffff)
#define WJ_AVFS_INTR_HW_DONE_MASK                                     (1U << 0U)
#define WJ_AVFS_INTR_PS_ITERATE_MAX_EXCEED_MASK                       (1U << 1U)
#define WJ_AVFS_INTR_PS_BOUNCE_LOOP_MASK                              (1U << 2U)
#define WJ_AVFS_INTR_PS_DATA_ERR_MASK                                 (1U << 3U)
#define WJ_AVFS_INTR_VOL_ERR_MASK                                     (1U << 4U)
#define WJ_AVFS_INTR_VS_DONE_MASK                                     (1U << 5U)
#define WJ_AVFS_INTR_VS_ERR_MASK                                      (1U << 6U)
#define WJ_AVFS_INTR_TS_DONE_MASK                                     (1U << 7U)
#define WJ_AVFS_INTR_TS_ERR_MASK                                      (1U << 8U)
#define WJ_AVFS_INTR_PS_DONE_MASK                                     (1U << 9U)
#define WJ_AVFS_INTR_PS_ERR_MASK                                      (1U << 10U)
#define WJ_AVFS_INTR_PMIC_CMD_ERR_MASK                                (1U << 11U)
#define WJ_AVFS_INTR_PMIC_AHB_ERR_MASK                                (1U << 12U)
#define WJ_AVFS_INTR_PMIC_DONE_MASK                                   (1U << 13U)
#define WJ_AVFS_INTR_FREQ_CHG_DONE_MASK                               (1U << 14U)
#define WJ_AVFS_INTR_FREQ_NOT_MATCH_MASK                              (1U << 15U)
#define WJ_AVFS_INTR_DUAL_RAIL_MAX_MASK                               (1U << 16U)
#define WJ_AVFS_INTR_DUAL_RAIL_MIN_MASK                               (1U << 17U)

#define WJ_AVFS_INTR_ERR_MASK                                         (WJ_AVFS_INTR_PS_ITERATE_MAX_EXCEED_MASK | \
																	   WJ_AVFS_INTR_VOL_ERR_MASK | \
																	   WJ_AVFS_INTR_VS_ERR_MASK | \
																	   WJ_AVFS_INTR_TS_ERR_MASK | \
																	   WJ_AVFS_INTR_PS_ERR_MASK | \
																	   WJ_AVFS_INTR_PS_DATA_ERR_MASK | \
																	   WJ_AVFS_INTR_PMIC_AHB_ERR_MASK | \
																	   WJ_AVFS_INTR_PMIC_CMD_ERR_MASK)

/* offset 0x408~0x40c temperature intr flag */
#define WJ_AVFS_TMP_INTR_MASK                                         (0x7)
#define WJ_AVFS_TMP_INTR_TMP_FREQ_DONW_MASK                           (1U << 0U)
#define WJ_AVFS_TMP_INTR_TMP_HIGH_WARN_MASK                           (1U << 1U)
#define WJ_AVFS_TMP_INTR_TMP_LOW_WARN_MASK                            (1U << 2U)


/******************************************************** aclk  ctrl  **********************************/

/* offset 0x500       aclk work mode */

/* offset 0x504       aclk det mode  */
#define WJ_AVFS_ACLK_DET_COARSE_POS                                    (0U)
#define WJ_AVFS_ACLK_DET_COARSE_MASK                                   (0xff << WJ_AVFS_ACLK_DET_COARSE_POS)

#define WJ_AVFS_ACLK_DET_RISE_POS                                      (8U)
#define WJ_AVFS_ACLK_DET_RISE_MASK                                     (0xf << WJ_AVFS_ACLK_DET_RISE_POS)

#define WJ_AVFS_ACLK_DET_FALL_POS                                      (12U)
#define WJ_AVFS_ACLK_DET_FALL_MASK                                     (0xf << WJ_AVFS_ACLK_DET_FALL_POS)

#define WJ_AVFS_ACLK_DET_DROOP_TH_POS                                  (16U)
#define WJ_AVFS_ACLK_DET_DROOP_TH_MASK                                 (0xf << WJ_AVFS_ACLK_DET_DROOP_TH_POS)

#define WJ_AVFS_ACLK_DET_WAIT_TIME_POS                                 (20U)
#define WJ_AVFS_ACLK_DET_WAIT_TIME_MASK                                (0x3 << WJ_AVFS_ACLK_DET_WAIT_TIME_POS)

#define WJ_AVFS_ACLK_DET_FORCE_MOE_EN                                  (1U << 22U)
#define WJ_AVFS_ACLK_DET_FINE_TUNE_EN                                  (1U << 23U)

/* offset 0x508 ~0x51c  aclk tld hw cfg */

/* offset 0x520  aclk config */
#define WJ_AVFS_ACLK_CALI_REQ_EN                                       (1U << 0U)
#define WJ_AVFS_ACLK_FREQ_DOWN_EN                                      (1U << 1U)

/* offset 0x524 aclk cali status */
#define WJ_AVFS_ACLK_STATUS_CALI_IDLE_MASK                             (1U << 0U)
#define WJ_AVFS_ACLK_STATUS_CALI_ERR_MASK                              (1U << 1U)
#define WJ_AVFS_ACLK_STATUS_CALI_BUSY_MASK                             (1U << 2U)

/* offset 0x528 det CODE          */
#define WJ_AVFS_ACLK_DET_CODE_COARSE_POS                               (0U)
#define WJ_AVFS_ACLK_DET_CODE_COARSE_MASK                              (0xff << WJ_AVFS_ACLK_DET_CODE_COARSE_POS)

#define WJ_AVFS_ACLK_DET_CODE_RISE_POS                                 (8U)
#define WJ_AVFS_ACLK_DET_CODE_RISE_MASK                                (0xf << WJ_AVFS_ACLK_DET_CODE_RISE_POS)

#define WJ_AVFS_ACLK_DET_CODE_FALL_POS                                 (12U)
#define WJ_AVFS_ACLK_DET_CODE_FALL_MASK                                (0xf << WJ_AVFS_ACLK_DET_CODE_FALL_POS)

/* offset 0x52c TLD CODE       */

#define WJ_AVFS_ACLK_OSC_CODE1_POS                                     (0U)
#define WJ_AVFS_ACLK_OSC_CODE1_MASK                                    (0xff << WJ_AVFS_ACLK_OSC_CODE1_POS)

#define WJ_AVFS_ACLK_OSC_CODE2_POS                                     (8U)
#define WJ_AVFS_ACLK_OSC_CODE2_MASK                                    (0xff << WJ_AVFS_ACLK_OSC_CODE2_POS)

#define WJ_AVFS_ACLK_OSC_CODE3_POS                                     (16U)
#define WJ_AVFS_ACLK_OSC_CODE3_MASK                                    (0xff << WJ_AVFS_ACLK_OSC_CODE3_POS )


#define WJ_AVFS_DEBUG_CMU_PLL_SW_POS                                   (0U)
#define WJ_AVFS_DEBUG_CMU_PLL_SW_MASK                                  (1U << WJ_AVFS_DEBUG_CMU_PLL_SW_POS)

#define WJ_AVFS_DEBUG_CMU_PLL0_SW0_POS                                 (1U)
#define WJ_AVFS_DEBUG_CMU_PLL0_SW0_MASK                                (3U << WJ_AVFS_DEBUG_CMU_PLL0_SW0_POS)

#define WJ_AVFS_DEBUG_CMU_PLL0_SW1_POS                                 (3U)
#define WJ_AVFS_DEBUG_CMU_PLL0_SW1_MASK                                (1U << WJ_AVFS_DEBUG_CMU_PLL0_SW1_POS)

#define WJ_AVFS_DEBUG_CMU_PLL1_SW0_POS                                 (4U)
#define WJ_AVFS_DEBUG_CMU_PLL1_SW0_MASK                                (3U << WJ_AVFS_DEBUG_CMU_PLL1_SW0_POS)

#define WJ_AVFS_DEBUG_CMU_PLL1_SW1_POS                                 (6U)
#define WJ_AVFS_DEBUG_CMU_PLL1_SW1_MASK                                (1 << WJ_AVFS_DEBUG_CMU_PLL1_SW1_POS)

typedef struct 
{
	__IOM uint32_t MAIN_CTRL;                             /*offset:0x000 avfs main ctrl reg            */ 
	__IOM uint32_t STATUS;                                /*offset:0x004 avfs sensor status reg        */
	__IOM uint32_t FREQ_LVL_VAL[5];                       /*offset:0x008~0x018 freq val for lvl0~lvl8  */
	__IOM uint32_t TARGET_FREQ;                           /*offset:0x01c target freq val               */
	__IOM uint32_t WORK_MODE_CTRL;                        /*offset:0x020 config param for avfs         */
	__IOM uint32_t TMR_PERIOD;                            /*offset:0x024 timer val for avfs            */
	uint32_t       RESERVED[54];
}wj_avfs_misc_regs_t;

typedef struct 
{
	__IOM uint32_t VOL_CFG;                               /*offset:0x100*/     
	__IOM uint32_t VOL_STATUS;                            /*offset:0x104*/
	__IOM uint32_t PMIC_I2C_SPI_SELECT;                   /*offset:0x108*/
	__IOM uint32_t PMIC_I2C_BASE_ADDR;                    /*offset:0x10c*/
	__IOM uint32_t PMIC_CMD_GRP_EN;                       /*offset:0x110*/
	__IOM uint32_t PMIC_CMD_A[CMD_GRP_LEN];               /*offset:0x114~0x138*/
	__IOM uint32_t PMIC_CMD_B[CMD_GRP_LEN];               /*offset:0x138~0x160*/
	__IOM uint32_t PMIC_COUNT_BASE;                       /*offset:0x164*/
	__IOM uint32_t PMIC_SPI_CMD0;                         /*offset:0x168*/
	__IOM uint32_t PMIC_SPI_CMD1;                         /*offset:0x16c*/
	__IOM uint32_t PMIC_SPI_BAUD;                         /*offset:0x170*/
	__IOM uint32_t PMIC_SPI_RX_SAMPLE_DLY;                /*offset:0x174*/
	__IOM uint32_t PMIC_MEM_ARRAY_VLTG;                   /*offset:0x178*/
	__IOM uint32_t PMIC_VOLTAGE_MASK;                     /*offset:0x17c*/
	__IOM uint32_t PMIC_DUAL_RAIL_ENA;                    /*offset:0x180*/
	__IOM uint32_t DUAL_RAIL_MEM_VOL_CFG;                 /*offset:0x184*/
	uint32_t       RESERVED1[30];
	
}wj_avfs_voltage_ctrl_regs_t;

typedef struct 
{
	__IOM uint32_t PVT_SW_CFG[PVT_TYPE_MAX];              /*offset:0x200*/ 
	__IOM uint32_t PSENSOR_MARGIN;                        /*offset:0x20c*/ 
	__IOM uint32_t PSENSOR_CFG;                           /*offset:0x210*/ 
	__IOM uint32_t VSENSOR_TH;                            /*offset:0x214*/ 
	__IOM uint32_t VSENSOR_CFG;                           /*offset:0x218*/ 
	__IOM uint32_t TSENSOR_TH;                            /*offset:0x21c*/ 
	__IOM uint32_t TSENSOR_WARN;                          /*offset:0x220*/ 
	__IOM uint32_t TSENSOR_CFG;                           /*offset:0x224*/ 
	__IOM uint32_t TSENSOR_DATA[TS_DATA_LEN >>1 ];        /*offset:0x228~0x234*/ 
	uint32_t       RESERVED3[49];                         /*offset:0x238~0x2f8*/
	__IOM uint32_t PSENSOR_DATA_ERR_STATUS;               /*offset:0x2fc*/ 
}wj_avfs_sensor_ctrl_regs_t;

typedef struct
{
	__IOM uint32_t pll_cfg0;
	__IOM uint32_t pll_cfg1;
}pll_cfg_t;

typedef struct 
{
	__IOM uint32_t FREQ_CTRL;                             /*offset:0x300*/        
	__IOM uint32_t CGM_CTRL;                              /*offset:0x304*/        
	__IOM uint32_t CGM_INC_PERIOD;                        /*offset:0x308*/        
	__IOM uint32_t CGM_FIX_PATTERN;                       /*offset:0x30c*/        
	__IOM uint32_t CGM_INC_PATTERN[CGM_INC_PATTERN_NUM >> 1];/*offset:0x310 ~ 0x32c*/      
	uint32_t       RESERVED[4];                           /*offset:0x330~0x340*/
	pll_cfg_t CMU_PLL_GEAR_CONFIG[CGM_PLL_GEAR_CONFIG_NUM];/*offset:0x340~0x384*/
	pll_cfg_t SW_PLL_CFG;                                 /*offset:0x388~0x38c*/
	__IOM uint32_t     PLL_CTRL;                          /*offset:0x390*/
	__IOM uint32_t     CLK_SWITCH;                        /*offset:0x394*/
	__IOM uint32_t     PLL_STATUS;                        /*offset:0x398*/
	__IOM uint32_t     CUR_FREQ_LVL;                      /*offset:0x39c*/
	__IOM uint32_t     PLL_DISABLE_TIME;                  /*offset:0x3a0*/
	__IOM uint32_t     FREQ_STABLE_TIME;                  /*offset:0x3a4*/
	__IOM uint32_t     PLL_LOCK_TIMER;                    /*offset:0x3a8*/
	__IOM uint32_t     PLL_LOCK_HD_TIME;                  /*offset:0x3ac*/
	__IOM uint32_t     PLL_LOCK_TIMER_ENA;                /*offset:0x3b0*/
	__IOM uint32_t     PLL_SWITCH_CFG[3];                 /*offset:0x3b4~0x3bc*/
	__IOM uint32_t     CLK_SWITCH_TIME;                   /*offset:0x3c0 */
	uint32_t       RESERVED2[15];
}wj_avfs_freq_ctrl_regs_t;

typedef struct 
{
	__IOM uint32_t AVFS_INT_EN;                           /*offset:0x400*/ 
	__IOM uint32_t AVFS_INT_STATUS;                       /*offset:0x404*/     
	__IOM uint32_t TMP_INT_EN;                            /*offset:0x408*/        
	__IOM uint32_t TMP_INT_STATUS;                        /*offset:0x40c*/            
	uint32_t       RESERVED4[60];
}wj_avfs_intr_regs_t;

typedef struct 
{
	__IOM uint32_t ACLK_MODE;                             /*offset:0x500*/  
	__IOM uint32_t ACLK_DET_MODE;                         /*offset:0x504*/      
	__IOM uint32_t ACLK_TLD_HW_CFG[FREQ_TLD_NUM];         /*offset:0x508*/         
	__IOM uint32_t ACLK_TLD_SW_CFG;                       /*offset:0x51c*/        
	__IOM uint32_t ACLK_CFG;                              /*offset:0x520*/ 
	__IOM uint32_t ACLK_STATUS;                           /*offset:0x524*/    
	__IOM uint32_t ACLK_DET_CODE;                         /*offset:0x528*/      
	__IOM uint32_t ACLK_TLD_NUMBER;                       /*offset:0x52c*/        
	uint32_t       RESERVED5[52];
}wj_avfs_aclk_ctrl_regs_t;

typedef struct 
{
	__IOM uint32_t FPV_P;
	__IOM uint32_t FPV_V;
	__IOM uint32_t FPV_VDDM;
	uint32_t       pad;
}fpv_param_t;

typedef enum {
	PS_WT_LOW,
	PS_WT_NORMAL,
	PS_WT_HIGH,
	PS_WT_MAX
}ps_weight_t;

typedef struct 
{
	fpv_param_t fpv[FPV_TABLE_LEN];                      /* 0x800 ~ 0x88c */
	__IOM uint32_t PS_LOW_WEIGHT[PS_WEIGHT_LEN>>1];      /* 0x890 ~0x8cc  */
	__IOM uint32_t PS_NOR_WEIGHT[PS_WEIGHT_LEN>>1];      /* 0x8d0 ~0x90c  */
	__IOM uint32_t PS_HIGH_WEIGHT[PS_WEIGHT_LEN>>1];     /* 0x910 ~0x94c  */
	uint32_t RESERVED1[44];
	__IOM uint32_t P_DATA[PS_DATA_LEN];                  /* 0xa00 ~0xa7c  */
	uint32_t RESRVED2[32];
	uint32_t RESRVED3[64];                               /* 0xb00 ~0xbfc  */
	uint32_t RESRVED4[64];                               /* 0xc00 ~0xcfc  */
	uint32_t RESRVED5[64];                               /* 0xd00 ~0xdfc  */
	__IOM uint32_t V_DATA[VS_DATA_LEN];                  /* 0xe00 ~0xe1c  */

}wj_avfs_mem_data_regs_t;

typedef struct 
{
	__IOM uint32_t pll_config;
	__IOM uint32_t clk_switch;
	uint32_t       RESERVED6[62];                         

}wj_avfs_debug_regs_t;

typedef struct {
	wj_avfs_misc_regs_t          misc_base;               /*offset:0x000~0x028*/
	wj_avfs_voltage_ctrl_regs_t  vol_base;                /*offset:0x100~0x178*/
	wj_avfs_sensor_ctrl_regs_t   sensor_base;             /*offset:0x300~0x330*/
	wj_avfs_freq_ctrl_regs_t     freq_base;               /*offset:0x200~0x23c*/
	wj_avfs_intr_regs_t          intr_base;               /*offset:0x400~0x410*/
    wj_avfs_aclk_ctrl_regs_t     aclk_base;               /*offset:0x500~0x530*/
	wj_avfs_debug_regs_t         debug;                	  /*offset:0x600~0x700*/
	uint32_t       RESERVED7[63];                         /*offset:0x600~0x7fc*/
	__IM uint32_t                IP_ID;                   /*offset:0x7fc      */
    wj_avfs_mem_data_regs_t      mem_data;                /*offset:0x800 ~0xe1c */
}wj_avfs_ctrl_regs_t;

static inline uint16_t wj_avfs_get_ts_data(wj_avfs_ctrl_regs_t *avfs_base,uint32_t sensor_index)
{
	uint16_t ts_data;
	ts_data = (sensor_index & 0x1) ? (avfs_base->sensor_base.TSENSOR_DATA[sensor_index >> 1] >> 16) \
		                                : (avfs_base->sensor_base.TSENSOR_DATA[sensor_index >> 1] & 0xfff);

	return ts_data;
}

static inline uint32_t wj_avfs_debug_get_current_work_pll(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->debug.clk_switch & WJ_AVFS_DEBUG_CMU_PLL_SW_MASK;
}

static inline uint16_t wj_avfs_debug_get_pll_config(wj_avfs_ctrl_regs_t *avfs_base,uint32_t pll_index)
{
	return (pll_index == 0) ? (avfs_base->debug.pll_config & 0xffff):(avfs_base->debug.pll_config>>16);
}

static inline uint16_t wj_avfs_debug_get_pll_sw_config(wj_avfs_ctrl_regs_t *avfs_base,uint32_t pll_index)
{
	uint16_t clk_switch = avfs_base->debug.clk_switch & 0xff;
	/*  
	 * bit define as follow
	 * bit[0]     pllx_sw1
	 * bit[2:1]   pllx_sw0
	 */
	 
	if(pll_index == 0) {
		return (((clk_switch & 0x8)>>3) ) | \
			   (((clk_switch & 0x6)>>1) << 1);
	}else {
		return (((clk_switch & 0x40)>>6) ) | \
			   (((clk_switch & 0x30)>>4) << 1);

	}
}

static inline void wj_avfs_misc_config_fpv(wj_avfs_ctrl_regs_t *avfs_base,uint32_t index,uint16_t p,uint16_t v,uint16_t vddm)
{
	avfs_base->mem_data.fpv[index].FPV_P = p;
	avfs_base->mem_data.fpv[index].FPV_V = v;
	avfs_base->mem_data.fpv[index].FPV_VDDM = vddm;
}

static inline void wj_avfs_misc_config_psensor_low_weight(wj_avfs_ctrl_regs_t *avfs_base, uint8_t index, uint8_t val)
{
	if(index & 0x1) {
		avfs_base->mem_data.PS_LOW_WEIGHT[index >> 1] &=~(0xff << 8);
		avfs_base->mem_data.PS_LOW_WEIGHT[index >> 1] |= (val << 8);
	}
	else {
		avfs_base->mem_data.PS_LOW_WEIGHT[index >> 1] &=~(0xff );
		avfs_base->mem_data.PS_LOW_WEIGHT[index >> 1] |= (val );
	}
}

static inline void wj_avfs_misc_config_psensor_normal_weight(wj_avfs_ctrl_regs_t *avfs_base, uint8_t index, uint8_t val)
{
	if(index & 0x1) {
		avfs_base->mem_data.PS_NOR_WEIGHT[index >> 1] &=~(0xff << 8);
		avfs_base->mem_data.PS_NOR_WEIGHT[index >> 1] |= (val << 8);
	}
	else {
		avfs_base->mem_data.PS_NOR_WEIGHT[index >> 1] &=~(0xff );
		avfs_base->mem_data.PS_NOR_WEIGHT[index >> 1] |= (val );
	}
}

static inline void wj_avfs_misc_config_psensor_high_weight(wj_avfs_ctrl_regs_t *avfs_base, uint8_t index, uint8_t val)
{
	if(index & 0x1) {
		avfs_base->mem_data.PS_HIGH_WEIGHT[index >> 1] &=~(0xff << 8);
		avfs_base->mem_data.PS_HIGH_WEIGHT[index >> 1] |= (val << 8);
	}
	else {
		avfs_base->mem_data.PS_HIGH_WEIGHT[index >> 1] &=~(0xff );
		avfs_base->mem_data.PS_HIGH_WEIGHT[index >> 1] |= (val );
	}
}

static inline void wj_avfs_misc_hw_auto_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->misc_base.MAIN_CTRL |= WJ_AVFS_MISC_MAINCTRL_HW_EN;
}

static inline void wj_avfs_misc_hw_auto_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->misc_base.MAIN_CTRL &= ~WJ_AVFS_MISC_MAINCTRL_HW_EN;
}

static inline void wj_avfs_misc_freq_change_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->misc_base.MAIN_CTRL |= WJ_AVFS_MISC_MAINCTRL_FREQ_CHG;
}

static inline uint32_t wj_avfs_misc_get_main_state(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->misc_base.STATUS & WJ_AFVS_MISC_STATUS_MAINSTATE_MASK;
}

static inline uint32_t wj_avfs_misc_get_freq_down_state(wj_avfs_ctrl_regs_t *avfs_base)
{
	return (avfs_base->misc_base.STATUS & WJ_AVFS_MISC_STATUS_FREQ_DN_MASK) >> \
	        WJ_AVFS_MISC_STATUS_FREQ_DN_POS;
}

static inline uint32_t wj_avfs_misc_get_freq_up_state(wj_avfs_ctrl_regs_t *avfs_base)
{
	return (avfs_base->misc_base.STATUS & WJ_AVFS_MISC_STATUS_FREQ_UP_MASK) >> \
	        WJ_AVFS_MISC_STATUS_FREQ_UP_POS;
}

static inline void wj_avfs_misc_set_target_freq(wj_avfs_ctrl_regs_t *avfs_base,uint32_t freq_val)
{
	avfs_base->misc_base.TARGET_FREQ = freq_val & 0xfff;
}

static inline void wj_avfs_misc_wkmode_bypass_vsensor(wj_avfs_ctrl_regs_t *avfs_base, bool enable)
{
	if(enable) {
		avfs_base->misc_base.WORK_MODE_CTRL |= WJ_AVFS_MISC_WKCTRL_VS_BP_EN;
	}
	else {
		avfs_base->misc_base.WORK_MODE_CTRL &= ~WJ_AVFS_MISC_WKCTRL_VS_BP_EN;
	}
}

static inline void wj_avfs_misc_wkmode_bypass_voltage(wj_avfs_ctrl_regs_t *avfs_base, bool enable)
{
	if(enable) {
		avfs_base->misc_base.WORK_MODE_CTRL |= WJ_AVFS_MISC_WKCTRL_VL_BP_EN;
	}
	else {
		avfs_base->misc_base.WORK_MODE_CTRL &= ~WJ_AVFS_MISC_WKCTRL_VL_BP_EN;
	}
}

static inline void wj_avfs_misc_wkmode_bypass_psensor(wj_avfs_ctrl_regs_t *avfs_base, bool enable)
{
	if(enable) {
		avfs_base->misc_base.WORK_MODE_CTRL |= WJ_AVFS_MISC_WKCTRL_PS_BP_EN;
	}
	else {
		avfs_base->misc_base.WORK_MODE_CTRL &= ~WJ_AVFS_MISC_WKCTRL_PS_BP_EN;
	}
}

static inline void wj_avfs_misc_wkmode_periodic_sensor_detect(wj_avfs_ctrl_regs_t *avfs_base, bool enable)
{
	if(enable) {
		avfs_base->misc_base.WORK_MODE_CTRL |= WJ_AVFS_MISC_WKCTRL_TMR_EN;
	}
	else {
		avfs_base->misc_base.WORK_MODE_CTRL &= ~WJ_AVFS_MISC_WKCTRL_TMR_EN;
	}
}

static inline void wj_avfs_misc_wkmode_bypass_freq_down(wj_avfs_ctrl_regs_t *avfs_base, bool enable)
{
	if(enable) {
		avfs_base->misc_base.WORK_MODE_CTRL |= WJ_AVFS_MISC_WKCTRL_FREQUP_BPFREQ_DN_EN;
	}
	else {
		avfs_base->misc_base.WORK_MODE_CTRL &= ~WJ_AVFS_MISC_WKCTRL_FREQUP_BPFREQ_DN_EN;
	}
}

static inline void wj_avfs_misc_wkmode_bypass_aclk(wj_avfs_ctrl_regs_t *avfs_base, bool enable)
{
	if(enable) {
		avfs_base->misc_base.WORK_MODE_CTRL |= WJ_AVFS_MISC_WKCTRL_ACLK_BP_EN;
	}
	else {
		avfs_base->misc_base.WORK_MODE_CTRL &= ~WJ_AVFS_MISC_WKCTRL_ACLK_BP_EN;
	}
}

static inline void wj_avfs_misc_wkmode_set_iteration_count(wj_avfs_ctrl_regs_t *avfs_base, uint32_t val)
{
	avfs_base->misc_base.WORK_MODE_CTRL =  (avfs_base->misc_base.WORK_MODE_CTRL & (~WJ_AVFS_MISC_WKCTRL_ITERATE_NUM_MASK) ) | \
	                                       ((val & 0xff) << WJ_AVFS_MISC_WKCTRL_ITERATE_NUM_POS );
}

static inline void wj_avfs_misc_set_monitor_timer(wj_avfs_ctrl_regs_t *avfs_base, uint32_t cycles)
{
	avfs_base->misc_base.TMR_PERIOD = cycles;
}

static inline uint32_t wj_avfs_misc_is_state_busy(wj_avfs_ctrl_regs_t *avfs_base)
{
	return (avfs_base->misc_base.STATUS & WJ_AVFS_MISC_STATE_BUSY_MASK) ? 1 : 0;
}

static inline void wj_avfs_vol_cfg_step_value(wj_avfs_ctrl_regs_t *avfs_base,uint32_t step_val)
{
	uint32_t temp = avfs_base->vol_base.VOL_CFG;
	avfs_base->vol_base.VOL_CFG = (temp & (~WJ_AVFS_VOL_CFG_STEP_MASK)) | \
	                    ((step_val & 0xff)<< WJ_AVFS_VOL_CFG_STEP_POS);
}

static inline void wj_avfs_vol_cfg_margin(wj_avfs_ctrl_regs_t *avfs_base, uint32_t margin_val)
{
	uint32_t temp = avfs_base->vol_base.VOL_CFG;
	avfs_base->vol_base.VOL_CFG = (temp & (~WJ_AVFS_VOL_CFG_MARGIN_MASK)) | \
	                              ((margin_val & 0xff ) << WJ_AVFS_VOL_CFG_MARGIN_POS);
}

static inline void wj_avfs_vol_cfg_pmic_dvs_rate(wj_avfs_ctrl_regs_t *avfs_base, uint32_t dvs_rate)
{
	uint32_t temp = avfs_base->vol_base.VOL_CFG;
	avfs_base->vol_base.VOL_CFG = (temp &(~WJ_AVFS_VOL_CFG_PMIC_DVS_RATE_MASK)) | \
	                              ((dvs_rate & 0xff) << WJ_AVFS_VOL_CFG_PMIC_DVS_RATE_POS);
}

static inline void wj_avfs_vol_cfg_mi_rg_dual_rail_mem_offset(wj_avfs_ctrl_regs_t *avfs_base,uint32_t rg_dual_rail_mem_offset)
{
	uint32_t temp = avfs_base->vol_base.VOL_CFG;
	avfs_base->vol_base.VOL_CFG = (temp &(~WJ_AVFS_VOL_CFG_RG_DUAL_RAIL_MEM_OFFSET_MASK)) | \
	                              ((rg_dual_rail_mem_offset & 0xff) << WJ_AVFS_VOL_CFG_RG_DUAL_RAIL_MEM_OFFSET_POS);
}

static inline void wj_avfs_vol_config_cur_voltage(wj_avfs_ctrl_regs_t *avfs_base, uint8_t cur_mem_vol, uint8_t cur_vol)
{
	avfs_base->vol_base.VOL_STATUS &= ~(0xffff0000);
	avfs_base->vol_base.VOL_STATUS |= (cur_mem_vol << WJ_AVFS_VOL_STATUS_CUR_MEM_VOL_VAL_POS) | \
									  (cur_vol << WJ_AVFS_VOL_STATUS_CUR_VOL_VAL_POS); 
}

static inline void wj_avfs_vol_get_cur_voltage(wj_avfs_ctrl_regs_t *avfs_base, uint8_t *cur_mem_vol, uint8_t *cur_vol)
{
	*cur_vol     =(uint8_t) (avfs_base->vol_base.VOL_STATUS >> WJ_AVFS_VOL_STATUS_CUR_VOL_VAL_POS);
	*cur_mem_vol =(uint8_t) ((avfs_base->vol_base.VOL_STATUS & WJ_AVFS_VOL_STATUS_CUR_MEM_VOL_VAL_MASK ) >> WJ_AVFS_VOL_STATUS_CUR_MEM_VOL_VAL_POS);
}

static inline uint32_t wj_avfs_vol_is_spi_busy(wj_avfs_ctrl_regs_t *avfs_base)
{
	return (avfs_base->vol_base.VOL_STATUS & WJ_AVFS_VOL_STATUS_PMIC_SPI_IDLE) ? 0 : 1;
}

static inline uint32_t wj_avfs_vol_is_pmic_adjust_busy(wj_avfs_ctrl_regs_t *avfs_base)
{
	return (avfs_base->vol_base.VOL_STATUS & WJ_AVFS_VOL_STATUS_PMIC_READY) ? 0 :1;
}

static inline void wj_avfs_vol_set_if_dev(wj_avfs_ctrl_regs_t *avfs_base, pmic_if_dev_t dev)
{
	avfs_base->vol_base.PMIC_I2C_SPI_SELECT = (dev == AVFS_PMIC_IF_I2C) ? 0 : 1;
}

static inline void wj_avfs_vol_set_i2c_base_addr(wj_avfs_ctrl_regs_t *avfs_base, uint32_t addr)
{
	avfs_base->vol_base.PMIC_I2C_BASE_ADDR = addr;
}

static inline void wj_avfs_vol_cmd_grp_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->vol_base.PMIC_CMD_GRP_EN = 1;
}

static inline void wj_avfs_vol_cmd_grp_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->vol_base.PMIC_CMD_GRP_EN = 0;
}

static inline void wj_avfs_vol_set_counter(wj_avfs_ctrl_regs_t *avfs_base, uint16_t val)
{
	avfs_base->vol_base.PMIC_COUNT_BASE = val;
}

static inline void wj_avfs_vol_spi_set_baud_div(wj_avfs_ctrl_regs_t *avfs_base, uint32_t baud_div)
{
	avfs_base->vol_base.PMIC_SPI_BAUD = baud_div;
}

static inline void wj_avfs_vol_spi_set_rx_sample_dly(wj_avfs_ctrl_regs_t *avfs_base, uint32_t dly_cnt)
{
	avfs_base->vol_base.PMIC_SPI_RX_SAMPLE_DLY = dly_cnt;
}

static inline void wj_avfs_vol_set_memarray_vltg(wj_avfs_ctrl_regs_t *avfs_base,
                                                 uint32_t            vltg0,
												 uint32_t            vltg1,
												 uint32_t            vltg2)
{
	vltg0 = vltg0 & 0xff;
	vltg1 = vltg1 & 0xff;
	vltg2 = vltg2 & 0xff;
	avfs_base->vol_base.PMIC_MEM_ARRAY_VLTG = vltg0 | (vltg1 << 8) | (vltg2 << 16);
}

static inline void wj_avfs_vol_set_voltage_mask(wj_avfs_ctrl_regs_t *avfs_base, uint32_t mask_or,uint32_t mask_and)
{
	avfs_base->vol_base.PMIC_VOLTAGE_MASK = (mask_or & 0xff) | ((mask_and & 0xff) << 8);
}

static inline void wj_avfs_vol_dual_rail_mem_vltg_mode(wj_avfs_ctrl_regs_t *avfs_base, pmic_dual_rail_vol_adjust_t type)
{
	if(type == PMIC_DUAL_RAIL_SYNC_ADJUST_VOL) {

		avfs_base->vol_base.PMIC_DUAL_RAIL_ENA &= (~2); 
	}
	else {
		avfs_base->vol_base.PMIC_DUAL_RAIL_ENA |= 2; 
	}
}
static inline void wj_avfs_vol_dual_rail_mem_vltg_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->vol_base.PMIC_DUAL_RAIL_ENA |= 1;
}

static inline void wj_avfs_vol_dual_rail_mem_config(wj_avfs_ctrl_regs_t *avfs_base,uint8_t vol_step_down, uint8_t margin_down, uint8_t margin_up, uint8_t vol_step_up)
{
	avfs_base->vol_base.DUAL_RAIL_MEM_VOL_CFG = vol_step_down | \
	                                            (margin_down << 8 ) |\
												(margin_up << 16) | \
												(vol_step_up << 24);
}
static inline void wj_avfs_vol_dual_rail_mem_vltg_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->vol_base.PMIC_DUAL_RAIL_ENA &= ~1;
}

static inline void wj_avfs_sensor_set_pmargin(wj_avfs_ctrl_regs_t *avfs_base, uint16_t margin)
{
	avfs_base->sensor_base.PSENSOR_MARGIN = margin;
}

static inline void wj_avfs_sensor_cfg_psensor_type(wj_avfs_ctrl_regs_t *avfs_base, ps_sample_type_t type)
{
	if(type == PS_SAMPLE_MIN) {
		avfs_base->sensor_base.PSENSOR_CFG |= WJ_AVFS_SENSOR_P_TYPE_MASK;
	}
	else {
		avfs_base->sensor_base.PSENSOR_CFG &= ~WJ_AVFS_SENSOR_P_TYPE_MASK;
	}

}

static inline void wj_avfs_sensor_ps_mgr_en(wj_avfs_ctrl_regs_t *avfs_base, bool mgr_ena)
{
	if(mgr_ena) {
		avfs_base->sensor_base.PSENSOR_CFG |= WJ_AVFS_SENSOR_P_MGR_MASK;
	}
	else {
		avfs_base->sensor_base.PSENSOR_CFG &= ~WJ_AVFS_SENSOR_P_MGR_MASK;
	}
}

static inline void wj_avfs_sensor_vs_mgr_en(wj_avfs_ctrl_regs_t *avfs_base, bool mgr_ena)
{
	avfs_base->sensor_base.VSENSOR_CFG = (mgr_ena) ? 1 : 0;
}

static inline void wj_avfs_sensor_set_vthd(wj_avfs_ctrl_regs_t *avfs_base, uint16_t low_th, uint16_t high_th)
{
	avfs_base->sensor_base.VSENSOR_TH = low_th | high_th << 16;
}


static inline void wj_avfs_sensor_set_tthd(wj_avfs_ctrl_regs_t *avfs_base, uint16_t low_th, uint16_t high_th)
{
	avfs_base->sensor_base.TSENSOR_TH = low_th | (high_th << 16);
}

static inline void wj_avfs_sensor_set_twarn(wj_avfs_ctrl_regs_t *avfs_base, uint16_t low_warn, uint16_t high_warn)
{
	avfs_base->sensor_base.TSENSOR_WARN = low_warn | (high_warn << 16);
}


static inline uint32_t wj_avfs_sensor_get_err_status(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->sensor_base.PSENSOR_DATA_ERR_STATUS;
}

static inline void wj_avfs_sensor_clear_ps_err_status(wj_avfs_ctrl_regs_t *avfs_base, uint32_t mask)
{
	avfs_base->sensor_base.PSENSOR_DATA_ERR_STATUS = mask;
}

static inline void wj_avfs_cgm_freq_direct_up_enable(wj_avfs_ctrl_regs_t* avfs_base)
{
	avfs_base->freq_base.FREQ_CTRL |= WJ_AVFS_FREQ_CTRL_DIRECT_UP_EN;
}

static inline void wj_avfs_cgm_freq_direct_up_disable(wj_avfs_ctrl_regs_t* avfs_base)
{
	avfs_base->freq_base.FREQ_CTRL &= ~WJ_AVFS_FREQ_CTRL_DIRECT_UP_EN;
}

static inline void wj_avfs_cgm_freq_cgm_up_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.FREQ_CTRL |= WJ_AVFS_FREQ_CTRL_FREQ_UP_CGM_EN;
}

static inline void wj_avfs_cgm_freq_cgm_up_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.FREQ_CTRL &= ~WJ_AVFS_FREQ_CTRL_FREQ_UP_CGM_EN;
}

static inline void wj_avfs_cgm_freq_down_bf_volup_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.FREQ_CTRL |= WJ_AVFS_FREQ_CTRL_FREQ_DOWN_BF_VOL_EN;
}

static inline void wj_avfs_cgm_freq_down_bf_volup_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.FREQ_CTRL &= ~WJ_AVFS_FREQ_CTRL_FREQ_DOWN_BF_VOL_EN;
}

static inline void wj_avfs_cgm_ctrl_inc_mode_req_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.CGM_CTRL |= WJ_AVFS_CGM_INC_MOD_REQ_EN;
}

static inline void wj_avfs_cgm_ctrl_fix_mode_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.CGM_CTRL |= WJ_AVFS_CGM_FIX_MOD_VLD_EN;
}

static inline void wj_avfs_cgm_ctrl_fix_mode_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.CGM_CTRL &= ~WJ_AVFS_CGM_FIX_MOD_VLD_EN;
}

static inline void wj_avfs_cgm_set_gap(wj_avfs_ctrl_regs_t *avfs_base, uint32_t gap)
{
	avfs_base->freq_base.CGM_CTRL &= ~WJ_AVFS_CGM_GAP_MASK;
	avfs_base->freq_base.CGM_CTRL |= ((gap & 0xf) << WJ_AVFS_CGM_GAP_POS);
}

static inline void wj_avfs_cgm_set_gap_margin(wj_avfs_ctrl_regs_t *avfs_base, uint32_t gap_margin)
{
	avfs_base->freq_base.CGM_CTRL &= ~WJ_AVFS_CGM_GAP_MARGIN_MASK;
	avfs_base->freq_base.CGM_CTRL |= ((gap_margin & 0xf) << WJ_AVFS_CGM_GAP_MARGIN_POS);
}

static inline void wj_avfs_cgm_set_ramp_rate(wj_avfs_ctrl_regs_t *avfs_base, uint16_t cyles)
{
	avfs_base->freq_base.CGM_INC_PERIOD = cyles;
}

static inline void wj_avfs_cgm_set_fix_patern(wj_avfs_ctrl_regs_t *avfs_base, uint16_t pattern)
{
	avfs_base->freq_base.CGM_FIX_PATTERN = pattern;
}

static inline void wj_avfs_cgm_set_inc_pattern(wj_avfs_ctrl_regs_t *avfs_base, uint16_t index, uint16_t pattern)
{
	uint32_t reg_index = index >> 1;
	uint32_t high_pos = index & 0x1;
	if(high_pos) {
		avfs_base->freq_base.CGM_INC_PATTERN[reg_index] &= ~(0xffff << 16);
		avfs_base->freq_base.CGM_INC_PATTERN[reg_index] |= pattern << 16;
	}
	else {
		avfs_base->freq_base.CGM_INC_PATTERN[reg_index] &= ~(0xffff);
		avfs_base->freq_base.CGM_INC_PATTERN[reg_index] |= pattern;
	}

}

static inline void wj_avfs_cmu_set_pll_gear_params(wj_avfs_ctrl_regs_t *avfs_base,pll_cfg_t *ptable,uint32_t cfg_num)
{
	uint32_t index = 0; 
	while(index < cfg_num && index < CGM_PLL_GEAR_CONFIG_NUM){
		avfs_base->freq_base.CMU_PLL_GEAR_CONFIG[index].pll_cfg0 = ptable->pll_cfg0;
		avfs_base->freq_base.CMU_PLL_GEAR_CONFIG[index].pll_cfg1 = ptable->pll_cfg1;
		index++;
		ptable++;
	}
}

static inline void wj_avfs_cmu_cfg_pll(wj_avfs_ctrl_regs_t *avfs_base,pll_cfg_t *pll_cfg)
{
	avfs_base->freq_base.SW_PLL_CFG.pll_cfg0 = pll_cfg->pll_cfg0;
	avfs_base->freq_base.SW_PLL_CFG.pll_cfg1 = pll_cfg->pll_cfg1;
}

static inline void wj_avfs_cmu_pll_ctrl(wj_avfs_ctrl_regs_t *avfs_base, uint32_t pll_idx)
{
	avfs_base->freq_base.PLL_CTRL = WJ_AVFS_CMU_FREQ_CHANGE_REQ_EN | \
	                               ( (pll_idx) ? (WJ_AVFS_CMU_PLL_SELECT_MASK) : 0);
}

static inline void wj_avfs_cmu_inc_mode_req(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.CLK_SWITCH |= WJ_AVFS_CMU_INC_MODE_REQ_EN;
}

static inline void wj_avfs_cmu_di_aclk_cali_start(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.CLK_SWITCH |= WJ_AVFS_CMU_DI_ACLK_CALI_REQ;
}

static inline void wj_avfs_cmu_di_freq_down(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.CLK_SWITCH |= WJ_AVFS_CMU_DI_ACLK_FREQ_DOWN;
}

static inline uint32_t wj_avfs_cmu_get_clk_switch(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->freq_base.CLK_SWITCH;
}

static inline void wj_avfs_cmu_set_clk_switch(wj_avfs_ctrl_regs_t *avfs_base,uint32_t clk_switch)
{
	avfs_base->freq_base.CLK_SWITCH = clk_switch;
}

static inline uint32_t wj_avfs_cmu_get_pll_status(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->freq_base.PLL_STATUS;
}

static inline uint32_t wj_avfs_cmu_get_cur_freq_lvl(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->freq_base.CUR_FREQ_LVL;
}

static inline void wj_avfs_cmu_set_pll_disable_time(wj_avfs_ctrl_regs_t *avfs_base,uint32_t cycles)
{
	avfs_base->freq_base.PLL_DISABLE_TIME = cycles;
}

static inline void wj_avfs_cmu_set_freq_stable_time(wj_avfs_ctrl_regs_t *avfs_base,uint32_t cycles)
{
	avfs_base->freq_base.FREQ_STABLE_TIME = cycles;
}

static inline void wj_avfs_cmu_set_pll_lock_time(wj_avfs_ctrl_regs_t *avfs_base, uint32_t cycles)
{
	avfs_base->freq_base.PLL_LOCK_TIMER = cycles;
}

static inline void wj_avfs_cmu_set_pll_lock_hold_time(wj_avfs_ctrl_regs_t *avfs_base, uint32_t cycles)
{
	avfs_base->freq_base.PLL_LOCK_HD_TIME = cycles;
}

static inline void wj_avfs_cmu_pll_lock_timer_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.PLL_LOCK_TIMER_ENA = 1;
}

static inline void wj_avfs_cmu_pll_lock_timer_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->freq_base.PLL_LOCK_TIMER_ENA = 0;
}

static inline void wj_avfs_cmu_set_clk_switch_wait_time(wj_avfs_ctrl_regs_t *avfs_base,uint32_t cycles)
{
	avfs_base->freq_base.CLK_SWITCH_TIME = cycles;
}


static inline void wj_avfs_intr_enable(wj_avfs_ctrl_regs_t *avfs_base, uint32_t intr_mask)
{
	avfs_base->intr_base.AVFS_INT_EN = intr_mask;
}

static inline void wj_avfs_intr_disable(wj_avfs_ctrl_regs_t *avfs_base, uint32_t intr_mask)
{
	avfs_base->intr_base.AVFS_INT_EN &= ~intr_mask;
}

static inline uint32_t wj_avfs_intr_get_status(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->intr_base.AVFS_INT_STATUS;
}

static inline void wj_avfs_intr_clear(wj_avfs_ctrl_regs_t *avfs_base, uint32_t intr_mask)
{
	avfs_base->intr_base.AVFS_INT_STATUS = intr_mask;
}

static inline void wj_avfs_intr_tmp_enable(wj_avfs_ctrl_regs_t *avfs_base, uint32_t intr_mask)
{
	avfs_base->intr_base.TMP_INT_EN = intr_mask;
}

static inline void wj_avfs_intr_tmp_disable(wj_avfs_ctrl_regs_t *avfs_base, uint32_t intr_mask)
{
	avfs_base->intr_base.TMP_INT_EN &= ~intr_mask;
}

static inline uint32_t wj_avfs_intr_tmp_get_status(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->intr_base.TMP_INT_STATUS;
}

static inline void wj_avfs_intr_tmp_clear(wj_avfs_ctrl_regs_t *avfs_base, uint32_t intr_mask)
{
	avfs_base->intr_base.TMP_INT_STATUS = intr_mask;
}

static inline void wj_avfs_aclk_set_work_mode(wj_avfs_ctrl_regs_t *avfs_base, uint16_t work_mode)
{
	avfs_base->aclk_base.ACLK_MODE = work_mode;
}

static inline void wj_avfs_aclk_set_det_fcoarse(wj_avfs_ctrl_regs_t *avfs_base, uint32_t coarse_val)
{
	uint32_t cfg = avfs_base->aclk_base.ACLK_DET_MODE;
	cfg &=~WJ_AVFS_ACLK_DET_CODE_COARSE_MASK;
	cfg |= coarse_val;
	avfs_base->aclk_base.ACLK_DET_MODE  = cfg;
}

static inline void wj_avfs_aclk_set_det_frise(wj_avfs_ctrl_regs_t *avfs_base, uint32_t rise_val)
{
	uint32_t cfg = avfs_base->aclk_base.ACLK_DET_MODE;
	cfg &=~WJ_AVFS_ACLK_DET_CODE_RISE_MASK;
	cfg |= (rise_val << WJ_AVFS_ACLK_DET_CODE_RISE_POS);
	avfs_base->aclk_base.ACLK_DET_MODE  = cfg;
}

static inline void wj_avfs_aclk_set_det_ffall(wj_avfs_ctrl_regs_t *avfs_base, uint32_t fall_val)
{
	uint32_t cfg = avfs_base->aclk_base.ACLK_DET_MODE;
	cfg &= ~WJ_AVFS_ACLK_DET_CODE_FALL_MASK;
	cfg |= (fall_val << WJ_AVFS_ACLK_DET_CODE_FALL_POS);
	avfs_base->aclk_base.ACLK_DET_MODE  = cfg;
}


static inline void wj_avfs_aclk_set_det_drop_th(wj_avfs_ctrl_regs_t *avfs_base, uint32_t drop_th)
{
	uint32_t cfg = avfs_base->aclk_base.ACLK_DET_MODE;
	cfg &= ~WJ_AVFS_ACLK_DET_DROOP_TH_MASK;
	cfg |= ((drop_th & 0xf) << WJ_AVFS_ACLK_DET_DROOP_TH_POS);
	avfs_base->aclk_base.ACLK_DET_MODE  = cfg;
}


static inline void wj_avfs_aclk_set_det_wait_time(wj_avfs_ctrl_regs_t *avfs_base, uint32_t wait_time)
{
	uint32_t cfg = avfs_base->aclk_base.ACLK_DET_MODE;
	cfg &= ~WJ_AVFS_ACLK_DET_WAIT_TIME_MASK;
	cfg |= ((wait_time & 0x3) << WJ_AVFS_ACLK_DET_WAIT_TIME_POS);
	avfs_base->aclk_base.ACLK_DET_MODE  = cfg;
}

static inline void wj_avfs_aclk_det_force_mode_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->aclk_base.ACLK_DET_MODE |= WJ_AVFS_ACLK_DET_FORCE_MOE_EN;
}


static inline void wj_avfs_aclk_det_force_mode_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->aclk_base.ACLK_DET_MODE &= ~WJ_AVFS_ACLK_DET_FORCE_MOE_EN;
}


static inline void wj_avfs_aclk_det_tune_mode_enable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->aclk_base.ACLK_DET_MODE |= WJ_AVFS_ACLK_DET_FINE_TUNE_EN;
}


static inline void wj_avfs_aclk_det_tune_mode_disable(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->aclk_base.ACLK_DET_MODE &= ~WJ_AVFS_ACLK_DET_FINE_TUNE_EN;
}

static inline void wj_avfs_aclk_tld_config(wj_avfs_ctrl_regs_t *avfs_base, uint32_t freq_tld_index, uint32_t val)
{
	uint32_t cfg = avfs_base->aclk_base.ACLK_TLD_HW_CFG[freq_tld_index >> 1];
	if (freq_tld_index & 0x01) {
		cfg &=~0xffff0000;
		cfg |= (val & 0xffff) << 16;
		avfs_base->aclk_base.ACLK_TLD_HW_CFG[freq_tld_index >> 1] = cfg;
	}
	else {
		cfg &=~0x0000ffff;
		cfg |= (val & 0xffff);
		avfs_base->aclk_base.ACLK_TLD_HW_CFG[freq_tld_index >> 1] = cfg;
	}
}

static inline void wj_avfs_aclk_set_sw_tld_config(wj_avfs_ctrl_regs_t *avfs_base, uint32_t tld_val)
{
	avfs_base->aclk_base.ACLK_TLD_SW_CFG = tld_val & 0xfff;
}

static inline void wj_avfs_aclk_start_cali(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->aclk_base.ACLK_CFG = WJ_AVFS_ACLK_CALI_REQ_EN;
}

static inline void wj_avfs_aclk_start_freq_down(wj_avfs_ctrl_regs_t *avfs_base)
{
	avfs_base->aclk_base.ACLK_CFG = WJ_AVFS_ACLK_FREQ_DOWN_EN;
}

static inline uint32_t wj_avfs_aclk_get_status(wj_avfs_ctrl_regs_t *avfs_base)
{
	return avfs_base->aclk_base.ACLK_STATUS;
}

static inline uint32_t wj_avfs_aclk_get_coarse_code(wj_avfs_ctrl_regs_t *avfs_base)
{
	while(avfs_base->aclk_base.ACLK_STATUS & WJ_AVFS_ACLK_STATUS_CALI_BUSY_MASK);
	return avfs_base->aclk_base.ACLK_DET_CODE & WJ_AVFS_ACLK_DET_CODE_COARSE_MASK;
}

static inline uint32_t wj_avfs_aclk_get_rise_code(wj_avfs_ctrl_regs_t *avfs_base)
{
	while(avfs_base->aclk_base.ACLK_STATUS & WJ_AVFS_ACLK_STATUS_CALI_BUSY_MASK);
	return (avfs_base->aclk_base.ACLK_DET_CODE & WJ_AVFS_ACLK_DET_CODE_RISE_MASK) >> WJ_AVFS_ACLK_DET_CODE_RISE_POS;
}

static inline uint32_t wj_avfs_aclk_get_fall_code(wj_avfs_ctrl_regs_t *avfs_base)
{
	while(avfs_base->aclk_base.ACLK_STATUS & WJ_AVFS_ACLK_STATUS_CALI_BUSY_MASK);
	return (avfs_base->aclk_base.ACLK_DET_CODE & WJ_AVFS_ACLK_DET_CODE_FALL_MASK) >> WJ_AVFS_ACLK_DET_CODE_FALL_POS;
}

static inline uint32_t wj_avfs_aclk_get_osc_num_val(wj_avfs_ctrl_regs_t *avfs_base,uint32_t osc_index)
{
	/* osc_index [0:2]  */
	while(!(avfs_base->aclk_base.ACLK_STATUS & WJ_AVFS_ACLK_STATUS_CALI_IDLE_MASK));
	return (avfs_base->aclk_base.ACLK_TLD_NUMBER >> (osc_index << 3)) & 0xff;
}

csi_error_t wj_avfs_misc_set_freq_lvl(wj_avfs_ctrl_regs_t *avfs_base, avfs_tar_freq_lvl_t lvl, uint32_t freq_val);

void wj_avfs_vol_config(wj_avfs_ctrl_regs_t *avfs_base,uint32_t step_val, uint32_t margin_val, uint32_t dvs_rate);

void wj_avfs_vol_config_i2c_cmd(wj_avfs_ctrl_regs_t *avfs_base, 
                                pmic_cmd_grp_t      grp_num,
								uint32_t            cmd_idx,
								uint32_t            i2c_reg,
								uint32_t            pmic_reg,
								uint32_t            pmic_reg_val);

void wj_avfs_vol_fill_cmd(wj_avfs_ctrl_regs_t * avfs_base,
                                pmic_cmd_grp_t        grp_num,
								uint32_t              cmd_idx,
								bool                  bcmd,
								bool                  bvltg0,
								bool                  bvltg1,
								uint32_t              addr,
								uint32_t              data);

void wj_avfs_vol_spi_write_reg(wj_avfs_ctrl_regs_t *avfs_base, uint8_t reg_addr, uint8_t reg_val,uint8_t rd_pol);

uint8_t wj_avfs_vol_spi_read_reg(wj_avfs_ctrl_regs_t *avfs_base,uint8_t reg_addr,uint8_t rd_pol);

void wj_avfs_cmu_pll_set_switch_param(wj_avfs_ctrl_regs_t *avfs_base,uint8_t *clk_switch_item,uint32_t item_num);

void wj_avfs_pvt_single_write_reg(wj_avfs_ctrl_regs_t *avfs_base,
                                  pvt_type_t type,
								  uint32_t   sensor_index,
								  uint32_t   sensor_reg,
								  uint8_t    value);

void wj_avfs_pvt_searial_write_reg(wj_avfs_ctrl_regs_t *avfs_base,
                                  pvt_type_t type,
								  uint32_t   sensor_reg,
								  uint8_t    value);

csi_error_t wj_avfs_pvt_get_sensor_single_data(wj_avfs_ctrl_regs_t *avfs_base,
                                            pvt_type_t type,
											uint32_t   sensor_index,
											uint16_t   *pdata);


csi_error_t wj_avfs_pvt_get_sensor_all_data(wj_avfs_ctrl_regs_t *avfs_base,
                                            pvt_type_t type,
											uint16_t  item_start,
											uint16_t  itme_num,
											uint16_t *pdata);
csi_error_t wj_avfs_pvt_single_read_reg(wj_avfs_ctrl_regs_t *avfs_base,
                                  pvt_type_t type,
								  uint32_t   sensor_index,
								  uint32_t   sensor_reg,
								  uint16_t    *value);

csi_error_t wj_avfs_pvt_searial_read_reg(wj_avfs_ctrl_regs_t *avfs_base,
                                  pvt_type_t type,
								  uint32_t   sensor_reg,
								  uint16_t   *item_array,
								  uint16_t    item_start,
								  uint16_t    item_num);
ts_range_t wj_avfs_sensor_get_tmp_range(wj_avfs_ctrl_regs_t *avfs_base);

void wj_avfs_sensor_ts_config(wj_avfs_ctrl_regs_t *avfs_base,bool mgr_ena,ts_sample_type_t type,bool bauto_freq_adjust);

void wj_avfs_pvt_send_user_cmd(wj_avfs_ctrl_regs_t *avfs_base,
                                  pvt_type_t type,
								  uint32_t   cmd,
								  uint32_t   sensor_index,
								  uint32_t   sensor_reg,
								  uint8_t    value);

#ifdef __cplusplus
}
#endif

#endif  /* _WJ_AVFS_LL_H_*/