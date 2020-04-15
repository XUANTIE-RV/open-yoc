/*
 * Copyright (C) 2018 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MDM_API_H__
#define __MDM_API_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* modem band mode */
enum mdm_band {
    BAND_3 = 3,
    BAND_5 = 5,
    BAND_8 = 8,
    BAND_ALL = 9,
};

/* modem psm mode  */
enum mdm_psm_mode {
    MDM_WITHOUT_PSM = 0,
    MDM_WITH_PSM = 1,
};

/* modem edrx mode  */
enum mdm_edrx_mode {
    MDM_WITHOUT_EDRX = 0,
    MDM_WITH_EDRX = 1,
};

/* modem ereg mode */
enum mdm_ereg_mode {
    MDM_EREG_NO_SEARCHING = 0,     /**< Not registered, UE is not currently searching */
    MDM_EREG_REGED = 1,            /**< Registered */
    MDM_EREG_SEARCHING = 2,        /**< Not registered, but UE is currently trying to searching */
    MDM_EREG_DENIED = 3,           /**< Registration denied */
    MDM_EREG_UNKNOWN = 4,          /**< Unknown */
    MDM_EREG_ROAMING = 5,          /**< Registered, roaming */
};

/* modem enode signal quality */
struct mdm_esq {
    int rsrp;                   /**< Reference Signal Receiving Power unit: dbm */
    int rsrq;                   /**< Reference Signal Receiving Quality unit: dbm */
    int rssi;                   /**< Received Signal Strength Indicator unit: db */
    int sinr;                   /**< Signal to InterferenceNoiseRatio unit: db */
};

/* modem psm time param */
struct mdm_psm_time {
    char requested_periodic[9];               /**< extended T3412 time uint: minute */
    char requested_active[9];                 /**< T3324 time uint: second */
};

/* modem edrx time param */
struct mdm_edrx_time {
    char requested_edrx_value[5];               /**< requested_edrx_value,
                                                  *   could be seted,
                                                  *   should ot ref AT COMMAND mannal for value means
                                                  **/
    char nw_provided_edrx_value[5];             /**< requested_edrx_value,
                                                  *   could not be seted,
                                                  *   should ot ref AT COMMAND mannal for value means
                                                  **/
#if 0
    char paging_time_window[5];                 /**< requested_edrx_value,
                                                  *   could not be seted,
                                                  *   should ot ref AT COMMAND mannal for value means
                                                  **/
#endif
};

/**
 * @brief get imsi from modem
 * @param[out] imsi buffer for imsi
 * @param[in] len  len of buffer
 * @returns  the len of imsi or -1 failed
 */
int mdm_get_imsi(uint8_t *imsi, uint32_t len);

/**
 * @brief get imei from modem
 * @param[out] imei buffer for imsi
 * @param[in] len  len of buffer
 * @returns  the len of imei or -1 failed
 */
int mdm_get_imei(uint8_t *imsi, uint32_t len);

/**
 * @brief get modem band
 * @param[out] band band mode
 * @returns 0 on success, -1 on failed
 */
int mdm_get_band(enum mdm_band* band);

/**
 * @brief set modem band
 * @param[in] band band mode
 * @returns 0 on success, -1 on failed
 */
int mdm_set_band(enum mdm_band band);

/**
 * @brief get modem esq
 * @param[out] esq enode signal quality
 * @returns 0 on success, -1 on failed
 */
int mdm_get_esq(struct mdm_esq* esq);

/**
 * @brief get modem psm mode
 * @param[out] mode psm mode
 * @param[out] time psm time param
 * @returns 0 on success, -1 on failed
 */
int mdm_get_psmmode(enum mdm_psm_mode* mode, struct mdm_psm_time* time);

/**
 * @brief set modem psm mode
 * @param[in] mode psm mode
 * @param[in] time psm time param
 * @returns 0 on success, -1 on failed
 * @NOTICES: the parameter will only take effect after reboot
 */
int mdm_set_psmmode(enum mdm_psm_mode mode, struct mdm_psm_time* time);

/**
 * @brief set modem edrx mode
 * @param[out] mode edex mode
 * @param[out] time edex time param
 * @returns 0 on success, -1 on failed
 */
int mdm_get_edrxmode(enum mdm_edrx_mode* mode, struct mdm_edrx_time* time);

/**
 * @brief set modem edrx mode
 * @param[in] mode edex mode
 * @param[in] time edex time param
 * @returns 0 on success, -1 on failed
 * @NOTICES: the parameter will only take effect after reboot
 */
int mdm_set_edrxmode(enum mdm_edrx_mode mode, struct mdm_edrx_time* time);

/**
 * @brief set modem ereg mode
 * @param[out] mode ereg mode
 * @returns 0 on success, -1 on failed
 */
int mdm_get_ereg(enum mdm_ereg_mode* mode);

#ifdef __cplusplus
}
#endif

#endif /* __MDM_API_H__ */
