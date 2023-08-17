<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="top-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">系统</text>
    </div>

    <div id="nor-id4" style="border-width: 1px; border-color: "></div>
    <div id="div-id5" class="voice_box" style="flex-direction: column">
      <div
        id="div-id6"
        class="info_box"
        style="
          width: 350px;
          height: 150px;
          flex-direction: column;
          bottom: 30px;
          border-width: 1px;
          border-color: ;
        "
      >
        <text id="txt-id7" class="larger_text" style="bottom: 10%">{{
          sys_name
        }}</text>
        <text id="txt-id8" class="text" style="top: 10%">{{
          sys_version
        }}</text>
      </div>

      <div
        id="div-id9"
        class="info_box"
        style="
          top: 50px;
          flex-direction: row;
          width: 350px;
          height: 100px;
          border-width: 1px;
          border-color: ;
        "
      >
        <text
          id="txt-id10"
          class="text"
          style="
            border-radius: 10px;
            border-width: 1px;
            border-color: white;
            padding: 15px 25px 15px 25px;
            right: 20%;
          "
          @click="check_fun1"
          >重启</text
        >
        <text
          id="txt-id11"
          class="text"
          style="
            border-radius: 10px;
            border-width: 1px;
            border-color: white;
            padding: 15px 25px 15px 25px;
            left: 20%;
          "
          @click="check_fun2"
          >重置</text
        >
      </div>
    </div>
  </div>
</template>
<script>
import { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch } from "falcon-ui";
import fota from "fota";
import kv from "system_kv";
import power from "power";

export default {
  components: { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch },

  data() {
    return {
      events:null,
      bg: require("../../../images/state_icons/bg_light_gray.png?base64"),
      sys_name: "Iot 小程序",
      sys_version: "V1.0.0",
      bt_1: "重启",
      bt_2: "恢复出厂设置",
    };
  },
  created() {
    this.events=[
      {
        module:"voice",
        event:"sessionBegin",
        callback:
          (result) => {
            console.log("sys created:"+ "sessionBegin tag=" +result.tag +" score=" +result.score);
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
            console.log("sys created:"+ "shortcutCMD " +JSON.stringify(result));
            console.log("sys created:"+ "shortcutCMD " +JSON.stringify(result.header));
            console.log("sys created:"+ "shortcutCMD " +JSON.stringify(result.payload));
            console.log("sys created:"+ "shortcutCMD " +JSON.stringify(result.payload.result));
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
    check_fun1() {
      power.reboot();
    },
    check_fun2() {
      kv.clear();
      kv.setItem("voice_state","on");
      power.reboot();
      console.log("sys check_fun2:"+ "reset set");
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

.info_box {
  justify-content: center;
  align-items: center;
}
</style>
  