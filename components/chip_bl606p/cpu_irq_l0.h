#ifndef COMPONENTS_CHIP_BL606_CPU_IRQ_L0_H_
#define COMPONENTS_CHIP_BL606_CPU_IRQ_L0_H_

#ifdef ARCH_ARM
#define IRQ_NUM_BASE 0
#endif /* ARCH_ARM */

#ifdef ARCH_RISCV
#if (__riscv_xlen == 64)
#define IRQ_NUM_BASE 16
#else /* __riscv_xlen */
#define IRQ_NUM_BASE 16
#endif
#endif /* ARCH_RISCV */


#ifdef ARCH_ARM
typedef enum {
    /******  Cortex-M4 Processor Exceptions Numbers ****************************************************************/
    NonMaskableInt_IRQn = -14,   /*!< 2 Cortex-M4 Non Maskable Interrupt                                */
    HardFault_IRQn = -13,        /*!< 3 Cortex-M4 Hard Fault Interrupt                                  */
    MemoryManagement_IRQn = -12, /*!< 4 Cortex-M4 Memory Management Interrupt                           */
    BusFault_IRQn = -11,         /*!< 5 Cortex-M4 Bus Fault Interrupt                                   */
    UsageFault_IRQn = -10,       /*!< 6 Cortex-M4 Usage Fault Interrupt                                 */
    SVCall_IRQn = -5,            /*!< 11 Cortex-M4 SV Call Interrupt                                    */
    DebugMonitor_IRQn = -4,      /*!< 12 Cortex-M4 Debug Monitor Interrupt                              */
    PendSV_IRQn = -2,            /*!< 14 Cortex-M4 Pend SV Interrupt                                    */
    SysTick_IRQn = -1,
} IRQn_Type;

#endif /* ARCH_ARM */


#ifdef ARCH_RISCV
#define CPU_LP_MAX_IRQ_NUM (IRQ_NUM_BASE + 32)
typedef enum {
    SSOFT_IRQn = 1,           /*!< 1 RISCV supervisor software Interrupt                             */
    MSOFT_IRQn = 3,           /*!< 3 RISCV machine software Interrupt                                */
    STIME_IRQn = 5,           /*!< 5 RISCV supervisor time Interrupt                                 */
    MTIME_IRQn = 7,           /*!< 7 RISCV machine time Interrupt                                    */
    SEXT_IRQn = 9,            /*!< 9 RISCV S-mode external  Interrupt                                */
    MEXT_IRQn = 11,           /*!< 11 RISCV M-mode external  Interrupt                               */
    CLIC_SOFT_PEND_IRQn = 12, /*!< 12 RISCV CLIC software pending  Interrupt                         */
#if (__riscv_xlen == 64)
    HPM_OVF_IRQn = 17, /*!< 17 RISCV HPM counter overflow Interrupt                           */
#endif /* __riscv_xlen */

    /******BL606P specific Interrupt Numbers **********************************************************************/
    BMX_MCU_BUS_ERR_IRQn                   = IRQ_NUM_BASE + 0,                    /*!< bmx mcu bus_err_int Interrupt                                     */
    BMX_MCU_TO_IRQn                        = IRQ_NUM_BASE + 1,                    /*!< bmx_timeout_int|mcu_timeout_int Interrupt                         */
    M0_RESERVED2_IRQn                      = IRQ_NUM_BASE + 2,                    /*!< reserved Interrupt                                                */
    IPC_M0_IRQn                            = IRQ_NUM_BASE + 3,                    /*!< ipc0_m0_irq Interrupt                                             */
    AUDIO_IRQn                             = IRQ_NUM_BASE + 4,                    /*!< Audio Interrupt                                                   */
    RF_TOP_INT0_IRQn                       = IRQ_NUM_BASE + 5,                    /*!< RF_TOP_INT0 Interrupt                                             */
    RF_TOP_INT1_IRQn                       = IRQ_NUM_BASE + 6,                    /*!< RF_TOP_INT1 Interrupt                                             */
    LZ4D_IRQn                              = IRQ_NUM_BASE + 7,                    /*!< LZ4 decompressor Interrupt                                        */
    GAUGE_ITF_IRQn                         = IRQ_NUM_BASE + 8,                    /*!< gauge_itf_int Interrupt                                           */
    SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 9,                    /*!< sec_eng_id1 Interrupt                                             */
    SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 10,                   /*!< sec_eng_id0 Interrupt                                             */
    SEC_ENG_ID1_CDET_IRQn                  = IRQ_NUM_BASE + 11,                   /*!< sec_eng_id1_cdet  Interrupt                                       */
    SEC_ENG_ID0_CDET_IRQn                  = IRQ_NUM_BASE + 12,                   /*!< sec_eng_id0_cdet Interrupt                                        */
    SF_CTRL_ID1_IRQn                       = IRQ_NUM_BASE + 13,                   /*!< sf_ctrl_id1 Interrupt                                             */
    SF_CTRL_ID0_IRQn                       = IRQ_NUM_BASE + 14,                   /*!< sf_ctrl_id0 Interrupt                                             */
    DMA0_ALL_IRQn                          = IRQ_NUM_BASE + 15,                   /*!< DMA0_INTR_ALL Interrupt                                           */
    DMA1_ALL_IRQn                          = IRQ_NUM_BASE + 16,                   /*!< DMA1_INTR_ALL Interrupt                                           */
    SDH_IRQn                               = IRQ_NUM_BASE + 17,                   /*!< sdh Interrupt                                                     */
    MM_ALL_IRQn                            = IRQ_NUM_BASE + 18,                   /*!< MM System All Interrupt                                           */
    IRTX_IRQn                              = IRQ_NUM_BASE + 19,                   /*!< IR TX Interrupt                                                   */
    IRRX_IRQn                              = IRQ_NUM_BASE + 20,                   /*!< IR RX Interrupt                                                   */
    USB_IRQn                               = IRQ_NUM_BASE + 21,                   /*!< USB  Interrupt                                                    */
    AUPDM_IRQn                             = IRQ_NUM_BASE + 22,                   /*!< aupdm_int Interrupt                                               */
    M0_RESERVED23_IRQn                     = IRQ_NUM_BASE + 23,                   /*!< reserved Interrupt                                                */
    EMAC_IRQn                              = IRQ_NUM_BASE + 24,                   /*!< EMAC  Interrupt                                                   */
    GPADC_DMA_IRQn                         = IRQ_NUM_BASE + 25,                   /*!< GPADC_DMA Interrupt                                               */
    EFUSE_IRQn                             = IRQ_NUM_BASE + 26,                   /*!< Efuse Interrupt                                                   */
    SPI0_IRQn                              = IRQ_NUM_BASE + 27,                   /*!< SPI0  Interrupt                                                   */
    UART0_IRQn                             = IRQ_NUM_BASE + 28,                   /*!< UART0 Interrupt                                                   */
    UART1_IRQn                             = IRQ_NUM_BASE + 29,                   /*!< UART1 Interrupt                                                   */
    UART2_IRQn                             = IRQ_NUM_BASE + 30,                   /*!< UART2 Interrupt                                                   */
    GPIO_DMA_IRQn                          = IRQ_NUM_BASE + 31,                   /*!< GPIO DMA Interrupt                                                */
    I2C0_IRQn                              = IRQ_NUM_BASE + 32,                   /*!< I2C0  Interrupt                                                   */
    PWM_IRQn                               = IRQ_NUM_BASE + 33,                   /*!< PWM Interrupt                                                     */
    IPC_RSVD_IRQn                          = IRQ_NUM_BASE + 34,                   /*!< ipc reserved Interrupt                                            */
    IPC_LP_IRQn                            = IRQ_NUM_BASE + 35,                   /*!< ipc lp_irq Interrupt                                              */
    TIMER0_CH0_IRQn                        = IRQ_NUM_BASE + 36,                   /*!< Timer0 Channel 0 Interrupt                                        */
    TIMER0_CH1_IRQn                        = IRQ_NUM_BASE + 37,                   /*!< Timer0 Channel 1 Interrupt                                        */
    TIMER0_WDT_IRQn                        = IRQ_NUM_BASE + 38,                   /*!< Timer0 Watch Dog Interrupt                                        */
    I2C1_IRQn                              = IRQ_NUM_BASE + 39,                   /*!< I2C1  Interrupt                                                   */
    I2S_IRQn                               = IRQ_NUM_BASE + 40,                   /*!< I2S  Interrupt                                                    */
    ANA_OCP_OUT_TO_CPU_0_IRQn              = IRQ_NUM_BASE + 41,                   /*!< ana_ocp_out_to_cpu_irq0 Interrupt                                 */
    ANA_OCP_OUT_TO_CPU_1_IRQn              = IRQ_NUM_BASE + 42,                   /*!< ana_ocp_out_to_cpu_irq1 Interrupt                                 */
    ANA_OCP_OUT_TO_CPU_2_IRQn              = IRQ_NUM_BASE + 43,                   /*!< ana_ocp_out_to_cpu_irq2 Interrupt                                 */
    GPIO_INT0_IRQn                         = IRQ_NUM_BASE + 44,                   /*!< GPIO Interrupt                                                    */
    DM_IRQn                                = IRQ_NUM_BASE + 45,                   /*!< DM Interrupt                                                      */
    BT_IRQn                                = IRQ_NUM_BASE + 46,                   /*!< BT Interrupt                                                      */
    M154_REQ_ACK_IRQn                      = IRQ_NUM_BASE + 47,                   /*!< M154 req enh ack Interrupt                                        */
    M154_INT_IRQn                          = IRQ_NUM_BASE + 48,                   /*!< M154 Interrupt                                                    */
    M154_AES_IRQn                          = IRQ_NUM_BASE + 49,                   /*!< m154 aes Interrupt                                                */
    PDS_WAKEUP_IRQn                        = IRQ_NUM_BASE + 50,                   /*!< PDS Wakeup Interrupt                                              */
    HBN_OUT0_IRQn                          = IRQ_NUM_BASE + 51,                   /*!< Hibernate out 0 Interrupt                                         */
    HBN_OUT1_IRQn                          = IRQ_NUM_BASE + 52,                   /*!< Hibernate out 1 Interrupt                                         */
    BOR_IRQn                               = IRQ_NUM_BASE + 53,                   /*!< BOR Interrupt                                                     */
    WIFI_IRQn                              = IRQ_NUM_BASE + 54,                   /*!< WIFI To CPU Interrupt                                             */
    BZ_PHY_INT_IRQn                        = IRQ_NUM_BASE + 55,                   /*!< BZ phy Interrupt                                                  */
    BLE_IRQn                               = IRQ_NUM_BASE + 56,                   /*!< BLE Interrupt                                                     */
    MAC_TXRX_TIMER_IRQn                    = IRQ_NUM_BASE + 57,                   /*!< MAC Tx Rx Timer Interrupt                                         */
    MAC_TXRX_MISC_IRQn                     = IRQ_NUM_BASE + 58,                   /*!< MAC Tx Rx Misc Interrupt                                          */
    MAC_RX_TRG_IRQn                        = IRQ_NUM_BASE + 59,                   /*!< MAC Rx Trigger Interrupt                                          */
    MAC_TX_TRG_IRQn                        = IRQ_NUM_BASE + 60,                   /*!< MAC tx Trigger Interrupt                                          */
    MAC_GEN_IRQn                           = IRQ_NUM_BASE + 61,                   /*!< MAC Gen Interrupt                                                 */
    MAC_PORT_TRG_IRQn                      = IRQ_NUM_BASE + 62,                   /*!< MAC Prot Trigger Interrupt                                        */
    WIFI_IPC_PUBLIC_IRQn                   = IRQ_NUM_BASE + 63,                   /*!< WIFI Ipc Interrupt                                                */
    IRQn_LAST,

} IRQn_Type;
#endif /* ARCH_RISCV */

#if defined(CPU_LP)
#define BMX_DSP_BUS_ERR_IRQn (IRQn_LAST)
#define ISP_AWB2_IRQn        (IRQn_LAST)
#define D0_RESERVED2_IRQn    (IRQn_LAST)
#define UART4_IRQn           (IRQn_LAST)
#define UART3_IRQn           (IRQn_LAST)
#define I2C2_IRQn            (IRQn_LAST)
#define I2C3_IRQn            (IRQn_LAST)
#define SPI1_IRQn            (IRQn_LAST)
#define ISP_AE_IRQn          (IRQn_LAST)
#define ISP_AWB0_IRQn        (IRQn_LAST)
#define SEOF_INT0_IRQn       (IRQn_LAST)
#define SEOF_INT1_IRQn       (IRQn_LAST)
#define SEOF_INT2_IRQn       (IRQn_LAST)
#define DVP2BUS_INT0_IRQn    (IRQn_LAST)
#define DVP2BUS_INT1_IRQn    (IRQn_LAST)
#define DVP2BUS_INT2_IRQn    (IRQn_LAST)
#define DVP2BUS_INT3_IRQn    (IRQn_LAST)
#define H264_BS_IRQn         (IRQn_LAST)
#define H264_FRAME_IRQn      (IRQn_LAST)
#define H264_SEQ_DONE_IRQn   (IRQn_LAST)
#define MJPEG_IRQn           (IRQn_LAST)
#define H264_S_BS_IRQn       (IRQn_LAST)
#define H264_S_FRAME_IRQn    (IRQn_LAST)
#define H264_S_SEQ_DONE_IRQn (IRQn_LAST)
#define DMA2_INT0_IRQn       (IRQn_LAST)
#define DMA2_INT1_IRQn       (IRQn_LAST)
#define DMA2_INT2_IRQn       (IRQn_LAST)
#define DMA2_INT3_IRQn       (IRQn_LAST)
#define DMA2_INT4_IRQn       (IRQn_LAST)
#define DMA2_INT5_IRQn       (IRQn_LAST)
#define DMA2_INT6_IRQn       (IRQn_LAST)
#define DMA2_INT7_IRQn       (IRQn_LAST)
#define SDH_MMC1_IRQn        (IRQn_LAST)
#define SDH_MMC3_IRQn        (IRQn_LAST)
#define SDH2PMU_WAKEUP1_IRQn (IRQn_LAST)
#define SDH2PMU_WAKEUP3_IRQn (IRQn_LAST)
#define EMAC2_IRQn           (IRQn_LAST)
#define MIPI_CSI_IRQn        (IRQn_LAST)
#define IPC_D0_IRQn          (IRQn_LAST)
#define APU_IRQn             (IRQn_LAST)
#define MJDEC_IRQn           (IRQn_LAST)
#define DVP2BUS_INT4_IRQn    (IRQn_LAST)
#define DVP2BUS_INT5_IRQn    (IRQn_LAST)
#define DVP2BUS_INT6_IRQn    (IRQn_LAST)
#define DVP2BUS_INT7_IRQn    (IRQn_LAST)
#define DMA2D_INT0_IRQn      (IRQn_LAST)
#define DMA2D_INT1_IRQn      (IRQn_LAST)
#define DISPLAY_IRQn         (IRQn_LAST)
#define PWM1_IRQn            (IRQn_LAST)
#define SEOF_INT3_IRQn       (IRQn_LAST)
#define RESERVED1_IRQn       (IRQn_LAST)
#define RESERVED2_IRQn       (IRQn_LAST)
#define OSD_IRQn             (IRQn_LAST)
#define DBI_IRQn             (IRQn_LAST)
#define ISP_WDR_IRQn         (IRQn_LAST)
#define OSDA_BUS_DRAIN_IRQn  (IRQn_LAST)
#define OSDB_BUS_DRAIN_IRQn  (IRQn_LAST)
#define OSD_PB_IRQn          (IRQn_LAST)
#define ISP_AWB1_IRQn        (IRQn_LAST)
#define MIPI_DSI_IRQn        (IRQn_LAST)
#define ISP_AE_HIST_IRQn     (IRQn_LAST)
#define TIMER1_CH0_IRQn      (IRQn_LAST)
#define TIMER1_CH1_IRQn      (IRQn_LAST)
#define TIMER1_WDT_IRQn      (IRQn_LAST)
#define WL_ALL_IRQn          (IRQn_LAST)
#define PDS_IRQn             (IRQn_LAST)
#endif /* CPU_LP */

#endif /* COMPONENTS_CHIP_BL606_CPU_IRQ_L0_H_ */
