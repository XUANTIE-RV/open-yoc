#include <clock_alarm.h>

#define TAG "APPALARM"

static void clock_alarm_cb(uint8_t id)
{
    clock_alarm_ctx_t *alarm = clock_alarm_get(id);
    if (alarm == NULL) {
        LOGE(TAG, "get alram info error");
        return;
    }

    LOGD(TAG, "id=%d time=%lld", alarm->id, (long long int)alarm->time);
}


void app_alrams_init()
{
    clock_alarm_init(clock_alarm_cb);
}
