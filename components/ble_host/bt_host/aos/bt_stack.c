/**
 *
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 */

#include "ble_os.h"
#include "bluetooth/bluetooth.h"
#include "bluetooth/conn.h"
#include "ble_internal.h"
#include "common/log.h"
#include "host/conn_internal.h"

#include <aos/bt.h>

static inline void _bt_addr_to_bt_dev_addr(const bt_addr_t *bt_addr, bt_dev_addr_t *dev_addr)
{
    memcpy(dev_addr->val, bt_addr->val, BT_STACK_BD_ADDR_LEN);
}

static inline void _bt_le_addr_to_bt_dev_addr(const bt_addr_le_t *le_addr, bt_dev_addr_t *dev_addr)
{
    dev_addr->type = le_addr->type;
    memcpy(dev_addr->val, le_addr->a.val, BT_STACK_BD_ADDR_LEN);
}

static inline void _bt_dev_addr_bt_addr_to(const bt_dev_addr_t *dev_addr, bt_addr_t *bt_addr)
{
    memcpy(bt_addr->val, dev_addr->val, BT_STACK_BD_ADDR_LEN);
}

static inline void _bt_dev_addr_bt_le_addr_to(const bt_dev_addr_t *dev_addr, bt_addr_le_t *le_addr)
{
    le_addr->type = dev_addr->type;
    memcpy(le_addr->a.val, dev_addr->val, BT_STACK_BD_ADDR_LEN);
}

#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
struct bt_stack_t {
    slist_t callback_list;
} bt_stack = { 0 };

static int bt_stack_event_callback(bt_stack_cb_event_t event, bt_stack_cb_param_t *param)
{
    slist_t *      tmp;
    bt_stack_cb_t *node;

    slist_for_each_entry_safe(&bt_stack.callback_list, tmp, node, bt_stack_cb_t, next)
    {
        if (node->callback) {
            node->callback(event, param);
        }
    }

    return 0;
}

static inline void br_auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    bt_stack_cb_param_t param = { 0 };

    _bt_addr_to_bt_dev_addr(&conn->br.dst, &param.key_notif.bda);
    param.key_notif.passkey = passkey;

    bt_stack_event_callback(BT_STACK_KEY_NOTIF_EVT, &param);
}

static inline void br_auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    bt_stack_cb_param_t param = { 0 };

    _bt_addr_to_bt_dev_addr(&conn->br.dst, &param.cfm_req.bda);
    param.cfm_req.num_val = passkey;

    bt_stack_event_callback(BT_STACK_CFM_REQ_EVT, &param);
}

static inline void br_auth_passkey_entry(struct bt_conn *conn)
{
    bt_stack_cb_param_t param = { 0 };

    _bt_addr_to_bt_dev_addr(&conn->br.dst, &param.key_req.bda);

    bt_stack_event_callback(BT_STACK_KEY_REQ_EVT, &param);
}

static inline void br_auth_cancel(struct bt_conn *conn)
{
    bt_stack_cb_param_t param = { 0 };
    param.auth_cmpl.status    = BT_SECURITY_ERR_UNSPECIFIED;
    _bt_addr_to_bt_dev_addr(&conn->br.dst, &param.auth_cmpl.bda);

    bt_stack_event_callback(BT_STACK_AUTH_CMPL_EVT, &param);
}

static inline void br_auth_pairing_confirm(struct bt_conn *conn)
{
    bt_conn_auth_pairing_confirm(conn);
}

static inline void br_auth_pin_entry(struct bt_conn *conn, bool highsec)
{
    bt_stack_cb_param_t param  = { 0 };
    param.pin_req.min_16_digit = highsec;
    _bt_addr_to_bt_dev_addr(&conn->br.dst, &param.pin_req.bda);

    bt_stack_event_callback(BT_STACK_PIN_REQ_EVT, &param);
}

static inline void br_auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    bt_stack_cb_param_t param = { 0 };
    param.auth_cmpl.status    = BT_SECURITY_ERR_SUCCESS;
    _bt_addr_to_bt_dev_addr(&conn->br.dst, &param.auth_cmpl.bda);

    bt_stack_event_callback(BT_STACK_AUTH_CMPL_EVT, &param);
}

static inline void br_auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    bt_stack_cb_param_t param = { 0 };
    param.auth_cmpl.status    = reason;
    _bt_addr_to_bt_dev_addr(&conn->br.dst, &param.auth_cmpl.bda);
    // TODO: Get remote device name
    param.auth_cmpl.device_name = NULL;

    bt_stack_event_callback(BT_STACK_AUTH_CMPL_EVT, &param);
}

/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_stack_register_callback(bt_stack_cb_t *callback)
{
    unsigned int key = irq_lock();

    slist_add_tail(&callback->next, &bt_stack.callback_list);

    irq_unlock(key);

    return;
}

/**
 * @brief      Set connectability mode for legacy bluetooth.
 * @param[in]  enable             enable/disable mode
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_connectable(int enable)
{
    return bt_br_set_connectable(enable);
}

/**
 * @brief      Set discoverability mode for legacy bluetooth.
 * @param[in]  enable             enable/disable mode
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_discoverable(int enable)
{
    return bt_br_set_discoverable(enable);
}

/**
 * @brief      Set device Extended Inquiry Response data
 * @param[in]  eir                Extended Inquiry Response data
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_eir(uint8_t fec_required, bt_eir_data_t *eir_data, size_t eri_num)
{
    if (fec_required > 1 || NULL == eir_data || 0 == eir_data) {
        return -BT_STACK_STATUS_EINVAL;
    }

    return bt_br_set_eir_data(fec_required, (const struct bt_data *)eir_data, eri_num);
}

/**
 * @brief      Update device Extended Inquiry Response data
 * @param[in]  eir                Extended Inquiry Response data
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_update_eir(bt_eir_data_t *eir_data)
{
    aos_check_return_val(eir_data, -BT_STACK_STATUS_EINVAL);

    return bt_br_update_eir((struct bt_data *)eir_data);
}

/**
 * @brief      Set device class of device
 * @param[in]  cod                device class of device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_cod(bt_stack_cod_t cod)
{
    return bt_br_set_cod(cod.service, cod.major, cod.minor);
}

/**
 * @brief      Get device class of device
 * @param[in]  cod                device class of device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_get_cod(bt_stack_cod_t *cod)
{
    aos_check_return_val(cod, -BT_STACK_STATUS_EINVAL);

    int   ret;
    u16_t service_clase;
    u8_t  major_class;
    u8_t  minor_class;

    ret = bt_br_get_cod(&service_clase, &major_class, &minor_class);
    if (ret) {
        return ret;
    }

    cod->service = service_clase;
    cod->major   = major_class;
    cod->minor   = minor_class;
    return ret;
}

/**
 * @brief      Call this function when BT_STACK_PIN_REQ_EVT generated, User should input the passkey displayed
 *             on remote device.
 * @param[in]  peer_addr          peer device bluetooth device address
 * @param[in]  pin                pin code, a string, length <= 16 bytes, only use for legacy 2.0 devices.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_pin_entry(bt_dev_addr_t *peer_addr, const char *pin)
{
    aos_check_return_val(peer_addr && pin, -BT_STACK_STATUS_EINVAL);

    int             ret;
    struct bt_conn *conn;
    bt_addr_t       addr;

    _bt_dev_addr_bt_addr_to(peer_addr, &addr);

    conn = bt_conn_lookup_addr_br(&addr);
    if (!conn) {
        return -BT_STACK_STATUS_EINVAL;
    }

    ret = bt_conn_auth_pincode_entry(conn, pin);

    bt_conn_unref(conn);

    return ret;
}

/**
 * @brief      Call this function when BT_STACK_KEY_REQ_EVT generated, User should input the passkey displayed
 *             on remote device.
 * @param[in]  peer_addr          peer device bluetooth device address
 * @param[in]  passkey            a numeric from 0 to 999999
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_passkey_entry(bt_dev_addr_t *peer_addr, uint32_t passkey)
{
    aos_check_return_val(peer_addr, -BT_STACK_STATUS_EINVAL);

    int             ret;
    struct bt_conn *conn;
    bt_addr_t       addr;

    _bt_dev_addr_bt_addr_to(peer_addr, &addr);

    conn = bt_conn_lookup_addr_br(&addr);
    if (!conn) {
        return -BT_STACK_STATUS_EINVAL;
    }

    ret = bt_conn_auth_passkey_entry(conn, passkey);

    bt_conn_unref(conn);

    return ret;
}

/**
 * @brief      When BT_STACK_CFM_REQ_EVT generated, User should call this function to comfrim the passkey is
 *             same with remote device, if not call bt_stack_pairing_cancel.
 * @param[in]  peer_addr          peer device bluetooth device address, NULL for remove all peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_passkey_confirm(bt_dev_addr_t *peer_addr)
{
    aos_check_return_val(peer_addr, -BT_STACK_STATUS_EINVAL);

    int             ret;
    struct bt_conn *conn;
    bt_addr_t       addr;

    _bt_dev_addr_bt_addr_to(peer_addr, &addr);

    conn = bt_conn_lookup_addr_br(&addr);
    if (!conn) {
        return -BT_STACK_STATUS_EINVAL;
    }

    ret = bt_conn_auth_passkey_confirm(conn);

    bt_conn_unref(conn);

    return ret;
}

/**
 * @brief      User call this function to reject this pairing request.
 * @param[in]  peer_addr          peer device bluetooth device address, NULL for remove all peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_pairing_cancel(bt_dev_addr_t *peer_addr)
{
    aos_check_return_val(peer_addr, -BT_STACK_STATUS_EINVAL);

    int             ret;
    struct bt_conn *conn;
    bt_addr_t       addr;

    _bt_dev_addr_bt_addr_to(peer_addr, &addr);

    conn = bt_conn_lookup_addr_br(&addr);
    if (!conn) {
        return -BT_STACK_STATUS_EINVAL;
    }

    ret = bt_conn_auth_cancel(conn);

    bt_conn_unref(conn);

    return ret;
}

#endif /* CONFIG_BT_BREDR */

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    if (conn->type == BT_CONN_TYPE_BR) {
        br_auth_passkey_display(conn, passkey);
    } else
#endif
        if (conn->type == BT_CONN_TYPE_LE)
    {
        le_auth_passkey_display(conn, passkey);
    }
}

static void auth_passkey_entry(struct bt_conn *conn)
{
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    if (conn->type == BT_CONN_TYPE_BR) {
        br_auth_passkey_entry(conn);
    } else
#endif
        if (conn->type == BT_CONN_TYPE_LE)
    {
        le_auth_passkey_entry(conn);
    }
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    if (conn->type == BT_CONN_TYPE_BR) {
        br_auth_passkey_confirm(conn, passkey);
    } else
#endif
        if (conn->type == BT_CONN_TYPE_LE)
    {
        le_auth_passkey_confirm(conn, passkey);
    }
}

static void auth_cancel(struct bt_conn *conn)
{
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    if (conn->type == BT_CONN_TYPE_BR) {
        br_auth_cancel(conn);
    } else
#endif
        if (conn->type == BT_CONN_TYPE_LE)
    {
        le_auth_cancel(conn);
    }
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    if (conn->type == BT_CONN_TYPE_BR) {
        br_auth_pairing_confirm(conn);
    } else
#endif
        if (conn->type == BT_CONN_TYPE_LE)
    {
        le_auth_pairing_confirm(conn);
    }
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    if (conn->type == BT_CONN_TYPE_BR) {
        br_auth_pairing_complete(conn, bonded);
    } else
#endif
        if (conn->type == BT_CONN_TYPE_LE)
    {
        le_auth_pairing_complete(conn, bonded);
    }
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    if (conn->type == BT_CONN_TYPE_BR) {
        br_auth_pairing_failed(conn, reason);
    } else
#endif
        if (conn->type == BT_CONN_TYPE_LE)
    {
        le_auth_pairing_failed(conn, reason);
    }
}

static const struct bt_conn_auth_cb auth_callbacks_display_only = {
#if (defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT) && CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .bt_security_err = NULL,
#endif
    .passkey_display  = auth_passkey_display,
    .passkey_entry    = NULL,
    .passkey_confirm  = NULL,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    .pincode_entry = NULL,
#endif
    .pairing_complete = auth_pairing_complete,
    .pairing_failed   = auth_pairing_failed,
};

static const struct bt_conn_auth_cb auth_callbacks_display_yesno = {
#if (defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT) && CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .bt_security_err = NULL,
#endif
    .passkey_display  = auth_passkey_display,
    .passkey_entry    = NULL,
    .passkey_confirm  = auth_passkey_confirm,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    .pincode_entry = NULL,
#endif
    .pairing_complete = auth_pairing_complete,
    .pairing_failed   = auth_pairing_failed,
};

static const struct bt_conn_auth_cb auth_callbacks_keyboard_only = {
#if (defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT) && CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .bt_security_err = NULL,
#endif
    .passkey_display  = NULL,
    .passkey_entry    = auth_passkey_entry,
    .passkey_confirm  = auth_passkey_confirm,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    .pincode_entry = br_auth_pin_entry,
#endif
    .pairing_complete = auth_pairing_complete,
    .pairing_failed   = auth_pairing_failed,
};

static const struct bt_conn_auth_cb auth_callbacks_none = {
#if (defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT) && CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .bt_security_err = NULL,
#endif
    .passkey_display  = NULL,
    .passkey_entry    = NULL,
    .passkey_confirm  = NULL,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = NULL,
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    .pincode_entry = NULL,
#endif
    .pairing_complete = auth_pairing_complete,
    .pairing_failed   = auth_pairing_failed,
};

static const struct bt_conn_auth_cb auth_callbacks_keyboard_display = {
#if (defined(CONFIG_BT_SMP_APP_PAIRING_ACCEPT) && CONFIG_BT_SMP_APP_PAIRING_ACCEPT)
    .bt_security_err = NULL,
#endif
    .passkey_display  = auth_passkey_display,
    .passkey_entry    = auth_passkey_entry,
    .passkey_confirm  = auth_passkey_confirm,
    .oob_data_request = NULL,
    .cancel           = auth_cancel,
    .pairing_confirm  = auth_pairing_confirm,
#if (defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR)
    .pincode_entry = NULL,
#endif
    .pairing_complete = auth_pairing_complete,
    .pairing_failed   = auth_pairing_failed,
};

static const struct bt_conn_auth_cb *auth_callbacks[] = {
    &auth_callbacks_display_only, &auth_callbacks_display_yesno,    &auth_callbacks_keyboard_only,
    &auth_callbacks_none,         &auth_callbacks_keyboard_display,
};

/**
 * @brief      Initialize the legacy bluetooth stack
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_init(void)
{
    init_param_t init = {
        .dev_name     = NULL,
        .dev_addr     = NULL,
        .conn_num_max = 1,
    };

    return ble_stack_init(&init);
}

/**
 * @brief      set device name for legacy bluetooth.
 * @param[in]  name               name of the device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_name(const char *name)
{
    aos_check_return_val(name, -BT_STACK_STATUS_EINVAL);

    return bt_set_name(name);
}

/**
 * @brief      get device name
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
const char *bt_stack_get_name()
{
    return bt_get_name();
}

/**
 * @brief      Set IO Capability
 * @param[in]  io_cap             IO Capability
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_io_capability(bt_stack_io_cap_t io_cap)
{
    if (io_cap > BT_STACK_IO_CAP_IO_KEYBOARD_DISPLAY) {
        return -BT_STACK_STATUS_EINVAL;
    }

    return bt_conn_auth_cb_register(auth_callbacks[io_cap]);
}

/**
 * @brief      Removes a device from the security database list of peer device.
 * @param[in]  peer_addr          peer device bluetooth device address, NULL for remove all peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_dev_unpair(bt_dev_addr_t *peer_addr)
{

    if (peer_addr) {
        bt_addr_le_t addr;
        addr.type = BT_ADDR_LE_PUBLIC;
        memcpy(&addr.a.val, peer_addr->val, BT_STACK_BD_ADDR_LEN);
        return bt_unpair(0, &addr);
    }

    return bt_unpair(0, NULL);
}

struct dev_addr_callback_func {
    void (*func)(bt_dev_addr_t *addr, void *data);
    void *data;
};

static void _bt_paired_key_find(const struct bt_bond_info *info, void *data)
{
    bt_dev_addr_t                  peer_addr;
    struct dev_addr_callback_func *call = (struct dev_addr_callback_func *)data;

    if (info) {
        _bt_le_addr_to_bt_dev_addr(&info->addr, &peer_addr);
        call->func(&peer_addr, call->data);
    }
}

/**
 * @brief      call func for each paired devices from the security database
 * @param[in]  func              callback func
 * @param[in]  data              func priv data
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_paired_dev_foreach(void (*func)(bt_dev_addr_t *addr, void *data), void *data)
{

    struct dev_addr_callback_func inpara = {
        .func = func,
        .data = data,
    };

    if (!func) {
        return -BT_STACK_STATUS_EINVAL;
    }

    bt_foreach_bond(0, _bt_paired_key_find, &inpara);

    return BT_STACK_STATUS_SUCCESS;
}