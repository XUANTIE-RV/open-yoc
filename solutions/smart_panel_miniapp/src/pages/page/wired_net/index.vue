<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="top-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">有线网络设置</text>
    </div>

    <div id="nor-id4" style="border-width: 1px; border-color: "></div>
    <div id="div-id5" class="voice_box" style="flex-direction: column">
      <div id="nor-id6" style="height: 50px"></div>
      <!-- <text class="text net_text" style="border-width: 1px; border-color:red ;" >I P地址:</text> -->
      <div id="nor-id7" style="flex-direction: row">
        <text
          id="txt-id8"
          class="text net_text"
          style="border-width: 1px; border-color: "
          >I P地址:
        </text>
        <div id="input_box-id9" class="input_box">
          <input
            id="input_id10"
            class="text textc"
            :value="value1"
            :placeholder="请输入ip地址"
            placeholderColor="#777"
            @input="ipaddInput"
            @confirm="onSubmit1"
            autofocus="true"
          />
        </div>
      </div>

      <!-- <text class="text net_text" style="border-width: 1px; border-color:red ;" >子网掩码:{{ net_mask }}</text> -->
      <div id="div-id11" style="flex-direction: row">
        <text
          id="txt-id12"
          class="text net_text"
          style="border-width: 1px; border-color: "
          >子网掩码:</text
        >
        <div id="input-id13" class="input_box">
          <input
            id="input-id14"
            class="text textc"
            :value="value2"
            :placeholder="net_mask"
            @input="ipaddInput"
            @confirm="onSubmit2"
            placeholderColor="#777"
            autofocus="true"
          />
        </div>
      </div>

      <!-- <text class="text net_text" style="border-width: 1px; border-color:red ;">网关地址:{{ net_gateway }}</text> -->
      <div id="div-id15" style="flex-direction: row">
        <text
          id="txt-id16"
          class="text net_text"
          style="border-width: 1px; border-color: "
          >网关地址:</text
        >
        <div id="div-id17" class="input_box">
          <input
            id="input-id18"
            class="text textc"
            :value="value3"
            :placeholder="net_gateway"
            @input="ipaddInput"
            @confirm="onSubmit3"
            placeholderColor="#777"
            autofocus="true"
          />
        </div>
      </div>
      <text
        class="text"
        style="
          position: absolute;
          border-width: 1px;
          border-color: aliceblue;
          border-radius: 5px;
          padding: 10px;
          top: 300px;
          left: 50px;
        "
        @click="onBack"
        >确 认</text
      >
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
      bg: require("../../../images/state_icons/bg_light_gray.png?base64"),
      net_ip: "192.168.0.100",
      net_mask: "xxx.xxx.xxx.xxx",
      net_gateway: "192.168.0.1",
      value1: "",
      value2: "",
      value3: "",
    };
  },
  created() {},

  watch: {},

  computed: {},
  mounted() {
    // this.$refs.textarea.getCursorPosition((r) => {
    //   console.log("wired_net mounted:"+"cursorPosition=" + r);
    // });
  },
  methods: {
    // ipaddInput: function (e) {
    //     console.log("wired_net ipaddInput:"+"input e=" + e+ " e.value="+e.value);
    //     this.value1 = e.value
    // },
    onSubmit1: function (e) {
      this.net_ip = e.value;
      console.log("wired_net onSubmit1:"+this.net_ip);
    },
    onSubmit2: function (e) {
      this.net_mask = e.value;
      console.log("wired_net onSubmit2:"+this.net_mask);
    },
    onSubmit3: function (e) {
      this.net_gateway = e.value;
      console.log("wired_net onSubmit3:"+this.net_gateway);
    },
    onBack() {
      this.$page.finish();
    },
  },
};
</script>
  
<style lang="less" scoped>
@import "base.less";

.textc {
  width: 200px;
  height: 50px;
}
.voice_box {
  height: 450px;
  width: 480px;
  flex-direction: column;
  // border-color: greenyellow;
  border-width: 1px;
}
.net_text {
  margin: 10px;
  text-align: left;
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
  