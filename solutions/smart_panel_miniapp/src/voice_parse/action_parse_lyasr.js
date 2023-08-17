/*
1 lyasr，打开空调
{
    "header" :
    {
        "namespace" : "NluModel",
        "version" : "1.0.0"
    },

    "payload":
    {
        "asrresult" : "打开空调", 
        "semantics" :
        [ {
            "domain" : "NLU_DOMAIN_ACC", 
            "intent" : "CTL_ACC_POWER_CTRL", 
            "slots" : 
                {
                    "SLOT_ACTION" : 
                        [ {
                            "value" : "开", 
                            "offset" : "0", 
                            "length" : "6"
                        } ]
                }
        } ]
    }
}
*/

import aui_action from "./aui_action";

//fix me：nlu_intention_get_and_slots_fill训练好，用算法提供的宏替换c和小程序的宏
let asrTable=[
    //灯光
    "CTL_ACC_LIGHT_ON_OFF",
    "CTL_ACC_LIGHT_BRIGHTNESS_CHANGE",
    "CTL_ACC_LIGHT_BRIGHTNESS_SET",
    "CTL_ACC_LIGHT_COLOUR_CHANGE",
    "CTL_ACC_LIGHT_COLOUR_SET",
    "CTL_ACC_LIGHT_LIVING_ROOM",
    "CTL_ACC_LIGHT_BED_ROOM",
    //窗帘
    "CTL_ACC_SCREEN_ON_OFF",
    "CTL_ACC_SCREEN_GAUZE_ON_OFF",
    "CTL_ACC_SCREEN_CLOTH_ON_OFF",
    //空调
    "CTL_ACC_AC_ON_OFF",
    "CTL_ACC_AC_TEMP_CHANGE",
    "CTL_ACC_AC_TEMP_SET",
    "CTL_ACC_AC_WIND_CHANGE",
    "CTL_ACC_AC_WIND_SET",
    "CTL_ACC_AC_MOD",
    //风扇
    "CTL_ACC_FAN_ON_OFF",
    "CTL_ACC_FAN_CHANGE",
    "CTL_ACC_FAN_SHAKE",
    //电视
    "CTL_ACC_TV_ON_OFF",
    "CTL_ACC_TV_VOLUMN_CHANGE",
    "CTL_ACC_TV_MUTE_UNMUTE",
    //场景
    "CTL_ACC_SCENE",
    //服务
    "CTL_ACC_SERVICE_ALONE",
    "CTL_ACC_SERVICE_CLEAN",
    "CTL_ACC_SERVICE_WIFI",
    "CTL_ACC_SERVICE_TEMP",
    "CTL_ACC_SERVICE_WEATHER",
    "CTL_ACC_SERVICE_HELP",
    "CTL_ACC_SERVICE_WATER",
    "CTL_ACC_SERVICE_SLIPPER",
    "CTL_ACC_SERVICE_REPAIR",
    "CTL_ACC_SERVICE_LOCATION",
    "CTL_ACC_SERVICE_ROOM_NUMBER",
    "CTL_ACC_SERVICE_CHECK_OUT",
    "CTL_ACC_SERVICE_EXTENSION",
    //音箱
    "CTL_ACC_PLAYER_VOLUMN_CHANGE",
    "CTL_ACC_PLAYER_MUTE_UNMUTE",
    "CTL_ACC_PLAYER_PLAY_MUSIC",
    "CTL_ACC_PLAYER_PAUSE_RESUME",
    "CTL_ACC_PLAYER_MUSIC_CHANGE",
    "CTL_ACC_PLAYER_VOLUMN_SET",
];

let asrAction=[
    //灯光
    aui_action.aui_action_obj_light_onoff_ctrl,
    null,//网关没有这个控制接口，也没有获取亮度的接口
    aui_action.aui_action_obj_light_brightness_ctrl,
    null,
    null,
    aui_action.aui_action_living_room_light_ctrl,
    aui_action.aui_action_bed_room_light_ctrl,
    //窗帘
    aui_action.aui_action_screen_ctrl,
    null,
    null,
    //空调
    null,
    null,
    null,
    null,
    null,
    null,
    //风扇
    null,
    null,
    null,
    //电视
    null,
    null,
    null,
    //场景
    aui_action.aui_action_mode,
    //服务
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    //音箱
    aui_action.aui_action_player_volume_change,
    aui_action.aui_action_player_mute_ctrl,
    aui_action.aui_action_player_local_play,
    aui_action.aui_action_player_play_ctrl,
    aui_action.aui_action_player_switch_song,
    aui_action.aui_action_player_volumn_set,
];

export default {
    //返回0时，页面读取asr结果，调用action
    //返回非零值，页面做异常处理
     asr(obj){
        let ret ={
            value:0,
            action:null,
            para:null
        };
        let cmd=obj[0].payload.semantics[0].intent;
        if(cmd){
            console.log("aliyunASR:"+"asr_intent=\""+cmd+"\"");
            let knownCmd=0;
            //遍历一维数组
            let i=0,len=asrTable.length;
            for(i=0;i<len;i++){
                if(asrTable[i]==cmd){
                    knownCmd=1;
                    break;
                }
            }
            if(knownCmd){
                ret.value=0; //唯一成功的路径
                ret.action=asrAction[i];
                ret.para=obj[0].payload.semantics[0].slots.SLOT_ACTION[0].value;
                return ret;
            }
            else{
                ret.value=-4;//asr有识别结果，但在asrTable中没有匹配的命令词
                return ret;
            }
        }
        else{
            ret.value=-2;//解析有误，没有intent字段
            return ret;
        }
    },
}
  