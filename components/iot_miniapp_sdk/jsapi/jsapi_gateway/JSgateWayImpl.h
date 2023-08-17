#pragma once
#include "jqutil_v2/jqutil.h"
#include "utils/StrongPtr.h"
#include <aos/aos.h>
#include <gateway.h>
#include <cJSON.h>
#include <vector>
#include <string>
#include "aos/kv.h"

#include "ulog/ulog.h"
#include "jsapi_publish.h"

using namespace jqutil_v2;
namespace aiot { 

#define GW_MAX_GW_GROUP_NUM 10

typedef struct {
    gw_evt_discovered_info_t gw_info;
}scan_subdev_array;   //meshdev scan infor

typedef struct {
    gw_subdev_t subdev_index;
    gw_evt_discovered_info_t gw_sub_info;
}add_subdev_array;   //meshdev scan infor

extern std::vector<scan_subdev_array> js_scandev_array;
extern std::vector<add_subdev_array> js_adddev_array;

// 支持的子设备类型
#define LIVING_ROOM	0   //客厅
#define BED_ROOM	1   //卧室
#define STUDY_ROOM	2   //书房
#define KITCHEN		3   //厨房
#define SHOWER_ROOM	4   //浴室

// 支持的子设备类型
#define OBJ_LIGHT	0   // 灯
#define OBJ_SCREEN	0   // 灯


// 支持的子设备操作
#define GATEWAY_OPT_ACT_OPEN  "ACT_OPEN"
#define GATEWAY_OPT_ACT_CLOSE  "ACT_CLOSE"
#define GATEWAY_OPT_ACT_BRIVALUE  "ACT_BRIVALUE"
#define GATEWAY_OPT_ACT_CCTVALUE  "ACT_CCTVALUE"

// 添加子设备结果
#define GATEWAY_ADD_NODE_SUCCESS            0
#define GATEWAY_ADD_NODE_OCC_AUTH_FAIL      1
#define GATEWAY_ADD_NODE_AUTOCONFIG_FAIL    2
#define GATEWAY_ADD_NODE_GET_TRIPPLES_FAIL  3

typedef struct {
    const char* mic;
    int value;
}brightness_set;

typedef struct 
{
    std::string dev_name;
    std::string dev_addr;
}nodeConfig;
extern std::vector<nodeConfig> nodelist;

class JSgateWayImpl : public JQPublishObject
{
    
public:
    void init(JQuick::sp<JQPublishObject> pub);
    int subDevCtrl(const char* gw_obj, const char* gw_opt,int gw_val, const char* macAdd);
    int meshCtrl(const char* gw_opt, int gw_val, const char* macAdd);
    int subDevMgmt(const char* gw_obj, const char* gw_type, const char* dev_type, const char* macAdd);
    int grpDevCtrl(const char* gw_opt, int gw_val, const char* macAdd);
    void nodeListGet();

private:
    JQuick::sp<JQPublishObject> _pub;

};
/* JS事件定义 */
static std::string GATEWAY_EVENT_SCAN_COMPLETE = "ProvShowDev";
static std::string GATEWAY_EVENT_ADD_RESULT = "AddNodeRst";
static std::string GATEWAY_EVENT_DEL_RESULT = "DelNodeRst";
static std::string GATEWAY_EVENT_CHANGE_STA = "NodeStatusRpt";

}  // namespace aiot
