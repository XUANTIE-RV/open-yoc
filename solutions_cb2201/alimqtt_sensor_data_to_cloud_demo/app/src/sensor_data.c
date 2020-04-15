/****************************************************************************
 *
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <yoc_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/log.h>

#include <devices/sensor.h>

/*payload sample
* {"type":1,"val":20}
* type:1  temperature
* type:2  humidity
*/
#define PAYLOAD_FMT "{\"type\":%d,\"val\":%d}"

static const char *TAG = "T&H";

static int g_humi_value = 0;
static int g_temp_value = 0;


void get_sensor_value(char *name, int *temp, int *humi)
{
    aos_dev_t *sensor_dev;
    struct _sensor_temphumi sval;
    int ret, i;


    LOGD(TAG, "capture T&H data");

    sensor_dev = sensor_open_id(name, 0);
    if (sensor_dev == NULL) {
        LOGE(TAG, "no %s driver", name);
        return;
    }

    for (i = 0; i < 3; i++) {
        ret = sensor_fetch(sensor_dev);

        if (ret == 0) {
            break;
        }

        //LOGE(TAG, "retry");
        aos_msleep(100);
    }

    if (ret) {
        sensor_close(sensor_dev);
		*temp = g_temp_value;
		*humi = g_humi_value;
        return;
    }

    if (sensor_getvalue(sensor_dev, (void *)&sval, sizeof(struct _sensor_temphumi)) < 0) {
        sensor_close(sensor_dev);
		*temp = g_temp_value;
		*humi = g_humi_value;
        return;
    }

    sensor_close(sensor_dev);

    g_temp_value = sval.degree / 10;
    g_humi_value = sval.rh / 10;

	*temp = g_temp_value;
	*humi = g_humi_value;
	
    LOGI(TAG, "Temp=%d Humi=%d", g_temp_value, g_humi_value);
}
