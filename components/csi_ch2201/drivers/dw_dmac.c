/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
* @file     dw_dmac.c
* @brief    CSI Source File for DMAC Driver
* @version  V1.0
* @date     02. June 2017
******************************************************************************/
#include <stdbool.h>
#include <string.h>
#include <csi_config.h>
#include "dw_dmac.h"
#include "csi_core.h"
#include "drv/dmac.h"
#include "drv/irq.h"
#include "soc.h"
#include <string.h>

#define ERR_DMA(errno) (CSI_DRV_ERRNO_DMA_BASE | errno)

typedef struct {
#ifdef CONFIG_LPM
    uint8_t dma_power_status;
#endif
    uint32_t base;
    uint32_t control_base;
    uint32_t irq;
    dma_event_cb_t cb_event;
    uint8_t ch_num;
    uint8_t ch_opened;
    dma_status_e status;
    uint32_t src_tw;
    uint32_t dst_tw;
} dw_dma_priv_t;

extern int32_t target_get_dmac(uint32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static dw_dma_priv_t dma_instance[CONFIG_DMA_CHANNEL_NUM];

static const dma_capabilities_t dma_capabilities = {
    .unalign_addr = 1,          ///< support for unalign address transfer when memory is source
};

#define readl(addr) \
    *((volatile uint32_t *)(addr))

#define writel(b, addr) \
    *((volatile uint32_t *)(addr)) = b

static void dw_dma_set_channel(uint32_t addr, uint8_t ch, uint32_t source, uint32_t dest, uint32_t size)
{
    writel(size, addr + (ch % 2) * 0x58 + DMA_REG_CTRLbx);
    writel(source, addr + (ch % 2) * 0x58 + DMA_REG_SARx);
    writel(dest, addr + (ch % 2) * 0x58 + DMA_REG_DARx);
}

static int32_t dw_dma_set_transfertype(uint32_t addr, uint8_t ch, dma_trans_type_e transtype)
{
    if (transtype >= DMA_PERH2PERH) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x300000);
    value |= transtype << 20;
    writel(value, addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);

    return 0;
}

static int32_t dw_dma_set_addrinc(uint32_t addr, uint8_t ch, enum_addr_state_e src_addrinc, enum_addr_state_e dst_addrinc)
{
    if ((src_addrinc != DMA_ADDR_INCREMENT && src_addrinc != DMA_ADDR_DECREMENT && src_addrinc != DMA_ADDR_NOCHANGE) ||
        (dst_addrinc != DMA_ADDR_INCREMENT && dst_addrinc != DMA_ADDR_DECREMENT && dst_addrinc != DMA_ADDR_NOCHANGE)) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x780);
    value |= (src_addrinc << 9);
    value |= (dst_addrinc << 7);
    writel(value, addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);

    return 0;
}

static int32_t dw_dma_set_transferwidth(uint32_t addr, uint8_t ch, dma_datawidth_e src_width, dma_datawidth_e dst_width)
{
    if ((src_width != DMA_DATAWIDTH_SIZE8 && src_width != DMA_DATAWIDTH_SIZE16 && src_width != DMA_DATAWIDTH_SIZE32) ||
        (dst_width != DMA_DATAWIDTH_SIZE8 && dst_width != DMA_DATAWIDTH_SIZE16 && dst_width != DMA_DATAWIDTH_SIZE32)) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x7e);

    if (src_width == DMA_DATAWIDTH_SIZE32) {
        value |= (src_width - 2) << 4;
    } else {
        value |= (src_width - 1) << 4;
    }

    if (dst_width == DMA_DATAWIDTH_SIZE32) {
        value |= (dst_width - 2) << 1;
    } else {
        value |= (dst_width - 1) << 1;
    }

    writel(value, addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);

    return 0;
}

static void dw_dma_set_burstlength(uint32_t addr, uint8_t ch, uint8_t burstlength)
{
    uint32_t value = readl(addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x1f800);
    value |= burstlength << 11 | burstlength << 14;
    writel(value, addr + (ch % 2) * 0x58 + DMA_REG_CTRLax);
}

/**
  \brief       Set software or hardware handshaking.
  \param[in]   addr pointer to dma register.
  \return      error code
*/
static void dw_dma_set_handshaking(uint32_t addr, uint8_t ch, dma_handshaking_select_e src_handshaking, dma_handshaking_select_e dst_handshaking)
{
    uint32_t value = readl(addr + (ch % 2) * 0x58 + DMA_REG_CFGax);
    value &= ~(0xc00);
    value |= (src_handshaking << 11  | dst_handshaking << 10);
    writel(value, addr + (ch % 2) * 0x58 + DMA_REG_CFGax);
}

static int32_t dw_dma_assign_hdhs_interface(uint32_t addr, uint8_t ch, dwenum_dma_device_e src_device, dwenum_dma_device_e dst_device)
{
    if (src_device < 0 || src_device >= DWENUM_DMA_MEMORY || dst_device < 0 || dst_device >= DWENUM_DMA_MEMORY) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % 2) * 0x58 + DMA_REG_CFGbx);
    value &= ~(0x7f80);
    value |= (src_device << 7 | dst_device << 11);
    writel(value, addr + (ch % 2) * 0x58 + DMA_REG_CFGbx);

    return 0;
}


void dw_dmac_irqhandler(int32_t idx)
{
    dw_dma_priv_t *dma_priv = &dma_instance[idx];
    uint32_t addr = dma_priv->base;

    if (addr == 0) {
        dma_priv = &dma_instance[idx + 1];
        addr = dma_priv->base;
    }

    /*
     * StatusInt_temp contain the information that which types of interrupr are
     * requested.
     */
    int32_t count = 0;
    uint32_t temp = 0;

    temp = readl(addr + DMA_REG_StatusTfr);

    if (temp) {
        for (count = 0; count < CONFIG_DMA_CHANNEL_NUM / 2; count++) {
            if ((temp & (1U << count)) != 0) {
                writel(1U << count, addr + DMA_REG_ClearTfr);
                temp &= ~(1U << count);

                if (count == 0) {
                    dma_priv = &dma_instance[idx];
                } else {
                    dma_priv = &dma_instance[idx + 1];
                }

                dma_priv->status = DMA_STATE_DONE;

                if (dma_priv->cb_event) {
                    dma_priv->cb_event(count + idx * 2, DMA_EVENT_TRANSFER_DONE, NULL);
                }

            }
        }

        return;
    }

    temp = readl(addr + DMA_REG_StatusErr);

    if (temp) {
        for (count = 0; count < CONFIG_DMA_CHANNEL_NUM / 2; count++) {
            if (temp == (1U << count)) {
                break;
            }
        }

        temp &= (1U << count);

        if (count == 0) {
            dma_priv = &dma_instance[idx];
        } else {
            dma_priv = &dma_instance[idx + 1];
        }

        dma_priv->status = DMA_STATE_ERROR;
        writel(temp, addr + DMA_REG_ClearTfr);

        if (dma_priv->cb_event) {
            dma_priv->cb_event(count + idx * 2, DMA_EVENT_TRANSFER_ERROR, NULL);
        }
    }
}

#ifdef CONFIG_LPM
static void manage_clock(void *handle, uint8_t enable)
{
    dw_dma_priv_t *dma_priv = handle;

    if (dma_priv == &dma_instance[0] || dma_priv == &dma_instance[1]) {
        drv_clock_manager_config(CLOCK_MANAGER_DMAC0, enable);
    } else {
        drv_clock_manager_config(CLOCK_MANAGER_DMAC1, enable);
    }
}
#endif

/**
  \brief       Get driver capabilities.
  \param[in]   idx    dmac index.
  \return      \ref dma_capabilities_t
*/
dma_capabilities_t csi_dma_get_capabilities(int32_t idx)
{
    if (idx > (CONFIG_DMA_CHANNEL_NUM - 1)) {
        dma_capabilities_t ret;
        memset(&ret, 0, sizeof(dma_capabilities_t));
        return ret;
    }

    return dma_capabilities;
}

/**
  \brief     get one free dma channel
  \return    channel num.if -1 alloc channle error
 */
int32_t csi_dma_alloc_channel(void)
{
    uint8_t ch_num = 0;

    // alloc a free channal
    uint32_t result = csi_irq_save();

    for (ch_num = 0; ch_num < CONFIG_DMA_CHANNEL_NUM; ch_num++) {
        if (dma_instance[ch_num].ch_opened != 0x1) {
            dma_instance[ch_num].ch_opened = 1;
            break;
        }
    }

    csi_irq_restore(result);

    if (ch_num >= CONFIG_DMA_CHANNEL_NUM) {
        return -1;
    }

    dma_instance[ch_num].ch_num = ch_num;
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;

    target_get_dmac(ch_num, &base, &irq, &handler);

    dw_dma_priv_t *dma_priv = &dma_instance[ch_num];

    dma_priv->base = base;
    dma_priv->irq  = irq;

    if (!(dma_instance[ch_num / 2 ].ch_opened == 1 && dma_instance[ch_num / 2 + 1].ch_opened == 1)) {
        drv_irq_register(dma_priv->irq, handler);
        drv_irq_enable(dma_priv->irq);

#ifdef CONFIG_LPM
        csi_dma_power_control(ch_num, DRV_POWER_FULL);
#endif
    }

    uint32_t addr = dma_priv->base;
    writel((1 << ch_num), addr + DMA_REG_ClearTfr);
    writel((1 << ch_num), addr + DMA_REG_ClearBlock);
    writel((1 << ch_num), addr + DMA_REG_ClearErr);
    writel((1 << ch_num), addr + DMA_REG_ClearSrcTran);
    writel((1 << ch_num), addr + DMA_REG_ClearDstTran);

    uint32_t value = 1 << ch_num | (1 << (ch_num + 8));
    writel(value, addr + DMA_REG_MaskTfr);
    writel(value, addr + DMA_REG_MaskErr);

    dma_priv->status = DMA_STATE_READY;

    return ch_num;
}

/**
  \brief       control dma power.
  \param[in]   ch      dma channel num
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_dma_power_control(int32_t ch, csi_power_stat_e state)
{
#ifdef CONFIG_LPM
    dw_dma_priv_t *handle = &dma_instance[ch];
    power_cb_t callback = {
        .wakeup = NULL,
        .sleep = NULL,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_DMA(DRV_ERROR_UNSUPPORTED);
#endif

}

/**
  \brief        release dma channel and related resources
  \param[in]    ch dma channel num
  \return       error code
*/
void csi_dma_release_channel(int32_t ch)
{
    if (ch >= CONFIG_DMA_CHANNEL_NUM || ch < 0) {
        return;
    }

    dw_dma_priv_t *dma_priv = &dma_instance[ch];

    uint32_t addr = dma_priv->base;
    dma_priv->status = DMA_STATE_FREE;
    dma_priv->ch_opened = 0;

    writel((1 << ch), addr + DMA_REG_ClearTfr);
    writel((1 << ch), addr + DMA_REG_ClearBlock);
    writel((1 << ch), addr + DMA_REG_ClearErr);
    writel((1 << ch), addr + DMA_REG_ClearSrcTran);
    writel((1 << ch), addr + DMA_REG_ClearDstTran);

    uint32_t value = (1 << (ch + 8));
    writel(value, addr + DMA_REG_MaskTfr);
    writel(value, addr + DMA_REG_MaskErr);

    if (dma_instance[ch / 2 ].ch_opened == 0 && dma_instance[ch / 2 + 1].ch_opened == 0) {
        drv_irq_disable(dma_priv->irq);
        drv_irq_unregister(dma_priv->irq);
#ifdef CONFIG_LPM
        csi_dma_power_control(ch, DRV_POWER_OFF);
#endif
    }
}

/**
  \brief        config dma channel
  \param[in]    ch          dma channel num
  \param[in]    config      dma channel transfer configure
  \param[in]    cb_event    Pointer to \ref dma_event_cb_t
  \return       error code
*/
int32_t csi_dma_config_channel(int32_t ch, dma_config_t *config, dma_event_cb_t cb_event, void *cb_arg)
{
    if (ch >= CONFIG_DMA_CHANNEL_NUM || ch < 0 || config == NULL) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    dw_dma_priv_t *dma_priv = &dma_instance[ch];
    dma_priv->cb_event = cb_event;

    if (dma_priv->ch_opened == 0) {
        return ERR_DMA(DRV_ERROR_BUSY);
    }

    uint32_t addr = dma_priv->base;

    /* Initializes corresponding channel registers */


    int32_t ret = dw_dma_set_transferwidth(addr, dma_priv->ch_num, config->src_tw, config->dst_tw);

    if (ret < 0) {
        return ret;
    }

    dma_priv->dst_tw = config->dst_tw;
    dma_priv->src_tw = config->src_tw;

//    int32_t grouplen = ((length * config->src_tw / config->dst_tw) - 1) % 16;

    ret = dw_dma_set_transfertype(addr, dma_priv->ch_num, config->type);

    if (ret < 0) {
        return ret;
    }

    if (config->type == DMA_MEM2MEM) {
        dw_dma_set_handshaking(addr, dma_priv->ch_num, DMA_HANDSHAKING_SOFTWARE, DMA_HANDSHAKING_SOFTWARE);
        ret = dw_dma_set_addrinc(addr, dma_priv->ch_num, config->src_inc, config->dst_inc);

        if (ret < 0) {
            return ret;
        }

    } else if (config->type == DMA_MEM2PERH) {
        dw_dma_set_handshaking(addr, dma_priv->ch_num, DMA_HANDSHAKING_SOFTWARE, DMA_HANDSHAKING_HARDWARE);
        ret = dw_dma_set_addrinc(addr, dma_priv->ch_num, config->src_inc, config->dst_inc);

        if (ret < 0) {
            return ret;
        }

        ret = dw_dma_assign_hdhs_interface(addr, dma_priv->ch_num, config->hs_if, config->hs_if);

        if (ret < 0) {
            return ret;
        }

    } else if (config->type == DMA_PERH2MEM) {
        dw_dma_set_handshaking(addr, dma_priv->ch_num, DMA_HANDSHAKING_HARDWARE, DMA_HANDSHAKING_SOFTWARE);
        ret = dw_dma_set_addrinc(addr, dma_priv->ch_num, config->src_inc, config->dst_inc);

        if (ret < 0) {
            return ret;
        }

        ret = dw_dma_assign_hdhs_interface(addr, dma_priv->ch_num, config->hs_if, config->hs_if);

        if (ret < 0) {
            return ret;
        }

    }

    writel(0x1, addr + DMA_REG_Cfg);
    dma_priv->status = DMA_STATE_READY;

    return 0;
}

/**
  \brief       start generate dma channel signal.
  \param[in]   ch          dma channel num
  \param[in]   psrcaddr    dma transfer source address
  \param[in]   pdstaddr    dma transfer destination address
  \param[in]   length      dma transfer length (unit: bytes).
*/
void csi_dma_start(int32_t ch, void *psrcaddr,
                   void *pdstaddr, uint32_t length)
{
    if (ch >= CONFIG_DMA_CHANNEL_NUM || ch < 0 || psrcaddr == NULL || pdstaddr == NULL) {
        return;
    }

    dw_dma_priv_t *dma_priv = &dma_instance[ch];

    dma_priv->status = DMA_STATE_BUSY;
    uint32_t addr = dma_priv->base;

    if ((length * dma_priv->src_tw) % dma_priv->dst_tw != 0) {
        return;
    }

    uint8_t i;

    for (i = 2; i > 0; i--) {
        if (!((length * dma_priv->src_tw / dma_priv->dst_tw) % (2 << i))) {
            break;
        }
    }

    int32_t grouplen = i;

    if (i == 0) {
        grouplen = 0;
    }

    dw_dma_set_burstlength(addr, dma_priv->ch_num, grouplen);
    dw_dma_set_channel(addr, dma_priv->ch_num, (uint32_t)psrcaddr, (uint32_t)pdstaddr, length);

    // interrupt enable
    uint32_t value = readl(addr + (dma_priv->ch_num % 2) * 0x58 + DMA_REG_CTRLax);
    value |= CK_DMA_INT_EN;
    writel(value, addr + (dma_priv->ch_num % 2) * 0x58 + DMA_REG_CTRLax);

    value = readl(addr + DMA_REG_ChEn);
    value |= (CK_DMA_CH_EN << (8 + (dma_priv->ch_num % 2))) | (CK_DMA_CH_EN << (dma_priv->ch_num % 2));
    writel(value, addr + DMA_REG_ChEn);
}

/**
  \brief       Stop generate dma channel signal.
  \param[in]   ch     dma channel num
*/
void csi_dma_stop(int32_t ch)
{
    if (ch >= CONFIG_DMA_CHANNEL_NUM || ch < 0) {
        return;
    }

    dw_dma_priv_t *dma_priv = &dma_instance[ch];

    dma_priv->status = DMA_STATE_DONE;

    uint32_t addr = dma_priv->base;

    uint32_t value = readl(addr + DMA_REG_ChEn);
    value |= (CK_DMA_CH_EN << (8 + (dma_priv->ch_num % 2)));
    value &= ~(CK_DMA_CH_EN << (dma_priv->ch_num % 2));
    writel(value, addr + DMA_REG_ChEn);
    // interrupt disable
    value = readl(addr + (dma_priv->ch_num % 2) * 0x58 + DMA_REG_CTRLax);
    value &= ~CK_DMA_INT_EN;
    writel(value, addr + (dma_priv->ch_num % 2) * 0x58 + DMA_REG_CTRLax);
}

/**
  \brief       Get DMA channel status.
  \param[in]   ch  dma channel num
  \return      DMA status \ref dma_status_e
*/
dma_status_e csi_dma_get_status(int32_t ch)
{
    if (ch >= CONFIG_DMA_CHANNEL_NUM || ch < 0) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    dw_dma_priv_t *dma_priv = &dma_instance[ch];

    return dma_priv->status;
}

