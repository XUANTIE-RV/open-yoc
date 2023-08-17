#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "jqutil_v2/jqutil.h"
#include "JSgateWayImpl.h"

#define TAG "JSGateWay"

static std::vector<void* > g_jsGateWay_obj;

/* jsapi message publish lock */
static aos_mutex_t g_jsapi_obj_mutex = NULL;
static void jsapi_obj_lock()
{
    if(g_jsapi_obj_mutex == NULL) {
        aos_mutex_new(&g_jsapi_obj_mutex);
    }

    aos_mutex_lock(&g_jsapi_obj_mutex, AOS_WAIT_FOREVER);
}

static void jsapi_obj_unlock()
{
    aos_mutex_unlock(&g_jsapi_obj_mutex);
}

using namespace jqutil_v2;
namespace aiot {
std::vector<scan_subdev_array> js_scandev_array;
std::vector<add_subdev_array> js_adddev_array;

std::vector<nodeConfig> nodelist;

class JSgateWay: public JQPublishObject {
public:
    void OnCtor(JQFunctionInfo &info);
    void OnGCCollect();

    void subDevCtrl(JQAsyncInfo &info);
    void meshCtrl(JQAsyncInfo &info);
    void subDevMgmt(JQAsyncInfo &info);
    void nodeListGet(JQAsyncInfo &info);
    void grpDevCtrl(JQAsyncInfo &info);

private:
    JSgateWayImpl _impl;
};

void JSgateWay::OnCtor(JQFunctionInfo &info)
{
    LOGD(TAG, ">>>>> JSgateWay::OnCtor %x", this);
    _impl.init(this);

    jsapi_obj_lock();
    g_jsGateWay_obj.push_back(this);
    jsapi_obj_unlock();
}

void JSgateWay::OnGCCollect(){
    LOGD(TAG, ">>>>> JSgateWay::OnGCCollect");

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsGateWay_obj.begin(); itor != g_jsGateWay_obj.end(); ){
        if (*itor == this) {
            itor = g_jsGateWay_obj.erase(itor);
        } else {
            itor++;
        }
    }
    jsapi_obj_unlock();
}

void JSgateWay::subDevCtrl(JQAsyncInfo &info)
{
    // 模拟通知 JS 空间扫描结果
    const char* gw_obj = info[0]["operands"].string_value().c_str();
    const char* gw_opt = info[0]["operator"].string_value().c_str();
    int gw_val = info[0]["value"].int_value();

    const char * macAdd = info[1].string_value().c_str();
	if(gw_opt == NULL || gw_obj ==NULL || macAdd == NULL)			
	{
		printf("cjson error...\r\n");
        info.post(-1);
        return;
	}
    LOGD(TAG, "dev mac obj %s", gw_obj);
    LOGD(TAG, "dev mac opt %s", gw_opt);
    LOGD(TAG, "dev mac macadd %s", macAdd);
    
    int ret = _impl.subDevCtrl(gw_obj, gw_opt,gw_val, macAdd);
    info.post(ret);
}

void JSgateWay::meshCtrl(JQAsyncInfo &info)
{
    const char* gw_opt = info[0]["operator"].string_value().c_str();
    int gw_val = info[0]["value"].int_value();
    const char * macAdd = info[1].string_value().c_str();

	if(gw_opt == NULL || macAdd == NULL)			
	{
		printf("cjson error...\r\n");
        info.post(-1);
        return;
	}

    LOGD(TAG, "dev mac opt %s", gw_opt);
    LOGD(TAG, "dev mac val %d", gw_val);
    LOGD(TAG, "dev mac macadd %s", macAdd);

    int ret = _impl.meshCtrl(gw_opt, gw_val , macAdd);
    info.post(ret);
}

void JSgateWay::subDevMgmt(JQAsyncInfo &info)
{
    const char* gw_opt = info[0]["operator"].string_value().c_str();
    const char* gw_type = info[0]["roomType"].string_value().c_str();
    const char* dev_type = info[0]["deviceType"].string_value().c_str();
    const char * macAdd = info[1].string_value().c_str();

	if(gw_opt == NULL || gw_type ==NULL || dev_type == NULL)			
	{
		printf("cjson error...\r\n");
        info.post(-1);
        return;
	}

    int ret = _impl.subDevMgmt(gw_opt, gw_type, dev_type, macAdd);
    info.post(ret);
}
// 用来返回子设备列表
void JSgateWay::nodeListGet(JQAsyncInfo &info)
{
    Bson::array result;
     _impl.nodeListGet();

    if(nodelist.empty() == true){
        LOGD(TAG, "node List is empty");
        info.post(result);
    }else{
        std::vector<nodeConfig>::iterator iter = nodelist.begin();
        for (; iter != nodelist.end(); iter++){
            Bson::object config;
            config["dev_name"] = (*iter).dev_name;
            config["dev_addr"] = (*iter).dev_addr;
            result.push_back(config);
        }
        info.post(result);
    }
}

void JSgateWay::grpDevCtrl(JQAsyncInfo &info)
{
    const char* gw_opt = info[0]["operate"].string_value().c_str();
    int gw_val = info[0]["value"].int_value();
    const char * grpName = info[1].string_value().c_str();

    int ret = _impl.grpDevCtrl(gw_opt, gw_val, grpName);
    info.post(ret);
}

static JSValue creategateWay(JQModuleEnv* env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "gateway");

    // 设定 C++ 对象工厂函数
    tpl->InstanceTemplate()->setObjectCreator([]() {
      return new JSgateWay();
    });
    tpl->SetProtoMethodPromise("subDevCtrl", &JSgateWay::subDevCtrl);
    tpl->SetProtoMethodPromise("meshCtrl", &JSgateWay::meshCtrl);
    tpl->SetProtoMethodPromise("subDevMgmt", &JSgateWay::subDevMgmt);
    tpl->SetProtoMethodPromise("nodeListGet", &JSgateWay::nodeListGet);
    tpl->SetProtoMethodPromise("grpDevCtrl", &JSgateWay::grpDevCtrl);

    JQPublishObject::InitTpl(tpl);

    return tpl->CallConstructor();
}

// extern
JSValue init_aiot_gateWay(JQModuleEnv* env)
{
    // 返回模块
    return creategateWay(env);
}

}  // namespace aiot

int bt_addr_val_to_str(const uint8_t addr[6], char *str, size_t len)                                            //tranlates addr to str
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

struct update_status {
    int16_t subdev;
    uint8_t onoff;
};               //meshdev onoffstatus update

extern "C" void jsapi_gateway_publish_nodeStatusRpt(void* subdevstatus){
    update_status* onoffstatus = static_cast<update_status*>(subdevstatus);

    char mac_str[20];
    memset(mac_str, 0, sizeof(mac_str));
    
    subdev_info_t subdev_info = {0};
    if(gateway_subdev_get_info(onoffstatus->subdev, &subdev_info) == 0) {
        bt_addr_val_to_str(subdev_info.dev_addr, mac_str, sizeof(mac_str));
        LOGD(TAG, "new node index%d the mac:%s\n", onoffstatus->subdev, mac_str);
    }

    Bson::object config;
    config["dev_state"] = (onoffstatus->onoff == 1 ? 1 : 0);
    config["dev_addr"] = std::string(mac_str);
    config["dev_name"] = std::string(subdev_info.name);

    jsapi_obj_lock();
    for(auto& obj : g_jsGateWay_obj) {
        aiot::JSgateWay * self = static_cast<aiot::JSgateWay *>(obj);
        self->publish(aiot::GATEWAY_EVENT_CHANGE_STA, config);
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_gateway_publish_provShowDev(){

    // LOGD(TAG, "%s",dev_addr_str);
    uint8_t                  dev_addr_str[20];
    Bson::array result;
    std::vector<aiot::scan_subdev_array>::iterator iter = aiot::js_scandev_array.begin();
    for (; iter != aiot::js_scandev_array.end(); iter++){
        Bson::object config;
        bt_addr_val_to_str((*iter).gw_info.protocol_info.ble_mesh_info.dev_addr, (char *)dev_addr_str, sizeof(dev_addr_str));
        config["dev_addr"] = (char *)dev_addr_str;
        result.push_back(config);
        LOGD(TAG, "{{{{{exist----%s-----}}}}}",(char *)dev_addr_str);
    }

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsGateWay_obj.begin(); itor != g_jsGateWay_obj.end(); itor++){
        aiot::JSgateWay * self = (aiot::JSgateWay *)(*itor);
        LOGD(TAG, ">>>>> jsapi_JSgateWay_publish %x", self);
        self->publish(aiot::GATEWAY_EVENT_SCAN_COMPLETE, result);
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_gateway_publish_addNodeRst(const char* result, void* gw_evt_param){
    Bson::object config;
    char kvbuf[256];
    memset(kvbuf, 0, sizeof(kvbuf)); 
    gw_evt_subdev_add_t *gw_evt_subdev_add = NULL;
    char kv_index[16];
    char kv_pos[32];
    subdev_info_t subdev_info = { 0 };
    gw_evt_subdev_add = (gw_evt_subdev_add_t *)gw_evt_param;
    int16_t index = gw_evt_subdev_add->subdev;
    gw_subdev_t subdev = index;
    uint8_t mac_str[20];
    int ret = gateway_subdev_get_info(subdev, &subdev_info);
    if(ret == 0)
    {
        bt_addr_val_to_str(subdev_info.dev_addr, (char*)mac_str, sizeof(mac_str));
        LOGD(TAG, "new node index%d the mac:%s\n", subdev, mac_str); 
    }

    for(int i = 1; i < 11; i++){
        snprintf(kv_index, sizeof(kv_index), "index%d", index);

        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if(ret > 0){
            // 键值中有数据
        }else{
            snprintf(kvbuf, sizeof(kvbuf), "%d/%s", subdev, mac_str );
            aos_kv_setstring(kv_index, kvbuf);
        }
        
    }

    if(strcmp(result, "success") == 0){
        config["dev_addr"] = (char*)mac_str;
        config["rst_code"] = GATEWAY_ADD_NODE_SUCCESS;
    }else if(strcmp(result, "fail") == 0){
        config["rst_code"] = GATEWAY_ADD_NODE_OCC_AUTH_FAIL;
        config["dev_addr"] = (char*)mac_str;
    }

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsGateWay_obj.begin(); itor != g_jsGateWay_obj.end(); itor++){
        aiot::JSgateWay * self = (aiot::JSgateWay *)(*itor);
        self->publish(aiot::GATEWAY_EVENT_ADD_RESULT, config);
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_gateway_publish_delNodeRst(const char* result){
    Bson::object config;

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsGateWay_obj.begin(); itor != g_jsGateWay_obj.end(); itor++){
        aiot::JSgateWay * self = (aiot::JSgateWay *)(*itor);
        self->publish(aiot::GATEWAY_EVENT_DEL_RESULT, Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void jsapi_miniapp_init_finish(){
    LOGD(TAG, "jsapi_miniapp_init_finish +++++++++++");

    jsapi_obj_lock();
    std::vector<void* >::iterator itor;
    for (itor = g_jsGateWay_obj.begin(); itor != g_jsGateWay_obj.end(); itor++){
        aiot::JSgateWay * self = (aiot::JSgateWay *)(*itor);
        self->publish("jsapi_miniapp_init_finish", Bson());
    }
    jsapi_obj_unlock();
}

extern "C" void add_scan_meshdev(gw_evt_discovered_info_t* g_gw_msg){
    aiot::scan_subdev_array _gw_info;
    // _gw_info.micAdd = (*dev_addr_str);
    _gw_info.gw_info = (*g_gw_msg);
    uint8_t                  new_dev_addr_str[20];
    uint8_t                  old_dev_addr_str[20];
    bt_addr_val_to_str(g_gw_msg->protocol_info.ble_mesh_info.dev_addr, (char *)new_dev_addr_str, sizeof(new_dev_addr_str));
    
    _gw_info.gw_info.protocol_info.ble_mesh_info.bearer = 0x01;
    _gw_info.gw_info.protocol_info.ble_mesh_info.oob_info = 0x00;
    _gw_info.gw_info.protocol_info.ble_mesh_info.addr_type = 0x00;

    std::vector<aiot::scan_subdev_array>::iterator iter = aiot::js_scandev_array.begin();
    for (; iter != aiot::js_scandev_array.end(); iter++){
        bt_addr_val_to_str((*iter).gw_info.protocol_info.ble_mesh_info.dev_addr, (char *)old_dev_addr_str, sizeof(old_dev_addr_str));
        // LOGD(TAG, "{{{{{compare----%s--%s-----}}}}}",(char *)new_dev_addr_str, (char *)old_dev_addr_str);
        if(strcmp((char *)new_dev_addr_str, (char *)old_dev_addr_str) == 0){
            return;
        }
    }
    for (; iter != aiot::js_scandev_array.end(); iter++){
        bt_addr_val_to_str((*iter).gw_info.protocol_info.ble_mesh_info.dev_addr, (char *)old_dev_addr_str, sizeof(old_dev_addr_str));
        LOGD(TAG, "{{{{{exist----%s-----}}}}}",(char *)old_dev_addr_str);
    }
    aiot::js_scandev_array.push_back(_gw_info);
}

