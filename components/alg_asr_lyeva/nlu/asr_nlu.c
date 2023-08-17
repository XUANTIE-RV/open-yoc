/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include <board.h>
#include <aos/aos.h>

#if !(defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW)

#ifdef __cplusplus
extern "C" {
#endif

#include "lyeva/nlu.h"
#define ASR_GENE_NUM 6

#if defined(CONFIG_ALG_ASR_LYEVA_INTENTION) && CONFIG_ALG_ASR_LYEVA_INTENTION
//#include "lyeva/nlu_domain.h"会有很多重定义
//fix me：nlu_intention_get_and_slots_fill训练好，用算法提供的宏替换c和小程序的宏
//CTL_domain_type_intension
typedef enum{
    //灯光
    CTL_ACC_LIGHT_ON_OFF = 0,
    CTL_ACC_LIGHT_BRIGHTNESS_CHANGE,
    CTL_ACC_LIGHT_BRIGHTNESS_SET,
    CTL_ACC_LIGHT_COLOUR_CHANGE,
    CTL_ACC_LIGHT_COLOUR_SET,
    CTL_ACC_LIGHT_LIVING_ROOM,
    CTL_ACC_LIGHT_BED_ROOM,
    //窗帘
    CTL_ACC_SCREEN_ON_OFF,
    CTL_ACC_SCREEN_GAUZE_ON_OFF,
    CTL_ACC_SCREEN_CLOTH_ON_OFF,
    //空调
    CTL_ACC_AC_ON_OFF,
    CTL_ACC_AC_TEMP_CHANGE,
    CTL_ACC_AC_TEMP_SET,
    CTL_ACC_AC_WIND_CHANGE,
    CTL_ACC_AC_WIND_SET,
    CTL_ACC_AC_MOD,
    //风扇
    CTL_ACC_FAN_ON_OFF,
    CTL_ACC_FAN_CHANGE,
    CTL_ACC_FAN_SHAKE,
    //电视
    CTL_ACC_TV_ON_OFF,
    CTL_ACC_TV_VOLUMN_CHANGE,
    CTL_ACC_TV_MUTE_UNMUTE,
    //场景
    CTL_ACC_SCENE,
    //服务
    CTL_ACC_SERVICE_ALONE,
    CTL_ACC_SERVICE_CLEAN,
    CTL_ACC_SERVICE_WIFI,
    CTL_ACC_SERVICE_TEMP,
    CTL_ACC_SERVICE_WEATHER,
    CTL_ACC_SERVICE_HELP,
    CTL_ACC_SERVICE_WATER,
    CTL_ACC_SERVICE_SLIPPER,
    CTL_ACC_SERVICE_REPAIR,
    CTL_ACC_SERVICE_LOCATION,
    CTL_ACC_SERVICE_ROOM_NUMBER,
    CTL_ACC_SERVICE_CHECK_OUT,
    CTL_ACC_SERVICE_EXTENSION,
    //音箱
    CTL_ACC_PLAYER_VOLUMN_CHANGE,
    CTL_ACC_PLAYER_MUTE_UNMUTE,
    CTL_ACC_PLAYER_PLAY_MUSIC,
    CTL_ACC_PLAYER_PAUSE_RESUME,
    CTL_ACC_PLAYER_MUSIC_CHANGE,
    CTL_ACC_PLAYER_VOLUMN_SET,
    //其他
    CTL_ACC_OTHERS,
} acc_intention_en;
static const char *acc_intention_str[]={
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
    //其他
    "CTL_ACC_OTHERS",
};
typedef enum{
    NLU_DOMAIN_ACC = 0,
    NLU_DOMAIN_OTHERS,
} nlu_domain_en;
static const char *nlu_domain_str[]={
    "NLU_DOMAIN_ACC",
    "NLU_DOMAIN_OTHERS",
};
static const char* asrTable[][ASR_GENE_NUM]={
    //灯光
    {"打开灯","请开灯","开灯","把灯打开","打开照明","打开照明灯"},
    {"关灯","请关灯","睡觉了","关上灯","关闭照明","关闭照明灯"},

    {"亮度调低","调暗一点","暗一点","灯暗一点"},
    {"亮度调高","调亮一点","亮一点","灯亮一点"},

    {"亮度调到x","灯亮度x"},
    {"亮度调到最低","灯亮度调到最低","灯调到最暗"},
    {"亮度调到最高","灯亮度调到最高","灯调到最亮"},
    
    {"色温调高","灯光调暖","灯暖一些","灯暖一点"},
    {"色温调低","灯光调冷","灯冷一些","灯冷一点"},

    {"色温最高","白光"},
    {"色温最低","黄光"},
    {"色温x"},

    {"打开客厅灯","开客厅灯"},
    {"关闭客厅灯","关客厅灯","关闭客厅灯"},

    {"打开卧室灯","开卧室灯"},
    {"关闭卧室灯","关卧室灯","关闭卧室灯"},

    //窗帘
    {"开窗帘","打开窗帘","请打开窗帘"},
    {"关窗帘","关闭窗帘","请关闭窗帘"},

    //空调
    {"打开空调","请开空调","开空调"},
    {"关闭空调","请关闭空调","关空调"},

    {"调高一度","温度高一点","有点冷"},
    {"调低一度","温度低一点","有点热"},

    {"空调温度设为x度","空调温度x度","空调x度"},

    {"空调风速调高一点","空调风量调高一点","空调风调高一点","空调风调大一点","风大一些"},
    {"空调风速调低一点","空调风量调低一点","空调风调低一点","空调风调小一点","安静一些"},

    {"空调风速调到最高","空调风速调到中风","空调风速调到微风"},

    {"制冷模式","制冷","空调设为制冷模式","冷风"},
    {"制热模式","制热","空调设为制热模式","热风"},
    {"自动模式","自动","空调设为自动模式"},

    //风扇
    {"打开风扇","请开风扇","开风扇","把风扇打开","风扇启动"},
    {"关闭风扇","请关风扇","关上风扇","风扇停止"},

    {"将风扇调高","风量调高","风调大","大风"},
    {"将风扇调低","风量调低","风调小","小风"},

    {"风扇摇头","风扇摆头"},
    {"停止摇头","停止摆头"},

    //电视
    {"打开电视","请开电视","开电视","开机"},
    {"关闭电视","请关电视","关电视","关机"},

    {"调低电视音量","音量减","声音小一点"},
    {"调高电视音量","音量加","声音大一点"},

    {"电视静音","暂停"},

    //窗帘
    {"打开窗帘","请开窗帘","开窗帘","拉开窗帘"},
    {"关闭窗帘","请关窗帘","关窗帘","关上窗帘","拉上窗帘"},

    {"打开纱帘","请开纱帘","开纱帘","拉开纱帘"},
    {"关闭纱帘","请关纱帘","关纱帘","关上纱帘","拉上纱帘"},

    {"打开布帘","请开布帘","开布帘","拉开布帘"},
    {"关闭布帘","请关布帘","关布帘","关上布帘","拉上布帘"},

    //场景
    {"阅读模式","我要开始看书了","我要看会书"},
    {"睡眠模式","启动睡眠模式","打开睡眠模式","我要睡觉了","我想睡觉了"},
    {"起床模式","打开起床模式","启动起床模式","起床模式","我要起床了","早安"},
    {"淋浴模式","打开淋浴模式","启动淋浴模式","该洗白白了","我要洗澡了"},
    {"观影模式","启动观影模式","启动影院模式","我想看电影了","看电影了"},
    {"启动工作模式","打开工作模式","我要工作了","开始工作"},
    {"明亮模式","启动明亮模式","房间好暗啊","打开明亮模式"},
    {"离开模式","启动离家模式","打开离家模式","帮我看好家","离家模式","我要出去了"},
    {"欢迎模式","打开回家模式","启动回家模式","回家模式","我回来了"},
    {"浪漫模式","启动浪漫模式","我想要浪漫的客房","需要浪漫","浪漫一点"},
    {"温馨模式","打开温馨模式","启动温馨模式","我想要温馨的客房","温馨一点"},
    {"会客模式","启动会客模式","我要会客了","打开会客模式","有客人来了"},
    {"休闲模式","休闲模式","启动休闲模式","打开休闲模式","我要放松一下","我要喝茶"},
    {"柔和模式","灯光调柔和一些","柔和模式","灯光柔和一点"},
    {"免清洁模式","不要清洁","不需要清洁","不需要打扫","房间不要打扫","免清洁模式"},
    {"上课模式","上课场景","上课","板书模式","板书场景"},
    {"投影模式","投影场景","投影","多媒体模式","多媒体场景"},
    {"自习模式","自习场景","自习"},
    {"课间模式","课间场景","课间","下课"},
    {"放学模式","放学场景","放学"},
    {"恒照模式","恒照场景","恒照","自动调光","自动调灯"},
    {"喝茶模式","我要喝茶","喝茶场景","品茶场景","品茶模式"},
    {"等待模式","等待"},
    {"离家模式","打开离家模式","设置离家模式"},
    {"睡眠模式","打开睡眠模式","设置睡眠模式"},
    {"影音模式","打开影音模式","设置影音模式"},
    {"回家模式","打开回家模式","设置回家模式"},

    //服务
    {"勿扰模式","打开勿扰模式","不要打扰我"},

    {"请打扫","通知打扫卫生","打扫卫生","清理房间"},

    {"wifi密码是多少","无线密码","网络密码"},

    {"温度多少","室内温度"},

    {"天气怎样","报告下天气","天气如何"},

    {"呼叫前台","联系前台","联系总台","呼叫总台","通知前台"},

    {"送瓶水","送点水","纯净水","矿泉水"},

    {"送双拖鞋","拿双拖鞋","缺拖鞋","没有拖鞋"},

    {"需要维修","要报修","空调坏了","灯坏了","开关坏了","报修"},

    {"酒店地址","酒店的位置"},

    {"房间号是多少","房号多少","房号是多少","这是哪个房间"},

    {"退房","退掉房间"},

    {"续住","继续住"},

    //音箱
    {"增加音量","音量调高"},
    {"降低音量","音量调低"},

    {"静音"},
    {"取消静音"},

    {"我要听歌","播放歌曲","播放一首歌曲"},

    {"暂停播放"},
    {"继续播放"},

    {"上一首","播放上一首"},
    {"下一首","播放下一首"},

    {"音量调到x"},
};
//fix me :acc_intention_en补全后，换成枚举
static int asrIntension[]={
    //灯光
    CTL_ACC_LIGHT_ON_OFF,
    CTL_ACC_LIGHT_ON_OFF,

    CTL_ACC_LIGHT_BRIGHTNESS_CHANGE,
    CTL_ACC_LIGHT_BRIGHTNESS_CHANGE,

    CTL_ACC_LIGHT_BRIGHTNESS_SET,
    CTL_ACC_LIGHT_BRIGHTNESS_SET,
    CTL_ACC_LIGHT_BRIGHTNESS_SET,
    
    CTL_ACC_LIGHT_COLOUR_CHANGE,
    CTL_ACC_LIGHT_COLOUR_CHANGE,

    CTL_ACC_LIGHT_COLOUR_SET,
    CTL_ACC_LIGHT_COLOUR_SET,
    CTL_ACC_LIGHT_COLOUR_SET,

    CTL_ACC_LIGHT_LIVING_ROOM,
    CTL_ACC_LIGHT_LIVING_ROOM,

    CTL_ACC_LIGHT_BED_ROOM,
    CTL_ACC_LIGHT_BED_ROOM,

    //窗帘
    CTL_ACC_SCREEN_ON_OFF,
    CTL_ACC_SCREEN_ON_OFF,

    //空调
    CTL_ACC_AC_ON_OFF,
    CTL_ACC_AC_ON_OFF,

    CTL_ACC_AC_TEMP_CHANGE,
    CTL_ACC_AC_TEMP_CHANGE,

    CTL_ACC_AC_TEMP_SET,

    CTL_ACC_AC_WIND_CHANGE,
    CTL_ACC_AC_WIND_CHANGE,

    CTL_ACC_AC_WIND_SET,

    CTL_ACC_AC_MOD,
    CTL_ACC_AC_MOD,
    CTL_ACC_AC_MOD,

    //风扇
    CTL_ACC_FAN_ON_OFF,
    CTL_ACC_FAN_ON_OFF,

    CTL_ACC_FAN_CHANGE,
    CTL_ACC_FAN_CHANGE,

    CTL_ACC_FAN_SHAKE,
    CTL_ACC_FAN_SHAKE,

    //电视
    CTL_ACC_TV_ON_OFF,
    CTL_ACC_TV_ON_OFF,

    CTL_ACC_TV_VOLUMN_CHANGE,
    CTL_ACC_TV_VOLUMN_CHANGE,

    CTL_ACC_TV_MUTE_UNMUTE,

    //窗帘
    CTL_ACC_SCREEN_ON_OFF,
    CTL_ACC_SCREEN_ON_OFF,

    CTL_ACC_SCREEN_GAUZE_ON_OFF,
    CTL_ACC_SCREEN_GAUZE_ON_OFF,

    CTL_ACC_SCREEN_CLOTH_ON_OFF,
    CTL_ACC_SCREEN_CLOTH_ON_OFF,

    //场景
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,
    CTL_ACC_SCENE,

    //服务
    CTL_ACC_SERVICE_ALONE,

    CTL_ACC_SERVICE_CLEAN,

    CTL_ACC_SERVICE_WIFI,

    CTL_ACC_SERVICE_TEMP,

    CTL_ACC_SERVICE_WEATHER,

    CTL_ACC_SERVICE_HELP,

    CTL_ACC_SERVICE_WATER,

    CTL_ACC_SERVICE_SLIPPER,

    CTL_ACC_SERVICE_REPAIR,

    CTL_ACC_SERVICE_LOCATION,

    CTL_ACC_SERVICE_ROOM_NUMBER,

    CTL_ACC_SERVICE_CHECK_OUT,

    CTL_ACC_SERVICE_EXTENSION,

    //音箱
    CTL_ACC_PLAYER_VOLUMN_CHANGE,
    CTL_ACC_PLAYER_VOLUMN_CHANGE,

    CTL_ACC_PLAYER_MUTE_UNMUTE,
    CTL_ACC_PLAYER_MUTE_UNMUTE,

    CTL_ACC_PLAYER_PLAY_MUSIC,

    CTL_ACC_PLAYER_PAUSE_RESUME,
    CTL_ACC_PLAYER_PAUSE_RESUME,

    CTL_ACC_PLAYER_MUSIC_CHANGE,
    CTL_ACC_PLAYER_MUSIC_CHANGE,

    CTL_ACC_PLAYER_VOLUMN_SET,
};
static const char* asrPara[]={
    //灯光
    "on",
    "off",

    "low",
    "high",

    "x",
    "min",
    "max",
    
    "high",
    "low",

    "max",
    "min",
    "x",

    "on",
    "off",

    "on",
    "off",

    //窗帘
    "on",
    "off",

    //空调
    "on",
    "off",

    "high",
    "low",

    "x",

    "high",
    "low",

    "max",

    "cold",
    "hot",
    "auto",

    //风扇
    "on",
    "off",

    "high",
    "low",

    "on",
    "off",

    //电视
    "on",
    "off",

    "low",
    "high",

    "mute",

    //窗帘
    "on",
    "off",

    "on",
    "off",

    "on",
    "off",

    //场景
    "x",
    "2",
    "x",
    "x",
    "3",
    "x",
    "x",
    "1",
    "4",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "x",
    "1",
    "2",
    "3",
    "4",

    //服务
    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    "x",

    //音箱
    "high",
    "low",

    "mute",
    "unmute",

    "x",

    "pause",
    "resume",

    "prev",
    "next",

    "x",
};
static int asrTableLen=sizeof(asrTable)/sizeof(char*[ASR_GENE_NUM]);
#endif

int asr_nlu_init()
{
    return nlu_init();
}

int asr_nlu_deinit()
{
    return nlu_deinit();
}
#if defined(CONFIG_ALG_ASR_LYEVA_INTENTION) && CONFIG_ALG_ASR_LYEVA_INTENTION
//测试通过：asrTable所有泛化，除了空调一度 （数字“一”后面还有单位“度”）
//空掉调高一度(保留一)  vs  空调一度（一替换成x） 程序没法区分 
static int nlu_intention_get_by_look_up_table(const char *text, int *domain_lable, int *intention_lable, double *score, nlu_slots_result_t *slots)
{ 
    static char tempStr[100];
    static char para[10];
    //GB2312 2个字节，utf-8 3个字节
    static int codeLen=strlen("一");
    const char *temp=text;
    int i=0,j=0,beginIndex=-1,endIndex=-1,knownCmd=0;
    
    //“空调二十度”->“空调x度”
    //基础数字：0-9，权值：十，百，千……，测试通过：[0,111]
    while(*temp){
        if(strncmp(temp,"零",codeLen)==0||strncmp(temp,"一",codeLen)==0||strncmp(temp,"二",codeLen)==0||strncmp(temp,"三",codeLen)==0||strncmp(temp,"四",codeLen)==0||
           strncmp(temp,"五",codeLen)==0||strncmp(temp,"六",codeLen)==0||strncmp(temp,"七",codeLen)==0||strncmp(temp,"八",codeLen)==0||strncmp(temp,"九",codeLen)==0||
           strncmp(temp,"十",codeLen)==0||strncmp(temp,"百",codeLen)==0||strncmp(temp,"千",codeLen)==0 ){
            strncpy(para+j,temp,codeLen);
            j+=codeLen;
            beginIndex=i;
            break;
        }
        temp++;
        i++;
    }
    if(beginIndex!=-1){
        temp=text+beginIndex+codeLen;
        i=beginIndex+codeLen;
        while(strncmp(temp,"零",codeLen)==0||strncmp(temp,"一",codeLen)==0||strncmp(temp,"二",codeLen)==0||strncmp(temp,"三",codeLen)==0||strncmp(temp,"四",codeLen)==0||
              strncmp(temp,"五",codeLen)==0||strncmp(temp,"六",codeLen)==0||strncmp(temp,"七",codeLen)==0||strncmp(temp,"八",codeLen)==0||strncmp(temp,"九",codeLen)==0||
              strncmp(temp,"十",codeLen)==0||strncmp(temp,"百",codeLen)==0||strncmp(temp,"千",codeLen)==0 ){
                strncpy(para+j,temp,codeLen);
                j+=codeLen;
                temp+=codeLen;
                i+=codeLen;
            }
        endIndex=i;
    }

    para[j]='\0';
    //开灯->开灯   上一首->上一首（not 上x首） （不是asr末尾的数字“一”应该保留）
    if(beginIndex==-1||(strcmp(para,"一")==0&&endIndex!=strlen(text))){
        temp=text;
    }
    //音量调到20->音量调到x
    else{
        strcpy(tempStr,text);
        tempStr[beginIndex]='x';
        strcpy(tempStr+beginIndex+1,text+endIndex);
        temp=tempStr;
    }

    //遍历二维数组
    for(i=0;i<asrTableLen;i++){
        for(j=0;j<ASR_GENE_NUM;j++){
            if(asrTable[i][j]&&strcmp(asrTable[i][j],temp)==0){
                knownCmd=1;
                break;
            }
        }
        if(knownCmd==1){
            break;
        } 
    }
    if(knownCmd==1){
        *domain_lable=NLU_DOMAIN_ACC;
        *intention_lable=asrIntension[i];
        slots->size=1;
        slots->sub_slots[0].major_slot=SLOT_ACTION;
        if(beginIndex==-1||(strcmp(para,"一")==0&&endIndex!=strlen(text))){
            strcpy(slots->sub_slots[0].key_words,asrPara[i]);
        }
        else{
            strcpy(slots->sub_slots[0].key_words,para);
        }
    }
    else{
        *domain_lable=NLU_DOMAIN_ACC;
        *intention_lable=CTL_ACC_OTHERS;
    }
    return 0;
}
int nlu_intention_to_json_str_by_user_defined_micro(char *text, int domain_result, int intention_result, nlu_slots_result_t *slots_result, char *json_str,
                                                    int json_str_buffer_size)
{
    if (!text || strlen(text) == 0 || !json_str || !json_str_buffer_size)
    {
        return -1;
    }
    sprintf(json_str,"[{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\" },\"payload\": {\"asrresult\":\"%s\",\"semantics\": [{\"domain\":\"%s\","
                     "\"intent\":\"%s\",\"slots\":{\"SLOT_ACTION\":[{\"value\": \"%s\",\"offset\":\"0\",\"length\":\"0\"}]}}]}}]",
            text,nlu_domain_str[domain_result],acc_intention_str[intention_result],slots_result->sub_slots[0].key_words);
    return 0;
}
#endif

int asr_nlu_get(char *asr_result, char *nlu_reslut, size_t nlu_reslut_size)
{
    int    intenion_lable = 0;
    int    domain_lable   = 0;
    double score          = 0;

    nlu_slots_result_t slots_result = { 0x0 };
    int            ret;

#if defined(CONFIG_ALG_ASR_LYEVA_INTENTION) && CONFIG_ALG_ASR_LYEVA_INTENTION
    ret = nlu_intention_get_by_look_up_table(asr_result, &domain_lable, &intenion_lable, &score, &slots_result);
#else
    ret = nlu_intention_get_and_slots_fill(asr_result, &domain_lable, &intenion_lable, &score, &slots_result);
#endif
    
    if (ret) {
        return ret;
    }
#if defined(CONFIG_ALG_ASR_LYEVA_INTENTION) && CONFIG_ALG_ASR_LYEVA_INTENTION
    return nlu_intention_to_json_str_by_user_defined_micro(asr_result, domain_lable, intenion_lable, &slots_result, nlu_reslut,
                                                           nlu_reslut_size);
#else                                    
    return nlu_intention_slots_to_json_str(asr_result, domain_lable, intenion_lable, &slots_result, nlu_reslut,
                                           nlu_reslut_size);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
