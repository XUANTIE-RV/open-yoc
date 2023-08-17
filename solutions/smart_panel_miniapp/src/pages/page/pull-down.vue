<template>
    <div id="pull_down_div-id0" class="background">

      <div id="pull_down_div-id1" class="pull-down">
        <scroller scroll-direction="vertical" show-scrollbar="false" scrollWithAnimatio="false" scrollable="false" class="network-scroller">
        <!-- <div class="flex-self fun_unit" style="width: 424px; height: 130px; border-width: 1px; border-color: red; margin-bottom: 17px;">


        </div> -->

        <div id="pull_down_div-id2" class="flex-self" style="width: 424px; height: 130px; margin-bottom: 17px; flex-direction: row;">

          <div id="pull_down_div-id3" class="fun_unit" @click="show_image_play" style="height: 100%; width: 277px; border-radius: 8px; border-color: white; margin-right: 17px;">
            <image style="width: 105px; height: 106px; position: absolute; top: 12px; left: 15px;" :src="require('../../images/music_icons/music_cricle.png?base64')" />
            <image id="music_image" class="wave" style="width: 105px; height: 40px; position: absolute; top: 20px; left: 150px;" :src="require('../../images/music_icons/musicala.png?base64')" />
            <text id="txt-id4" style="width: 140px; height: 33px; color: #FFFFFF; font-size: 24px; position: absolute; top: 78px; left: 137px; overflow: auto ;">{{ image_name }}</text>
          </div>
          <div id="pull_down_div-id4" class="fun_unit" style="height: 100%; width: 130px; border-radius: 8px; border-color: white;" @click= "toSet" >
            <image id="set_image1" class="icon_image" :src="require('../../images/menu_icons/ic_drop_down_menu_setup_nor.png?base64')" />
            <text id="txt-id3" class="small_text">设置</text>
          </div>

        </div>

        <div id="pull_down_div-id5" class="flex-self" style="width: 424px; height: 130px; margin-bottom: 17px; flex-direction: row;">
          <div id="pull_down_div-id6" class="fun_unit" style="height: 100%; width: 130px; border-radius: 8px; border-color: white; margin-right: 17px;" @click="onClickNavtoSence">
            <image id="scene_image" style="width: 64px; height: 64px;" class="icon_image" :src="require('../../images/menu_icons/changjing.png?base64')" />
            <text id="txt-id2" class="small_text">场景选择</text>
          </div>
          <div id="pull_down_div-id7" class="fun_unit" style="height: 100%; width: 130px; border-radius: 8px; border-color: white; margin-right: 17px;" @click="onClickNavtoDevmng">
            <image id="mage_image" class="icon_image" :src="require('../../images/menu_icons/device.png?base64')" />
            <text id="txt-id1" class="small_text">设备管理</text>
          </div>
          <div id="pull_down_div-id8" class="fun_unit" style="height: 100%; width: 130px; border-radius: 8px; border-color: white;" @click="onClickP2t">
            <image id="voice_image" class="icon_image" :src="require('../../images/menu_icons/ic_setup_Khousekeeper_nor.png?base64')" />
            <text id="txt-id0" class="small_text">语音</text>
          </div>
        </div>

        <!-- 这里如果想添加控制功能直接打开 <div class="flex-self" style="width: 424px; height: 130px; border-width: 1px; border-color: ; flex-direction: row;"  @click="onClickNavtoDevmng_ctrl">
          <div style="height: 100%; width: 130px; border-radius: 8px; border-width: 1px; border-color: white; margin-right: 17px;">
            <image class="icon_image" :src="require('../../images/menu_icons/ic_my_host_host_offline_nor@2x.png?base64')" />
            <text class="small_text">设备控制</text>
          </div>
          <div style="height: 100%; width: 130px; border-radius: 8px; border-width: 1px; border-color: white; margin-right: 17px;"></div>
          <div style="height: 100%; width: 130px; border-radius: 8px; border-width: 1px; border-color: white; "></div> 
        </div> -->

        </scroller>
      </div>
      <music-player id="music-id0" class="image_play" style="display:none" />
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
    </div>
</template>
<script>
import { FlIcon, FlPopup } from "falcon-ui";
import voice from 'voice'

export default {
    components: { FlIcon, FlPopup },
    computed: {

    },
    created() {
      this.$page.on("show", this.onPageShow);
      this.$page.on("hide", this.onPageHide);
    },
    destroyed() {
      this.$page.off("show", this.onPageShow);
      this.$page.off("hide", this.onPageHide);
    },
  
    data() {
        return {
          events:null,
          show_popup: false,
          isPlaying: false,
          msg: "    请说...   ",
          image_name: "音乐播放器",
          transition: false,
          overlayTransition: false,
          position: "center",
          popContentClass: "demo-popup-content",
        };
    },
  
    methods: {
      handlePopupClose(){
        return false; //可以关
      },
      onPageShow(){
        console.log(">>> page Pulldown onPageShow");
 
        this.events=[
          {
            module:"voice",
            event:"sessionBegin",
            callback:
              (result) => {
                this.showPop();
              },   
            token:null
          },
          {
            module:"voice",
            event:"sessionEnd",
            callback:
              () => {
                this.show_popup = false;
              },   
            token:null
          },
        ];
        $falcon.trigger("on_event",this.events);
      },

      onPageHide(){
        console.log(">>> page Pulldown onPageHide");
        $falcon.trigger("off_event",this.events);
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
      },
      // 跳转系统设置页面
      toSet(){
        $falcon.navTo("systemSet")
      },
      // 跳转音乐播放器页面
      show_image_play(){
        $falcon.navTo("musicPlay")
      },
      // 直接语音唤醒
      onClickP2t(){
        voice.pushToTalk();
      },
      // 跳转子设备管理界面
      onClickNavtoDevmng(){
        $falcon.navTo("mesh")
      },
      // 跳转场景控制页面
      onClickNavtoSence(){
        $falcon.navTo("scences")
      }
    },
    watch: {
    },
};
</script>
  
<style lang="less" scoped>
@import "base.less";
.pull-down{
  flex-direction: column;
  width: 424px;
  height: 424px;
  margin: 28px 28px 28px 28px
}
.network-scroller{
    width: 100%;
    height: 100%;
}
.flex-self{
  
}
.icon_image{
  position: absolute;
  margin: 21px 39px 57px 39px;
  width: 52px;
  height: 52px;
}
.fun_unit{
  background-color: #1E212A;
  align-items: center;
}
.small_text{
  margin-top: 87px;
}
.small_icon{
  position: absolute;
  width: 24px;
  height: 24px;
}

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
  