#include "JSWifiImpl.h"
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>
#include <uservice/eventid.h>
#include <devices/netdrv.h>
#include <devices/wifi.h>
#include <vector>
#include <aos/kv.h>

#define TAG "JS_WiFi"

extern "C" void publish_wifi_jsapi(std::string result, aiot::WifiConnectInfo *_info);
namespace aiot {
static js_wifi_config g_js_wifi_config;
static int wifi_is_pairing = 0;
static int net_reset_delay_time = 2;

static std::vector<aiot::WifiApRecord> records;
static int g_net_gotip = 0;
static aos_mutex_t js_wifi_config_mutex;

/* app & cli & event */
static void wifi_cb_sta_connect_fail(rvm_dev_t *dev, rvm_hal_wifi_err_code_t err, void *arg)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static void wifi_cb_status_change(rvm_dev_t *dev, rvm_hal_wifi_event_id_t stat, void *arg)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static void js_wifi_config_add_ap(js_wifi_config *config, char *ssid)
{
    if (config->ap_num >= WIFI_CONFIG_NUM_MAX_AP) {
        return;
    }
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);
    strcpy(config->ap_ssid[config->ap_num ++], ssid);
    aos_mutex_unlock(&js_wifi_config_mutex);
}

static void print_encrypt_mode(rvm_hal_wifi_encrypt_type_t encryptmode)
{
    switch (encryptmode) {
        case WIFI_ENC_TKIP:
            printf("TKIP");
            break;

        case WIFI_ENC_AES:
            printf("CCMP");
            break;

        case WIFI_ENC_TKIP_AES_MIX:
            printf("TKIP+CCMP");
            break;

        default:
            printf("ERR");
            break;
    }
}

static void wifi_cb_scan_compeleted(rvm_dev_t *dev, uint16_t number, rvm_hal_wifi_ap_record_t *ap_records)
{
    int i;

    printf("\nbssid / channel / signal dbm / flags / ssid\n\n");

    for (i = 0; i < number; i++) {
        rvm_hal_wifi_ap_record_t *ap_record = &ap_records[i];

        printf("%02x:%02x:%02x:%02x:%02x:%02x %02d %d ",
               ap_record->bssid[0], ap_record->bssid[1], ap_record->bssid[2],
               ap_record->bssid[3], ap_record->bssid[4], ap_record->bssid[5],
               ap_record->channel, ap_record->rssi);

        aiot::WifiApRecord record;
        if (ap_record->link_status == WIFI_STATUS_GOTIP) {
            record.status = aiot::WIFI_STATUS_COMPLETED;
        } else {
            record.status = aiot::WIFI_STATUS_DISCONNECTED;
        }
        record.bssid = jq_printf("%02x:%02x:%02x:%02x:%02x:%02x",
               ap_record->bssid[0], ap_record->bssid[1], ap_record->bssid[2],
               ap_record->bssid[3], ap_record->bssid[4], ap_record->bssid[5]);
        record.ssid = (const char*)ap_record->ssid;
        record.rssi = ap_record->rssi;
        record.isEncrypt = ap_record->authmode == WIFI_AUTH_OPEN;
        records.push_back(record);

        switch (ap_record->authmode) {
            case WIFI_AUTH_OPEN:
                printf("[OPEN]");
                break;

            case WIFI_AUTH_WEP:
                printf("[WEP]");
                break;

            case WIFI_AUTH_WPA_PSK:
                printf("[WPA-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                break;

            case WIFI_AUTH_WPA2_PSK:
                printf("[WPA2-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                break;

            case WIFI_AUTH_WPA_WPA2_PSK:
                printf("[WPA-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                printf("[WPA2-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                break;

            default:
                printf("[NOT SUPPORT]");
                break;
        }

        printf("    %s\n",  ap_record->ssid);
    }
    for (int j = 0; j < number; ++j) {
        js_wifi_config_add_ap(&g_js_wifi_config, (char *)ap_records[j].bssid);
    }
}

static void wifi_cb_fatal_err(rvm_dev_t *dev, void *arg)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static rvm_hal_wifi_event_func evt_cb = {
    .sta_connect_fail = wifi_cb_sta_connect_fail,
    .status_change = wifi_cb_status_change,
    .scan_compeleted = wifi_cb_scan_compeleted,     //rtl8723ds_devops.c: 1126
    .fatal_err = wifi_cb_fatal_err
};

static void js_wifi_config_init(js_wifi_config *config){
    LOGD(TAG, "js_wifi_config_init");
    memset(config, 0, sizeof(js_wifi_config));
    int ret = -1;
    char wifi_ssid_key[32];
    char wifi_psk_key[32];
    config->empty_ssid_table = 0xFFFF;

    config->max_num = WIFI_CONFIG_NUM_MAX;
    ret = aos_mutex_new(&js_wifi_config_mutex);
    if (ret != 0) {
        return ;
    }

    for (int i = 0; i < config->max_num; i++) {
        snprintf(wifi_ssid_key, sizeof(wifi_ssid_key), WIFI_CONFIG_SSID_KEY_PATTERN, i);
        if (aos_kv_getstring(wifi_ssid_key, config->ssid[i], sizeof(config->ssid[i])) < 0) {
            // fail to 1，success to 0
            config->empty_ssid_table |= (1 << i);
            LOGD(TAG, "wifi config %d is empty!!",i);
            continue;
        }

        snprintf(wifi_psk_key, sizeof(wifi_psk_key), WIFI_CONFIG_PSK_KEY_PATTERN, i);
        aos_kv_getstring(wifi_psk_key, config->psk[i], sizeof(config->psk[i]));
        config->empty_ssid_table &= ~(1 << i);
        LOGI(TAG, "%s ssid: %s, psk %s", __func__, config->ssid[i], config->psk[i]);  
    }

    if (aos_kv_getint(WIFI_CONFIG_NUM_CUR_KEY, &config->cur_used_ssid) < 0) {
        config->cur_used_ssid = 0;
    }
    LOGD(TAG, "\t %d \t %#X \t", config->cur_used_ssid, config->empty_ssid_table);
}

static void js_wifi_config_add(js_wifi_config *config, const char* ssid, const char* psk){
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);

    LOGD(TAG,"ssid:%s, psk:%s\n",ssid,psk);
    // 比较是否已存在
    for (int i = 0; i < config->max_num; i ++) {
        if (strcmp(config->ssid[i], ssid) == 0) {
                if ((config->empty_ssid_table & (1 << i))) {
                    config->empty_ssid_table &= ~(1 << i);
                }
                strcpy(config->ssid[i], ssid);
                strcpy(config->psk[i], psk);
                // config->cur_used_ssid=i;  // 设置当前使用的WIFI
                // LOGD(TAG, "wifi_config_add %d cur_used_ssid %d", i ,config->cur_used_ssid);
                aos_mutex_unlock(&js_wifi_config_mutex);
                return;
        }
    }
    // wifi config to js_wifi_config
    if(config->empty_ssid_table == 0xFFFF){
        strcpy(config->ssid[config->cur_used_ssid], ssid);
        strcpy(config->psk[config->cur_used_ssid], psk);
        config->empty_ssid_table = 0xFFFE;
    }else{
        strcpy(config->ssid[(config->cur_used_ssid + 1) % config->max_num], ssid);
        strcpy(config->psk[(config->cur_used_ssid + 1) % config->max_num], psk);
        config->empty_ssid_table &= ~(1 << ((config->cur_used_ssid + 1) % config->max_num ));
    }


    LOGD(TAG, "%#x", config->empty_ssid_table);

    // config->cur_used_ssid = ((config->cur_used_ssid + 1) % config->max_num);
    // config->empty_ssid_table &= ~(1 << config->cur_used_ssid);

    aos_mutex_unlock(&js_wifi_config_mutex);
}

static void js_wifi_config_del(js_wifi_config *config, const char* ssid){
    int i;

    if (ssid == NULL) {
        return;
    }

    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < config->max_num; i ++) {
        if (!(config->empty_ssid_table & (1 << i)) && strcmp(config->ssid[i], ssid) == 0) {
            config->empty_ssid_table |= (1 << i);
            memset(config->ssid[i], 0, sizeof(MAX_SSID_SIZE + 1));
            memset(config->psk[i], 0, sizeof(MAX_PASSWD_SIZE + 1));
            
            aos_mutex_unlock(&js_wifi_config_mutex);
            LOGD(TAG, "%d\n", config->empty_ssid_table);
            return;
        }
    }
    
    aos_mutex_unlock(&js_wifi_config_mutex);
}

static void js_wifi_config_change(js_wifi_config *config, const char* ssid, const char* psk){
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);

    LOGD(TAG,"change ssid:%s, psk:%s\n",ssid,psk);
    // 比较是否已存在
    for (int i = 0; i < config->max_num; i ++) {
        if (strcmp(config->ssid[i], ssid) == 0 &&
            strcmp(config->psk[i], psk) == 0) {
                if ((config->empty_ssid_table & (1 << i))) {
                    config->empty_ssid_table &= ~(1 << i);
                }
                aos_mutex_unlock(&js_wifi_config_mutex);
                return;
        }
    }
    // wifi config to js_wifi_config
    strcpy(config->ssid[(config->cur_used_ssid + 1) % config->max_num], ssid);
    strcpy(config->psk[(config->cur_used_ssid + 1) % config->max_num], psk);

    LOGD(TAG, "%d,%d\n", config->empty_ssid_table, config->empty_ssid_table);
    aos_mutex_unlock(&js_wifi_config_mutex);
}

static int js_wifi_config_select_ssid(js_wifi_config *config, char **select_ssid, char **select_psk){
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);
    int i, j;
    int old_ssid = config->cur_used_ssid;
    for (j = 0; j < config->ap_num; j ++) {

        for (i = 0; i < config->max_num; i++) {
            config->cur_used_ssid = ((config->cur_used_ssid + 1) % config->max_num);
            if (old_ssid != config->cur_used_ssid && (!(config->empty_ssid_table & (1 << config->cur_used_ssid)) &&
                    (strcmp(config->ap_ssid[j], config->ssid[config->cur_used_ssid]) == 0) )) {
                *select_ssid = config->ssid[config->cur_used_ssid];
                *select_psk  = config->psk[config->cur_used_ssid];
                aos_mutex_unlock(&js_wifi_config_mutex);
                return config->cur_used_ssid;
            }
        }
    }

    /* if no qualified ssid, just pick the next saved ssid */
    for (i = 0; i < config->max_num; i++) {
        config->cur_used_ssid = ((config->cur_used_ssid + 1) % config->max_num);
        if (!(config->empty_ssid_table & (1 << config->cur_used_ssid))) {
            *select_ssid = config->ssid[config->cur_used_ssid];
            *select_psk  = config->psk[config->cur_used_ssid];
            aos_mutex_unlock(&js_wifi_config_mutex);
            return config->cur_used_ssid;
        }
    }
    aos_mutex_unlock(&js_wifi_config_mutex);
    return 0;
}

static void js_wifi_config_list(js_wifi_config *config, std::vector<WifiConfig>* configList){
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);
    WifiConfig _configList;

    for (int i = 0; i < config->max_num; i ++) {
        // 添加检测 列表 是否为空

        _configList.ssid = config->ssid[i];
        _configList.psk = config->psk[i];

        if(_configList.ssid == ""){
            continue;
        }
        configList->push_back(_configList);
    }
    aos_mutex_unlock(&js_wifi_config_mutex);
}

static void js_wifi_config_save(js_wifi_config *config){
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);
    char wifi_ssid_key[32];
    char wifi_psk_key[32];
    char ssid[33] = {0};
    char psk[65] = {0};
    int i;
    int cur = 0;

    for (i = 0; i < config->max_num; i++) {
        snprintf(wifi_ssid_key, sizeof(wifi_ssid_key), WIFI_CONFIG_SSID_KEY_PATTERN, i);
        snprintf(wifi_psk_key, sizeof(wifi_psk_key), WIFI_CONFIG_PSK_KEY_PATTERN, i);

        // 删除wifi config for kv操作
        if (config->empty_ssid_table & (1 << i)) {
            aos_kv_del(wifi_ssid_key);
            aos_kv_del(wifi_psk_key);
            LOGD(TAG, "del !!!!!!!!!!!!!!!!! %s", wifi_ssid_key);
            continue;
        }

        aos_kv_getstring(wifi_ssid_key, ssid, sizeof(ssid));
        aos_kv_getstring(wifi_psk_key, psk, sizeof(psk));

        if (strcmp(ssid, config->ssid[i]) != 0 || strcmp(psk, config->psk[i]) != 0) {
            LOGI(TAG, "%s ssid: %s, psk %s\n", __func__, config->ssid[i], config->psk[i]);
            aos_kv_setstring(wifi_ssid_key, config->ssid[i]);
            aos_kv_setstring(wifi_psk_key, config->psk[i]);
            config->cur_used_ssid = i;
        }
    }

    int ret = aos_kv_getint(WIFI_CONFIG_NUM_CUR_KEY, &cur);
    //获取kv中当前的cur
    if (ret < 0 || cur != config->cur_used_ssid) {
        aos_kv_setint(WIFI_CONFIG_NUM_CUR_KEY, config->cur_used_ssid);
    }
    aos_mutex_unlock(&js_wifi_config_mutex);
}

static int js_wifi_ctrl_connect(js_wifi_config *config, const char* ssid){
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);

    rvm_hal_wifi_config_t wifi_config;
    wifi_config.mode = WIFI_MODE_STA;

    LOGD(TAG,"connect ssid:%s\n",ssid);
    // compare is exist
    for (int i = 0; i < config->max_num; i ++) {
        if (strcmp(config->ssid[i], ssid) == 0) {
            strcpy(wifi_config.ssid, config->ssid[i]);
            strcpy(wifi_config.password, config->psk[i]);
        }
    }

    netmgr_hdl_t netmgr_hdl = netmgr_get_handle("wifi");
    netmgr_config_wifi(netmgr_hdl,  wifi_config.ssid, strlen(wifi_config.ssid), 
                                    wifi_config.password, strlen(wifi_config.password));
    rvm_dev_t *dev = netmgr_get_dev(netmgr_hdl);
    rvm_hal_net_set_hostname(dev, "T-head");
    if(0 == netmgr_start(netmgr_hdl))
    {
        LOGD(TAG, "ssid{%s}, psk{%s}\n", wifi_config.ssid, wifi_config.password);
        aos_mutex_unlock(&js_wifi_config_mutex);
        return 0;
    }else{
        LOGD(TAG, "net_start fail!!");
        aos_mutex_unlock(&js_wifi_config_mutex);
        return -1;
    }
    
}

static void user_local_event_cb(uint32_t event_id, const void *param, void *obj)
{
    netmgr_hdl_t netmgr_hdl = netmgr_get_handle("wifi");
    aos_mutex_lock(&js_wifi_config_mutex, AOS_WAIT_FOREVER);
    //rvm_dev_t *dev = netmgr_get_dev(netmgr_hdl);
    //unsigned char mac[6] = {0};
    int reason = *(int *)param;
    Bson::array result;
    rvm_dev_t *wifi_dev = NULL;
    wifi_dev = rvm_hal_device_open("wifi");
    rvm_hal_wifi_ap_record_t ap_info;
    WifiConnectInfo *C_Info = new WifiConnectInfo();
    char* bssid = (char *) malloc(256);

    rvm_hal_wifi_sta_get_link_status(wifi_dev, &ap_info);
    sprintf(bssid, "%02x:%02x:%02x:%02x:%02x:%02x", ap_info.bssid[0], ap_info.bssid[1], ap_info.bssid[2],ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]);
    (*C_Info).bssid = bssid;
    LOGD(TAG, " %s", bssid);

    (*C_Info).ssid = g_js_wifi_config.ssid[g_js_wifi_config.cur_used_ssid];
    std::string s( (char *) ap_info.ssid);
    (*C_Info).ssid = s;
    free(bssid);

    if (event_id == EVENT_NETMGR_GOT_IP) {
        g_net_gotip = 0;
        wifi_is_pairing = 1;
        LOGI(TAG, "Got IP");
        // js_wifi_config_save(&WIFIconfig);
        //这里获取当前连接到的WiFi信息
        (*C_Info).reason = WIFI_EVENT_COMPLETED;
        publish_wifi_jsapi(WIFI_EVENT_COMPLETED, C_Info);
        LOGD(TAG, "publish connect success !!!!");
    } else if (event_id == EVENT_NETMGR_NET_DISCON) {
        g_net_gotip++;
        wifi_is_pairing = 0;
        LOGI(TAG, "Net down");
        LOGD(TAG, "[[[[[[net reset cnt %d reson:%d]]]]]]", g_net_gotip, reason);

            //在这里返回WiFi断开的原因
        if (reason == NET_DISCON_REASON_WIFI_NOEXIST) {
            LOGD(TAG, "AP not found");
            (*C_Info).bssid = " ";
            (*C_Info).reason = WIFI_DISCONNECT_REASON_CONNECT_FAILED;
            publish_wifi_jsapi(WIFI_EVENT_DISCONNECTED, C_Info);
        } else if (reason == NET_DISCON_REASON_WIFI_PSK_ERR) {
            LOGD(TAG, "password error");
            (*C_Info).bssid = " ";
            (*C_Info).reason = WIFI_DISCONNECT_REASON_AUTH_FAILED;
            publish_wifi_jsapi(WIFI_EVENT_DISCONNECTED, C_Info);
        } else if (reason == NET_DISCON_REASON_NORMAL){
            LOGD(TAG, "normal discon \n");
            (*C_Info).bssid = " ";
            (*C_Info).reason = WIFI_DISCONNECT_REASON_CONNECT_FAILED;
            publish_wifi_jsapi(WIFI_EVENT_DISCONNECTED, C_Info);
        }
        if(!wifi_is_pairing){
            if(g_net_gotip == 3){
                char *ssid = NULL, *psk = NULL;
                // stop cur wifi
                if(0 == netmgr_stop(netmgr_hdl)){
                    LOGD(TAG, "stop cur success");
                }
                int ret = js_wifi_config_select_ssid(&g_js_wifi_config, &ssid, &psk);
                LOGD(TAG, "check new ssid:%s,psk:%s", ssid, psk);
                if (ret >= 0) {
                    netmgr_config_wifi(netmgr_hdl, ssid, strlen(ssid), psk,
                                        strlen(psk));
                }
            }
            netmgr_reset(netmgr_hdl, net_reset_delay_time);
            /* double delay time to reconnect */
            net_reset_delay_time *= 2;
            if (net_reset_delay_time > NET_RESET_DELAY_TIME) {
                net_reset_delay_time = NET_RESET_DELAY_TIME;
            }
        }

    } else {
        
    }    /*do exception process */
    aos_mutex_unlock(&js_wifi_config_mutex);
}

void JSWifiImpl::init(JQuick::sp<JQPublishObject> pub)
{
    *_pub = *pub;
    js_wifi_config_init(&g_js_wifi_config);

    netmgr_hdl_t netmgr_hdl = netmgr_get_handle("wifi");
    rvm_dev_t *dev = netmgr_get_dev(netmgr_hdl);
    // TODO: connect by ssid
    rvm_hal_wifi_install_event_cb(dev, &evt_cb);

    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb,this);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, this);
    event_subscribe(EVENT_NETMGR_NET_CONFIG, user_local_event_cb, this);    
}

void JSWifiImpl::scan(ScanCallback cb)
{
    wifi_scan_config_t scan_config;
    
    netmgr_hdl_t netmgr_hdl = netmgr_get_handle("wifi");
    rvm_dev_t *dev = netmgr_get_dev(netmgr_hdl);

    memset(scan_config.ssid, 0, sizeof(scan_config.ssid));
    memset(scan_config.bssid, 0, sizeof(scan_config.bssid));
    scan_config.channel = 0;
    scan_config.show_hidden = 0;
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scan_config.scan_time.active.max = 0;
    scan_config.scan_time.active.min = 0;
    scan_config.scan_time.passive = 0;

    rvm_hal_wifi_install_event_cb(dev, &evt_cb);
    // LOGI(TAG, "wifi block scan");
    rvm_hal_wifi_start_scan(dev, &scan_config, 1);

    cb(records);
    records.clear();
}

void JSWifiImpl::addConfig(const std::string &ssid, const std::string &psk)
{
    js_wifi_config_add(&g_js_wifi_config, ssid.c_str(), psk.c_str());
    js_wifi_config_save(&g_js_wifi_config);
}

void JSWifiImpl::removeConfig(const std::string &ssid)
{
    js_wifi_config_del(&g_js_wifi_config, ssid.c_str());
    js_wifi_config_save(&g_js_wifi_config);
}

void JSWifiImpl::changeConfig(const std::string &ssid, const std::string &psk)
{
    js_wifi_config_change(&g_js_wifi_config, ssid.c_str(), psk.c_str());
    js_wifi_config_save(&g_js_wifi_config);
}

std::vector<WifiConfig> JSWifiImpl::listConfig()
{
    // TODO: get config list
    std::vector<WifiConfig> configList;
    js_wifi_config_list(&g_js_wifi_config, &configList);
    return configList;
}

int JSWifiImpl::connect(const std::string &ssid)
{
    if(0 == js_wifi_ctrl_connect(&g_js_wifi_config, ssid.c_str())){
        return 0;
    }else {
        return -1;
    }
}

int JSWifiImpl::disconnect()
{
    // TODO: do disconnect current ssid connection
    netmgr_hdl_t netmgr_hdl = netmgr_get_handle("wifi");

    if(0 == netmgr_stop(netmgr_hdl))
    {
        LOGD(TAG, "wifi netmgr_stop success !");
        WifiConnectInfo *C_Info = new WifiConnectInfo();
        char* bssid = (char *) malloc(256);
        rvm_dev_t *wifi_dev = NULL;
        wifi_dev = rvm_hal_device_open("wifi");
        rvm_hal_wifi_ap_record_t ap_info;
        rvm_hal_wifi_sta_get_link_status(wifi_dev, &ap_info);
        sprintf(bssid, "%02x:%02x:%02x:%02x:%02x:%02x", ap_info.bssid[0], ap_info.bssid[1], ap_info.bssid[2],ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]);
        (*C_Info).bssid = bssid;
        (*C_Info).ssid = g_js_wifi_config.ssid[g_js_wifi_config.cur_used_ssid];
        (*C_Info).reason = WIFI_DISCONNECT_REASON_NORMAL;
        publish_wifi_jsapi(WIFI_EVENT_DISCONNECTED, C_Info);
    }else{
        LOGD(TAG, "wifi netmgr_stop fail !");
        return -1;
    }
    if(!netmgr_is_linkup(netmgr_hdl))
        LOGD(TAG, "link down!!");
    return 0;
}

}  // namespace aiot