/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include "ulog/ulog.h"
#include "aos/cli.h"
#include <tmedia_core/entity/parser/parser_inc.h>
#include <tmedia_core/entity/format/format_inc.h>

#define TAG "video_parser_demo"

using namespace std;

#define INBUF_SIZE 4096

extern "C" int cli_reg_cmd_video_parser(void);

static void cmd_video_parser_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 3) {
        if (strcmp(argv[1], "test") == 0) {
            int ret = 0;
            int packet_count = 0;

            uint8_t inbuf[INBUF_SIZE] = {0};
            FILE *fp = fopen(argv[2], "rb");
            if (!fp) {
                printf("Could not open %s\n", argv[2]);
                return;
            }

            TMParser *parser;
            TMMediaInfo::CodecID codecID = TMMediaInfo::CodecID::H264;
            parser = TMParserFactory::CreateEntity(codecID);
            if (!parser) {
                printf("Could not CreateEntity\n");
                return;
            }

            uint8_t *data;
            size_t data_size;
            while(!feof(fp)) {
                data_size = fread(inbuf, 1, INBUF_SIZE, fp);
                if (INBUF_SIZE != data_size)
                    break;

                data = inbuf;
                while (data_size > 0) {
                    TMVideoPacket packet;
                    ret = parser->Parse(packet, data, data_size);

                    if (ret >= 0) {
                        data += ret;
                        data_size -= ret;
                    } else {
                        printf("read packet error, ret = %d\n", ret);
                        break;
                    }

                    if (packet.mDataLength) {
                        printf("  Read VideoPacket[%3d] OK: Type=%d, Size=%8d\n",
                            packet_count, static_cast<typename std::underlying_type<TMMediaInfo::PictureType>::type>(packet.mPictureType), packet.mDataLength);
                        packet_count++;
                    }
                }
            }
            fclose(fp);
            delete parser;
        }
    } else {
        printf("video_parser: invaild argv");
    }
}

static void cmd_video_demuxer_parser_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 3) {
        if (strcmp(argv[1], "test") == 0) {
            int ret = 0;
            TMFormatDemuxer *mDemuxer = TMFormatDemuxerFactory::CreateEntity(TMMediaInfo::FormatID::TS);    
            ret = mDemuxer->Open(argv[2]);
            if (ret != TMResult::TM_OK) {
                return;
            }
            TMParser *parser;
            TMMediaInfo::CodecID codecID = TMMediaInfo::CodecID::H264;
            parser = TMParserFactory::CreateEntity(codecID);
            if (!parser) {
                printf("Could not CreateEntity\n");
                return;
            }
            
            TMVideoPacket packet;
            while(1) {
                ret = mDemuxer->ReadPacket(packet);
                if (ret == 0) {
                    size_t data_size = packet.mDataLength;
                    uint8_t *data = packet.mData;
                    while (data_size > 0) {
                        TMVideoPacket videoPkt;
                        ret = parser->Parse(videoPkt, data, data_size);
                        if (ret >= 0) {
                            data += ret;
                            data_size -= ret;
                        } else {
                            printf("parser read packet error, ret = %d\n", ret);
                            break;
                        }

                        if (videoPkt.mDataLength) {
                            // int key_frame = (videoPkt.GetPacketType() == TMVideoPacket::Type::I) ? 1 : 0; 
                        }
                    }
                } else if (ret == TMResult::TM_EOF) {
                    printf("read packet eof\n");
                    break;
                } else {
                    printf("demuxer read packet error, ret = %d\n", ret);
                    break;
                }
            }
            packet.Free();
            mDemuxer->Close();
            delete parser;
        }
    } else {
        printf("video_parser: invaild argv");
    }
}

int cli_reg_cmd_video_parser(void)
{
    static const struct cli_command cmd_info = {
        "video_parser",
        "\n\tvideo_parser test",
        cmd_video_parser_func,
    };
    static const struct cli_command cmd_info1 = {
        "video_demuxer_parser",
        "\n\tvideo_demuxer_parser test",
        cmd_video_demuxer_parser_func,
    };

    aos_cli_register_command(&cmd_info);
    aos_cli_register_command(&cmd_info1);

    return 0;
}
