#ifndef COMPONENTS_CHIP_BL606_CPU_IRQ_D0_H_
#define COMPONENTS_CHIP_BL606_CPU_IRQ_D0_H_

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
typedef enum {
    /******BL606P specific Interrupt Numbers **********************************************************************/
    BMX_DSP_BUS_ERR_IRQn = IRQ_NUM_BASE + 0,            /*!< BMX DSP BUS Error Interrupt                                       */
    ISP_AWB2_IRQn        = IRQ_NUM_BASE + 1,            /*!< ISP AWB3 Interrupt                                                */
    D0_RESERVED2_IRQn    = IRQ_NUM_BASE + 2,            /*!< IPC reserved Interrupt                                            */
    UART4_IRQn           = IRQ_NUM_BASE + 3,            /*!< UART4 Interrupt                                                   */
    UART3_IRQn           = IRQ_NUM_BASE + 4,            /*!< UART3 Interrupt                                                   */
    I2C2_IRQn            = IRQ_NUM_BASE + 5,            /*!< I2C2 Interrupt                                                    */
    I2C3_IRQn            = IRQ_NUM_BASE + 6,            /*!< I2C3 Interrupt                                                    */
    SPI1_IRQn            = IRQ_NUM_BASE + 7,            /*!< SPI1 Interrupt                                                    */
    ISP_AE_IRQn          = IRQ_NUM_BASE + 8,            /*!< ISP AE Interrupt                                                  */
    ISP_AWB0_IRQn        = IRQ_NUM_BASE + 9,            /*!< ISP AWB1 Interrupt                                                */
    SEOF_INT0_IRQn       = IRQ_NUM_BASE + 10,           /*!< SEOF INT0 Interrupt                                               */
    SEOF_INT1_IRQn       = IRQ_NUM_BASE + 11,           /*!< SEOF INT1 Interrupt                                               */
    SEOF_INT2_IRQn       = IRQ_NUM_BASE + 12,           /*!< SEOF INT2 Interrupt                                               */
    DVP2BUS_INT0_IRQn    = IRQ_NUM_BASE + 13,           /*!< DVP2BUS INT0 Interrupt                                            */
    DVP2BUS_INT1_IRQn    = IRQ_NUM_BASE + 14,           /*!< DVP2BUS INT1 Interrupt                                            */
    DVP2BUS_INT2_IRQn    = IRQ_NUM_BASE + 15,           /*!< DVP2BUS INT2 Interrupt                                            */
    DVP2BUS_INT3_IRQn    = IRQ_NUM_BASE + 16,           /*!< DVP2BUS INT3 Interrupt                                            */
    H264_BS_IRQn         = IRQ_NUM_BASE + 17,           /*!< H264 BS Interrupt                                                 */
    H264_FRAME_IRQn      = IRQ_NUM_BASE + 18,           /*!< H264 Frame Interrupt                                              */
    H264_SEQ_DONE_IRQn   = IRQ_NUM_BASE + 19,           /*!< H264 SEQ Done Interrupt                                           */
    MJPEG_IRQn           = IRQ_NUM_BASE + 20,           /*!< MJPEG Interrupt                                                   */
    H264_S_BS_IRQn       = IRQ_NUM_BASE + 21,           /*!< H264 S BS Interrupt                                               */
    H264_S_FRAME_IRQn    = IRQ_NUM_BASE + 22,           /*!< H264 S Frame Interrupt                                            */
    H264_S_SEQ_DONE_IRQn = IRQ_NUM_BASE + 23,           /*!< H264 S SEQ Done Interrupt                                         */
    DMA2_INT0_IRQn       = IRQ_NUM_BASE + 24,           /*!< DMA2 INT0 Interrupt                                               */
    DMA2_INT1_IRQn       = IRQ_NUM_BASE + 25,           /*!< DMA2 INT1 Interrupt                                               */
    DMA2_INT2_IRQn       = IRQ_NUM_BASE + 26,           /*!< DMA2 INT2 Interrupt                                               */
    DMA2_INT3_IRQn       = IRQ_NUM_BASE + 27,           /*!< DMA2 INT3 Interrupt                                               */
    DMA2_INT4_IRQn       = IRQ_NUM_BASE + 28,           /*!< DMA2 INT4 Interrupt                                               */
    DMA2_INT5_IRQn       = IRQ_NUM_BASE + 29,           /*!< DMA2 INT5 Interrupt                                               */
    DMA2_INT6_IRQn       = IRQ_NUM_BASE + 30,           /*!< DMA2 INT6 Interrupt                                               */
    DMA2_INT7_IRQn       = IRQ_NUM_BASE + 31,           /*!< DMA2 INT7 Interrupt                                               */
    SDH_MMC1_IRQn        = IRQ_NUM_BASE + 32,           /*!< SDH MMC1 Interrupt                                                */
    SDH_MMC3_IRQn        = IRQ_NUM_BASE + 33,           /*!< SDH MMC3 Interrupt                                                */
    SDH2PMU_WAKEUP1_IRQn = IRQ_NUM_BASE + 34,           /*!< SDH2PMU Wakeup1 Interrupt                                         */
    SDH2PMU_WAKEUP3_IRQn = IRQ_NUM_BASE + 35,           /*!< SDH2PMU Wakeup3 Interrupt                                         */
    EMAC2_IRQn           = IRQ_NUM_BASE + 36,           /*!< EMAC2 Interrupt                                                   */
    MIPI_CSI_IRQn        = IRQ_NUM_BASE + 37,           /*!< MIPI CSI Interrupt                                                */
    IPC_D0_IRQn          = IRQ_NUM_BASE + 38,           /*!< IPC D0 Interrupt                                                  */
    APU_IRQn             = IRQ_NUM_BASE + 39,           /*!< APU Interrupt                                                     */
    MJDEC_IRQn           = IRQ_NUM_BASE + 40,           /*!< MJDEC Interrupt                                                   */
    DVP2BUS_INT4_IRQn    = IRQ_NUM_BASE + 41,           /*!< DVP2BUS INT4 Interrupt                                            */
    DVP2BUS_INT5_IRQn    = IRQ_NUM_BASE + 42,           /*!< DVP2BUS INT5 Interrupt                                            */
    DVP2BUS_INT6_IRQn    = IRQ_NUM_BASE + 43,           /*!< DVP2BUS INT6 Interrupt                                            */
    DVP2BUS_INT7_IRQn    = IRQ_NUM_BASE + 44,           /*!< DVP2BUS INT7 Interrupt                                            */
    DMA2D_INT0_IRQn      = IRQ_NUM_BASE + 45,           /*!< DMA2D INT0 Interrupt                                              */
    DMA2D_INT1_IRQn      = IRQ_NUM_BASE + 46,           /*!< DMA2D INT1 Interrupt                                              */
    DISPLAY_IRQn         = IRQ_NUM_BASE + 47,           /*!< Display Interrupt                                                 */
    PWM1_IRQn            = IRQ_NUM_BASE + 48,           /*!< PWM1 Interrupt                                                    */
    SEOF_INT3_IRQn       = IRQ_NUM_BASE + 49,           /*!< SEOF INT0 Interrupt                                               */
    RESERVED1_IRQn       = IRQ_NUM_BASE + 50,           /*!< Reserved Interrupt                                                */
    RESERVED2_IRQn       = IRQ_NUM_BASE + 51,           /*!< Reserved Interrupt                                                */
    OSD_IRQn             = IRQ_NUM_BASE + 52,           /*!< OSD Interrupt                                                     */
    DBI_IRQn             = IRQ_NUM_BASE + 53,           /*!< DBI Interrupt                                                     */
    ISP_WDR_IRQn         = IRQ_NUM_BASE + 54,           /*!< ISP WDR Interrupt                                                 */
    OSDA_BUS_DRAIN_IRQn  = IRQ_NUM_BASE + 55,           /*!< OSDA Bus Drain Interrupt                                          */
    OSDB_BUS_DRAIN_IRQn  = IRQ_NUM_BASE + 56,           /*!< OSDB Bus Drain Interrupt                                          */
    OSD_PB_IRQn          = IRQ_NUM_BASE + 57,           /*!< OSD PB Interrupt                                                  */
    ISP_AWB1_IRQn        = IRQ_NUM_BASE + 58,           /*!< ISP AWB2 Interrupt                                                */
    MIPI_DSI_IRQn        = IRQ_NUM_BASE + 59,           /*!< MIPI DSI Interrupt                                                */
    ISP_AE_HIST_IRQn     = IRQ_NUM_BASE + 60,           /*!< ISP AE HIST Interrupt                                             */
    TIMER1_CH0_IRQn      = IRQ_NUM_BASE + 61,           /*!< Timer1 Channel 0 Interrupt                                        */
    TIMER1_CH1_IRQn      = IRQ_NUM_BASE + 62,           /*!< Timer1 Channel 1 Interrupt                                        */
    TIMER1_WDT_IRQn      = IRQ_NUM_BASE + 63,           /*!< Timer1 Watch Dog Interrupt                                        */
    AUDIO_IRQn           = IRQ_NUM_BASE + 64,           /*!< Audio Interrupt                                                   */
    WL_ALL_IRQn          = IRQ_NUM_BASE + 65,           /*!< WL System All Interrupt                                           */
    PDS_IRQn             = IRQ_NUM_BASE + 66,           /*!< PDS Interrupt                                                     */
    IRQn_LAST,

} IRQn_Type;
#endif /* ARCH_RISCV */

#if defined(CPU_D0)
#define BMX_MCU_BUS_ERR_IRQn                   (IRQn_LAST)
#define BMX_MCU_TO_IRQn                        (IRQn_LAST)
#define M0_RESERVED2_IRQn                      (IRQn_LAST)
#define IPC_M0_IRQn                            (IRQn_LAST)
#define RF_TOP_INT0_IRQn                       (IRQn_LAST)
#define RF_TOP_INT1_IRQn                       (IRQn_LAST)
#define LZ4D_IRQn                              (IRQn_LAST)
#define GAUGE_ITF_IRQn                         (IRQn_LAST)
#define SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn (IRQn_LAST)
#define SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn (IRQn_LAST)
#define SEC_ENG_ID1_CDET_IRQn                  (IRQn_LAST)
#define SEC_ENG_ID0_CDET_IRQn                  (IRQn_LAST)
#define SF_CTRL_ID1_IRQn                       (IRQn_LAST)
#define SF_CTRL_ID0_IRQn                       (IRQn_LAST)
#define DMA0_ALL_IRQn                          (IRQn_LAST)
#define DMA1_ALL_IRQn                          (IRQn_LAST)
#define SDH_IRQn                               (IRQn_LAST)
#define MM_ALL_IRQn                            (IRQn_LAST)
#define IRTX_IRQn                              (IRQn_LAST)
#define IRRX_IRQn                              (IRQn_LAST)
#define USB_IRQn                               (IRQn_LAST)
#define AUPDM_IRQn                             (IRQn_LAST)
#define M0_RESERVED23_IRQn                     (IRQn_LAST)
#define EMAC_IRQn                              (IRQn_LAST)
#define GPADC_DMA_IRQn                         (IRQn_LAST)
#define EFUSE_IRQn                             (IRQn_LAST)
#define SPI0_IRQn                              (IRQn_LAST)
#define UART0_IRQn                             (IRQn_LAST)
#define UART1_IRQn                             (IRQn_LAST)
#define UART2_IRQn                             (IRQn_LAST)
#define GPIO_DMA_IRQn                          (IRQn_LAST)
#define I2C0_IRQn                              (IRQn_LAST)
#define PWM_IRQn                               (IRQn_LAST)
#define IPC_RSVD_IRQn                          (IRQn_LAST)
#define IPC_LP_IRQn                            (IRQn_LAST)
#define TIMER0_CH0_IRQn                        (IRQn_LAST)
#define TIMER0_CH1_IRQn                        (IRQn_LAST)
#define TIMER0_WDT_IRQn                        (IRQn_LAST)
#define I2C1_IRQn                              (IRQn_LAST)
#define I2S_IRQn                               (IRQn_LAST)
#define ANA_OCP_OUT_TO_CPU_0_IRQn              (IRQn_LAST)
#define ANA_OCP_OUT_TO_CPU_1_IRQn              (IRQn_LAST)
#define ANA_OCP_OUT_TO_CPU_2_IRQn              (IRQn_LAST)
#define GPIO_INT0_IRQn                         (IRQn_LAST)
#define DM_IRQn                                (IRQn_LAST)
#define BT_IRQn                                (IRQn_LAST)
#define M154_REQ_ACK_IRQn                      (IRQn_LAST)
#define M154_INT_IRQn                          (IRQn_LAST)
#define M154_AES_IRQn                          (IRQn_LAST)
#define PDS_WAKEUP_IRQn                        (IRQn_LAST)
#define HBN_OUT0_IRQn                          (IRQn_LAST)
#define HBN_OUT1_IRQn                          (IRQn_LAST)
#define BOR_IRQn                               (IRQn_LAST)
#define WIFI_IRQn                              (IRQn_LAST)
#define BZ_PHY_INT_IRQn                        (IRQn_LAST)
#define BLE_IRQn                               (IRQn_LAST)
#define MAC_TXRX_TIMER_IRQn                    (IRQn_LAST)
#define MAC_TXRX_MISC_IRQn                     (IRQn_LAST)
#define MAC_RX_TRG_IRQn                        (IRQn_LAST)
#define MAC_TX_TRG_IRQn                        (IRQn_LAST)
#define MAC_GEN_IRQn                           (IRQn_LAST)
#define MAC_PORT_TRG_IRQn                      (IRQn_LAST)
#define WIFI_IPC_PUBLIC_IRQn                   (IRQn_LAST)
#endif

#endif /* COMPONENTS_CHIP_BL606_CPU_IRQ_D0_H_ */
