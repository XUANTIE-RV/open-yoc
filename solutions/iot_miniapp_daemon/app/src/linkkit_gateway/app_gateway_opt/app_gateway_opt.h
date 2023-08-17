#ifndef __GATEWAY_OPT_H__
#define __GATEWAY_OPT_H__

#include <ulog/ulog.h>
#include <gateway.h>
#include <stdbool.h>
#include <gateway.h>
#include <stdbool.h>
#include "bt/app_bt.h"
#include "../../event_mgr/app_event.h"
#include "../app_gateway_main.h"
#include "../app_gateway_ut.h"
#include "jsapi_publish.h"
#include <aos/kv.h>

#define state_change "gateway cb event type: "
#define sub_change "sub dev event type: "

// bool doonce = true;
// int dev;

extern int app_gateway_init();

#endif
