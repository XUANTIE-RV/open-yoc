<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="top-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
    </div>

    <div id="nor-id3" style="border-width: 1px; border-color: red"></div>
    <div id="box-id4" class="voice_box">
      <div id="div-id5" class="voice_baby_image" style="align-content: center">
        <image
          id="img-id6"
          class="voice_baby_icon"
          style="width: 250px; height: 250px"
          resize="contain"
          :src="
            require('../../../images/menu_icons/ic_setup_Khousekeeper_nor.png?base64')
          "
        />
        <text
          id="txt-id7"
          class="text"
          style="position: absolute"
          resize="contain"
          >{{ to_voice_text }}</text
        >
      </div>
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
      to_voice_text: "我是芯宝 ^_^ !!!",
    };
  },
  created() {
    this.events=[
      {
        module:"voice",
        event:"sessionBegin",
        callback:
          (result) => {
            console.log("voice_baby created:"+ "sessionBegin tag=" +result.tag +" score=" +result.score);
          },
        token:null
      },
      {
        module:"voice",
        event:"shortcutCMD",
        callback:
          (result) => {
            let obj = JSON.parse(result);
            var result = obj.payload.result;
            console.log("voice_baby created:"+ "shortcutCMD " +JSON.stringify(result));
            console.log("voice_baby created:"+ "shortcutCMD " +JSON.stringify(result.header));
            console.log("voice_baby created:"+ "shortcutCMD " +JSON.stringify(result.payload));
            console.log("voice_baby created:"+ "shortcutCMD " +JSON.stringify(result.payload.result));
            this.to_voice_text = ": " + result.payload.asrresult + " !!";
          },   
        token:null
      },
    ];
    $falcon.trigger("on_event",this.events);
  },
  beforeDestroy() {
    $falcon.trigger("off_event",this.events);
  },

  watch: {},

  computed: {},
  methods: {
    onBack() {
      this.$page.finish();
    },
    disMute() {
      voice.reqNLP();
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
  border-color: greenyellow;
  border-width: 1px;

  justify-content: center;
  align-items: center;
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
  