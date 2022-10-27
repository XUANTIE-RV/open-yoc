#include <ble_os.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <atomic.h>
#include <misc/util.h>
#include <misc/byteorder.h>

#include <net/buf.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_NET)

#include "common/log.h"

#include "crypto.h"
#include "adv.h"
#include "mesh.h"
#include "net.h"
#include "lpn.h"
#include "friend.h"
#include "proxy.h"
#include "ble_transport.h"
#include "access.h"
#include "foundation.h"
#include "beacon.h"
#include "settings.h"
#include "prov.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_prov.h"
#include "provisioner_main.h"
#include "provisioner_proxy.h"
#endif

#ifdef CONFIG_BT_MESH_EVENT_CALLBACK
#include "mesh_event_port.h"
#endif

#if defined(CONFIG_BT_MESH_EXT_ADV)	&& CONFIG_BT_MESH_EXT_ADV > 0
#include "ext_net.h"
#endif

extern bool bt_mesh_net_rx_stat;

#ifndef CONFIG_EXT_NET_ADV_BUF_MAX_FRAG_SIZE
#define CONFIG_EXT_NET_ADV_BUF_MAX_FRAG_SIZE 10
#endif


typedef struct {
    struct net_buf *head_node;
    u16_t dst_addr;
    u8_t  flag;
} buf_frag_head;

static buf_frag_head buf_heads[CONFIG_EXT_NET_ADV_BUF_MAX_FRAG_SIZE] = {0x0};

static void insert_buf(struct net_buf *head, struct net_buf *buf,const struct bt_mesh_send_cb *cb, void *cb_data)
{

    if(head && buf) {
        net_buf_frag_add(head, buf);
    }

    BT_MESH_ADV(buf)->cb = cb;
    BT_MESH_ADV(buf)->cb_data = cb_data;
    BT_MESH_ADV(buf)->busy = 1;
    net_buf_unref(buf);

}


int ext_frag_buffer( struct net_buf *buf, uint8_t frag, uint16_t dst_addr, const struct bt_mesh_send_cb *cb, void *cb_data)
{
    BT_DBG("Ext frag type:%02x Dst:%04x %p",frag,dst_addr,buf);
    uint8_t i = 0;
    if(frag == EXT_NET_TRANS_FIRST) {
        for(i = 0 ; i < CONFIG_EXT_NET_ADV_BUF_MAX_FRAG_SIZE; i++) {
            if(buf_heads[i].head_node == NULL) {
                buf_heads[i].head_node = buf;
                buf_heads[i].dst_addr  = dst_addr;
                insert_buf(NULL,buf,cb,cb_data);
                return 0;
            }
        }
    } else if(frag == EXT_NET_TRANS_INTERM || frag == EXT_NET_TRANS_LAST) {
        for(i = 0 ; i < CONFIG_EXT_NET_ADV_BUF_MAX_FRAG_SIZE; i++) {
            if(buf_heads[i].head_node && buf_heads[i].dst_addr == dst_addr) {
                insert_buf(buf_heads[i].head_node, buf, cb, cb_data);
                if(frag == EXT_NET_TRANS_LAST) {
					k_sched_disable();
                    bt_mesh_adv_send(buf_heads[i].head_node, BT_MESH_ADV(buf_heads[i].head_node)->cb, BT_MESH_ADV(buf_heads[i].head_node)->cb_data);
				    k_sched_enable();
                    memset(&buf_heads[i],0,sizeof(buf_frag_head));
                    return 0;
                }
            }
        }

    }

	if(i == CONFIG_EXT_NET_ADV_BUF_MAX_FRAG_SIZE) {
		return -ENOMEM;
	}

    return 0;
}


