/**
 *
 * Copyright (c) 2017-2022  Alibaba Group Holding Limited
 *
 */

#ifndef __BLE_BLE_INTERNALH__
#define __BLE_BLE_INTERNALH__

#ifdef __cplusplus
extern "C"
{
#endif

    void le_auth_passkey_display(struct bt_conn *conn, unsigned int passkey);

    void le_auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey);

    void le_auth_passkey_entry(struct bt_conn *conn);

    void le_auth_cancel(struct bt_conn *conn);

    void le_auth_pairing_confirm(struct bt_conn *conn);

    void le_auth_pairing_complete(struct bt_conn *conn, bool bonded);

    void le_auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_BLE_INTERNALH__ */
