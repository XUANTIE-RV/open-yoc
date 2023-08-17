<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="top-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">亮度与息屏</text>
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
          id="id7"
          :src="require('../../../images/menu_icons/ic_brightness_low_nor.png')"
          style="width: 35px; height: 35px"
        />
        <fl-seekbar
          id="seekbar-id8"
          v-model="value"
          v-bind="seekbarCfg"
          @change="volumeChanged"
          @click="volumeChanged"
          style=""
        />
        <image
          id="img-id9"
          :src="
            require('../../../images/menu_icons/ic_brightness_high_nor.png')
          "
          style="width: 35px; height: 35px"
        />
        <text id="txt-id10" class="text" style="margin-left: 10px; width: 70px"
          >{{ screen_volume }}%</text
        >
      </div>

      <div id="div-id11" class="scroller-v-item" style="">
        <text id="txt-id12" class="text" style="margin-left: 28px"
          >是否开启自动息屏：</text
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
        <image
          id="img-id14"
          :src="
            isAutoOff
              ? require('../../../images/menu_icons/ic_chiose_press.png')
              : require('../../../images/menu_icons/ic_chiose_nor.png')
          "
          style="
            position: absolute;
            width: 35px;
            height: 35px;
            margin-left: 400px;
          "
          @click="changeOpen"
        />
      </div>
      <div id="div-id15" class="scroller-v-item" style="">
        <text id="txt-id16" class="text" style="margin-left: 28px"
          >自动息屏时间：{{ dis_time }}</text
        >
        <div
          id="div-id17"
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
          id="txt-id18"
          class="text"
          style="
            position: absolute;
            padding: 10px 10px 10px 10px;
            margin-left: 380px;
          "
          @click="showDialog"
          >{{ off_display }}秒</text
        >
      </div>
    </scroller>
    <my-dialog
      id="dialog-id19"
      v-model="off_display_show"
      title="自动息屏时间"
      center
      contentCenter
      :dialogStyle="{ width: '450', height: '300px' }"
      :showCancel="false"
      confirmText="确定"
    >
      <image
        id="img-id20"
        :src="require('../../../images/menu_icons/screen_display.png')"
        style="position: absolute; margin-left: 20px; width: 50px; height: 50px"
      />
      <fl-seekbar
        id="seekbar-id21"
        v-model="value1"
        v-bind="seekbarCfg1"
        @change="screenChange"
        useBlockStyle
        :length="340"
        style="margin-left: 70px"
      />
    </my-dialog>
  </div>
</template>
<script>
import { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlDialog } from "falcon-ui";
import MyDialog from "@/components/MyDialog.vue";
import smta from "smta";
import screen from "screen";
import kv from "system_kv";

var screenState = [""];

export default {
  components: { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlDialog, MyDialog },

  data() {
    return {
      bg: require("../../../images/state_icons/bg_light_gray.png"),
      seekbarCfg: {
        length: 250,
        min: 0,
        max: 100,
        activeColor: "#FFFFFF",
        disabledColor: "green",
        handleSize: "26px",
      },
      off_display_show: false,
      screen_volume: "",
      off_display: "",
      // 默认息屏时间
      value1: "",
      isAutoOff: false,
      seekbarCfg1: { min: 10, max: 300, blockWidth: "60px" },
    };
  },
  created() {
    this.value = this.$page.loadOptions.screen;
    this.screen_volume = this.value;

    screen.getInfo().then((res) => {
      console.log(res)
      this.value1 = res.autoOffTimeout;
      this.off_display = this.value1;
      this.isAutoOff = res.isAutoOff;
    });
  },
  methods: {
      onShow() {
      console.log("screen onShow:"+"updated");  
      screen.getInfo().then((res) => {
      this.value1 = res.autoOffTimeout;
      this.off_display = this.value1;
      this.isAutoOff = res.isAutoOff;
      });
    },
    onBack() {
      this.$page.finish();
      $falcon.trigger("screenChange", this.screen_volume);
    },
    volumeChanged(e) {
      screen.setBrightness(e);
      this.screen_volume = e;
    },
    showDialog(index) {
      this.off_display_show = true;
    },
    screenChange(e) {
      this.off_display = e;
      console.log("screen screenChange:"+"setAutoOffTimeout="+e+"s"); 
      screen.setAutoOffTimeout(e);
    },
    changeOpen() {
      this.isAutoOff = !this.isAutoOff;

      console.log("screen changeOpen:"+"setAutoOff " + this.isAutoOff); 
      if (this.isAutoOff) {
        screen.setAutoOff(true);
      } else {
        screen.setAutoOff(false);
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
  