#ifndef APP_WIFI_H
#define APP_WIFI_H
#include "cvi_type.h"
#include <uservice/event.h>


CVI_S32 APP_WifiInit();
CVI_S32 WifiDestroy();
CVI_S32 WifiConnect(CVI_U8 *SSID, CVI_S32 SSIDLength, CVI_U8 *Passwd, CVI_S32 PasswdLength);
CVI_S32 WifiConfig_Write(CVI_U8 *SSID, CVI_S32 SSIDLength, CVI_U8 *Passwd, CVI_S32 PasswdLen);
CVI_S32 WifiConfig_Read(CVI_U8 *SSID, CVI_S32 *SSIDLength, CVI_U8 *Passwd, CVI_S32 *PasswdLen);
CVI_S32 WifiConfig_Clean();
CVI_S32 WifiGetMac(CVI_U8 *Mac);
CVI_S32 WifiGetIp(CVI_U8 *Ip);
CVI_S32 WifiDisConnect();
CVI_S32 WifiGetLinkStatus();
CVI_S32 Wifi_SendMsgData(CVI_U8 *RawData, CVI_U16 RawDataLen);
CVI_S32 Wifi_RecvSubscribe(event_callback_t cb,void *parm);
CVI_S32 Wifi_RecvUnSubscribe(event_callback_t cb,void *parm);
#endif