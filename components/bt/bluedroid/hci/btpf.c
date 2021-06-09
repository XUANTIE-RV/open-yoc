#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "btpf.h"
#include "osi/alarm.h"

#define HCI_EV_CONN_COMPLETE		0x03
#define HCI_EV_SYNC_CONN_COMPLETE	0x2c
#define HCI_EV_DISCONN_COMPLETE		0x05
#define HCI_EV_MODE_CHANGE		0x14
#define HCI_EV_LE_META			0x3e
#define HCI_EV_LE_CONN_COMPLETE		0x01
#define HCI_EV_LE_CONN_UPDATE_COMPLETE	0x03

#define L2CAP_CONN_REQ		0x02
#define L2CAP_CONN_RSP		0x03
#define L2CAP_DISCONN_REQ	0x06
#define L2CAP_DISCONN_RSP	0x07

#define RTL_FROM_REMOTE		0
#define RTL_TO_REMOTE		1

#define RTL_PROFILE_MATCH_SCID		(1 << 0)
#define RTL_PROFILE_MATCH_DCID		(1 << 1)

#define PAN_PACKET_COUNT                5

#define ACL_CONN	0x0
#define SYNC_CONN	0x1
#define LE_CONN		0x2

#define PSM_SDP     0x0001
#define PSM_RFCOMM  0x0003
#define PSM_PAN     0x000F
#define PSM_HID     0x0011
#define PSM_HID_INT 0x0013
#define PSM_AVCTP   0x0017
#define PSM_AVDTP   0x0019
#define PSM_FTP     0x1001
#define PSM_BIP     0x1003
#define PSM_OPP     0x1015

/* TODO */
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN
#endif

#ifdef __LITTLE_ENDIAN
struct sbc_frame_hdr {
	uint8_t syncword:8;		/* Sync word */
	uint8_t subbands:1;		/* Subbands */
	uint8_t allocation_method:1;	/* Allocation method */
	uint8_t channel_mode:2;		/* Channel mode */
	uint8_t blocks:2;		/* Blocks */
	uint8_t sampling_frequency:2;	/* Sampling frequency */
	uint8_t bitpool:8;		/* Bitpool */
	uint8_t crc_check:8;		/* CRC check */
} __attribute__ ((packed));

int setup_monitor_timer(struct rtl_btpf *btpf, u8 idx);
void del_monitor_timer(struct rtl_btpf *btpf, u8 idx);
extern void rtk_vendor_cmd_to_fw(uint16_t opcode, uint32_t send_len, uint8_t* send_data);
/*
 * only the bit field in 8-bit is affected by endian, not the 16-bit or 32-bit.
 * why?
 */
struct rtp_header {
	unsigned cc:4;
	unsigned x:1;
	unsigned p:1;
	unsigned v:2;

	unsigned pt:7;
	unsigned m:1;

	uint16_t sequence_number;
	uint32_t timestamp;
	uint32_t ssrc;
	uint32_t csrc[0];
} __attribute__ ((packed));

#else
/* big endian */
struct sbc_frame_hdr {
	uint8_t syncword:8;		/* Sync word */
	uint8_t sampling_frequency:2;	/* Sampling frequency */
	uint8_t blocks:2;		/* Blocks */
	uint8_t channel_mode:2;		/* Channel mode */
	uint8_t allocation_method:1;	/* Allocation method */
	uint8_t subbands:1;		/* Subbands */
	uint8_t bitpool:8;		/* Bitpool */
	uint8_t crc_check:8;		/* CRC check */
} __attribute__ ((packed));

struct rtp_header {
	unsigned v:2;
	unsigned p:1;
	unsigned x:1;
	unsigned cc:4;

	unsigned m:1;
	unsigned pt:7;

	uint16_t sequence_number;
	uint32_t timestamp;
	uint32_t ssrc;
	uint32_t csrc[0];
} __attribute__ ((packed));
#endif /* __LITTLE_ENDIAN */

/* FIXME: osif */
#define GFP_KERNEL	0

#define offsetof2(TYPE, MEMBER)	((size_t)&((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({			\
	(type *)( (char *)ptr - offsetof2(type,member) );})
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void __list_del_entry(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = entry;
	entry->prev = entry;
}

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

#if 0
#define rtlbt_dbg(fmt, ...)	do { } while (0)
#define rtlbt_info(fmt, ...)	printf("RTLBTPF: "fmt"\n", ##__VA_ARGS__)
#define rtlbt_warn(fmt, ...)	printf("RTLBTPF: "fmt"\n", ##__VA_ARGS__)
#define rtlbt_err(fmt, ...)	printf("RTLBTPF: "fmt"\n", ##__VA_ARGS__)
#else
#define rtlbt_dbg(fmt, ...)
#define rtlbt_info(fmt, ...)
#define rtlbt_warn(fmt, ...)
#define rtlbt_err(fmt, ...)
#endif

static inline void *kzalloc(int sz, u32 flags)
{
	void *p;
	p = malloc(sz);
	if (p)
		memset(p, 0, sz);
	return p;
}

static inline void kfree(void *p)
{
	free(p);
}

osi_alarm_t *timerid[2];

/* HCI conn information */
struct rtl_hci_conn {
	struct list_head list;
	u8 type;
	u8 pf_bits;
	u16 handle;
	u8 pf_refs[PROFILE_MAX];
	struct list_head pf_list;
};

struct rtl_profile_id {
	u16 match_flags;
	u16 dcid;
	u16 scid;
};

/* profile information */
struct rtl_profile {
	struct list_head list;
	u16 psm;
	u16 dcid;
	u16 scid;
	u8  idx;
	u8  flags;
	void *conn; /* point to HCI conn information */
};

static int psm_to_profile(u16 psm)
{
	switch (psm) {
	case PSM_AVCTP:
	case PSM_SDP:
		return -1; /* ignore */

	case PSM_HID:
	case PSM_HID_INT:
		return PROFILE_HID;

	case PSM_AVDTP:
		return PROFILE_A2DP;

	case PSM_PAN:
	case PSM_OPP:
	case PSM_FTP:
	case PSM_BIP:
	case PSM_RFCOMM:
		return PROFILE_PAN;

	default:
		return -1;
	}
}

static struct rtl_profile *lookup_pf_by_psm(struct rtl_hci_conn *conn,
					    uint16_t psm)
{
	struct list_head *head = &conn->pf_list;
	struct list_head *pos;
	struct list_head *n;
	struct rtl_profile *p;

	list_for_each_safe(pos, n, head) {
		p = list_entry(pos, struct rtl_profile, list);
		if (p->psm == psm)
			return p;
	}

	return NULL;
}

static struct rtl_hci_conn *rtl_hci_conn_lookup(struct rtl_btpf *btpf,
						u16 handle)
{
	struct list_head *head = &btpf->conn_list;
	struct list_head *p, *n;
	struct rtl_hci_conn *conn;

	list_for_each_safe(p, n, head) {
		conn = list_entry(p, struct rtl_hci_conn, list);
		if ((handle & 0xfff) == conn->handle)
			return conn;
	}

	return NULL;
}

static void rtl_profile_list_purge(struct rtl_hci_conn *conn)
{
	struct list_head *p, *n;
	struct rtl_profile *pf;

	list_for_each_safe(p, n, &conn->pf_list) {
		pf = list_entry(p, struct rtl_profile, list);
		list_del(&pf->list);
		kfree(pf);
	}
}

static void rtl_hci_conn_list_purge(struct rtl_btpf *btpf)
{
	struct list_head *head = &btpf->conn_list;
	struct list_head *p, *n;
	struct rtl_hci_conn *conn;

	list_for_each_safe(p, n, head) {
		conn = list_entry(p, struct rtl_hci_conn, list);
		if (conn) {
			rtl_profile_list_purge(conn);
			list_del(&conn->list);
			kfree(conn);
		}
	}
}

static struct rtl_profile *profile_alloc(u16 psm, u8 idx, u16 dcid, u16 scid)
{
	struct rtl_profile *pf;

	pf = kzalloc(sizeof(struct rtl_profile), GFP_KERNEL);
	if (!pf)
		return NULL;

	pf->psm = psm;
	pf->scid = scid;
	pf->dcid = dcid;
	pf->idx = idx;
	INIT_LIST_HEAD(&pf->list);

	return pf;
}

static struct rtl_profile *rtl_profile_lookup(struct list_head *head,
					      struct rtl_profile_id *id)
{
	struct list_head *p, *n;
	struct rtl_profile *tmp;

	if (!id->match_flags) {
		rtlbt_warn("%s: no match flags", __func__);
		return NULL;
	}

	list_for_each_safe(p, n, head) {
		tmp = list_entry(p, struct rtl_profile, list);

		if ((id->match_flags & RTL_PROFILE_MATCH_SCID) &&
		    id->scid != tmp->scid)
			continue;

		if ((id->match_flags & RTL_PROFILE_MATCH_DCID) &&
		    id->dcid != tmp->dcid)
			continue;

		return tmp;
	}

	return NULL;
}

static void btpf_update_to_controller(struct rtl_btpf *btpf)
{
	struct list_head *head = &btpf->conn_list;
	struct list_head *pos, *n;
	struct rtl_hci_conn *conn;
	uint8_t handle_num = 0;
	uint32_t buf_sz;
	uint8_t *buf;
	uint8_t *p;

	list_for_each_safe(pos, n, head) {
		conn = list_entry(pos, struct rtl_hci_conn, list);
		if (conn && conn->pf_bits)
			handle_num++;
	}

	buf_sz = 1 + handle_num * 3 + 1;

	buf = kzalloc(buf_sz, 0);

	if (!buf) {
		rtlbt_err("%s: alloc error", __func__);
		return;
	}

	p = buf;
	rtlbt_info("%s: buf_sz %u", __func__, buf_sz);
	rtlbt_info("%s: handle num %u", __func__, handle_num);

	*p++ = handle_num;
	list_for_each_safe(pos, n, head) {
		conn = list_entry(pos, struct rtl_hci_conn, list);
		if (conn && conn->pf_bits) {
			rtlbt_info("%s: handle 0x%04x", __func__, conn->handle);
			rtlbt_info("%s: profile_bitmap 0x%02x", __func__,
				   conn->pf_bits);
			*p++ = (conn->handle & 0xff);
			*p++ = ((conn->handle >> 8) & 0xff);
			*p++ = conn->pf_bits;
			handle_num--;
		}

		if (!handle_num)
			break;
	}

	rtlbt_info("%s: profile state 0x%02x", __func__, btpf->pf_state);

	*p++ = btpf->pf_state;

	rtk_vendor_cmd_to_fw(0xfc19, buf_sz, buf);

	kfree(buf);
}

static void update_profile_state(struct rtl_btpf *btpf, u8 idx, u8 busy)
{
	u8 update = 0;

	if (!(btpf->pf_bits & (1 << idx))) {
		rtlbt_err("%s: profile(%x) not exist", __func__, idx);
		return;
	}

	if (busy) {
		if (!(btpf->pf_state & (1 << idx))) {
			update = 1;
			btpf->pf_state |= (1 << idx);
		}
	} else {
		if (btpf->pf_state & (1 << idx)) {
			update = 1;
			btpf->pf_state &= ~(1 << idx);
		}
	}

	if (update) {
		rtlbt_info("%s: pf_bits 0x%02x", __func__, btpf->pf_bits);
		rtlbt_info("%s: pf_state 0x%02x", __func__, btpf->pf_state);
		btpf_update_to_controller(btpf);
	}
}

static int profile_conn_get(struct rtl_btpf *btpf, struct rtl_hci_conn *conn,
			    u8 idx)
{
	int update = 0;
	u8 i;

	rtlbt_dbg("%s: idx %u", __func__, idx);

	if (!conn || idx >= PROFILE_MAX)
		return -1;

	if (!btpf->pf_refs[idx]) {
		update = 1;
		btpf->pf_bits |= (1 << idx);

		/* SCO is always busy */
		if (idx == PROFILE_SCO)
			btpf->pf_state |= (1 << idx);

		/* TODO: */
		setup_monitor_timer(btpf, idx);
	}
	btpf->pf_refs[idx]++;

	if (!conn->pf_refs[idx]) {
		update = 1;
		conn->pf_bits |= (1 << idx);
	}
	conn->pf_refs[idx]++;

	rtlbt_info("%s: btpf->pf_bits 0x%02x", __func__, btpf->pf_bits);
	for (i = 0; i < PROFILE_MAX; i++)
		rtlbt_info("%s: btpf->pf_refs[%u] %d", __func__, i,
			   btpf->pf_refs[i]);

	if (update)
		btpf_update_to_controller(btpf);

	return 0;
}

static int profile_conn_put(struct rtl_btpf *btpf, struct rtl_hci_conn *conn,
			    u8 idx)
{
	int need_update = 0;
	u8 i;

	rtlbt_dbg("%s: idx %u", __func__, idx);

	if (!conn || idx >= PROFILE_MAX)
		return -1;

	if (!btpf->pf_refs[idx]) {
		rtlbt_info("%s: profile refcount is 0", __func__);
		return -1;
	}

	btpf->pf_refs[idx]--;
	if (!btpf->pf_refs[idx]) {
		need_update = 1;
		btpf->pf_bits &= ~(1 << idx);
		btpf->pf_state &= ~(1 << idx);
		/* TODO: */
		del_monitor_timer(btpf, idx);
	}

	conn->pf_refs[idx]--;
	if (!conn->pf_refs[idx]) {
		need_update = 1;
		conn->pf_bits &= ~(1 << idx);

		/* Clear hid interval if needed */
		if (idx == PROFILE_HID &&
		    (conn->pf_bits & (1 << PROFILE_HID2))) {
			conn->pf_bits &= ~(1 << PROFILE_HID2);
			btpf->pf_refs[PROFILE_HID2]--;
		}
	}

	rtlbt_info("%s: btpf->pf_refs[%u] %d", __func__, idx,
		   btpf->pf_refs[idx]);
	rtlbt_info("%s: pf_bits 0x%02x", __func__, btpf->pf_bits);
	for (i = 0; i < PROFILE_MAX; i++)
		rtlbt_info("%s: btpf->pf_refs[%u] %d", __func__, i,
			   btpf->pf_refs[i]);

	if (need_update)
		btpf_update_to_controller(btpf);

	return 0;
}

static void hid_state_update(struct rtl_btpf *btpf, u16 handle,
			     u16 interval)
{
	u8 update = 0;
	struct rtl_hci_conn *conn;

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn)
		return;

	rtlbt_info("%s: handle 0x%04x, interval 0x%x", __func__, handle,
		   interval);
	if (!(conn->pf_bits & (1 << PROFILE_HID))) {
		rtlbt_dbg("hid not connected in the handle");
		return;
	}

	if (interval < 60) {
		if (!(conn->pf_bits & (1 << PROFILE_HID2))) {
			update = 1;
			conn->pf_bits |= (1 << PROFILE_HID2);

			btpf->pf_refs[PROFILE_HID2]++;
			if (btpf->pf_refs[PROFILE_HID2] == 1)
				btpf->pf_state |= (1 << PROFILE_HID);
		}
	} else {
		if (conn->pf_bits & (1 << PROFILE_HID2)) {
			update = 1;
			conn->pf_bits &= ~(1 << PROFILE_HID2);

			btpf->pf_refs[PROFILE_HID2]--;
			if (!btpf->pf_refs[PROFILE_HID2])
				btpf->pf_state &= ~(1 << PROFILE_HID);
		}
	}

	if (update)
		btpf_update_to_controller(btpf);
}

static int handle_l2cap_conn_req(struct rtl_btpf *btpf, u16 handle, u16 psm,
				 u16 cid, u8 dir)
{
	struct rtl_profile *pf;
	int idx = psm_to_profile(psm);
	struct rtl_profile_id id;
	struct rtl_hci_conn *conn;

	if (idx < 0) {
		rtlbt_info("no need to parse psm %04x", psm);
		return 0;
	}

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn)
		return -1;

	if (dir == RTL_TO_REMOTE) {
		id.match_flags = RTL_PROFILE_MATCH_SCID;
		id.scid = cid;
	} else {
		id.match_flags = RTL_PROFILE_MATCH_DCID;
		id.dcid = cid;
	}

	pf = rtl_profile_lookup(&conn->pf_list, &id);
	if (pf) {
		rtlbt_warn("%s: profile already exists", __func__);
		return -1;
	}

	if (dir == RTL_TO_REMOTE)
		pf = profile_alloc(psm, (u8)idx, 0, cid);
	else
		pf = profile_alloc(psm, (u8)idx, cid, 0);

	if (!pf) {
		rtlbt_err("%s: allocate profile failed", __func__);
		return -1;
	}

	if (psm == PSM_AVDTP) {
		struct rtl_profile *pinfo = lookup_pf_by_psm(conn, psm);

		if (!pinfo) {
			rtlbt_info("%s: Add a2dp signal channel", __func__);
			pf->flags = A2DP_SIGNAL;
		} else {
			rtlbt_info("%s: Add a2dp media channel", __func__);
			pf->flags = A2DP_MEDIA;
		}
	}

	pf->conn = (void *)conn;
	list_add_tail(&pf->list, &conn->pf_list);

	return 0;
}

static int handle_l2cap_conn_rsp(struct rtl_btpf *btpf,
		u16 handle, u16 dcid,
		u16 scid, u8 dir, u8 result)
{
	struct rtl_profile *pf;
	struct rtl_hci_conn *conn;
	struct rtl_profile_id id;

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn) {
		rtlbt_err("%s: no acl connection\n", __func__);
		return -1;
	}

	if (dir == RTL_FROM_REMOTE) {
		id.match_flags = RTL_PROFILE_MATCH_SCID;
		id.scid = scid;
		pf = rtl_profile_lookup(&conn->pf_list, &id);
	} else {
		id.match_flags = RTL_PROFILE_MATCH_DCID;
		id.dcid = scid;
		pf = rtl_profile_lookup(&conn->pf_list, &id);
	}

	if (!pf) {
		rtlbt_err("%s: profile not found", __func__);
		return -1;
	}

	if (!result) {
		rtlbt_info("l2cap connection success");
		if (dir == RTL_FROM_REMOTE)
			pf->dcid = dcid;
		else
			pf->scid = dcid;

		profile_conn_get(btpf, conn, pf->idx);
	}

	return 0;
}

static int handle_l2cap_disconn_req(struct rtl_btpf *btpf,
		u16 handle, u16 dcid,
		u16 scid, u8 dir)
{
	struct rtl_profile *pf;
	struct rtl_hci_conn *conn;
	int err = 0;
	struct rtl_profile_id id = {
		.match_flags = RTL_PROFILE_MATCH_SCID |
			       RTL_PROFILE_MATCH_DCID,
		.scid   = scid,
		.dcid   = dcid,
	};

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn) {
		rtlbt_err("%s: no connection", __func__);
		err = -1;
		goto done;
	}

	if (dir == RTL_FROM_REMOTE) {
		id.scid = dcid;
		id.dcid = scid;
		pf = rtl_profile_lookup(&conn->pf_list, &id);
	} else {
		pf = rtl_profile_lookup(&conn->pf_list, &id);
	}

	if (!pf) {
		rtlbt_err("%s: no profile", __func__);
		err = -1;
		goto done;
	}

	profile_conn_put(btpf, conn, pf->idx);
	/* if the profile is a2dp sink */
	if (pf->idx == PROFILE_A2DP && (conn->pf_bits & (1 << PROFILE_SINK)))
		profile_conn_put(btpf, conn, PROFILE_SINK);
	list_del(&pf->list);
	kfree(pf);

done:
	rtlbt_info("%s: handle %04x, dcid %04x, scid %04x, dir %x",
		   __func__, handle, dcid, scid, dir);

	return err;
}

static void hci_reset_conn(struct rtl_btpf *btpf, struct rtl_hci_conn *conn)
{
	struct list_head *pos, *next;
	struct rtl_profile *pf;

	list_for_each_safe(pos, next, &conn->pf_list) {
		pf = list_entry(pos, struct rtl_profile, list);
		if (pf->scid && pf->dcid) {
			/* If both scid and dcid are bigger than zero,
			 * L2cap connection exists.
			 */
			profile_conn_put(btpf, conn, pf->idx);
			list_del(&pf->list);
			kfree(pf);
		}
	}

	conn->pf_bits = 0;
	memset(conn->pf_refs, 0, PROFILE_MAX);
}

static void hci_conn_complete_evt(struct rtl_btpf *btpf, u16 handle,
				  u8 link_type)
{
	struct rtl_hci_conn *conn;

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn) {
		conn = kzalloc(sizeof(struct rtl_hci_conn), GFP_KERNEL);
		if (conn) {
			conn->handle = handle;
			INIT_LIST_HEAD(&conn->pf_list);
			list_add_tail(&conn->list, &btpf->conn_list);
			conn->pf_bits = 0;
			memset(conn->pf_refs, 0, PROFILE_MAX);
			/* sco or esco */
			if (link_type == 0 || link_type == 2) {
				conn->type = SYNC_CONN;
				profile_conn_get(btpf, conn, PROFILE_SCO);
			} else {
				conn->type = ACL_CONN;
			}
		} else {
			rtlbt_err("%s: hci conn allocate fail.", __func__);
			return;
		}
	} else {
		/* If the connection has already existed, reset connection
		 * information
		 */
		rtlbt_warn("%s: hci conn handle(0x%x) already existed",
			   __func__, handle);
		hci_reset_conn(btpf, conn);
		/* sco or esco */
		if (link_type == 0 || link_type == 2) {
			conn->type = SYNC_CONN;
			profile_conn_get(btpf, conn, PROFILE_SCO);
		} else {
			conn->type = ACL_CONN;
		}
	}
}

static void disconn_profiles(struct rtl_btpf *btpf, struct rtl_hci_conn *conn)
{
	struct list_head *pos, *next;
	struct rtl_profile *pf;

	list_for_each_safe(pos, next, &conn->pf_list) {
		pf = list_entry(pos, struct rtl_profile, list);
		if (pf->scid && pf->dcid) {
			/* If both scid and dcid are bigger than zero,
			 * L2cap connection exists.
			 */
			profile_conn_put(btpf, conn, pf->idx);
			/* Check if there is a2dp sink */
			if (pf->flags == A2DP_MEDIA &&
			    (conn->pf_bits & (1 << PROFILE_SINK)))
				profile_conn_put(btpf, conn, PROFILE_SINK);

			list_del(&pf->list);
			kfree(pf);
		}
	}

	btpf_update_to_controller(btpf);
}

static int hci_disconn_complete_evt(struct rtl_btpf *btpf, u16 handle)
{
	struct rtl_hci_conn *conn;

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn) {
		rtlbt_err("hci conn handle(0x%x) not found", handle);
		return -1;
	}

	switch (conn->type) {
	case ACL_CONN:
		disconn_profiles(btpf, conn);
		break;

	case SYNC_CONN:
		profile_conn_put(btpf, conn, PROFILE_SCO);
		break;

	case LE_CONN:
		profile_conn_put(btpf, conn, PROFILE_HID);
		break;

	default:
		break;
	}

	list_del(&conn->list);
	kfree(conn);

	return 0;
}

static void rtl_le_conn_compl_evt(struct rtl_btpf *btpf, u16 handle,
				  u16 interval)
{
	struct rtl_hci_conn *conn;

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn) {
		conn = kzalloc(sizeof(struct rtl_hci_conn), GFP_KERNEL);
		if (conn) {
			conn->handle = handle;
			INIT_LIST_HEAD(&conn->pf_list);
			list_add_tail(&conn->list, &btpf->conn_list);
			conn->pf_bits = 0;
			memset(conn->pf_refs, 0, PROFILE_MAX);
			conn->type = LE_CONN;
			/* We consider le is the same as hid */
			profile_conn_get(btpf, conn, PROFILE_HID);
			hid_state_update(btpf, handle, interval);
		} else {
			rtlbt_err("%s: hci conn allocate fail.", __func__);
		}
	} else {
		rtlbt_warn("%s: hci conn handle(%x) already existed.", __func__,
			   handle);
		hci_reset_conn(btpf, conn);
		conn->type = LE_CONN;
		profile_conn_get(btpf, conn, PROFILE_HID);
		hid_state_update(btpf, handle, interval);
	}
}
/*
static const char sample_freqs[4][8] = {
	"16", "32", "44.1", "48"
};

static const uint8_t sbc_blocks[4] = { 4, 8, 12, 16 };

static const char chan_modes[4][16] = {
	"MONO", "DUAL_CHANNEL", "STEREO", "JOINT_STEREO"
};

static const char alloc_methods[2][12] = {
	"LOUDNESS", "SNR"
};
*/
//static const uint8_t subbands[2] = { 4, 8 };

void print_sbc_header(struct sbc_frame_hdr *hdr)
{
	rtlbt_info("syncword: %02x", hdr->syncword);
	rtlbt_info("freq %skHz", sample_freqs[hdr->sampling_frequency]);
	rtlbt_info("blocks %u", sbc_blocks[hdr->blocks]);
	rtlbt_info("channel mode %s", chan_modes[hdr->channel_mode]);
	rtlbt_info("allocation method %s",
		   alloc_methods[hdr->allocation_method]);
	rtlbt_info("subbands %u", subbands[hdr->subbands]);
}

static void parse_media_header(u8 *data)
{
	struct sbc_frame_hdr *sbc_header;
	struct rtp_header *rtph;
	u8 bitpool;

	rtph = (struct rtp_header *)data;

	rtlbt_info("rtp: v %u, cc %u, pt %u", rtph->v, rtph->cc, rtph->pt);
	/* move forward */
	data += sizeof(struct rtp_header) + rtph->cc * 4 + 1;

	/* point to the sbc frame header */
	sbc_header = (struct sbc_frame_hdr *)data;
	bitpool = sbc_header->bitpool;

	print_sbc_header(sbc_header);

	rtlbt_info("bitpool %u", bitpool);

	/* TODO: */
	rtk_vendor_cmd_to_fw(0xfc51, 1, &bitpool);
}

static void packet_increment(struct rtl_btpf *btpf, u16 handle,
		u16 ch_id, u8 *payload, u16 length, u8 dir)
{
	struct rtl_profile *pf;
	struct rtl_hci_conn *conn;
	struct rtl_profile_id id;

	conn = rtl_hci_conn_lookup(btpf, handle);
	if (!conn)
		goto done;

	if (conn->type != ACL_CONN)
		return;

	memset(&id, 0, sizeof(id));
	if (dir == RTL_FROM_REMOTE) {
		id.match_flags = RTL_PROFILE_MATCH_SCID;
		id.scid = ch_id;
	} else {
		id.match_flags = RTL_PROFILE_MATCH_DCID;
		id.dcid = ch_id;
	}
	pf = rtl_profile_lookup(&conn->pf_list, &id);
	if (!pf)
		goto done;

	if (pf->idx == PROFILE_A2DP && pf->flags == A2DP_MEDIA) {
		/* avdtp media data */
		if (!(btpf->pf_state & (1 << PROFILE_A2DP))) {
			update_profile_state(btpf, PROFILE_A2DP, 1);
			if (dir == RTL_FROM_REMOTE) {
				if (!(conn->pf_bits & (1 << PROFILE_SINK))) {
					btpf->pf_bits |= (1 << PROFILE_SINK);
					conn->pf_bits |= (1 << PROFILE_SINK);
					profile_conn_get(btpf, conn, PROFILE_SINK);
				}
				update_profile_state(btpf, PROFILE_SINK, 1);
			}

			/* We assume it is SBC if the packet length is bigger
			 * than 100 bytes
			 */
			rtlbt_info("length %u", length);
			if (length > 100)
				parse_media_header(payload);
		}
		btpf->icount.a2dp++;
	}

	if (pf->idx == PROFILE_PAN)
		btpf->icount.pan++;

done:
	return;
}

static void a2dp_timer_func(void *param)
{
	struct rtl_btpf *btpf = param;

	if (btpf->icount.a2dp)
		rtlbt_info("%s: a2dp packets %u", __func__, btpf->icount.a2dp);
	if (!btpf->icount.a2dp) {
		/* TODO: if there are two a2dp links */
		if (btpf->pf_state & (1 << PROFILE_A2DP)) {
			rtlbt_info("%s: a2dp busy->idle!", __func__);
			update_profile_state(btpf, PROFILE_A2DP, 0);
			if (btpf->pf_bits & (1 << PROFILE_SINK))
				update_profile_state(btpf, PROFILE_SINK, 0);
		}
	}
	btpf->icount.a2dp = 0;
	/* mod_timer(&btpf->a2dp_count_timer,
	 * 	  jiffies + msecs_to_jiffies(1000));
	 */
}

static void pan_timer_func(void *param)
{
	struct rtl_btpf *btpf = param;

	if (btpf->icount.pan)
		rtlbt_info("%s: pan packets %d", __func__,
			  btpf->icount.pan);
	if (btpf->icount.pan < PAN_PACKET_COUNT) {
		if (btpf->pf_state & (1 << PROFILE_PAN)) {
			rtlbt_info("%s: pan busy->idle!", __func__);
			update_profile_state(btpf, PROFILE_PAN, 0);
		}
	} else {
		if (!(btpf->pf_state & (1 << PROFILE_PAN))) {
			rtlbt_info("timeout_handler: pan idle->busy!");
			update_profile_state(btpf, PROFILE_PAN, 1);
		}
	}
	btpf->icount.pan = 0;
	/* mod_timer(&btpf->pan_count_timer,
	 * 	  jiffies + msecs_to_jiffies(1000));
	 */
}

void hci_process_evt(struct rtl_btpf *btpf, u8 *p, u16 len)
{
	u8 evt;
	// u8 plen;
	u8 status;
	u8 link_type;
	u8 subevt;
	u16 handle;
	u16 interval;

	evt = p[0];
	// plen = p[1];

	p += 2;

	switch (evt) {
	case HCI_EV_CONN_COMPLETE:
	case HCI_EV_SYNC_CONN_COMPLETE:
		status = p[0];
		if (status)
			return;
		handle = ((u16)p[2]) << 8 | p[1];
		link_type = p[9];
		hci_conn_complete_evt(btpf, handle, link_type);
		break;
	case HCI_EV_DISCONN_COMPLETE:
		/* TODO: Is it needed to check status ? */
		handle = ((u16)p[2]) << 8 | p[1];
		hci_disconn_complete_evt(btpf, handle);
		break;
	case HCI_EV_MODE_CHANGE:
		status = p[0];
		if (status)
			return;
		handle = ((u16)p[2]) << 8 | p[1];
		interval = ((u16)p[5] << 8 | p[4]);
		hid_state_update(btpf, handle, interval);
		break;
	case HCI_EV_LE_META:
		subevt = p[0];
		p++;
		status = p[0];
		if (status)
			return;
		switch (subevt) {
		case HCI_EV_LE_CONN_COMPLETE:
			handle = ((u16)p[2] << 8 | p[1]);
			interval = ((u16)p[12] << 8 | p[11]);
			rtl_le_conn_compl_evt(btpf, handle, interval);
			break;
		case HCI_EV_LE_CONN_UPDATE_COMPLETE:
			handle = ((u16)p[2] << 8 | p[1]);
			interval = ((u16)p[4] << 8 | p[3]);
			hid_state_update(btpf, handle, interval);
			break;
		}
		break;
	default:
		break;
	}

}

void l2_process_frame(struct rtl_btpf *btpf, u8 *data, u16 len, u8 out)
{
	u16 handle;
	u16 flags;
	u16 chan_id;
	u16 psm, scid, dcid, res;
	u16 l2_len;
	u8 code;

	handle = ((u16)data[1] << 8 | data[0]);
	flags = (handle >> 12);
	handle = (handle & 0x0fff);
	data += 4;
	len -= 4;

	/* continuing fragment */
	if (flags == 0x01)
		return;

	l2_len = ((u16)data[1] << 8 | data[0]);
	chan_id = ((u16)data[3] << 8 | data[2]);
	data += 4;
	len -= 4;

	if (chan_id != 0x0001) {
		if (btpf->pf_bits & (1 << PROFILE_A2DP) ||
		    btpf->pf_bits & (1 << PROFILE_PAN))
			packet_increment(btpf, handle, chan_id, data, l2_len,
					 out);
		return;
	}

	if (len < 3)
		return;

	code = data[0];
	data += 4;
	switch (code) {
	case L2CAP_CONN_REQ:
		psm = ((u16)data[1] << 8 | data[0]);
		scid = ((u16)data[3] << 8 | data[2]);
		rtlbt_info("l2cap conn req: psm %04x, scid %04x, out %u",
			   psm, scid, out);
		handle_l2cap_conn_req(btpf, handle, psm, scid, out);
		break;

	case L2CAP_CONN_RSP:
		dcid = ((u16)data[1] << 8 | data[0]);
		scid = ((u16)data[3] << 8 | data[2]);
		res = ((u16)data[5] << 8 | data[4]);
		rtlbt_info("l2cap conn rsp: dcid %04x, scid %04x, res %04x, out %u",
			   dcid, scid, res, out);
		handle_l2cap_conn_rsp(btpf, handle, dcid, scid, out, res);
		break;

	case L2CAP_DISCONN_REQ:
		dcid = ((u16)data[1] << 8 | data[0]);
		scid = ((u16)data[3] << 8 | data[2]);
		rtlbt_info("l2cap disconn req: dcid %04x, scid %04x",
			   dcid, scid);
		handle_l2cap_disconn_req(btpf, handle, dcid, scid, out);
		break;

	case L2CAP_DISCONN_RSP: /* TODO */
	default:
		break;
	}
}

int btpf_open(struct rtl_btpf *btpf)
{
	memset(btpf, 0, sizeof(*btpf));

	/* TODO: init timers of packet counting */

	INIT_LIST_HEAD(&btpf->conn_list);

	return 0;
}

void btpf_close(struct rtl_btpf *btpf)
{
	//struct list_head *pos;
	//struct list_head *n;

	/* TODO: Stop and delete the timers for packet counting */

	rtl_hci_conn_list_purge(btpf);
	memset(btpf, 0, sizeof(*btpf));
}

/* TODO */
int setup_monitor_timer(struct rtl_btpf *btpf, u8 idx)
{

	if (idx == 2) { // a2dp
		timerid[0] = osi_alarm_new("a2dp", a2dp_timer_func, btpf, 0);
		osi_alarm_set_periodic(timerid[0], 1000);
	} else if (idx == 3) { // pan
		timerid[1] = osi_alarm_new("pan", pan_timer_func, btpf, 1000);
		osi_alarm_set_periodic(timerid[1], 1000);
	}

	return 0;
}

void del_monitor_timer(struct rtl_btpf *btpf, u8 idx)
{
	if (idx == 2) { // a2dp
		osi_alarm_cancel(timerid[0]);
		osi_alarm_free(timerid[1]);
	} else if (idx == 3) { // pan
		osi_alarm_cancel(timerid[1]);
		osi_alarm_free(timerid[1]);
	}
}
