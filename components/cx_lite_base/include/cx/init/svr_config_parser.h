/**
 * @file svr_config_parser.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_INIT_SVR_CONFIG_PARSER_H
#define CX_INIT_SVR_CONFIG_PARSER_H

#include <memory>
#include <functional>
#include <cx/source/vid_channel.h>
#include <cx/source/aud_channel.h>
#include <cx/common/json.h>

namespace cx {
namespace init {

/**
 * Config file example:
 * 
 * 
{
    "recorder#0": {
        "video_input": [
            {
                "camera": "camera_rgb0",
                "width": 1280,
                "height": 1080,
                "fps": 30,
                "encoder": "h264"
            }
        ],
        "storage_space": 1024000,
        "segment_size": 10240,
        "root_path": "/fatfs/record"
    },
    "link_visual#0": {
        "video_input": [
            {
                "camera": "camera_rgb0",
                "width": 1280,
                "height": 1080,
                "fps": 30,
                "encoder": "h265"
            },
            {
                "camera": "camera_rgb0",
                "width": 1280,
                "height": 720,
                "fps": 30,
                "encoder": "h265"
            }
        ]
    }
}
 * 
 */


class ServiceConfigParser final {
public:
    using ParserFunc = std::function<void (int svrIndex, cx::Json obj)>;

    static int Parse(const char *svrConfigStr, const char *inputConfigStr);
    static int RegisterServiceParser(const std::string &svrName, const ParserFunc &func);

    static int GetVidSourceConfig(cx::Json root, int vidIndex, source::VidChannelConfig &config);
    static int GetAudSourceConfig(cx::Json root, int audIndex, source::AudChannelConfig &config);

private:
    static std::map<const std::string, ParserFunc> mPaserTable;
    static std::vector<source::VidInputConfig>     vidInputConfigs;       
};

}
}

#define REGISTER_SERVICE_CONFIG(service_name, func)     \
    class ServiceConfigParser##service_name final { \
    public: \
        ServiceConfigParser##service_name() \
        {   \
            cx::init::ServiceConfigParser::RegisterServiceParser(""#service_name"", func); \
        } \
    }; \
const static ServiceConfigParser##service_name gRegSvrConfig##service_name;


#endif /* CX_INIT_SVR_CONFIG_PARSER_H */

