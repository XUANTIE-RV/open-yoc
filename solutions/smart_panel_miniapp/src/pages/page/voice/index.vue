<template>
  <div id="bgr-id0" class="background" v-bind:style="{ backgroundImage: 'url(' + bg + ')' }">
    <div id="top-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">语音助手</text>
    </div>

    <div id="div-id4" style=""></div>
    <div id="box-id5" class="voice_box">
      <scroller id="scroller-id6" scroll-direction="vertical" class="sound-scroller">
        <div id="div-id7" class="scroller-v-item" style="" @click="disMute">
          <text id="txt-id8" class="text" style="margin-left: 28px">语音控制</text>
          <div
            id="div-id9"
            style="
              position: absolute;
              top: 70px;
              margin: 0 0 0 28px;
              width: 424px;
              height: 2px;
              background-color: wheat;
            "
          ></div>
          <fl-switch
            id="switch-id10"
            v-model="value1"
            v-bind="seekbarCfg"
            style="position: absolute; left: 400px"
          />
        </div>
        <div id="div-id11" class="scroller-v-item" style="">
          <text id="txt-id12" class="text" style="margin-left: 28px">默认唤醒词</text>
          <div
            id="div-id13"
            style="
              position: absolute;
              top: 70px;
              margin: 0 0 0 28px;
              width: 424px;
              height: 2px;
              background-color: wheat;
            "
          ></div>
          <text
            id="txt=id14"
            style="
              margin-left: 28px;
              font-size: 20px;
              color: #818182;
              position: absolute;
              left: 350px;
            "
            >你好芯宝</text
          >
        </div>
      </scroller>
    </div>
  </div>
</template>
<script>
import { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch } from "falcon-ui";
import fota from "fota";
import smta from "smta";
import kv from "system_kv";
import voice from "voice";

export default {
  components: { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch },

  data() {
    return {
      events:null,
      bg: require("../../../images/state_icons/bg_light_gray.png?base64"),
      value1: true,
      seekbarCfg: { activeColor: "#027AFF", width: "53px", height: "20px" },
    };
  },
  created() {
    let res=kv.getItem("voice_state");
    if(res == "on"){
      this.value1=true;
    }else{
      this.value1=false;
    }

    this.events=[
      {
        module:"voice",
        event:"sessionBegin",
        callback:
          (result) => {
            console.log("voice created:"+"sessionBegin tag="+ result.tag + " score=" + result.score);
          },   
        token:null
      },
      {
        module:"voice",
        event:"sessionEnd",
        callback:
          () => {
            // 退出语音交互动画界面
            console.log("voice created:"+"sessionEnd");
            //   this.$page.finish();
          },   
        token:null
      },
      {
        module:"voice",
        event:"shortcutCMD",
        callback:
          () => {
            console.log("voice created:"+"shortcutCMD");
          },   
        token:null
      },
      {
        module:"voice",
        event:"nlpBegin",
        callback:
          () => {
            console.log("voice created:"+"nlpBegin");
          },   
        token:null
      },
      {
        module:"voice",
        event:"nlpEnd",
        callback:
          (result) => {
            console.log("voice created:"+"nlpEnd");
          },   
        token:null
      },

    ];
    $falcon.trigger("on_event",this.events);
  },
  beforeDestroy() {
    $falcon.trigger("off_event",this.events);
    //没必要频繁读写内存，离开时写，进入时读
    if(this.value1)
      kv.setItem("voice_state","on");
    else
      kv.setItem("voice_state","off");
  },
  
  watch: {
    value1() {
      if (this.value1) {
        voice.setMute(0);
      } else {
        voice.setMute(1);
      }
    },
  },

  computed: {},
  methods: {
    onBack() {
      this.$page.finish();
    },
    disMute() {
      // voice.reqNLP();
    },
    setMute() {
      voice.setMute(0);
    },
  },
};
</script>
  
<style lang="less" scoped>
@import "base.less";
.voice_box {
  height: 450px;
  width: 480px;
  // border-color: greenyellow;
  border-width: 1px;

  // justify-content: center;
  // align-items: center;
}
.circle_box {
  height: 200px;
  width: 200px;
  justify-content: center;
  align-items: center;
  flex-direction: column;

  border-color: white;
  border-width: 5px;
  border-radius: 100px;
}
.info_box {
  justify-content: center;
  align-items: center;
}
.voice_btn {
  font-size: 24px;
}
#scroll-item() {
  // align-items: flex-start;
  // align-content: center;
  align-items: center;
  justify-content: flex-start;
  flex-direction: row;
  background-color: transparent;
  height: 80px;
}
.scroller-v-item {
  width: 100%;
  #scroll-item();
}
</style>
  