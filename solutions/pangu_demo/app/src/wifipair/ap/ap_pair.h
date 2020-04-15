
#ifndef __AP_PAIR_H__
#define __AP_PAIR_H__


typedef int (*ap_pair_callback_fn)(int timeout, char *ssid, char *password);
int ap_pair_start(int timeout_ms, char *ssid, char*password, ap_pair_callback_fn fn);


#endif


