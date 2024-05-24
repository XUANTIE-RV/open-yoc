/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "AmrnbEncoderTest"

#include <utils/Log.h>

#include <audio_utils/sndfile.h>
#include <stdio.h>

#include "gsmamr_enc.h"

#include "AmrnbEncTestEnvironment.h"

#define OUTPUT_FILE "/data/local/tmp/amrnbEncode.out"

constexpr int32_t kInputBufferSize = L_FRAME * 2;  // 160 samples * 16-bit per sample.
constexpr int32_t kOutputBufferSize = 1024;
constexpr int32_t kNumFrameReset = 200;
constexpr int32_t kMaxCount = 10;
struct AmrNbEncState {
    void *encCtx;
    void *pidSyncCtx;
};

static AmrnbEncTestEnvironment *gEnv = nullptr;

class AmrnbEncoderTest : public ::testing::TestWithParam<pair<string, int32_t>> {
  public:
    AmrnbEncoderTest() : mAmrEncHandle(nullptr) {}

    ~AmrnbEncoderTest() {
        if (mAmrEncHandle) {
            free(mAmrEncHandle);
            mAmrEncHandle = nullptr;
        }
    }

    AmrNbEncState *mAmrEncHandle;
    int32_t EncodeFrames(int32_t mode, FILE *fpInput, FILE *mFpOutput,
                         int32_t frameCount = INT32_MAX);
};

int32_t AmrnbEncoderTest::EncodeFrames(int32_t mode, FILE *fpInput, FILE *mFpOutput,
                                       int32_t frameCount) {
    int32_t frameNum = 0;
    uint16_t inputBuf[kInputBufferSize];
    uint8_t outputBuf[kOutputBufferSize];
    while (frameNum < frameCount) {
        int32_t bytesRead = fread(inputBuf, 1, kInputBufferSize, fpInput);
        if (bytesRead != kInputBufferSize && !feof(fpInput)) {
            ALOGE("Unable to read data from input file");
            return -1;
        } else if (feof(fpInput) && bytesRead == 0) {
            break;
        }
        Frame_Type_3GPP frame_type = (Frame_Type_3GPP)mode;
        int32_t bytesGenerated =
                AMREncode(mAmrEncHandle->encCtx, mAmrEncHandle->pidSyncCtx, (Mode)mode,
                          (Word16 *)inputBuf, outputBuf, &frame_type, AMR_TX_WMF);
        frameNum++;
        if (bytesGenerated < 0) {
            ALOGE("Error in encoging the file: Invalid output format");
            return -1;
        }

        // Convert from WMF to RFC 3267 format.
        if (bytesGenerated > 0) {
            outputBuf[0] = ((outputBuf[0] << 3) | 4) & 0x7c;
        }
        fwrite(outputBuf, 1, bytesGenerated, mFpOutput);
    }
    return 0;
}

TEST_F(AmrnbEncoderTest, CreateAmrnbEncoderTest) {
    mAmrEncHandle = (AmrNbEncState *)malloc(sizeof(AmrNbEncState));
    ASSERT_NE(mAmrEncHandle, nullptr) << "Error in allocating memory to Codec handle";
    for (int count = 0; count < kMaxCount; count++) {
        int32_t status = AMREncodeInit(&mAmrEncHandle->encCtx, &mAmrEncHandle->pidSyncCtx, 0);
        ASSERT_EQ(status, 0) << "Error creating AMR-NB encoder";
        ALOGV("Successfully created encoder");
    }
    if (mAmrEncHandle) {
        AMREncodeExit(&mAmrEncHandle->encCtx, &mAmrEncHandle->pidSyncCtx);
        ASSERT_EQ(mAmrEncHandle->encCtx, nullptr) << "Error deleting AMR-NB encoder";
        ASSERT_EQ(mAmrEncHandle->pidSyncCtx, nullptr) << "Error deleting AMR-NB encoder";
        free(mAmrEncHandle);
        mAmrEncHandle = nullptr;
        ALOGV("Successfully deleted encoder");
    }
}

TEST_P(AmrnbEncoderTest, EncodeTest) {
    mAmrEncHandle = (AmrNbEncState *)malloc(sizeof(AmrNbEncState));
    ASSERT_NE(mAmrEncHandle, nullptr) << "Error in allocating memory to Codec handle";
    int32_t status = AMREncodeInit(&mAmrEncHandle->encCtx, &mAmrEncHandle->pidSyncCtx, 0);
    ASSERT_EQ(status, 0) << "Error creating AMR-NB encoder";

    string inputFile = gEnv->getRes() + GetParam().first;
    FILE *fpInput = fopen(inputFile.c_str(), "rb");
    ASSERT_NE(fpInput, nullptr) << "Error opening input file " << inputFile;

    FILE *fpOutput = fopen(OUTPUT_FILE, "wb");
    ASSERT_NE(fpOutput, nullptr) << "Error opening output file " << OUTPUT_FILE;

    // Write file header.
    fwrite("#!AMR\n", 1, 6, fpOutput);

    int32_t mode = GetParam().second;
    int32_t encodeErr = EncodeFrames(mode, fpInput, fpOutput);
    ASSERT_EQ(encodeErr, 0) << "EncodeFrames returned error for Codec mode: " << mode;

    fclose(fpOutput);
    fclose(fpInput);

    AMREncodeExit(&mAmrEncHandle->encCtx, &mAmrEncHandle->pidSyncCtx);
    ASSERT_EQ(mAmrEncHandle->encCtx, nullptr) << "Error deleting AMR-NB encoder";
    ASSERT_EQ(mAmrEncHandle->pidSyncCtx, nullptr) << "Error deleting AMR-NB encoder";
    free(mAmrEncHandle);
    mAmrEncHandle = nullptr;
    ALOGV("Successfully deleted encoder");
}

TEST_P(AmrnbEncoderTest, ResetEncoderTest) {
    mAmrEncHandle = (AmrNbEncState *)malloc(sizeof(AmrNbEncState));
    ASSERT_NE(mAmrEncHandle, nullptr) << "Error in allocating memory to Codec handle";
    int32_t status = AMREncodeInit(&mAmrEncHandle->encCtx, &mAmrEncHandle->pidSyncCtx, 0);
    ASSERT_EQ(status, 0) << "Error creating AMR-NB encoder";

    string inputFile = gEnv->getRes() + GetParam().first;
    FILE *fpInput = fopen(inputFile.c_str(), "rb");
    ASSERT_NE(fpInput, nullptr) << "Error opening input file " << inputFile;

    FILE *fpOutput = fopen(OUTPUT_FILE, "wb");
    ASSERT_NE(fpOutput, nullptr) << "Error opening output file " << OUTPUT_FILE;

    // Write file header.
    fwrite("#!AMR\n", 1, 6, fpOutput);

    int32_t mode = GetParam().second;
    // Encode kNumFrameReset first
    int32_t encodeErr = EncodeFrames(mode, fpInput, fpOutput, kNumFrameReset);
    ASSERT_EQ(encodeErr, 0) << "EncodeFrames returned error for Codec mode: " << mode;

    status = AMREncodeReset(mAmrEncHandle->encCtx, mAmrEncHandle->pidSyncCtx);
    ASSERT_EQ(status, 0) << "Error resting AMR-NB encoder";

    // Start encoding again
    encodeErr = EncodeFrames(mode, fpInput, fpOutput);
    ASSERT_EQ(encodeErr, 0) << "EncodeFrames returned error for Codec mode: " << mode;

    fclose(fpOutput);
    fclose(fpInput);

    AMREncodeExit(&mAmrEncHandle->encCtx, &mAmrEncHandle->pidSyncCtx);
    ASSERT_EQ(mAmrEncHandle->encCtx, nullptr) << "Error deleting AMR-NB encoder";
    ASSERT_EQ(mAmrEncHandle->pidSyncCtx, nullptr) << "Error deleting AMR-NB encoder";
    free(mAmrEncHandle);
    mAmrEncHandle = nullptr;
    ALOGV("Successfully deleted encoder");
}

// TODO: Add more test vectors
INSTANTIATE_TEST_SUITE_P(AmrnbEncoderTestAll, AmrnbEncoderTest,
                         ::testing::Values(make_pair("bbb_raw_1ch_8khz_s16le.raw", MR475),
                                           make_pair("bbb_raw_1ch_8khz_s16le.raw", MR515),
                                           make_pair("bbb_raw_1ch_8khz_s16le.raw", MR59),
                                           make_pair("bbb_raw_1ch_8khz_s16le.raw", MR67),
                                           make_pair("bbb_raw_1ch_8khz_s16le.raw", MR74),
                                           make_pair("bbb_raw_1ch_8khz_s16le.raw", MR795),
                                           make_pair("bbb_raw_1ch_8khz_s16le.raw", MR102),
                                           make_pair("bbb_raw_1ch_8khz_s16le.raw", MR122),
                                           make_pair("sinesweepraw.raw", MR475),
                                           make_pair("sinesweepraw.raw", MR515),
                                           make_pair("sinesweepraw.raw", MR59),
                                           make_pair("sinesweepraw.raw", MR67),
                                           make_pair("sinesweepraw.raw", MR74),
                                           make_pair("sinesweepraw.raw", MR795),
                                           make_pair("sinesweepraw.raw", MR102),
                                           make_pair("sinesweepraw.raw", MR122)));

int main(int argc, char **argv) {
    gEnv = new AmrnbEncTestEnvironment();
    ::testing::AddGlobalTestEnvironment(gEnv);
    ::testing::InitGoogleTest(&argc, argv);
    int status = gEnv->initFromOptions(argc, argv);
    if (status == 0) {
        status = RUN_ALL_TESTS();
        ALOGV("Test result = %d\n", status);
    }
    return status;
}
