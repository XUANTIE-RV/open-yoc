/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/sf.h"
#include "avutil/common.h"

static struct {
    const char  *name;
    esf_t       type;
    sf_t        sf;
} _esf_maps[] = {
    { "s8",      ESF_S8,       sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(8)       },
    { "s16le",   ESF_S16LE,    sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(16)      },
    { "s16be",   ESF_S16BE,    sf_make_signed(1) | sf_make_bigendian(1) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(16)      },
    { "s32le",   ESF_S32LE,    sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(32)      },
    { "s32be",   ESF_S32BE,    sf_make_signed(1) | sf_make_bigendian(1) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(32)      },
    { "u8",      ESF_U8,       sf_make_signed(0) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(8)       },
    { "u16le",   ESF_U16LE,    sf_make_signed(0) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(16)      },
    { "u16be",   ESF_U16BE,    sf_make_signed(0) | sf_make_bigendian(1) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(16)      },
    { "u32le",   ESF_U32LE,    sf_make_signed(0) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(32)      },
    { "u32be",   ESF_U32BE,    sf_make_signed(0) | sf_make_bigendian(1) | sf_make_planar(0) | sf_make_float(0) | sf_make_bit(32)      },

    { "f32le",   ESF_F32LE,    sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(1) | sf_make_bit(32)      },
    { "f32be",   ESF_F32BE,    sf_make_signed(1) | sf_make_bigendian(1) | sf_make_planar(0) | sf_make_float(1) | sf_make_bit(32)      },
    { "f64le",   ESF_F64LE,    sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(0) | sf_make_float(1) | sf_make_bit(64)      },
    { "f64be",   ESF_F64BE,    sf_make_signed(1) | sf_make_bigendian(1) | sf_make_planar(0) | sf_make_float(1) | sf_make_bit(64)      },

    { "s8p",     ESF_S8P,      sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(8)       },
    { "s16lep",  ESF_S16LEP,   sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(16)      },
    { "s16bep",  ESF_S16BEP,   sf_make_signed(1) | sf_make_bigendian(1) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(16)      },
    { "s32lep",  ESF_S32LEP,   sf_make_signed(1) | sf_make_bigendian(0) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(32)      },
    { "s32bep",  ESF_S32BEP,   sf_make_signed(1) | sf_make_bigendian(1) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(32)      },
    { "u8p",     ESF_U8P,      sf_make_signed(0) | sf_make_bigendian(0) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(8)       },
    { "u16lep",  ESF_U16LEP,   sf_make_signed(0) | sf_make_bigendian(0) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(16)      },
    { "u16bep",  ESF_U16BEP,   sf_make_signed(0) | sf_make_bigendian(1) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(16)      },
    { "u32lep",  ESF_U32LEP,   sf_make_signed(0) | sf_make_bigendian(0) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(32)      },
    { "u32bep",  ESF_U32BEP,   sf_make_signed(0) | sf_make_bigendian(1) | sf_make_planar(1) | sf_make_float(0) | sf_make_bit(32)      },
};

/**
 * @brief  get aformat info from sf_t
 * @param  [in] sf
 * @param  [in] outf
 * @return
 */
void get_aformat_from_sf(sf_t sf, aformat_t *outf)
{
    if (outf) {
        memset(outf, 0, sizeof(aformat_t));
        outf->be      = sf_get_bigendian(sf);
        outf->bits    = sf_get_bit(sf);
        outf->issign  = sf_get_signed(sf);
        outf->planar  = sf_get_planar(sf);
        outf->isfloat = sf_get_float(sf);
    }
}

/**
 * @brief  get esf type from sf_t
 * @param  [in] sf
 * @return ESF_NONE on error
 */
esf_t get_esf_type(sf_t sf)
{
    int i;
    esf_t type = ESF_NONE;
    sf_t _sf = sf_make_signed(sf_get_signed(sf)) | sf_make_bigendian(sf_get_bigendian(sf)) |
               sf_make_planar(sf_get_planar(sf)) | sf_make_bit(sf_get_bit(sf)) | sf_make_float(sf_get_float(sf));

    for (i = 0; i < ARRAY_SIZE(_esf_maps); i++) {
        if (_sf == _esf_maps[i].sf)
            return _esf_maps[i].type;
    }

    return type;
}

/**
 * @brief  get format str of sf
 * @param  [in] sf
 * @return not null
 */
char* sf_get_format(sf_t sf)
{
    static char buf[128];

    snprintf(buf, sizeof(buf), "sf = %u, rate = %u, ch = %u, bits = %u, siged = %u, float = %u, endian = %u",
             sf, sf_get_rate(sf), sf_get_channel(sf), sf_get_bit(sf), sf_get_signed(sf), sf_get_float(sf),
             sf_get_bigendian(sf));

    return buf;
}

/**
 * @brief  get sf name
 * @param  [in] sf
 * @return "unknown" on error
 */
const char* get_sf_name(sf_t sf)
{
    int i;
    sf_t _sf = sf_make_signed(sf_get_signed(sf)) | sf_make_bigendian(sf_get_bigendian(sf)) |
               sf_make_planar(sf_get_planar(sf)) | sf_make_bit(sf_get_bit(sf)) | sf_make_float(sf_get_float(sf));

    for (i = 0; i < ARRAY_SIZE(_esf_maps); i++) {
        if (_sf == _esf_maps[i].sf)
            return _esf_maps[i].name;
    }

    return "unknown";
}

/**
 * @brief  get esf name
 * @param  [in] esf
 * @return "unknown" on error
 */
const char* get_esf_name(esf_t esf)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(_esf_maps); i++) {
        if (esf == _esf_maps[i].type)
            return _esf_maps[i].name;
    }

    return "unknown";
}

/**
 * @brief  get buffer size of given number samples
 * @param  [out] linesize
 * @param  [in] sf
 * @param  [in] nb_samples
 * @return > 0 on success
 */
int sf_get_buffer_size(int *linesize, sf_t sf, int nb_samples)
{
    int rc = -1;

    if (!(linesize && (nb_samples > 0))) {
        return rc;
    }

    rc = sf_get_frame_size(sf) * nb_samples;
    if (rc > 0) {
        *linesize = rc;
    }

    return rc;
}


