<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="div-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">场景模式</text>
    </div>
    <div
      id="div-id4"
      class="section_1"
      style="flex-direction: row; justify-content: space-between"
    >
      <div
        id="div-id5"
        class="div_box"
        @click="fun_1()"
      >
        <image
          id="img-id6"
          class="image"
          :src="require('../images/menu_icons/ic_scences_leave_home_nor.png')"
        />
        <text id="txt-id7" class="fun_text">离家模式</text>
      </div>
      <div
        id="div-id8"
        class="div_box"
        @click="fun_2()"
      >
        <image
          id="img-id9"
          class="image"
          :src="require('../images/menu_icons/ic_scences_sleep_nor.png')"
        />
        <text id="txt-id10" class="fun_text">睡眠模式</text>
      </div>
    </div>
    <div
      id="div-id11"
      class="section_2"
      style="flex-direction: row; justify-content: space-between"
    >
      <div
        id="div-id12"
        class="div_box"
        @click="fun_3()"
      >
        <image
          class="image"
          :src="require('../images/menu_icons/ic_scences_movie_nor.png')"
        />
        <text class="fun_text">影音模式</text>
      </div>
      <div
        id="div-id13"
        class="div_box"
        @click="fun_4()"
      >
        <image
          class="image"
          :src="require('../images/menu_icons/ic_scences_goto_home_nor.png')"
        />
        <text class="fun_text">回家模式</text>
      </div>
    </div>

    <fl-popup
      id="popup-id14"
      v-model="show"
      :overlay-transition="overlayTransition"
      :position="position"
      :transition="transition"
      :handleClose="handlePopupClose"
      @open="onPopupOpen"
      @close="onPopupClose"
    >
      <div id="div-id15" :class="popContentClass">
        <text class="text">{{ message }}</text>
      </div>
    </fl-popup>
  </div>
</template>
<script>
import gw from "gateway";
import { FlIcon, FlPopup } from "falcon-ui";
export default {
  name: "index",
  components: { FlIcon, FlPopup },
  data() {
    const scences_fun = ["离家模式", "睡眠模式", "影音模式", "回家模式"];

    return {
      token:null,
      bg: require("../images/state_icons/bg_light_gray.png"),
      // popup内容
      show: false,
      transition: false,
      overlayTransition: false,
      position: "center",
      popContentClass: "demo-popup-content",
      message: "执行成功",
    };
  },

  created() {
    this.token=$falcon.on("showPop_open", () => {
      this.message = "没有可控设备";
      this.showPop(true, true, 'center');
    })
  },
  beforeDestroy() {
    $falcon.off("showPop_open", this.token);
  },

  methods: {

    onBack() {
      this.$page.finish();
    },
    fun_1() {
      console.log("Scences fun_1:"+"jump_1");
      var flag = 0;
      gw.nodeListGet().then(async (props) => {
        for (var i in props) {
          if (props[i].dev_name === "light1" || props[i].dev_name === "light2" || props[i].dev_name === "screen") {
            flag = 1;
          }
        }

        if(flag){
          // flag = 1 代表有设备
          this.showPop(true, true, 'center');
          $falcon.trigger("open_leave_home");
        }else{
          // 0 代表没有设备
          this.message = "没有可控设备";
          this.showPop(true, true, 'center');
        }
      });
    },

    fun_2() {
      console.log("Scences fun_2:"+"jump_2");
      var flag = 0;
      gw.nodeListGet().then(async (props) => {
        for (var i in props) {
          if (props[i].dev_name === "light1" || props[i].dev_name === "light2" || props[i].dev_name === "screen") {
            flag = 1;
          }
        }

        if(flag){
          // flag = 1 代表有设备
          this.showPop(true, true, 'center');
          $falcon.trigger("open_sleep_home");
        }else{
          // 0 代表没有设备
          this.message = "没有可控设备";
          this.showPop(true, true, 'center');
        }
      });
    },

    fun_3() {
      console.log("Scences fun_3:"+"jump_3");
      var flag = 0;
      gw.nodeListGet().then(async (props) => {
        for (var i in props) {
          if (props[i].dev_name === "light1" || props[i].dev_name === "light2" || props[i].dev_name === "screen") {
            flag = 1;
          }
        }

        if(flag){
          // flag = 1 代表有设备
          this.showPop(true, true, 'center');
          $falcon.trigger("open_video_mode");
        }else{
          // 0 代表没有设备
          this.message = "没有可控设备";
          this.showPop(true, true, 'center');
        }
      });
    },

    fun_4() {
      console.log("Scences fun_4:"+"jump_4");
      var flag = 0;
      gw.nodeListGet().then(async (props) => {
        for (var i in props) {
          if (props[i].dev_name === "light1" || props[i].dev_name === "light2" || props[i].dev_name === "screen") {
            flag = 1;
          }
        }

        if(flag){
          // flag = 1 代表有设备
          this.showPop(true, true, 'center');
          $falcon.trigger("open_back_home");
        }else{
          // 0 代表没有设备
          this.message = "没有可控设备";
          this.showPop(true, true, 'center');
        }
      });
    },

    showPop(transition, overlayTransition, position) {
      this.transition = transition;
      this.overlayTransition = overlayTransition;
      this.position = position;
      this.show = true;

      this.popContentClass = "demo-popup-content";
      if (position === "left" || position === "right") {
        this.popContentClass += " demo-popup-content-h";
      } else if (position === "top" || position == "bottom") {
        this.popContentClass += " demo-popup-content-v";
      }
    },
    handlePopupClose() {
      return false;
    },
    onPopupOpen() {
      console.log("Scences onPopupOpen:"+"popup open event");
      this.timerId = setTimeout(() => {
        this.show = false;
      }, 1000);
    },
    onPopupClose() {
      clearTimeout(this.timerId);
      console.log("Scences onPopupClose:"+"popup close event");
      this.message = "执行成功"
    },
  },
};
</script>

<style lang="less" scoped>
@import "base.less";

.section_1 {
  width: 407px;
  height: 180px;
  margin: 23px 0 0 28px;
}

.div_box {
  background-color: @btn-background-color;
  width: 198px;
  height: 156px;
  border-radius: @border-radius-normal;
  align-items: center;
}

.image {
  width: 66px;
  height: 66px;
  margin-top: 20px;
}

.section_2 {
  width: 407px;
  height: 180px;
  margin: 23px 0 0 28px;
}

.fun_text {
  padding-top: 10px;
  color: #ffffff;
  font-size: 24px;
}

.demo-popup-content {
  width: 50%;
  height: 30%;
  align-items: center;
  justify-content: center;

  background-color: black;
  background-color: @card-background-color;
  border-radius: @border-radius-normal;
  padding: @padding-normal;
}
</style>
