/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */
#ifdef CONFIG_TMEDIA_PARSER_H264_SENO
#include <tmedia_backend_seno/parser/bs_read.h>
#include <tmedia_backend_seno/parser/parser_h264_seno.h>

using namespace std;

TMParserH264Seno::TMParserH264Seno()
{
    mRetainFrameSize = 1920*1080;
    mRetainBuf = NULL;
    mRetainBufSize = 0;
}

TMParserH264Seno::~TMParserH264Seno()
{
    if (mRetainBufSize > 0) {
        mRetainBufSize = 0;
        delete [] mRetainBuf;
        mRetainBuf = NULL;
    }
}

int TMParserH264Seno::Open(TMPropertyList *propList)
{
    if (propList != NULL) {
        uint32_t size = 0;
        int ret = propList->Get(TMParser::PropID::RETAIN_FRAME_SIZE, &size);
        if (ret != TMResult::TM_PROPERTY_NOT_FOUND) {
            mRetainFrameSize = size;
            return TMResult::TM_OK;
        } else {
            return TMResult::TM_EINVAL;
        }
    }

    return TMResult::TM_OK;
}

int TMParserH264Seno::Close()
{
    return TMResult::TM_OK;
}

int TMParserH264Seno::Parse(TMPacket &packet, uint32_t &width, uint32_t &height, const uint8_t *buf, size_t buf_size)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMParserH264Seno::Parse(TMPacket &packet, const uint8_t *buf, size_t buf_size)
{
    if (typeid(packet) != typeid(TMVideoPacket)) {
        LOG_E("Please use TMVideoPacket!\n");
        return TMResult::TM_EINVAL;
    }
    if (buf == NULL) {
        LOG_E("Buffer is NULL!\n");
        return TMResult::TM_EINVAL;
    }

    const uint8_t *find_buf = buf;
    static uint8_t start_code1[4] = {0x00, 0x00, 0x00, 0x01};
    static uint8_t start_code2[3] = {0x00, 0x00, 0x01};
    uint32_t cur_pos = 0;
    // If there is mRetainBuf last time, it is directly spliced to the mRetainBuf
    if (mRetainBufSize > 0) {
        if ((mRetainBufSize + buf_size) > mRetainFrameSize) {
            LOG_E("mRetainBufSize will exceed mRetainFrameSize! Set larger mRetainFrameSize please\n");
            return TMResult::TM_ENOMEM;
        }
        // LOG_D("retain buf(size=%ld) in last parse. use continue.\n", mRetainBufSize);
        memcpy(mRetainBuf + mRetainBufSize, buf, buf_size);
        find_buf = mRetainBuf;
    }
    // Find first separator
    int32_t startcode_len = 0;
    int32_t start_pos = -1;
    do {
        if (CompareByteArray(start_code1, &find_buf[cur_pos], 4)) {
            startcode_len = 4;
            start_pos = cur_pos; // cur_pos+4 if without 00 00 00 01
            cur_pos+=4;
            break;
        } else if (CompareByteArray(start_code2, &find_buf[cur_pos], 3)) {
            startcode_len = 3;
            start_pos = cur_pos; // cur_pos+3 if without 00 00 01
            cur_pos+=3;
            break;
        } else {
            // LOG_D("can't find nalu start code in pos=%d, continue\n", cur_pos);
            cur_pos++;
        }
    } while (cur_pos < (buf_size+mRetainBufSize-4));
    // If the first separator is not found, the current buf will be discarded
    if (start_pos < 0) {
        LOG_E("can't find nalu start code, return\n");
        return TMResult::TM_EINVAL;
    }
    // Find second separator && NALU_TYPE is NIDR
    int32_t end_pos = -1;
    do {
        if (CompareByteArray(start_code1, &find_buf[cur_pos], 4)) {
            //end_pos = cur_pos; // cur_pos+4 if without 00 00 00 01
            cur_pos+=4;
            //break;
            if ( ((find_buf[cur_pos]&0x1f) != 1)&&((find_buf[cur_pos]&0x1f) != 7) ) {
                cur_pos++;
            } else {
                end_pos = cur_pos - 4;
                break;
            }
        } else if (CompareByteArray(start_code2, &find_buf[cur_pos], 3)) {
            //end_pos = cur_pos; // cur_pos+3 if without 00 00 01
            cur_pos+=3;
            //break;
            if ( ((find_buf[cur_pos]&0x1f) != 1)&&((find_buf[cur_pos]&0x1f) != 7) ) {
                cur_pos++;
            } else {
                end_pos = cur_pos - 3;
                break;
            }
        } else {
            // LOG_D("can't find next nalu start code in pos=%d, continue\n", cur_pos);
            cur_pos++;
        }
    } while (cur_pos < (buf_size+mRetainBufSize-4));
    // The second separator is not found. Keep the current buf and consider that there may be mRetainBuf last time
    if (end_pos < 0) {
        // LOG_D("can't find next nalu start code, retain buf(size=%ld), return\n", mRetainBufSize + buf_size);
        if (mRetainBufSize > 0) { // The last time there was mRetainBuf, it was spliced at the beginning
            mRetainBufSize = mRetainBufSize + buf_size;
        } else { // No mRetainBuf last time
            mRetainBufSize = buf_size - start_pos;
            mRetainBuf = new uint8_t[mRetainBufSize + mRetainFrameSize];
            memcpy(mRetainBuf, buf + start_pos, mRetainBufSize);
        }
        return buf_size;
    }
    // LOG_D("start_pos=%d end_pos=%d cur_pos=%d\n", start_pos, end_pos, cur_pos);

    TMVideoPacket &v_packet = dynamic_cast<TMVideoPacket&>(packet);
    v_packet.mPictureType = (find_buf[start_pos+startcode_len]&0x1f) == 1 ? GetFrameType(&find_buf[start_pos+startcode_len], end_pos-start_pos-startcode_len) : TMMediaInfo::PictureType::I;
    v_packet.PrepareBuffer(end_pos - start_pos);
    v_packet.Copy(&find_buf[start_pos], end_pos - start_pos);

    uint32_t use_buf_size = end_pos - start_pos - mRetainBufSize;

    if (mRetainBufSize > 0) {
        mRetainBufSize = 0;
        delete [] mRetainBuf;
        mRetainBuf = NULL;
    }

    return use_buf_size;
}

bool TMParserH264Seno::CompareByteArray(const uint8_t *byte_arr1, const uint8_t *byte_arr2, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if(byte_arr1[i] != byte_arr2[i])
            return false;
    }
    return true;
}

TMMediaInfo::PictureType TMParserH264Seno::GetFrameType(const uint8_t *nalu_buf, size_t nalu_buf_size)
{
    bs_t s;
    bs_init(&s, nalu_buf + 1, nalu_buf_size - 1);
    bs_read_ue(&s);
    int frame_type = bs_read_ue(&s);

    switch(frame_type)
    {
    case 0: case 5: /* P */
        return TMMediaInfo::PictureType::P;
    case 1: case 6: /* B */
        return TMMediaInfo::PictureType::B;
    case 2: case 7: /* I */
        return TMMediaInfo::PictureType::I;
    case 3: case 8: /* SP */
        return TMMediaInfo::PictureType::P;
    case 4: case 9: /* SI */
        return TMMediaInfo::PictureType::I;
    default:
        return TMMediaInfo::PictureType::UNKNOWN;
    }

    return TMMediaInfo::PictureType::UNKNOWN;
}

REGISTER_PARSER_CLASS(TMMediaInfo::CodecID::H264, TMParserH264Seno)
#endif