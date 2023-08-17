/*
 * jsapi扩展mock
 */
import gw from "gateway";

//(事件，注册函数数组)
let eventFunc = {
  sessionBegin: [],
  sessionEnd: [],
  shortcutCMD: [],
  nlpBegin:[],
  nlpEnd:[],
};
//注册函数数组和记号数组一一对应
let eventToken = {
  sessionBegin: [],
  sessionEnd: [],
  shortcutCMD: [],
  nlpBegin:[],
  nlpEnd:[],
};

//(事件，返回值数组)
let eventRet = {

  sessionBegin:
  {
    tag: "nihaoxinbao",
    score: 65
  },

  sessionEnd:
  {

  },
  //debug=1 ，复制“打开客厅灯” 替换shortcutCMD中的result （ctrl+c，修改js，然后aiot-cli simulator ./）
  //asr测试： 打开客厅灯  开灯  离家模式  下一首
  shortcutCMD:"{\"header\":{\"namespace\":\"VirtualAssistant\",\"name\":\"RecognitionCompleted\",\"status\":20000000,\"message_id\":\"2372e565ddfb469792e1b9db3c4e0761\","+
  "\"task_id\":\"c43cb044cf4078c20c48497d91db6651\",\"status_text\":\"Gateway:SUCCESS:Success.\"},\"payload\":{\"result\":\"打开客厅灯\",\"confidence\":0.44200456142425537}}",
  nlpBegin:"",
  //debug=2
  //nlp测试： 我要听歌  （拦截的情况下 nlpEnd1被忽略）
  shortcutCMD1:"{\"header\":{\"namespace\":\"VirtualAssistant\",\"name\":\"RecognitionCompleted\",\"status\":20000000,\"message_id\":\"2372e565ddfb469792e1b9db3c4e0761\","+
  "\"task_id\":\"c43cb044cf4059a9ed6a45b60e8233f1\",\"status_text\":\"Gateway:SUCCESS:Success.\"},\"payload\":{\"result\":\"我要听歌\",\"confidence\":0.44200456142425537}}",
  nlpEnd1: "{\"header\":{\"namespace\":\"VirtualAssistant\",\"name\":\"DialogResultGenerated\",\"status\":20000000,\"message_id\":\"86c614e8bde34e48b84ea37ae2ed5b9d\","+
  "\"task_id\":\"c43cb044cf4059a9ed6a45b60e8233f1\",\"status_text\":\"Gateway:SUCCESS:Success.\"},\"payload\":{\"action\":\"play_music\","+
  "\"spoken_text\":\"好的，请听我唱邓文怡，邓力玮的我爱北京天安门。\","+
  "\"action_params\":[{\"name\":\"listenFile\",\"value\":\"http://other.player.ri01.sycdn.kuwo.cn/fec12da728c9d303ec50bb02c3f3714e/646dd297/resource/n2/1/49/1136432107.mp3\"}]}}",
  //debug=3
  //nlp测试： 杭州天气(语音合成)
  shortcutCMD2:"{\"header\":{\"namespace\":\"VirtualAssistant\",\"name\":\"RecognitionCompleted\",\"status\":20000000,\"message_id\":\"2372e565ddfb469792e1b9db3c4e0761\","+
  "\"task_id\":\"c43cb044cf40d4bbadd6918dd2c901a2\",\"status_text\":\"Gateway:SUCCESS:Success.\"},\"payload\":{\"result\":\"杭州天气\",\"confidence\":0.44200456142425537}}",
  nlpEnd2:"{\"header\":{\"namespace\":\"VirtualAssistant\",\"name\":\"DialogResultGenerated\",\"status\":20000000,\"message_id\":\"48a4d91cb2664a45be83a39c6528fd81\","+
  "\"task_id\":\"c43cb044cf40d4bbadd6918dd2c901a2\",\"status_text\":\"Gateway:SUCCESS:Success.\"},\"payload\":{\"action\":\"ask_weather\",\"semantics\":"+
  "[{\"score\":1,\"slots\":{\"geo_level_3\":[{\"tag\":\"ALI.CITY\",\"score\":0.0,\"offset\":0,\"count\":2,\"raw_value\":\"杭州\",\"norm_value\":\"杭州\"}]},\"domain\":\"weather\",\"source\":\"jsgf\",\"intent\":\"get_weather\"}],"+
  "\"spoken_text\":\"杭州市今天小雨转多云，15到20摄氏度，持续无风向微风。PM2.5指数25。建议着薄外套……仔衫裤等服装。年老体弱者应适当添加衣物，宜着夹克衫、薄毛衣等。\"}}",
  //debug=20
  //CTL_ACC_PLAYER_VOLUMN_SET  九十九
  shortcutCMD_lyasr:"[{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\" },\"payload\": {\"asrresult\":\"打开灯\",\"semantics\": [{\"domain\":\"NLU_DOMAIN_ACC\","+
  "\"intent\":\"CTL_ACC_SCENE\",\"slots\":{\"SLOT_ACTION\":[{\"value\": \"4\",\"offset\":\"0\",\"length\":\"6\"}]}}]}}]"
};
export default {
  on(event, callback) {

    let debug=20;

    if(debug){
      if(event=="shortcutCMD"){
        if(debug==1){
          if(eventRet.shortcutCMD.indexOf("关闭客厅灯")!=-1){
            $falcon.trigger("open_light1");
          }
          else if(eventRet.shortcutCMD.indexOf("关闭卧室灯")!=-1){
            $falcon.trigger("open_light2");
          }
          else if(eventRet.shortcutCMD.indexOf("关窗帘")!=-1){
            $falcon.trigger("open_screen");
          }
          else if(eventRet.shortcutCMD.indexOf("关灯")!=-1){
            let operations = {
              operate: "ACT_OPEN",
              value: 1,
            };
            gw.grpDevCtrl(operations, "OBJ_LIGHT");
          }
          else if(eventRet.shortcutCMD.indexOf("离家模式")!=-1){
            $falcon.trigger("open_light1");
            $falcon.trigger("open_light2");
            $falcon.trigger("open_screen");
          }
          else if(eventRet.shortcutCMD.indexOf("睡眠模式")!=-1){
            $falcon.trigger("open_light1");
            $falcon.trigger("open_light2");
            $falcon.trigger("open_screen");
          }
          else if(eventRet.shortcutCMD.indexOf("电影模式")!=-1){
            $falcon.trigger("open_screen");
          }
          setTimeout(() => { 
            eventFunc.shortcutCMD[0](eventRet.shortcutCMD);
          }, 2000);
        }
        else if(debug==2){
          setTimeout(() => { 
            eventFunc.shortcutCMD[0](eventRet.shortcutCMD1);
            setTimeout(()=>{
              eventFunc.nlpEnd[0](eventRet.nlpEnd1);
            },1000);
          }, 1000);
        }
        else if(debug==3){
          setTimeout(() => { 
            eventFunc.shortcutCMD[0](eventRet.shortcutCMD2);
            setTimeout(()=>{
              eventFunc.nlpEnd[0](eventRet.nlpEnd2);
            },1000);
          }, 1000);
        }
        //开关先亮后灭
        else if(debug==10){
          setTimeout(()=>{

            let operations = {
              operands: "switch1",
              operator: "ACT_OPEN",
              value: 1,
            };
            gw.subDevCtrl(operations, "22:22:22:22:22:22");
            setTimeout(()=>{

              let operations = {
                operands: "switch1",
                operator: "ACT_CLOSE",
                value: 1,
              };
              gw.subDevCtrl(operations, "22:22:22:22:22:22");
            },1000);
          },1000);
        }
        //测试package_lyeva_asr
        else if(debug==20){
          setTimeout(() => { 
            eventFunc.shortcutCMD[0](eventRet.shortcutCMD_lyasr);
          }, 2000);
        }
      }
    }

    console.log('mock voice.on' + " event:" + event);
    let token = 0;
    if (!eventFunc[event].includes(callback)) {
      eventFunc[event].push(callback);
      //根据callback生成token:1 函数自身有一个objectId 2函数的内容转为string 3根据绑定的时间生成token
      token = new Date().getTime()
      eventToken[event].push(token);
    }
    return token;
  },

  off(event, token) {
    console.log('mock voice.off' + " event:" + event);
    let index = eventToken[event].indexOf(token)
    if (index != -1) {
      eventFunc[event].splice(index, 1);
      eventToken[event].splice(index, 1)
    }
  },

  reqTTS(text) {
    console.log('mock voice.reqTTS');
  },

  setMute(state) {
    console.log('mock voice.setMute');
  },

  getMute() {
    console.log('mock voice.getMute');
  },

  pushToTalk() {
    console.log('mock voice.pushToTalk');
    for (let i = 0; i < eventFunc.sessionBegin.length; i++) {
      eventFunc.sessionBegin[i](eventRet.sessionBegin);
    }
  },

  stopTalk() {
    console.log('mock voice.stopTalk');
  }
}
