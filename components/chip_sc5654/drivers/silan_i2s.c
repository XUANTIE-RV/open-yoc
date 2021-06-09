#include "silan_printf.h"
#include "silan_adev.h"
#include "silan_errno.h"

void silan_i2s_config(I2S_CFG *pi2s_cfg)
{
    int master, mode, wlen, ws, tr, fthd, chnl;
    I2S_CORE_REGS *i2s = pi2s_cfg->regs;

    //SL_LOG("config i2s addr=%p", i2s);
    int fifo_depth = (pi2s_cfg->id == I2S_ID_PCM) ? I2S_FIFO_DEPTH/2 :
                                                   I2S_FIFO_DEPTH;

    master      = pi2s_cfg->master;
    mode        = pi2s_cfg->prot;
    wlen        = pi2s_cfg->wlen;
    ws            = pi2s_cfg->ws;
    fthd        = fifo_depth - pi2s_cfg->burst - 1;
    tr            = pi2s_cfg->tr;
    chnl        = (1 << (pi2s_cfg->ch + 1)) - 1;

    i2s->SRESET       = I2S_RESET;
    i2s->I2S_CTRL  = I2S_EN;

    i2s->I2S_CTRL |= (1        << 20) | \
                      (1        << 12);
    i2s->CLK_CTRL  = (master   << 24) | \
                     (ws       << 8 ) | \
                     (1        << 0 );

    if(tr & I2S_TR_TO) {
        i2s->I2S_CTRL |= (1    << 16) | \
                         (1    << 26) | \
                         (chnl << 22);
        i2s->I2S_CHNL[0].TCR  =         \
                     (fthd     << 24) | \
                     (0        << 20) | \
                     (mode     << 16) | \
                     (wlen     << 8 ) | \
                     (1        << 4 ) | \
                     (0        << 0 );
    }
    fthd        = pi2s_cfg->burst - 1;
    if(tr & I2S_TR_RO) {
        i2s->I2S_CTRL |= (1    << 8 ) | \
                         (1    << 31) | \
                         (chnl << 27);
        i2s->I2S_CHNL[0].RCR  =         \
                     (fthd     << 24) | \
                     (0        << 20) | \
                     (mode     << 16) | \
                     (wlen     << 8 ) | \
                     (1        << 4 ) | \
                     (1        << 3 ) | \
                     (0        << 0);
    }
}

void silan_i2s_start(I2S_CFG *pi2s_cfg)
{
    int tr;
    I2S_CORE_REGS *i2s = pi2s_cfg->regs;
    tr = pi2s_cfg->tr;

    //SL_LOG("regs=%p, tr=%d", i2s, tr);
    if(tr & I2S_TR_TO) {
        i2s->I2S_CHNL[0].TCR  |= 1;
    }
    if(tr & I2S_TR_RO) {
        i2s->I2S_CHNL[0].RCR  |= 1;
    }
}

void silan_i2s_stop(I2S_CFG *pi2s_cfg)
{
    int tr;
    I2S_CORE_REGS *i2s = pi2s_cfg->regs;
    tr = pi2s_cfg->tr;

    if(tr & I2S_TR_TO) {
        i2s->I2S_CHNL[0].TCR  &= ~(1);
    }
    if(tr & I2S_TR_RO) {
        i2s->I2S_CHNL[0].RCR  &= ~(1);
    }
}

int32_t silan_i2s_dev_open(I2S_CFG *pi2s_cfg)
{
    if((pi2s_cfg->id >= I2S_ID_LMT) || (pi2s_cfg->id == I2S_ID_NONE))
        return -ENODEV;

    switch(pi2s_cfg->id) {
        case I2S_ID_O1:
            pi2s_cfg->tx_srcid = DMAC_REQ_SRCID_O1_I2S_TX0;
            pi2s_cfg->tr = I2S_TR_TO;
            break;
        case I2S_ID_O2:
            pi2s_cfg->tx_srcid = DMAC_REQ_SRCID_O2_I2S_TX;
            pi2s_cfg->tr = I2S_TR_TO;
            break;
        case I2S_ID_O3:
            pi2s_cfg->tx_srcid = DMAC_REQ_SRCID_O3_I2S_TX;
            pi2s_cfg->tr = I2S_TR_TO;
            break;
        case I2S_ID_I1:
            pi2s_cfg->rx_srcid = DMAC_REQ_SRCID_I1_I2S_RX0;
            pi2s_cfg->tr = I2S_TR_RO;
            break;
        case I2S_ID_I2:
            pi2s_cfg->rx_srcid = DMAC_REQ_SRCID_I2_I2S_RX;
            pi2s_cfg->tr = I2S_TR_RO;
            break;
        case I2S_ID_I3:
            pi2s_cfg->rx_srcid = DMAC_REQ_SRCID_I3_I2S_RX;
            pi2s_cfg->tr = I2S_TR_RO;
            break;
        case I2S_ID_PCM:
            pi2s_cfg->tx_srcid = DMAC_REQ_SRCID_PCM_I2S_TX;
            pi2s_cfg->rx_srcid = DMAC_REQ_SRCID_PCM_I2S_RX;
            break;
        default:
            break;
    }

    pi2s_cfg->regs = (I2S_CORE_REGS *)(I2S_BASE_ADDR + 0x10000*(pi2s_cfg->id));

    silan_i2s_config(pi2s_cfg);

    return ENONE;
}

