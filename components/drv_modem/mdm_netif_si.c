/**
 * File: mdm_netif_si.c
 * Brief: Implementation of Sanechips
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author: Zhao Hao
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include <assert.h>
#include <stdlib.h>

#include <k_api.h>
#include <k_timer.h>
#include <k_list.h>
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/ip_addr.h"
#include "arpa/inet.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/ip4.h"
#include "lwip/ip6.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/netifapi.h"
#include <aos/aos.h>

#include <devices/device.h>
#include <devices/ethernet.h>
#include <devices/netdrv.h>

#include "oss.h"

#include "mdm_netif_si.h"
#include "mdm_ws_format.h"

#ifdef CONFIG_YUNOS_PSM
#include "psm.h"
#include <bsp/yunos_bsp_pow.h>
#endif
#include <aos/yloop.h>

#define LOG_DOMAIN "[mdm/mdm_netif_si]"

static char *TAG = "MDM";

typedef struct {
    aos_dev_t device;
    struct netif netif;
    void *priv;
} eth_dev_t;


extern void mdm_zping_recv_debug(struct pbuf *skb, struct netif *dev);
extern void mdm_zping_send_debug(char *psbuf, struct netif *dev);

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
#define MAIL_FAIL_LIMIT 10
#define RETRYTIME       100

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
#define MDM_DEBUGF(LEVEL, fmt, ...)  LWIP_DEBUGF(LEVEL, (LOG_DOMAIN fmt, \
        ##__VA_ARGS__))

#define MDM_PRINT_PACKET(buf,type)  wireshark_format_print(buf,type)

#define MDM_PACKET_NUM_RECORD(type)  mdm_packet_num_record(type)

#ifdef CONFIG_ZPING
#define MDM_ZPING_RECV_DEBUG(pbuf,netif) mdm_zping_recv_debug(pbuf,netif)
#define MDM_ZPING_SEND_DEBUG(psbuf,netif) mdm_zping_send_debug(psbuf,netif)
#else
#define MDM_ZPING_RECV_DEBUG(pbuf,netif)
#define MDM_ZPING_SEND_DEBUG(psbuf,netif)
#endif

/*******************************************************************************
 *                        Local function declarations                          *
 ******************************************************************************/

/*******************************************************************************
 *                         Local variable definitions                          *
 ******************************************************************************/

eth_dev_t *nbiot_dev;

extern u8_t netif_num;

//记录接收和发送数据包个数
static u32_t packet_recv_num = 0;
static u32_t packet_send_num = 0;

#ifdef CONFIG_MODEM_DATA_ADDRESS_RECORD
//记录核间通信发送的数据地址，然后和接收的free的地址比较，查看是否一致
static int mdm_data_addr[ADDR_RECORD_NUM] = {0};

////记录AP发送丢包的个数:UP模式发包会一直申请内存往MODEM发送，当超过一定数量AP就丢弃该数据包，防止内存耗尽
static u32_t send_packet_drop_num = 0;
#endif

//记录AP接收丢包的个数:当AP内存不足时，收到包后就会丢弃
static u32_t recv_packet_drop_num = 0;

//记录modem来的脏包
static u32_t unknow_ip_packet = 0;

//记录无法投入TCPIP处理的包
static u32_t mbox_fail_packet = 0;

static ktimer_t  mdm_sendtimer;
klist_t mdm_send_list;
static aos_sem_t g_mdm_send_sem;

//核间通信不让睡
#ifdef CONFIG_YUNOS_PSM
static s32_t  psm_num;
#endif

/*******************************************************************************
 *                      Local function implementations                         *
 ******************************************************************************/
//核间通信发送数据地址记录
#ifdef CONFIG_MODEM_DATA_ADDRESS_RECORD
static int mdm_data_addr_record(void *data)
{
    int i = 0;
    int write_in = -1;

    for (i = 0; i < ADDR_RECORD_NUM; i++) {
        if (mdm_data_addr[i] == 0) {
            mdm_data_addr[i] = (int)data;
            write_in = 1;
            break;
        }
    }

    if (write_in == -1) {
        return -1;
    }

    return 1;
}

//核间通信free数据地址查找
static void mdm_data_addr_find(void *data)
{
    int i = 0;
    int find = -1;

    for (i = 0; i < ADDR_RECORD_NUM; i++) {
        if (mdm_data_addr[i] == (int)data) {
            mdm_data_addr[i] = 0;
            find = 1;
            break;
        }
    }

    if (find == -1) {
        LWIP_ASSERT("can't find free data address of modem (mdm_netif_si.c)!!!", 0);
    }
}
#endif

//记录收、发数据包个数
static void mdm_packet_num_record(int type)
{
    if (type == MDM_PACKET_RECV) {
        packet_recv_num++;
    } else if (type == MDM_PACKET_SEND) {
        packet_send_num++;
    } else {
        LWIP_DEBUGF(NETDOG_DEBUG, ("packet  recv or send type unknow!\n"));
    }
}

//检测modem的数据是否为ipv4或ipv6
static u8_t mdm_packet_check(char *data, unsigned int len)
{
    struct ip_hdr *iphdr = NULL;
    //struct ip6_hdr *ip6hdr =  NULL;
    unsigned int datalen;

    char ip_type = data[0];
    ip_type = ip_type >> 4;

    if (ip_type == 4) {
        iphdr = (struct ip_hdr *)data;
        datalen = IPH_LEN(iphdr);

        if (datalen != len) {
            return -1;
        }
    } else if (ip_type == 6) {
        //ip6hdr = (struct ip6_hdr *)data;
        //datalen = ip6hdr->_plen + sizeof(struct ip6_hdr);
        //if (datalen != len)
        return -1;
    } else {
        return -1;
    }

    return 0;
}


//网络设备激活，配置IP地址
static void act_netdev(struct pdp_active_info *pdpactinfo, struct netif *netif)
{
#if LWIP_IPV4
    ip4_addr_t netmask, ipaddr, gw;
#endif
    struct pdp_ip_info *actinfo = &(pdpactinfo->act_info);

    if (actinfo->ip46flag & PDP_V4_ACTIVE) {
#if LWIP_IPV4
        ipaddr.addr = actinfo->ip;
        gw.addr = actinfo->gateway;
        IP4_ADDR(&netmask, 255, 255, 255, 255);

        netif_set_addr(netif, &ipaddr, &netmask, &gw);
#endif
    }

    if (actinfo->ip46flag & PDP_V6_ACTIVE) {

       /*
       此处将链路地址设置功能提到ipv6功能激活的最前面，否则链路地址还没有配上，nd6定时器已经启动，链路地址
       为"::"，发出去的RS源地址为"::"，接收到的RA目的地址为"::",lwip现在内部没有目的地址"::"的数据包的接收能力，
       该种包全部转发。
       */
#if LWIP_IPV6
        netif_create_ip6_linklocal_address(netif, 1);
#endif

#if LWIP_IPV6_AUTOCONFIG
        /* IPv6 address autoconfiguration not enabled by default */
        netif->ip6_autoconfig_enabled = 1;
#endif /* LWIP_IPV6_AUTOCONFIG */
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
        netif->rs_count = LWIP_ND6_MAX_MULTICAST_SOLICIT;
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

    }

    if (pdpactinfo->mod_flags & MDM_UP_MOD) {
        netif->mod_flags |= MDM_UP_MOD;
    } else if (pdpactinfo->mod_flags & MDM_CP_MOD) {
        netif->mod_flags |= MDM_CP_MOD;
    } else {
        LWIP_ASSERT("act mod_flags type is not right", 0);
    }

    netif->cid = pdpactinfo->c_id;

    netif_set_link_up(netif);
    netif_set_up(netif);
}


/*******************************************************************************
 *                      Global function implementations                        *
 ******************************************************************************/

#ifdef CONFIG_MODEM_DATA_ADDRESS_RECORD
void mdm_pakcet_num_print(void)
{
    MDM_DEBUGF(MODEM_DEBUG, "\ntotal packet num - recv:%u, send:%u\n",
               packet_recv_num, packet_send_num);
    MDM_DEBUGF(MODEM_DEBUG,
               "\nerr packet num - unknow_ip_packet:%u, mbox_fail_packet:%u\n",
               unknow_ip_packet, mbox_fail_packet);
    MDM_DEBUGF(MODEM_DEBUG, "\ninterface lost num - send:%u, recv:%u\n",
               send_packet_drop_num, recv_packet_drop_num);
}
#endif

int mdm_modflag_set(unsigned char mod_flags)
{
    struct netif *netif  = &nbiot_dev->netif;

    netif->mod_flags = mod_flags;

    return 0;
}

//网络设备激活接口，供AT命令调用
int mdm_netif_pdp_act(struct pdp_active_info *pdpactinfo)
{
    struct netif *netif  = &nbiot_dev->netif;

    switch (pdpactinfo->act_info.ip46flag) { //判断激活类型是否正确，否则断言
        case PDP_V4_ACTIVE:
        case PDP_V6_ACTIVE:
        case PDP_V4_ACTIVE|PDP_V6_ACTIVE:
            break;

        default:
            LWIP_ASSERT("act ip type is not right", 0);
            break;
    }

    if (netif == NULL) {
        LWIP_ASSERT("netif_pdp_act: netif = NULL", 0);
    }

    //内部设置激活模式：CP or UP
    act_netdev(pdpactinfo, netif);

    netif_set_default(
        netif);//此处需要根据激活类型设置默认网口，目前没有源地址路由，设置一个默认网口
    return ERR_OK;
}


//网络设备去活接口
int mdm_netif_pdp_deact(unsigned int c_id, unsigned char ip46flag)
{
    struct netif *netif  = &nbiot_dev->netif;

#if LWIP_IPV6
    int i = 0;
#endif /* LWIP_IPV6 */

    if (netif == NULL) {
        LWIP_ASSERT("netif_pdp_deact: netif = NULL", 0);
    }

    netif->mod_flags = 0;
    netif->cid = 0;

#if LWIP_IPV6_AUTOCONFIG
    /* IPv6 address autoconfiguration not enabled by default */
    netif->ip6_autoconfig_enabled = 0;
#endif /* LWIP_IPV6_AUTOCONFIG */
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
    netif->rs_count = 0;
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

    netif_set_link_down(netif);
    netif_set_down(netif);

#if LWIP_IPV4
    ip_addr_set_zero_ip4(&netif->ip_addr);
    ip_addr_set_zero_ip4(&netif->netmask);
    ip_addr_set_zero_ip4(&netif->gw);
#endif /* LWIP_IPV4 */
#if LWIP_IPV6

    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        ip_addr_set_zero_ip6(&netif->ip6_addr[i]);
        netif->ip6_addr_state[i] = IP6_ADDR_INVALID;
    }

#endif /* LWIP_IPV6 */

    /*此处需要查看去激活的是不是默认网口，但该处实际不需要，内部不会真实删除默认网口。默认网
    去激活后，网口就被down了，路由数据包时，会检查网口是不是up状态-haozhao*/
    netif_set_default(NULL);

    krhino_timer_stop(&mdm_sendtimer);

    return ERR_OK;
}

static unsigned int extract_cid(void *data, unsigned int cid_offset)
{
    char *read_data = (char *)data;

    return (unsigned int)read_data[cid_offset];
}

//UP模式下，用于给数据包前面预留一个空间给modem使用
static unsigned int mdm_get_netbuf_reserve(struct netif *inp)
{
#if 0

    if (inp == NULL) {
        LWIP_ASSERT("inp == NULL!", 0);
        return ERR_VAL;
    }

    if (inp->mod_flags & MDM_UP_MOD) {
        return IP_HEAD_RESERVE + CID_RESERVE;
    } else {

        return 0;
    }

#else
    return IP_HEAD_RESERVE + CID_RESERVE;
#endif
}

#ifdef CONFIG_YUNOS_PSM
void mdm_psm_ref(unsigned int cmd)
{

    if (cmd != MDM_CMD_SEND) {
        return;
    }

    CPSR_ALLOC();
    YUNOS_CPU_INTRPT_DISABLE();
    psm_num++;
    YUNOS_CPU_INTRPT_ENABLE();

    //drv_sleep_flag_set(FLAG_MODEM);
}

void mdm_psm_unref()
{

    CPSR_ALLOC();
    YUNOS_CPU_INTRPT_DISABLE();
    psm_num--;
    YUNOS_CPU_INTRPT_ENABLE();

    if (psm_num < 0) {
        LWIP_ASSERT("psm_num < 0", 0);
    }

    //if( psm_num == 0)
    //  drv_sleep_flag_clr(FLAG_MODEM);
}

#endif

//向cp核发送IP包和free通知
static int mdm_netif_up_send(struct modem_msg *msg)
{
    return oss_icp_channel_send(OSS_ICP_CHANNEL_PS, msg,
                                sizeof(struct  modem_msg));
}

static void mdm_retry_expired(void *timer, void *arg)
{

    klist_t *head = &mdm_send_list;
    klist_t *end  = head;
    klist_t *tmp;
    klist_t *delete_list;
    struct send_data_node *data_node = NULL;
    struct modem_msg *msg;

    if (is_klist_empty(&mdm_send_list)) {
        krhino_timer_stop((ktimer_t *)timer);
        return;
    }

    for (tmp = head->next; tmp != end; tmp = tmp->next) {
        data_node = krhino_list_entry(tmp, struct send_data_node, send_list);
        msg = data_node->msg;

        if (mdm_netif_up_send(msg) < 0) {
            //krhino_timer_start(&mdm_sendtimer);
            return;
        }

#ifdef CONFIG_YUNOS_PSM
        mdm_psm_ref(msg->cmd);
#endif

        MDM_DEBUGF(MODEM_DEBUG,
                   "<== k_timer Send to CP success ,cmd is %d (0:data,1:free) ",
                   data_node->msg->cmd);
        delete_list = tmp;
        tmp = tmp->prev;
        klist_rm(delete_list);
        mem_free(data_node->msg);
        mem_free(data_node);
    }
}

static void mdm_send(void *data, unsigned int len, unsigned int cmd,
                     unsigned int offset)
{
    struct modem_msg msg = { 0 };
    struct modem_msg *retry_msg =  NULL;
    struct send_data_node *node;

    msg.cmd = cmd;
    msg.data = (void *)oss_phy2bus((oss_phy_t)data);
    msg.offset = offset;
    msg.len = len;

    if (mdm_netif_up_send(&msg) < 0) {
        retry_msg = mem_malloc(sizeof(struct modem_msg));
        node = mem_malloc(sizeof(struct send_data_node));
        memcpy(retry_msg, &msg, sizeof(struct modem_msg));

        node->msg = retry_msg;

        MDM_DEBUGF(MODEM_DEBUG,
                   "<== mdm_netif_up_send fail ,add send_list, cmd is %d (0:data,1:free) ", cmd);
        klist_add(&mdm_send_list, &node->send_list);

        if (mdm_sendtimer.timer_state == TIMER_DEACTIVE) {
            krhino_timer_start(&mdm_sendtimer);
        }

        return;
    }


#ifdef CONFIG_YUNOS_PSM
    mdm_psm_ref(cmd);
#endif

    MDM_DEBUGF(MODEM_DEBUG, "<== Send to CP success ,cmd is %d (0:data,1:free) \n",
               cmd);

}

static void mdm_try_send(void *data, unsigned int len, unsigned int cmd,
                         unsigned int offset, int mod)
{
    struct modem_msg msg = {0};

    msg.cmd    = cmd;
    msg.mod    = mod;
    msg.data   = (void *)oss_phy2bus((oss_phy_t)data);
    msg.offset = offset;
    msg.len    = len;

    if (mod == DATA_VIA_CP) {
        /* we shoud wait the sem forever, printf error log to warning user */
        while (aos_sem_wait(&g_mdm_send_sem, aos_kernel_ms2tick(30000))) {
            LOGE(TAG, "mdm send sem wait");
        }
    }

    while (mdm_netif_up_send(&msg) < 0) {
        LOGE(TAG, "mdm send");
        sleep(1);
    }

#ifdef CONFIG_YUNOS_PSM
    mdm_psm_ref(cmd);
#endif

    MDM_DEBUGF(MODEM_DEBUG,
               "<== Send to CP success ,cmd is %d (0:data,1:free), data %x, len %d \n",
               cmd, (u32_t)data, len);
}

#if 0
extern int mdm_send_from_user(char *atcmd, size_t size);
static int at_send_data_to_mdm(unsigned char cid, void *data, unsigned short size)
{
    char at_cmd[48];

    snprintf(at_cmd, 48, "AT+ZSODCP=%d,%d,%u\r", cid, size, (unsigned int)data);

    aos_sem_wait(&g_mdm_send_sem, AOS_WAIT_FOREVER);

    return mdm_send_from_user(at_cmd, strlen(at_cmd) + 1);
}
#endif

//数据包发送接口，ipv4、ipv6最终都使用该接口
static err_t mdm_tx(struct netif *netif, struct pbuf *q)
{
    unsigned int data_len = 0;
    unsigned int copy_len = 0;
    char *read_data = NULL;

    MDM_PRINT_PACKET(q, MDM_PACKET_SEND);

    data_len = q->tot_len;

    read_data = (char *)mem_malloc(data_len + mdm_get_netbuf_reserve(netif));

    if (read_data == NULL) {
        MDM_DEBUGF(MODEM_DEBUG, "AP mem_malloc fail ,try again :retry time %d ms\n",
                   RETRYTIME);
        return ERR_MEM;
    }

    memset(read_data, 0x00, data_len + mdm_get_netbuf_reserve(netif));

    copy_len = pbuf_copy_partial(q, read_data + mdm_get_netbuf_reserve(netif),
                                 data_len, 0);

    if (copy_len != data_len) {

        LWIP_ASSERT("Read pbuf_copy_partial failed!", 0);
        MDM_DEBUGF(MODEM_DEBUG, "Usecase 1: Read failed\n");
        mem_free((void *)read_data);
        return ERR_MEM;
    }

    //调试信息记录
    MDM_PACKET_NUM_RECORD(MDM_PACKET_SEND);
    MDM_ZPING_SEND_DEBUG(read_data + mdm_get_netbuf_reserve(netif), netif);

#ifdef CONFIG_MODEM_DATA_ADDRESS_RECORD

    if (MDM_DATA_ADDR_RECORD(read_data) < 0) {
        mem_free((void *)read_data);
        send_packet_drop_num++;
        MDM_DEBUGF(MODEM_DEBUG, "AP packet list is full, drop send packet num is %u\n",
                   send_packet_drop_num);
        return ERR_MEM;
    }

#endif

#if 0
#ifdef CONFIG_NET_ACTIVE_PDP
    int ret;

    if (netif->mod_flags & MDM_CP_MOD) {

#ifdef CONFIG_YUNOS_PSM
        mdm_psm_ref(MDM_CMD_SEND);
#endif
        ret = at_send_data_to_mdm(netif->cid, (char *)oss_phy2bus((oss_phy_t)read_data),
                                  data_len);

        if (ret) {
            mem_free((void *)read_data);
            return ERR_MEM;
        } else {
            MDM_DEBUGF(MODEM_DEBUG,
                       "<== Send data to CP , CP mod, data %x, data_bus %x \n",
                       (unsigned int)read_data, (unsigned int)oss_phy2bus((oss_phy_t)read_data));
            return ERR_OK;
        }
    }

#endif
#endif

    read_data[IP_HEAD_RESERVE] = netif->cid;//数据段的IP_HEAD_RESERVE位赋cid�?
    if ((netif->mod_flags & MDM_CP_MOD) != 0) {

        mdm_try_send((void *) read_data,
                     (u32_t) data_len + mdm_get_netbuf_reserve(netif), MDM_CMD_SEND,
                     IP_HEAD_RESERVE, DATA_VIA_CP);

    } else if ((netif->mod_flags & MDM_UP_MOD) != 0) {

        mdm_try_send((void *) read_data,
                     (u32_t) data_len + mdm_get_netbuf_reserve(netif), MDM_CMD_SEND,
                     IP_HEAD_RESERVE, DATA_VIA_UP);

    } else {

        LWIP_ASSERT(NULL, "***Mod failed!");
    }

    return ERR_OK;
}

//mdm_rx:从驱动接收到数据，快速转发或投递给tcpip线程处理。提供给驱动做回调函数
static void mdm_rx(void *data, unsigned int len, unsigned int c_id)
{
    struct pbuf *pbuf = NULL;
    struct netif *netif  = &nbiot_dev->netif;

    if (data == NULL || len < 0) {
        LWIP_ASSERT("data == NULL || len < 0", 0);
        return ;
    }

    if (mdm_packet_check(data, len) < 0) {
        unknow_ip_packet++;
        MDM_DEBUGF(MODEM_DEBUG, "input packet is unkown packet, num is %u!\n",
                   unknow_ip_packet);
        return;
    }

    if (netif == NULL) {
        LWIP_ASSERT("pnetif == NULL!", 0);
        return ;
    }

    if (netif_is_up(netif) == 0) {
        return ;
    }

    pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);

    /*核间通讯注册回调中不允许存在耗时操作，且由于核
    间线程优先级已调制最低，那么此处申请不到内存证明
    上层发生了严重错误，丢包是明智的选择*/
    if (pbuf == NULL) {
        recv_packet_drop_num++;
        MDM_DEBUGF(MODEM_DEBUG, "AP memory is full, drop recv packet num is %u\n",
                   recv_packet_drop_num);
        //LWIP_ASSERT("No enough space to malloc char to mdm_rx!", 0);
        return ;
    }

    if (pbuf_take(pbuf, data, len) != ERR_OK) {
        LWIP_ASSERT("pbuf is not big enough!", 0);
        pbuf_free(pbuf);
        pbuf = NULL;
        return ;
    }

    MDM_PACKET_NUM_RECORD(MDM_PACKET_RECV);
    MDM_PRINT_PACKET(pbuf, MDM_PACKET_RECV);
    MDM_ZPING_RECV_DEBUG(pbuf, netif);

    while (netif->input(pbuf, netif) != ERR_OK) {
        mbox_fail_packet++;
        MDM_DEBUGF(MODEM_DEBUG,
                   "mdm_rx: netif input error,drop packet, num is %u !!\n", mbox_fail_packet);
        pbuf_free(pbuf);
        pbuf = NULL;
        return ;
    }
}

/* Recv message or data from CP core throuth ps channel, register by channel_register */
static void mdm_netif_up_recv(void *data, unsigned short len)
{
    struct modem_msg *msg = (struct modem_msg *)data;
    void *data_phy = NULL;
    char *payload = NULL;

    data_phy = (void *)oss_bus2phy((oss_bus_t)msg->data);

    if (msg->cmd == MDM_CMD_FREE) {
#ifdef CONFIG_YUNOS_PSM
        mdm_psm_unref();
#endif

        if (msg->mod == DATA_VIA_CP) {
            aos_sem_signal(&g_mdm_send_sem);
        }

        MDM_DEBUGF(MODEM_DEBUG,
                   "free packet address %x, len: %d\n",
                   (unsigned int)msg->data, msg->len);
        MDM_DATA_ADDR_FIND(data_phy);
        aos_free(data_phy);//和代康商量不用释放msg
        return;
    }

    else if (msg->cmd == MDM_CMD_SEND) {

        MDM_DEBUGF(MODEM_DEBUG, "MDM_CMD_SEND before change:%x------after change:%x\n",
                   (unsigned int)msg->data, (unsigned int)data_phy);

        //CP侧下行数据给我的是指向CID 首部的地址,长度包含CID
        payload = (char *)data_phy + msg->offset +
                  LOCATION_CID; // 指向IP头，msg->offset 是核间reserve 头，LOCATION_CID是CID 占用

        //modem_input内不转化msg->data地址，使AP可见
        //modem_input向tcpip线程内投递数据包的拷贝
        mdm_rx((void *)payload, msg->len - LOCATION_CID, extract_cid(data_phy,
                msg->offset)); //绝对发送成功
        mdm_send(data_phy, msg->len, MDM_CMD_FREE, 0);

        return;
    }

    assert(!"msg->cmd_id is unknow!");
}


#if 0
//CP模式下给atctl调用的接�?接收IP包，该data是AP 不可见地址，需要转�?void mdm_cp_recv(void *data, unsigned int len, unsigned int c_id)
{
    void *data_phy = NULL;

    data_phy = (void *)oss_bus2phy((oss_bus_t)data);

    //modem_input内不转化msg->data地址，使AP可见

    mdm_rx(data_phy, len, c_id);

    mdm_send(data_phy, len, MDM_CMD_FREE, 0);
}
#endif

//数据包发送接口
#if LWIP_IPV4
static err_t ip_output(struct netif *netif, struct pbuf *q,
                       const ip4_addr_t *ipaddr)
{
    return mdm_tx(netif, q);
}
#endif
#if LWIP_IPV6
static err_t ip_output6(struct netif *netif, struct pbuf *q,
                        const ip6_addr_t *ipaddr)
{
    return mdm_tx(netif, q);
}
#endif


// 单个网口初始化
static err_t ipnetif_init(struct netif *netif)
{

#if LWIP_NETIF_HOSTNAME
    netif->hostname = (char *)mem_malloc(HOSTNAME_SIZE);
    memset(netif->hostname, 0x00, HOSTNAME_SIZE);
    sprintf(netif->hostname, "en%d", netif_num);
#endif

    /* should named "en0" for ifconfig check in cli */
    netif->name[0] = 'e';
    netif->name[1] = 'n';
    netif->num = 0;

#if LWIP_IPV4
    netif->output = ip_output;
#endif

#if LWIP_IPV6
    netif->output_ip6 = ip_output6;
#endif
    netif->linkoutput = NULL;
    netif->mtu =
        NET_MTU;//TCP_MSS + 54 + 10;/* mss+net head(ip20+tcp20+mac14) + redundance */
    netif->hwaddr_len = 0;
    netif->flags = 0;

    netif->hwaddr[0] = 0x00;
    netif->hwaddr[1] = 0x1B;
    netif->hwaddr[2] = 0xFC;
    netif->hwaddr[3] = 0x9A;
    netif->hwaddr[4] = 0xA4;
    netif->hwaddr[5] = 0xA0 + 2 * netif_num - 1;

    mdm_at_init();

    return ERR_OK;
}

void mdm_netif_status_cb(struct netif *netif)
{
    if (netif_is_up(netif)) {
        event_publish(EVENT_NBIOT_LINK_UP, netif);
    } else {
        event_publish(EVENT_NBIOT_LINK_DOWN, netif);
    }
}

static void mdm_netif_init(struct netif *netif)
{
    kstat_t ret = 0;

#if LWIP_IPV4
#define IF_ADDRINIT &ipaddr, &netmask, &gw,
    ip4_addr_t ipaddr, netmask, gw;

#ifdef CONFIG_NET_ACTIVE_PDP
    IP4_ADDR(&gw, 0, 0, 0, 0);
    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
#endif

#else /* LWIP_IPV4 */
#define IF_ADDRINIT
#endif /* LWIP_IPV4 */

#ifndef CONFIG_NET_ACTIVE_PDP
    IP4_ADDR(&ipaddr, 192, 168, 103, 150);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 103, 1);
#endif

    netif_add(netif, IF_ADDRINIT NULL, ipnetif_init, tcpip_input);

#ifndef CONFIG_NET_ACTIVE_PDP
#if LWIP_IPV6
    netif_create_ip6_linklocal_address(netif, 1);
    netif->ip6_autoconfig_enabled = 1;
#endif
    netif_set_default(netif);
    netif_set_up(netif);
    netif_set_link_up(netif);
#endif
    netif_set_status_callback(netif, mdm_netif_status_cb);

    oss_icp_channel_register_cb(OSS_ICP_CHANNEL_PS, mdm_netif_up_recv); //注册通道
    //oss_icp_channel_register_cb(OSS_ICP_CHANNEL_RAMDUMP, void_fun); //注册通道
    //oss_icp_channel_register_cb(OSS_ICP_CHANNEL_ZCAT, void_fun); //注册通道

    klist_init(&mdm_send_list);
    ret = krhino_timer_create(&mdm_sendtimer, "Mdm_Sendtimer", mdm_retry_expired,
                              RETRYTIME, RETRYTIME, (void *)NULL, 0);

    if (ret != RHINO_SUCCESS) {

        assert(!"mdm_sendtimer timer creat fail!");
        return;
    }

    aos_sem_new(&g_mdm_send_sem, 1);

    if (g_mdm_send_sem.hdl == NULL) {
        assert(!"g_mdm_send_sem creat fail!");
        return;
    }

    extern int yunos_bsp_icp_eable_irq(void);
    yunos_bsp_icp_eable_irq();
}


/*******************************************************************************
 *                      Global function implementations                        *
 ******************************************************************************/
static int lwip_tcpip_init(void)
{
#ifdef PERF
    perf_init("/tmp/minimal.perf");
#endif /* PERF */

    tcpip_init(NULL, NULL);

    LWIP_PLATFORM_DIAG(("TCP/IP initialized.\n"));

    return 0;
}


#ifdef CONFIG_YOC_LPM
int net_peripheral_restore(void)
{
    return 0;
}
#endif

/*****************************************
* common driver interface
******************************************/
static aos_dev_t *nbiot_dev_init(driver_t *drv, void *config, int id)
{
    aos_dev_t *dev = device_new(drv, sizeof(eth_dev_t), id);
    nbiot_dev = (eth_dev_t *)dev;

    return dev;
}

#define nbiot_dev_uninit device_free

static int nbiot_dev_open(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;
    // power on eth module

    mdm_netif_init(netif);

    return 0;
}

static int nbiot_dev_close(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    netifapi_netif_set_down(netif);
    //netifapi_netif_common(netif, netif_power_down, NULL);

    return 0;
}

int nbiot_start(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    //netifapi_netif_common(netif, low_level_init, NULL);
    netifapi_netif_set_default(netif);
    netifapi_netif_set_up(netif);

    return 0;
}

int nbiot_restart(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    netifapi_netif_set_down(netif);

    //netifapi_netif_common(netif, low_level_init, NULL);

    netifapi_netif_set_default(netif);
    netifapi_netif_set_up(netif);

    return 0;
}


static int nbiot_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask), *netif_ip_netmask4(netif));

    return 0;
}


static int nbiot_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}
// for L3 operations
static net_ops_t nbiot_net_driver = {
    .get_ipaddr = nbiot_get_ipaddr,
    .subscribe = nbiot_subscribe,
};


// for L2
static eth_driver_t nbiot_eth_driver = {
    //.set_packet_filter = eth_set_packet_filter,
    .start = nbiot_start, //start L2 link up, can be put to net driver for all device
    .restart = nbiot_restart,
};

static netdev_driver_t nbiot_driver = {
    .drv = {
        .name   = "nbiot",
        .init   = nbiot_dev_init,
        .uninit = nbiot_dev_uninit,
        .open   = nbiot_dev_open,
        .close  = nbiot_dev_close,
    },
    .link_type = NETDEV_TYPE_NBIOT,
};

void nbiot_driver_register(void)
{
    nbiot_driver.net_ops = &nbiot_net_driver;
    nbiot_driver.link_ops = &nbiot_eth_driver;

    //run eth_dev_init to create eth_dev_t and bind this driver
    driver_register(&nbiot_driver.drv, NULL, 0);
    lwip_tcpip_init();
}

