/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __SL_WIFI_PROV_H__
#define __SL_WIFI_PROV_H__
#if defined(__cplusplus)
extern "C" {
#endif

/** register smartliving wifi provisioning method, include both smartconfig and device ap
  * @return 0 success, else failed
 */
int wifi_prov_sl_register();

/** report provisioning success result to cloud, do device biding with cellphone
 */
void wifi_prov_sl_start_report();

/** stop reporting to cloud, reporting thread would stop asyncrously
 */
void wifi_prov_sl_stop_report();

/** set wifi connect state after provisioning
 * @param connected 1：连接成功；0：连接失败
 */
void wifi_prov_sl_set_connected(int connected);

#if defined(__cplusplus)
}
#endif
#endif  /* __SL_WIFI_PROV_H__ */

