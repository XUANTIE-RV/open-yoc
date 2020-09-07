/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_pic.c
 * @brief    CSI Source File for pic Driver
 * @version  V1.0
 * @date     20. August 2018
 ******************************************************************************/

#include <errno.h>
#include <soc.h>
#include <silan_pic.h>
#include <string.h>

#if defined(__XCC__)
#include "xtensa/xos.h"

#ifdef _SUPPORT_XOS_
extern XosThread thread_audio;
#endif
#endif

#define RTC_HDL_NUM        1
static hdl_t rtc_hdl[RTC_HDL_NUM];

#define ADC_HDL_NUM        1
static hdl_t adc_hdl[ADC_HDL_NUM];

#define SPDIF_HDL_NUM      2
static hdl_t spdif_hdl[SPDIF_HDL_NUM];

#define DMAC_HDL_NUM       8
static hdl_t sdmac_hdl[DMAC_HDL_NUM];
static hdl_t admac_hdl[DMAC_HDL_NUM];

#define GPIO_HDL_NUM       32
static hdl_t gpio1_hdl[GPIO_HDL_NUM];
static hdl_t gpio2_hdl[GPIO_HDL_NUM];

#define TIMER_HDL_NUM      7
static hdl_t timer_hdl[TIMER_HDL_NUM];

#define MISC_HDL_NUM       10
static hdl_t misc_hdl[MISC_HDL_NUM];

#define PMU_HDL_NUM        11
static hdl_t pmu_hdl[PMU_HDL_NUM];
static hdl_t sdmmc_hdl[1];

void pic_do_hdl(uint32_t stat, hdl_t *hdl)
{
    int subid = 0;

    while (stat) {
        if (stat & 0x1) {
            if(hdl[subid])
                (*hdl[subid])(subid);
        }
        stat >>= 1;
        subid++;
    }
}

void pic_hdl_rtc(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_RTC);
    pic_do_hdl(stat, rtc_hdl);
}

void pic_hdl_adc(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_SAR);
    pic_do_hdl(stat, adc_hdl);
}

  void pic_hdl_sdmmc(uint32_t irqid)
 {
     uint32_t stat;

     stat = INTR_STS_MSK(PIC_IRQID_SDMMC);
     pic_do_hdl(stat, sdmmc_hdl);
 }

 void pic_hdl_spdif(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_SPDIF);
    pic_do_hdl(stat, spdif_hdl);
}

 void pic_hdl_sdmac(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_SDMAC);
    pic_do_hdl(stat, sdmac_hdl);
}

 void pic_hdl_admac(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_ADMAC);
    pic_do_hdl(stat, admac_hdl);
#if defined(__XCC__)
#ifdef _SUPPORT_XOS_
    xos_thread_wake(&thread_audio, NULL, 0);
#endif
#endif
}

 void pic_hdl_gpio1(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_GPIO1);
    pic_do_hdl(stat, gpio1_hdl);
}

 void pic_hdl_gpio2(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_GPIO2);
    pic_do_hdl(stat, gpio2_hdl);
}

 void pic_hdl_misc(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_MISC);
    pic_do_hdl(stat, misc_hdl);
}

 void pic_hdl_pmu(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_PMU);
    pic_do_hdl(stat, pmu_hdl);
}

 void pic_hdl_timer(uint32_t irqid)
{
    uint32_t stat;

    stat = INTR_STS_MSK(PIC_IRQID_TIMER);
    pic_do_hdl(stat, timer_hdl);
}

int silan_pic_request(pic_irq_t id, int subid, hdl_t hdl)
{
    int err = 0;

    if ((id < 0) || (id >= PIC_IRQID_LMT) || (!hdl)) {
        return -EINVAL;
    }

    INTR_CTR_MSK(id) &= ~(1 << (subid));

    if ((id == PIC_IRQID_SPDIF) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= SPDIF_HDL_NUM) {
            return -EINVAL;
        }

        spdif_hdl[subid] = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_SDMAC) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= DMAC_HDL_NUM) {
            return -EINVAL;
        }

        sdmac_hdl[subid] = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_ADMAC) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= DMAC_HDL_NUM) {
            return -EINVAL;
        }

        admac_hdl[subid] = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_GPIO1) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= GPIO_HDL_NUM) {
            return -EINVAL;
        }

        gpio1_hdl[subid] = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_GPIO2) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= GPIO_HDL_NUM) {
            return -EINVAL;
        }

        gpio2_hdl[subid] = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_TIMER) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= TIMER_HDL_NUM) {
            return -EINVAL;
        }

        timer_hdl[subid] = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_MISC) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= MISC_HDL_NUM) {
            return -EINVAL;
        }

        misc_hdl[subid]   = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_PMU) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= PMU_HDL_NUM) {
            return -EINVAL;
        }

        pmu_hdl[subid]   = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_SAR) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= ADC_HDL_NUM) {
            return -EINVAL;
        }

        adc_hdl[subid]   = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else if ((id == PIC_IRQID_RTC) && (subid != PIC_SUBID_SOLO)) {
        if (subid >= RTC_HDL_NUM) {
            return -EINVAL;
        }

        rtc_hdl[subid]   = hdl;
        INTR_CTR_MSK(id) |= (1 << (subid));
    }
    else {
        err = pic_port_request(id, hdl);
        if (err) {
            return err;
        }
        // INTR_CTR_MSK(id) |= (1 << (subid));
    }

    return 0;
}

void silan_pic_free(pic_irq_t id, int subid)
{
    uint32_t mask;

    if ((id < 0) || (id >= PIC_IRQID_LMT)) {
        return;
    }

    mask = INTR_CTR_MSK(id);

    if (mask & (1<<subid)) {
        mask &= (~(1<<subid));
        INTR_CTR_MSK(id) = mask;

        if (id == PIC_IRQID_SPDIF) {
            if (subid >= SPDIF_HDL_NUM)
                return;
            spdif_hdl[subid] = NULL;
        } else if (id == PIC_IRQID_GPIO1) {
            if (subid >= GPIO_HDL_NUM)
                return;
            gpio1_hdl[subid] = NULL;
        } else if (id == PIC_IRQID_GPIO2) {
            if (subid >= GPIO_HDL_NUM)
                return;
            gpio2_hdl[subid] = NULL;
        } else if (id == PIC_IRQID_TIMER) {
            if (subid >= TIMER_HDL_NUM)
                return;
            timer_hdl[subid] = NULL;
        } else if (id == PIC_IRQID_MISC) {
            if (subid >= MISC_HDL_NUM)
                return;
            misc_hdl[subid]  = NULL;
        } else if (id == PIC_IRQID_ADMAC) {
            if (subid >= DMAC_HDL_NUM)
                return;
            admac_hdl[subid] = NULL;
        } else if (id == PIC_IRQID_SDMAC) {
            if (subid >= DMAC_HDL_NUM)
                return;
            sdmac_hdl[subid] = NULL;
        } else if (id == PIC_IRQID_PMU) {
            if (subid >= PMU_HDL_NUM)
                return;
            pmu_hdl[subid]   = NULL;
        } else if (id == PIC_IRQID_SAR) {
            if (subid >= ADC_HDL_NUM)
                return;
            adc_hdl[subid]   = NULL;
        } else if (id == PIC_IRQID_RTC) {
            if (subid >= RTC_HDL_NUM)
                return;
            rtc_hdl[subid]   = NULL;
        }
    }
    if (!mask) {
        pic_port_free(id);
    }
}

uint32_t silan_pic_get_subid(pic_irq_t id)
{
    return INTR_CTR_MSK(id);
}

uint32_t silan_pic_get_subid_stat(pic_irq_t id)
{
    return INTR_STS_MSK(id);
}

void silan_pic_init()
{
    /* FIXME, all interrupt should be masked. */
    //pic_port_init();

    memset(rtc_hdl, 0, RTC_HDL_NUM * sizeof(hdl_t));
    memset(adc_hdl, 0, ADC_HDL_NUM * sizeof(hdl_t));
    memset(spdif_hdl, 0, SPDIF_HDL_NUM * sizeof(hdl_t));
    memset(sdmac_hdl, 0, DMAC_HDL_NUM  * sizeof(hdl_t));
    memset(admac_hdl, 0, DMAC_HDL_NUM  * sizeof(hdl_t));
    memset(gpio1_hdl, 0, GPIO_HDL_NUM  * sizeof(hdl_t));
    memset(gpio2_hdl, 0, GPIO_HDL_NUM  * sizeof(hdl_t));
    memset(timer_hdl, 0, TIMER_HDL_NUM * sizeof(hdl_t));
    memset(misc_hdl,  0, TIMER_HDL_NUM * sizeof(hdl_t));

    // Register second-level interrupt
    pic_port_request(PIC_IRQID_RTC,   (void *)pic_hdl_rtc);
    pic_port_request(PIC_IRQID_SAR,   (void *)pic_hdl_adc);
    pic_port_request(PIC_IRQID_SPDIF, (void *)pic_hdl_spdif);
    pic_port_request(PIC_IRQID_SDMAC, (void *)pic_hdl_sdmac);
    pic_port_request(PIC_IRQID_ADMAC, (void *)pic_hdl_admac);
    pic_port_request(PIC_IRQID_GPIO1, (void *)pic_hdl_gpio1);
    pic_port_request(PIC_IRQID_GPIO2, (void *)pic_hdl_gpio2);
    pic_port_request(PIC_IRQID_MISC,  (void *)pic_hdl_misc);
    pic_port_request(PIC_IRQID_PMU,   (void *)pic_hdl_pmu);
    pic_port_request(PIC_IRQID_TIMER, (void *)pic_hdl_timer);
   // pic_port_request(PIC_IRQID_SDMMC, (void *)pic_hdl_sdmmc);
}
