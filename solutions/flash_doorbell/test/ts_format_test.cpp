/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include "ulog/ulog.h"
#include "aos/cli.h"
#include <tmedia_core/entity/format/format_inc.h>

#define TAG "mpegts_demo"

using namespace std;

static void muxer_callback(void *user_data, const uint8_t *packet, size_t size)
{
    ofstream *outts = (ofstream*)user_data;

    outts->write((const char*)packet, size);
    //LOGD(TAG, "packet = %p, size = %lu", packet, size);

    return;
}

extern "C" int cli_reg_cmd_mpegts(void);

static void cmd_mpegts_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 3) {
        if (strcmp(argv[1], "test") == 0) {
            int rc;
            TMPacket packet;
            int packetCount;
            string in_file_name = argv[2];
            string out_file_name = in_file_name + "out.ts";
            //string OutputMediaFile = "./output/2018_csky_h264_1080p_8s.h264";
            //ofstream outFile(OutputMediaFile, ios::out | ios::binary);

            ofstream outts(out_file_name, ios::binary);

            TMVideoEncoderParams vParam;
            TMFormatMuxer *formatMuxer;
            TMFormatDemuxer *formatDemuxer;
            TMMediaInfo::FormatID formatID = TMMediaInfo::FormatID::TS;

            formatDemuxer = TMFormatDemuxerFactory::CreateEntity(formatID);
            rc = formatDemuxer->Open(in_file_name);
            if (rc != TMResult::TM_OK) {
                delete formatDemuxer;
            }

            /* support h264 only */
            vParam.mCodecID = TMMediaInfo::CodecID::H264;
            formatMuxer = TMFormatMuxerFactory::CreateEntity(formatID);
            formatMuxer->SetMuxerCallback(muxer_callback, &outts);
            formatMuxer->AddStream(0, vParam);

            packetCount = 0;
            packet.Init();
            while(true) {
                rc = formatDemuxer->ReadPacket(packet);
                if (rc == 0) {
                    uint8_t* dataP = packet.mData + packet.mDataOffset;

                    printf("  Read Packet[%3d] OK: StreamID=%d, Size=%8d, Data=0x{%02x %02x %02x %02x %02x %02x %02x %02x}\n",
                           packetCount, packet.mStreamIndex, packet.mDataLength,
                           dataP[0], dataP[1], dataP[2], dataP[3],
                           dataP[4], dataP[5], dataP[6], dataP[7]);
                } else if (rc == TMResult::TM_EOF) {
                    printf("read packet eof\n");
                    break;
                } else {
                    printf("read packet error, rc = %d\n", rc);
                    break;
                }
                formatMuxer->WritePacket(0, packet);
                //outFile.write((const char*)(packet.mData + packet.mDataOffset), packet.mDataLength);
                packetCount++;
                if (packetCount >= 148)
                    break;
            }

            packet.Free();
            //outFile.close();
            formatDemuxer->Close();
            delete formatMuxer;
            delete formatDemuxer;
        }
    } else {
        printf("mpegts: invaild argv");
    }
}

int cli_reg_cmd_mpegts(void)
{
    static const struct cli_command cmd_info = {
        "mpegts",
        "\n\tmpegts test",
        cmd_mpegts_func,
    };

    aos_cli_register_command(&cmd_info);

    return 0;
}
