#include <ble_os.h>
#include <errno.h>
#include <misc/stack.h>
#include <misc/util.h>

#include <net/buf.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <api/mesh.h>
#include "prov.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_ADV)
#include "common/log.h"
#include "net.h"
#include "ext_adv.h"
#include "adv.h"
#include "inc/beacon.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_main.h"
#include "provisioner_prov.h"
#include "provisioner_beacon.h"
#endif



#if defined(CONFIG_BT_MESH_EXT_ADV)	&& CONFIG_BT_MESH_EXT_ADV > 0

enum {
    GATHER_TIMER_STOP,
    GATHER_TIMER_RUN,
};

enum {
    EXT_ADV_GATHER_IDLE,
    EXT_ADV_GATHER_IN,
    EXT_ADV_GATHER_FINSH,
};

k_timer_t  g_gather_timer;
static u8_t g_gather_timer_state = 0;
extern vendor_beacon_cb g_vendor_beacon_cb;
adv_buf_frag_head g_buf_frag_head[CONFIG_EXT_ADV_BUF_MAX_FRAG_SIZE] = {0x0};
struct net_buf_simple *total_buffer = NET_BUF_SIMPLE(MAX_TOTAL_AD_DATA_LEN);


#ifdef CONFIG_BT_MESH_PROVISIONER
extern const bt_addr_le_t *dev_addr;
#endif

extern void bt_mesh_ext_scan_cb(const struct bt_le_scan_recv_info *info, struct net_buf *buf);
extern int ext_process_siggle_buffer(const struct bt_le_scan_recv_info *info, struct net_buf_simple* buf);

u8_t get_net_if(u8_t gap_phy)
{
    switch (gap_phy) {
    case BT_GAP_LE_PHY_1M:
        return BT_MESH_NET_IF_EXT_ADV_1M;
    case BT_GAP_LE_PHY_2M:
        return BT_MESH_NET_IF_EXT_ADV_2M;
    case BT_GAP_LE_PHY_CODED:
        return BT_MESH_NET_IF_EXT_ADV_CODED;
    default:
        return INVALID_NET_IF;
    }
}

int ext_adv_buf_data_process(const struct bt_le_scan_recv_info *info, struct net_buf *buf_head)
{
    int ret = 0;
    net_buf_simple_init(total_buffer,0);
    struct net_buf* temp_head = buf_head;
    if(info->adv_type == BT_LE_ADV_EXT_ADV_COMPLETE && buf_head->frags) {
        while(temp_head) {
            net_buf_simple_add_mem(total_buffer, temp_head->data, temp_head->len);
            temp_head = temp_head->frags;
        }
        ret = ext_process_siggle_buffer(info, total_buffer);
        net_buf_unref(buf_head);
    } else {
        ret = ext_process_siggle_buffer(info,&buf_head->b);
    }
    return ret;
}



static bool is_data_duplicate(struct net_buf *head_buf,struct net_buf *buf)
{
    while(head_buf) {
        if(head_buf->len == buf->len && !memcmp(head_buf->data, buf->data, buf->len)) {
            return true;
        }
        head_buf = head_buf->frags;
    }
    return false;
}


static void gather_timeout(void *timer, void *args)
{
    int timer_now = aos_now_ms();

    for(int i = 0 ; i < CONFIG_EXT_ADV_BUF_MAX_FRAG_SIZE; i++) {
        if(g_buf_frag_head[i].flag == EXT_ADV_GATHER_IN) {
            if(timer_now - g_buf_frag_head[i].start_time >= EXT_ADV_GATHER_TIMEOUT) {
                BT_DBG("Gathertimeout re:%p %d %d",g_buf_frag_head[i].head_node,g_buf_frag_head[i].head_node->ref,timer_now);
                net_buf_unref(g_buf_frag_head[i].head_node);
                memset(&g_buf_frag_head[i],0x0,sizeof(adv_buf_frag_head));
            }
        }
    }

    g_gather_timer_state = GATHER_TIMER_RUN;
    k_timer_start(&g_gather_timer, EXT_ADV_GATHER_TIMEOUT);
}




struct net_buf * ext_adv_buf_data_gather(const struct bt_le_scan_recv_info *info, struct net_buf *buf, u8_t *gather_index)
{
    u8_t unsed_index = HEAD_BUF_INDEX_NOT_FOUND;
    u8_t head_index  = HEAD_BUF_INDEX_NOT_FOUND;
    struct net_buf* clone_buf = NULL;

    for(int i = 0 ; i < CONFIG_EXT_ADV_BUF_MAX_FRAG_SIZE; i++) {
        if(unsed_index == HEAD_BUF_INDEX_NOT_FOUND && !g_buf_frag_head[i].head_node) {
            unsed_index = i;
        }
        if((head_index == HEAD_BUF_INDEX_NOT_FOUND)  &&  (g_buf_frag_head[i].flag == EXT_ADV_GATHER_IN) && \
           !memcmp(g_buf_frag_head[i].info.addr,info->addr,sizeof(bt_addr_le_t)) ) {
            if(info->sid == g_buf_frag_head[i].info.sid) {
                head_index = i;
            }
        }

        if(head_index != HEAD_BUF_INDEX_NOT_FOUND && unsed_index != HEAD_BUF_INDEX_NOT_FOUND) {
            break;
        }
    }

    //todo add mutex
    k_timer_start(&g_gather_timer, EXT_ADV_GATHER_TIMEOUT);

    if(info->adv_type == BT_LE_ADV_EXT_ADV_INCOMPLETE && head_index == HEAD_BUF_INDEX_NOT_FOUND) {
        if(unsed_index == HEAD_BUF_INDEX_NOT_FOUND) {
            BT_DBG("No enough buf head for new adv buf");
            *gather_index = HEAD_BUF_INDEX_NOT_FOUND;
            return NULL;
        } else {
            clone_buf = net_buf_clone(buf,K_NO_WAIT);
            if(!clone_buf) {
                BT_WARN("Not enough space for adv buf clone");
                *gather_index = HEAD_BUF_INDEX_NOT_FOUND;
                return NULL;
            }
            g_buf_frag_head[unsed_index].head_node = clone_buf;
            memcpy(&g_buf_frag_head[unsed_index].info,info,sizeof(struct bt_le_scan_recv_info));
            memcpy(&g_buf_frag_head[unsed_index].addr,info->addr,sizeof(bt_addr_le_t));
            g_buf_frag_head[unsed_index].info.addr = &g_buf_frag_head[unsed_index].addr;
            g_buf_frag_head[unsed_index].start_time = aos_now_ms();
            g_buf_frag_head[unsed_index].flag  = EXT_ADV_GATHER_IN;
            *gather_index = head_index;
            BT_DBG("Add buffer head %p to index:%02x,ref:%d,SID:%d,at %d len:%d",clone_buf,unsed_index, clone_buf->ref, g_buf_frag_head[unsed_index].info.sid,g_buf_frag_head[unsed_index].start_time,clone_buf->len);
            return NULL;
        }
    } else if(info->adv_type == BT_LE_ADV_EXT_ADV_INCOMPLETE && head_index != HEAD_BUF_INDEX_NOT_FOUND) {
        if(is_data_duplicate(g_buf_frag_head[head_index].head_node, buf)) {
            BT_DBG("drop the duplicate data");
            *gather_index = HEAD_BUF_INDEX_NOT_FOUND;
            return NULL;
        }
        clone_buf = net_buf_clone(buf,K_NO_WAIT);
        if(!clone_buf) {
            BT_WARN("Not enough space for adv buf clone");
            *gather_index = HEAD_BUF_INDEX_NOT_FOUND;
            return NULL;
        }
        net_buf_frag_add_with_flags(g_buf_frag_head[head_index].head_node, clone_buf);
        *gather_index = head_index;
        BT_DBG("Add frag %p to head:%p,Ref %d len %d",clone_buf,g_buf_frag_head[head_index].head_node,clone_buf->ref,clone_buf->len);
        return NULL;
    } else if(info->adv_type == BT_LE_ADV_EXT_ADV_COMPLETE && head_index == HEAD_BUF_INDEX_NOT_FOUND) {
        *gather_index = HEAD_BUF_INDEX_NOT_FOUND;

        return buf;
    } else if(info->adv_type == BT_LE_ADV_EXT_ADV_COMPLETE && head_index != HEAD_BUF_INDEX_NOT_FOUND) {
        clone_buf = net_buf_clone(buf,K_NO_WAIT);
        if(!clone_buf) {
            BT_WARN("Not enough space for adv buf clone");
            *gather_index = HEAD_BUF_INDEX_NOT_FOUND;
            return NULL;
        }
        net_buf_frag_add(g_buf_frag_head[head_index].head_node, clone_buf);
        g_buf_frag_head[unsed_index].flag  = EXT_ADV_GATHER_FINSH;
        *gather_index = head_index;
        BT_DBG("Add the last frag %p to head:%p,Ref %d len:%d",clone_buf,g_buf_frag_head[head_index].head_node,clone_buf->ref,clone_buf->len);
        return g_buf_frag_head[head_index].head_node;
    } else {
        *gather_index = HEAD_BUF_INDEX_NOT_FOUND;
        return NULL;
    }

}




/** @brief The scanner has stopped scanning after scan timeout. */
static void bt_mesh_ext_scan_timeout()
{
}


struct bt_le_scan_cb ext_cb = {
    .buf_recv = bt_mesh_ext_scan_cb,
    .timeout = bt_mesh_ext_scan_timeout,
};



int bt_mesh_ext_adv_init()
{
#if defined(CONFIG_BT_MESH_EXT_ADV) && CONFIG_BT_MESH_EXT_ADV
    bt_le_scan_cb_register(&ext_cb);
    k_timer_init(&g_gather_timer, gather_timeout, NULL);
    g_gather_timer_state = GATHER_TIMER_STOP;
    return 0;
#else
    BT_ERR("Ble ext adv should support");
    return -ENOTSUP;
#endif
}


#endif

