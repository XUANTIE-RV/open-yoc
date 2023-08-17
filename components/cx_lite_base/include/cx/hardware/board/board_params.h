/**
 * @file record.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_HARDWARE_BOARD_BOARD_PARAMS_H
#define CX_HARDWARE_BOARD_BOARD_PARAMS_H

#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <cx/common/log.h>
#include <cx/common/type.h>

namespace cx {
namespace board {

struct SensorConfig {
    struct Tag {
        enum Type : uint8_t {
            INVALID = 0,
            NAME_VALID,
            ID_VALID
        }        tagType = INVALID;

        std::string name;
        uint32_t    id;

        bool operator==(const Tag &other)
        {
            if ((tagType == NAME_VALID && other.tagType == NAME_VALID && name == other.name)
                || (tagType == ID_VALID && other.tagType == ID_VALID && id == other.id)) {
                    return true;
            }

            return false;
        }

        bool valid()
        {
            return tagType != INVALID;
        }
        
    }           tag;
    ImageSize   size;
    uint16_t    fps;
    PixelFormat format;
    bool        onlineMode;
};

struct BoardConfig {
    std::vector<SensorConfig>   sensors;

    SensorConfig *GetSensorConfig(SensorConfig::Tag tag)
    {
        for (auto &sensorConfig : sensors) {
            if (sensorConfig.tag == tag) {
                return &sensorConfig;
            }
        }

        return nullptr;
    }
};

/**
 * Config file example:
 * 
{
  "sensors": [
    {
      "tag": "ir0",
      "width": 640,
      "height": 480
    },
    {
      "tag": "ir0",
      "width": 1280,
      "height": 720
    }
  ]
}
 * 
 */
class BoardConfigParser final {
public:

    static int Parse(const char *configStr);

    static std::shared_ptr<BoardConfig> GetConfig(void)
    {
        return mConfig;
    }

private:
    static std::shared_ptr<BoardConfig>    mConfig;
};

}
}



#endif /* CX_HARDWARE_BOARD_BOARD_PARAMS_H */

