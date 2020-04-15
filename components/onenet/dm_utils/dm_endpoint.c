/*
 *  FIPS-197 compliant AES implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */



#include <string.h>
#include <stddef.h>
#include "cis_internals.h"
#include "cis_api.h"
#include "dm_endpoint.h"
#include "j_base64.h"
#include "j_aes.h"
//#include <softap_api.h>

#define EP_MEM_SIZE  (264)

//mdy by zpr Options g_opt = {"CMEI_IMEI","IMSI", "v1.0","M100000029","I62996t7tZgibNzSm764A8Ax63dAZ6y4",4,NULL,5683,60,0,NULL};//300
Options g_opt;
extern unsigned char need_start_dm;
bool dmSdkInit(void)
{
    //to init g_opt
    //zxic_nv_get(dm_retry_time, &g_opt.nLifetime, 2);
    //zxic_nv_get(dm_appkey, g_opt.szAppKey, 16);
    //zxic_nv_get(dm_pwd, g_opt.szPwd, 32);
    //zxic_nv_get(modelsms, g_opt.szDMv, 15);//��ǰDM�汾�ϱ����֧��10���ַ�

    cissys_getIMEI((uint8_t *)g_opt.szCMEI_IMEI2, sizeof(g_opt.szCMEI_IMEI2));
    cissys_getIMSI((uint8_t *)g_opt.szIMSI, sizeof(g_opt.szIMSI));

    if (g_opt.szIMSI[0] == '0' && g_opt.szIMSI[1] == '0'
        && (g_opt.szIMSI[2] == '1' || g_opt.szIMSI[2] == '2')) {
        need_start_dm = 0;
        //zxic_global_set(dm_flag, &need_start_dm, 1);
        return false;
    }

    return true;
}

int to_encode(unsigned char *szin, unsigned char *szout)
{
    //AES CBC
    /* A 256 bit key */
    unsigned char *passwd;// = "00000000000000000000000000000000";

    if (strlen(g_opt.szPwd) > 0) {
        passwd = (uint8_t *)g_opt.szPwd;
    } else {
        printf("pwd is null\n");
        return 0;
    }

    //
    unsigned char *key = cis_malloc(64);
    unsigned char *shaout = cis_malloc(256);
    unsigned char *ciphertext = cis_malloc(EP_MEM_SIZE);
    unsigned char *encData = NULL;
    int encDataLen = 0;
    int ciphertext_len = 0;

    if (key == NULL || shaout == NULL || ciphertext == NULL) {
        printf("mem err\n");
        goto out;
    }

    memset(key, 0, 64);
    memset(shaout, 0, 256);
    memset(ciphertext, 0, EP_MEM_SIZE);
    StrSHA256((char *)passwd, strlen((char *)passwd), (char *)shaout);
    HexStrToByte((char *)shaout, strlen((char *)shaout), key);
    ciphertext_len = EncryptionAES((char *)szin, strlen((char *)szin), key, ciphertext);
    j_base64_encode(ciphertext, ciphertext_len, &encData, (uint32_t *)&encDataLen);
    memcpy(szout, encData, encDataLen);

    j_base64_free(encData, encDataLen);
out:

    if (key) {
        cis_free(key);
    }

    if (shaout) {
        cis_free(shaout);
    }

    if (ciphertext) {
        cis_free(ciphertext);
    }

    return encDataLen;
}


int genDmRegEndpointName(char **data, bool isReg)
{
    unsigned char *szEpname = NULL;
    char *szStars = "****";
    int ret = -1;
    unsigned char *base64tmp = NULL;
    unsigned char *epnametmp = NULL;

    szEpname = cis_malloc(EP_MEM_SIZE);

    if (szEpname == NULL) {
        ret = -2;
        goto out;
    }

    memset(szEpname, 0, EP_MEM_SIZE);
    snprintf((char *)szEpname, EP_MEM_SIZE, "%s-%s-%s-%s",
             strlen(g_opt.szCMEI_IMEI) > 0 ? g_opt.szCMEI_IMEI : szStars,
             strlen(g_opt.szCMEI_IMEI2) > 0 ? g_opt.szCMEI_IMEI2 : szStars,
             strlen(g_opt.szIMSI) > 0 ? g_opt.szIMSI : szStars,
             strlen(g_opt.szDMv) > 0 && isReg ? g_opt.szDMv : szStars);

    base64tmp = (unsigned char *)cis_malloc(EP_MEM_SIZE);//szEpname is free now,use again;

    if (base64tmp == NULL) {
        ret = -3;
        goto out;
    }

    memset(base64tmp, 0, EP_MEM_SIZE);

    if (to_encode(szEpname, base64tmp) == 0) {
        ret = -4;
        goto out;
    }

    epnametmp = cis_malloc(EP_MEM_SIZE * 4);

    if (epnametmp == NULL) {
        ret = -5;
        goto out;
    }

    memset(epnametmp, 0, EP_MEM_SIZE * 4);
    snprintf((char *)epnametmp, EP_MEM_SIZE * 4, "I@#@%s@#@%s", base64tmp, g_opt.szAppKey);
    *data = (char *)cis_malloc(strlen((char *)epnametmp) + 1);

    if (*data == NULL) {
        ret = -6;
        goto out;
    }

    memcpy(*data, epnametmp, strlen((char *)epnametmp));
    ret = 0;
out:

    if (szEpname) {
        cis_free(szEpname);
    }

    if (epnametmp) {
        cis_free(epnametmp);
    }

    if (base64tmp) {
        cis_free(base64tmp);
    }

    printf("get ep %d\n", ret);
    return ret;
}

int prv_getDmUpdateQueryLength(st_context_t *contextP,
                               st_server_t *server)
{
    int index;
    //int res;
    //char buffer[21];

    index = strlen("epi=");
    index += strlen(contextP->privData);
    return index + 1;
}

int prv_getDmUpdateQuery(st_context_t *contextP,
                         st_server_t *server,
                         char *buffer,
                         size_t length)
{
    int index;
    int res;//,name_len;

    index = utils_stringCopy(buffer, length, "epi=");

    if (index < 0) {
        return 0;
    }

    res = utils_stringCopy(buffer + index, length - index, contextP->privData);

    if (res < 0) {
        return 0;
    }

    index += res;

    if (index < (int)length) {
        buffer[index++] = '\0';
    } else {
        return 0;
    }

    return index;
}
