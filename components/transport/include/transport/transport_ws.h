/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 * Tuan PM <tuanpm at live dot com>
 */

#ifndef _TRANSPORT_WS_H_
#define _TRANSPORT_WS_H_

#include "transport/transport.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Create web socket transport
 *
 * @return
 *  - transport
 *  - NULL
 */
transport_handle_t transport_ws_init(transport_handle_t parent_handle,
                                        const char *path,
                                        const char *protocol,
                                        int send_type,
                                        int buffer_size);

void transport_ws_set_path(transport_handle_t t, const char *path);

// void transport_ws_set_protocol(transport_handle_t t, const char *protocol);

void transport_ws_set_sendtype(transport_handle_t t, int send_type);

#ifdef __cplusplus
}
#endif

#endif /* _TRANSPORT_WS_H_ */
