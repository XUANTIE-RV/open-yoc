#include "JSgateWayImpl.h"
#include <uservice/eventid.h>
#include <vector>
#include <aos/kv.h>
#include <string.h>

#define TAG "JS_gateWay"
#define ERR_CHAR 0XFF

namespace aiot {
static  aos_timer_t gateway_timer;
static int times = 5;

uint8_t gw_subdev_traverse(gw_subdev_t subdev, void *data)
{
    char nodeL[20];
    gw_status_t   ret         = 0;
    subdev_info_t subdev_info = { 0 };

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD(TAG, "subdev[%d] mac: %02x%02x%02x%02x%02x%02x name: %s", subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0], subdev_info.name);
    }
    snprintf(nodeL, sizeof(nodeL), "%02X:%02X:%02X:%02X:%02X:%02X", subdev_info.dev_addr[5], subdev_info.dev_addr[4], subdev_info.dev_addr[3],
                                                                    subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0]);
    
    nodeConfig temp;
    temp.dev_addr = nodeL;
    temp.dev_name = subdev_info.name;
    nodelist.push_back(temp);

    return GW_SUBDEV_ITER_CONTINUE;
}

uint8_t gw_del_subdev_traverse(gw_subdev_t subdev, void *data)
{
    char nodeL[20];
    gw_status_t   ret         = 0;
    char kv_index[16];
    subdev_info_t subdev_info = { 0 };

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD(TAG, "subdev[%d] mac: %02x%02x%02x%02x%02x%02x name: %s", subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0], (char*)subdev_info.name);
    }
    snprintf(nodeL, sizeof(nodeL), "%02X:%02X:%02X:%02X:%02X:%02X", subdev_info.dev_addr[5], subdev_info.dev_addr[4], subdev_info.dev_addr[3],
                                                                    subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0]);

    LOGD(TAG, ">>>>>compare ---subdev: %s--- del: %s----<<<<<<<",nodeL, (char*)data);
    if(strcmp((char *)nodeL, (char*)data) == 0) {
        snprintf(kv_index, sizeof(kv_index), "index%d", subdev);
        aos_kv_del(kv_index);
        if(gateway_subdev_del(subdev)){
            LOGW(TAG, "Gateway deleted successfully,but subdev failed.");
        }
    }

    return GW_SUBDEV_ITER_CONTINUE;
}

uint8_t  on = atoi("1");
uint8_t  off = atoi("0");
uint8_t gw_subdev_on(gw_subdev_t subdev, void *data)
{
    char nodeL[20];
    gw_status_t   ret         = 0;
    subdev_info_t subdev_info = { 0 };

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD(TAG, "subdev[%d] mac: %02x%02x%02x%02x%02x%02x name: %s", subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0], (char*)subdev_info.name);
    }
    snprintf(nodeL, sizeof(nodeL), "%02X:%02X:%02X:%02X:%02X:%02X", subdev_info.dev_addr[5], subdev_info.dev_addr[4], subdev_info.dev_addr[3],
                                                                    subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0]);

    LOGD(TAG, ">>>>>compare ---subdev: %s--- onoff: %s----<<<<<<<",nodeL, (char*)data);
    if(strcmp((char *)nodeL, (char*)data) == 0) {
        gateway_subdev_set_onoff(subdev, on);
    }

    return GW_SUBDEV_ITER_CONTINUE;
}

uint8_t gw_subdev_off(gw_subdev_t subdev, void *data)
{
    char nodeL[20];
    gw_status_t   ret         = 0;
    subdev_info_t subdev_info = { 0 };

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD(TAG, "subdev[%d] mac: %02x%02x%02x%02x%02x%02x name: %s", subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0], (char*)subdev_info.name);
    }
    snprintf(nodeL, sizeof(nodeL), "%02X:%02X:%02X:%02X:%02X:%02X", subdev_info.dev_addr[5], subdev_info.dev_addr[4], subdev_info.dev_addr[3],
                                                                    subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0]);

    LOGD(TAG, ">>>>>compare ---subdev: %s--- onoff: %s----<<<<<<<",nodeL, (char*)data);
    if(strcmp((char *)nodeL, (char*)data) == 0) {
        gateway_subdev_set_onoff(subdev,off);
    }

    return GW_SUBDEV_ITER_CONTINUE;
}

uint8_t gw_subdev_set_brightness(gw_subdev_t subdev, void *data)
{
    char nodeL[20];
    gw_status_t   ret         = 0;
    subdev_info_t subdev_info = { 0 };
    brightness_set *config = (brightness_set*)data;

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD(TAG, "subdev[%d] mac: %02x%02x%02x%02x%02x%02x name: %s", subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0], (char*)subdev_info.name);
    }
    snprintf(nodeL, sizeof(nodeL), "%02X:%02X:%02X:%02X:%02X:%02X", subdev_info.dev_addr[5], subdev_info.dev_addr[4], subdev_info.dev_addr[3],
                                                                    subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0]);

    LOGD(TAG, ">>>>>compare ---subdev: %s--- val: %d----<<<<<<<",nodeL, config->value);
    if(strcmp((char *)nodeL, (char*)config->mic) == 0) {
            gateway_subdev_set_brightness(subdev, config->value);
    }

    return GW_SUBDEV_ITER_CONTINUE;
}

#if 0
static uint8_t char2u8(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else {
        return ERR_CHAR;
    }
}

static int str2hex(uint8_t hex[], char *s, uint8_t cnt)
{
    uint8_t i;

    if (!s) {
        return 0;
    }

    for (i = 0; (*s != '\0') && (i < cnt); i++, s += 2) {
        hex[i] = ((char2u8(*s) & 0x0f) << 4) | ((char2u8(*(s + 1))) & 0x0f);
    }

    return i;
}

static int str2bt_addr(const char *str, uint8_t addr[6])
{
    uint8_t i, j;
    uint8_t tmp;

    if (strlen(str) != 17 || !addr) {
        return -EINVAL;
    }

    for (i = 5, j = 1; *str != '\0'; str++, j++) {
        if (!(j % 3) && (*str != ':')) {
            return -EINVAL;
        } else if (*str == ':') {
            i--;
            continue;
        }

        addr[i] = addr[i] << 4;

        tmp = char2u8(*str);

        if (tmp == ERR_CHAR) {
            return -EINVAL;
        }

        addr[i] |= tmp;
    }

    return 0;
}

static int _gateway_add_mesh_subdev(int argc, char **argv)
{
    if (argc < 5) {
        LOGE(TAG, "Err argc %d", argc);
        return -1;
    }

    int err = 0;

    uint8_t             addr_type = atoi(argv[1]);
    uint8_t             oob_info  = atoi(argv[3]);
    uint8_t             bearer    = atoi(argv[4]);
    gw_discovered_dev_t subdev;

    if (addr_type != 0x1 && addr_type != 0x0) {
        LOGE(TAG, "Err addr type %02x", addr_type);
        return -1;
    }

    if (bearer != 0x01 && bearer != 0x02) {
        LOGE(TAG, "Err bearer type %02x", bearer);
        return -1;
    }

    err = str2bt_addr((char *)argv[0], subdev.protocol_info.ble_mesh_info.dev_addr);
    if (err) {
        LOGE(TAG, "Err addr data");
        return -1;
    }

    if (str2hex(subdev.protocol_info.ble_mesh_info.uuid, (char *)argv[2], strlen(argv[2])) < 1) {
        LOGE(TAG, "Err uuid data");
        return -1;
    }
    subdev.protocol_info.ble_mesh_info.addr_type = addr_type;
    subdev.protocol_info.ble_mesh_info.oob_info  = oob_info;
    subdev.protocol_info.ble_mesh_info.bearer    = bearer;

    return gateway_subdev_add(subdev);
}
#endif

#define GROUP_NONE        0
#define GROUP_LIVING_ROOM 1
#define GROUP_BED_ROOM    2
#define GROUP_OBJ_LIGHT   3
#define GROUP_OBJ_SCREEN  4
//g_mgmt_ctx.subdev_ctx->subdevs[index].subgrp[GROUP_TYPE_ROOM]=GROUP_LIVING_ROOM/GROUP_BED_ROOM
#define GROUP_TYPE_ROOM   0
//g_mgmt_ctx.subdev_ctx->subdevs[index].subgrp[GROUP_TYPE_DEV]=GROUP_OBJ_LIGHT/GROUP_OBJ_SCREEN
#define GROUP_TYPE_DEV    1

// set subdev room type：无0，客厅1，卧室2，其他
static void _js_gateway_subgrp_set_roomType(int index, const char* gw_type){
    gw_subgrp_t curGroup=gateway_subdev_get_subgrpx(index,GROUP_TYPE_ROOM);

    if(strcmp(gw_type, "LIVING_ROOM") == 0){
        /*
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_LIVING_ROOM,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_LIVING_ROOM,index);
        aos_msleep(100);
        */
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_add_subdev(GROUP_LIVING_ROOM,index);
        gateway_subgrp_add_subdev(GROUP_LIVING_ROOM,index);
        gateway_subdev_set_subgrpx(index,GROUP_TYPE_ROOM,GROUP_LIVING_ROOM);
        return ;
    }else if(strcmp(gw_type, "BED_ROOM") == 0){
        /*
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_BED_ROOM,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_BED_ROOM,index);
        aos_msleep(100);
        */
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_add_subdev(GROUP_BED_ROOM,index);
        gateway_subgrp_add_subdev(GROUP_BED_ROOM,index);
        gateway_subdev_set_subgrpx(index,GROUP_TYPE_ROOM,GROUP_BED_ROOM);
        return ;
    }else {
        // 遇到不支持的组类型
        LOGD(TAG, "error, this is type is not support");
    }
}

// set subdev dev type
static void _js_gateway_subgrp_set_dev_type(int index, const char* dev_type){
    gw_subgrp_t curGroup=gateway_subdev_get_subgrpx(index,GROUP_TYPE_DEV);

    if(strcmp(dev_type, "OBJ_LIGHT") == 0){
        /*
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_OBJ_LIGHT,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_OBJ_LIGHT,index);
        aos_msleep(100);
        */
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_add_subdev(GROUP_OBJ_LIGHT,index);
        gateway_subgrp_add_subdev(GROUP_OBJ_LIGHT,index);
        gateway_subdev_set_subgrpx(index,GROUP_TYPE_DEV,GROUP_OBJ_LIGHT);
        return ;
    } else if(strcmp(dev_type, "OBJ_SCREEN") == 0){
        /*
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_del_subdev(curGroup,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_OBJ_SCREEN,index);
        aos_msleep(100);
        gateway_subgrp_add_subdev(GROUP_OBJ_SCREEN,index);
        aos_msleep(100);
        */
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_del_subdev(curGroup,index);
        gateway_subgrp_add_subdev(GROUP_OBJ_SCREEN,index);
        gateway_subgrp_add_subdev(GROUP_OBJ_SCREEN,index);
        gateway_subdev_set_subgrpx(index,GROUP_TYPE_DEV,GROUP_OBJ_SCREEN);
        return ;
    } else {
        // 遇到不支持的设备类型
        LOGD(TAG, "error, this is type is not support");
    }
}

static void gateway_timer_cb(void *_pub, void* obj){
    // 默认扫描子设备 5 秒钟，
    gateway_discover_stop();
    aos_timer_stop(&gateway_timer);
    jsapi_gateway_publish_provShowDev();
}

int bt_addr_val_to_str(const uint8_t addr[6], char *str, size_t len)                                            //tranlates addr to str
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

static void js_gateway_subdev_add(const char* mic){
    std::vector<aiot::scan_subdev_array>::iterator iter = aiot::js_scandev_array.begin();
    uint8_t                  dev_addr_str[20];

    for (; iter != aiot::js_scandev_array.end(); iter++){
        bt_addr_val_to_str((*iter).gw_info.protocol_info.ble_mesh_info.dev_addr, (char *)dev_addr_str, sizeof(dev_addr_str));

        LOGD(TAG, "{{{{{compare---%s--%s-----}}}}}",(char *)dev_addr_str, mic);
        if(strcmp((char *)dev_addr_str, mic) == 0) {
            if(gateway_subdev_add((*iter).gw_info) == 0){
                // 添加成功
                LOGD(TAG, "<<<<<<<<<<<<<< fun run suc >>>>>>>>>>>>>>>");
            }else{
                LOGD(TAG, "<<<<<<<<<<<<<<<add fail>>>>>>>>>>>>>>>");
            }
            return;
        }
	}
    LOGD(TAG, "not exist mic");
}

static void js_gateway_subdev_del(const char* mic){
    char _mac[20];
    memcpy(_mac, mic, sizeof(_mac));
    gateway_subdev_foreach(gw_del_subdev_traverse, _mac);
}

#if 0
static void js_gateway_subdev_set_onoff(const char* mic, uint8_t* onoff){    
    char kv_index[10];
    char kv_pos[128];

    for(int i = 1; i < 11; i++){
        snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if(ret > 0){
            char* kv_strs = strtok(kv_pos, "/");
            kv_strs = strtok(NULL, "/");
            LOGD(TAG, ">>>>>compare ---kv: %s---del: %s----<<<<<<<",kv_strs, mic);
            if(strcmp(mic, kv_strs) == 0){
                gateway_subdev_set_onoff(i,(*onoff));
                return;
            }
        }
    }
}

static void js_gateway_subdev_set_brightness(const char* mic, int gw_val){
    char kv_index[10];
    char kv_pos[128];
    for(int i = 1; i < 11; i++){
        snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if(ret > 0){

            char* kv_strs = strtok(kv_pos, "/");
            kv_strs = strtok(NULL, "/");
            LOGD(TAG, ">>>>>compare ---kv: %s---del: %s----<<<<<<<",kv_strs, mic);
            if(strcmp(mic, kv_strs) == 0){
                gateway_subdev_set_brightness(i, gw_val);
                return;
            }
        }
    }
}
#endif

gw_subgrp_t get_group_Index(const char* grpName){
    // for (i = 0; i < GW_MAX_GW_GROUP_NUM; i++) {
    //     snprintf(key_string, sizeof(key_string), "grp-name-00%d", i);
    //     LOGD(TAG, "key_string %s", key_string);
    //     memset(subgrp_name, 0, sizeof(subgrp_name)); 
    //     ret = aos_kv_get(key_string, subgrp_name, &buffer_len);
    //     if (ret != 0) {
    //
    //     }else{
    //         LOGD(TAG, "grpname1 %s", subgrp_name);
    //         LOGD(TAG, "grpname2 %s", grpName);
    //         if(strcmp(grpName, subgrp_name) == 0){
    //             LOGD(TAG, "find group is %d", i);
    //             return i;
    //         }
    //     }
    // }

    if(strcmp(grpName, "LIVING_ROOM") == 0){
        return 1;
    }else if(strcmp(grpName, "BED_ROOM") == 0){
        return 2;
    }else if(strcmp(grpName, "OBJ_LIGHT") == 0){
        return 3;
    }else if(strcmp(grpName, "OBJ_SCREEN") == 0){
        return 4;
    }
    return -1;
}

void JSgateWayImpl::init(JQuick::sp<JQPublishObject> pub){
    _pub = pub;
    if(!aos_timer_new_ext(&gateway_timer, gateway_timer_cb, this, times*1000 ,0, 0)){
        LOGD(TAG, "timer create success\r\n");}
    else{
        LOGD(TAG, "aos timer is error");
    }
}

int JSgateWayImpl::subDevMgmt(const char* gw_obj, const char* roomType, const char* dev_type, const char* macAdd){
    //根据mic地址 设备名称 所在房间 所属类型
    char kv_index[10];
    char kv_pos[64];
    char kv_pos_e[256];
    char nodeL[20];
    subdev_info_t subdev_info = { 0 };

    for(int i = 1; i < 11; i++){
        sprintf(kv_index, "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if(ret > 0){
            // 比较 mac 地址 确认index 号
            char* kv_strs = strtok(kv_pos, "/");
            kv_strs = strtok(NULL, "/");            
            if(strcmp(macAdd, kv_strs) == 0){
                ret = gateway_subdev_get_info(i, &subdev_info);
                // set name
                gateway_subdev_set_name(i,gw_obj);

                // add subdev to group
                _js_gateway_subgrp_set_roomType(i, roomType);
                _js_gateway_subgrp_set_dev_type(i, dev_type);

                snprintf(kv_pos_e, sizeof(kv_pos_e), "%s/%s/%s/%s/%s",kv_pos, kv_strs, gw_obj, roomType, dev_type);
                aos_kv_setstring(kv_index,kv_pos_e);
                snprintf(nodeL, sizeof(nodeL), "%02X:%02X:%02X:%02X:%02X:%02X", subdev_info.dev_addr[5], subdev_info.dev_addr[4], subdev_info.dev_addr[3],
                                                                            subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0]);
                return 0;
            }
        }else{
            continue;
        }
    }


    return 1;
}

// 该接口用来控制网关服务对mesh的操作，如扫描子设备、添加或删除子设备等。
int JSgateWayImpl::meshCtrl(const char* gw_opt, int gw_val, const char* mic){
    if(strcmp(gw_opt, "ACT_DEVSHOW") == 0){
        // 执行操作扫描子设备
        LOGD(TAG, "start scan sub dev");
        // uint32_t timeout = 5000;
        uint32_t timeout = GW_DEV_DISCOVER_NO_TIMEOUT;
        js_scandev_array.clear();
        gateway_discover_start(timeout);
        aos_timer_start(&gateway_timer);
    }else if(strcmp(gw_opt, "ACT_DEVADD") == 0){
        // 添加子设备
        js_gateway_subdev_add(mic);
    }else if(strcmp(gw_opt, "ACT_DEVDEL") == 0){
        // 删除子设备
        js_gateway_subdev_del(mic);
    }
    return -1;
}

// 该接口用来控制网关群控操作
int JSgateWayImpl::grpDevCtrl(const char* gw_opt, int gw_val, const char* grpName){
    gw_subgrp_t grpIndex;
    grpIndex = get_group_Index(grpName);
    LOGD(TAG, "opt is %s", gw_opt);
    LOGD(TAG, "opt grp index is %d", grpIndex);
    LOGD(TAG, "++++++++%d", gw_val);
    if(strcmp(gw_opt, GATEWAY_OPT_ACT_OPEN) == 0){
        gateway_subgrp_set_onoff(grpIndex, on);
        return 0;
    }else if(strcmp(gw_opt, GATEWAY_OPT_ACT_CLOSE) == 0){
        gateway_subgrp_set_onoff(grpIndex, off);
        return 0;
    }else if(strcmp(gw_opt, GATEWAY_OPT_ACT_BRIVALUE) == 0){
        LOGD(TAG, "opt grp index is %d", grpIndex);
        LOGD(TAG, "++++++++%d", gw_val);
        gateway_subgrp_set_brightness(grpIndex, gw_val);
        return 0;
    }
    return 1;
}

int JSgateWayImpl::subDevCtrl(const char* gw_obj, const char* gw_opt, int gw_val, const char* mic){

    char _mac[20];
    memcpy(_mac, mic, sizeof(_mac));
    if(strcmp(gw_opt, GATEWAY_OPT_ACT_OPEN) == 0){
        // 开灯
        gateway_subdev_foreach(gw_subdev_on, _mac);
        return 0;
    }else if(strcmp(gw_opt, GATEWAY_OPT_ACT_CLOSE) == 0){
        // 关灯
        gateway_subdev_foreach(gw_subdev_off, _mac);
        return 0;
    }else if(strcmp(gw_opt, GATEWAY_OPT_ACT_BRIVALUE) == 0){
        // 调亮度
        brightness_set config;
        config.mic = _mac;
        config.value = gw_val;
        gateway_subdev_foreach(gw_subdev_set_brightness, &config);

        return 0;
    }else if(strcmp(gw_opt, GATEWAY_OPT_ACT_CCTVALUE) == 0){
        return 0;
    }
    return 1;
}

void JSgateWayImpl::nodeListGet(){
    nodelist.clear();
    gateway_subdev_foreach(gw_subdev_traverse, NULL);
}
}  // namespace aiot