## 简介

amrwb实现了AMR-WB语音格式的解码，其遵循Apache License。

AMR-WB 音频带宽在 50Hz-7000Hz,相对于 200Hz-3400Hz 为宽带,支持九种速率模式,分别为:模式0(6.60kbit/s)、模式 1(8.85kbit/s)、模式 2 (12.65kbit/s)、模式 3(14.25kbit/s)、模式 4(15.85kbit/s)、模式 5(18.25kbit/s)、模式 6(19.85kbit/s)、模式 7(23.05kbit/s)和模式 8(23.85kbit/s)

## 相关接口

具体接口使用请参见pvamrwbdecoder.h中的说明。

## 如何使用

### amrwb解码示例

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "pvamrwbdecoder.h"
#include <sndfile.h>

// Constants for AMR-WB.
enum {
    kInputBufferSize = 64,
    kSamplesPerFrame = 320,
    kBitsPerSample = 16,
    kOutputBufferSize = kSamplesPerFrame * kBitsPerSample/8,
    kSampleRate = 16000,
    kChannels = 1,
    kFileHeaderSize = 9,
    kMaxSourceDataUnitSize = 477 * sizeof(int16_t)
};

const uint32_t kFrameSizes[] = { 17, 23, 32, 36, 40, 46, 50, 58, 60 };

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage %s <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the input file.
    FILE* fpInput = fopen(argv[1], "rb");
    if (fpInput == NULL) {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Validate the input AMR file.
    char header[kFileHeaderSize];
    int bytesRead = fread(header, 1, kFileHeaderSize, fpInput);
    if ((bytesRead != kFileHeaderSize) ||
        (memcmp(header, "#!AMR-WB\n", kFileHeaderSize) != 0)) {
        fprintf(stderr, "Invalid AMR-WB file\n");
        fclose(fpInput);
        return EXIT_FAILURE;
    }

    // Open the output file.
    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(SF_INFO));
    sfInfo.channels = kChannels;
    sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfInfo.samplerate = kSampleRate;
    SNDFILE *handle = sf_open(argv[2], SFM_WRITE, &sfInfo);
    if (handle == NULL) {
        fprintf(stderr, "Could not create %s\n", argv[2]);
        fclose(fpInput);
        return EXIT_FAILURE;
    }

    // Allocate the decoder memory.
    uint32_t memRequirements = pvDecoder_AmrWbMemRequirements();
    void *decoderBuf = malloc(memRequirements);
    assert(decoderBuf != NULL);

    // Create AMR-WB decoder instance.
    void *amrHandle;
    int16_t *decoderCookie;
    pvDecoder_AmrWb_Init(&amrHandle, decoderBuf, &decoderCookie);

    // Allocate input buffer.
    uint8_t *inputBuf = (uint8_t*) malloc(kInputBufferSize);
    assert(inputBuf != NULL);

    // Allocate input sample buffer.
    int16_t *inputSampleBuf = (int16_t*) malloc(kMaxSourceDataUnitSize);
    assert(inputSampleBuf != NULL);

    // Allocate output buffer.
    int16_t *outputBuf = (int16_t*) malloc(kOutputBufferSize);
    assert(outputBuf != NULL);

    // Decode loop.
    int retVal = EXIT_SUCCESS;
    while (1) {
        // Read mode.
        uint8_t modeByte;
        bytesRead = fread(&modeByte, 1, 1, fpInput);
        if (bytesRead != 1) break;
        int16 mode = ((modeByte >> 3) & 0x0f);

        // AMR-WB file format cannot have mode 10, 11, 12 and 13.
        if (mode >= 10 && mode <= 13) {
            fprintf(stderr, "Encountered illegal frame type %d\n", mode);
            retVal = EXIT_FAILURE;
            break;
        }

        if (mode >= 9) {
            // Produce silence for comfort noise, speech lost and no data.
            memset(outputBuf, 0, kOutputBufferSize);
        } else /* if (mode < 9) */ {
            // Read rest of the frame.
            int32_t frameSize = kFrameSizes[mode];
            bytesRead = fread(inputBuf, 1, frameSize, fpInput);
            if (bytesRead != frameSize) break;

            int16 frameType, frameMode;
            RX_State_wb rx_state;
            frameMode = mode;
            mime_unsorting(
                    (uint8_t *)inputBuf,
                    inputSampleBuf,
                    &frameType, &frameMode, 1, &rx_state);

            int16_t numSamplesOutput;
            pvDecoder_AmrWb(
                    frameMode, inputSampleBuf,
                    outputBuf,
                    &numSamplesOutput,
                    decoderBuf, frameType, decoderCookie);

            if (numSamplesOutput != kSamplesPerFrame) {
                fprintf(stderr, "Decoder encountered error\n");
                retVal = EXIT_FAILURE;
                break;
            }

            for (int i = 0; i < kSamplesPerFrame; ++i) {
                outputBuf[i] &= 0xfffC;
            }
        }

        // Write output to wav.
        sf_writef_short(handle, outputBuf, kSamplesPerFrame / kChannels);
    }

    // Close input and output file.
    fclose(fpInput);
    sf_close(handle);

    // Free allocated memory.
    free(inputBuf);
    free(inputSampleBuf);
    free(outputBuf);

    return retVal;
}
```

