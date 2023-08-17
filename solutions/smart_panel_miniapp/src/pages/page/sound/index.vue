<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="top-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">声音</text>
    </div>

    <div id="div-id4"></div>

    <scroller
      id="scroller-id5"
      scroll-direction="vertical"
      class="sound-scroller"
    >
      <div
        id="div-id6"
        style="
          width: 100%;
          height: 78px;
          flex-direction: row;
          justify-content: center;
          align-items: center;
        "
      >
        <image
          id="img-id7"
          :src="require('../../../images/menu_icons/ic_music_mute.png')"
          style="width: 35px; height: 35px"
        />
        <fl-seekbar
          id="seekbar-id8"
          v-model="value"
          v-bind="seekbarCfg"
          @change="volumeChanged"
          @click="volumeChanged"
        />
        <image
          id="img-id9"
          :src="require('../../../images/menu_icons/ic_music_unmute.png')"
          style="width: 35px; height: 35px"
        />
        <text id="txt-id10" class="text" style="margin-left: 10px; width: 70px"
          >{{ ui_volume }}%</text
        >
      </div>

      <div id="div-id11" class="scroller-v-item" style="" @click="disMute">
        <text id="txt-id12" class="text" style="margin-left: 28px"
          >是否静音</text
        >
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
        <fl-switch
          id="switch-id14"
          v-model="value1"
          v-bind="switchCfg"
          style="position: absolute; left: 400px"
        />
      </div>
    </scroller>
  </div>
</template>
<script>
import { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch } from "falcon-ui";
import smta from "smta";
import kv from "system_kv";
import voice from "voice";
import fota from "fota";

export default {
  components: { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch },

  data() {
    return {
      events:null,
      bg: require("../../../images/state_icons/bg_light_gray.png"),
      seekbarCfg: {
        length: 250,
        min: 0,
        max: 100,
        activeColor: "#FFFFFF",
        disabledColor: "green",
        handleSize: "26px",
      },
      switchCfg: { activeColor: "#027AFF", width: "53px", height: "20px" },
      value1: false,
      ui_volume: "",
    };
  },
  created() {
    this.value = this.$page.loadOptions.volume;
    this.ui_volume = this.value;
    this.events=[
      {
        module:"smta",
        event:"mute",
        callback:
          function (mute) {
            if (mute) {
              smta.mute();
              kv.setItem("mute_state","true")
            } else {
              smta.unmute();
              kv.setItem("mute_state","false")
            }
            console.log("sound created:"+"mute=" + mute);
          }.bind(this),   
        token:null
      },
    ];
    $falcon.trigger("on_event",this.events);
    let res=kv.getItem('mute_state');
    if(res == "true"){
      this.value1 = true;
    }else{
      this.value1 = false;
    }
  },
  beforeDestroy() {
    $falcon.trigger("off_event",this.events);
  },
  watch: {
    value1() {
      if (this.value1) {
        smta.mute();
      } else {
        smta.unmute();
      }
    },
  },

  computed: {},
  methods: {
    onBack() {
      console.log("sound onBack");
      this.$page.finish();
      $falcon.trigger("soundChange", this.ui_volume);
    },

    playAudio2() {
      let url = "file:///mnt/sys_starting.mp3";
      let type = 1;
      console.log("sound playAudio2:"+url);
      smta.play(url, type);
    },

    // 更改音量 会触发的函数
    volumeChanged(e) {
      console.log("sound volumeChanged:"+e);
      this.ui_volume = e;
      if (e === 0) {
        smta.setVol(e);
        // this.ui_volume = "0";
      } else {
        this.ui_volume = e;
        smta.setVol(e);
      }
    },
  },
};
</script>
  
<style lang="less" scoped>
@import "base.less";
.network-scroller {
  width: 100%;
  height: 100%;
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
  