#ifndef __LPM_MIT_VAD_H__
#define __LPM_MIT_VAD_H__

typedef enum {
	kVadVoiceModeStartpoint = 0,
	kVadVoiceModeVoice,
	kVadVoiceModeEndpoint
}lpm_VadVoiceMode;

typedef enum {
	kVadDetectError = -1,
	kVadDetectResultSilence = 0,
	kVadDetectResultStartpoint,
	kVadDetectResultVoice,
	kVadDetectResultEndpoint,
	kVadDetectResultSilenceTimeout,
	kVadDetectResultVoiceTimeout
}lpm_VadDetectResult;

int lpm_vad_init();
int lpm_vad_uninit();
int lpm_vad_reset();
lpm_VadDetectResult lpm_vad_detect(const char * data, int length);
int lpm_vad_voice_prepare(lpm_VadVoiceMode mode);
int lpm_vad_voice_get(lpm_VadVoiceMode mode, char * data, int length);
int lpm_vad_threshold_set(int threshold); //threshold:default=5

#endif //__LPM_MIT_VAD_H__