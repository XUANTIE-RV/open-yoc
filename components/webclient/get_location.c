
#include <string.h>
#include <webclient.h>
#include <cJSON.h>

#define TAG  "app"

static char *g_longitude = NULL; // 经度
static char *g_latitude = NULL;  // 纬度

static int get_location() {
    if (g_latitude && g_longitude)
        return 0;

    int ret = -1;
    size_t size;

    char *s = http_wget("http://api.map.baidu.com/location/ip?ak=bVcVtFGlzndIhYOlWxsYTkgw0nrpDvLa&coor=bd09ll", &size);

    if (s) {
        cJSON *js = cJSON_Parse(s);
        if (js) {
            cJSON *v = cJSON_GetObjectItemByPath(js, "content.point.x");
            if (v) {
                g_longitude = strdup(v->valuestring);
                v = cJSON_GetObjectItemByPath(js, "content.point.y");
                if (v) {
                    g_latitude = strdup(v->valuestring);

                    ret = 0;
                }
            }
            cJSON_Delete(js);
        }
        free(s);
    }
    if (g_longitude == NULL || g_latitude == NULL) {
        g_longitude = "120.0";
        g_latitude = "30.0";
    }

    return ret;
}

const char *get_longitude() {
    if (g_longitude == NULL)
        get_location();

    return g_longitude;
}

const char *get_latitude() {
    if (g_latitude == NULL)
        get_location();

    return g_latitude;
}
