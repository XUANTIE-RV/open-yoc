/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#include "property_parser.h"

#include <assert.h>

PropertyParser::PropertyParser(PropertyHandler handler, void *arg) {
    handler_ = handler;
    arg_ = arg;
    assert(handler_);

    property_map_["MicSwitch"] = "0";
    property_map_["SpeakerSwitch"] = "0";
    property_map_["StatusLightSwitch"] = "0";
    property_map_["DayNightMode"] = "0";
    property_map_["StreamVideoQuality"] = "0";
    property_map_["SubStreamVideoQuality"] = "0";
    property_map_["ImageFlipState"] = "0";
    property_map_["EncryptSwitch"] = "0";
    property_map_["AlarmSwitch"] = "0";
    property_map_["MotionDetectSensitivity"] = "0";
    property_map_["VoiceDetectionSensitivity"] = "0";
    property_map_["AlarmFrequencyLevel"] = "0";
    property_map_["StorageStatus"] = "1";
    property_map_["StorageTotalCapacity"] = "128.000";
    property_map_["StorageRemainCapacity"] = "64.12";
    property_map_["StorageRecordMode"] = "0";

}

PropertyParser::~PropertyParser() {

}


void PropertyParser::GetAllProperty() {
    if (handler_) {
        std::string output = "{";
        for (std::map<std::string, std::string>::iterator search = property_map_.begin(); search != property_map_.end();
             ++search) {
            if (search != property_map_.begin()) {
                output += ",";
            }
            output += ("\"" + search->first + "\":" + search->second);
        }
        output += "}"; //批量上传，例如 {"AlarmFrequencyLevel":0,"AlarmSwitch":0}
        handler_(arg_, output);
    }
}

void PropertyParser::SetProperty(const std::string &key, const std::string &value) {
    std::map<std::string, std::string>::iterator search = property_map_.find(key);
    if (search != property_map_.end()) {
        property_map_[key] = value;
    }
    std::string output = "{\"" + key + "\":" + value + "}"; //例如 {"EncryptSwitch": 1}
    if (handler_) {
        handler_(arg_, output);
    }
}
