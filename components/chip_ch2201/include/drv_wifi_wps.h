/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __CSI_WIFI_WPS_H__
#define __CSI_WIFI_WPS_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WPS_APIs  WPS APIs
  * @brief WPS APIs
  *
  * WPS can only be used when station is enabled.
  *
  */

/** @addtogroup WPS_APIs
  * @{
  */

#define CSI_ERR_WIFI_REGISTRAR   (CSI_DRV_ERRNO_WIFI_BASE + 51)  /*!< WPS registrar is not supported */
#define CSI_ERR_WIFI_WPS_TYPE    (CSI_DRV_ERRNO_WIFI_BASE + 52)  /*!< WPS type error */
#define CSI_ERR_WIFI_WPS_SM      (CSI_DRV_ERRNO_WIFI_BASE + 53)  /*!< WPS state machine is not initialized */

typedef enum wps_type {
    WPS_TYPE_DISABLE = 0,
    WPS_TYPE_PBC,
    WPS_TYPE_PIN,
    WPS_TYPE_MAX,
}
wps_type_t;

/**
  * @brief     Enable Wi-Fi WPS function.
  *
  * @attention WPS can only be used when station is enabled.
  *
  * @param     wps_type : WPS type, so far only WPS_TYPE_PBC and WPS_TYPE_PIN is supported
  *
  * @return
  *          - CSI_OK : succeed
  *          - CSI_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - CSI_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - CSI_ERR_WIFI_FAIL : wps initialization fails
  */
int32_t csi_wifi_wps_enable(wps_type_t wps_type);

/**
  * @brief  Disable Wi-Fi WPS function and release resource it taken.
  *
  * @return
  *          - CSI_OK : succeed
  *          - CSI_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  */
int32_t csi_wifi_wps_disable(void);

/**
  * @brief     WPS starts to work.
  *
  * @attention WPS can only be used when station is enabled.
  *
  * @param     timeout_ms : maximum blocking time before API return.
  *          - 0 : non-blocking
  *          - 1~120000 : blocking time
  *
  * @return
  *          - CSI_OK : succeed
  *          - CSI_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - CSI_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - CSI_ERR_WIFI_WPS_SM : wps state machine is not initialized
  *          - CSI_ERR_WIFI_FAIL : wps initialization fails
  */
int32_t csi_wifi_wps_start(int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __CSI_WIFI_H__ */

