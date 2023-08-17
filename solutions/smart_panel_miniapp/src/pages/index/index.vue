<template>
  <div
    id="div-0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
    style="flex-direction: column"
    @click="handleClick"
  >
    <slider
      id="slider-0"
      infinite="false"
      vertical="true"
      class="clock-slider"
      :index="rootItemIndex"
      previousMargin="0"
      nextMargin="0"
    >
      <div id="div-1" class="background" style="background-color: transparent">
        <pull-down />
      </div>

      <div id="div-2" class="background" style="background-color: transparent">
        <div id="div-3" style="position: absolute; width: 480px; height: 250px">
          <text
            id="text-0"
            style="
              width: 272px;
              height: 154px;
              margin: 2px 0 0 23px;
              font-size: 100px;
              color: white;
            "
            >{{ cur_time }}</text
          >
          <text
            id="text-1"
            class="text"
            style="position: absolute; margin: 140px 0 0 35px; font: bold"
            >{{ cur_date }}</text
          >
          <image
            id="image-0"
            :src="require('../../images/state_icons/icon_weather.png')"
            style="
              position: absolute;
              width: 84px;
              height: 84px;
              top: 30px;
              left: 370px;
            "
          />
          <text
            id="text-2"
            style="
              position: absolute;
              margin: 140px 0 0 380px;
              font-size: 26px;
              color: white;
              font: bold;
            "
            >{{ cur_temp }}</text
          >
        </div>

        <div
          id="div-4"
          class="voice_wizard"
          style="
            position: absolute;
            margin: 229px 0 199px 17px;
            width: 452px;
            height: 52px;
            flex-direction: row;
          "
        >
          <image
            id="text-1"
            :src="require('../../images/voice/ic_setup_Khousekeeper_nor.png')"
            style="width: 52px; height: 52px"
          />
          <text
            id="text-3"
            class="small_text"
            style="align-self: center; margin: 0 0 0 5px"
            >{{ cur_voice_wizard }}</text
          >
        </div>

        <div id="div-5" class="sw_button">
          <LightCard
            id="switch-0"
            v-model="value1"
            title="客厅灯"
            height="160px"
            width="128px"
          />
          <ScreenCard
            id="switch-1"
            v-model="value3"
            title="窗帘"
            height="160px"
            width="128px"
            style="margin-left: 18px"
          />
          <SwitchCard
            id="switch-2"
            v-model="value2"
            title="开关状态"
            height="160px"
            width="128px"
            style="margin-left: 18px"
            :disabled="true"
          />
        </div>
      </div>
    </slider>

    <fl-popup
      id="popup-0"
      ref="sys_await_popup"
      v-model="show_popup"
      :overlay-transition="overlayTransition"
      :position="position"
      :transition="transition"
      :handleClose="handlePopupClose"
      @open="onPopupOpen"
      @close="onPopupClose"
    >
      <div id="div-6" :class="popContentClass">
        <text id="text-4" class="text">{{ msg }}</text>
      </div>
    </fl-popup>
    <!-- <fl-popup
      id="sys_await_popup"
      v-model="sys_await"
      :overlay-transition="overlayTransition"
      :position="position"
      :transition="transition"
      :handleClose="handlePopupClose"
    >
      <div id="div-7" :class="popContentClass">
        <text id="text-8" class="text">初始化程序...</text>
      </div>
    </fl-popup> -->
  </div>
</template>

<script>
import LightCard from "../../components/LightCard.vue";
import SwitchCard from "../../components/SwitchCard.vue";
import ScreenCard from "../../components/ScreenCard.vue";
import Scences from "@/components/Scences.vue";
import SystemSet from "@/pages/page/systemSet.vue";
import musicPlay from "@/pages/page/music_play/index.vue";
import PullDown from "@/pages/page/pull-down.vue";
import { FlSlider, FlImage } from "falcon-ui";
import voice from "voice";
import smta from "smta";
import fota from "fota";
import wifi from "wifi";
import kv from "system_kv";
import power from "power";
import screen from "screen";
import http from "http";
import gw from "gateway";
import { FlPopup } from "falcon-ui";
import aliyun from "../../voice_parse/action_parse_aliyun";
import lyasr from "../../voice_parse/action_parse_lyasr";


var cnum = ["一", "二", "三", "四", "五", "六", "日"];
var voiceParse= 1;//0:aliyun ,1:lyasr

export default {
  name: "index",
  components: {
    FlPopup,
    LightCard,
    ScreenCard,
    SwitchCard,
    Scences,
    PullDown,
    SystemSet,
    FlSlider,
    FlImage,
    musicPlay,
    gw,
    screen,
  },
  data() {
      return {
        onToken:null,
        offToken:null,
        events:null,
        //以后要加需求：首页显示网络状态的图标
        wifiState:0,
        wifiStateBak:-1,
      };
  },
  created() {
    console.log("index created");
    this.$page.on("show", this.onPageShow);
    this.$page.on("hide", this.onPageHide);
    // 开机默认状态为未连接网络
    kv.setItem("wifi_state","disconnect");
    //所有的rom->ram都在这儿
    let res=kv.getItem("voice_state");
    if(res == "on"){
      voice.setMute(0);
    }else if(res == "off"){
      voice.setMute(1);
    }else if(res=="undefined"){
      voice.setMute(0);
      kv.setItem("voice_state","on");
    }

    // http请求，准备获得天气信息。
    // http.request({
    //     url: 'http://101.200.53.67:8080/service/iss?platform=&screen=&text=%E5%A4%A9%E6%B0%94%E6%80%8E%E4%B9%88%E6%A0%B7&appkey=zbxho64p3rtetxuwmcloxh32f5cpzdp3nrjbjyqx&filterName=search&ver=3.2&returnType=json&appsig=A6702357B7904B43907E7803665FBD5FE08C57A5&appver=1.0.1&history=&voiceid=8AAAD808EDF04697AF3C74C22DC4CF0E&scenario=child&gps=&method=iss.getTalk&dpi=&viewId=',
    //     method: 'GET',
    //     headers: {
    //     }
    //   }).then((res) => {
    //     console.log("index created:"+`http get then ${JSON.stringify(res)}`);
    //     this.result = 'httpGetResponse: ' + JSON.stringify(res);
    //   }).catch((err) => {
    //     console.log("index created:"+err+" "+`http get catch ${JSON.stringify(err)}`);
    //     this.result = 'httpGetError: ' + JSON.stringify(err);
    //   })
    
    /*
    用法：用于全局事件的注册和取消注册（$falcon 、jsapi modules） 
    1 定义events
    data() {return {  events:null,  }}

    2 定义事件数组（顺序无所谓），并在events中保存数组的地址
    //普通函数 vs 箭头函数：普通函数.bind(this)，函数内部this才能==vue ;箭头函数内部this=定义环境的this=vue
    //异步函数 vs 同步函数：调用async函数，是新建一个线程运行，所以可以奢侈地在那儿await
    created() {
      this.events=[
        //可以把数字、字符串、字符、对象等内容放在同一个数组，但还是建议用对象实现
        {
          module:"",
          event:"",
          callback: (异步)普通函数/ （异步）箭头函数 ,
          token:null
        },
        {
          module:"",
          event:"",
          callback:
          ,   
          token:null
        },
      ];
    }
    3 在created注册事件
    $falcon.trigger("on_event",this.events);

    4 在beforeDestroy取消注册事件
    $falcon.trigger("off_event",this.events);

    */
    
    this.onToken=$falcon.on("on_event",(e) =>{
      let events=e.data;
      for(let i=0;i<events.length;i++){
        switch(events[i].module){
          case "$falcon":
            events[i].token=$falcon.on(events[i].event,events[i].callback);
            break;
          case "voice":
            events[i].token=voice.on(events[i].event,events[i].callback);
            break;
          case "smta":
            events[i].token=smta.on(events[i].event,events[i].callback);
            break;
          case "fota":
            events[i].token=fota.on(events[i].event,events[i].callback);
            break;
          case "wifi":
            events[i].token=wifi.on(events[i].event,events[i].callback);
            break;
          case "kv":
            events[i].token=kv.on(events[i].event,events[i].callback);
            break;
          case "power":
            events[i].token=power.on(events[i].event,events[i].callback);
            break;
          case "screen":
            events[i].token=screen.on(events[i].event,events[i].callback);
            break;
          case "http":
            events[i].token=http.on(events[i].event,events[i].callback);
            break;
          case "gw":
            events[i].token=gw.on(events[i].event,events[i].callback);
            break;
          default:
            console.log("on_event:"+"no module "+events[i].module);
            break;
        }
      }
    });
    this.offToken=$falcon.on("off_event",(e) =>{
      let events=e.data;
      for(let i=0;i<events.length;i++){
        switch(events[i].module){
          case "$falcon":
            $falcon.off(events[i].event,events[i].token);
            break;
          case "voice":
            voice.off(events[i].event,events[i].token);
            break;
          case "smta":
            smta.off(events[i].event,events[i].token);
            break;
          case "fota":
            fota.off(events[i].event,events[i].token);
            break;
          case "wifi":
            wifi.off(events[i].event,events[i].token);
            break;
          case "kv":
            kv.off(events[i].event,events[i].token);
            break;
          case "power":
            power.off(events[i].event,events[i].token);
            break;
          case "screen":
            screen.off(events[i].event,events[i].token);
            break;
          case "http":
            http.off(events[i].event,events[i].token);
            break;
          case "gw":
            gw.off(events[i].event,events[i].token);
            break;
          default:
            console.log("global_event off:"+"no module "+events[i].module);
            break;
        }
      }
    });

    this.events=[
      //控制单个设备：没有显示该设备未上线
      {
        module:"$falcon",
        event:"set_light_value",
        callback:
          async (obj) => {
            console.log("index created:"+obj.data.title+" "+obj.data.count+" "+obj);
            if (obj.data.title === "客厅灯") {
              var operations = {
                operands: "light1",
                operator: "ACT_BRIVALUE",
                value: 1,
              };
              operations.value = obj.data.count;

              gw.nodeListGet().then(async (props) => {
                for (var i in props) {
                  console.log("index created:"+props[i]);
                  if (props[i].dev_name === "light1") {
                    gw.subDevCtrl(operations, props[i].dev_addr);
                    return;
                  }
                }
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
              });
            }
            if (obj.data.title === "卧室灯") {
              var operations = {
                operands: "light2",
                operator: "ACT_BRIVALUE",
                value: 1,
              };
              operations.value = obj.data.count;

              gw.nodeListGet().then(async (props) => {
                for (var i in props) {
                  console.log("index created:"+props[i]);
                  if (props[i].dev_name === "light2") {
                    gw.subDevCtrl(operations, props[i].dev_addr);
                    return;
                  }
                }
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
              });
            }
          } ,
        token:null
      },
      {
        module:"$falcon",
        event:"open_light1",
        callback:
          async () => {
            // 开灯
            var operations = {
              operands: "light1",
              operator: "ACT_OPEN",
              value: 1,
            };

            gw.nodeListGet().then((props) => {
              if (!props) {
                this.value1 = false;
                this.light1_status =0;
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
                return;
              }
              for (var i in props) {
                console.log("index created:"+props[i]);
                if (props[i].dev_name === "light1") {
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  return;
                }
              }
              this.value1 = false;
              this.light1_status =0;
              this.msg = "该设备未上线"
              this.showPop(false, false, "center");
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"close_light1",
        callback:
          async () => {
            var operations = {
              operands: "light1",
              operator: "ACT_CLOSE",
              value: 1,
            };
            gw.nodeListGet().then((props) => {
              if (!props) {
                this.value1 = false;
                this.light1_status =0;
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
                return;
              }
              for (var i in props) {
                console.log("index created:"+props[i]);
                if (props[i].dev_name === "light1") {
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  this.value1 = false;
                  this.light1_status = 0;
                  return;
                }
              }
              this.value1 = false;
              this.light1_status =0;
              this.msg = "该设备未上线"
              this.showPop(false, false, "center");
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"open_light2",
        callback:
          async () => {
            var operations = {
              operands: "light2",
              operator: "ACT_OPEN",
              value: 1,
            };
            gw.nodeListGet().then((props) => {
              if (!props) {
                this.value2 = false;
                this.light2_status =0;
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
                return;
              }
              for (var i in props) {
                console.log("index created:"+props[i]);
                if (props[i].dev_name === "light2") {
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  return;
                }
              }
              this.value2 = false;
              this.light2_status =0;
              this.msg = "该设备未上线"
              this.showPop(false, false, "center");
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"close_light2",
        callback:
          async () => {
            var operations = {
              operands: "light2",
              operator: "ACT_CLOSE",
              value: 1,
            };
            gw.nodeListGet().then((props) => {
              if (!props) {
                this.value2 = false;
                this.light2_status =0;
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
                return;
              }
              for (var i in props) {
                console.log("index created:"+props[i]);
                if (props[i].dev_name === "light2") {
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  this.value2 = false;
                  this.light2_status = 0;
                  return;
                }
              }
              this.value2 = false;
              this.light2_status =0;
              this.msg = "该设备未上线"
              this.showPop(false, false, "center");
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"open_screen",
        callback:
          async () => {
            var operations = {
              operands: "screen",
              operator: "ACT_OPEN",
              value: 1,
            };

            gw.nodeListGet().then((props) => {
              console.log("index created:"+props);
              if (!props) {
                this.value3 = false;
                this.screen_status = 0;
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
                return;
              }
              for (var i in props) {
                console.log("index created:"+props[i]);
                if (props[i].dev_name === "screen") {
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  return;
                }
              }
              this.value3 = false;
              this.screen_status = 0;
              this.msg = "该设备未上线"
              this.showPop(false, false, "center");
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"close_screen",
        callback:
          async () => {
            var operations = {
              operands: "screen",
              operator: "ACT_CLOSE",
              value: "1",
            };
            gw.nodeListGet().then((props) => {
              if (!props) {
                this.value3 = false;
                this.screen_status = 0;
                this.msg = "该设备未上线"
                this.showPop(false, false, "center");
                return;
              }
              for (var i in props) {
                console.log("index created:"+props[i]);
                if (props[i].dev_name === "screen") {
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  this.value3 = false;
                  this.screen_status = 0;
                  return;
                }
              }
              this.value3 = false;
              this.screen_status = 0;
              this.msg = "该设备未上线"
              this.showPop(false, false, "center");
            });
          },   
        token:null
      },
      //控制多个设备，一个都没有时，之前已经弹窗：没有可控设备
      //该函数至少可以控制一个设备，所以函数体内，不应该再有任何弹窗
      //一个都没有弹窗没有可控设备，有的话就控制有的那个
      {
        module:"$falcon",
        event:"open_leave_home",
        callback:
          async () => {
            var operations = {
              operate: "ACT_CLOSE",
              value: 1,
            };
            gw.grpDevCtrl(operations, "OBJ_LIGHT");
            
            var operations = {
              operate: "ACT_CLOSE",
              value: 1,
            };
            gw.grpDevCtrl(operations, "OBJ_SCREEN");

            this.light1_status = 0;
            this.light2_status = 0;
            this.screen_status = 0;
            this.value1 = false;
            this.value2 = false;
            this.value3 = false;
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"open_sleep_home",
        callback:
          async () => {
            var operations1 = {
              operate: "ACT_CLOSE",
              value: 1,
            };
            gw.grpDevCtrl(operations1, "OBJ_LIGHT");

            var operations = {
              operate: "ACT_CLOSE",
              value: 1,
            };
            gw.grpDevCtrl(operations, "OBJ_SCREEN");
            this.light1_status = 0;
            this.light2_status = 0;
            this.screen_status = 0;
            this.value1 = false
            this.value2 = false
            this.value3 = false
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"open_video_mode",
        callback:
          async () => {
            var operations = {
              operands: "",
              operator: "",
              value: 0,
            };
            var existFlag=[false,false,false];
            gw.nodeListGet().then((props) => {
              //至少有一个已入网设备，不会进入这里
              if (!props) {
                this.value1 = false
                this.value2 = false
                this.value3 = false
                this.light1_status = 0;
                this.light2_status = 0;
                this.screen_status = 0;
                console.log("index created:"+"something wrong!");
                return;
              }
              //有谁控谁：开灯调亮度关窗帘
              for (var i in props) {
                if (props[i].dev_name === "light1") {
                  operations.operands="light1";
                  operations.operator="ACT_OPEN";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  operations.operator="ACT_BRIVALUE";
                  operations.value=20;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  existFlag[0]=true;
                }
                else if (props[i].dev_name === "light2") {
                  operations.operands="light2";
                  operations.operator="ACT_OPEN";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  operations.operator="ACT_BRIVALUE";
                  operations.value=20;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  existFlag[1]=true;
                }
                else if (props[i].dev_name === "screen") {
                  operations.operands="screen";
                  operations.operator="ACT_CLOSE";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  this.value3 = false;
                  this.screen_status = 0;
                  existFlag[2]=true;
                }
              }
              //不在线显示为关闭
              if(!existFlag[0]){
                this.value1 = false;
                this.light1_status = 0;
              }
              if(!existFlag[1]){
                this.value2 = false;
                this.light2_status = 0;
              }
              if(!existFlag[2]){
                this.value3 = false;
                this.screen_status = 0;
              }
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"open_back_home",
        callback:
          async () => {
            var operations = {
              operands: "",
              operator: "",
              value: 0,
            };
            var existFlag=[false,false,false];
            gw.nodeListGet().then((props) => {
              //至少有一个已入网设备，不会进入这里
              if (!props) {
                this.value1 = false
                this.value2 = false
                this.value3 = false
                this.light1_status = 0;
                this.light2_status = 0;
                this.screen_status = 0;
                console.log("index created:"+"something wrong!");
                return;
              }
              //有谁控谁：开灯开窗帘
              for (var i in props) {
                if (props[i].dev_name === "light1") {
                  operations.operands="light1";
                  operations.operator="ACT_OPEN";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  existFlag[0]=true;
                }
                else if (props[i].dev_name === "light2") {
                  operations.operands="light2";
                  operations.operator="ACT_OPEN";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  existFlag[1]=true;
                }
                else if (props[i].dev_name === "screen") {
                  operations.operands="screen";
                  operations.operator="ACT_OPEN";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  existFlag[2]=true;
                }
              }
              //不在线显示为关闭
              if(!existFlag[0]){
                this.value1 = false;
                this.light1_status = 0;
              }
              if(!existFlag[1]){
                this.value2 = false;
                this.light2_status = 0;
              }
              if(!existFlag[2]){
                this.value3 = false;
                this.screen_status = 0;
              }
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"open_light_all",
        callback:
          async () => {
            var operations = {
              operands: "",
              operator: "",
              value: 0,
            };
            var existFlag=[false,false];
            gw.nodeListGet().then((props) => {
              //至少有一个已入网设备，不会进入这里
              if (!props) {
                this.value1 = false
                this.value2 = false
                this.light1_status = 0;
                this.light2_status = 0;
                console.log("index created:"+"something wrong!");
                return;
              }
              //有谁控谁：开灯调亮度关窗帘
              for (var i in props) {
                if (props[i].dev_name === "light1") {
                  operations.operands="light1";
                  operations.operator="ACT_OPEN";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  existFlag[0]=true;
                }
                else if (props[i].dev_name === "light2") {
                  operations.operands="light2";
                  operations.operator="ACT_OPEN";
                  operations.value=1;
                  gw.subDevCtrl(operations, props[i].dev_addr);
                  existFlag[1]=true;
                }
              }
              //不在线显示为关闭
              if(!existFlag[0]){
                this.value1 = false;
                this.light1_status = 0;
              }
              if(!existFlag[1]){
                this.value2 = false;
                this.light2_status = 0;
              }
            });
          },   
        token:null
      },
      {
        module:"$falcon",
        event:"close_light_all",
        callback:
          async () => {
            var operations = {
              operate: "ACT_CLOSE",
              value: 1,
            };
            gw.grpDevCtrl(operations, "OBJ_LIGHT");
            this.light1_status = 0;
            this.light2_status = 0;
            this.value1 = false;
            this.value2 = false;
          },   
        token:null
      },
      //jsapi
      {
        module:"wifi",
        event:"connected",
        callback:
          (event) => {
            console.log("network created:"+"success!" + event.data);
            kv.setItem("wifi_state","connect")
            kv.setItem("connected_info", event.ssid)
            $falcon.trigger("wifi_change","已连接");
            this.wifiState=1;
            if(this.wifiStateBak!=this.wifiState){
              this.wifiStateBak=this.wifiState;
            }
            //切换wifi时也要播放
            smta.play("file:///mnt/wifi_conn_succ.mp3" ,1);
          },   
        token:null
      },
      {
        module:"wifi",
        event:"disconnected",
        callback:
          (event) => {
            console.log("network created:"+"failure!" + event.data);
            kv.setItem("wifi_state","disconnect")
            $falcon.trigger("wifi_change","未连接");
            this.wifiState=0;
            if(this.wifiStateBak!=this.wifiState){
              this.wifiStateBak=this.wifiState;
              smta.play("file:///mnt/wifi_conn_fail.mp3" ,1);
            }
          },   
        token:null
      },
      {
        module:"gw",
        event:"jsapi_miniapp_init_finish",
        callback:
          () => {
            console.log("js space -=> main init finish <=-")
            this.show_popup = false;
          },   
        token:null
      },
      {
        module:"gw",
        event:"NodeStatusRpt",
        callback:
          (props) => {
            console.log("network created:"+props.dev_addr+" "+props.dev_name+" "+props.dev_state);
            if(props.dev_name=== "light1"){
              this.updateFlag1=true;
              if(props.dev_state){
                this.value1 = true;
                this.light1_status = 1;
              }
              else{
                this.value1 = false;
                this.light1_status =0;
              }
            }
            else if(props.dev_name=== "light2"){
              this.updateFlag2=true;
              if(props.dev_state){
                this.value2 = true;
                this.light2_status = 1;
              }
              else{
                this.value2 = false;
                this.light2_status =0;
              }
            }
            else if(props.dev_name=== "screen"){
              this.updateFlag3=true;
              if(props.dev_state){
                this.value3 = true;
                this.screen_status = 1;
              }
              else{
                this.value3 = false;
                this.screen_status = 0;
              }
            }
            else if(props.dev_name=== "switch1"){
              if(props.dev_state){
                this.value2 = true;
                this.light2_status = 1;
              }
              else{
                this.value2 = false;
                this.light2_status =0;
              }
            }
          },   
        token:null
      },
      {
        module:"voice",
        event:"sessionBegin",
        callback:
          (result) => {
            smta.play("file:///mnt/wakeup.wav",1);
          },   
        token:null
      },
      {
        module:"voice",
        event:"sessionEnd",
        callback:
          () => {  

          },   
        token:null
      },
      //asr + 异常处理
      {
        module:"voice",
        event:"shortcutCMD",
        callback:
          async (result) => {
            console.log("shortcutCMD:"+result);
            let obj = JSON.parse(result);
            let ret=null;

            //根据json自动区分：但是aliyun异常json中，没有特征字段表明是阿里云
            /*
            if(obj&&obj.header&&obj.header.namespace &&obj.header.namespace==="VirtualAssistant"){
              ret=aliyun.asr(obj);
            }
            else{
              console.log("shortcutCMD:"+"no matching cloud");
              return ;
            }
            */
            //定义全局变量手动区分
            if(voiceParse==0){
              ret=aliyun.asr(obj);
              /* 识别返回，云端断句 */
              if (ret.value == -1 || ret.value == -3) {
                  //云端识别错误，断句
                  voice.stopTalk();
              } else {
                  //识别完成，断句
                  let name = obj.header.name;
                  if (name) {
                      //-2识别完成，但没有payload.result；-4识别完成，但没有匹配的asr action
                      if (name ==="RecognitionCompleted") {
                          voice.stopTalk();
                      } else {
                          console.log("shortcutCMD:"+"unknown nlp error");
                      }
                  }
              }
              //（当前对-4的异常处理，播放一段云端的tts）
              if (ret.value == -1) {
                smta.play("file:///mnt/npl_nothing.mp3",1,1);   
              } else if (ret.value == -2) {
                smta.play("file:///mnt/npl_nothing.mp3",1,1); 
              } else if (ret.value == -3) {
                smta.play("file:///mnt/npl_nothing.mp3",1,1);   
              } else if (ret.value == -10) {
                smta.play("file:///mnt/npl_nothing.mp3",1,1);    
              } else {
                  ;
              }
            }
            else if(voiceParse==1){
              ret=lyasr.asr(obj);
              if (ret.value !=0) {
                smta.play("file:///mnt/npl_confused.mp3",1,1);   
              }

            }
            console.log("shortcutCMD:"+"ret ="+ret.value);

            //调用action 
            if(ret.value==0){
              let actionResult=await ret.action(ret.para);
              if(actionResult.value==-1){
                if(actionResult.msg=="该设备未上线"){
                  this.msg = "该设备未上线"
                  this.showPop(false, false, "center");
                }
                else if(actionResult.msg=="没有可控设备"){
                  this.msg = "没有可控设备"
                  this.showPop(false, false, "center");
                }
              }
            }
          },   
        token:null
      },
      {
        module:"voice",
        event:"nlpBegin",
        callback:
          async () => {
            console.log("index created:"+"nlpBegin");
          },   
        token:null
      },
      //nlp only
      {
        module:"voice",
        event:"nlpEnd",
        callback:
          async (result) => {
            console.log("nlpEnd:"+result);
            let obj = JSON.parse(result);
            let ret=null;
            //根据json自动区分：但是aliyun异常json中，没有特征字段表明是阿里云
            /*
            if(obj&&obj.header&&obj.header.namespace &&obj.header.namespace==="VirtualAssistant"){
              ret=aliyun.nlp(obj);
            }
            else{
              console.log("nlpEnd:"+"no matching cloud");
              return ;
            }
            */
            //定义全局变量手动区分
            if(voiceParse==0){
              ret=aliyun.nlp(obj);
            }
            else if(voiceParse==1){
              ret=lyasr.nlp(obj);
            }
            console.log("nlpEnd:"+"ret ="+ret.value);
            //调用action
            if(ret.value==0){
              let actionResult=await ret.action(ret.para);
              if(actionResult.value!=0){
                console.log("nlpEnd:"+"actionResult ="+actionResult);
              }
            }
          },   
        token:null
      },

    ];
    
    $falcon.trigger("on_event",this.events);


  },
  data() {
    return {
      message: "",
      rootItemIndex: 1,
      rootItem: 0,
      bg: require("../../images/state_icons/bg_light_gray.png"),
      voice_bg: require("../../images/voice/img_voice_default.png"),
      cur_voice_wizard: "你好芯宝......",
      cur_time: "12:30",
      cur_date: "10月11日 星期六",
      cur_temp: "16度",
      value1: false,
      value2: false,
      value3: false,
      light1_status: 0,
      light2_status: 0,
      screen_status: 0,
      result: "",
      timerId: 0,
      show_popup: false,
      sys_await: true,
      transition: false,
      overlayTransition: false,
      position: "center",
      popContentClass: "demo-popup-content",
      intervalId: null,
      count1: 0,
      count2: 0,
      // msg: "该设备未上线",
      msg: "    请等待...   ",
      updateFlag1:false,
      updateFlag2:false,
      updateFlag3:false,
      timer1:0,
      timer2:0,
      timer3:0,
    };
  },
  //value1，light1_status对应左侧的客厅灯
  //value3,screen_status对应中间的窗帘
  //value2，light2_status对应右边的开关
  //后续，value2和value3对调，light2_status改成switch_status ，底层增加obj_switch
  watch: {
    // 对灯1的状态进行检测
    value1() {
      if (this.value1) {
        if(this.light1_status){
          return;
        }
        console.log("index watch:"+"light1_" + this.value1);
        $falcon.trigger("open_light1");
        //设备入网但断电时收不到NodeStatusRpt
        this.updateFlag1=false;
        this.timer1=setTimeout(()=>{
          if(!this.updateFlag1){
            this.value1 = false;
            this.light1_status =0;
          }else{
            this.timer1=0;
          }

        },1000);
      } else {
        if(this.timer1) {
          clearTimeout(this.timer1);
          this.timer1=0;
        }
        if(!this.light1_status){
          return;
        }
        console.log("index watch:"+"light1_" + this.value1);
        $falcon.trigger("close_light1");
      }
    },
    /*
    value2() {
      if (this.value2) {
        // gw 1
        if(this.light2_status){
          return;
        }
        console.log("index watch:"+"light2_" + this.value2);
        $falcon.trigger("open_light2");
        //设备入网但断电时收不到NodeStatusRpt
        this.updateFlag2=false;
        this.timer2=setTimeout(()=>{
          if(!this.updateFlag2){
            this.value2 = false;
            this.light2_status =0;
          }
          else{
            this.timer2=0;
          }

        },1000);
      } else {
        if(this.timer2) {
          clearTimeout(this.timer2);
          this.timer2=0;
        }
        // gw 0
        if(!this.light2_status){
          return;
        }
        console.log("index watch:"+"light2_" + this.value2);
        $falcon.trigger("close_light2");
      }
    },
    */
    // 对 窗帘状态进行控制
    value3() {
      if (this.value3) {
        if(this.screen_status){
          return;
        }
        console.log("index watch:"+"screen_" + this.value3);
        $falcon.trigger("open_screen");
        //设备入网但断电时收不到NodeStatusRpt
        this.updateFlag3=false;
        this.timer3=setTimeout(()=>{
          if(!this.updateFlag3){
            this.value3 = false;
            this.screen_status =0;
          }
          else{
            this.timer3=0;
          }

        },5000);
      } else {
        if(this.timer3) {
          clearTimeout(this.timer3);
          this.timer3=0;
        }
        if(!this.screen_status){
          return;
        }
        console.log("index watch:"+"screen_" + this.value3);
        $falcon.trigger("close_screen");
      }
    },
  },
  methods: {
    handlePopupClose(){
       return true;//关不掉
    },
    showPop(transition, overlayTransition, position) {
      this.transition = transition;
      this.overlayTransition = overlayTransition;
      this.position = position;
      this.show_popup = true;

      this.popContentClass = "demo-popup-content";
      if (position === "left" || position === "right") {
        this.popContentClass += " demo-popup-content-h";
      } else if (position === "top" || position == "bottom") {
        this.popContentClass += " demo-popup-content-v";
      }

      this.timerId = setTimeout(() => {
        this.show_popup = false;
      }, 1000);
    },
    // showPop1(transition, overlayTransition, position) {
    //   this.transition = transition;
    //   this.overlayTransition = overlayTransition;
    //   this.position = position;
    //   this.sys_await = true;

    //   this.popContentClass = "demo-popup-content";
    //   if (position === "left" || position === "right") {
    //     this.popContentClass += " demo-popup-content-h";
    //   } else if (position === "top" || position == "bottom") {
    //     this.popContentClass += " demo-popup-content-v";
    //   }
    // },
    onPageShow() {
      console.log("index onPageShow");
      if (!this.timerId) {
        this.updateTime();
      }
    },
    onPageHide() {
      console.log("index onPageHide");
      if (this.timerId) {
        clearTimeout(this.timerId);
        this.timerId = 0;
      }
    },
    updateTime() {
      //测试疯狂点击首页按钮
      //console.log("------------------------------------------------------------"+gw.devList[0].isOpen+gw.devList[1].isOpen+gw.devList[2].isOpen)
      // do something
      const now = new Date();
      const hour = String(now.getHours());
      const minutes = String(now.getMinutes());

      this.cur_time = hour.padStart(2, "0") + ":" + minutes.padStart(2, "0");
      // const now = new Date();
      const month = String(now.getMonth() + 1);
      const day = String(now.getDate());
      const week = String(now.getDay());
      var weeks = "日";
      if(week == 0){
        weeks = "日";
      }else if(week == 1){
        weeks = "一"
      }else if(week == 2){
        weeks = "二"
      }else if(week == 3){
        weeks = "三"
      }else if(week == 4){
        weeks = "四"
      }else if(week == 5){
        weeks = "五"
      }else if(week == 6){
        weeks = "六"
      }

      this.cur_date = month.padStart(2, "0") +  "月" + day.padStart(2, "0") +  "日" + "  星期" + weeks;
      this.timerId = setTimeout(() => {
        this.updateTime();
      }, 1000);
    },
    cur_time_c() {
      const now = new Date();
      const hour = String(now.getHours());
      const minutes = String(now.getMinutes());

      this.cur_time = hour.padStart(2, "0") + ":" + minutes.padStart(2, "0");
    },
    cur_date_c() {
      const now = new Date();
      const month = String(now.getMonth() + 1);
      const day = String(now.getDate());
      const week = String(now.getDay());

      this.cur_date = month.padStart(2, "0") + "月" + day.padStart(2, "0") + "日" + "  星期" + week;
    },
    incrementCount1() {
      this.count1++;
      console.log("index incrementCount1:"+this.count1);
      if (this.count1 % 20 === 0) {
        $falcon.trigger("set_light1_l", this.count1);
      }
      if (this.count1 % 100 === 0) {
        this.count1 = 10;
      }
    },
    startCount1() {
      if (!this.value1) {
        this.value1 = true;
      }
      this.intervalId = setInterval(this.incrementCount1, 100);
    },
    stopCount1() {
      clearInterval(this.intervalId);
      this.intervalId = null;
    },
    incrementCount2() {
      this.count2++;
      if (this.count2 % 20 === 0) {
        $falcon.trigger("set_light2_l", this.count2);
      }
      if (this.count2 % 100 === 0) {
        this.count2 = 10;
      }
    },
    startCount2() {
      if (!this.value2) {
        this.value2 = true;
      }
      this.intervalId = setInterval(this.incrementCount2, 100);
    },
    stopCount2() {
      clearInterval(this.intervalId);
      this.intervalId = null;
    },
  },
  mounted() {
    this.show_popup = true;
    if (!this.timerId) {
      this.updateTime();
    }
  },
  beforeDestroy() {
    $falcon.trigger("off_event",this.events);
  },
  destroyed() {
    this.$page.off("show", this.onPageShow);
    this.$page.off("hide", this.onPageHide);

    $falcon.off("on_event", this.onToken);
    $falcon.off("off_event", this.offToken);
  },
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";

.wrapper {
  justify-content: center;
  align-items: center;
}

.greeting {
  text-align: center;
  margin-top: 20px;
  font-size: 50px;
  color: #41b883;
}
.jump-message {
  margin: 24px;
  font-size: 32px;
  color: #727272;
}
.message {
  font-size: 20px;
  color: #727272;
  margin-top: 12px;
}
.btn {
  .shadow();
}

.fl-doc-demo-block {
  &__title {
    margin: 0;
    padding: 32px 16px 16px;
    color: @text-color-default;
    font-size: 14px;
    line-height: 16px;
  }
}

.sw_button {
  position: absolute;
  top: 290px;
  width: 434px;
  height: 160px;

  flex-direction: row;
  margin-left: 28px;
  margin-bottom: 43px;
}
.clock-item-wrapper {
  width: 600px;
  height: 100%;
  padding: 0;
  transform: scale(0.8);
  .transition();
}
.clock-item-wrapper-selected {
  transform: scale(1);
}

@card-background-color: #343f50;
@radius-normal: 24px;
@space-normal: 24px;

.demo-popup-content {
  width: 50%;
  height: 30%;
  align-items: center;
  justify-content: center;

  background-color: @card-background-color;
  border-radius: @radius-normal;
  padding: @space-normal;
}

.demo-popup-content-h {
  height: 100%;
}

.demo-popup-content-v {
  width: 100%;
}
</style>
