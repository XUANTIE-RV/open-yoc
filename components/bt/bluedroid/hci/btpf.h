#ifndef __BTPF_H__
#define __BTPF_H__
#include <stdint.h>

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;

struct list_head {
	struct list_head *next, *prev;
};

#define A2DP_SIGNAL	0x01
#define A2DP_MEDIA	0x02
/* Bluetooth profiling */
enum __profile_type {
	PROFILE_SCO = 0,
	PROFILE_HID = 1,
	PROFILE_A2DP = 2,
	PROFILE_PAN = 3,
	PROFILE_HID2 = 4, /* hid interval */
	PROFILE_HOGP = 5,
	PROFILE_VOICE = 6,
	PROFILE_SINK = 7,
	PROFILE_MAX = 8
};


struct pf_pkt_icount {
	u32 a2dp;
	u32 pan;
	u32 hogp;
	u32 voice;
};

struct rtl_btpf {
	struct list_head	conn_list;

	u8	pf_bits;
	u8	pf_state;
	int	pf_refs[PROFILE_MAX];

	struct pf_pkt_icount	icount;
};

int btpf_open(struct rtl_btpf *btpf);
void btpf_close(struct rtl_btpf *btpf);
void l2_process_frame(struct rtl_btpf *btpf, u8 *data, u16 len, u8 out);
void hci_process_evt(struct rtl_btpf *btpf, u8 *p, u16 len);

#endif

