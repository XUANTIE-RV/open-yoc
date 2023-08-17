#pragma once
#include "jqutil_v2/jqutil.h"
#include "utils/StrongPtr.h"
#include "jsapi_publish.h"
#include <aos/aos.h>
#include <vector>
#include <string>

#define MAX_SSID_SIZE             32
#define MAX_PASSWD_SIZE           64
#define WIFI_CONFIG_NUM_MAX             5
#define WIFI_CONFIG_NUM_MAX_AP          15
#define WIFI_CONFIG_NUM_CUR_KEY         "wifi_cur"
#define WIFI_CONFIG_SSID_KEY_PATTERN    "wifi_ssid%d"
#define WIFI_CONFIG_PSK_KEY_PATTERN     "wifi_psk%d"
#define NET_RESET_DELAY_TIME         30  /* 最大网络重连时间 second*/

using namespace jqutil_v2;
namespace aiot {
static std::string WIFI_STATUS_DISCONNECTED = "disconnected";
static std::string WIFI_STATUS_COMPLETED = "completed";

typedef struct WifiApRecord {
    std::string status;      /* current ap status */
    std::string bssid;           /* MAC address of AP */
    std::string ssid;            /* SSID of AP */
    int8_t  rssi;                /* signal strength of AP */
    bool isEncrypt;              /* whether has encrypt mode of AP */
    inline Bson toBson()
    {
        Bson::object result;
        result["status"] = status;
        result["bssid"] = bssid;
        result["ssid"] = ssid;
        result["rssi"] = (int)rssi;
        result["isEncrypt"] = isEncrypt;
        return result;
    }
} WifiApRecord;

typedef struct WifiConnectInfo {
    std::string reason;
    std::string bssid;           /* MAC address of AP */
    std::string ssid;            /* SSID of AP */
    inline Bson toBson()
    {
        Bson::object result;
        result["reason"] = reason;
        result["bssid"] = bssid;
        result["ssid"] = ssid;
        return result;
    }
} WifiConnectInfo;

// WIFI config 数据结构
typedef struct {
    std::string ssid;  // primary key of config
    std::string psk;
} WifiConfig;

typedef struct app_wifi_config {
    char    ssid[WIFI_CONFIG_NUM_MAX][MAX_SSID_SIZE + 1];
    char    psk[WIFI_CONFIG_NUM_MAX][MAX_PASSWD_SIZE + 1];
    char    ap_ssid[WIFI_CONFIG_NUM_MAX_AP][MAX_PASSWD_SIZE + 1];
    int     cur_used_ssid;
    uint32_t empty_ssid_table;
    int ap_num;
    int max_num;
} js_wifi_config;


// 全局监听wifi事件，并使用 JQPublisObject::publish 将结果抛送到JS层:
static std::string WIFI_EVENT_SCAN_RESULT = "scan_result";
static std::string WIFI_EVENT_DISCONNECTED = "disconnected";
static std::string WIFI_EVENT_COMPLETED = "connected";

// 在 WIFI_EVENT_DISCONNECTED 事件中，reason 字段有如下两种情况
static std::string WIFI_DISCONNECT_REASON_CONNECT_FAILED = "connect_failed";
static std::string WIFI_DISCONNECT_REASON_AUTH_FAILED = "auth_failed";
static std::string WIFI_DISCONNECT_REASON_NORMAL = "NORMAL_DISCONNECT";


class JSWifiImpl : public JQPublishObject
{ 
public:
    using ScanCallback = std::function<void(std::vector<WifiApRecord> &recordList)>;

public:
    void init(JQuick::sp<JQPublishObject> pub);
    // void init(JQPublishObject pub);
    void scan(ScanCallback cb);
    void addConfig(const std::string &ssid, const std::string &psk);
    void removeConfig(const std::string &ssid);
    void changeConfig(const std::string &ssid, const std::string &psk);
    std::vector<WifiConfig> listConfig();
    int connect(const std::string &ssid);
    int disconnect();

    JQPublishObject* _pub = new JQPublishObject();
private:
};

}  // namespace aiot
