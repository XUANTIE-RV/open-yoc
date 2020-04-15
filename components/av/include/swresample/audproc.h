/*
 * Modules of audio processors
 * definition header file
 */

#ifndef AUDPROC_H
#define AUDPROC_H

#define HAVE_AUDPROC

#ifdef HAVE_AUDPROC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "swresample/audproc_common.h"
#include "swresample/aplanis.h"

//#define HAVE_PTHREAD

#define FPRINTF(a)           PRINTF_DBG a

// Audio processor state
typedef enum {
    AUDPROC_STATE_INIT,      // initial state before created
    AUDPROC_STATE_CREATED,   // engine created
    AUDPROC_STATE_CONFIGING, // configuration changed
    AUDPROC_STATE_ACTIVE     // active/enabled
} audproc_state;

// Audio processor context
typedef struct _AudProc AudProc;
struct _AudProc {
    void *obj;                     // target
    char name[64];
    int procId;                    // ID (enum audproc_id)
    int state;                     // current state (audproc_effect_state)
    int onoff;
    int (*init)(AudProc *);
    int (*set_parameter)(AudProc *, int cmd, void *value);
    int (*get_parameter)(AudProc *, int cmd, size_t *size, void *value);
    int (*proc)(AudProc *);
    int (*finish)(AudProc *);

    aQuality  quality;              // processing quality

    SAMPLE   *in;                   // input buffer pointer
    int   *sin;                  // optional second input buffer pointer

    int      inSize;                // input size of 'SAMPLE'
    aFormat  inFormat;              // input audio format

    int      sinSize;               // input size of 'SAMPLE' of second
    aFormat  sinFormat;             // input audio format of second

    SAMPLE   *out;                  // output buffer pointer
    int   *sout;                 // optional second output buffer pointer

    int      outSize;               // output size of 'SAMPLE' of second
    aFormat  outFormat;             // output audio format of second

    int      soutSize;              // output size of 'SAMPLE'
    aFormat  soutFormat;            // output audio format

    int allocated;                  // 1 - output is allocated by itself
    // 0 - output using the input buffer of next
    // processor in the processor chain
};

// types of audio processor modules
// see plist_name
typedef enum {
    AUDPROC_NULL = -1,
    AUDPROC_MIX,         // Up or Down Mixing
    AUDPROC_VBASS,       // Virtual Bass
    AUDPROC_DIALENH,     // Dialog Enhancement
    AUDPROC_3D,          // 3D Audio
    AUDPROC_SURROUND,    // Virtual Surround
    AUDPROC_STEREO2MONO, // Stereo to Mono
    AUDPROC_MONO2STEREO, // Mono to Stereo
    AUDPROC_RESAMPLE,    // Resample
    AUDPROC_EQ,          // Digital Equalizer
    AUDPROC_LOUDCTL,     // Loudness Control
    AUDPROC_PTS,         // Pitch and/or Time Shift
    AUDPROC_VOLUMECTL,   // Volume Control
    AUDPROC_ALC,         // Level Control
    AUDPROC_AUTOMUTE,    // Auto Mute
    AUDPROC_REVERB,      // Reverbration
    AUDPROC_ECHO,        // Echo
    AUDPROC_BASSTREBLE,  // Bass and Treble
    AUDPROC_COMPRESSOR,  // Dynamic Range Compressor
    AUDPROC_VOICECANCEL, // Voice Cancel
    AUDPROC_CUSTOM,      // Custom
    AUDPROC_SHOW_PS,     // Show Ps
    AUDPROC_SHOW_PEAK,   // Show Peak
    AUDPROC_MUTE_DECT,   // Mute Detection
    AUDPROC_VAD,         // Voice Activity Detection
    AUDPROC_NS,          // Noise Suppression
    AUDPROC_AGC,         // Automatic Gain Control
    AUDPROC_AEC,         // Acoustic Echo Control
    AUDPROC_AFC,         // Acoustic Feedback Control
    AUDPROC_SOSF,        // Second Order Sections Filter
    AUDPROC_DELAY,       // Delay
    AUDPROC_LIMIT,       // Limit
    AUDPROC_DIRECT_SOUND,// Direct Sound
    AUDPROC_NOISE_GATE,  // Noise Gate
    AUDPROC_ENERGY,      // Energy
    AUDPROC_NUMS,

    /* INTERNAL AUDPROC */
    AUDPROC_SOUND_EQ = AUDPROC_NUMS+9,
} audproc_id;

// names/id/entry of audio processor modules
typedef struct {
    AudProc    *(*create)(char *);
    void        (*finish)(AudProc *);
    audproc_id  id;
    char        name[32];
} audproc_node;

typedef struct {
    char ch;
    char bits;
    char mode;
    char level;
    int  fs;
    int  frame_size;
} vc_ctrl;

typedef struct {
    int flt_id;
    int on;
    int gain;
    int fc;
    int q;
} ae_eq_ctrl;

#ifdef HAVE_MIX
extern AudProc *mix_proc_create(char *);
#endif
#ifdef HAVE_3D
extern AudProc *_3d_proc_create(char *);
#endif
#ifdef HAVE_VBASS
extern AudProc *vbass_proc_create(char *);
#endif
#ifdef HAVE_DIALENH
extern AudProc *dialenh_proc_create(char *);
#endif
#ifdef HAVE_SURROUND
extern AudProc *surround_proc_create(char *);
#endif
#ifdef HAVE_STEREO2MONO
extern AudProc *stereo2mono_proc_create(char *);
#endif
#ifdef HAVE_MONO2STEREO
extern AudProc *mono2stereo_proc_create(char *);
#endif
#ifdef HAVE_RESAMPLE
extern AudProc *resample_proc_create(char *);
#endif
#ifdef HAVE_EQ
extern AudProc *eq_proc_create(char *);
#endif
#ifdef HAVE_LOUDCTL
extern AudProc *loudctl_proc_create(char *);
#endif
#ifdef HAVE_PTS
extern AudProc *ps_proc_create(char *);
#endif
#ifdef HAVE_VOLUMECTL
extern AudProc *volumectl_proc_create(char *);
#endif
#ifdef HAVE_AUTOMUTE
extern AudProc *automute_proc_create(char *);
#endif
#ifdef HAVE_REVERB
extern AudProc *reverb_proc_create(char *);
#endif
#ifdef HAVE_ECHO
extern AudProc *echo_proc_create(char *);
#endif
#ifdef HAVE_BASSTREBLE
extern AudProc *basstreble_proc_create(char *);
#endif
#ifdef HAVE_COMPRESSOR
extern AudProc *compressor_proc_create(char *);
#endif
#ifdef HAVE_VAD
extern AudProc *vad_proc_create(char *);
#endif
#ifdef HAVE_NS
extern AudProc *ns_proc_create(char *);
#endif
#ifdef HAVE_AGC
extern AudProc *agc_proc_create(char *);
#endif
#ifdef HAVE_AEC
extern AudProc *aec_proc_create(char *);
#endif
#ifdef HAVE_AFC
extern AudProc *afc_proc_create(char *);
#endif
#ifdef HAVE_SOSF
extern AudProc *ss_proc_create(char *);
#endif
#ifdef HAVE_VOICECANCEL
extern AudProc *vcctl_proc_create(char *);
#endif
#ifdef HAVE_CUSTOM
extern AudProc *custom_proc_create(char *);
#endif
#ifdef HAVE_SHOW_PS
extern AudProc *show_ps_proc_create(char *);
#endif
#ifdef HAVE_SHOW_PEAK
extern AudProc *show_peak_proc_create(char *);
#endif
#ifdef HAVE_MUTE_DECT
extern AudProc *mute_dect_proc_create(char *);
#endif
#ifdef HAVE_DELAY
extern AudProc *delay_proc_create(char *);
#endif
#ifdef HAVE_LIMIT
extern AudProc *limit_proc_create(char *);
#endif
#ifdef HAVE_DIRECTSOUND
extern AudProc *d_sound_proc_create(char *);
#endif
#ifdef HAVE_NOISEGATE
extern AudProc *ng_proc_create(char *);
#endif
#ifdef HAVE_ENERGY
extern AudProc *energy_proc_create(char *);
#endif

extern AudProc *aud_proc_create(char *);
extern void     aud_proc_finish(AudProc *);

static const audproc_node ap_node[AUDPROC_NUMS] = {
#ifdef HAVE_MIX
    {&mix_proc_create, &aud_proc_finish, AUDPROC_MIX, "Up or Down Mixing"},
#else
    {NULL, NULL, AUDPROC_NULL, ""},
#endif
#ifdef HAVE_VBASS
    {&vbass_proc_create, &aud_proc_finish, AUDPROC_VBASS, "Virtual Bass"},
#else
    {NULL, NULL, AUDPROC_NULL, ""},
#endif
#ifdef HAVE_DIALENH
    {
        &dialenh_proc_create, &aud_proc_finish,
        AUDPROC_DIALENH, "dialog enhance"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_3D
    {&_3d_proc_create, &aud_proc_finish, AUDPROC_3D, "3D Audio"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_SURROUND
    {&surround_proc_create, &aud_proc_finish, AUDPROC_SURROUND, "Virtual Surround"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_STEREO2MONO
    {&stereo2mono_proc_create, &aud_proc_finish, AUDPROC_STEREO2MONO, "Stereo to Mono"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_MONO2STEREO
    {&mono2stereo_proc_create, &aud_proc_finish, AUDPROC_MONO2STEREO, "Mono to Stereo"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_RESAMPLE
    {
        &resample_proc_create, &aud_proc_finish,
        AUDPROC_RESAMPLE, "Resample"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_EQ
    {&eq_proc_create, &aud_proc_finish, AUDPROC_EQ, "Digital Equalizer"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_LOUDCTL
    {
        &loudctl_proc_create, &aud_proc_finish,
        AUDPROC_LOUDCTL, "Loudness Control"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_PTS
    {
        &ps_proc_create, &aud_proc_finish,
        AUDPROC_PTS, "Pitch and/or Time Shift"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_VOLUMECTL
    {
        &volumectl_proc_create, &aud_proc_finish,
        AUDPROC_VOLUMECTL, "Volume Control"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_VOLUMECTL
    {
        &volumectl_proc_create, &aud_proc_finish,
        AUDPROC_VOLUMECTL, "Level Control"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_AUTOMUTE
    {
        &automute_proc_create, &aud_proc_finish,
        AUDPROC_AUTOMUTE, "Auto Mute"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_REVERB
    {&reverb_proc_create, &aud_proc_finish, AUDPROC_REVERB, "Reverbration"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_ECHO
    {&echo_proc_create, &aud_proc_finish, AUDPROC_ECHO, "Echo"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_BASSTREBLE
    {
        &basstreble_proc_create, &aud_proc_finish,
        AUDPROC_BASSTREBLE, "Bass and Treble"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_COMPRESSOR
    {
        &compressor_proc_create, &aud_proc_finish,
        AUDPROC_COMPRESSOR, "Dynamic Range Compressor"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_VOICECANCEL
    {
        &vcctl_proc_create, &aud_proc_finish,
        AUDPROC_VOICECANCEL, "Voice Cancel"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_CUSTOM
    {
        &custom_proc_create, &aud_proc_finish,
        AUDPROC_CUSTOM, "Custom"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_SHOW_PS
    {
        &show_ps_proc_create, &aud_proc_finish,
        AUDPROC_SHOW_PS, "Show Ps"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_SHOW_PEAK
    {
        &show_peak_proc_create, &aud_proc_finish,
        AUDPROC_SHOW_PEAK, "Show Peak"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_MUTE_DECT
    {
        &mute_dect_proc_create, &aud_proc_finish,
        AUDPROC_MUTE_DECT, "Mute Detection"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_VAD
    {
        &vad_proc_create, &aud_proc_finish,
        AUDPROC_VAD, "Voice Activity Detection"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_NS
    {
        &ns_proc_create, &aud_proc_finish,
        AUDPROC_NS, "Noise Suppression"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_AGC
    {
        &agc_proc_create, &aud_proc_finish,
        AUDPROC_AGC, "Automatic Gain Control"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_AEC
    {
        &aec_proc_create, &aud_proc_finish,
        AUDPROC_AEC, "Acoustic Echo Control"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_AFC
    {
        &afc_proc_create, &aud_proc_finish,
        AUDPROC_AFC, "Acoustic Feedback Control"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_SOSF
    {&ss_proc_create, &aud_proc_finish, AUDPROC_EQ, "Second Order Sections Filter"},
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_DELAY
    {
        &delay_proc_create, &aud_proc_finish,
        AUDPROC_DELAY, "Delay"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_DELAY
    {
        &limit_proc_create, &aud_proc_finish,
        AUDPROC_LIMIT, "Limit"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_DIRECTSOUND
    {
        &d_sound_proc_create, &aud_proc_finish,
        AUDPROC_DIRECT_SOUND, "Direct Sound"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_NOISEGATE
    {
        &ng_proc_create, &aud_proc_finish,
        AUDPROC_NOISE_GATE, "Noise Gate"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
#ifdef HAVE_ENERGY
    {
        &energy_proc_create, &aud_proc_finish,
        AUDPROC_ENERGY, "Energy"
    },
#else
    {NULL, NULL, AUDPROC_NULL, {0}},
#endif
};

/* configure commands */

/* general to all features */
#define PROC_SET_IN_AFORMAT        100 /* (cmd struct aFormat) */
#define PROC_SET_OUT_AFORMAT       101 /* (cmd struct aFormat) */
#define GET_OUTPUT_BUFFER_SIZE     102 /* (cmd 1 int)          */

/* mix */
#define MIX_SET_MODE               1  /* (cmd int[0..1]), static    */
#define MIX_GET_MODE               2  /* (cmd 1 int[0..1]), dynamic */

/* surround on/off */
#define MIX_SET_SURROUND_ON        3  /* (cmd int[0..1], dynamic    */
#define MIX_GET_SURROUND_ON        4  /* (cmd 1 int[0..1], dynamic  */
#define MIX_SET_HORI_LEVEL         5  /* (cmd 1 int[0..200], dynamic */
#define MIX_GET_HORI_LEVEL         6  /* (cmd 1 int[0..200], dynamic */
#define MIX_SET_RDELAY             7  /* (cmd int [10..100], static */
#define MIX_GET_RDELAY             8  /* (cmd 1 int[10..100], dynamic */
#define MIX_SET_HEADPHONE_ON       9  /* (cmd int[0..1]), dynamic   */
#define MIX_GET_HEADPHONE_ON       10 /* (cmd 1 int[0..1]), dynamic */

/* virtual bass */
#define MIX_SET_VBASS_ON           11 /* (cmd int[0..1]), dynamic   */
#define MIX_GET_VBASS_ON           12 /* (cmd 1 int[0..1]), dynamic */
#define MIX_SET_VBASS_LEVEL        13 /* (cmd int[0..99]), dynamic  */
#define MIX_GET_VBASS_LEVEL        14 /* (cmd 1 int[0..99]), dynamic  */
#define MIX_SET_VBASS_SPEAKER_SZ   15 /* (cmd int[0..7]), dynamic   */
#define MIX_GET_VBASS_SPEAKER_SZ   16 /* (cmd 1 int[0..7]), dynamic   */
#define MIX_SET_VBASS_INPUT_GAIN   17 /* (cmd int[0..100]), dynamic */
#define MIX_GET_VBASS_INPUT_GAIN   18 /* (cmd 1 int[0..100]), dynamic */

/* dialog enhancement, effective when surround is on */
#define MIX_SET_DIALENH_ON         19  /* (cmd int[0..1]), dynamic   */
#define MIX_SET_DIALENH_BASE       20 /* (cmd int[0..400]), dynamic */
#define MIX_SET_DIALENH_TREBLE     21 /* (cmd int[0..400]), dynamic */
#define MIX_GET_DIALENH_ON         22 /* (cmd 1 int[0..1]), dynamic  */
#define MIX_GET_DIALENH_BASE       23 /* (cmd 1 int[0..400]), dynamic */
#define MIX_GET_DIALENH_TREBLE     24 /* (cmd 1 int[0..400]), dynamic */


/* resample */
#define RES_SET_QUALITY            1  /* (cmd int[0..3], static    */
#define RES_GET_QUALITY            2  /* (cmd 1 int[0..3], dynamic  */

/* eq */
/*
 * 'eq_set_bands' must be the first to be called
 * in EQ
 */
#define EQ_SET_BANDS               1  /* (cmd int0[0..7], static          */
#define EQ_SET_MAX_GAIN            2  /* (cmd int0[1000..4000], static    */
#define EQ_SET_LEVEL_OUT           3  /* (cmd int1nt0[0/1],       dynamic */
/*
 * set parameters for individual band or all bands
 * depends on the first paramter after 'cmd'
 * if the first parameter is between 0 to bands - 1,
 * it's for individual band, if it's value si 'bands'
 * then it will set up for all bands
 */
#define EQ_SET_ON                  4  /* (cmd int0[idx] int1...intn[0/1], dynamic   */
#define EQ_SET_TYPE                5  /* (cmd int0[idx] int1...intn[0..7],dynamic   */
#define EQ_SET_FC                  6  /* (cmd int0[idx] int1...intn[20..20000], dynamic */
#define EQ_SET_GAIN                7  /* (cmd int0[idx] int1...intn[-2000..2000], dynamic */
#define EQ_SET_Q                   8  /* (cmd int0[idx] int1...intn[0.1..100], dynamic */
#define EQ_SET_WIDTH               9  /* (cmd int0[idx] int1...int6[10..10000], dynamic */

#define EQ_GET_GAIN_RANGE          20 /* (cmd 2 int0 int1[-20..20]), dynamic */
#define EQ_GET_BANDS               21 /* (cmd 1 int0[0..7], dynamic */
#define EQ_GET_LEVEL_RANGE         22 /* (cmd 1 int0[0..32767], dynamic */
#define EQ_GET_LEVEL               23 /* (cmd 7 int0..int6[0..32767],dynamic */

/* loudctl */
#define LOUDCTL_SET_AMPLITUTE      1  /* (cmd int[0..32767], dynamic */
#define LOUDCTL_SET_ENERGY         2  /* (cmd int[0..32767], dynamic */
#define LOUDCTL_GET_AMPLITUTE      3  /* (cmd 1 int[0..32767], dynamic */
#define LOUDCTL_GET_ENERGY         4  /* (cmd 1 int[0..32767], dynamic */

/* pitch and/or time shift */
#define PTS_SET_LEVEL              1  /* (cmd int[50..200], dynamic */
#define PTS_GET_LEVEL              2  /* (cmd 2 int0(min) int1(max)[50..200]), 
                                          dynamic */
#define PTS_SET_MODE         	   3  /* (cmd int0[0..1], dynamic */

/* volumectl */
#define VOLCTL_SET                 1  /* (cmd int0[0..1] int1..
                                          int8[0..100/-64..63]), dynamic */
#define VOLCTL_GET_DB_RANGE        2  /* (cmd 2 int0(min) int(max)[-64..63]),
                                          dynamic */

/* automute */
#define AMUTE_SET_DB               1  /* (cmd int0[0..1] int1..
                                          int8[0..100/-64..63]), dynamic */
#define AMUTE_SET_TIME             2  /* (cmd int0) */
#define AMUTE_GET_DB_RANGE         3  /* (cmd 2 int0(min) int(max)[-64..63]),
                                          dynamic */

/* reverb */
#ifdef __SILAN_DRIVERS_CORE_DSP__
#define REVERB_MODE                3
#else
#define REVERB_MODE				   1
#endif
#define REVERB_SET_TYPE            1  /* (cmd int[0...], dynamic */
#define REVERB_GET_TYPE            2  /* (cmd 1 int[0...], dynamic */
#define REVERB_SET_CUSTOM          3  /* (cmd int[0...], dynamic */
#define REVERB_SET_PARA            4  /* FDNPara */
#define REVERB_SET_MODE            5  /* Mode */

/* echo */
#define ECHO_SET_DELAY             1  /* (cmd 1 int[a...b], dynamic */
#define ECHO_SET_DTYPE             2  /* (cmd 1 int[a...b], dynamic */
#define ECHO_SET_DGAIN             3  /* (cmd 1 int[a...b], dynamic */
#define ECHO_SET_STYPE             4  /* (cmd 1 int[a...b], dynamic */
#define ECHO_SET_SHIFT             5  /* (cmd 1 int[a...b], dynamic */
#define ECHO_SET_DP_NUMBER         6  /* (cmd 1 int[a...b], dynamic */
#define ECHO_SET_MODE              7  /* (cmd 1 int[a...b], dynamic */

/* sample bits convert */             /* no configurable parameters */

/* Bass and Treble */
#define BTR_SET_BASS               1  /* (cmd int !0), dynamic */
#define BTR_SET_TREBLE             2  /* (cmd int !0), dynamic */

/* Dynamic Range Compressor */
#define COMPRESSOR_SET_THRESHOLD   1 /* (cmd int[-600.. -10]), 1/10 dB unit,
                                         dynamic */
#define COMPRESSOR_SET_NOISEFLOOR  2 /* (cmd int[-800..-200]), 1/10 dB unit,
                                         dynamic */
#define COMPRESSOR_SET_RATIO       3 /* (cmd int[10..100]), 1/10 unit,
                                         dynamic */
#define COMPRESSOR_SET_ATTACK      4 /* (cmd int[0.1Fs..5*Fs]), samples,
                                         equals sample frequency * attacktime
                                         dynamic */
#define COMPRESSOR_SET_RELEASE     5 /* (cmd int[0..30*Fs]), samples,
                                         equals sample frequency * releasetime
                                         dynamic */
#define COMPRESSOR_SET_MAKEUP      6 /* (cmd int[0/1]), dynamic */
#define COMPRESSOR_SET_USEPEAK     7 /* (cmd int[0/1]), dynamic */
#define COMPRESSOR_SET_LOGDOMAIN   8 /* (cmd int[0/1]), dynamic */
#define COMPRESSOR_SET_KWIDTH      9 /* (cmd int[-300..-5]), 1/10 dB unit,
                                         dynamic */

/* Voice Activity Detection */
#define VAD_SET_MODE               1 /* (cmd int[0, 1, 2, 3]) */

/* Noise Suppression */
#define NS_SET_MODE               1 /* (cmd int[0, 1, 2, 3]) */

/* Second Order Section Filter Banks */
#define SOS_SET_ON                1 /* (cmd int[0...chn-1]) */
#define SOS_SET_FILTER_BANK       2 /* (cmd ch num scl g b a ... ) */
/* voice cancellation */
#define VOICECANCEL_SET_MODE       1
#define VOICECANCEL_SET_LEVEL      2
/* Delay */
#define SET_DELAY                  1
/* Direct Sound */
#define DIRECT_SOUND_GET_BUF       1

/* Noise Gate */
#define NOISE_GATE_SET             1

/* Mute Detection */
#define GET_AVERAGE_VAL            1

/* Show Peak */
#define GET_PEAK_VALUE             1

/* Show Ps */
#define GET_PS_VALUE               1

/* Energy */
#define GET_ENERGY                 1
#define SET_ENERGY                 2
/* AFC */
#define AFC_SET_GAIN               1
#define AFC_SET_FREQ               2

/* Limit */
#define SET_LIMIT				   1
#define SET_RATIO				   2
#define SET_ATTACK				   3
#define SET_RELEASE				   4

// function phototypes
void        showAllAudProc(FILE *fp);
int         getProcSamples(AudProc *proc, int *in_samples,
                           int *out_samples);
void        shrinkProcOutBuffer(AudProc *proc, int out_samples);
void       *getFirstNode(void *l);
void       *getNextNode(void *node);
void       *getNextAudProc(void *node);
audproc_id  getAudProcId(void *proc);
AudProc    *getAudProcFromName(void *l, char *name);
audproc_id  getAudIdFromName(void *l, char *name);
int         loadAudProc(void *l, char *gname, audproc_id id);
int         unloadAudProc(void *l, char *gname);
void       *initAudProc(void);
int         procAudProc(void *l, SAMPLE *in, int inSize,
                        aFormat *informat, SAMPLE **out,
                        int *outSize, aFormat *outformat);
int         finishAudProc(void *l);

int         setprocParam(void *proc, int cmd, void *value);
int         getprocParam(void *proc, int cmd,
                         size_t *size, void *value);

int         setprocsinout(void *proc, SAMPLE *s, int size, aFormat sFormat, int is_in);
int         getprocsinout(void *proc, SAMPLE **s, int *size, aFormat *sFormat, int is_in);

void        lockAudProc(void);
void        unlockAudProc(void);

#endif /* HAVE_AUDPROC */
#endif /* AUDPROC_H */
