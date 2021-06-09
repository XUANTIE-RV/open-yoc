/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     rsa.c
 * @brief    CSI Source File for RSA Driver
 * @version  V1.0
 * @date     24. Jan 2020
 ******************************************************************************/
#ifndef CONFIG_TEE_CA
#include <drv/irq.h>
#include <stdio.h>
#include <string.h>
#include <wj_rsa_ll.h>
#include <drv/rsa.h>
#include <drv/common.h>
extern uint8_t modulus[];
static uint32_t g_acc[RSA_KEY_WORD] = {0};
static rsa_middle_t rsa_config_t;

static void rsa_get_result(csi_rsa_t *rsa,   uint8_t *result);
static void rsa_calc_rsaresult(csi_rsa_t *rsa);
static inline uint32_t rsa_raise_exception(wj_rsa_regs_t *addr);

static void wj_rsa_irq_handler(void *arg)
{
    csi_rsa_callback_t  cb_func;

    CSI_PARAM_CHK_NORETVAL(arg);
    csi_rsa_t *rsa = (csi_rsa_t *)arg;
    wj_rsa_regs_t *addr;
    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);

    if (rsa_raise_exception(addr)) {
        cb_func = rsa->cb;

        if (cb_func) {
            cb_func(rsa, RSA_EVENT_ERROR, NULL);
        }
    } else {
        if (wj_rsa_get_irsr_cal_q_done(addr)) {
            wj_rsa_en_irsr_cal_q_done(addr);
            rsa_calc_rsaresult(rsa);
            ///<calculate Q finish
        } else  if (wj_rsa_get_irsr_rsa_done(addr)) {
            uint8_t  result = 0xff;
            wj_rsa_en_irsr_rsa_done(addr);
            rsa_get_result(rsa, &result);
            cb_func = rsa->cb;

            if (cb_func) {
                if (result == 0xff) {
                    cb_func(rsa, RSA_EVENT_COMPLETE, NULL);

                } else {
                    if (result == 1) {
                        cb_func(rsa, RSA_EVENT_VERIFY_SUCCESS, NULL);
                    } else {
                        cb_func(rsa, RSA_EVENT_VERIFY_FAILED, NULL);
                    }
                }
            }
        }

    }

}

void wj_rsa_clear_intr(wj_rsa_regs_t *addr)
{
    wj_rsa_reset_imr(addr, WJ_RSA_IMR_RSA_DONE_EN | \
                     WJ_RSA_IMR_Abnormal1_EN | \
                     WJ_RSA_IMR_Abnormal2_EN | \
                     WJ_RSA_IMR_Abnormal3_EN | \
                     WJ_RSA_IMR_Abnormal4_EN | \
                     WJ_RSA_IMR_CAL_Q_DONE_EN);

    wj_rsa_set_irsr(addr, WJ_RSA_IRSR_RSA_DONE_EN | \
                    WJ_RSA_IRSR_Abnormal1_EN | \
                    WJ_RSA_IRSR_Abnormal2_EN | \
                    WJ_RSA_IRSR_Abnormal3_EN | \
                    WJ_RSA_IRSR_Abnormal4_EN | \
                    WJ_RSA_IRSR_CAL_Q_DONE_EN);
}

void wj_rsa_en_intr(wj_rsa_regs_t *addr)
{
    wj_rsa_set_imr(addr, WJ_RSA_IMR_RSA_DONE_EN | \
                   WJ_RSA_IMR_Abnormal1_EN | \
                   WJ_RSA_IMR_Abnormal2_EN | \
                   WJ_RSA_IMR_Abnormal3_EN | \
                   WJ_RSA_IMR_Abnormal4_EN | \
                   WJ_RSA_IMR_CAL_Q_DONE_EN);


    wj_rsa_set_irsr(addr, WJ_RSA_IRSR_RSA_DONE_EN | \
                    WJ_RSA_IRSR_Abnormal1_EN | \
                    WJ_RSA_IRSR_Abnormal2_EN | \
                    WJ_RSA_IRSR_Abnormal3_EN | \
                    WJ_RSA_IRSR_Abnormal4_EN | \
                    WJ_RSA_IRSR_CAL_Q_DONE_EN);


}

/**
  \brief       Initialize RSA Interface. 1. Initializes the resources needed for the RSA interface 2.registers event callback function
  \param[in]   rsa  rsa handle to operate.
  \param[in]   idx  device id
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_init(csi_rsa_t *rsa, uint32_t idx)
{
    wj_rsa_regs_t *addr;

    CSI_PARAM_CHK(rsa, CSI_ERROR);

    target_get(DEV_WJ_RSA_TAG, idx, &rsa->dev);

    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);
    memset((void*)addr,0,sizeof(wj_rsa_regs_t));
    wj_rsa_clear_intr(addr);

    csi_irq_attach(rsa->dev.irq_num, &wj_rsa_irq_handler, &rsa->dev);
    csi_irq_enable(rsa->dev.irq_num);

    return CSI_OK;
}

/**
  \brief       De-initialize RSA Interface. stops operation and releases the software resources used by the interface
  \param[in]   rsa  rsa handle to operate.
  \return      none
*/
void csi_rsa_uninit(csi_rsa_t *rsa)
{
    wj_rsa_regs_t *addr;

    CSI_PARAM_CHK_NORETVAL(rsa);

    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);

    wj_rsa_clear_intr(addr);
    csi_irq_detach(rsa->dev.irq_num);

    csi_irq_disable(rsa->dev.irq_num);

    rsa->cb   = NULL;
    memset((void*)addr,0,sizeof(wj_rsa_regs_t));

}

/**
  \brief       attach the callback handler to RSA
  \param[in]   rsa  operate handle.
  \param[in]   cb    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
csi_error_t csi_rsa_attach_callback(csi_rsa_t *rsa, csi_rsa_callback_t cb, void *arg)
{
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    rsa->cb = cb;
    rsa->arg = arg;
    return CSI_OK;
}

/**
  \brief       detach the callback handler
  \param[in]   rsa  operate handle.
*/
void csi_rsa_detach_callback(csi_rsa_t *rsa)
{
    CSI_PARAM_CHK_NORETVAL(rsa);
    rsa->cb = NULL;
}

static uint32_t get_valid_bits(const uint32_t *addr, uint32_t wordsize, uint32_t keywords)
{
    uint32_t i = 0;
    uint32_t j = 0;

    for (i = wordsize; i > 0; i--) {
        if (addr[i - 1]) {
            break;
        }
    }

    for (j = keywords; j > 0; j--) {
        if (addr[i - 1] & (0x1 << (j - 1))) {
            break;
        }
    }

    return ((i - 1) << 5) + j;
}

#ifndef RSA_USING_ID2KEY
static uint32_t get_first_nonzero_words(uint32_t *a, uint32_t max_words)
{
    uint32_t i = 0;

    for (i = max_words; i > 0; i--) {
        if (a[i - 1]) {
            return i;
        }
    }

    return 0;
}

static uint32_t word_array_left_shift(uint32_t *a, uint32_t words,
                                      uint32_t shift_bits, uint32_t *r)
{
    uint32_t i = 0;
    uint32_t w = shift_bits >> 5;
    uint32_t b = shift_bits - (w << 5);

    for (i = 0; i < w; i++) {
        r[i] = 0;
    }

    uint32_t tmp = 0;

    for (i = 0; i < words; i++) {
        r[w + i] = (tmp | ((a[i] << b) & (~((0x1 << b) - 1))));
        tmp = ((a[i] >> (32 - b)) & ((0x1 << b) - 1));
    }

    r[w + i] = tmp;

    return 0;
}

static uint32_t _word_array_sub(uint32_t *a, uint32_t a_words,
                                uint32_t *b, uint32_t b_words,
                                uint32_t *r)
{
    uint32_t i;
    uint64_t tmp = 0;
    uint32_t borrow = 0;

    for (i = 0; i < b_words; i++) {
        tmp = UINT32_TO_UINT64(a[i]) - UINT32_TO_UINT64(b[i]) - UINT32_TO_UINT64(borrow);
        r[i] = UINT64L_TO_UINT32(tmp);
        borrow = ((UINT64H_TO_UINT32(tmp) == 0) ? (0) : (0xffffffff - UINT64H_TO_UINT32(tmp) + 1));
    }

    for (i = b_words; i < a_words; i++) {
        tmp = UINT32_TO_UINT64(a[i]) - UINT32_TO_UINT64(borrow);
        r[i] = UINT64L_TO_UINT32(tmp);
        borrow = ((UINT64H_TO_UINT32(tmp) == 0) ? (0) : (0xffffffff - UINT64H_TO_UINT32(tmp) + 1));
    }

    if (borrow) {
        return -1;
    }

    return 0;
}
#endif

static void convert_byte_array(uint8_t *in, uint8_t *out, uint32_t len)
{
    uint32_t idx, round = len >> 1;

    for (idx = 0; idx < round; idx++) {
        uint8_t tmp = *(in + idx);
        *(out + idx) = *(in + len - 1 - idx);
        *(out + len - 1 - idx) = tmp;
    }

    if (len & 0x1) {
        *(out + round) = *(in + round);
    }
}

static void convert_buf_to_bndata(const uint8_t *src, uint32_t src_bytes,
                                  uint32_t *dst, uint32_t dst_words)
{
    memset(dst, 0, dst_words << 2);
    convert_byte_array((uint8_t *)src, (uint8_t *)dst, src_bytes);
}

static void convert_bndata_to_buf(const uint32_t *src, uint32_t src_words,
                                  uint8_t *dst, uint32_t dst_bytes)
{
    memset(dst, 0, dst_bytes);
    convert_byte_array((uint8_t *)src, (uint8_t *)dst, dst_bytes);
}


static const uint8_t der_sha256_t[] = {
    0x30, 0x31,
    0x30, 0x0d,
    0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, /* id-sha256 */
    0x05, 0x00,
    0x04, 0x20
};

static const uint8_t der_sha1_t[] = {
    0x30, 0x21,
    0x30, 0x09,
    0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
    0x05, 0x00,
    0x04, 0x14
};

static const uint8_t der_md5_t[] = {
    0x30, 0x20, /* type Sequence, length 0x20 (32) */
    0x30, 0x0c, /* type Sequence, length 0x09 */
    0x06, 0x08, /* type OID, length 0x05 */
    0x2a, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05, /* id-md5 */
    0x05, 0x00, /* NULL */
    0x04, 0x10  /* Octet string, length 0x10 (16), followed by md5 hash */
};
static uint32_t rsa_padding_pkcs(uint8_t  *dgst,
                                 uint8_t  *out,
                                 uint32_t  type,
                                 uint32_t  keybytes)

{
    uint32_t i;
    uint8_t *p;
    uint8_t *der;
    uint32_t der_len;
    uint32_t hashlen;
    uint32_t pslen;

    if (type == MD5_PADDING) {
        der     = (uint8_t *)der_md5_t;
        der_len = sizeof(der_md5_t);
        hashlen = MD5_HASH_SZ;
    } else if (type == SHA256_PADDING) {
        der     = (uint8_t *)der_sha256_t;
        der_len = sizeof(der_sha256_t);
        hashlen = SHA256_HASH_SZ;
    } else {
        der     = (uint8_t *)der_sha1_t;
        der_len = sizeof(der_sha1_t);
        hashlen = SHA1_HASH_SZ;
    }

    p = (uint8_t *)out;

    *(p++) = 0x00;
    *(p++) = 0x01;

    /* pad out with 0xff data */
    pslen = keybytes - 3 - der_len - hashlen;

    for (i = 0; i < pslen; i++) {
        p[i] = 0xff; /* PS */
    }

    p += pslen;
    *(p++) = 0x0;

    for (i = 0; i < der_len; i++) {
        p[i] = der[i];
    }

    p += der_len;

    for (i = 0; i < hashlen; i++) {
        p[i] = dgst[i];
    }

    return 0;
}

static uint32_t rsa_checking_pkcs(uint8_t *dgst,
                                  uint8_t *in,
                                  uint32_t inlen,
                                  uint8_t *is_valid,
                                  uint32_t type,
                                  uint32_t keybytes)
{
    uint32_t i;
    uint32_t ret;
    const uint8_t *p;
    uint8_t *der = (uint8_t *)der_sha1_t;
    uint32_t der_len = sizeof(der_sha1_t);
    uint32_t hashlen = SHA1_HASH_SZ;
    uint32_t pslen;

    if (type == MD5_PADDING) {
        der     = (uint8_t *)der_md5_t;
        der_len = sizeof(der_md5_t);
        hashlen = MD5_HASH_SZ;
    } else if (type == SHA1_PADDING) {
        der     = (uint8_t *)der_sha1_t;
        der_len = sizeof(der_sha1_t);
        hashlen = SHA1_HASH_SZ;
    } else if (type == SHA256_PADDING) {
        der     = (uint8_t *)der_sha256_t;
        der_len = sizeof(der_sha256_t);
        hashlen = SHA256_HASH_SZ;
    }

    *is_valid = 0;

    pslen = keybytes - 3 - der_len - hashlen;
    p = in;
    p++;

    if (*(p) != 0x01) {
        ret = -1;
        goto _verify_fail;
    }

    p++;

    /* scan PS */
    for (i = 0; i < pslen; i++) {
        if (*(p + i) != 0xff) {
            ret = -2;
            goto _verify_fail;
        }
    }

    p += pslen;

    if ((*p) != 0x00) {
        ret = -1;
        goto _verify_fail;
    }

    p++;

    /* scan t */
    for (i = 0; i < der_len; i++) {
        if (*(p + i) != der[i]) {
            ret = -3;
            goto _verify_fail;
        }
    }

    p += der_len;

    for (i = 0; i < hashlen; i++) {
        if (*(p + i) != dgst[i]) {
            ret = -4;
            goto _verify_fail;
        }
    }

    *is_valid = 1;
    ret = 0;

_verify_fail:

    return ret;
}

static uint32_t rsa_padding_es_pkcs(uint8_t *dgst,
                                    uint32_t dgstlen,
                                    uint8_t *out,
                                    uint32_t padding,
                                    uint32_t keybytes)

{
    uint32_t i;
    uint8_t *p;
    uint32_t pslen;

    p = (uint8_t *)out;

    *(p++) = 0x00;
    *(p++) = 0x02;

    /* pad out with 0xff data */
    pslen = keybytes - 3 - dgstlen;

    for (i = 0; i < pslen; i++) {
        p[i] = 0xff; /* PS */
    }

    p += pslen;
    *(p++) = 0x0;

    for (i = 0; i < dgstlen; i++) {
        p[i] = dgst[i];
    }

    return 0;
}

static uint32_t rsa_checking_es_pkcs(uint8_t *out,
                                     uint32_t *out_size,
                                     uint8_t *src,
                                     uint32_t src_size,
                                     uint32_t padding,
                                     uint32_t keybytes)
{
    uint32_t i;
    uint8_t *p;
    uint8_t *p_src;
    uint32_t pslen;

    p = (uint8_t *)src;
    p_src = p;
    *(p++) = 0x00;

    if (padding == PKCS1_PADDING) {
        if (*(p++) != 0x02) {
            return -1;
        }
    } else {
        if (*(p++) != 0x01) {
            return -2;
        }
    }

    pslen = src_size - 2;

    while (pslen--) {
        if (*(p++) == 0x0) {
            break;
        }
    }

    if (padding == PKCS1_PADDING) {
        *out_size = pslen;
    } else {
        *out_size = keybytes;
    }

    for (i = 0; i < *out_size; i++) {
        if (padding == PKCS1_PADDING) {
            out[i] = p[i];
        } else {
            out[i] = p_src[i];
        }
    }

    return 0;
}

#ifndef RSA_USING_ID2KEY
static uint32_t word_array_mod(uint32_t *a, uint32_t a_words,
                               uint32_t *b, uint32_t b_words,
                               uint32_t *r, uint32_t keywords)
{
    uint32_t ret;
    bignum_t tmpa;
    bignum_t tmpb;

    memset(&tmpa, 0, sizeof(tmpa));
    memset(&tmpb, 0, sizeof(tmpa));

    uint32_t b_valid_bits = get_valid_bits(b, b_words, keywords);

    memcpy(tmpa.pdata, a, (a_words << 2));

    do {
        uint32_t tmpa_words = get_first_nonzero_words(tmpa.pdata, a_words);
        uint32_t tmpa_valid_bits = get_valid_bits(tmpa.pdata, tmpa_words, keywords);

        if (tmpa_valid_bits > b_valid_bits + 1) {
            memset(tmpb.pdata, 0, (a_words << 2));
            word_array_left_shift(b, b_words, tmpa_valid_bits - b_valid_bits - 1,
                                  tmpb.pdata);
            uint32_t tmpb_words = get_first_nonzero_words(tmpb.pdata, a_words);
            ret = _word_array_sub(tmpa.pdata, tmpa_words, tmpb.pdata, tmpb_words, tmpa.pdata);
        } else if (tmpa_words == b_words) {
            memcpy(r, tmpa.pdata, (tmpa_words << 2));
            ret = _word_array_sub(r, tmpa_words, b, b_words, tmpa.pdata);
        } else {
            ret = _word_array_sub(tmpa.pdata, tmpa_words, b, b_words, tmpa.pdata);
        }
    } while (ret == 0);

    return 0;
}

static uint32_t sw_exptmod_2_2m(const uint32_t *modulus, uint32_t words, uint32_t *tmp_c)
{
    bignum_t tmp;

    memset(&tmp, 0, sizeof(bignum_t));

    uint32_t m_valid_bits = (words << 5);

    uint32_t data1 = 0x1;
    word_array_left_shift(&data1, 1, (m_valid_bits << 1), tmp.pdata);
    tmp.words = get_first_nonzero_words(tmp.pdata, words * 2 + 1);

    uint32_t ret = word_array_mod(tmp.pdata, tmp.words,
                                  (uint32_t *)modulus, words, tmp_c, words);

    if (ret != 0) {
        return ret;
    }

    return 0;
}
#endif

static inline uint32_t rsa_raise_exception(wj_rsa_regs_t *addr)
{
    if (wj_rsa_get_irsr(addr) & (WJ_RSA_IRSR_Abnormal1_EN | \
                                 WJ_RSA_IRSR_Abnormal2_EN | \
                                 WJ_RSA_IRSR_Abnormal3_EN | \
                                 WJ_RSA_IRSR_Abnormal4_EN)) {
        return 1;
    } else {

        return 0;
    }
}

static inline uint32_t rsa_loadm(wj_rsa_regs_t *addr, uint32_t *data, uint32_t length)
{
    uint32_t i;
    uint32_t baseaddr = (uint32_t)&addr->RSA_MWID + RSA_RFM_OFFSET;

    for (i = 0; i < length; i++) {
        *(volatile uint32_t *)baseaddr = data[i];
        baseaddr = baseaddr + 4;
    }

    return 0;
}

static void rsa_loadd(wj_rsa_regs_t *addr, uint32_t *data, uint32_t length)
{
    uint32_t i;
    uint32_t baseaddr = (uint32_t)&addr->RSA_MWID + RSA_RFD_OFFSET;

    for (i = 0;  i < length; i++) {
        *(volatile uint32_t *)baseaddr = data[i];
        baseaddr = baseaddr + 4;
    }
}

static void rsa_loadc(wj_rsa_regs_t *addr, uint32_t *data, uint32_t length)
{
    uint32_t i;
    uint32_t baseaddr = (uint32_t)&addr->RSA_MWID + RSA_RFC_OFFSET;

    for (i = 1; i < length + 1; i++) {
        *(volatile uint32_t *)baseaddr = data[i - 1];
        baseaddr = baseaddr + 4;
    }
}

static void rsa_loadb(wj_rsa_regs_t *addr, uint32_t *data, uint32_t length)
{
    uint32_t i;
    uint32_t baseaddr = (uint32_t)&addr->RSA_MWID + RSA_RFB_OFFSET;

    for (i = 0; i < length; i++) {
        *(volatile uint32_t *)baseaddr = data[i];
        baseaddr = baseaddr + 4;
    }
}

static void rsa_read_r(wj_rsa_regs_t *addr, uint32_t data[], uint32_t length)
{
    uint32_t i;
    uint32_t baseaddr = (uint32_t)&addr->RSA_MWID + RSA_RFR_OFFSET;

    for (i = 0; i < length; i++) {
        data[i] = *(uint32_t *)baseaddr;
        baseaddr = baseaddr + 4;
    }
}

static inline int32_t rsa_opr_reset(wj_rsa_regs_t *addr)
{

    volatile uint32_t timeout = 0x1000000;
    wj_rsa_en_ctrl_little_endian(addr);
    /* reset for safe */
    wj_rsa_en_rst_rst(addr);

    while (wj_rsa_get_rst_rst(addr)) {
        if (!(timeout--)) {
            return 1;
        }
    }

    return 0;
}

static uint32_t rsa_exptmod(csi_rsa_t *rsa, const uint32_t *modulus, const uint32_t *exponent,
                            const uint32_t *base, uint32_t *out, uint32_t keywords)
{
#ifndef RSA_USING_ID2KEY
    uint32_t tmp_c[RSA_KEY_WORD];
#endif
    uint32_t ret = 0;
    register uint32_t timeout = 0x1000000;
    wj_rsa_regs_t *addr;
    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);

    if ((NULL == exponent) || (NULL == base) || (NULL == out)) {
        return 1;
    }

#ifndef RSA_USING_ID2KEY

    if (g_acc[0] != 0x0) {
        memcpy(tmp_c, g_acc, sizeof(g_acc));
    } else {
        ret = sw_exptmod_2_2m(modulus, keywords, tmp_c);

        if (ret != 0) {
            return ret;
        }
    }

#endif

    /* reset for safe */
    ret = rsa_opr_reset(addr);

    if (ret) {
        return ret;
    }

    /* clear and disable int */
    wj_rsa_clear_intr(addr);
    /* set m */
    wj_rsa_set_mwid(addr, keywords >> 1);

    rsa_loadm(addr, (uint32_t *)modulus, keywords);
    /* set d */
    wj_rsa_set_dwid(addr, get_valid_bits(exponent, keywords, keywords) - 1);
    rsa_loadd(addr, (uint32_t *)exponent, keywords);
    /* set b */
    wj_rsa_set_bwid(addr, keywords >> 1);
    rsa_loadb(addr, (uint32_t *)base, keywords);

    /* set c */
#ifndef RSA_USING_ID2KEY
    rsa_loadc(addr, tmp_c, keywords);
#else
    rsa_loadc(addr, g_acc, keywords);
#endif

#ifdef CONFIG_CHIP_CH2201

    if (rsa_run_adjust_sp_if_need(__get_SP())) {
        uint32_t result = csi_irq_save();
        rsa_calq_by_assemble();
        csi_irq_restore(result);
    } else {
#endif
        wj_rsa_en_ctrl_rsa_en(addr);
        wj_rsa_en_ctrl_cal_en(addr);

        while ((! wj_rsa_get_irsr_cal_q_done(addr) && (!rsa_raise_exception(addr)))) {
            if (!(timeout--)) {
                return CSI_ERROR;
            }
        }

#ifdef CONFIG_CHIP_CH2201
    }

#endif

    if (!rsa_raise_exception(addr)) {
#ifdef CONFIG_CHIP_CH2201

        if (rsa_run_adjust_sp_if_need(__get_SP())) {
            uint32_t result = csi_irq_save();
            rsa_run_by_assemble();
            csi_irq_restore(result);
        } else {
#endif
            wj_rsa_en_ctrl_rsa_en(addr);
            wj_rsa_en_ctrl_rsa_start(addr);

            while ((!wj_rsa_get_irsr_rsa_done(addr)) && (wj_rsa_get_lp_cnt(addr) < MAX_RSA_LP_CNT) && (!rsa_raise_exception(addr)));

#ifdef CONFIG_CHIP_CH2201
        }

#endif

        if ((wj_rsa_get_lp_cnt(addr) >= MAX_RSA_LP_CNT)
            || rsa_raise_exception(addr)) {
            ret = 1;
        } else {
            rsa_read_r(addr, out, keywords);
        }
    } else {
        ret = 1;
    }

    wj_rsa_en_rst_rst(addr);

    return ret;

}

static void rsa_calc_rsaresult(csi_rsa_t *rsa)
{
    wj_rsa_regs_t *addr;
    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);

#ifdef CONFIG_CHIP_CH2201

    if (rsa_run_adjust_sp_if_need(__get_SP())) {
        uint32_t result = csi_irq_save();
        rsa_run_by_assemble();
        csi_irq_restore(result);
    } else {
#endif
        wj_rsa_en_ctrl_rsa_en(addr);
        wj_rsa_en_ctrl_rsa_start(addr);

#ifdef CONFIG_CHIP_CH2201
    }

#endif
}

///rsa_state:<1:encrypt,2:decrypt，3：sign,4:verify
static void rsa_get_result(csi_rsa_t *rsa,   uint8_t *result)
{
    wj_rsa_regs_t *addr;

    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);
    rsa_read_r(addr, rsa_config_t.pout, rsa_config_t.u32keywords);

    wj_rsa_en_rst_rst(addr);

    switch (rsa_config_t.rsa_state) {
        case 1:
            convert_bndata_to_buf(rsa_config_t.pout, rsa_config_t.u32keywords, rsa_config_t.pdst, rsa_config_t.u32dst_words);
            break;

        case 2:
            convert_byte_array((uint8_t *)rsa_config_t.pout, (uint8_t *)rsa_config_t.pout, rsa_config_t.u32dst_words);

            rsa_checking_es_pkcs(rsa_config_t.pouts,
                                 rsa_config_t.pout_size,
                                 (uint8_t *)rsa_config_t.pout,
                                 rsa_config_t.u32dst_words,
                                 rsa_config_t.u32padding,
                                 rsa_config_t.u32dst_words);
            break;

        case 3:
            convert_bndata_to_buf(rsa_config_t.pout, rsa_config_t.u32keywords, rsa_config_t.pdst, rsa_config_t.u32dst_words);
            break;

        case 4:
            convert_byte_array((uint8_t *)rsa_config_t.pout, (uint8_t *)rsa_config_t.pout, rsa_config_t.u32dst_words);

            rsa_checking_pkcs(rsa_config_t.pdst,
                              (uint8_t *)rsa_config_t.pout,
                              rsa_config_t.u32dst_words,
                              result,
                              rsa_config_t.u32type,
                              rsa_config_t.u32dst_words);
            break;
    }

    rsa->state.busy = 0U;
    rsa_config_t.rsa_state = 0;
}

static uint32_t rsa_exptmod_asy(csi_rsa_t *rsa, const uint32_t *modulus, const uint32_t *exponent,
                                const uint32_t *base, uint32_t *out, uint32_t keywords,
                                uint8_t *dst, uint32_t dst_words,
                                uint8_t *outs, uint32_t *out_size, uint32_t padding)
{
#ifndef RSA_USING_ID2KEY
    uint32_t tmp_c[RSA_KEY_WORD];
#endif
    uint32_t ret = 0;
    wj_rsa_regs_t *addr;
    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);

    if ((NULL == exponent) || (NULL == base) || (NULL == out)) {
        return 1;
    }

#ifndef RSA_USING_ID2KEY

    if (g_acc[0] != 0x0) {
        memcpy(tmp_c, g_acc, sizeof(g_acc));
    } else {
        ret = sw_exptmod_2_2m(modulus, keywords, tmp_c);

        if (ret != 0) {
            return ret;
        }
    }

#endif

    /* reset for safe */
    ret = rsa_opr_reset(addr);

    if (ret) {
        return ret;
    }

    /*clear and enable interrupt*/
    wj_rsa_en_intr(addr);
    /* set m */
    wj_rsa_set_mwid(addr, keywords >> 1);

    rsa_loadm(addr, (uint32_t *)modulus, keywords);
    /* set d */
    wj_rsa_set_dwid(addr, get_valid_bits(exponent, keywords, keywords) - 1);
    rsa_loadd(addr, (uint32_t *)exponent, keywords);
    /* set b */
    wj_rsa_set_bwid(addr, keywords >> 1);
    rsa_loadb(addr, (uint32_t *)base, keywords);

    /* set c */
#ifndef RSA_USING_ID2KEY
    rsa_loadc(addr, tmp_c, keywords);
#else
    rsa_loadc(addr, g_acc, keywords);
#endif

#ifdef CONFIG_CHIP_CH2201

    if (rsa_run_adjust_sp_if_need(__get_SP())) {
        uint32_t result = csi_irq_save();
        rsa_calq_by_assemble();
        csi_irq_restore(result);
    } else {
#endif
        wj_rsa_en_ctrl_rsa_en(addr);
        wj_rsa_en_ctrl_cal_en(addr);


#ifdef CONFIG_CHIP_CH2201
    }

#endif
    rsa_config_t.pdst = dst;
    rsa_config_t.pout_size = out_size;
    rsa_config_t.pouts = outs;
    rsa_config_t.u32keywords = keywords;
    rsa_config_t.u32padding = padding;
    rsa_config_t.u32dst_words = dst_words;

    return ret;

}

static uint32_t rsa_exptmod_asy1(csi_rsa_t *rsa, const uint32_t *modulus, const uint32_t *exponent,
                                 const uint32_t *base, uint32_t *out, uint32_t keywords,
                                 uint32_t dst_words, uint8_t *dst, uint32_t type)
{
#ifndef RSA_USING_ID2KEY
    uint32_t tmp_c[RSA_KEY_WORD];
#endif
    uint32_t ret = 0;
    wj_rsa_regs_t *addr;
    addr = (wj_rsa_regs_t *)HANDLE_REG_BASE(rsa);

    if ((NULL == exponent) || (NULL == base) || (NULL == out)) {
        return 1;
    }

#ifndef RSA_USING_ID2KEY

    if (g_acc[0] != 0x0) {
        memcpy(tmp_c, g_acc, sizeof(g_acc));
    } else {
        ret = sw_exptmod_2_2m(modulus, keywords, tmp_c);

        if (ret != 0) {
            return ret;
        }
    }

#endif

    /* reset for safe */
    ret = rsa_opr_reset(addr);

    if (ret) {
        return ret;
    }

    /*clear and enable interrupt*/
    wj_rsa_en_intr(addr);
    /* set m */
    wj_rsa_set_mwid(addr, keywords >> 1);

    rsa_loadm(addr, (uint32_t *)modulus, keywords);
    /* set d */
    wj_rsa_set_dwid(addr, get_valid_bits(exponent, keywords, keywords) - 1);
    rsa_loadd(addr, (uint32_t *)exponent, keywords);
    /* set b */
    wj_rsa_set_bwid(addr, keywords >> 1);
    rsa_loadb(addr, (uint32_t *)base, keywords);

    /* set c */
#ifndef RSA_USING_ID2KEY
    rsa_loadc(addr, tmp_c, keywords);
#else
    rsa_loadc(addr, g_acc, keywords);
#endif

#ifdef CONFIG_CHIP_CH2201

    if (rsa_run_adjust_sp_if_need(__get_SP())) {
        uint32_t result = csi_irq_save();
        rsa_calq_by_assemble();
        csi_irq_restore(result);
    } else {
#endif
        wj_rsa_en_ctrl_rsa_en(addr);
        wj_rsa_en_ctrl_cal_en(addr);

#ifdef CONFIG_CHIP_CH2201
    }

#endif

    rsa_config_t.u32dst_words = dst_words;
    rsa_config_t.pdst = dst;
    rsa_config_t.u32type = type;

    return ret;

}
/**
  \brief       generate rsa key pair.
  \param[in]   rsa       rsa handle to operate.
  \param[out]  context   Pointer to the rsa context
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_gen_key(csi_rsa_t *rsa, csi_rsa_context_t *context)
{
    return CSI_OK;
}

static int rsa_encrypt(csi_rsa_t *rsa, uint8_t *n,         uint8_t *e,
                uint8_t *src,       uint32_t src_size,
                uint8_t *out,
                uint32_t padding,   uint32_t keybits_len)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_e[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;
    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(e, keybytes, tmp_e, keywords);

    if (padding == PKCS1_PADDING) {
        ret = rsa_padding_es_pkcs(src,
                                  src_size,
                                  (uint8_t *)tmp_in_out,
                                  padding,
                                  keybytes);

        if (ret != 0) {
            return ret;
        }

        convert_byte_array((uint8_t *)tmp_in_out, (uint8_t *)tmp_in_out, keybytes);
    } else {
        convert_byte_array((uint8_t *)src, (uint8_t *)tmp_in_out, keybytes);
    }

    ret = rsa_exptmod(rsa, tmp_n, tmp_e, tmp_in_out, tmp_in_out, keywords);

    if (ret != 0) {
        return ret;
    }

    convert_bndata_to_buf(tmp_in_out, keywords, out, keybytes);

    return ret;
}

static int rsa_encrypt_asy(csi_rsa_t *rsa, uint8_t *n,         uint8_t *e,
                    uint8_t *src,       uint32_t src_size,
                    uint8_t *out,
                    uint32_t padding,   uint32_t keybits_len)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_e[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;
    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(e, keybytes, tmp_e, keywords);

    if (padding == PKCS1_PADDING) {
        ret = rsa_padding_es_pkcs(src,
                                  src_size,
                                  (uint8_t *)tmp_in_out,
                                  padding,
                                  keybytes);

        if (ret != 0) {
            return ret;
        }

        convert_byte_array((uint8_t *)tmp_in_out, (uint8_t *)tmp_in_out, keybytes);
    } else {
        convert_byte_array((uint8_t *)src, (uint8_t *)tmp_in_out, keybytes);
    }

    ret = rsa_exptmod_asy(rsa, tmp_n, tmp_e, tmp_in_out, tmp_in_out, keywords, out, keybytes, NULL, NULL, 0);

    return ret;
}

/**
  \brief       encrypt
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \return      \ref csi_error_t
*/

csi_error_t csi_rsa_encrypt(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out)
{
    csi_error_t ret = CSI_OK;
    uint32_t bit_length = 1024;
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    CSI_PARAM_CHK(context->n, CSI_ERROR);
    CSI_PARAM_CHK(context->e, CSI_ERROR);
    CSI_PARAM_CHK(src, CSI_ERROR);
    CSI_PARAM_CHK(out, CSI_ERROR);

    if (src_size <= 0  || (context->padding_type != RSA_PADDING_MODE_PKCS1)) {
        ret = CSI_ERROR;
    } else {

        rsa->state.busy = 1U;

        if (context->key_bits == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_encrypt(rsa, (uint8_t *)context->n, (uint8_t *)context->e, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)out, (uint32_t)(context->padding_type), bit_length);
        rsa->state.busy = 0U;
    }

    return ret;
}

int rsa_decrypt(csi_rsa_t *rsa, uint8_t *n,         uint8_t *d,
                uint8_t *src,       uint32_t src_size,
                uint8_t *out,       uint32_t *out_size,
                uint32_t padding,   uint32_t keybits_len)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_d[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;

    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(d, keybytes, tmp_d, keywords);
    convert_buf_to_bndata(src, src_size, tmp_in_out, keywords);

    ret = rsa_exptmod(rsa, tmp_n, tmp_d, tmp_in_out, tmp_in_out, keywords);

    if (ret != 0) {
        return ret;
    }

    convert_byte_array((uint8_t *)tmp_in_out, (uint8_t *)tmp_in_out, keybytes);

    ret = rsa_checking_es_pkcs(out,
                               out_size,
                               (uint8_t *)tmp_in_out,
                               keybytes,
                               padding,
                               keybytes);

    return ret;
}

int rsa_decrypt_asy(csi_rsa_t *rsa, uint8_t *n,         uint8_t *d,
                    uint8_t *src,       uint32_t src_size,
                    uint8_t *out,       uint32_t *out_size,
                    uint32_t padding,   uint32_t keybits_len)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_d[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;

    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(d, keybytes, tmp_d, keywords);
    convert_buf_to_bndata(src, src_size, tmp_in_out, keywords);

    ret = rsa_exptmod_asy(rsa, tmp_n, tmp_d, tmp_in_out, tmp_in_out, keywords, out, keybytes, out, out_size, padding);
    return ret;
}

/**
  \brief       decrypt
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result size
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_decrypt(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out, uint32_t *out_size)
{
    csi_error_t ret = CSI_OK;
    uint32_t bit_length = 1024;
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    CSI_PARAM_CHK(context->n, CSI_ERROR);
    CSI_PARAM_CHK(context->d, CSI_ERROR);
    CSI_PARAM_CHK(src, CSI_ERROR);
    CSI_PARAM_CHK(out, CSI_ERROR);
    CSI_PARAM_CHK(out_size, CSI_ERROR);

    if (src_size <= 0 || (context->padding_type != RSA_PADDING_MODE_PKCS1)) {
        ret = CSI_ERROR;
    } else {
        rsa->state.busy = 1U;

        if (context->key_bits == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_decrypt(rsa, (uint8_t *)context->n, (uint8_t *)context->d, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)out, (uint32_t *)out_size, (uint32_t)(context->padding_type), bit_length);
        rsa->state.busy = 0U;
    }

    return ret;
}

int rsa_sign(csi_rsa_t *rsa, uint8_t *n,        uint8_t *d,
             uint8_t *src,      uint32_t src_size,
             uint8_t *signature,
             uint32_t type,     uint32_t keybits_len)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_d[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;

    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(d, keybytes, tmp_d, keywords);

    ret = rsa_padding_pkcs(src,
                           (uint8_t *)tmp_in_out,
                           type,
                           keybytes);

    if (ret != 0) {
        return ret;
    }

    convert_byte_array((uint8_t *)tmp_in_out, (uint8_t *)tmp_in_out, keybytes);

    ret = rsa_exptmod(rsa, tmp_n, tmp_d, tmp_in_out, tmp_in_out, keywords);

    if (ret != 0) {
        return ret;
    }

    convert_bndata_to_buf(tmp_in_out, keywords, signature, keybytes);
    return 0;
}

int rsa_sign_asy(csi_rsa_t *rsa, uint8_t *n,        uint8_t *d,
                 uint8_t *src,      uint32_t src_size,
                 uint8_t *signature,
                 uint32_t type,     uint32_t keybits_len)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_d[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;

    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(d, keybytes, tmp_d, keywords);

    ret = rsa_padding_pkcs(src,
                           (uint8_t *)tmp_in_out,
                           type,
                           keybytes);

    if (ret != 0) {
        return ret;
    }

    convert_byte_array((uint8_t *)tmp_in_out, (uint8_t *)tmp_in_out, keybytes);
    ret = rsa_exptmod_asy(rsa, tmp_n, tmp_d, tmp_in_out, tmp_in_out, keywords, signature, keybytes, NULL, NULL, 0);
    return 0;
}

/**
  \brief       rsa sign
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  signature Pointer to the signature
  \param[in]   hash_type the source data hash type
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_sign(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, csi_rsa_hash_type_t hash_type)
{
    csi_error_t ret = CSI_OK;
    uint32_t bit_length = 1024;
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    CSI_PARAM_CHK(context->n, CSI_ERROR);
    CSI_PARAM_CHK(context->d, CSI_ERROR);
    CSI_PARAM_CHK(src, CSI_ERROR);
    CSI_PARAM_CHK(signature, CSI_ERROR);

    if (src_size <= 0 || (hash_type != RSA_HASH_TYPE_MD5
                          && hash_type != RSA_HASH_TYPE_SHA1
                          && hash_type != RSA_HASH_TYPE_SHA256)) {
        ret = CSI_ERROR;
    } else {
        rsa->state.busy = 1U;

        if (context->key_bits  == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_sign(rsa, (uint8_t *)context->n, (uint8_t *)context->d, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)signature, (uint32_t)(hash_type), bit_length);
        rsa->state.busy = 0U;
    }

    return ret;
}

int rsa_verify(csi_rsa_t *rsa, uint8_t *n,          uint8_t *e,
               uint8_t *src,        uint32_t src_size,
               uint8_t *signature,  uint32_t sig_size,
               uint32_t type,       uint32_t keybits_len,
               uint8_t *result)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_e[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;

    *result = 0;

    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(e, keybytes, tmp_e, keywords);
    convert_buf_to_bndata(signature, sig_size, tmp_in_out, keywords);

    ret = rsa_exptmod(rsa, tmp_n, tmp_e, tmp_in_out, tmp_in_out, keywords);

    if (ret != 0) {
        return ret;
    }

    convert_byte_array((uint8_t *)tmp_in_out, (uint8_t *)tmp_in_out, keybytes);

    ret = rsa_checking_pkcs(src,
                            (uint8_t *)tmp_in_out,
                            keybytes,
                            result,
                            type,
                            keybytes);

    return ret;
}

int rsa_verify_asy(csi_rsa_t *rsa, uint8_t *n,          uint8_t *e,
                   uint8_t *src,        uint32_t src_size,
                   uint8_t *signature,  uint32_t sig_size,
                   uint32_t type,       uint32_t keybits_len)
{
    uint32_t ret;
    uint32_t tmp_n[RSA_KEY_WORD];
    uint32_t tmp_e[RSA_KEY_WORD];
    uint32_t tmp_in_out[RSA_KEY_WORD];
    uint32_t keywords = 0, keybytes = 0;



    keywords = GET_KEY_WORD(keybits_len);
    keybytes = GET_KEY_BYTE(keybits_len);

    convert_buf_to_bndata(n, keybytes, tmp_n, keywords);
    convert_buf_to_bndata(e, keybytes, tmp_e, keywords);
    convert_buf_to_bndata(signature, sig_size, tmp_in_out, keywords);

    ret = rsa_exptmod_asy1(rsa, tmp_n, tmp_e, tmp_in_out, tmp_in_out, keywords, keybytes, src, type);
    return ret;
}
/**
  \brief       rsa verify
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  the signature size
  \param[in]   hash_type the source data hash type
  \return      verify result
*/
bool csi_rsa_verify(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, uint32_t sig_size, csi_rsa_hash_type_t hash_type)
{
    // csi_rsa_callback_t  cb_func;
    bool  result;
    CSI_PARAM_CHK(rsa, false);
    CSI_PARAM_CHK(context, false);
    CSI_PARAM_CHK(context->n, false);
    CSI_PARAM_CHK(context->e, false);
    CSI_PARAM_CHK(src, false);
    CSI_PARAM_CHK(signature, false);


    if (src_size <= 0 || sig_size <= 0 || (context->padding_type != RSA_PADDING_MODE_PKCS1)) {
        result = false;
    } else {

        rsa->state.busy = 1U;

        uint32_t bit_length = 1024;

        if (context->key_bits == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_verify(rsa, (uint8_t *)context->n, (uint8_t *)context->e, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)signature, sig_size, (uint32_t)(context->padding_type), bit_length, (uint8_t *)&result);
        rsa->state.busy = 0U;
    }

    return result;
}

/**
  \brief       encrypt(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_encrypt_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out)
{
    csi_error_t ret = CSI_OK;
    uint32_t bit_length = 1024;

    CSI_PARAM_CHK(rsa, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    CSI_PARAM_CHK(context->n, CSI_ERROR);
    CSI_PARAM_CHK(context->e, CSI_ERROR);
    CSI_PARAM_CHK(src, CSI_ERROR);
    CSI_PARAM_CHK(out, CSI_ERROR);

    if (src_size <= 0  || (context->padding_type != RSA_PADDING_MODE_PKCS1)) {
        ret = CSI_ERROR;
    } else {

        rsa->state.busy = 1U;

        if (context->key_bits == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_config_t.rsa_state = 1;
        rsa_encrypt_asy(rsa, (uint8_t *)context->n, (uint8_t *)context->e, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)out, (uint32_t)(context->padding_type), bit_length);
    }

    return ret;
}

/**
  \brief       decrypt(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result size
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_decrypt_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *out, uint32_t *out_size)
{
    csi_error_t ret = CSI_OK;
    uint32_t bit_length = 1024;
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    CSI_PARAM_CHK(context->n, CSI_ERROR);
    CSI_PARAM_CHK(context->d, CSI_ERROR);
    CSI_PARAM_CHK(src, CSI_ERROR);
    CSI_PARAM_CHK(out, CSI_ERROR);
    CSI_PARAM_CHK(out_size, CSI_ERROR);

    if (src_size <= 0 || (context->padding_type != RSA_PADDING_MODE_PKCS1)) {
        ret = CSI_ERROR;
    } else {
        rsa->state.busy = 1U;

        if (context->key_bits == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_config_t.rsa_state = 2;
        rsa_decrypt_asy(rsa, (uint8_t *)context->n, (uint8_t *)context->d, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)out, (uint32_t *)out_size, (uint32_t)(context->padding_type), bit_length);
    }

    return ret;
}

/**
  \brief       rsa sign(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  signature Pointer to the signature
  \param[in]   hash_type the source data hash type
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_sign_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, csi_rsa_hash_type_t hash_type)
{
    csi_error_t ret = CSI_OK;
    uint32_t bit_length = 1024;
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    CSI_PARAM_CHK(context->n, CSI_ERROR);
    CSI_PARAM_CHK(context->d, CSI_ERROR);
    CSI_PARAM_CHK(src, CSI_ERROR);
    CSI_PARAM_CHK(signature, CSI_ERROR);

    if (src_size <= 0 || (hash_type != RSA_HASH_TYPE_MD5
                          && hash_type != RSA_HASH_TYPE_SHA1
                          && hash_type != RSA_HASH_TYPE_SHA256)) {
        ret = CSI_ERROR;
    } else {
        rsa->state.busy = 1U;

        if (context->key_bits  == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_config_t.rsa_state = 3;
        rsa_sign_asy(rsa, (uint8_t *)context->n, (uint8_t *)context->d, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)signature, (uint32_t)(hash_type), bit_length);
    }

    return ret;
}

/**
  \brief       rsa verify(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  the signature size
  \param[in]   hash_type the source data hash type
  \return      verify result
*/
csi_error_t csi_rsa_verify_async(csi_rsa_t *rsa, csi_rsa_context_t *context, void *src, uint32_t src_size, void *signature, uint32_t sig_size, csi_rsa_hash_type_t hash_type)
{
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    CSI_PARAM_CHK(context->n, CSI_ERROR);
    CSI_PARAM_CHK(context->e, CSI_ERROR);
    CSI_PARAM_CHK(src, CSI_ERROR);
    CSI_PARAM_CHK(signature, CSI_ERROR);


    if (src_size <= 0 || sig_size <= 0 || (context->padding_type != RSA_PADDING_MODE_PKCS1)) {
    } else {

        rsa->state.busy = 1U;

        uint32_t bit_length = 1024;

        if (context->key_bits == RSA_KEY_BITS_2048) {
            bit_length = 2048;
        }

        rsa_config_t.rsa_state = 4;
        rsa_verify_asy(rsa, (uint8_t *)context->n, (uint8_t *)context->e, (uint8_t *)src, (uint32_t)src_size, (uint8_t *)signature, sig_size, (uint32_t)(context->padding_type), bit_length);
    }

    return CSI_OK;
}

/**
  \brief       Get RSA state.
  \param[in]   rsa      rsa handle to operate.
  \param[out]  state    rsa state \ref csi_rsa_state_t.
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_get_state(csi_rsa_t *rsa, csi_rsa_state_t *state)
{
    CSI_PARAM_CHK(rsa, CSI_ERROR);
    *state = rsa->state;
    return CSI_OK;
}

/**
  \brief       Get big prime data
  \param[in]   rsa          rsa handle to operate.
  \param[in]   p            Pointer to the prime
  \param[in]   bit_length   Pointer to the prime bit length
  \return      \ref csi_error_t
*/
csi_error_t csi_rsa_get_prime(csi_rsa_t *rsa, void *p, uint32_t bit_length)
{
    return CSI_UNSUPPORTED;
}
#endif