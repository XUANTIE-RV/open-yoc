/* keys_br.c - Bluetooth BR/EDR key handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ble_os.h>
#include <string.h>
#include <atomic.h>
#include <misc/util.h>
#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/hci.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_KEYS)
#define LOG_MODULE_NAME bt_keys_br
#include "common/log.h"

#include "hci_core.h"
#include "keys.h"
#include "settings.h"

#if defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR

struct bt_keys_link_key keys_br_pool[CONFIG_BT_MAX_PAIRED];

#if defined(CONFIG_BT_KEYS_OVERWRITE_OLDEST) && CONFIG_BT_KEYS_OVERWRITE_OLDEST
static u32_t old_key_index;
#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */

#if defined(CONFIG_BT_KEYS_OVERWRITE_OLDEST) && CONFIG_BT_KEYS_OVERWRITE_OLDEST
static struct bt_keys_link_key *bt_keys_overwrite(const bt_addr_t *bd_addr)
{
	bt_addr_le_t addr;
	struct bt_keys_link_key *key = &keys_br_pool[old_key_index++];

	addr.type = BT_ADDR_LE_PUBLIC;
	memcpy(addr.a.val, key->addr.val, sizeof(addr.a.val));

	old_key_index %= CONFIG_BT_MAX_PAIRED;

	bt_unpair(0, &addr);

	bt_addr_copy(&key->addr, bd_addr);

	return key;
}
#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */

struct bt_keys_link_key *bt_keys_find_link_key(const bt_addr_t *addr)
{
	struct bt_keys_link_key *key;
	int i;

	BT_DBG("%s", bt_addr_str(addr));

	for (i = 0; i < ARRAY_SIZE(keys_br_pool); i++) {
		key = &keys_br_pool[i];

		if (!bt_addr_cmp(&key->addr, addr)) {
			return key;
		}
	}

	return NULL;
}

struct bt_keys_link_key *bt_keys_get_link_key(const bt_addr_t *addr)
{
	struct bt_keys_link_key *key;

	key = bt_keys_find_link_key(addr);
	if (key) {
		return key;
	}

	key = bt_keys_find_link_key(BT_ADDR_ANY);
	if (key) {
		bt_addr_copy(&key->addr, addr);
		BT_DBG("created %p for %s", key, bt_addr_str(addr));

		return key;
	}

#if defined(CONFIG_BT_KEYS_OVERWRITE_OLDEST) && CONFIG_BT_KEYS_OVERWRITE_OLDEST
		return bt_keys_overwrite(addr);
#endif

	BT_DBG("unable to create link key for %s", bt_addr_str(addr));

	return NULL;
}

void bt_keys_link_key_clear(struct bt_keys_link_key *link_key)
{
	BT_DBG("%s", bt_addr_str(&link_key->addr));

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		char key[BT_SETTINGS_KEY_MAX];

		strcpy(key, "bt/br_keys/");
		bin2hex(link_key->addr.val, 6, &key[11], BT_SETTINGS_KEY_MAX - 11);

		BT_DBG("Deleting key %s", log_strdup(key));
		settings_delete(key);
	}

	(void)memset(link_key, 0, sizeof(*link_key));
}

void bt_keys_link_key_clear_addr(const bt_addr_t *addr)
{
	struct bt_keys_link_key *key;

	if (!addr) {
		(void)memset(keys_br_pool, 0, sizeof(keys_br_pool));
		return;
	}

	key = bt_keys_find_link_key(addr);
	if (key) {
		bt_keys_link_key_clear(key);
	}
}

#if defined(CONFIG_BT_SETTINGS) && CONFIG_BT_SETTINGS
int bt_br_keys_store(struct bt_keys_link_key *link_key)
{
	char key[BT_SETTINGS_KEY_MAX];
	int err;

	strcpy(key, "bt/br_keys/");
	bin2hex(link_key->addr.val, 6, &key[11], BT_SETTINGS_KEY_MAX - 11);

	err = settings_save_one(key, &link_key->flags, sizeof(struct bt_keys_link_key) - sizeof(bt_addr_t));
	if (err) {
		BT_ERR("Failed to save keys (err %d)", err);
		return err;
	}

	BT_DBG("Stored keys for %s (%s)", bt_addr_str(&link_key->addr),
	       log_strdup(key));

	return 0;
}

static int br_keys_set(const char *name, size_t len_rd, settings_read_cb read_cb,
		    void *cb_arg)
{
	struct bt_keys_link_key *keys;
	bt_addr_t addr;
	ssize_t len;
	char val[sizeof(struct bt_keys_link_key)];

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -EINVAL;
	}

	len = read_cb(cb_arg, val, sizeof(val));
	if (len < 0) {
		BT_ERR("Failed to read value (err %zd)", len);
		return -EINVAL;
	}

	BT_DBG("name %s val %s", log_strdup(name),
	       (len) ? bt_hex(val, sizeof(val)) : "(null)");

	hex2bin(name, sizeof(addr.val) * 2, addr.val, sizeof(addr.val));

	keys = bt_keys_get_link_key(&addr);
	if (!keys) {
		BT_ERR("Failed to allocate keys for %s", bt_addr_str(&addr));
		return -ENOMEM;
	}

	memcpy(&keys->flags, val, len);

	BT_DBG("Successfully restored keys for %s", bt_addr_str(&addr));

	return 0;
}

static int br_keys_commit(void)
{
	BT_DBG("");

	return 0;
}

int bt_br_key_settings_init()
{
    SETTINGS_HANDLER_DEFINE(bt_br_keys, "bt/br_keys", NULL, br_keys_set, br_keys_commit,
			       NULL);
    return 0;
}

#endif /* defined(CONFIG_BT_SETTINGS) && CONFIG_BT_SETTINGS */

#endif /* defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR */