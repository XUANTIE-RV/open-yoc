#ifndef DMA_H_
#define DMA_H_
#include "reg_dma.h"
#include "HAL_def.h"
#define DMA_CH_NUMBER 8

/* Exported types ---------------------------------------------*/
typedef enum
{
    CH_UART1_RX = 0,
    CH_UART1_TX = 1,
    CH_UART2_RX = 2,
    CH_UART2_TX = 3,
    CH_UART3_RX = 4,
    CH_UART3_TX = 5,
    CH_SPI1_RX = 8,
    CH_SPI1_TX = 9,
    CH_SPI2_RX = 10,
    CH_SPI2_TX = 11,
    CH_I2C1_RX = 12,
    CH_I2C1_TX = 13,
    CH_I2C2_RX = 14,
    CH_I2C2_TX = 15,
	
    CH_PDMCH0_RX = 17,
	CH_PDMCH1_RX = 18,
    CH_CRYPT_WR = 19,
    CH_CRYPT_RD = 20,
    CH_TRNG = 21,
    CH_ECC = 22,
    CH_ADC = 23,
    CH_LSADTIM1_CH1 = 24,
    CH_LSADTIM1_CH2 = 25,
    CH_LSADTIM1_CH3 = 26,
    CH_LSADTIM1_CH4 = 27,
    CH_LSADTIM1_UP = 28,
    CH_LSADTIM1_TRIG = 29,
    CH_LSADTIM1_COM = 30,
    CH_LSGPTIM2_CH1 = 32,
    CH_LSGPTIM2_CH2 = 33,
    CH_LSGPTIM2_CH3 = 34,
    CH_LSGPTIM2_CH4 = 35,
    CH_LSGPTIM2_UP = 36,
    CH_LSGPTIM2_TRIG = 37,
    CH_LSGPTIM3_CH1 = 40,
    CH_LSGPTIM3_CH2 = 41,
    CH_LSGPTIM3_CH3 = 42,
    CH_LSGPTIM3_CH4 = 43,
    CH_LSGPTIM3_UP = 44,
    CH_LSGPTIM3_TRIG = 45,
    CH_LSGPTIM1_CH1 = 48,
    CH_LSGPTIM1_CH2 = 49,
    CH_LSGPTIM1_UP = 50,
    CH_LSGPTIM1_TRIG = 51,
    CH_LSGPTIM1_COM = 52,
    CH_BSTIM1_UP = 53,
    CH_NONE = 63,
} DMA_ChnSig_TypeDef;

#define DMA_ALLSEL_NONE 0x3f3f3f3f

/**
 * @brief HAL DMA state definition for register SR
 */
typedef enum
{
    IDLE = 0,          // idle
    RD_CTLD = 1,       // reading channel control data
    RD_SRCPTR = 2,     // reading source data end pointer
    RD_DSTPTR = 3,     // reading desitination data end pointer
    RD_SRCD = 4,       // reading source data
    WR_DSTD = 5,       // writing destination data
    WT_REQCLR = 6,     // waiting for DMA req to clear
    WR_CTLD = 7,       // writing channel control data
    STALL = 8,         // stalled
    DONE = 9,          // done
    PERI_SCATGTH = 10, // peripheral scatter-gather transition
    UNDEF = 11,
} HAL_DMA_SR_STATUS_TypeDef;

/**
  * @brief  HAL DMA State structures definition  
  */
typedef enum
{
  HAL_DMA_STATE_RESET             = 0x00U,  /*!< DMA not yet initialized or disabled */  
  HAL_DMA_STATE_READY             = 0x01U,  /*!< DMA initialized and ready for use   */
  HAL_DMA_STATE_BUSY              = 0x02U,  /*!< DMA process is ongoing              */     
  HAL_DMA_STATE_TIMEOUT           = 0x03U   /*!< DMA timeout state                   */  
} HAL_DMA_StateTypeDef;

/** 
 * @brief HAL DMA control structure definiton (in system memory, pointed by register BPTR control structure base pointer)
 * 
**/
typedef union
{
    struct
    {
        uint32_t  CYCLE_CTRL    : 3;       /*!< DMA cycle operating mode, stop, basic, auto-req, ping-pong, mem scatter/gather */
        uint32_t  NEXT_USEBURST : 1;       /*!< used when performing a peripheral scatter-gather and completing an ALT control structure DMA cycle */
        uint32_t  N_MINUS_1     : 10;      /*!< total DMA transfer numbers */
        uint32_t  RPOWER        : 4;       /*!< how many DMA transfers can occur before re-arbitrate */
        uint32_t  SRC_PROT_CTRL : 3;       /*!< HPROT[3:1] value, when reads the source data */
        uint32_t  DST_PROT_CTRL : 3;       /*!< HPROT[3:1] value, when writes the destination data */
        uint32_t  SRC_SIZE      : 2;       /*!< source data size, 00 - byte... */
        uint32_t  SRC_INC       : 2;       /*!< source address increment. depends on SRC_SIZE */
        uint32_t  DST_SIZE      : 2;       /*!< destination data size, 00 - byte... */
        uint32_t  DST_INC       : 2;       /*!< destination address increment. depends on SRC_SIZE */
    }s;
    uint32_t word;
} DMA_CTRL_TypeDef;

/** 
 * @brief HAL DMA Channel Control Structure
 */
typedef struct
{
    uint32_t *src_end_ptr;  // source data pointer to end address of source data. reads this mem loc when starts at 2^R transfer. DMA does not write to this mem location
    uint32_t *dst_end_ptr;  // dst data pointer to end address of dst data. reads this mem loc when starts at 2^R transfer. DMA does not write to this mem location
    DMA_CTRL_TypeDef ctrl;  // provides control information for DMA transfer
    uint32_t unused;        // un-used definition for HW
} DMA_CTRLSTURCT_TypeDef;

/**
 * @brief DMA call back function 
 */
typedef struct
{
    void (*XferCpltCallback)(void *arg);
    void (*XferErrorCallback)(void *arg);
    void *XferCplt_arg;
    void *XferError_arg;
} HAL_DMA_Callback_TypeDef;

/** 
 * @brief DMA channel configuration definition
 */
typedef struct
{
    //DMA_CTRLSTURCT_TypeDef ctrlStruct; /*!< control structure */
    uint32_t *src_end_ptr;
    uint32_t *dst_end_ptr;
		DMA_ChnSig_TypeDef sigSel;         /*!< input signal mux selection */
    uint8_t chnIndex;                  /*!< channel index */
		uint8_t cycle_ctrl;                /*!< specify transfer modes - basic/auto/scatter-gather */
		uint8_t next_useburst;             /*!< next transfer use burst, used in peripheral scatter-gather mode */
		uint8_t rpower;                    /*!< DMA cycles before re-arbitration */ 
		uint8_t data_size;                 /*!< DMA transfer data size, in bytes/half-word/word */
		uint8_t src_inc;                   /*!< Source address increment mothod, plus byte/half-word/word/no-inc */
		uint8_t dst_inc;                   /*!< Source address increment mothod, plus byte/half-word/word/no-inc */ 
    uint8_t primary;                   /*!< primary control data structure or alternate */
    uint8_t burst;                     /*!< busrt > */
    uint8_t priority;                  /*!< channel priority */
	  uint8_t intEn;                     /*!< channel interrupt enable for DMA_done */
} DMA_CONFIG_TypeDef;

/**
 * @brief DMA handle structure definition
 */
typedef struct __DMA_HandleTypeDef
{
    reg_dma_t                 *Instance;                           /*!< DMA Channel Base Address               */
    DMA_CONFIG_TypeDef        Config;                              /*!< DMA channel communication parameter    */
    HAL_DMA_StateTypeDef      State;                               /*!< DMA state                              */
	  void                      *Parent;                             /*!< Parent object state                    */  
		HAL_LockTypeDef           Lock;                                /*!< Locking object                     */
    void (*XferCpltCallback)  (struct __DMA_HandleTypeDef * hdma); /*!< DMA transfer complete callback         */
    void (*XferErrorCallback) (struct __DMA_HandleTypeDef * hdma); /*!< DMA transfer error callback            */
    void                      *XferCplt_arg;
    void                      *XferError_arg;
} DMA_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMA_Exported_Constants DMA Exported Constants
  * @{
  */

/** @defgroup DMA_Error_Code DMA Error Code
  * @{
  */
#define HAL_DMA_ERROR_NONE 0x00000000U          /*!< No error             */
#define HAL_DMA_ERROR_TE 0x00000001U            /*!< Transfer error       */
#define HAL_DMA_ERROR_NO_XFER 0x00000004U       /*!< no ongoing transfer  */
#define HAL_DMA_ERROR_TIMEOUT 0x00000020U       /*!< Timeout error        */
#define HAL_DMA_ERROR_NOT_SUPPORTED 0x00000100U /*!< Not supported mode   */

/** 
 * @brief HAL DMA Status definition
 */
enum HAL_DMA_EnStat
{
    DISABLED = 0x0,
    ENABLED = 0x1,
};

/**
 * @brief HAL DMA channel priority level definition
 *        Priority: 
 *        1. first, by channel number, ch0 highest
 *        2. second, channel' priority, 1 - high, 0 - default
 *        Priority sequence:
 *        chn0+high, chn1+high, ..., chn7+high, chn0+default, chn1+default, ..., chn7+default
 */
enum HAL_DMA_CHPRIO
{
    DMA_PRIO_DEFAULT = 0x0,
    DMA_PRIO_HIGH = 0x1,
};

/**** Definitions for DMA control data structure ************/
/**
 * @brief HAL DMA Channel control information defintion
 */
enum HAL_DMA_CTRL_FIELD
{
    DMA_CTRL_CYCLE_CTRL_MASK = (int)0x7,
    DMA_CTRL_CYCLE_CTRL_POS = 0,
    DMA_CTRL_NEXT_USEBURST_MASK = (int)0x8,
    DMA_CTRL_NEXT_USERBURST_POS = 3,
    DMA_CTRL_NMINUS1_MASK = (int)0x3FF,
    DMA_CTRL_NMINUS1_POS = 4,
    DMA_CTRL_RPOWER_MASK = (int)0x3c000,
    DMA_CTRL_RPOWER_POS = 14,
    DMA_CTRL_SRC_PROT_MASK = (int)0x1c0000,
    DMA_CTRL_SRC_PROT_POS = 18,
    DMA_CTRL_DST_PROT_MASK = (int)0xE00000,
    DMA_CTRL_RST_PROT_POS = 21,
    DMA_CTRL_SRC_SIZE_MASK = (int)0x3000000,
    DMA_CTRL_SRC_SIZE_POS = 24,
    DMA_CTRL_SRC_INC_MASK = (int)0xc0000000,
    DMA_CTRL_SRC_INC_POS = 26,
    DMA_CTRL_DST_SIZE_MASK = (int)0x30000000,
    DMA_CTRL_DST_SIZE_POS = 28,
    DMA_CTRL_DST_INC_MASK = (int)0xc0000000,
    DMA_CTRL_DST_INC_POS = 30
};

/**
 * @brief HAL DMA Cycle Contrl definition
 */
enum HAL_DMA_CTRL_CYCLE_CTRL
{
    DMA_CC_STOP          = 0x0,  // stop, indicates data structure invalid
    DMA_CC_BASIC         = 0x1,  // controller must receive a new request, prior to entering arbit process
    DMA_CC_AUTOREQ       = 0x2,  // controller auto insert a req for appropriate chan during arbit. Means an init req is enough
    DMA_CC_PINGPONG      = 0x3,  // performs DMA cycle using 1 data structure, after completes, use the other data structure.
    DMA_CC_MEM_SCAT_PRI  = 0x4,  // memory scatter/gather in primary
    DMA_CC_MEM_SCAT_ALT  = 0x5,  // memory scatter/gather in alternate
    DMA_CC_PERI_SCAT_PRI = 0x6,  // peripheral scaater/gather primary
    DMA_CC_PERI_SCAT_ALT = 0x7,  // peripheral scaater/gather alternate
};

/**
 * @brief HAL DMA control structure's R_power definition
 *        Controls how many DMA xfer can occur before the controller rearbitrates.
 */
enum HAL_DMA_CTRL_RPOWER
{
    DMA_RPOWER1 = 0x0,    // arbit after each DMA transfer
    DMA_RPOWER2 = 0x1,    // arbit after 2 DMA transfer
    DMA_RPOWER4 = 0x2,    // arbit after 4 DMA transfer
    DMA_RPOWER8 = 0x3,    // arbit after 8 DMA transfer
    DMA_RPOWER16 = 0x4,   // arbit after 16 DMA transfer
    DMA_RPOWER32 = 0x5,   // arbit after 32 DMA transfer
    DMA_RPOWER64 = 0x6,   // arbit after 64 DMA transfer
    DMA_RPOWER128 = 0x7,  // arbit after 128 DMA transfer
    DMA_RPOWER256 = 0x8,  // arbit after 256 DMA transfer
    DMA_RPOWER512 = 0x9,  // arbit after 512 DMA transfer
    DMA_RPOWER1024 = 0xf, // arbit after 1024 DMA transfer, also means no arbit
};

/**
 * @brief HAL DMA channel control structure address increment size definition
 *        The address increment size depends on data width
 *        source data width = byte:  all valid
 *        source data width = halfword: 2'b01/10/11
 *        source data width = word: 2'b10/11
 */
enum HAL_DMA_CTRL_ADDRINCR
{
    DMA_INC_BYTE = 0x0,     // address increment + 1
    DMA_INC_HALFWORD = 0x1, // address increment + 2
    DMA_INC_WORD = 0x2,     // address increment + 4
    DMA_INC_NONE = 0x3      // address no increment
};

/**
 * @brief HAL DMA channel control structure data size definition
 */
enum DMA_CTRL_DATASIZE
{
    DMA_SIZE_BYTE = 0x0,
    DMA_SIZE_HALFWORD = 0x1,
    DMA_SIZE_WORD = 0x2
};


HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
void HAL_DMA_LL_Init(void);
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
//void HAL_DMA_IRQHandler(void);

/* Exported macros -----------------------------------------------------------*/
/** @defgroup DMA_Exported_Macros DMA Exported Macros
  * @{
  */

/**
  * @brief  Enable the DMA controller.
  * @param  __HANDLE__: DMA register base
  * @retval None
  */
#define __HAL_DMA_MASTER_ENABLE(__HANDLE__) (SET_BIT((__HANDLE__)->CFG, DMA_CFG_EN))

/**
  * @brief  Enable the DMA controller.
  * @param  __HANDLE__: DMA register base
  * @retval None
  */
#define __HAL_DMA_MSATER_DISABLE(__HANDLE__) (CLEAR_BIT((__HANDLE__)->CFG, DMA_CFG_EN))

/** @brief  Reset DMA handle state.
  * @param  __HANDLE__: DMA handle
  * @retval None
  */
#define __HAL_DMA_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_DMA_STATE_RESET)

/**
  * @brief  Enable the specified DMA Channel.
  * @param  __HANDLE__: DMA handle
  * @retval None
  */
#define __HAL_DMA_ENABLE(__HANDLE__, __CHN__) (SET_BIT((__HANDLE__)->ENSET, (1 << __CHN__)))

/**
  * @brief  Disable the specified DMA Channel.
  * @param  __HANDLE__: DMA handle
  * @retval None
  */
#define __HAL_DMA_DISABLE(__HANDLE__, __CHN__) (SET_BIT((__HANDLE__)->ENCLR, (1 << __CHN__)))

/* Interrupt & Flag management */
/**
  * @brief  Enables the specified DMA Channel interrupts.
  * @param  __HANDLE__: DMA handle
  * @param __INTERRUPT__: specifies the DMA interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA_IT_HT:  Half transfer complete interrupt mask
  *            @arg DMA_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_DMA_ENABLE_IT(__HANDLE__, __CHN__) (SET_BIT((__HANDLE__)->DONEIEF, (1 << __CHN__)))

/**
  * @brief  Disable the specified DMA Channel interrupts.
  * @param  __HANDLE__: DMA handle
  * @param  __CHN__: specifies the DMA interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA_IT_HT:  Half transfer complete interrupt mask
  *            @arg DMA_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_DMA_DISABLE_IT(__HANDLE__, __CHN__) (CLEAR_BIT((__HANDLE__)->DONEIEF, (1 << __CHN__)))

#define __HAL_DMA_EN_ERR_IT(__HANDLE__)    (WRITE_REG((__HANDLE__)->ERRIEF, 1))
#define __HAL_DMA_DIS_ERR_IT(__HANDLE__)   (WRITE_REG((__HANDLE__)->ERRIEF, 0))

#define __HAL_DMA_CLEAR_FLAG(__HANDLE__, __CHN__) (__HANDLE__->DONEICF = (1 << __CHN__))
#define __HAL_DMA_CLEAR_ERR_FLAG(__HANDLE__) (__HANDLE__->ERRICF = 1)

#endif //(_LE501X_HAL_DMA_H)
