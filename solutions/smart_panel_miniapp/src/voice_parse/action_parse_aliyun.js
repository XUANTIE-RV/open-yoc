/*
1 阿里云，杭州天气
{

    "header":
    {
        "namespace":"VirtualAssistant",
        "name":"DialogResultGenerated",
        "status":20000000,
        "message_id":"165256a1f83a4afb8dce6def5ec65f7f",
        "task_id":"c43cb044cf40b9180027d0605412784c",
        "status_text":"Gateway:SUCCESS:Success."
    },

    "payload":
    {
        "action":"ask_weather",
        "semantics":
        [{
            "score":1,
            "slots":
            {
                "geo_level_3":
                [{
                    "tag":"ALI.CITY",
                    "score":0.0,
                    "offset":0,
                    "count":2,
                    "raw_value":"杭州",
                    "norm_value":"杭州"
                }]
            },
            "domain":"weather",
            "source":"jsgf",
            "intent":"get_weather"
        }],

        "spoken_text":"杭州市今天多云转阴，……年老体弱者宜着针织长袖衬衫、马甲和长裤。"
    }

}
*/
import aui_action from "./aui_action";

//文字识别的结果
let asrTable=[
    /*-----------------------------------------------子设备控制-----------------------------------------------*/
    //控制单个子设备:单个控制 
    ["打开客厅灯","开客厅灯","打开客厅登","开客厅登"],
    ["关闭客厅灯","关客厅灯","关闭客厅登","关客厅登"],
    ["打开卧室灯","开卧室灯","打开卧室登","打开卧式登","打开卧式灯","开卧室登","开卧式登","开卧式灯"],
    ["关闭卧室灯","关卧室灯","关闭卧室登","关闭卧式登","关闭卧式灯","关卧室登","关卧式登","关卧式灯"],
    ["开窗帘","打开窗帘","请打开窗帘"],
    ["关窗帘","关闭窗帘","请关闭窗帘"],

    //控制多个子设备:分组控制   
    ["开灯","打开灯","打开照明灯","请打开照明灯","请打开灯","请开灯"],
    ["关灯","关闭灯","关闭照明灯","请关闭照明灯","请关闭灯","请关灯"],

    //控制多个子设备：模式控制 
    ["离家模式","打开离家模式","设置离家模式"],
    ["睡眠模式","打开睡眠模式","设置睡眠模式"],
    ["电影模式","影音模式","打开影音模式","设置影音模式"],
    ["回家模式","打开回家模式","设置回家模式"],

    /*------------------------------------------------网关控制------------------------------------------------*/
    //音乐播放器
    ["下一首","下一首音乐"],
    ["上一首","上一首音乐"],
    ["暂停","暂停音乐","暂停播放"],
    ["继续播放"],
    ["停止","停止音乐","停止播放"],
    ["我要听歌"],
];
//asrAction不需要传递参数：一个命令对应一个接口函数（相比于多个命令一个接口，用空间（代码内存）换时间（执行效率））
let asrAction=[
    /*-----------------------------------------------子设备控制-----------------------------------------------*/
    //控制单个子设备
    aui_action.aui_action_open_living_room_light,
    aui_action.aui_action_close_living_room_light,
    aui_action.aui_action_open_bed_oom_light,
    aui_action.aui_action_close_bed_room_light,
    aui_action.aui_action_open_screen,
    aui_action.aui_action_close_screen,

    //控制多个子设备
    aui_action.aui_action_open_obj_light,
    aui_action.aui_action_close_obj_light,

    //控制多个子设备：模式控制
    aui_action.aui_action_open_leave_home,
    aui_action.aui_action_open_sleep_home,
    aui_action.aui_action_open_video_mode,
    aui_action.aui_action_open_back_home,

    /*-----------------------------------------------网关控制-----------------------------------------------*/
    //音乐播放器
    aui_action.aui_action_player_next,
    aui_action.aui_action_player_prev,
    aui_action.aui_action_player_pause,
    aui_action.aui_action_player_resume,
    aui_action.aui_action_player_stop,
    aui_action.aui_action_player_local_play,
];
//nlpAction需要传递参数：这也是云端需要分两次返回的原因
//第一次返回文字识别结果，第二次返回nlpAction的参数
let nlpAction={
    "play_music":aui_action.aui_action_player_online_play,//入参：在线url
    "tts":aui_action.aui_action_player_tts //入参：spoken_text
}
let exceptionMessage={
    "-1":"无法匹配到可处理的技能",
    "-2":"解析有错误",
    "-3":"网络异常",
    "-4":"意图不支持",//asr有识别结果，但在asrTable中没有匹配的命令词
    "-10":"其他错误",
}

let g_nlp_task_id="";

export default {
    exceptionMessage,

    //ret.value!=0 ，页面异常处理
    //ret.value==0，页面调用ret.action
    asr(obj){
        let ret ={
            value:0,
            action:null,
            para:null
        };
        let aui_result=obj.aui_result;  
        if(aui_result){
            console.log("aliyunASR:"+aui_result);
            if(aui_result===-101){
            ret.value=-3;
            return ret;
            }
            else{
            ret.value=-1;
            return ret;
            }
        }
        let name =obj.header.name;
        if(name){
            console.log("aliyunASR:"+name);
        }
        else{
            ret.value=-10;
            return ret;
        }
        //asr
        if(name==="RecognitionCompleted"){
            let cmd=obj.payload.result;
            if(cmd){
                console.log("aliyunASR:"+"asr_text=\""+cmd+"\"");
                let knownCmd=0;
                //遍历二维数组
                let i=0,j=0,len1=asrTable.length,len2=0;
                for(i=0;i<len1;i++){
                    len2=asrTable[i].length;
                    for(j=0;j<len2;j++){
                        if (asrTable[i][j] == cmd) {
                            knownCmd=1;
                            break;
                        }
                    }
                    if(knownCmd==1){
                        break;
                    }
                }
                //忽略后面的NLP处理
                if(knownCmd){
                    let task_id=obj.header.task_id;
                    if(task_id){
                    g_nlp_task_id=task_id;
                    }
                    ret.value=0; //唯一成功的路径
                    ret.action=asrAction[i];
                    return ret;
                }
                else{
                    ret.value=-4; //意图不支持
                    //fix me：当前对-4的异常处理用第一种方法
                    /*
                    asr有识别结果，但在asrTable中没有匹配的命令词，可以有三种异常处理
                    1 播一段云端的tts，不忽略第二次的nlp返回
                    2 页面播放自定义的提示音：
                      g_nlp_task_id=task_id;//忽略第二次的nlp返回，不播放tts
                      页面shortcutCMD的回调函数中增加一种异常处理
                      if(ret.value==-4){
                        smta.play("file:///mnt/XXX.mp3",1,1);
                      }
                    3 没有任何反应，没有tts也没有提示音
                      g_nlp_task_id=task_id;//没有tts
                      页面没有==-4的判断;//没有提示音
                    */ 
                    return ret;
                }

            }
            else{ 
                console.log("aliyunASR:"+"RecognitionCompleted but no cmd!");
                ret.value=-2; //解析有误
                return ret;
            }
        }
        else{
            ret.value=-1;
            return ret;
        }      
    },

    //nlp不做异常处理，仅有-2和0两种返回
    //ret.value==0，页面调用ret.action(ret.para)
    nlp(obj){

        let ret ={
            value:0,
            action:"",
            para:""
        };

        let task_id=obj.header.task_id;
        if(task_id===g_nlp_task_id){
            g_nlp_task_id="";
            ret.value=-2;
            return ret;
        }
        let action=obj.payload.action;
        let spoken_text=obj.payload.spoken_text;

        if(action){
            console.log("aliyunNLP:"+"nlp_action=\""+action+"\"");
        }
        
        //保留在线播放的功能
        //虽然有spoken_text="好的，请听我唱小鸭子"，但是既然已经在放歌了，就不必放tts了
        if(action&&action=="play_music"){
            let temp=obj.payload.action_params[0].name;
            let music_url=obj.payload.action_params[0].value;
            if(temp=="listenFile"&&music_url){
                console.log("aliyunNLP:"+"play_music url=\""+music_url+"\"");

                ret.value=0;
                ret.action=nlpAction[action];
                ret.para=music_url;
                return ret;
            }
            else{
                ret.value=-2;
                return ret;
            }
        }
        //页面播放tts时再判断是否静音
        else if(spoken_text){
            console.log("aliyunNLP:"+"spoken_text=\""+spoken_text+"\"");
            ret.value=0;
            ret.action=nlpAction["tts"]; //ask_weather等action可以归类为需要tts的action
            ret.para= spoken_text;
            return ret;
        }
        else{
            console.log("aliyunNLP:"+"好的！");
            ret.value=0;
            ret.action=nlpAction["tts"]; //ask_weather等action可以归类为需要tts的action
            ret.para= "好的";
            return ret;
        }
    },
    
}
  