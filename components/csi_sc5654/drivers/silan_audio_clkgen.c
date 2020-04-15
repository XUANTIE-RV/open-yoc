#include "silan_printf.h"
#include "silan_audio_clkgen.h"
#include "silan_i2s.h"
#include "silan_errno.h"

static inline void clkgen_reg_set(uint32_t val, uint32_t addr, int offset)
{
    uint32_t tmp;
    tmp = __REG32(addr);
    tmp &= (~(0xff  << offset));
    tmp |= (val << offset);
    __REG32(addr) = tmp;
}

void silan_i2s_clock_config(I2S_ID i2s_id, int rate)
{
    uint32_t val;

    switch (rate) {
        case 8000 : { val = 0xfa; break; }
        case 16000: { val = 0xf2; break; }
        case 32000: { val = 0xea; break; }
        case 64000: { val = 0xe2; break; }
        case 12000: { val = 0xda; break; }
        case 24000: { val = 0xd2; break; }
        case 48000: { val = 0xca; break; }
        case 96000: { val = 0xc2; break; }
        case 11025: { val = 0xda; break; }
        case 22050: { val = 0xd2; break; }
        case 44100: { val = 0xca; break; }
        case 88200: { val = 0xc2; break; }
        default:     { val = 0xca; break; }
    }
    switch (i2s_id) {
        case I2S_ID_I1:
            clkgen_reg_set(val, SILAN_SYSCFG_REG5,  8);
            break;
        case I2S_ID_I2:
            clkgen_reg_set(val, SILAN_SYSCFG_REG5, 16);
            break;
        case I2S_ID_I3:
            clkgen_reg_set(val, SILAN_SYSCFG_REG5, 24);
            break;
        case I2S_ID_O1:
            clkgen_reg_set(val, SILAN_SYSCFG_REG6,  0);
            break;
        case I2S_ID_O2:
            clkgen_reg_set(val, SILAN_SYSCFG_REG6,  8);
            break;
        case I2S_ID_O3:
            clkgen_reg_set(val, SILAN_SYSCFG_REG6, 16);
            break;
        case I2S_ID_PCM:
            clkgen_reg_set(val, SILAN_SYSCFG_REG6, 24);
            break;
        default:
            break;
    }

    if ((rate == 11025)||(rate ==22050)||(rate==44100)||(rate==88200)) {
        audiopll_config(AUDIOPLL_45M);
    } else {
        audiopll_config(AUDIOPLL_49M);
    }
}

int32_t silan_i2s_route_config(I2S_ID i2s_id, CODEC_ID codec_id, CLK_MODE master)
{
    AUDIO_ROUTE_REG1 route_reg;
    uint8_t i2s_sel;

    if((i2s_id == I2S_ID_I1) && (codec_id == (CODEC_ID_1|CODEC_ID_2))) {
        SL_LOG("I2S_I1 not support connect to CODEC1/2 both");
        return -EINVAL;
    }

    route_reg.d32 = __REG32(SILAN_SYSCFG_REG17);
    switch (codec_id) {
        case CODEC_ID_EXT:
            i2s_sel = master ? ROUTE_I2S_SEL_CLKGEN : ROUTE_I2S_SEL_EXT;
            break;
        case CODEC_ID_1:
            i2s_sel = master ? ROUTE_I2S_SEL_CLKGEN : ROUTE_I2S_SEL_CODEC1;
            break;
        case CODEC_ID_2:
            i2s_sel = master ? ROUTE_I2S_SEL_CLKGEN : ROUTE_I2S_SEL_CODEC2;
            break;
        case CODEC_ID_3:        //CODEC_ID_1 | CODEC_ID_2
            i2s_sel = master ? ROUTE_I2S_SEL_CLKGEN : ROUTE_I2S_SEL_CODEC2;
            route_reg.b.dac2_sel = 1;
            break;
        default:
            i2s_sel = ROUTE_I2S_SEL_CLKGEN;
            break;
    }

    switch (i2s_id) {
        case I2S_ID_O1:
            if(codec_id == CODEC_ID_2) {
                SL_LOG("I2S_O1<->CODEC_2 Not Permited");
                return -EINVAL;
            }
            route_reg.b.i2s_o1_sel = i2s_sel;
            route_reg.b.i2s_o1_ext_master = master;
            break;
        case I2S_ID_O2:
            if(codec_id == CODEC_ID_1) {
                SL_LOG("I2S_O2<->CODEC_1 Not Permited");
                return -EINVAL;
            }
            route_reg.b.i2s_o2_sel = i2s_sel;
            route_reg.b.i2s_o2_ext_master = master;
            break;
        case I2S_ID_O3:
            if(codec_id > CODEC_ID_EXT) {
                SL_LOG("I2S_O3<->CODEC Not Permited");
                return -EINVAL;
            }
            route_reg.b.i2s_o3_sel = i2s_sel;
            route_reg.b.i2s_o3_ext_master = master;
            break;
        case I2S_ID_I1:
            if(codec_id == CODEC_ID_2) {
                SL_LOG("I2S_I1<->CODEC_2 Not Permited");
                return -EINVAL;
            }
            route_reg.b.i2s_i1_sel = i2s_sel;
            route_reg.b.i2s_i1_ext_master = master;
            break;
        case I2S_ID_I2:
            if(codec_id == CODEC_ID_1) {
                SL_LOG("I2S_I2<->CODEC_1 Not Permited");
                return -EINVAL;
            }
            route_reg.b.i2s_i2_sel = i2s_sel;
            route_reg.b.i2s_i2_ext_master = master;
            break;
        case I2S_ID_I3:
            if(codec_id > CODEC_ID_EXT) {
                SL_LOG("I2S_I3<->CODEC Not Permited");
                return -EINVAL;
            }
            route_reg.b.i2s_i3_sel = i2s_sel;
            route_reg.b.i2s_i3_ext_master = master;
            break;
        case I2S_ID_PCM:
            if(codec_id > CODEC_ID_EXT) {
                SL_LOG("I2S_PCM<->CODEC Not Permited");
                return -EINVAL;
            }
            route_reg.b.i2s_pcm_ext_master = master;
            break;
        default:
            SL_LOG("I2S ID Error");
            return -EINVAL;
            break;
    }
    __REG32(SILAN_SYSCFG_REG17) = route_reg.d32;

    return ENONE;
}

void silan_i2s_route_init(void)
{
    AUDIO_ROUTE_REG2 route_reg;

    route_reg.d32 = __REG32(SILAN_SYSCFG_REG18);
    route_reg.b.codec1_dac_sel = ROUTE_CODEC_SEL_CLKGEN;
    route_reg.b.codec1_adc_sel = ROUTE_CODEC_SEL_CLKGEN;
    route_reg.b.codec2_dac_sel = ROUTE_CODEC_SEL_CLKGEN;
    route_reg.b.codec2_adc_sel = ROUTE_CODEC_SEL_CLKGEN;
    __REG32(SILAN_SYSCFG_REG18) = route_reg.d32;
}

