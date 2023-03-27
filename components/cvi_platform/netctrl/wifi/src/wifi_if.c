/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: app_wifi.c
 * Description:
 *   ....
 */
#include "wifi_if.h"
#include <stdio.h>
#include <devices/netdrv.h>
#include <devices/wifi.h>
#include "cJSON.h"
#include <yoc/netmgr_service.h>
#include <yoc/netmgr.h>
#include "fatfs_vfs.h"
#include "vfs.h"
#include "drv/pin.h"
#include "pinctrl-mars.h"
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
#include <hi3861l_devops.h>
#include <wal_net.h>
#elif (CONFIG_APP_RTL8723DS_WIFI_SUPPORT == 1)
#include <devices/rtl8723ds.h>
#ifndef RTL8723DS_DEV_WIFI_EN_CHN
#define RTL8723DS_DEV_WIFI_EN_CHN -1
#endif
#ifndef RTL8723DS_DEV_WIFI_POWER_CHN
#define RTL8723DS_DEV_WIFI_POWER_CHN -1
#endif
#ifndef RTL8723DS_DEV_WIFI_SDIO_IDX
#define RTL8723DS_DEV_WIFI_SDIO_IDX 0
#endif
#endif

#define WIFICONFIGFILE SD_FATFS_MOUNTPOINT"/wifi.cfg"
static netmgr_hdl_t s_netmgr;

CVI_S32 APP_WifiInit(void)
{
	if (s_netmgr)
		return CVI_FAILURE;
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
	wifi_hi3861l_register(NULL);
	printf("%s wifi_hi3861l_register\n", __func__);
#elif (CONFIG_APP_RTL8723DS_WIFI_SUPPORT == 1)
	rtl8723ds_gpio_pin pin = {
		.wl_en = RTL8723DS_DEV_WIFI_EN_CHN,
		.power = RTL8723DS_DEV_WIFI_POWER_CHN,
		.sdio_idx = RTL8723DS_DEV_WIFI_SDIO_IDX,
	};
	wifi_rtl8723ds_register(&pin);
#else
	return CVI_FAILURE;
#endif
	s_netmgr = netmgr_dev_wifi_init();
	if (!s_netmgr) {
		printf("%s netmge_dev_wifi_init\n", __func__);
		return CVI_FAILURE;
	}
	event_service_init(NULL);
#if (CONFIG_APP_RTL8723DS_WIFI_SUPPORT == 1)
	utask_t *task = utask_new("netmgr", 10 * 1024, 8, AOS_DEFAULT_APP_PRI);
	netmgr_service_init(task);
#else
	netmgr_service_init(NULL);
#endif

#if (CONFIG_APP_ALIHI3861_WIFI_SUPPORT == 1 || CONFIG_APP_RTL8723DS_WIFI_SUPPORT == 1)
// WIFI固件没有MSG信息 不需要屏蔽GETMAC卡信息
	CVI_U8 mac[64] = {0};
	CVI_U8 SSID[64] = {0};
	CVI_U8 Passwd[64] = {0};
	CVI_S32 SSIDLen = 0;
	CVI_S32 PasswdLen = 0;

	WifiGetMac(mac);
	if (WifiConfig_Read(SSID, &SSIDLen, Passwd, &PasswdLen) == CVI_SUCCESS) {
		WifiConnect(SSID, SSIDLen, Passwd, PasswdLen);
	}
#endif
	return CVI_SUCCESS;
}

CVI_S32 WifiDestroy()
{
	if (!s_netmgr) {
		return CVI_FAILURE;
	}
	netmgr_dev_wifi_deinit(s_netmgr);
	netmgr_service_deinit();
	s_netmgr = NULL;
	return CVI_SUCCESS;
}

CVI_S32 WifiConnect(CVI_U8 *SSID, CVI_S32 SSIDLength, CVI_U8 *Passwd, CVI_S32 PasswdLength)
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
	}
	printf("%s SSID:%s Passwd %s\n",__func__,(CVI_CHAR *)SSID,(CVI_CHAR *)Passwd);
	//WifiDisConnect();
	aos_msleep(5);
	if (netmgr_config_wifi(s_netmgr, (CVI_CHAR *)SSID, SSIDLength, (CVI_CHAR *)Passwd, PasswdLength) != CVI_SUCCESS) {
		printf("%s netmgr_config_wifi err\n", __func__);
	}
	aos_msleep(10);
	if (netmgr_start(s_netmgr) != CVI_SUCCESS) {
		printf("%s netmgr_start err\n", __func__);
	}
	return CVI_SUCCESS;
}

CVI_S32 WifiConfig_Write(CVI_U8 *SSID, CVI_S32 SSIDLength, CVI_U8 *Passwd, CVI_S32 PasswdLen)
{
	cJSON *root = cJSON_CreateObject();

	if (!root) {
		return CVI_FAILURE;
	}
	cJSON_AddStringToObject(root, "ssid", (CVI_CHAR *)SSID);
	cJSON_AddStringToObject(root, "password", (CVI_CHAR *)Passwd);
	CVI_CHAR *pStr = cJSON_Print(root);
	if (pStr) {
		int fildfd = aos_open(WIFICONFIGFILE, O_CREAT | O_RDWR | O_TRUNC);
		if (fildfd > 0) {
			aos_write(fildfd, pStr, strlen(pStr));
			aos_sync(fildfd);
			aos_close(fildfd);
		}
		free(pStr);
	}
	cJSON_Delete(root);
	return CVI_SUCCESS;
}

CVI_S32 WifiConfig_Read(CVI_U8 *SSID, CVI_S32 *SSIDLength, CVI_U8 *Passwd, CVI_S32 *PasswdLen)
{
	int fildfd = -1;
	cJSON *root = NULL;
	cJSON *pjson_ssid = NULL;
	cJSON *pjson_passwd = NULL;
	CVI_CHAR ReadBuf[128] = {0};
	CVI_U16 ReadLen = 0;
	CVI_S32 Ret = CVI_SUCCESS;

	fildfd = aos_open(WIFICONFIGFILE, O_RDONLY);
	if (fildfd <= 0) {
		printf("%s aos_open err\n", __func__);
		return CVI_FAILURE;
	}
	ReadLen = aos_read(fildfd, ReadBuf, sizeof(ReadBuf));
	aos_close(fildfd);
	if (ReadLen <= 0) {
		printf("%s read err\n", __func__);
		return CVI_FAILURE;
	}
	root = cJSON_Parse(ReadBuf);
	if (!root) {
		Ret = CVI_FAILURE;
		goto EXIT;
	}
	pjson_ssid = cJSON_GetObjectItem(root, "ssid");
	if (!pjson_ssid) {
		Ret = CVI_FAILURE;
		goto EXIT;
	}
	pjson_passwd = cJSON_GetObjectItem(root, "password");
	if (pjson_ssid && pjson_ssid->type == cJSON_String) {
		memcpy(SSID, pjson_ssid->valuestring, strlen(pjson_ssid->valuestring));
		*SSIDLength = strlen(pjson_ssid->valuestring);
	}
	if (pjson_passwd && pjson_passwd->type == cJSON_String) {
		memcpy(Passwd, pjson_passwd->valuestring, strlen(pjson_passwd->valuestring));
		*PasswdLen = strlen(pjson_passwd->valuestring);
	} else
		*PasswdLen = 0;
EXIT:
	if (root)
		cJSON_free(root);
	return Ret;
}

CVI_S32 WifiConfig_Clean()
{
	if (access(WIFICONFIGFILE, F_OK) == 0) {
		remove(WIFICONFIGFILE);
	}
	return CVI_SUCCESS;
}

CVI_S32 WifiGetMac(CVI_U8 *Mac)
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
		return CVI_FAILURE;
	}
	rvm_dev_t *dev = netmgr_get_dev(s_netmgr);
	if (!dev) {
		printf("%s err dev Null\n", __func__);
		return CVI_FAILURE;
	}
	if (rvm_hal_wifi_get_mac_addr(dev, Mac) != CVI_SUCCESS) {
		printf("%s err get mac err\n", __func__);
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

CVI_S32 WifiGetIp(CVI_U8 *Ip)
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
		return CVI_FAILURE;
	}
	rvm_dev_t *dev = netmgr_get_dev(s_netmgr);
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	if (!dev) {
		printf("%s err dev Null\n", __func__);
		return CVI_FAILURE;
	}
	if (rvm_hal_net_get_ipaddr(dev, &ipaddr, &netmask, &gw) != CVI_SUCCESS) {
		printf("%s err get mac err\n", __func__);
		return CVI_FAILURE;
	}
	sprintf((CVI_CHAR *)Ip,"%s",ipaddr_ntoa((ip4_addr_t *)ip_2_ip4(&ipaddr)));
	return CVI_SUCCESS;
}

CVI_S32 WifiDisConnect(void)
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
	}
	if (netmgr_stop(s_netmgr) != CVI_SUCCESS) {
		printf("%s netmgr_stop err\n", __func__);
	}
	return CVI_SUCCESS;
}

CVI_S32 WifiGetLinkStatus()
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
	}
	rvm_dev_t *dev = netmgr_get_dev(s_netmgr);
	if (!dev) {
		printf("%s err dev Null\n", __func__);
		return CVI_FAILURE;
	}
	rvm_hal_wifi_ap_record_t info = {0};
	if (rvm_hal_wifi_sta_get_link_status(dev, &info) != CVI_SUCCESS) {
		printf("%s  get_link err\n", __func__);
		return CVI_FAILURE;
	}
	if (info.link_status == WIFI_STATUS_LINK_UP || info.link_status == WIFI_STATUS_GOTIP) {
		return CVI_SUCCESS;
	} else {
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

CVI_S32 Wifi_SendMsgData(CVI_U8 *RawData, CVI_U16 RawDataLen)
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
		return CVI_FAILURE;
	}
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
	hi_channel_send_msg_to_dev(RawData, RawDataLen);
#endif
	return CVI_SUCCESS;
}

CVI_S32 Wifi_RecvSubscribe(event_callback_t cb,void *parm)
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
	}
	rvm_dev_t *dev = netmgr_get_dev(s_netmgr);
	if (!dev) {
		printf("%s err dev Null\n", __func__);
		return CVI_FAILURE;
	}
	return rvm_hal_net_subscribe(dev,0,cb,parm);
}

CVI_S32 Wifi_RecvUnSubscribe(event_callback_t cb,void *parm)
{
	if (!s_netmgr) {
		printf("%s err Please Init Wifi\n", __func__);
	}
	rvm_dev_t *dev = netmgr_get_dev(s_netmgr);
	if (!dev) {
		printf("%s err dev Null\n", __func__);
		return CVI_FAILURE;
	}
	return rvm_hal_net_unsubscribe(dev,0,cb,parm);
}
