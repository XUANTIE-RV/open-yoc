/**
 * @file file.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_UTIL_H
#define CX_COMMON_UTIL_H

#include <string>
#include <functional>
#include <cx/common/type.h>
#include <tmedia_core/common/common_inc.h>
namespace cx {

/*********** file system related utlity function ***********/
int     Util_GetPathSizeKB(const std::string &path, uint32_t &size);
int     Util_DeletePath(const std::string &path);
bool    Util_AccessPath(const std::string &path);
int     Util_CreatePath(const std::string &path);
int     Util_IterateFolder(const std::string &path, std::function<void (const char *fname)> func, const char *postfix);

/*********** time convertion related utlity function ***********/
/* unix time to string format YYYYMMDDhhmmss */
int     Util_ConvertUnixTime2String(uint32_t unixTime, std::string &str);
int     Util_ConvertString2UnixTime(const std::string &str, uint32_t &unixTime);

/*********** time convertion related utlity function ***********/
TMImageInfo::PixelFormat Util_MapPixelFormat(PixelFormat inParam);
TMMediaInfo::CodecID     Util_MapVideoEncodeType(VencoderType type);
TMMediaInfo::CodecID     Util_MapAudioEncodeType(AudioEncoderType type);

AudioSampleBits_e        MapAudioSampleBits(AudioSampleBits inParam);
AudioPcmDataType_e       MapAudioPcmDataType(AudioPcmDataType inParam);
AudioSampleChannels_e    MapAudioSampleChannels(AudioSampleChannels inParam);


int Util_String2Int(const std::string &str, int &val);

}


#endif /* CX_COMMON_UTIL_H */

