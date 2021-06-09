/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_mmc.c
 * @brief    CSI Source File for mmc Driver
 * @version  V1.0
 * @date     29. June 2018
 ******************************************************************************/

#include <drv/sdif.h>
#include "silan_pic.h"
#include "silan_syscfg.h"
#include <dw_mmc_reg.h>
#include "silan_common.h"
#include <dw_mmc.h>
#include <csi_core.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ERR_SDIF(errno) (CSI_DRV_ERRNO_SDIF_BASE | errno)

typedef struct  {
    uint32_t base;
    uint32_t irq;
} dw_sdif_priv_t;

/*
 * brief sdif handle
 *
 * Defines the structure to save the sdif state information and callback function. The detail interrupt status when
 * send command or transfer data can be obtained from interrupt_flags field by using mask defined in
 * sdif_interrupt_flag_t;
 * note All the fields except interrupt_flags and transfered_words must be allocated by the user.
 */
typedef struct {
    /* Transfer parameter */
    sdif_data_t *volatile       data;                   ///< Data to transfer
    sdif_command_t *volatile    command;                ///< Command to send

    /* Transfer status */
    volatile uint32_t           interrupt_flags;        ///< Interrupt flags of last transaction
    volatile uint32_t           dma_interrupt_flags;    ///< DMA interrupt flags of last transaction
    volatile uint32_t           transfered_words;       ///< Words transferred by polling way

    /* Callback functions */
    sdif_callback_t    callback;               ///< Callback function
    void *                      user_data;              ///< Parameter for transfer complete callback
} sdif_state_t;

sdif_state_t g_sdifState[FSL_FEATURE_SOC_SDIF_COUNT];

/* SDIF - Peripheral instance base addresses */
/** Peripheral SDIF base address */
#define SDIF0_BASE                               SILAN_SDMMC_BASE
#define SDIF1_BASE                               SILAN_SDIO_BASE
/** Peripheral SDIF base pointer */
#define SDIF0                                    ((SDIF_TYPE *)SDIF0_BASE)
#define SDIF1                                    ((SDIF_TYPE *)SDIF1_BASE)
/** Array initializer of SDIF peripheral base addresses */
#define SDIF_BASE_ADDRS                          { SDIF0_BASE, SDIF1_BASE }
/** Array initializer of SDIF peripheral base pointers */
#define SDIF_BASE_PTRS                           { SDIF0, SDIF1 }
/** Interrupt vectors for the SDIF peripheral type */
#define SDIF_IRQS                                { SDMMC_IRQn, SDIO_IRQn }

/* Typedef for interrupt handler. */
typedef void (*sdif_isr_t)(SDIF_TYPE *base, sdif_state_t *handle);

/*! @brief convert the name here, due to RM use SDIO */
//#define SDIF_DriverIRQHandler SDIO_DriverIRQHandler
/*! @brief define the controller support sd/sdio card version 2.0 */
#define SDIF_SUPPORT_SD_VERSION (0x20)
/*! @brief define the controller support mmc card version 4.4 */
#define SDIF_SUPPORT_MMC_VERSION (0x44)
/*! @brief define the timeout counter */
#define SDIF_TIMEOUT_VALUE (6553500U)
/*! @brief this value can be any value */
#define SDIF_POLL_DEMAND_VALUE (0xFFU)
/*! @brief DMA descriptor buffer1 size */
#define SDIF_DMA_DESCRIPTOR_BUFFER1_SIZE(x) (x & 0x1FFFU)
/*! @brief DMA descriptor buffer2 size */
#define SDIF_DMA_DESCRIPTOR_BUFFER2_SIZE(x) ((x & 0x1FFFU) << 13U)
/*! @brief RX water mark value */
#define SDIF_RX_WATERMARK (15U)
/*! @brief TX water mark value */
#define SDIF_TX_WATERMARK (16U)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the instance.
 *
 * @param base SDIF peripheral base address.
 * @return Instance number.
 */
// static uint32_t SDIF_GetInstance(SDIF_TYPE *base);

/*
* @brief config the SDIF interface before transfer between the card and host
* @param SDIF base address
* @param transfer config structure
*/
static status_t SDIF_TransferConfig(SDIF_TYPE *base, sdif_transfer_t *transfer);

/*
* @brief wait the command done function and check error status
* @param SDIF base address
* @param command config structure
*/
static status_t SDIF_WaitCommandDone(SDIF_TYPE *base, sdif_command_t *command);

/*
* @brief transfer data in a blocking way
* @param SDIF base address
* @param data config structure
* @param indicate current transfer mode:DMA or polling
*/
static status_t SDIF_TransferDataBlocking(SDIF_TYPE *base, sdif_data_t *data, bool isDMA);

/*
* @brief read the command response
* @param SDIF base address
* @param sdif command pointer
*/
static status_t SDIF_ReadCommandResponse(SDIF_TYPE *base, sdif_command_t *command);

/*
* @brief handle transfer command interrupt
* @param SDIF base address
* @param sdif handle
* @param interrupt mask flags
*/
static void SDIF_TransferHandleCommand(SDIF_TYPE *base, sdif_state_t *handle, uint32_t interrupt_flags);

/*
* @brief handle transfer data interrupt
* @param SDIF base address
* @param sdif handle
* @param interrupt mask flags
*/
static void SDIF_TransferHandleData(SDIF_TYPE *base, sdif_state_t *handle, uint32_t interrupt_flags);

/*
* @brief handle DMA transfer
* @param SDIF base address
* @param sdif handle
* @param interrupt mask flag
*/
static void SDIF_TransferHandleDMA(SDIF_TYPE *base, sdif_state_t *handle, uint32_t interrupt_flags);

/*
* @brief driver IRQ handler
* @param SDIF base address
* @param sdif handle
*/
static void SDIF_TransferHandleIRQ(SDIF_TYPE *base, sdif_state_t *handle);

/*
* @brief read data port
* @param SDIF base address
* @param sdif data
* @param the number of data been transferred
*/
static uint32_t SDIF_ReadDataPort(SDIF_TYPE *base, sdif_data_t *data, uint32_t transfered_words);

/*
* @brief write data port
* @param SDIF base address
* @param sdif data
* @param the number of data been transferred
*/
static uint32_t SDIF_WriteDataPort(SDIF_TYPE *base, sdif_data_t *data, uint32_t transfered_words);

/*
* @brief read data by blocking way
* @param SDIF base address
* @param sdif data
*/
static status_t SDIF_ReadDataPortBlocking(SDIF_TYPE *base, sdif_data_t *data);

/*
* @brief write data by blocking way
* @param SDIF base address
* @param sdif data
*/
static status_t SDIF_WriteDataPortBlocking(SDIF_TYPE *base, sdif_data_t *data);

/*
* @brief handle sdio interrupt
* This function will call the SDIO interrupt callback
* @param SDIF base address
* @param SDIF handle
*/
static void SDIF_TransferHandleSDIOInterrupt(SDIF_TYPE *base, sdif_state_t *handle);

/*
* @brief handle card detect
* This function will call the card_inserted callback
* @param SDIF base addres
* @param SDIF handle
*/
static void SDIF_TransferHandleCardDetect(SDIF_TYPE *base, sdif_state_t *handle);

static void SDIF0_DriverIRQHandler(void);
static void SDIF1_DriverIRQHandler(void);


static void SDIF_IRQEnable(int id, void *handle);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief SDIF internal handle pointer array */
static sdif_state_t *s_sdifState[FSL_FEATURE_SOC_SDIF_COUNT];

/*! @brief SDIF base pointer array */
static SDIF_TYPE *const s_sdifBase[] = SDIF_BASE_PTRS;

/*! @brief SDIF IRQ name array */
static const int s_sdifIRQ[] = SDIF_IRQS;

/* SDIF ISR for transactional APIs. */
static sdif_isr_t s_sdifIsr;

static void *s_sdifIRQEntry[] = {SDIF0_DriverIRQHandler, SDIF1_DriverIRQHandler};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief set card data bus width
 * @param base SDIF peripheral base address.
 * @param data bus width type
 */
static inline void SDIF_SetCardBusWidth(SDIF_TYPE *base, sdif_bus_width_e type)
{
    base->CTYPE = type;
}

uint32_t SDIF_GetInstance(SDIF_TYPE *base)
{
    uint8_t instance = 0U;

    while ((instance < ARRAY_SIZE(s_sdifBase)) && (s_sdifBase[instance] != base)) {
        instance++;
    }

    assert(instance < ARRAY_SIZE(s_sdifBase));

    return instance;
}

static status_t SDIF_TransferConfig(SDIF_TYPE *base, sdif_transfer_t *transfer)
{
    sdif_command_t *command = transfer->command;
    sdif_data_t *data = transfer->data;

    if ((command == NULL) || (data && (data->block_size > SDIF_BLKSIZ_BLOCK_SIZE_MASK))) {
        return kStatus_SDIF_InvalidArgument;
    }

    if (data != NULL) {
        /* config the block size register ,the block size maybe smaller than FIFO
         depth, will test on the board */
        base->BLKSIZ = SDIF_BLKSIZ_BLOCK_SIZE(data->block_size);
        /* config the byte count register */
        base->BYTCNT = SDIF_BYTCNT_BYTE_COUNT(data->block_size * data->block_count);

        command->flags |= kSDIF_DataExpect; /* need transfer data flag */

        if (data->tx_data_buffer != NULL) {
            command->flags |= kSDIF_DataWriteToCard; /* data transfer direction */
        } else {
            /* config the card read threshold,enable the card read threshold */
            /*
            if (data->block_size <= (SDIF_FIFO_COUNT * sizeof(uint32_t)))
            {
                base->CARDTHRCTL = SDIF_CARDTHRCTL_CARDRDTHREN_MASK | SDIF_CARDTHRCTL_CARDTHRESHOLD(data->block_size);
            }
            else
            {
                base->CARDTHRCTL &= ~SDIF_CARDTHRCTL_CARDRDTHREN_MASK;
            }
            */
        }

        if (data->stream_transfer) {
            command->flags |= kSDIF_DataStreamTransfer; /* indicate if use stream transfer or block transfer  */
        }

        if ((data->enable_auto_command12) &&
            (data->block_count > 1U)) { /* indicate if auto stop will send after the data transfer done */
            command->flags |= kSDIF_DataTransferAutoStop;
        }
    }

    /* R2 response length long */
    if (command->response_type == SDIF_CARD_RESPONSE_R2) {
        command->flags |= (kSDIF_CmdCheckResponseCRC | kSDIF_CmdResponseLengthLong | kSDIF_CmdResponseExpect);
    } else if ((command->response_type == SDIF_CARD_RESPONSE_R3) || (command->response_type == SDIF_CARD_RESPONSE_R4)) {
        command->flags |= kSDIF_CmdResponseExpect; /* response R3 do not check Response CRC */
    } else {
        if (command->response_type != SDIF_CARD_RESPONSE_NONE) {
            command->flags |= (kSDIF_CmdCheckResponseCRC | kSDIF_CmdResponseExpect);
        }
    }

    if (command->type == SDIF_CARD_COMMAND_ABORT) {
        command->flags |= kSDIF_TransferStopAbort;
    }

    /* wait pre-transfer complete */
    command->flags |= kSDIF_WaitPretransfer_complete | kSDIF_CmdDataUseHoldReg;

    return kStatus_Success;
}

static status_t SDIF_ReadCommandResponse(SDIF_TYPE *base, sdif_command_t *command)
{
    /* check if command exist,if not, do not read the response */
    if (NULL != command) {
        /* read response */
        command->response[0U] = base->RESP[0U];

        if (command->response_type == SDIF_CARD_RESPONSE_R2) {
            command->response[1U] = base->RESP[1U];
            command->response[2U] = base->RESP[2U];
            command->response[3U] = base->RESP[3U];
        }

        if ((command->resopnse_error_flags != 0U) &&
            ((command->response_type == SDIF_CARD_RESPONSE_R1) || (command->response_type == SDIF_CARD_RESPONSE_R1b) ||
             (command->response_type == SDIF_CARD_RESPONSE_R6) || (command->response_type == SDIF_CARD_RESPONSE_R5))) {
            if (((command->resopnse_error_flags) & (command->response[0U])) != 0U) {
                return kStatus_SDIF_ResponseError;
            }
        }
    }

    return kStatus_Success;
}

static status_t SDIF_WaitCommandDone(SDIF_TYPE *base, sdif_command_t *command)
{
    uint32_t status = 0U;

    do {
        status = SDIF_GetInterruptStatus(base);
    } while ((status & kSDIF_CommandDone) != kSDIF_CommandDone);

    /* clear interrupt status flag first */
    SDIF_ClearInterruptStatus(base, status & kSDIF_CommandTransferStatus);

    if ((status & (kSDIF_ResponseError | kSDIF_ResponseCRCError | kSDIF_ResponseTimeout | kSDIF_HardwareLockError)) !=
        0u) {
        return kStatus_SDIF_SendCmdFail;
    } else {
        return SDIF_ReadCommandResponse(base, command);
    }
}

status_t SDIF_ReleaseDMADescriptor(SDIF_TYPE *base, sdif_dma_config_t *dmaConfig)
{
    assert(NULL != dmaConfig);
    assert(NULL != dmaConfig->dma_des_buffer_start_addr);

    sdif_dma_descriptor_t *dmaDesAddr;
    uint32_t *tempDMADesBuffer = dmaConfig->dma_des_buffer_start_addr;
    uint32_t dmaDesBufferSize = 0U;

    dmaDesAddr = (sdif_dma_descriptor_t *)tempDMADesBuffer;

    /* chain descriptor mode */
    if (dmaConfig->mode == SDIF_CHAIN_DMA_MODE) {
        while (((dmaDesAddr->dmaDesAttribute & kSDIF_DMADescriptorDataBufferEnd) != kSDIF_DMADescriptorDataBufferEnd) &&
               (dmaDesBufferSize < dmaConfig->dma_des_buffer_len * sizeof(uint32_t))) {
            /* set the OWN bit */
            dmaDesAddr->dmaDesAttribute |= kSDIF_DMADescriptorOwnByDMA;
            dmaDesAddr++;
            dmaDesBufferSize += sizeof(sdif_dma_descriptor_t);
        }

        /* if access dma des address overflow, return fail */
        if (dmaDesBufferSize > dmaConfig->dma_des_buffer_len * sizeof(uint32_t)) {
            return kStatus_Fail;
        }

        dmaDesAddr->dmaDesAttribute |= kSDIF_DMADescriptorOwnByDMA;
    }
    /* dual descriptor mode */
    else {
        while (((dmaDesAddr->dmaDesAttribute & kSDIF_DMADescriptorEnd) != kSDIF_DMADescriptorEnd) &&
               (dmaDesBufferSize < dmaConfig->dma_des_buffer_len * sizeof(uint32_t))) {
            dmaDesAddr = (sdif_dma_descriptor_t *)tempDMADesBuffer;
            dmaDesAddr->dmaDesAttribute |= kSDIF_DMADescriptorOwnByDMA;
            tempDMADesBuffer += dmaConfig->dma_dws_skip_len;
        }

        /* if access dma des address overflow, return fail */
        if (dmaDesBufferSize > dmaConfig->dma_des_buffer_len * sizeof(uint32_t)) {
            return kStatus_Fail;
        }

        dmaDesAddr->dmaDesAttribute |= kSDIF_DMADescriptorOwnByDMA;
    }

    /* reload DMA descriptor */
    base->PLDMND = SDIF_POLL_DEMAND_VALUE;

    return kStatus_Success;
}

static uint32_t SDIF_ReadDataPort(SDIF_TYPE *base, sdif_data_t *data, uint32_t transfered_words)
{
    uint32_t i;
    uint32_t totalWords;
    uint32_t wordsCanBeRead; /* The words can be read at this time. */
    uint32_t readWatermark = ((base->FIFOTH & SDIF_FIFOTH_RX_WMARK_MASK) >> SDIF_FIFOTH_RX_WMARK_SHIFT);

    if ((base->CTRL & SDIF_CTRL_USE_INTERNAL_DMAC_MASK) == 0U) {
        if (data->block_size % sizeof(uint32_t) != 0U) {
            data->block_size +=
                sizeof(uint32_t) - (data->block_size % sizeof(uint32_t)); /* make the block size as word-aligned */
        }

        totalWords = ((data->block_count * data->block_size) / sizeof(uint32_t));

        /* If watermark level is equal or bigger than totalWords, transfers totalWords data. */
        if (readWatermark >= totalWords) {
            wordsCanBeRead = totalWords;
        }
        /* If watermark level is less than totalWords and left words to be sent is equal or bigger than readWatermark,
        transfers watermark level words. */
        else if ((readWatermark < totalWords) && ((totalWords - transfered_words) >= readWatermark)) {
            wordsCanBeRead = readWatermark;
        }
        /* If watermark level is less than totalWords and left words to be sent is less than readWatermark, transfers
        left
        words. */
        else {
            wordsCanBeRead = (totalWords - transfered_words);
        }

        i = 0U;

        while (i < wordsCanBeRead) {
            data->rx_date_buffer[transfered_words++] = base->FIFO[0];
            i++;
        }
    }

    return transfered_words;
}

static uint32_t SDIF_WriteDataPort(SDIF_TYPE *base, sdif_data_t *data, uint32_t transfered_words)
{
    uint32_t i;
    uint32_t totalWords;
    uint32_t wordsCanBeWrite; /* The words can be read at this time. */
    uint32_t writeWatermark = ((base->FIFOTH & SDIF_FIFOTH_TX_WMARK_MASK) >> SDIF_FIFOTH_TX_WMARK_SHIFT);

    writeWatermark = writeWatermark - ((base->STATUS  >> 17) & 0x1ff);

    if ((base->CTRL & SDIF_CTRL_USE_INTERNAL_DMAC_MASK) == 0U) {

        if (data->block_size % sizeof(uint32_t) != 0U) {
            data->block_size +=
                sizeof(uint32_t) - (data->block_size % sizeof(uint32_t)); /* make the block size as word-aligned */
        }

        totalWords = ((data->block_count * data->block_size) / sizeof(uint32_t));

        /* If watermark level is equal or bigger than totalWords, transfers totalWords data. */
        if (writeWatermark >= totalWords) {
            wordsCanBeWrite = totalWords;
        }
        /* If watermark level is less than totalWords and left words to be sent is equal or bigger than writeWatermark,
        transfers watermark level words. */
        else if ((writeWatermark < totalWords) && ((totalWords - transfered_words) >= writeWatermark)) {
            wordsCanBeWrite = writeWatermark;
        }
        /* If watermark level is less than totalWords and left words to be sent is less than writeWatermark, transfers
        left
        words. */
        else {
            wordsCanBeWrite = (totalWords - transfered_words);
        }

        i = 0U;

        while (i < wordsCanBeWrite) {
            base->FIFO[i] = data->tx_data_buffer[transfered_words++];
            i++;
        }
    }

    return transfered_words;
}

static status_t SDIF_ReadDataPortBlocking(SDIF_TYPE *base, sdif_data_t *data)
{
    uint32_t totalWords;
    uint32_t transfered_words = 0U;
    status_t error = kStatus_Success;
    uint32_t status;
    bool transferOver = false;

    if (data->block_size % sizeof(uint32_t) != 0U) {
        data->block_size +=
            sizeof(uint32_t) - (data->block_size % sizeof(uint32_t)); /* make the block size as word-aligned */
    }

    totalWords = ((data->block_count * data->block_size) / sizeof(uint32_t));

    while ((transfered_words < totalWords) && (error == kStatus_Success)) {
        /* wait data transfer complete or reach RX watermark */
        do {
            status = SDIF_GetInterruptStatus(base);

            if (status & kSDIF_DataTransferError) {
                if (!(data->enable_ignore_error)) {
                    error = kStatus_Fail;
                    break;
                }
            }
        } while (((status & (kSDIF_DataTransferOver | kSDIF_ReadFIFORequest)) == 0U) && (!transferOver));

        if ((status & kSDIF_DataTransferOver) == kSDIF_DataTransferOver) {
            transferOver = true;
        }

        if (error == kStatus_Success) {
            transfered_words = SDIF_ReadDataPort(base, data, transfered_words);
        }

        /* clear interrupt status */
        SDIF_ClearInterruptStatus(base, status);
    }

    return error;
}

static status_t SDIF_WriteDataPortBlocking(SDIF_TYPE *base, sdif_data_t *data)
{
    uint32_t totalWords;
    uint32_t transfered_words = 0U;
    status_t error = kStatus_Success;
    uint32_t status;

    if (data->block_size % sizeof(uint32_t) != 0U) {
        data->block_size +=
            sizeof(uint32_t) - (data->block_size % sizeof(uint32_t)); /* make the block size as word-aligned */
    }

    totalWords = ((data->block_count * data->block_size) / sizeof(uint32_t));

    while ((transfered_words < totalWords) && (error == kStatus_Success)) {
        /* wait data transfer complete or reach RX watermark */
        do {
            status = SDIF_GetInterruptStatus(base);

            if (status & kSDIF_DataTransferError) {
                if (!(data->enable_ignore_error)) {
                    error = kStatus_Fail;
                }
            }
        } while ((status & kSDIF_WriteFIFORequest) == 0U);

        if (error == kStatus_Success) {
            transfered_words = SDIF_WriteDataPort(base, data, transfered_words);
        }

        /* clear interrupt status */
        SDIF_ClearInterruptStatus(base, status);
    }

    while ((SDIF_GetInterruptStatus(base) & kSDIF_DataTransferOver) != kSDIF_DataTransferOver) {
    }

    if (SDIF_GetInterruptStatus(base) & kSDIF_DataTransferError) {
        if (!(data->enable_ignore_error)) {
            error = kStatus_Fail;
        }
    }

    SDIF_ClearInterruptStatus(base, (kSDIF_DataTransferOver | kSDIF_DataTransferError));

    return error;
}

static bool SDIF_Reset(SDIF_TYPE *base, uint32_t mask, uint32_t timeout)
{
    /* reset through CTRL */
    base->CTRL |= mask;

    /* DMA software reset */
    if (mask & kSDIF_ResetDMAInterface) {
        /* disable DMA first then do DMA software reset */
        base->BMOD = (base->BMOD & (~SDIF_BMOD_DE_MASK)) | SDIF_BMOD_SWR_MASK;
    }

    /* check software DMA reset here for DMA reset also need to check this bit */
    while ((base->CTRL & mask) != 0U) {
        if (!timeout) {
            break;
        }

        timeout--;
    }

    return timeout ? true : false;
}

static status_t SDIF_TransferDataBlocking(SDIF_TYPE *base, sdif_data_t *data, bool isDMA)
{
    assert(NULL != data);

    uint32_t dmaStatus = 0U;
    status_t error = kStatus_Success;

    /* in DMA mode, only need to wait the complete flag and check error */
    if (isDMA) {
        do {
            dmaStatus = SDIF_GetInternalDMAStatus(base);

            if ((dmaStatus & kSDIF_DMAFatalBusError) == kSDIF_DMAFatalBusError) {
                SDIF_ClearInternalDMAStatus(base, kSDIF_DMAFatalBusError | kSDIF_AbnormalInterruptSummary);
                error = kStatus_SDIF_DMATransferFailWithFBE; /* in this condition,need reset */
            }

            /* Card error summary, include EBE,SBE,Data CRC,RTO,DRTO,Response error */
            if ((dmaStatus & kSDIF_DMACardErrorSummary) == kSDIF_DMACardErrorSummary) {
                SDIF_ClearInternalDMAStatus(base, kSDIF_DMACardErrorSummary | kSDIF_AbnormalInterruptSummary);

                if (!(data->enable_ignore_error)) {
                    error = kStatus_SDIF_DataTransferFail;
                }

                /* if error occur, then return */
                break;
            }
        } while ((dmaStatus & (kSDIF_DMATransFinishOneDescriptor | kSDIF_DMARecvFinishOneDescriptor)) == 0U);

        /* clear the corresponding status bit */
        SDIF_ClearInternalDMAStatus(base, (kSDIF_DMATransFinishOneDescriptor | kSDIF_DMARecvFinishOneDescriptor |
                                           kSDIF_NormalInterruptSummary));

        SDIF_ClearInterruptStatus(base, SDIF_GetInterruptStatus(base));
    } else {
        if (data->rx_date_buffer != NULL) {
            error = SDIF_ReadDataPortBlocking(base, data);
        } else {
            error = SDIF_WriteDataPortBlocking(base, data);
        }
    }

    return error;
}

static void SDIF_TransferHandleCommand(SDIF_TYPE *base, sdif_state_t *handle, uint32_t interrupt_flags)
{
    assert(handle->command);
    uint32_t sdif = SDIF_GetInstance(base);

    /* cmd buffer full, in this condition user need re-send the command */
    if (interrupt_flags & kSDIF_HardwareLockError) {
        if (handle->callback.command_reload) {
            handle->callback.command_reload(sdif, handle->user_data);
        }
    }
    /* transfer command done */
    else {
        if ((kSDIF_CommandDone & interrupt_flags) != 0U) {
            /* transfer error */
            if (interrupt_flags & (kSDIF_ResponseError | kSDIF_ResponseCRCError | kSDIF_ResponseTimeout)) {
                handle->callback.transfer_complete(sdif, handle, kStatus_SDIF_SendCmdFail, handle->user_data);
            } else {
                SDIF_ReadCommandResponse(base, handle->command);

                if (((handle->data) == NULL) && (handle->callback.transfer_complete)) {
                    handle->callback.transfer_complete(sdif, handle, kStatus_Success, handle->user_data);
                }
            }
        }
    }
}

static void SDIF_TransferHandleData(SDIF_TYPE *base, sdif_state_t *handle, uint32_t interrupt_flags)
{
    assert(handle->data);
    uint32_t sdif = SDIF_GetInstance(base);

    /* data starvation by host time out, software should read/write FIFO*/
    if (interrupt_flags & kSDIF_DataStarvationByHostTimeout) {
        if (handle->data->rx_date_buffer != NULL) {
            handle->transfered_words = SDIF_ReadDataPort(base, handle->data, handle->transfered_words);
        } else if (handle->data->tx_data_buffer != NULL) {
            handle->transfered_words = SDIF_WriteDataPort(base, handle->data, handle->transfered_words);
        } else {
            handle->callback.transfer_complete(sdif, handle, kStatus_SDIF_DataTransferFail, handle->user_data);
        }
    }
    /* data transfer fail */
    else if (interrupt_flags & kSDIF_DataTransferError) {
        if (!handle->data->enable_ignore_error) {
            handle->callback.transfer_complete(sdif, handle, kStatus_SDIF_DataTransferFail, handle->user_data);
        }
    }
    /* need fill data to FIFO */
    else if (interrupt_flags & kSDIF_WriteFIFORequest) {
        handle->transfered_words = SDIF_WriteDataPort(base, handle->data, handle->transfered_words);
    }
    /* need read data from FIFO */
    else if (interrupt_flags & kSDIF_ReadFIFORequest) {
        handle->transfered_words = SDIF_ReadDataPort(base, handle->data, handle->transfered_words);
    } else {
    }

    /* data transfer over */
    if (interrupt_flags & kSDIF_DataTransferOver) {
        while ((handle->data->rx_date_buffer != NULL) && ((base->STATUS & SDIF_STATUS_FIFO_COUNT_MASK) != 0U)) {
            handle->transfered_words = SDIF_ReadDataPort(base, handle->data, handle->transfered_words);
        }

        if (handle->data->tx_data_buffer) { // SDIF Write
            handle->callback.transfer_complete(sdif, handle, kStatus_Success, handle->user_data);
        }
    }
}

static void SDIF_TransferHandleDMA(SDIF_TYPE *base, sdif_state_t *handle, uint32_t interrupt_flags)
{
    uint32_t sdif = SDIF_GetInstance(base);

    if (interrupt_flags & kSDIF_DMAFatalBusError) {
        handle->callback.transfer_complete(sdif, handle, kStatus_SDIF_DMATransferFailWithFBE, handle->user_data);
    } else if (interrupt_flags & kSDIF_DMADescriptorUnavailable) {
        if (handle->callback.dma_des_unavailable) {
            handle->callback.dma_des_unavailable(sdif, handle->user_data);
        }
    } else if ((interrupt_flags & (kSDIF_AbnormalInterruptSummary | kSDIF_DMACardErrorSummary)) &&
               (!handle->data->enable_ignore_error)) {
        handle->callback.transfer_complete(sdif, handle, kStatus_SDIF_DataTransferFail, handle->user_data);
    }
    /* card normal summary */
    else {
        if (handle->data->rx_date_buffer) {
            handle->callback.transfer_complete(sdif, handle, kStatus_Success, handle->user_data);
        }
    }
}

static void SDIF_TransferHandleSDIOInterrupt(SDIF_TYPE *base, sdif_state_t *handle)
{

    uint32_t sdif = SDIF_GetInstance(base);
    if (handle->callback.sdif_interrupt != NULL) {
        handle->callback.sdif_interrupt(sdif, handle->user_data);
    }
}

static void SDIF_TransferHandleCardDetect(SDIF_TYPE *base, sdif_state_t *handle)
{
    uint32_t sdif = SDIF_GetInstance(base);

    if (SDIF_DetectCardInsert(base, false)) {
        if ((handle->callback.card_inserted) != NULL) {
            handle->callback.card_inserted(sdif, handle->user_data);
        }
    } else {
        if ((handle->callback.card_removed) != NULL) {
            handle->callback.card_removed(sdif, handle->user_data);
        }
    }
}

static void SDIF_TransferHandleIRQ(SDIF_TYPE *base, sdif_state_t *handle)
{
    assert(handle);

    uint32_t interrupt_flags, dma_interrupt_flags;

    interrupt_flags = SDIF_GetInterruptStatus(base);
    dma_interrupt_flags = SDIF_GetInternalDMAStatus(base);

    handle->interrupt_flags = interrupt_flags;
    handle->dma_interrupt_flags = dma_interrupt_flags;

    if ((interrupt_flags & kSDIF_CommandTransferStatus) != 0U) {
        SDIF_TransferHandleCommand(base, handle, (interrupt_flags & kSDIF_CommandTransferStatus));
    }

    if ((interrupt_flags & kSDIF_DataTransferStatus) != 0U) {
        SDIF_TransferHandleData(base, handle, (interrupt_flags & kSDIF_DataTransferStatus));
    }

    if (interrupt_flags & kSDIF_SDIOInterrupt) {
        SDIF_TransferHandleSDIOInterrupt(base, handle);
    }

    if (dma_interrupt_flags & kSDIF_DMAAllStatus) {
        SDIF_TransferHandleDMA(base, handle, dma_interrupt_flags);
    }

    if (interrupt_flags & kSDIF_CardDetect) {
        SDIF_TransferHandleCardDetect(base, handle);
    }

    SDIF_ClearInterruptStatus(base, interrupt_flags);
    SDIF_ClearInternalDMAStatus(base, dma_interrupt_flags);
}

status_t SDIF_SendCommand(SDIF_TYPE *base, sdif_command_t *cmd, uint32_t timeout)
{
    assert(NULL != cmd);

    base->CMDARG = cmd->argument;
    base->CMD = SDIF_CMD_CMD_INDEX(cmd->index) | SDIF_CMD_START_CMD_MASK | (cmd->flags & (~SDIF_CMD_CMD_INDEX_MASK));

    /* wait start_cmd bit auto clear within timeout */
    while ((base->CMD & SDIF_CMD_START_CMD_MASK) == SDIF_CMD_START_CMD_MASK) {
        if (!timeout) {
            break;
        }

        --timeout;
    }

    return timeout ? kStatus_Success : kStatus_Fail;
}

bool SDIF_SendCardActive(SDIF_TYPE *base, uint32_t timeout)
{
    bool enINT = false;
    sdif_command_t command = {.index = 0U, .argument = 0U};

    /* add for conflict with interrupt mode,close the interrupt temporary */
    if ((base->CTRL & SDIF_CTRL_INT_ENABLE_MASK) == SDIF_CTRL_INT_ENABLE_MASK) {
        enINT = true;
        base->CTRL &= ~SDIF_CTRL_INT_ENABLE_MASK;
    }

    command.flags = SDIF_CMD_SEND_INITIALIZATION_MASK;

    if (SDIF_SendCommand(base, &command, timeout) == kStatus_Fail) {
        return false;
    }

    /* wait command done */
    while ((SDIF_GetInterruptStatus(base) & kSDIF_CommandDone) != kSDIF_CommandDone) {
    }

    /* clear status */
    SDIF_ClearInterruptStatus(base, kSDIF_CommandDone);

    /* add for conflict with interrupt mode */
    if (enINT) {
        base->CTRL |= SDIF_CTRL_INT_ENABLE_MASK;
    }

    return true;
}


uint32_t SDIF_SetCardClock(SDIF_TYPE *base, uint32_t srcClock_Hz, uint32_t target_HZ)
{
    sdif_command_t cmd = {.index = 0U, .argument = 0U};
    uint32_t divider = 0U, targetFreq = target_HZ;

    /* if target freq bigger than the source clk, set the target_HZ to
     src clk, this interface can run up to 52MHZ with card */
    if (srcClock_Hz < targetFreq) {
        targetFreq = srcClock_Hz;
    }

    /* disable the clock first,need sync to CIU*/
    SDIF_EnableCardClock(base, false);
    /* update the clock register and wait the pre-transfer complete */
    cmd.flags = kSDIF_CmdUpdateClockRegisterOnly | kSDIF_WaitPretransfer_complete;
    SDIF_SendCommand(base, &cmd, SDIF_TIMEOUT_VALUE);

    /*calculate the divider*/
    if (targetFreq != srcClock_Hz) {
        divider = srcClock_Hz % (targetFreq * 2U) ? (srcClock_Hz / targetFreq / 2U + 1U) : (srcClock_Hz / targetFreq / 2U);
    }

    /* load the clock divider */
    base->CLKDIV = SDIF_CLKDIV_CLK_DIVIDER0(divider);
    /* update the divider to CIU */
    SDIF_SendCommand(base, &cmd, SDIF_TIMEOUT_VALUE);

    /* enable the card clock and sync to CIU */
    SDIF_EnableCardClock(base, true);
    SDIF_SendCommand(base, &cmd, SDIF_TIMEOUT_VALUE);

    /* return the actual card clock freq */

    return (divider != 0U) ? (srcClock_Hz / (divider * 2U)) : srcClock_Hz;
}

bool SDIF_AbortReadData(SDIF_TYPE *base, uint32_t timeout)
{
    /* assert this bit to reset the data machine to abort the read data */
    base->CTRL |= SDIF_CTRL_ABORT_READ_DATA_MASK;

    /* polling the bit self clear */
    while ((base->CTRL & SDIF_CTRL_ABORT_READ_DATA_MASK) == SDIF_CTRL_ABORT_READ_DATA_MASK) {
        if (!timeout) {
            break;
        }

        timeout--;
    }

    return base->CTRL & SDIF_CTRL_ABORT_READ_DATA_MASK ? false : true;
}

status_t SDIF_InternalDMAConfig(SDIF_TYPE *base, sdif_dma_config_t *config, const uint32_t *data, uint32_t dataSize)
{
    assert(NULL != config);
    assert(NULL != data);

    uint32_t dmaEntry = 0U, i, dmaBufferSize = 0U, dmaBuffer1Size = 0U;
    uint32_t *tempDMADesBuffer = config->dma_des_buffer_start_addr;
    const uint32_t *dataBuffer = data;
    sdif_dma_descriptor_t *descriptorPoniter = NULL;
    uint32_t maxDMABuffer = FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE * (config->mode);

    if ((((uint32_t)data % SDIF_INTERNAL_DMA_ADDR_ALIGN) != 0U) ||
        (((uint32_t)tempDMADesBuffer % SDIF_INTERNAL_DMA_ADDR_ALIGN) != 0U)) {
        return kStatus_SDIF_DMAAddrNotAlign;
    }

    /* check the read/write data size,must be a multiple of 4 */
    if (dataSize % sizeof(uint32_t) != 0U) {
        dataSize += sizeof(uint32_t) - (dataSize % sizeof(uint32_t));
    }

    /* FIXME: maybe not idle here */
    while (base->IDSTS >> SDIF_IDSTS_FSM_SHIFT) {}
    memset(tempDMADesBuffer, 0, config->dma_des_buffer_len * sizeof(uint32_t));

    /*config the bus mode*/
    if (config->enable_fix_burst_len) {
        base->BMOD |= SDIF_BMOD_FB_MASK;
    }

    /* calculate the dma descriptor entry due to DMA buffer size limit */
    /* if data size smaller than one descriptor buffer size */
    if (dataSize > maxDMABuffer) {
        dmaEntry = dataSize / maxDMABuffer + (dataSize % maxDMABuffer ? 1U : 0U);
    } else { /* need one dma descriptor */
        dmaEntry = 1U;
    }

    /* check the DMA descriptor buffer len one more time,it is user's responsibility to make sure the DMA descriptor
    table
    size is bigger enough to hold the transfer descriptor */
    if (config->dma_des_buffer_len * sizeof(uint32_t) < (dmaEntry * sizeof(sdif_dma_descriptor_t) + config->dma_dws_skip_len)) {
        return kStatus_SDIF_DescriptorBufferLenError;
    }

    switch (config->mode) {
        case SDIF_DUAL_DMA_MODE:
            base->BMOD |= SDIF_BMOD_DSL(config->dma_dws_skip_len); /* config the distance between the DMA descriptor */

            for (i = 0U; i < dmaEntry; i++) {
                if (dataSize > FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE) {
                    dmaBufferSize = FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE;
                    dataSize -= dmaBufferSize;
                    dmaBuffer1Size = dataSize > FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE ?
                                     FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE :
                                     dataSize;
                    dataSize -= dmaBuffer1Size;
                } else {
                    dmaBufferSize = dataSize;
                    dmaBuffer1Size = 0U;
                }

                descriptorPoniter = (sdif_dma_descriptor_t *)tempDMADesBuffer;

                if (i == 0U) {
                    descriptorPoniter->dmaDesAttribute = kSDIF_DMADescriptorDataBufferStart;
                }

                descriptorPoniter->dmaDesAttribute |= kSDIF_DMADescriptorOwnByDMA | kSDIF_DisableCompleteInterrupt;
                descriptorPoniter->dmaDataBufferSize =
                    SDIF_DMA_DESCRIPTOR_BUFFER1_SIZE(dmaBufferSize) | SDIF_DMA_DESCRIPTOR_BUFFER2_SIZE(dmaBuffer1Size);

                descriptorPoniter->dmaDataBufferAddr0 = ptr_cpu_to_dma((uint32_t *)dataBuffer);
                descriptorPoniter->dmaDataBufferAddr1 = ptr_cpu_to_dma((uint32_t *)(dataBuffer + dmaBufferSize / sizeof(uint32_t)));
                dataBuffer += (dmaBufferSize + dmaBuffer1Size) / sizeof(uint32_t);

                /* descriptor skip length */
                tempDMADesBuffer += config->dma_dws_skip_len + sizeof(sdif_dma_descriptor_t) / sizeof(uint32_t);
            }

            /* enable the completion interrupt when reach the last descriptor */
            descriptorPoniter->dmaDesAttribute &= ~kSDIF_DisableCompleteInterrupt;
            descriptorPoniter->dmaDesAttribute |= kSDIF_DMADescriptorDataBufferEnd | kSDIF_DMADescriptorEnd;
            break;

        case SDIF_CHAIN_DMA_MODE:
            for (i = 0U; i < dmaEntry; i++) {
                if (dataSize > FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE) {
                    dmaBufferSize = FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE;
                    dataSize -= FSL_FEATURE_SDIF_INTERNAL_DMA_MAX_BUFFER_SIZE;
                } else {
                    dmaBufferSize = dataSize;
                }

                descriptorPoniter = (sdif_dma_descriptor_t *)tempDMADesBuffer;

                if (i == 0U) {
                    descriptorPoniter->dmaDesAttribute = kSDIF_DMADescriptorDataBufferStart;
                }

                descriptorPoniter->dmaDesAttribute |=
                    kSDIF_DMADescriptorOwnByDMA | kSDIF_DMASecondAddrChained | kSDIF_DisableCompleteInterrupt;
                descriptorPoniter->dmaDataBufferSize =
                    SDIF_DMA_DESCRIPTOR_BUFFER1_SIZE(dmaBufferSize); /* use only buffer 1 for data buffer*/
                descriptorPoniter->dmaDataBufferAddr0 = ptr_cpu_to_dma((uint32_t *)dataBuffer);
                dataBuffer += dmaBufferSize / sizeof(uint32_t);
                tempDMADesBuffer +=
                    sizeof(sdif_dma_descriptor_t) / sizeof(uint32_t); /* calculate the next descriptor address */
                /* this descriptor buffer2 pointer to the next descriptor address */
                descriptorPoniter->dmaDataBufferAddr1 = ptr_cpu_to_dma(tempDMADesBuffer);
            }

            /* enable the completion interrupt when reach the last descriptor */
            descriptorPoniter->dmaDesAttribute &= ~kSDIF_DisableCompleteInterrupt;
            descriptorPoniter->dmaDesAttribute |= kSDIF_DMADescriptorDataBufferEnd;
            break;

        default:
            break;
    }

    // /* use internal DMA interface */
    // base->CTRL |= SDIF_CTRL_USE_INTERNAL_DMAC_MASK;
    // /* enable the internal SD/MMC DMA */
    // base->BMOD |= SDIF_BMOD_DE_MASK;
    // csi_dcache_clean_invalid();
    // /* enable DMA status check */
    // base->IDINTEN |= kSDIF_DMAAllStatus;
    // /* load DMA descriptor buffer address */
    // base->DBADDR = cpu_to_dma((uint32_t)config->dma_des_buffer_start_addr);

    return kStatus_Success;
}

void SDIF_Init(SDIF_TYPE *base, sdif_config_t *config)
{
    assert(NULL != config);

    /*config timeout register */
    base->TMOUT = ((base->TMOUT) & ~(SDIF_TMOUT_RESPONSE_TIMEOUT_MASK | SDIF_TMOUT_DATA_TIMEOUT_MASK)) |
                  SDIF_TMOUT_RESPONSE_TIMEOUT(config->response_timeout) | SDIF_TMOUT_DATA_TIMEOUT(config->data_timeout);

    /* config the card detect debounce clock count */
    base->DEBNCE = SDIF_DEBNCE_DEBOUNCE_COUNT(config->card_det_debounce_clock);

    /*config the watermark/burst transfer value */
    base->FIFOTH =
        SDIF_FIFOTH_TX_WMARK(SDIF_TX_WATERMARK) | SDIF_FIFOTH_RX_WMARK(SDIF_RX_WATERMARK) | SDIF_FIFOTH_DMA_MTS(1U);

    /* enable the interrupt status  */
    SDIF_EnableInterrupt(base, kSDIF_AllInterruptStatus);

    /* clear all interrupt/DMA status */
    SDIF_ClearInterruptStatus(base, kSDIF_AllInterruptStatus);
    SDIF_ClearInternalDMAStatus(base, kSDIF_DMAAllStatus);
}

status_t SDIF_TransferBlocking(SDIF_TYPE *base, sdif_dma_config_t *dmaConfig, sdif_transfer_t *transfer)
{
    assert(NULL != transfer);

    bool isDMA = false;
    sdif_data_t *data = transfer->data;
    status_t error = kStatus_Fail;

    /* config the transfer parameter */
    if (SDIF_TransferConfig(base, transfer) != kStatus_Success) {
        return kStatus_SDIF_InvalidArgument;
    }

    /* if need transfer data in dma mode, config the DMA descriptor first */
    if ((data != NULL) && (dmaConfig != NULL)) {
        /* use internal DMA mode to transfer between the card and host*/
        isDMA = true;

        if ((error = SDIF_InternalDMAConfig(base, dmaConfig, data->rx_date_buffer ? data->rx_date_buffer : data->tx_data_buffer,
                                            data->block_size * data->block_count)) ==
            kStatus_SDIF_DescriptorBufferLenError) {
            return kStatus_SDIF_DescriptorBufferLenError;
        }

        if (data->rx_date_buffer) {
            csi_dcache_clean_invalid_range(data->rx_date_buffer, data->block_size * data->block_count);
        } else {
            csi_dcache_clean_range((uint32_t *)data->tx_data_buffer, data->block_size * data->block_count);
        }

        csi_dcache_clean_invalid_range(dmaConfig->dma_des_buffer_start_addr, dmaConfig->dma_des_buffer_len * sizeof(uint32_t));

        // csi_dcache_clean_invalid();
        /* use internal DMA interface */
        base->CTRL |= SDIF_CTRL_USE_INTERNAL_DMAC_MASK;
        /* enable the internal SD/MMC DMA */
        base->BMOD |= SDIF_BMOD_DE_MASK;
        /* enable DMA status check */
        base->IDINTEN |= kSDIF_DMAAllStatus;
        /* load DMA descriptor buffer address */
        base->DBADDR = cpu_to_dma((uint32_t)dmaConfig->dma_des_buffer_start_addr);

        /* if DMA descriptor address or data buffer address not align with SDIF_INTERNAL_DMA_ADDR_ALIGN, switch to
        polling transfer mode, disable the internal DMA */
        if (error == kStatus_SDIF_DMAAddrNotAlign) {
            isDMA = false;
            SDIF_EnableInternalDMA(base, false);
            /* reset FIFO and clear RAW status for host transfer */
            SDIF_Reset(base, kSDIF_ResetFIFO, SDIF_TIMEOUT_VALUE);
            SDIF_ClearInterruptStatus(base, kSDIF_AllInterruptStatus);
        }
    }

    /* send command first */
    if (SDIF_SendCommand(base, transfer->command, SDIF_TIMEOUT_VALUE) != kStatus_Success) {
        return kStatus_SDIF_SyncCmdTimeout;
    }

    /* wait the command transfer done and check if error occurs */
    if (SDIF_WaitCommandDone(base, transfer->command) != kStatus_Success) {
        return kStatus_SDIF_SendCmdFail;
    }

    /* if use DMA transfer mode ,check the corresponding status bit */
    if (data != NULL) {
        /* handle data transfer */
        if (SDIF_TransferDataBlocking(base, data, isDMA) != kStatus_Success) {
            return kStatus_SDIF_DataTransferFail;
        }
    }

    return kStatus_Success;
}

status_t SDIF_TransferNonBlocking(SDIF_TYPE *base,
                                  sdif_state_t *handle,
                                  sdif_dma_config_t *dmaConfig,
                                  sdif_transfer_t *transfer)
{
    assert(NULL != transfer);

    sdif_data_t *data = transfer->data;
    status_t error = kStatus_Fail;

    /* save the data and command before transfer */
    handle->data = transfer->data;
    handle->command = transfer->command;
    handle->transfered_words = 0U;
    handle->interrupt_flags = 0U;
    handle->dma_interrupt_flags = 0U;

    /* config the transfer parameter */
    if (SDIF_TransferConfig(base, transfer) != kStatus_Success) {
        return kStatus_SDIF_InvalidArgument;
    }

    if ((data != NULL) && (dmaConfig != NULL)) {
        /* use internal DMA mode to transfer between the card and host*/
        if ((error = SDIF_InternalDMAConfig(base, dmaConfig, data->rx_date_buffer ? data->rx_date_buffer : data->tx_data_buffer,
                                            data->block_size * data->block_count)) ==
            kStatus_SDIF_DescriptorBufferLenError) {
            return kStatus_SDIF_DescriptorBufferLenError;
        }

        if (data->rx_date_buffer) {
            csi_dcache_clean_invalid_range(data->rx_date_buffer, data->block_size * data->block_count);
        } else {
            csi_dcache_clean_range((uint32_t *)data->tx_data_buffer, data->block_size * data->block_count);
        }

        csi_dcache_clean_invalid_range(dmaConfig->dma_des_buffer_start_addr, dmaConfig->dma_des_buffer_len * sizeof(uint32_t));

        // csi_dcache_clean_invalid();
        /* use internal DMA interface */
        base->CTRL |= SDIF_CTRL_USE_INTERNAL_DMAC_MASK;
        /* enable the internal SD/MMC DMA */
        base->BMOD |= SDIF_BMOD_DE_MASK;
        /* enable DMA status check */
        base->IDINTEN |= kSDIF_DMAAllStatus;
        /* load DMA descriptor buffer address */
        base->DBADDR = cpu_to_dma((uint32_t)dmaConfig->dma_des_buffer_start_addr);

        /* if DMA descriptor address or data buffer address not align with SDIF_INTERNAL_DMA_ADDR_ALIGN, switch to
        polling transfer mode, disable the internal DMA */
        if (error == kStatus_SDIF_DMAAddrNotAlign) {
            SDIF_EnableInternalDMA(base, false);
        }
    }

    /* send command first */
    if (SDIF_SendCommand(base, transfer->command, SDIF_TIMEOUT_VALUE) != kStatus_Success) {
        return kStatus_SDIF_SyncCmdTimeout;
    }

    return kStatus_Success;
}

static void SDIF_TransferCreateHandle(SDIF_TYPE *base,
                                      sdif_state_t *handle,
                                      sdif_callback_t *callback,
                                      void *user_data)
{
    assert(handle);
    assert(callback);

    /* reset the handle. */
    memset(handle, 0U, sizeof(*handle));

    /* Set the callback. */
    handle->callback.sdif_interrupt = callback->sdif_interrupt;
    handle->callback.dma_des_unavailable = callback->dma_des_unavailable;
    handle->callback.command_reload = callback->command_reload;
    handle->callback.transfer_complete = callback->transfer_complete;
    handle->callback.card_inserted = callback->card_inserted;
    handle->callback.card_removed = callback->card_removed;
    handle->user_data = user_data;

    /* Save the handle in global variables to support the double weak mechanism. */
    s_sdifState[SDIF_GetInstance(base)] = handle;

    /* save IRQ handler */
    s_sdifIsr = SDIF_TransferHandleIRQ;

    /* enable the global interrupt */
    SDIF_EnableGlobalInterrupt(base, true);

    SDIF_IRQEnable(s_sdifIRQ[SDIF_GetInstance(base)], s_sdifIRQEntry[SDIF_GetInstance(base)]);
}

void SDIF_GetCapability(SDIF_TYPE *base, sdif_capability_t *capability)
{
    assert(NULL != capability);

    capability->sd_version = SDIF_SUPPORT_SD_VERSION;
    capability->mmc_version = SDIF_SUPPORT_MMC_VERSION;
    capability->max_block_length = SDIF_BLKSIZ_BLOCK_SIZE_MASK;
    /* set the max block count = max byte count / max block size */
    capability->max_block_count = SDIF_BYTCNT_BYTE_COUNT_MASK / SDIF_BLKSIZ_BLOCK_SIZE_MASK;
    capability->flags = SDIF_SUPPORT_HIGH_SPEED | SDIF_SUPPORT_DMA_SPEED | SDIF_SUPPORT_USPEND_RESUME |
                        SDIF_SUPPORT_V330;// | SDIF_SUPPORT_4BIT | SDIF_SUPPORT_8BIT;
}


void SDIF_Deinit(SDIF_TYPE *base)
{
}

#if defined(SDIF0)
void SDIF0_DriverIRQHandler(void)
{
    assert(s_sdifState[0]);

    s_sdifIsr(SDIF0, s_sdifState[0]);
}
#endif

#if defined(SDIF1)
void SDIF1_DriverIRQHandler(void)
{
    assert(s_sdifState[1]);

    s_sdifIsr(SDIF1, s_sdifState[1]);
}
#endif

void SDIF_IRQEnable(int id, void *handle)
{
    silan_pic_request(id, 0, handle);
}

void SDIF_Binding(SDIF_TYPE **base, uint32_t sdif)
{
    *base = s_sdifBase[sdif];
}

/**
  \brief  Get sdif handle
  \param[in]   idx sdif index
  \return      sdif handle
*/
sdif_handle_t drv_sdif_get_handle(uint32_t idx)
{
    return (sdif_handle_t)s_sdifBase[idx];
}

/**
  \brief  Get sdif index
  \param[out]   handle  SDIF handle to operate.
  \return       sdif index
*/
uint32_t drv_sdif_get_idx(sdif_handle_t handle)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;
    return SDIF_GetInstance(base);
}

/**
  \brief  Initializes the SDIF according to the specified
  \param[in]   idx sdif index
  \param[in]   callback callback Structure pointer to contain all callback functions.
  \param[in]   user_data user_data Callback function parameter.
  \return sdif handle if success
*/
sdif_handle_t drv_sdif_initialize(uint32_t idx, sdif_callback_t *callback, void *user_data)
{
    //extern void pic_hdl_spdif(uint32_t irqid);
    //pic_port_request(PIC_IRQID_SPDIF, (void *)pic_hdl_spdif);

    SDIF_TYPE *base;

    if (idx == 0) {
        silan_sd_cclk_config(SD_CCLK_CLKSYS_DIV2, CLK_ON);
        base = SDIF0;
    } else if (idx == 1) {

        silan_sdio_cclk_config(SDIO_CCLK_CLKSYS_DIV2, CLK_ON);
        base = SDIF1;
    } else {
        return NULL;
    }

    SDIF_TransferCreateHandle(base, &g_sdifState[idx], callback, user_data);
    return (sdif_handle_t)base;
}

/**
  \brief       De-initialize SD Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  sdio handle to operate.
  \return      error code
*/
void drv_sdif_uninitialize(sdif_handle_t handle)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;
    SDIF_Deinit(base);
}

/**
  \brief  Initializes the SDIF config
  \param[in]   idx sdif index
  \param[in]   host  sdif config descriptor \ref sdif_config_t
*/
void drv_sdif_config(sdif_handle_t handle, sdif_config_t *config)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;
    SDIF_Init(base, config);
}

/**
  \brief       Get driver capabilities.
  \param[in]   handle  SDIF handle to operate.
  \param[out]  capability SDIF capability information \ref sdif_capabilities_t
 */
void drv_sdif_get_capabilities(sdif_handle_t handle, sdif_capability_t *capability)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;
    SDIF_GetCapability(base, capability);
}

/**
  \brief       control sdif power.
  \param[in]   handle  SDIF handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t drv_sdif_power_control(sdif_handle_t handle, csi_power_stat_e state)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;

    switch (state) {
        case DRV_POWER_OFF:
            SDIF_EnableCardPower(base, false);
            break;

        case DRV_POWER_LOW:
            break;

        case DRV_POWER_FULL:
            SDIF_EnableCardPower(base, true);
            break;

        case DRV_POWER_SUSPEND:
            break;

        default:
            break;
    }

    return  0;
}

/**
  \brief   send command to the card
  \param[in]  handle  SDIF handle to operate.
  \param[in]  cmd command configuration collection
  \param[in]  retries total number of tries
  \return  error code
 */
int32_t drv_sdif_send_command(sdif_handle_t handle, sdif_command_t *cmd, uint32_t retries)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;
    return SDIF_SendCommand(base, cmd, retries);
}

/**
  \brief  SDIF transfer function data/cmd in a non-blocking way
      this API should be use in interrupt mode, when use this API user
      must call drv_sdif_create_state first, all status check through
      interrupt
  \param[in] handle  SDIF handle to operate.
  \param[in] state sdif state information   \ref sdif_state_t
  \param[in] dma_config DMA config structure \ref sdif_dma_config_t
        This parameter can be config as:
        1. NULL
            In this condition, polling transfer mode is selected
        2. avaliable DMA config
            In this condition, DMA transfer mode is selected
  \param[in] transfer  sdif transfer configuration collection. \ref sdif_transfer_t
  \return error code
  */
int32_t drv_sdif_transfer(sdif_handle_t handle, sdif_dma_config_t *dma_config, sdif_transfer_t *transfer)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;
    uint32_t sdif;
    if (base == SDIF0) {
        sdif = 0;
    } else if (base == SDIF1) {
        sdif = 1;
    } else {
        return 0;
    }
    return SDIF_TransferNonBlocking(base, &g_sdifState[sdif], dma_config, transfer);
}

/**
 \brief    Poll-wait for the response to the last command to be ready.  This
   function should be called even after sending commands that have no
   response (such as CMD0) to make sure that the hardware is ready to
   receive the next command.
 \param[in]  handle  SDIF handle to operate.
 \param[in]  cmd : The command that was sent.  See 32-bit command definitions above.
*/
void drv_sdif_transfer_abort(sdif_handle_t handle)
{

}


/**
 \brief Called after change in Bus width has been selected (via ACMD6).  Most
    controllers will need to perform some special operations to work
    correctly in the new bus mode.
 \param[in]  handle  SDIF handle to operate.
 \param[in]  bus_width wide bus mode \ref sdif_bus_width_e.
*/
void drv_sdif_bus_width(sdif_handle_t handle, sdif_bus_width_e bus_width)
{
    SDIF_TYPE *base = (SDIF_TYPE *)handle;

    if (bus_width == 1) {
        bus_width = SDIF_CTYPE_CARD_WIDTH0_MASK;
    } else if (bus_width == 2) {
        bus_width = SDIF_CTYPE_CARD_WIDTH1_MASK;
    }

    SDIF_SetCardBusWidth(base, bus_width);
}

/**
 \brief Enable/disable SDIF clocking
 \param[in]  handle  SDIF handle to operate.
 \param[in]  target_hz card bus clock frequency united in Hz.
 \return code error
*/
uint32_t drv_sdif_set_clock(sdif_handle_t handle, uint32_t target_hz)
{
    uint32_t source_clock_hz;
    SDIF_TYPE *base = (SDIF_TYPE *)handle;
    if (base == SDIF0) {
        source_clock_hz = silan_get_sd_cclk();
    } else if (base == SDIF1) {
        source_clock_hz = silan_get_sdio_cclk();
    } else {
        return 0;
    }
    return SDIF_SetCardClock(base, source_clock_hz, target_hz);
}

