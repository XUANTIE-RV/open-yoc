/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "sdk_assistant.h"

#include <stdlib.h>
#include <string.h>

lv_device_auth_s *lvDeviceAuthCopy(const lv_device_auth_s *auth, unsigned int num) {
    if (!(num && auth)) {
        return NULL;
    }

    lv_device_auth_s *auth_new = (lv_device_auth_s *)malloc(sizeof(lv_device_auth_s) * num);
    if (!auth_new) {
        return NULL;
    }
    memset(auth_new, 0, sizeof(lv_device_auth_s) * num);

    for (unsigned int i = 0; i < num; i++) {
        auth_new[i].device_name = (char *)malloc(strlen(auth[i].device_name) + 1);
        auth_new[i].product_key = (char *)malloc(strlen(auth[i].product_key) + 1);
        auth_new[i].device_secret = (char *)malloc(strlen(auth[i].device_secret) + 1);
        if (!(auth_new[i].device_name && auth_new[i].product_key && auth_new[i].device_secret)) {
            lvDeviceAuthDelete(auth_new, i+1);
            return NULL;
        }

        memcpy(auth_new[i].product_key, auth[i].product_key, strlen(auth[i].product_key));
        auth_new[i].product_key[strlen(auth[i].product_key)] = '\0';
        memcpy(auth_new[i].device_name, auth[i].device_name, strlen(auth[i].device_name));
        auth_new[i].device_name[strlen(auth[i].device_name)] = '\0';
        memcpy(auth_new[i].device_secret, auth[i].device_secret, strlen(auth[i].device_secret));
        auth_new[i].device_secret[strlen(auth[i].device_secret)] = '\0';
        auth_new[i].dev_id = auth[i].dev_id;
    }

    return auth_new;
}

void lvDeviceAuthDelete(lv_device_auth_s *auth, unsigned int num) {
    if (!auth) {
        return;
    }

    for (unsigned int i = 0; i < num; i++) {
        if (auth[i].product_key) {
            free(auth[i].product_key);
        }
        if (auth[i].device_secret) {
            free(auth[i].device_secret);
        }
        if (auth[i].device_name) {
            free(auth[i].device_name);
        }
    }

    free(auth);
}

static int lvDeviceAuthCompare(const lv_device_auth_s *auth_1, const lv_device_auth_s *auth_2) {
    if (!(auth_1 && auth_2 &&
            auth_1->product_key && auth_1->device_name &&
            auth_2->product_key && auth_2->device_name)) {
        return -1;
    }

    if (!((strlen(auth_1->product_key) == strlen(auth_2->product_key) &&
        memcmp(auth_1->product_key, auth_2->product_key, strlen(auth_1->product_key)) == 0))) {
        return -1;
    }

    if (!((strlen(auth_1->device_name) == strlen(auth_2->device_name) &&
        memcmp(auth_1->device_name, auth_2->device_name, strlen(auth_1->device_name)) == 0))) {
        return -1;
    }

    if (auth_1->dev_id != auth_2->dev_id) {
        return -1;
    }

    return 0;
}

unsigned int lvDeviceAuthCompares(const lv_device_auth_s *auth_1, unsigned int num, const lv_device_auth_s *auth_2) {
    if (!(auth_1 && num && auth_2)) {
        return -1;
    }

    unsigned int res = num;
    unsigned int i = 0;
    for (; i < num; i ++) {
        if (lvDeviceAuthCompare(auth_1, &auth_2[i]) == 0) {
            res = i;
            break;
        }
    }

    return res;
}
