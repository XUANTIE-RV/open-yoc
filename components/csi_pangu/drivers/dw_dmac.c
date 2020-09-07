/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
* @file     dw_dmac.c
* @brief    CSI Source File for DMAC Driver
* @version  V1.0
* @date     02. June 2017
******************************************************************************/

#include <stdbool.h>
#include <string.h>
#include <dw_dmac.h>
#include <csi_core.h>
#include <drv/irq.h>
#include <drv/dmac.h>
#include <soc.h>

#define ERR_DMA(errno) (CSI_DRV_ERRNO_DMA_BASE | errno)

extern int32_t target_get_dmac(uint32_t idx, uint32_t *base, uint32_t *irq, void **handler);
extern uint8_t drv_dma_get_ahb_master_idx(uint32_t addr);
extern int32_t dma_mux_channel_alloc(dwenum_dma_device_e func);
extern int32_t dma_mux_channel_release(dwenum_dma_device_e func);

dw_dma_priv_t dma_instance[CONFIG_DMA_CHANNEL_NUM];

static const dma_capabilities_t dma_capabilities = {
    .unalign_addr = 1,          ///< support for unalign address transfer when memory is source
};

#define readl(addr) \
    *((volatile uint32_t *)(addr))

#define writel(b, addr) \
    *((volatile uint32_t *)(addr)) = b

static void dw_dma_set_channel(uint32_t addr, uint8_t ch, uint32_t source, uint32_t dest, uint32_t size, uint32_t prer_dmac_ch_num)
{
    writel(size, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLbx);
    writel(source, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_SARx);
    writel(dest, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_DARx);
}

static int32_t dw_dma_set_transfertype(uint32_t addr, uint8_t ch, dma_trans_type_e transtype, uint32_t prer_dmac_ch_num)
{
    if (transtype >= DMA_PERH2PERH) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x300000);
    value |= transtype << 20;
    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);

    return 0;
}

static int32_t dw_dma_set_addrinc(uint32_t addr, uint8_t ch, enum_addr_state_e src_addrinc, enum_addr_state_e dst_addrinc, uint32_t prer_dmac_ch_num)
{
    if ((src_addrinc != DMA_ADDR_INCREMENT && src_addrinc != DMA_ADDR_DECREMENT && src_addrinc != DMA_ADDR_NOCHANGE) ||
        (dst_addrinc != DMA_ADDR_INCREMENT && dst_addrinc != DMA_ADDR_DECREMENT && dst_addrinc != DMA_ADDR_NOCHANGE)) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x780);
    value |= (src_addrinc << 9);
    value |= (dst_addrinc << 7);
    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);

    return 0;
}

static int32_t dw_dma_set_transferwidth(uint32_t addr, uint8_t ch, dma_datawidth_e src_width, dma_datawidth_e dst_width, uint32_t prer_dmac_ch_num)
{
    if ((src_width != DMA_DATAWIDTH_SIZE8 && src_width != DMA_DATAWIDTH_SIZE16 && src_width != DMA_DATAWIDTH_SIZE32) ||
        (dst_width != DMA_DATAWIDTH_SIZE8 && dst_width != DMA_DATAWIDTH_SIZE16 && dst_width != DMA_DATAWIDTH_SIZE32)) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
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

    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);

    return 0;
}

static void dw_dma_set_burstlength(uint32_t addr, uint8_t ch, uint8_t burstlength, uint32_t prer_dmac_ch_num)
{
    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x1f800);
    value |= burstlength << 11 | burstlength << 14;
    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
}

static void dw_dma_set_master(uint32_t addr, uint8_t ch, uint8_t src_idx, uint8_t dst_idx, uint32_t prer_dmac_ch_num)
{
    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    value &= ~(0x7800000);
    value |= src_idx << 25 | dst_idx << 23;
    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
}

/**
  \brief       Set software or hardware handshaking.
  \param[in]   addr pointer to dma register.
  \return      error code
*/
static void dw_dma_set_handshaking(uint32_t addr, uint8_t ch, dma_handshaking_select_e src_handshaking, dma_handshaking_select_e dst_handshaking, uint32_t prer_dmac_ch_num)
{
    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CFGax);
    value &= ~(0xc00);
    value |= (src_handshaking << 11  | dst_handshaking << 10);
    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CFGax);
}

static int32_t dw_dma_assign_hdhs_interface(uint32_t addr, uint8_t ch, dwenum_dma_device_e src_device, dwenum_dma_device_e dst_device, uint32_t prer_dmac_ch_num)
{
    if (src_device < 0 || src_device >= DWENUM_DMA_MEMORY || dst_device < 0 || dst_device >= DWENUM_DMA_MEMORY) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CFGbx);
    value &= ~(0x7f80);
    value |= (src_device << 7 | dst_device << 11);
    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CFGbx);

    return 0;
}

static void dw_dma_addr_reload_enable(uint32_t addr, uint8_t ch, uint32_t src_reload_en, uint32_t dst_reload_en, uint32_t prer_dmac_ch_num)
{
    uint32_t value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CFGax);
    value &= ~(0x01 << 30 | 0x01 << 31);

    if (src_reload_en) {
        value |= 0x01 << 30;
    }

    if (dst_reload_en) {
        value |= 0x01 << 31;
    }

    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CFGax);
}

void dw_dmac_irqhandler(int32_t idx)
{
    uint32_t prer_dmac_ch_num = 0;
    uint32_t addr;
    uint32_t ch_offet = 0; //used for dma1 channel
    dw_dma_priv_t *dma_instance_p = NULL;

    if (idx == 0) {
        prer_dmac_ch_num = CONFIG_PER_DMAC0_CHANNEL_NUM;
        /*get dma0 base*/
        target_get_dmac(0, &addr, NULL, NULL);
        dma_instance_p = &dma_instance[0];
    } else {
        prer_dmac_ch_num = CONFIG_PER_DMAC1_CHANNEL_NUM;
        /*get dma1 base*/
        target_get_dmac(CONFIG_PER_DMAC0_CHANNEL_NUM, &addr, NULL, NULL);
        dma_instance_p = &dma_instance[CONFIG_PER_DMAC0_CHANNEL_NUM];
        ch_offet = CONFIG_PER_DMAC0_CHANNEL_NUM;
    }

    /*
     * StatusInt_temp contain the information that which types of interrupr are
     * requested.
     */
    int32_t count = 0;
    dw_dma_priv_t *dma_priv = NULL;
    uint32_t status_tfr = readl(addr + DMA_REG_StatusTfr);
    uint32_t status_block = readl(addr + DMA_REG_StatusBlock);
    uint32_t status_err = readl(addr + DMA_REG_StatusErr);

    writel(status_tfr, addr + DMA_REG_ClearTfr);
    writel(status_block, addr + DMA_REG_ClearBlock);
    writel(status_err, addr + DMA_REG_ClearErr);


    if (status_tfr) {
        for (count = 0; count < prer_dmac_ch_num; count++) {
            if ((status_tfr & (1U << count)) != 0) {
                status_tfr &= ~(1U << count);

                dma_priv = &dma_instance_p[count];

                dma_priv->status = DMA_STATE_DONE;

                if (dma_priv->cb_event) {
                    dma_priv->cb_event(count + ch_offet,
                                       DMA_EVENT_TRANSFER_DONE,
                                       dma_priv->cb_arg);
                }

            }
        }

        return;
    }

    if (status_block) {
        for (count = 0; count < prer_dmac_ch_num; count++) {
            if ((status_block & (1U << count)) != 0) {
                status_block &= ~(1U << count);

                dma_priv = &dma_instance_p[count];

                dma_priv->status = DMA_STATE_BUSY;

                if (dma_priv->cb_event) {
                    dma_priv->cb_event(count + ch_offet,
                                       DMA_EVENT_TRANSFER_MODE_DONE,
                                       dma_priv->cb_arg);
                }

            }
        }

        return;
    }

    if (status_err) {
        for (count = 0; count < prer_dmac_ch_num; count++) {
            if (status_err & (1U << count)) {
                status_err &= ~(1U << count);
                dma_priv = &dma_instance_p[count];

                dma_priv->status = DMA_STATE_ERROR;

                if (dma_priv->cb_event) {
                    dma_priv->cb_event(count + ch_offet,
                                       DMA_EVENT_TRANSFER_ERROR,
                                       dma_priv->cb_arg);
                }
            }
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
  \param[in] ch channel num. if -1 then allocate a free channal in this dma
                             else allocate a fix channal
  \return    dma channel num. if -1 alloc channle error
 */
int32_t csi_dma_alloc_channel_ex(int32_t ch)
{
    uint8_t ch_num = 0;

    // alloc a free channal
    uint32_t result = csi_irq_save();

    if (ch == -1) {
        for (ch_num = 0; ch_num < CONFIG_DMA_CHANNEL_NUM; ch_num++) {
            if (dma_instance[ch_num].ch_opened != 0x1) {
                dma_instance[ch_num].ch_opened = 1;
                break;
            }
        }
    } else {
        ch_num = ch;

        if (dma_instance[ch_num].ch_opened != 0x1) {
            dma_instance[ch_num].ch_opened = 1;
        } else {
            csi_irq_restore(result);
            return -1;
        }

    }

    csi_irq_restore(result);

    if (ch_num >= CONFIG_DMA_CHANNEL_NUM) {
        return -1;
    }

    if (ch_num < CONFIG_PER_DMAC0_CHANNEL_NUM) {
        dma_instance[ch_num].prer_dmac_ch_num = CONFIG_PER_DMAC0_CHANNEL_NUM;
    } else {
        dma_instance[ch_num].prer_dmac_ch_num = CONFIG_PER_DMAC1_CHANNEL_NUM;
    }

    uint8_t ch_num_record = ch_num;
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;

    int ret = target_get_dmac(ch_num, &base, &irq, &handler);

    if (ret == -1) {
        return -1;
    }

    dma_instance[ch_num].ch_num = ch_num;
    dma_instance[ch_num].ch_mux_func = -1;

    dw_dma_priv_t *dma_priv = &dma_instance[ch_num];
    dw_dma_priv_t *dma_instance_p = dma_instance;

    if (dma_instance[ch_num].ch_num >= CONFIG_PER_DMAC0_CHANNEL_NUM) {
        dma_instance[ch_num].ch_num -= CONFIG_PER_DMAC0_CHANNEL_NUM;
        dma_instance_p = &dma_instance[ch_num];
        ch_num -= CONFIG_PER_DMAC0_CHANNEL_NUM;
    }

    dma_priv->base = base;
    dma_priv->irq  = irq;

    uint32_t i;
    uint32_t ch_open_num = 0;
    uint32_t prer_dmac_ch_num = dma_priv->prer_dmac_ch_num;

    for (i = ch_num / prer_dmac_ch_num; i < ((ch_num / prer_dmac_ch_num + 1) * prer_dmac_ch_num - 1); i++) {
        if (dma_instance_p[i].ch_opened == 0x1) {
            ch_open_num++;
        }
    }

    if (ch_open_num >= 1) {
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
    writel(value, addr + DMA_REG_MaskBlock);
    writel(value, addr + DMA_REG_MaskErr);

    dma_priv->status = DMA_STATE_READY;

    return ch_num_record;
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

    if (ch_num < CONFIG_PER_DMAC0_CHANNEL_NUM) {
        dma_instance[ch_num].prer_dmac_ch_num = CONFIG_PER_DMAC0_CHANNEL_NUM;
    } else {
        dma_instance[ch_num].prer_dmac_ch_num = CONFIG_PER_DMAC1_CHANNEL_NUM;
    }

    uint8_t ch_num_record = ch_num;
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;

    int ret = target_get_dmac(ch_num, &base, &irq, &handler);

    if (ret == -1) {
        return -1;
    }

    dma_instance[ch_num].ch_num = ch_num;
    dma_instance[ch_num].ch_mux_func = -1;


    dw_dma_priv_t *dma_priv = &dma_instance[ch_num];
    dw_dma_priv_t *dma_instance_p = dma_instance;

    if (dma_instance[ch_num].ch_num >= CONFIG_PER_DMAC0_CHANNEL_NUM) {
        dma_instance[ch_num].ch_num -= CONFIG_PER_DMAC0_CHANNEL_NUM;
        dma_instance_p = &dma_instance[ch_num];
        ch_num -= CONFIG_PER_DMAC0_CHANNEL_NUM;
    }

    dma_priv->base = base;
    dma_priv->irq  = irq;

    uint32_t i;
    uint32_t ch_open_num = 0;
    uint32_t prer_dmac_ch_num = dma_priv->prer_dmac_ch_num;

    for (i = ch_num / prer_dmac_ch_num; i < ((ch_num / prer_dmac_ch_num + 1) * prer_dmac_ch_num - 1); i++) {
        if (dma_instance_p[i].ch_opened == 0x1) {
            ch_open_num++;
        }
    }

    if (ch_open_num >= 1) {
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
    writel(value, addr + DMA_REG_MaskBlock);
    writel(value, addr + DMA_REG_MaskErr);


    dma_priv->status = DMA_STATE_READY;

    return ch_num_record;
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

    uint32_t i;
    uint32_t ch_open_num = 0;

    if (ch <  CONFIG_PER_DMAC0_CHANNEL_NUM) {
        for (i = 0; i < CONFIG_PER_DMAC0_CHANNEL_NUM; i++) {
            if (dma_instance[i].ch_opened == 0x1) {
                ch_open_num++;
                break;
            }
        }
    } else {
        for (i = CONFIG_PER_DMAC0_CHANNEL_NUM; i < CONFIG_DMA_CHANNEL_NUM; i++) {
            if (dma_instance[i].ch_opened == 0x1) {
                ch_open_num++;
                break;
            }
        }
    }

#if defined(CONFIG_CHIP_PANGU)

    if (dma_instance[ch].ch_mux_func != -1) {
        dma_mux_channel_release(dma_instance[ch].ch_mux_func);
    }

#endif

    if (ch_open_num == 0) {
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
    dma_priv->ch_mux_func = config->hs_if;
    dma_priv->cb_event = cb_event;
    dma_priv->cb_arg = cb_arg;

    if (dma_priv->ch_opened == 0) {
        return ERR_DMA(DRV_ERROR_BUSY);
    }

    uint32_t addr = dma_priv->base;

    /* Initializes corresponding channel registers */


    int32_t ret = dw_dma_set_transferwidth(addr, dma_priv->ch_num, config->src_tw, config->dst_tw, dma_priv->prer_dmac_ch_num);

    if (ret < 0) {
        return ret;
    }

    dma_priv->dst_tw = config->dst_tw;
    dma_priv->src_tw = config->src_tw;

//    int32_t grouplen = ((length * config->src_tw / config->dst_tw) - 1) % 16;

    ret = dw_dma_set_transfertype(addr, dma_priv->ch_num, config->type, dma_priv->prer_dmac_ch_num);

    if (ret < 0) {
        return ret;
    }

    if (config->type == DMA_MEM2MEM) {
        dw_dma_set_handshaking(addr, dma_priv->ch_num, DMA_HANDSHAKING_SOFTWARE, DMA_HANDSHAKING_SOFTWARE, dma_priv->prer_dmac_ch_num);
        ret = dw_dma_set_addrinc(addr, dma_priv->ch_num, config->src_inc, config->dst_inc, dma_priv->prer_dmac_ch_num);

        if (ret < 0) {
            return ret;
        }

    } else if (config->type == DMA_MEM2PERH) {
        dw_dma_set_handshaking(addr, dma_priv->ch_num, DMA_HANDSHAKING_SOFTWARE, DMA_HANDSHAKING_HARDWARE, dma_priv->prer_dmac_ch_num);
        ret = dw_dma_set_addrinc(addr, dma_priv->ch_num, config->src_inc, config->dst_inc, dma_priv->prer_dmac_ch_num);

        if (ret < 0) {
            return ret;
        }

#if defined(CONFIG_CHIP_PANGU)
        uint32_t hs_if = dma_mux_channel_alloc(config->hs_if);

        if (hs_if < 0) {
            return -1;
        }

#else
        uint32_t hs_if = config->hs_if;
#endif
        ret = dw_dma_assign_hdhs_interface(addr, dma_priv->ch_num, 0, hs_if, dma_priv->prer_dmac_ch_num);

        if (ret < 0) {
            return ret;
        }

    } else if (config->type == DMA_PERH2MEM) {
        dw_dma_set_handshaking(addr, dma_priv->ch_num, DMA_HANDSHAKING_HARDWARE, DMA_HANDSHAKING_SOFTWARE, dma_priv->prer_dmac_ch_num);
        ret = dw_dma_set_addrinc(addr, dma_priv->ch_num, config->src_inc, config->dst_inc, dma_priv->prer_dmac_ch_num);

        if (ret < 0) {
            return ret;
        }

#if defined(CONFIG_CHIP_PANGU)
        uint32_t hs_if = dma_mux_channel_alloc(config->hs_if);

        if (hs_if < 0) {
            return -1;
        }

#else
        uint32_t hs_if = config->hs_if;
#endif

        ret = dw_dma_assign_hdhs_interface(addr, dma_priv->ch_num, hs_if, 0, dma_priv->prer_dmac_ch_num);

        if (ret < 0) {
            return ret;
        }

    }

    dw_dma_addr_reload_enable(addr, dma_priv->ch_num, config->src_reload_en, config->dest_reload_en, dma_priv->prer_dmac_ch_num);

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

    uint32_t prer_dmac_ch_num = dma_priv->prer_dmac_ch_num;
    dw_dma_set_burstlength(addr, dma_priv->ch_num, grouplen, prer_dmac_ch_num);
    dw_dma_set_channel(addr, dma_priv->ch_num, (uint32_t)psrcaddr, (uint32_t)pdstaddr, length, prer_dmac_ch_num);

    uint8_t src_master_idx = drv_dma_get_ahb_master_idx((uint32_t)psrcaddr);
    uint8_t dst_master_idx = drv_dma_get_ahb_master_idx((uint32_t)pdstaddr);
    dw_dma_set_master(addr, dma_priv->ch_num, src_master_idx, dst_master_idx, prer_dmac_ch_num);

    // interrupt enable
    uint32_t value = readl(addr + (dma_priv->ch_num % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    value |= CK_DMA_INT_EN;
    writel(value, addr + (dma_priv->ch_num % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    csi_dcache_clean_invalid();
    value = readl(addr + DMA_REG_ChEn);
    value |= (CK_DMA_CH_EN << (8 + (dma_priv->ch_num % prer_dmac_ch_num))) | (CK_DMA_CH_EN << (dma_priv->ch_num % prer_dmac_ch_num));
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
    uint32_t prer_dmac_ch_num = dma_priv->prer_dmac_ch_num;
    uint32_t value = readl(addr + DMA_REG_ChEn);
    value |= (CK_DMA_CH_EN << (8 + (dma_priv->ch_num % prer_dmac_ch_num)));
    value &= ~(CK_DMA_CH_EN << (dma_priv->ch_num % prer_dmac_ch_num));
    writel(value, addr + DMA_REG_ChEn);
    // interrupt disable
    value = readl(addr + (dma_priv->ch_num % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    value &= ~CK_DMA_INT_EN;
    writel(value, addr + (dma_priv->ch_num % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
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

