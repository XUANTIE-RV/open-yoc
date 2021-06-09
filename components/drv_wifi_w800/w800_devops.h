#ifndef __W800_DEVOPS_H__
#define __W800_DEVOPS_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int         smartcfg_pin;
} w800_wifi_param_t;

void wifi_w800_register(w800_wifi_param_t *config);
int wifi_is_connected_to_ap(void);

#ifdef __cplusplus
}
#endif

#endif /* __W800_DEVOPS_H__ */
