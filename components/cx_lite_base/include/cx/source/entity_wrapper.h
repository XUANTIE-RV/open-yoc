/**
 * @file record.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_SROUCE_ENTITY_WRAPPER_H
#define CX_SROUCE_ENTITY_WRAPPER_H

#include <memory>
#include <vector>
#include <cx/common/frame.h>
#include <cx/common/type.h>
#include <tmedia_core/entity/vpss/vpss.h>
#include <tmedia_core/entity/format/camera.h>
#include <cx/hardware/board/board_params.h>
#include <tmedia_core/entity/codec/codec_inc.h>

namespace cx {
namespace source {

struct VpssWrapper {
    bool                    valid;      // if this vpss is valid
    // int                     in_use;
    std::string             label;

    // ImageSize               inputSize;
    ImageSize               outputSize;
    uint16_t                outputFps;
    int16_t                 rotationAngle;
    int16_t                 startFrame;
    // PixelFormat             inputFormat;
    PixelFormat             outputFormat;

    // bool                    cropValid;           
    CropType                cropType;
    // uint32_t                cropX;          
    // uint32_t                cropY;          
    // uint32_t                cropW;          
    // uint32_t                cropH;          

    std::shared_ptr<TMVpss> entity;

    /* record the number of channels each group used, group index is the vector index */
    // static std::vector<int> groupChanOccupied;

    int16_t                 inPadNum;
    int16_t                 outPadCnt;
};

struct ViWrapper {
    bool                        valid;
    int                         in_use;
    /* when VI is in online mode, it would route it's image to the direct VPSS,
       without ram access or multi-dest distribution  */
    bool                        onlineMode;         
    std::shared_ptr<TMCamera>   entity;
    board::SensorConfig::Tag    tag;
    ImageSize                   size;
    uint16_t                    fps;
    PixelFormat                 format;
    int16_t                     outPadCnt;
};

struct VoWrapper {
    bool                             valid;
    int                              in_use;
    std::shared_ptr<TMVideoOutput>   entity;
    ImageCoordinate                  coordinate;
    ImageSize                        size;
    PixelFormat                      format;
    uint16_t                         outputFps;
};

struct EncodeWrapper {
    bool                             valid;
    VencoderType                     type;
    uint16_t                         targetBitRate;
    int16_t                          inPadNum;
    
    std::shared_ptr<TMVideoEncoder>  videoEntity;
    std::shared_ptr<TMAudioEncoder>  audioEntity;
};

struct AudioInputWrapper {
    bool                        start;
    std::shared_ptr<std::pair<shared_ptr<TMAudioInput>, int>>   entity;
};

}
} 
#endif /* CX_SROUCE_ENTITY_WRAPPER_H */

