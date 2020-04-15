/*******************************************************************************
 *
 * Copyright (c) 2017 China Mobile and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bai Jie & Long Rong, China Mobile - initial API and implementation
 *
 *******************************************************************************/

/*
 Copyright (c) 2017 Chinamobile

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.


 Bai Jie <baijie@chinamobile.com>
 Long Rong <longrong@chinamobile.com>

*/
/************************************************************************/
/* nb-iot middle software of china mobile api                           */
/************************************************************************/
#include "cis_config.h"
#include "cis_def.h"
#include "cis_log.h"
#include "cis_if_sys.h"
#include "cis_internals.h"



#define NBCFG_USE_BIGENDIAN                 1

#define NBCFG_GetVersion(data)              ((data >> 4) & 0x0F)
#define NBCFG_GetCC(data)                   (data & 0x0F)

#define NBCFG_GetConfigId(data)             (cis_cfgid_t)(data & 0x0F)
#define NBCFG_GetConfigFlag(data)           (data & 0x80)
#define NBCFG_GetConfigSize(data)           (NBCFG_MAKE_U16(data))

#define NBCFG_Getconfig_LogEnabled(data)    ((data >> 7) & 0x01)
#define NBCFG_Getconfig_LogExtoutput(data)  ((data >> 6) & 0x01)
#define NBCFG_Getconfig_LogType(data)       ((data >> 4) & 0x03)
#define NBCFG_Getconfig_LogOutputlevel(data)     (data  & 0x0F)
#define NBCFG_GetConfig_Istcount(data)      ((data >> 4) & 0x0F)

#define NBCFG_GetByteHi(p)                  (((uint8_t)(*p) >> 4) & 0x0F)
#define NBCFG_GetByteLow(p)                 ((uint8_t)(*p) & 0x0F)

#define NBCFG_HeaderSize                    (0x03)
#define NBCFG_MAKE_U16(p)                   (prvMakeU16(p))
#define NBCFG_MAKE_U32(p)                   (prvMakeU32(p))

#if CIS_ENABLE_BOOTSTRAP
#define NBCFG_DEFAULT_HOST                  ("183.230.40.39")
#else
#define NBCFG_DEFAULT_HOST                  ("183.230.40.40")
#endif
#define NBCFG_DEFAULT_MTU                   (1024)

#pragma pack(1)
struct st_cis_config {
    uint8_t version;
    uint8_t count;
    uint16_t size;
};

typedef struct st_cis_config cis_cfg_t;
//static cis_cfg_t prv_config_header;
//static cis_cfg_init_t prv_cfg_init;
//static cis_cfg_net_t prv_cfg_net;
//static cis_cfg_sys_t prv_cfg_sys;

static uint16_t prvMakeU16(uint8_t *p);
#if 0
static uint32_t prvMakeU32(uint8_t *p);
#endif

static bool prvCheckSum(uint8_t *config, uint16_t size, uint32_t checksum);
static bool prvInitParser(st_context_t *context, uint8_t *config, uint16_t size);
static bool prvSysParser(st_context_t *context, uint8_t *config, uint16_t size);
static bool prvNetParser(st_context_t *context, uint8_t *config, uint16_t size);
static void prvDefaultConfig(st_context_t *context);

typedef bool (*CALLBACK_ConfigParser)(st_context_t *context, uint8_t *config, uint16_t size);
struct st_config_trans {
    cis_cfgid_t  id;
    CALLBACK_ConfigParser callback;
};

static const struct st_config_trans prv_config_parser[] = {
    {cis_cfgid_init, prvInitParser},
    {cis_cfgid_sys, prvSysParser},
    {cis_cfgid_net, prvNetParser}
};

cis_ret_t cis_config_init(void *context, void *config, uint16_t config_len)
{
    int index = 0;
    int loop;
    uint8_t *cfg_ptr = (uint8_t *)config;
    uint32_t checksum = 0;
    cis_cfg_t prv_config_header = {0};

    if (config == NULL || config_len == 0) {
        prvDefaultConfig(context);
        return CIS_RET_NO_ERROR;
    }

    prv_config_header.version = NBCFG_GetVersion(*cfg_ptr);
    prv_config_header.count = NBCFG_GetCC(*cfg_ptr);
    prv_config_header.size = NBCFG_GetConfigSize(cfg_ptr + sizeof(uint8_t));
    cfg_ptr += NBCFG_HeaderSize;

    CIS_LOGD("Config Header Info:");
    CIS_LOGD("Version:[0x%02x]", prv_config_header.version);
    CIS_LOGD("Count:%d", prv_config_header.count);
    CIS_LOGD("Size:%d bytes", prv_config_header.size);
    CIS_LOGD("----------------");

    if (prv_config_header.version > 0x01) {
        CIS_LOGE("ERROR:config init failed.don't support version.");
        return CIS_RET_ERROR;
    }

    if (prv_config_header.size != config_len) {
        CIS_LOGE("ERROR:config init failed.config len invalid.(%u vs %u)", prv_config_header.size, config_len);
        return CIS_RET_ERROR;
    }

    //TODO:CHECK SUM;
    if (!prvCheckSum((uint8_t *)config, config_len, checksum)) {
        CIS_LOGE("ERROR:config init failed from checksum.");
    }

    for (loop = 0; loop < prv_config_header.count; loop++) {
        uint8_t item_header = *(uint8_t *)cfg_ptr;
        cis_cfgid_t cfgid = NBCFG_GetConfigId(item_header);
        uint16_t  cfg_size = NBCFG_MAKE_U16(cfg_ptr + sizeof(uint8_t));

        uint8_t item_headlen = sizeof(uint8_t) + sizeof(cfg_size);

        for (index = 0; index < sizeof(prv_config_parser) / sizeof(struct st_config_trans); index++) {
            if (prv_config_parser[index].id == cfgid) {
                if (!prv_config_parser[index].callback(context, cfg_ptr + item_headlen, cfg_size - item_headlen)) {
                    CIS_LOGE("ERROR:config parser failed,cfgid=%d", cfgid);
                    return CIS_RET_ERROR;
                }

                break;
            }
        }

        if (index >= (sizeof(prv_config_parser) / sizeof(struct st_config_trans))) {
            CIS_LOGW("warning!! jump unknown config id,cfgid=%d", cfgid);

        }

        cfg_ptr += cfg_size;
    }

    CIS_LOGI("The config file parse finish.");
    return CIS_RET_OK;
}

cis_ret_t cis_config_get(void *context, cis_cfgid_t cfgid, cis_cfgret_t *ret)
{
    switch (cfgid) {
        case cis_cfgid_init: {
            ret->data.cfg_init = &(((st_context_t *)context)->prv_cfg_init);
            break;
        }

        case cis_cfgid_net: {
            ret->data.cfg_net = &(((st_context_t *)context)->prv_cfg_net);
            break;
        }

        case cis_cfgid_sys: {
            ret->data.cfg_sys = &(((st_context_t *)context)->prv_cfg_sys);
            break;
        }

        default:
            return CIS_RET_ERROR;
    }

    return CIS_RET_OK;
}



//////////////////////////////////////////////////////////////////////////
//private function


uint16_t prvMakeU16(uint8_t *p)
{
    uint8_t data[2] = {0};
#if NBCFG_USE_BIGENDIAN

    if (utils_checkBigendian()) {
        data[0] = *((uint8_t *)p);
        data[1] = *((uint8_t *)p + 1);
    } else {
        data[1] = *((uint8_t *)p);
        data[0] = *((uint8_t *)p + 1);
    }

#else

    if (utils_checkBigendian()) {
        data[1] = *((uint8_t *)p + 0);
        data[0] = *((uint8_t *)p + 1);
    } else {
        data[0] = *((uint8_t *)p + 0);
        data[1] = *((uint8_t *)p + 1);
    }

#endif//NBCFG_USE_BIGENDIAN


    return *(uint16_t *)data;
}

#if 0
uint32_t prvMakeU32(uint8_t *p)
{
    uint8_t data[4] = {0};
#if NBCFG_USE_BIGENDIAN

    if (utils_checkBigendian()) {
        data[0] = *((uint8_t *)p + 0);
        data[1] = *((uint8_t *)p + 1);
        data[2] = *((uint8_t *)p + 2);
        data[3] = *((uint8_t *)p + 3);

    } else {
        data[3] = *((uint8_t *)p + 0);
        data[2] = *((uint8_t *)p + 1);
        data[1] = *((uint8_t *)p + 2);
        data[0] = *((uint8_t *)p + 3);
    }

#else

    if (utils_checkBigendian()) {
        data[3] = *((uint8_t *)p + 0);
        data[2] = *((uint8_t *)p + 1);
        data[1] = *((uint8_t *)p + 2);
        data[0] = *((uint8_t *)p + 3);
    } else {
        data[0] = *((uint8_t *)p + 0);
        data[1] = *((uint8_t *)p + 1);
        data[2] = *((uint8_t *)p + 2);
        data[3] = *((uint8_t *)p + 3);
    }

#endif//NBCFG_USE_BIGENDIAN
    return *(uint32_t *)data;
}
#endif

bool prvCheckSum(uint8_t *config, uint16_t size, uint32_t checksum)
{
    return TRUE;
}


bool prvInitParser(st_context_t *context, uint8_t *config, uint16_t size)
{
    CIS_LOGI(">>Config Init");
    CIS_LOGI("----------------");
    return TRUE;
}

bool prvSysParser(st_context_t *context, uint8_t *config, uint16_t size)
{
    uint8_t *cfg_ptr = config;
#if CIS_ENABLE_BOOTSTRAP
    context->prv_cfg_sys.bootstrap_enabled = *cfg_ptr;
#else
    context->prv_cfg_sys.bootstrap_enabled = false;
#endif//CIS_ENABLE_BOOTSTRAP
    cfg_ptr += 1;
#if CIS_ENABLE_LOG
    context->prv_cfg_sys.log_enabled = NBCFG_Getconfig_LogEnabled(*cfg_ptr);
#else
    context->prv_cfg_sys.log_enabled = false;
#endif//CIS_ENABLE_CIS_LOG
    context->prv_cfg_sys.log_ext_output = NBCFG_Getconfig_LogExtoutput(*cfg_ptr);
    context->prv_cfg_sys.log_output_type = NBCFG_Getconfig_LogType(*cfg_ptr);
    context->prv_cfg_sys.log_output_level = NBCFG_Getconfig_LogOutputlevel(*cfg_ptr);
    context->prv_cfg_sys.log_buffer_size = NBCFG_MAKE_U16(cfg_ptr + 1);
    cfg_ptr += 3;

    context->prv_cfg_sys.user_data.len = NBCFG_MAKE_U16(cfg_ptr);
    cfg_ptr += 2;
    context->prv_cfg_sys.user_data.data = NULL;

    if (context->prv_cfg_sys.user_data.len > 0) {
        context->prv_cfg_sys.user_data.data = cfg_ptr;
    }

    cfg_ptr += context->prv_cfg_sys.user_data.len;

    CIS_LOGI(">>Config Sys");
    CIS_LOGD("Log Enabled:%u", context->prv_cfg_sys.log_enabled);
    CIS_LOGD("Log Extend Output Type:%u", context->prv_cfg_sys.log_ext_output);
    CIS_LOGD("Log Output Type:%u", context->prv_cfg_sys.log_output_type);
    CIS_LOGD("Log Output Level:%u", context->prv_cfg_sys.log_output_level);
    CIS_LOGD("Log Buffer Size:%u", context->prv_cfg_sys.log_buffer_size);
    CIS_LOGD("Userdata Len:%u", context->prv_cfg_sys.user_data.len);
    CIS_LOGI("----------------");

    return TRUE;
}

bool prvNetParser(st_context_t *context, uint8_t *config, uint16_t size)
{
    uint8_t *cfg_ptr = config;

    context->prv_cfg_net.mtu = NBCFG_MAKE_U16(cfg_ptr);
    cfg_ptr += sizeof(uint16_t);
    context->prv_cfg_net.linktype = NBCFG_GetByteHi(cfg_ptr);
    context->prv_cfg_net.bandtype = NBCFG_GetByteLow(cfg_ptr);
    cfg_ptr += sizeof(uint8_t);

    context->prv_cfg_net.apn.len = NBCFG_MAKE_U16(cfg_ptr);
    cfg_ptr += sizeof(uint16_t);
    context->prv_cfg_net.apn.data = (uint8_t *)cfg_ptr;
    cfg_ptr += context->prv_cfg_net.apn.len;

    context->prv_cfg_net.username.len = NBCFG_MAKE_U16(cfg_ptr);
    cfg_ptr += sizeof(uint16_t);
    context->prv_cfg_net.username.data = (uint8_t *)cfg_ptr;
    cfg_ptr += context->prv_cfg_net.username.len;

    context->prv_cfg_net.password.len = NBCFG_MAKE_U16(cfg_ptr);
    cfg_ptr += sizeof(uint16_t);
    context->prv_cfg_net.password.data = (uint8_t *)cfg_ptr;
    cfg_ptr += context->prv_cfg_net.password.len;


    context->prv_cfg_net.host.len = NBCFG_MAKE_U16(cfg_ptr);
    cfg_ptr += sizeof(uint16_t);
    context->prv_cfg_net.host.data = (uint8_t *)cfg_ptr;
    cfg_ptr += context->prv_cfg_net.host.len;


    context->prv_cfg_net.user_data.len = NBCFG_MAKE_U16(cfg_ptr);
    cfg_ptr += sizeof(uint16_t);
    context->prv_cfg_net.user_data.data = (uint8_t *)cfg_ptr;
    cfg_ptr += context->prv_cfg_net.user_data.len;


    CIS_LOGI(">>Config Net");
    CIS_LOGD("Net Mtu:%u", context->prv_cfg_net.mtu);
    CIS_LOGD("Net Linktype:%u", context->prv_cfg_net.linktype);
    CIS_LOGD("Net Bandtype:%u", context->prv_cfg_net.bandtype);

    if (context->prv_cfg_net.username.len > 0) {
        CIS_LOGD("Net Username:%s", context->prv_cfg_net.username.data);
    }

    if (context->prv_cfg_net.password.len > 0) {
        CIS_LOGD("Net Password:%s", context->prv_cfg_net.password.data);
    }

    if (context->prv_cfg_net.apn.len > 0) {
        CIS_LOGD("Net Apn:%s", context->prv_cfg_net.apn.data);
    }

    if (context->prv_cfg_net.host.len > 0) {
        CIS_LOGD("Net Host:%s", context->prv_cfg_net.host.data);
    }

    CIS_LOGD("Userdata Len:%u", context->prv_cfg_net.user_data.len);
    CIS_LOGI("----------------");

    return TRUE;
}


void prvDefaultConfig(st_context_t *context)
{
#if CIS_ENABLE_BOOTSTRAP
    context->prv_cfg_sys.bootstrap_enabled = true;
#else
    context->prv_cfg_sys.bootstrap_enabled = false;
#endif//CIS_ENABLE_BOOTSTRAP
#if CIS_ENABLE_LOG
    context->prv_cfg_sys.log_enabled = true;
#else
    context->prv_cfg_sys.log_enabled = false;
#endif//CIS_ENABLE_CIS_LOG
    context->prv_cfg_sys.log_ext_output = 0;
    context->prv_cfg_sys.log_output_type = 0;
    context->prv_cfg_sys.log_output_level = 3;
    context->prv_cfg_sys.log_buffer_size = 200;
    context->prv_cfg_sys.user_data.len = 0;
    context->prv_cfg_sys.user_data.data = NULL;


    context->prv_cfg_net.mtu = NBCFG_DEFAULT_MTU;
    context->prv_cfg_net.linktype = 0;
    context->prv_cfg_net.bandtype = 0;
    context->prv_cfg_net.apn.len = 0;
    context->prv_cfg_net.apn.data = NULL;
    context->prv_cfg_net.username.len = 0;
    context->prv_cfg_net.username.data = NULL;
    context->prv_cfg_net.password.len = 0;
    context->prv_cfg_net.password.data = NULL;
    context->prv_cfg_net.host.len = (uint16_t)utils_strlen(NBCFG_DEFAULT_HOST);
    context->prv_cfg_net.host.data = (uint8_t *)NBCFG_DEFAULT_HOST;
    context->prv_cfg_net.user_data.len = 0;
    context->prv_cfg_net.user_data.data = NULL;


    CIS_LOGI(">>Default Config Sys");
    CIS_LOGD("Log Enabled:%u", context->prv_cfg_sys.log_enabled);
    CIS_LOGD("Log Extend Output Type:%u", context->prv_cfg_sys.log_ext_output);
    CIS_LOGD("Log Output Type:%u", context->prv_cfg_sys.log_output_type);
    CIS_LOGD("Log Output Level:%u", context->prv_cfg_sys.log_output_level);
    CIS_LOGD("Log Buffer Size:%u", context->prv_cfg_sys.log_buffer_size);
    CIS_LOGD("Userdata Len:%u", context->prv_cfg_sys.user_data.len);

    CIS_LOGI(">>Default Config Net");
    CIS_LOGI("Net Mtu:%u", context->prv_cfg_net.mtu);
    CIS_LOGI("Net Linktype:%u", context->prv_cfg_net.linktype);
    CIS_LOGI("Net Bandtype:%u", context->prv_cfg_net.bandtype);

    if (context->prv_cfg_net.username.len > 0) {
        CIS_LOGI("Net Username:%s", context->prv_cfg_net.username.data);
    }

    if (context->prv_cfg_net.password.len > 0) {
        CIS_LOGI("Net Password:%s", context->prv_cfg_net.password.data);
    }

    if (context->prv_cfg_net.apn.len > 0) {
        CIS_LOGI("Net Apn:%s", context->prv_cfg_net.apn.data);
    }

    if (context->prv_cfg_net.host.len > 0) {
        CIS_LOGI("Net Host:%s", context->prv_cfg_net.host.data);
    }

    CIS_LOGD("Userdata Len:%u", context->prv_cfg_net.user_data.len);
}
