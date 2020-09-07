/*
 * libaplanis - Audio Post processing library
 */

#ifndef APLANS_H
#define APLANS_H

# ifdef __cplusplus
extern "C" {
# endif

//#include "common.h"
#include <inttypes.h>
typedef char                  ap_int8_t;
typedef unsigned char         ap_uint8_t;
typedef short                 ap_int16_t;
typedef unsigned short        ap_uint16_t;
typedef int                   ap_int32_t;
typedef unsigned int          ap_uint32_t;
typedef int64_t               ap_int64_t;
typedef uint64_t              ap_uint64_t;

#define DECLARE_ALIGNED(n,t,v) 	t __attribute__ ((aligned (n))) v

#if 1
/* Internal calculation accuracy macros */
/* QFBIT  - Floating point */
/* Q16BIT - 16 bit */
/* Q20BIT - 20 bit */
/* Q24BIT - 24 bit */
/* Q32BIT - 32 bit */
/* Input sample data accuracy support */
/* QINFBIT  - Floating point */
/* QIN16BIT - 16 bit */
/* QIN24BIT - 24 bit */
/* QIN32BIT - 32 bit */
/* Output sample data accuracy supports */
/* QOUTFBIT  - Floating point */
/* QOUT16BIT - 16 bit */
/* QOUT24BIT - 24 bit */
/* QOUT32BIT - 32 bit */
#define Q16BIT
//#define Q20BIT
//#define Q24BIT
//#define Q32BIT
//#define QFBIT
#define QIN16BIT
//#define QIN24BIT
//#define QIN32BIT
#define QOUT16BIT
//#define QOUT24BIT
//#define QOUT32BIT
#else

#if defined(Q32BIT) || defined(Q24BIT) || defined(Q20BIT) || defined(Q16BIT) || defined(QFBIT)
#else
#define Q16BIT
#endif

#if defined(QIN32BIT) || defined(QIN16BIT) || defined(QINFBIT)
#else
#define QIN16BIT
#endif

#if defined(QOUT32BIT) || defined(QOUT16BIT) || defined(QOUTFBIT)
#else
#define QOUT16BIT
#endif

#endif

#ifdef QFBIT
#define mSoftwareClip        0     /* no software to do clipping */
#else
#define mSoftwareClip        1     /* using software to do clipping */
#endif
#define mHardwareShift       0     /* has hardware multiplier shift */

/* define data types according to Q-values */
#if defined(QFBIT)

#if defined(Q32BIT) || defined(Q24BIT) || defined(Q20BIT) || defined(Q16BIT)
#error Multiple Internal sample depth defined.
#endif
typedef float SAMPLE;
typedef float USAMPLE
typedef float COEFF;
typedef float SAMPLE32;
typedef float USAMPLE32;
typedef float ACCU;
typedef float UACCU;

#define SAMPLE_ALIGNED       16
#define mSampleBits          0

#elif defined(Q32BIT)

#if defined(QFBIT) || defined(Q24BIT) || defined(Q16BIT)
#error Multiple Internal sample depth defined.
#endif
typedef ap_int32_t  SAMPLE;
typedef ap_uint32_t USAMPLE;
typedef ap_int32_t  COEFF;
typedef ap_int32_t  SAMPLE32;
typedef ap_uint32_t USAMPLE32;
typedef ap_int64_t  ACCU;
typedef ap_uint64_t UACCU;

#define SAMPLE_ALIGNED       8
#define mSampleBits          32

#elif defined(Q24BIT)

#if defined(Q20BIT) || defined(Q16BIT)
#error Multiple Internal sample depth defined.
#endif

typedef ap_int32_t  SAMPLE;
typedef ap_uint32_t USAMPLE;
typedef ap_int32_t  COEFF;
typedef ap_int32_t  SAMPLE32;
typedef ap_uint32_t USAMPLE32;
typedef ap_int64_t  ACCU;
typedef ap_uint64_t UACCU;

#define SAMPLE_ALIGNED       8
#define mSampleBits          24

#elif defined(Q20BIT)

#if defined(Q16BIT)
#error Multiple Internal sample depth defined.
#endif

typedef ap_int32_t  SAMPLE;
typedef ap_uint32_t USAMPLE;
typedef ap_int32_t  COEFF;
typedef ap_int32_t  SAMPLE32;
typedef ap_uint32_t USAMPLE32;
typedef ap_int64_t  ACCU;
typedef ap_uint64_t UACCU;

#define SAMPLE_ALIGNED       8
#define mSampleBits          20

#elif defined(Q16BIT)

typedef ap_int16_t  SAMPLE;
typedef ap_uint16_t USAMPLE;
typedef ap_int16_t  COEFF;
typedef ap_int32_t  SAMPLE32;
typedef ap_uint32_t USAMPLE32;
typedef ap_int32_t  ACCU;
typedef ap_uint32_t UACCU;

#define SAMPLE_ALIGNED       4
#define mSampleBits          16
#else
#error None sample depth defined.
#endif

/* Input data types */
#if defined(QIN32BIT)
typedef ap_int32_t INSAMPLE;
#define mInSampleBits        32
#elif defined(QIN24BIT)
typedef ap_int32_t INSAMPLE;
#define mInSampleBits        24
#elif defined(QIN16BIT)
typedef ap_int16_t INSAMPLE;
#define mInSampleBits        16
#else
typedef float INSAMPLE;
#define mInSampleBits        0
#endif

/* Output data types */
#if defined(QOUT32BIT)
typedef ap_int32_t OUTSAMPLE;
#define mOutSampleBits       32
#elif defined(QOUT24BIT)
typedef ap_int32_t OUTSAMPLE;
#define mOutSampleBits       24
#elif defined(QOUT16BIT)
typedef ap_int16_t OUTSAMPLE;
#define mOutSampleBits       16
#else
typedef float OUTSAMPLE;
#define mOutSampleBits       0
#endif

#if 0
#if (mSampleBits == 0)
#define QONE                 1.0f
#elif (msampleBits > 16)
#define QONE                 (ACCU)2147483647
#else
#define QONE                 (ACCU)32767
#endif

#define QHALF                (QONE)/2
#endif


#define Q15ONE               32767

#define mMin(b)              (~0 << ((b) - 1))
#define mMax(b)              (~mMin(b))
#define fMaxV                1.0
#define fMinV                (-(fMaxV))

#define Clip(a, min, max)    ((a)>(max)?(max):((a)<(min)?(min):(a)))

#if (mSampleBits) != 0
#define mMinV                     mMin(mSampleBits)
#define mMaxV                     mMax(mSampleBits)
#define mMinVIn                   mMin(mInSampleBits)
#define mMaxVIn                   mMax(mInSampleBits)
#define mMinVOut                  mMin(mOutSampleBits)
#define mMaxVOut                  mMax(mOutSampleBits)
#define ClipCoeff(a, min, max)    ((a)>(max)?(max):((a)<(min)?(min):(a)))
#else
#define mMinV                     fMinV
#define mMaxV                     fMaxV
#define mMinVIn                   fMinV
#define mMaxVIn                   fMaxV
#define mMinVOut                  fMinV
#define mMaxVOut                  fMaxV
#define ClipCoeff(a, min, max)    (a)
#endif
/* Macros for I/O sample to/from internal sample */
#define FtoI(t, f, b)        ((t)Clip(((f) * mMax(b)/fMaxV) + \
                              ((f) >= 0 ? 0.5 : -0.5), mMin(b), mMax(b)))

#define ItoF(t, i, b)        ((t)Clip((t)(i) * fMaxV/mMax(b), fMinV, fMaxV))

/* InputToSample() macro */
#define I_B_DIFF            ((mSampleBits) - (mInSampleBits))
#if (mSampleBits) != 0 && (mInSampleBits) != 0 && (I_B_DIFF) > 0
#define InputToSample(i)    (SAMPLE)(i)
#elif (mSampleBits) != 0 && (mInSampleBits) != 0 && (I_B_DIFF) <= 0
#define InputToSample(i)    (SAMPLE)(i)
#elif (mSampleBits) != 0 && (mInSampleBits) == 0
#define InputToSample(i)    FtoI(SAMPLE, i, mSampleBits)
#elif (mSampleBits) == 0 && (mInSampleBits) != 0
#define InputToSample(i)    ItoF(SAMPLE, i, mInSampleBits)
#else
#define InputToSample(i)    ((SAMPLE)Clip(i, mMinV, mMaxV))
#endif

/* SampleToOutput() macro */
#define O_B_DIFF            ((mInSampleBits) - (mOutSampleBits))
#if (mSampleBits) != 0 && (mOutSampleBits) != 0 && (O_B_DIFF) > 0
#define SampleToOutput(o)   (OUTSAMPLE)(Clip(o, mMinVOut, mMaxVOut))
#elif (mSampleBits) != 0 && (mOutSampleBits) != 0 && (O_B_DIFF) <= 0
#define SampleToOutput(o)   (OUTSAMPLE)(Clip((o << (-O_B_DIFF)), mMinVOut, mMaxVOut))
#elif (mSampleBits) != 0 && (mOutSampleBits) == 0
#define SampleToOutput(o)   ItoF(OUTSAMPLE, o, mSampleBits)
#elif (mSampleBits) == 0 && (mOutSampleBits) != 0
#define SampleToOutput(o)   FtoI(OUTSAMPLE, o, mOutSampleBits)
#else
#define SampleToOutput(o)   (OUTSAMPLE)(Clip(o, mMinV, mMaxV))
#endif

#ifdef HIFI2
#include <xtensa/tie/xt_hifi2.h>
#define HIFI            2
#elifdef HIFI3
#include <xtensa/tie/xt_hifi3.h>
#define HIFI            3
#else
#define HIFI            0
#endif

#define HIFI_16B        (HIFI != 0) && (mSampleBits == 16)
#define HIFI_24B        (HIFI != 0) && (mSampleBits == 24)
#define HIFI_32B        (HIFI != 0) && (mSampleBits == 32)

/* Audio processor modules we have */
//#define HAVE_MIX
//#define HAVE_VBASS
//#define HAVE_DIALENH
//#define HAVE_3D
//#define HAVE_SURROUND
//#define HAVE_STEREO2MONO
//#define HAVE_MONO2STEREO
#define HAVE_RESAMPLE
//#define HAVE_EQ
//#define HAVE_LOUDCTL
//#define HAVE_PTS
//#define HAVE_VOLUMECTL
//#define HAVE_AUTOMUTE
//#define HAVE_REVERB
//#define HAVE_ECHO
//#define HAVE_BASSTREBLE
//#define HAVE_COMPRESSOR
//#define HAVE_VOICECANCEL
//#define HAVE_CUSTOM
//#define HAVE_SHOW_PS
//#define HAVE_SHOW_PEAK
//#define HAVE_MUTE_DECT
//#define HAVE_VAD
//#define HAVE_NS
//#define HAVE_AGC
//#define HAVE_AEC
//#define HAVE_AFC
//#define HAVE_SOSF
//#define HAVE_DELAY
//#define HAVE_LIMIT
//#define HAVE_DIRECTSOUND
//#define HAVE_NOISEGATE
//#define HAVE_ENERGY


/* return values */
#define AP_LEVEL_TOO_SMALL   -1
#define AP_LEVEL_TOO_LARGE   -2
#define AP_NULL_MEMORY       -3
#define AP_PARAM_INVALID     -4
#define AP_OK                 1

/* general definitions */
#define MAX_CH                8
#define NUM_FS               12
static const int apfs[NUM_FS] = {8000, 11025, 12000, 16000, 22050, 24000,
                                 32000, 44100, 48000, 64000, 88200, 96000
                                };

#define MAXDB                63
#define MAXDB_24BITS         54
#define MINDB               -64
static const ap_int32_t db_table[MAXDB - MINDB + 1] = {
    /*   0 -  12 */  20, 23, 26, 29, 32, 36, 41, 46, 51, 58, 65, 73, 82,
    /*  13 -  23 */  92, 103, 116, 130, 146, 164, 184, 206, 231, 260, 292,
    /*  24 -  34 */ 327, 367, 412, 462, 519, 582, 653, 733, 823, 923, 1036,
    /*  35 -  44 */ 1162, 1304, 1463, 1642, 1842, 2067, 2319, 2602, 2920, 3276,
    /*  45 -  54 */ 3676, 4125, 4628, 5193, 5827, 6538, 7335, 8230, 9235, 10362,
    /*  55 -  63 */ 11626, 13045, 14636, 16422, 18426, 20675, 23197, 26028, 29204,
    /*  64 -  72 */ 32768, 36766, 41252, 46286, 51933, 58270, 65380, 73358, 82309,
    /*  73 -  80 */ 92352, 103621, 116265, 130451, 146369, 164229, 184268, 206752,
    /*  81 -  88 */ 231979, 260285, 292045, 327680, 367662, 412524, 462860, 519337,
    /*  89 -  96 */ 582706, 653807, 733584, 823094, 923527, 1036215, 1162652, 1304517,
    /*  97 - 104 */ 1463692, 1642290, 1842680, 2067520, 2319796, 2602854, 2920451, 3276800,
    /* 105 - 112 */ 3676629, 4125246, 4628604, 5193378, 5827066, 6538075, 7335840, 8230951,
    /* 113 - 119 */ 9235278, 10362151, 11626524, 13045173, 14636931, 16422905, 18426800,
    /* 120 - 126 */ 20675208, 23197962, 26028554, 29204514, 32768000, 36766296, 41252460,
    /* 127       */ 46286040
};

typedef struct _aFormat {
    int8_t  channels;
    int32_t sampleRate;
    int8_t  bits;
} aFormat;

typedef enum aQuality_ {
    DEFAULT_QUALITY = 0,
    MED_QUALITY     = 1,
    HIGH_QUALITY    = 2
} aQuality;

static inline short clamp(int sample)
{
    if ((sample >> 15) ^ (sample >> 31))
        sample = 0x7fff ^ (sample >> 31);
    return sample;
}


static inline void copy_audio_format(aFormat *dst, aFormat *src)
{
    if (!src || !dst) return;
    dst->channels   = src->channels;
    dst->sampleRate = src->sampleRate;
    dst->bits       = src->bits;
}

static inline int cmpaFormat(aFormat *f1, aFormat *f2)
{
    return (!(f1 && f2 &&
              (f1->sampleRate == f2->sampleRate) &&
              (f1->bits == f2->bits) &&
              (f1->channels == f2->channels)));
}

/* mix */
#ifdef HAVE_MIX
void *mix_init(aQuality);
int   mix_config(void *, aFormat *, aFormat *);
int   mix_set_mode(void *, int);
int   mix_set_surround(void *, int);
int   mix_set_headphone(void *, int);
int   mix_set_hori_level(void *, int);

int   mix_set_vbass(void *, int);
int   mix_set_vbass_level(void *, int);
int   mix_set_vbass_speaker_sz(void *, int);
int   mix_set_vbass_input_gain(void *, int);

int   mix_set_dialog(void *, int);
int   mix_set_dialog_base(void *, int);
int   mix_set_dialog_treble(void *, int);

int   mix_set_rdelay(void *, int);
int   mix_get_mode(void *, int *);
int   mix_get_surround(void *, int *);
int   mix_get_headphone(void *, int *);
int   mix_get_hori_level(void *, int *);

int   mix_get_vbass(void *, int *);
int   mix_get_vbass_level(void *, int *);
int   mix_get_vbass_speaker_sz(void *, int *);
int   mix_get_vbass_input_gain(void *, int *);

int   mix_get_dialog(void *, int *);
int   mix_get_dialog_base(void *, int *);
int   mix_get_dialog_treble(void *, int *);

int   mix_get_input_gain(void *, int *);
int   mix_get_speaker_size(void *, int *);
int   mix_get_rdelay(void *, int *);
int   mix_process(void *, SAMPLE *, SAMPLE *, int, int);
int   mix_finish(void *);
#endif /* HAVE_MIX */

/* resample */
#ifdef HAVE_RESAMPLE
void *res_init(aQuality);
int   res_config(void *, aFormat *, aFormat *);
int   res_process(void *, SAMPLE *, SAMPLE *, int, int *);
int   res_finish(void *);

#endif /* HAVE_RESAMPLE */

/* equalization */
#ifdef HAVE_EQ
void *equ_init(int, int, aQuality);
int   equ_config(void *, aFormat *);
int   equ_set_bands(void *, int *);
int   equ_set_max_gain(void *, int *);
int   equ_set_on(void *, st_eq_para *);
int   equ_set_type(void *, st_eq_para *);
int   equ_set_fc(void *, st_eq_para *);
int   equ_set_gain(void *, st_eq_para *);
int   equ_set_q(void *, st_eq_para *);
int   equ_set_width(void *, st_eq_para *);
int   equ_set_level_out(void *, int *);

int   equ_level_out(void *, int);
int   equ_get_level(void *, int *);
int   equ_get_level_range(void *, int *);
int   equ_get_bands(void *, int *);
int   equ_get_gain_range(void *, int *);

int   equ_process(void *, SAMPLE *, SAMPLE *, int);
int   equ_finish(void *);
#endif /* HAVE_EQ */

/* loudnesscontrol */
#ifdef HAVE_LOUDCTL
#define MAX_LDC_SIGNAL   mMaxV
#define MAX_LDC_ENERGY   mMaxV
void *ldc_init(aQuality);
int   ldc_config(void *, aFormat *);
int   ldc_set_signal_level(void *, int);
int   ldc_set_energy_level(void *, int);
int   ldc_get_signal_level(void *, int *);
int   ldc_get_energy_level(void *, int *);
int   ldc_process(void *, SAMPLE *, SAMPLE *, int);
int   ldc_finish(void *);
#endif /* HAVE_LOUDCTL */


/* pitch and/or time shift */
#ifdef HAVE_PTS
#define MIN_KEY              25
#define MAX_KEY              400
#define KEY_ZERO             100
typedef enum {
    TIME_SCALE = 0, // time scale only
    PITCH_SHIFT,    // pitch shift only
    PTIME_SCALE     // both time and pitch shift
} pts_mode;

void *pts_init(aQuality, pts_mode);
int   pts_config(void *, aFormat *);
int   pts_process(void *, SAMPLE *, SAMPLE *, int);
int   pts_get_level_range(void *, int *);
int   pts_set_level(void *, int);
int   pts_set_channel_on(void *, int *);
int   pts_finish(void *);
#endif /* HAVE_PTS */

/* volumecontrol */
#ifdef HAVE_VOLUMECTL
void *voc_init(aQuality);
int   voc_config(void *, aFormat *);
int   voc_process(void *, SAMPLE *, SAMPLE *, int);
int   voc_get_db_range(void *, int *);
int   voc_set_volume(void *, int *);
int   voc_finish(void *);
#endif /* HAVE_VOLUMECTL */

/* auto mute */
#ifdef HAVE_AUTOMUTE
void *amu_init(aQuality);
int   amu_config(void *, aFormat *);
int   amu_get_db_range(void *, int *);
int   amu_set_db(void *, int *);
int   amu_set_interval(void *, int *);
int   amu_process(void *, SAMPLE *, SAMPLE *, int);
int   amu_finish(void *);
#endif /* HAVE_AUTOMUTE */

/* reverb */
#ifdef HAVE_REVERB
typedef enum {
    VERB_RESET,
    VERB_VOCAL_I,
    VERB_VOCAL_II,
    VERB_BATHROOM,
    VERB_SMALL_ROOM1,
    VERB_SMALL_ROOM2,
    VERB_MEDIUM_ROOM,
    VERB_LARGE_ROOM,
    VERB_CHURCH_HALL,
    VERB_SANCTUARY,
    VERB_TYPE_MAX,
} reverb_type;

static const char reverb_name[VERB_TYPE_MAX][32] = {
    "Reset",
    "Vocal I",
    "Vocal II",
    "Bathroom",
    "Small Room Bright",
    "Small Room Dark",
    "Medium Room",
    "Large Room",
    "Church Hall",
    "Sanctuary"
};

void *rev_init(aQuality);
int   rev_config(void *, aFormat *);
int   rev_process(void *, SAMPLE *, SAMPLE *, int);
int   rev_get_type(void *, int *);
int   rev_set_type(void *, int);
int   rev_finish(void *);
#endif /* HAVE_REVERB */

/* acoustic echo cancel */
#ifdef HAVE_AEC
void *aec_init(aQuality);
int   aec_process(void *, SAMPLE *, int, int);
int   aec_func(void *, SAMPLE *, int );
int   aec_finish(void *);
#endif /* HAVE_AEC */

/* echo */
#ifdef HAVE_ECHO
#define MAX_BANDS              20//10
void *eho_init(aQuality);
int   eho_config(void *, aFormat *);
int   eho_set_dtype(void *, int);
int   eho_set_gain(void *, int);
int   eho_set_stype(void *, int);
int   eho_set_shift(void *, int);
int   eho_set_number(void *, int);
/* For ALL */
int   eho_set_delay(void *, int);
int   eho_process(void *, SAMPLE *, SAMPLE *, int);
int   eho_finish(void *);
#endif /* HAVE_ECHO */

/* basstreble */
#ifdef HAVE_BASSTREBLE
void *btr_init(int, aQuality);
int   btr_config(void *, aFormat *);
int   btr_set_bass_coeff(void *, int *);
int   btr_set_treble_coeff(void *, int *);
int   btr_process(void *, SAMPLE *, SAMPLE *, int);
int   btr_finish(void *);
#endif /* HAVE_BASSTREBLE */

/* dynamic compressor */
#ifdef HAVE_COMPRESSOR
void *drc_init(aQuality, int);
int   drc_config(void *, aFormat *);
int   drc_set_threshold(void *, int);
int   drc_set_noisefloor(void *, int);
int   drc_set_ratio(void *, int);
int   drc_set_attack(void *, int);
int   drc_set_release(void *, int);
int   drc_set_makeup(void *, int);
int   drc_set_peak(void *, int);
int   drc_set_logdomain(void *, int);
int   drc_set_kwidth(void *, int);
int   drc_process(void *, SAMPLE *, SAMPLE *, int);
int   drc_finish(void *);
#endif /* HAVE_COMPRESSOR */

/* Voice Activity Detection */
#ifdef HAVE_VAD
typedef enum {
    VAD_QUALITY_BEST = 0,
    VAD_LOW_BITRATE,
    VAD_AGGRESSIVE,
    VAD_VERY_AGGRESSIVE
} vad_mode_t;

void *vad_init(aQuality);
int   vad_config(void *, aFormat *);
int   vad_set_mode(void *, int);
int   vad_process(void *, SAMPLE *, SAMPLE *, int);
int   vad_finish(void *);
#endif

/* Noise Suppression */
#ifdef HAVE_NS
typedef enum {
    NS_LOW = 0,
    NS_MODERATE,
    NS_HIGH,
    NS_VERY_HIGH
} ns_mode_t;

void *ns_init(aQuality);
int   ns_config(void *, aFormat *);
int   ns_set_mode(void *, int);
int   ns_process(void *, SAMPLE *, SAMPLE *, int);
int   ns_finish(void *);
#endif

/* Automatic Gain Control */
#ifdef AHVE_AGC
#endif

/* Acoustic Echo Cancellation */
#ifdef HAVE_AEC
#endif

/* Voice Cancellation */
#ifdef HAVE_VOICECANCEL
void*vcl_init(aQuality quality);
int  vcl_config(void *pp, aFormat *in);
int  vcl_set_mode(void *pp, int mode);
int  vcl_set_level(void *pp, int level);
int  vcl_finish(void *pp);
#endif

/* Second Order Sections Filter */
#ifdef HAVE_SOSF
void *sos_init(int);
int   sos_config(void *, aFormat *);
int   sos_set_filter_bank(void *, int *);
int   sos_set_on(void *, int *);
int   sos_process(void *, SAMPLE *, SAMPLE *, int);
int   sos_finish(void *);
#endif /* HAVE_VOICECANCEL */

#ifdef HAVE_SHOW_PS
void *sps_init(void);
int   sps_config(void *pp, aFormat *in);
int   sps_process(void *pp, SAMPLE *x, SAMPLE *y, int frames);
int   sps_finish(void *pp);
#endif

#ifdef HAVE_SHOW_PEAK
void *peak_init(void);
int   peak_config(void *pp, aFormat *in);
int   peak_process(void *pp, SAMPLE *x, SAMPLE *y, int frames);
int   peak_finish(void *pp);
#endif

#ifdef HAVE_MUTE_DECT
void *mute_dect_init(void);
int   mute_dect_config(void *pp, aFormat *in);
int   mute_dect_process(void *pp, SAMPLE *x, SAMPLE *y, int frames);
int   mute_dect_finish(void *pp);
#endif

#ifdef HAVE_DELAY
void *delay_init(aQuality quality);
int delay_config(void *pp, aFormat *in);
int delay_set_delay(void *pp, int * delay);
int delay_process(void *pp, SAMPLE *inbuf, SAMPLE *outbuf, size_t insize);
int delay_finish(void *pp);
#endif

#ifdef HAVE_DIRECTSOUND
void *direct_sound_init(void);
int direct_sound_process(void *pp, SAMPLE *inbuf, SAMPLE *outbuf, size_t size);
int   direct_sound_config(void *pp, aFormat *in);
int   direct_sound_finish(void *pp);
int   direct_sound_getbuf(void *pp, SAMPLE *buf);
#endif

#ifdef HAVE_NOISEGATE
#define MAX_CH 6
void *noise_gate_init(aQuality quality);
int noise_gate_process(void *pp, SAMPLE *x, SAMPLE *y, size_t nb_samples);
int noise_gate_set_limit(void *pp, int limit[MAX_CH]);
int noise_gate_config(void *pp, aFormat *in);
int noise_gate_finish(void *pp);
#endif

#ifdef HAVE_ENERGY
void *energy_init(void);
int  energy_config(void *pp, aFormat *in);
int  energy_process(void *pp, SAMPLE *x, SAMPLE *y, int nb_samples);
int  energy_result(void *pp, void *result);
int  energy_finish(void *pp);
#endif

#ifdef HAVE_AFC
#endif

# ifdef __cplusplus
}
# endif

#endif
