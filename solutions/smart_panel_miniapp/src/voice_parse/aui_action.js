/*
1 定义接口的两种方式
同步函数：return promise (resolve value/reject Error)                     （简化：不reject Error）
异步函数：return value（封装成功的promise) /throw Error(封装失败的promise)  （简化：不throw Error）

2 调用接口的两种方式
同步等待 try{await}catch(error){}                                         （简化：不用catch）
异步回调 接口函数().then.catch                                             （简化：不用catch）

建议一般使用异步回调，必须等待返回值时用同步等待。

*/
import gw from "gateway";
import voice from "voice";
import smta from "smta";

let ret={
    value:0,
    msg:""
};

//任何时候：我要听歌，播放file:///mnt/alibaba1.mp3 cur_num=0
//没有放歌时，下一首，上一首，播放file:///mnt/alibaba1.mp3  cur_num=0
//已经在放歌时，下一首，上一首，基于在放的歌曲进行切换        cur_num++/-- 
//切歌时，应该调用stop，不然有线程在等resume
let urlList = [
    "file:///mnt/alibaba1.mp3",
    "file:///mnt/alibaba2.mp3",
    "file:///mnt/alibaba3.mp3",
];
let cur_num=0;

//fix me :nodeListGet如果能返回devtype，就不需要在页面判断了
let subdevGrpMem={
    OBJ_LIGHT:["light1","light2"],
    OBJ_SCREEN:["screen"],
};

let subdevMode={
    //离家：关灯关窗帘  
    leave_home:[
        {
            operands: "light1",
            operator: "ACT_CLOSE",
            value: 1,
        },
        {
            operands: "light2",
            operator: "ACT_CLOSE",
            value: 1,
        },
        {
            operands: "screen",
            operator: "ACT_CLOSE",
            value: 1,
        },
    ],
    //睡眠：关灯关窗帘  
    sleep_home:[
        {
            operands: "light1",
            operator: "ACT_CLOSE",
            value: 1,
        },
        {
            operands: "light2",
            operator: "ACT_CLOSE",
            value: 1,
        },
        {
            operands: "screen",
            operator: "ACT_CLOSE",
            value: 1,
        },
    ],
    //影音：开灯调亮度关窗帘  
    video_mode:[
        {
            operands: "light1",
            operator: "ACT_OPEN",
            value: 1,
        },
        {
            operands: "light1",
            operator: "ACT_BRIVALUE",
            value: 20,
        },
        {
            operands: "light2",
            operator: "ACT_OPEN",
            value: 1,
        },
        {
            operands: "light2",
            operator: "ACT_BRIVALUE",
            value: 20,
        },
        {
            operands: "screen",
            operator: "ACT_CLOSE",
            value: 1,
        },
    ],
    //回家：开灯开窗帘 
    back_home:[
        {
            operands: "light1",
            operator: "ACT_OPEN",
            value: 1,
        },
        {
            operands: "light2",
            operator: "ACT_OPEN",
            value: 1,
        },
        {
            operands: "screen",
            operator: "ACT_OPEN",
            value: 1,
        },
    ],
}

/*-----------------------------------------------子设备控制-----------------------------------------------*/
//控制单个子设备：devname是subdev_mgt设置的参数
async function subdev_open(devname){
    console.log("subdev_open: "+devname);
    //检查设备是否入网
    //fix me：检查设备是否上电
    let props=await gw.nodeListGet();
    for (let i in props) {
        if (props[i].dev_name === devname) {
            let operations = {
                operands: devname,
                operator: "ACT_OPEN",
                value: 1,
            };
            gw.subDevCtrl(operations, props[i].dev_addr);
            ret.value=0;
            return ret;
        }
    }
    ret.value=-1;
    ret.msg="该设备未上线";
    return ret;
};

async function subdev_close(devname){
    console.log("subdev_close: "+devname);
    //检查设备是否入网
    //fix me：检查设备是否上电
    let props=await gw.nodeListGet();
    for (let i in props) {
        if (props[i].dev_name === devname) {
            let operations = {
                operands: devname,
                operator: "ACT_CLOSE",
                value: 1,
            };
            gw.subDevCtrl(operations, props[i].dev_addr);
            ret.value=0;
            return ret;
        }
    }
    ret.value=-1;
    ret.msg="该设备未上线";
    return ret;
};
//控制多个子设备：roomtype、devtype是subdev_mgt设置的参数
async function subdev_room_open(roomtype){

}
async function subdev_room_close(roomtype){

}

async function subdev_grp_open(devtype){
    console.log("subdev_grp_open: "+devtype);
    //检查组里是否存在入网的设备
    //fix me：检查设备是否上电
    let props=await gw.nodeListGet();
    let flag=0;
    for (let i in props) {
        if(subdevGrpMem[devtype].includes(props[i].dev_name)){
            flag=1;
            break;
        }
    }
    if(flag){
        let operations = {
            operate: "ACT_OPEN",
            value: 1,
          };
        gw.grpDevCtrl(operations, devtype);
        ret.value=0;
        return ret;
    }
    else{
        ret.value=-1;
        ret.msg="没有可控设备";
        return ret;
    }
};
async function subdev_grp_close(devtype){
    console.log("subdev_grp_close:"+devtype);
    //检查组里是否存在入网的设备
    //fix me：检查设备是否上电
    let props=await gw.nodeListGet();
    let flag=0;
    for (let i in props) {
        if(subdevGrpMem[devtype].includes(props[i].dev_name)){
            flag=1;
            break;
        }
    }
    if(flag){
        let operations = {
            operate: "ACT_CLOSE",
            value: 1,
          };
        gw.grpDevCtrl(operations, devtype);
        ret.value=0;
        return ret;
    }
    else{
        ret.value=-1;
        ret.msg="没有可控设备";
        return ret;
    }

};
async function aui_action_obj_light_brightness_ctrl(brightness){
    console.log("aui_action_obj_light_brightness_ctrl: "+brightness);
    //检查组里是否存在入网的设备
    //fix me：检查设备是否上电
    let props=await gw.nodeListGet();
    let flag=0;
    for (let i in props) {
        if(subdevGrpMem["OBJ_LIGHT"].includes(props[i].dev_name)){
            flag=1;
            break;
        }
    }
    if(flag){
        let operations = {
            operate: "ACT_BRIVALUE",
            value: brightness,
          };
        gw.grpDevCtrl(operations, "OBJ_LIGHT");
        ret.value=0;
        return ret;
    }
    else{
        ret.value=-1;
        ret.msg="没有可控设备";
        return ret;
    }
};
//控制多个子设备：模式控制
async function subdev_mode(mode){
    console.log("subdev_mode: "+mode);
    let flag=0;
    let props=await gw.nodeListGet();
    for(let i=0;i<subdevMode[mode].length;i++){
        for (let j in props){
            if (props[j].dev_name === subdevMode[mode][i].operands) {  
                gw.subDevCtrl(subdevMode[mode][i], props[j].dev_addr);
                flag=1;
                break;
            }
        }
    }
    if(flag){   
        ret.value=0;
        return ret;
    }
    else{
        ret.value=-1;
        ret.msg="没有可控设备";
        return ret;
    }
};
/*-----------------------------------------------网关控制-----------------------------------------------*/
//音乐播放器
async function aui_action_player_next(){
    let status=smta.getStatus();
    let next_num=0;
    if(status==3){
        let url = smta.getUrl();
        //必须根据url来计算cur_num
        //否则的话就要想办法把音乐页面的cur_num再同步到首页
        //首页的cur_num就是个temp变量
        if(url==="file:///mnt/alibaba1.mp3"){
            next_num=1;
        }
        else if(url==="file:///mnt/alibaba2.mp3"){
            next_num=2;
        }
        else if(url==="file:///mnt/alibaba3.mp3"){
            next_num=0;
        }
    }
    else{
        next_num=0;
    }
    cur_num=next_num;
    await smta.stop();
    console.log("aui_action_player_next:"+"play_url="+urlList[cur_num]);
    await smta.play(urlList[cur_num],0);
    ret.value=0;
    return ret;
};
async function aui_action_player_prev(){
    let status=smta.getStatus();
    let prev_num=0;
    if(status==3){
        let url = smta.getUrl();
        //必须根据url来计算cur_num
        //否则的话就要想办法把音乐页面的cur_num再同步到首页
        //首页的cur_num就是个temp变量
        if(url==="file:///mnt/alibaba1.mp3"){
            prev_num=2;
        }
        else if(url==="file:///mnt/alibaba2.mp3"){
            prev_num=0;
        }
        else if(url==="file:///mnt/alibaba3.mp3"){
            prev_num=1;
        }
    }
    else{
        prev_num=0;
    }
    cur_num=prev_num;
    await smta.stop();
    console.log("aui_action_player_prev:"+"play_url="+urlList[cur_num]);
    await smta.play(urlList[cur_num],0);
    ret.value=0;
    return ret;
};
async function aui_action_player_pause(){
    console.log("aui_action_player_pause");
    let status=smta.getStatus();
    if(status==0){
        await smta.stop();
    }
    else{
        await smta.resumeEnable(0);
    }
    voice.reqTTS("好的");//smtaudio_start(MEDIA_SYSTEM, data, 0, 1);
    ret.value=0;
    return ret;
};
async function aui_action_player_resume(){
    console.log("aui_action_player_resume");
    let status=smta.getStatus();
    if(status==3){
        await smta.resumeEnable(1);
        let configs=await smta.resume();
        if(congig==-1){
            await smta.play(urlList[cur_num], 0);
        }
    }
    else{
        await smta.play(urlList[cur_num], 0);
    }
    ret.value=0;
    return ret;
};
//asr action :传递 cmd 
async function aui_action_player_stop(){
    await smta.stop();
    $falcon.trigger("audio_stop");
    ret.value=0;
    return ret;
};
//播放本地歌曲时：asr action，传递cmd
async function aui_action_player_local_play(){
    await smta.stop();
    console.log("aui_action_player_local_play:"+"play_url="+"file:///mnt/alibaba1.mp3");
    await smta.play("file:///mnt/alibaba1.mp3",0);
    cur_num=0;
    ret.value=0;
    return ret;
};
//播放在线歌曲时：nlp action, 传递 cmd +para
async function aui_action_player_online_play(para){
    if(!para){
        ret.value=-1;
        ret.msg="no url"
        return ret;
    }
    await smta.stop();
    console.log("aui_action_player_online_play:"+"play_url="+para);
    await smta.play(para,0);
    ret.value=0;
    return ret;
};
async function aui_action_player_tts(para){
    if(!para){
        ret.value=-1;
        ret.msg="no text"
        return ret;
    }
    console.log("aui_action_player_tts:"+"tts_text="+para);
    voice.reqTTS(para);
    ret.value=0;
    return ret;
};

let NumMap={
    零:0,
    一:1,
    二:2,
    三:3,
    四:4,
    五:5,
    六:6,
    七:7,
    八:8,
    九:9,
}
//千百十个：测试通过[0,111]
let bit=[0,0,0,0];
function getNum(str){
    let ret=0;
    //三十->三十零
    if(str.endsWith("十")){
        str=str.concat("零");
    }
    else if(str.endsWith("百")){
        str=str.concat("零零");
    }
    else if(str.endsWith("千")){
        str=str.concat("零零零");
    }
    //十->一十
    if(str.startsWith("十")){
        let tempStr="一";
        tempStr=tempStr.concat(str);
        str=tempStr;
    }
    let len=str.length;
    let j=bit.length-1;
    for(let i=len-1;i>=0;i--){
        if(NumMap[str[i]]!=undefined){
            bit[j--]=NumMap[str[i]];
        }
    };
    len=bit.length;
    for(let i=0;i<len;i++){
        ret=ret*10+bit[i];

    }
    return ret;
}


//fix me： ret 可以只是整数，0成功，-1-2-3代表不同的异常值；但是随着接口增多，异常值越来越多，不如增加message字段
//返回0 :操作成功
//返回-1：操作失败 ; 弹窗/界面显示失败原因。
//发出控制命令后返回（页面注册事件） vs 等到结果之后再返回 (aui_action.js注册事件)
export default {
    //asrAction不需要传递参数
    /*-----------------------------------------------子设备控制-----------------------------------------------*/
    //控制单个子设备
    async aui_action_open_living_room_light(){
        let result=await subdev_open("light1");
        return result;
    },
    async aui_action_close_living_room_light(){
        let result=await subdev_close("light1");
        return result;
    },
    async aui_action_living_room_light_ctrl(para){
        let result=null;
        if(para=="on"){
            result=await subdev_open("light1");
        }
        else{
            result=await subdev_close("light1");
        }
        return result;
    },
    async aui_action_open_bed_oom_light(){
        let result=await subdev_open("light2");
        return result;
    },
    async aui_action_close_bed_room_light(){
        let result=await subdev_close("light2");
        return result;
    },
    async aui_action_bed_room_light_ctrl(para){
        let result=null;
        if(para=="on"){
            result=await subdev_open("light2");
        }
        else{
            result=await subdev_close("light2");
        }
        return result;
    },
    async aui_action_open_screen(){
        let result=await subdev_open("screen");
        return result;
    },
    async aui_action_close_screen(){
        let result=await subdev_close("screen");
        return result;
    },
    async aui_action_screen_ctrl(para){
        let result=null;
        if(para=="on"){
            result=await subdev_open("screen");
        }
        else{
            result=await subdev_close("screen");
        }
        return result;
    },

    //控制多个子设备
    async aui_action_open_obj_light(){
        let result=await subdev_grp_open("OBJ_LIGHT");
        return result;
    },
    async aui_action_close_obj_light(){
        let result=await subdev_grp_close("OBJ_LIGHT");
        return result;
    },

    async aui_action_obj_light_onoff_ctrl(para){
        let result=null;
        if(para=="on"){
            result=await subdev_grp_open("OBJ_LIGHT");
        }
        else{
            result=await subdev_grp_close("OBJ_LIGHT");
        }
        return result;
    },

    aui_action_obj_light_brightness_ctrl,


    //控制多个子设备：模式控制
    async aui_action_open_leave_home(){
        let result=await subdev_mode("leave_home");
        return result;
    },
    async aui_action_open_sleep_home(){
        let result=await subdev_mode("sleep_home");
        return result;
    },
    async aui_action_open_video_mode(){
        let result=await subdev_mode("video_mode");
        return result;
    },
    async aui_action_open_back_home(){
        let result=await subdev_mode("back_home");
        return result;
    },
    async aui_action_mode(para){
        let result=null;
        switch(para){
            case "1":
              result=await subdev_mode("leave_home");
              break;
            case "2":
              result=await subdev_mode("sleep_home");
              break;
            case "3":
              result=await subdev_mode("video_mode");
              break;
            case "4":
              result=await subdev_mode("back_home");
              break;
            default:
              console.log("aui_action_mode:"+"no mode "+para);
              break;
          }
        return result;
    },

    /*-----------------------------------------------网关控制-----------------------------------------------*/
    //音乐播放器
    aui_action_player_next,
    aui_action_player_prev,
    aui_action_player_pause,
    aui_action_player_resume,
    aui_action_player_stop,
    aui_action_player_local_play,

    //nlpAction需要传递参数：这也是云端需要分两次返回的原因
    //第一次返回文字识别结果，第二次返回nlpAction的参数
    aui_action_player_online_play,//入参：在线url
    aui_action_player_tts, //入参：spoken_text

    //asr action :传递 cmd 
    async  aui_action_player_volume_change(para){
        let volumn=smta.getVol();
        if(para=="low"){
            volumn-=10;
            if(volumn<0){
                volumn=0;
            }
        }
        else{
            volumn+=10;
            if(volumn>100){
                volumn=100;
            } 
        }
        await smta.setVol(volumn);
        console.log("aui_action_player_volume_change:"+"new volumn="+volumn);
        ret.value=0;
        return ret;
    },
    async  aui_action_player_volumn_set(para){
        let volumn=getNum(para);
        if(volumn>100){
            volumn=100;
        }
        await smta.setVol(volumn);
        console.log("aui_action_player_volumn_set:"+"new volumn="+volumn);
        ret.value=0;
        return ret;
    },
    async  aui_action_player_mute_ctrl(para){
        if(para=="mute"){
            await smta.mute();
        }
        else{
            await smta.unmute();
        }
        console.log("aui_action_player_mute_ctrl:"+para);
        ret.value=0;
        return ret;
    },
    async  aui_action_player_play_ctrl(para){
        if(para=="pause"){
            //底层应该允许重复触发暂停
            await smta.pause();
        }
        else{
            await smta.resume();
        }
        console.log("aui_action_player_play_ctrl:"+para);
        ret.value=0;
        return ret;
    },
    async  aui_action_player_switch_song(para){
        let result=null;
        if(para=="prev"){
            result=await aui_action_player_prev();
        }
        else{
            result=await aui_action_player_next();
        }
        console.log("aui_action_player_switch_song:"+para);
        ret.value=0;
        return ret;
    },

}
  