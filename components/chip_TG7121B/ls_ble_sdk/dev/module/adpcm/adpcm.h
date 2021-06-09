#ifndef __ADPCM_H
#define __ADPCM_H

#include <stdint.h>
#include <string.h>
/* Extra ADPCM structure definition for google TV remoter */
typedef struct
{
    uint32_t previous_predict_adpcm;
    uint16_t tableIndex;
} google_tv_audio_adpcm;

typedef struct
{
    uint16_t frame_number;
    uint8_t remote_id; 
    google_tv_audio_adpcm adpcmVal;
} google_tv_audio_header;

uint8_t ADPCM_Encode(int32_t sample);
int16_t ADPCM_Decode(uint8_t code);
void Adpcm_FrameEncode(const int16_t *PCMBuffer, void *EncodeBuffer, int32_t Len);
void Adpcm_FrameDecode(int16_t *PCMBuffer, const void *EncodeBuffer, int32_t Len);
void Adpcm_FrameEncode_Restart(google_tv_audio_header *AudioHeader);
void Adpcm_FrameEncode_Google_TV_Audio(const int16_t *PCMBuffer, void *EncodeBuffer, google_tv_audio_header *AudioHeader, int32_t Len);

#endif /* __ADPCM_H*/
