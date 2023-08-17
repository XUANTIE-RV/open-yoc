<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="top-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">版本检测</text>
    </div>

    <div id="div-id4" style=""></div>
    <div id="div-id5" class="fota_box">
      <div id="div-id6" class="circle_box" style="margin-bottom: 10px">
        <text id="txt-id7" style="font-size: 24px; color: rgb(155, 248, 12)">{{
          version_id
        }}</text>
        <text id="txt-id8" style="font-size: 20px; color: rgb(155, 248, 12)">{{
          version_time
        }}</text>
        <text id="txt-id9" class="small_text">{{ sys_name }}</text>
      </div>
      <div
        id="div-id10"
        class="info_box"
        style="width: 350px; height: 100px; border-width: 1px; border-color: "
      >
        <text
          id="txt-id11"
          class="text"
          style="
            border-radius: 10px;
            border-width: 1px;
            border-color: white;
            padding: 10px 10px 10px 10px;
          "
          @click="check_fun"
          >开始检测</text
        >
        <text
          id="txt-id12"
          class="text"
          style="
            border-radius: 10px;
            border-width: 1px;
            border-color: white;
            padding: 5px 10px 5px 10px;
            margin-left: 25px;
          "
          @click="check_pull"
          >更新</text
        >
      </div>
    </div>
  </div>
</template>
<script>
import { FlIcon, FlPopup, FlCheckbox, FlSeekbar } from "falcon-ui";
import fota from "fota";
import smta from "smta";
import kv from "system_kv";

let version_info;

export default {
  components: { FlIcon, FlPopup, FlCheckbox, FlSeekbar },

  data() {
    return {
      bg: require("../../../images/state_icons/bg_light_gray.png"),
      version_id: "Vx.x.x",
      version_time: "xxxx.x.xx",
      sys_name: "yoc",
      check_info: "",
      events:null,
    };
  },
  async created() {
    this.$page.on("show", this.onPageShow);
    this.$page.on("hide", this.onPageHide);

    let res = await fota.start();
    /*异步函数，需要await等待返回值*/
    console.log("fota created:"+"StartFota res=" + res);

    this.events=[
      //下载就绪
      {
        module:"fota",
        event:"ready",
        callback:
          (props) => {
            console.log("fota created:"+"FOTA Ready");
            /*此时才可以调用fota.start方法*/
          },   
        token:null
      },
      //下载结束
      {
        module:"fota",
        event:"end",
        callback:
          async (props) => {
              // 下载成功
              console.log("fota created:"+"end check_pull "+props.msg);
              this.sys_name = "3秒后重启";

                /*3秒后触发重启事件*/
              fota.restart(3000);
          },   
        token:null
      },
      //下载过程
      {
        module:"fota",
        event:"download",
        callback:
          async (props) => {

              console.log("fota created:"+"check_pull "+"percent:" + props.percent + "\tspeed:" + props.speed);
              this.sys_name = props.percent + "%";
          },   
        token:null
      },
      //版本检测
      {
        module:"fota",
        event:"version",
        callback:
          async (props) => {
              console.log("fota created:"+props.code)
              console.log("fota created:"+props)
              if (props.code == 0) {
                // 版本检测成功
                if (
                  props.newversion === "" ||
                  props.newversion === props.curversion
                ) {
                  this.version_id="";
                  this.version_time = "您的系统已经是最新版本";
                  this.sys_name="";
                } else {
                  // 这里有新的版本信息，可以选择升级
                  this.version_id = "当前: V" + props.curversion.substr(0, 5);

                  // this.version_id = props.curversion;

                  this.version_time = "新版: V" + props.newversion.substr(0, 5);
                  // this.version_time = props.newversion;
                  this.sys_name = props.model;
                }
              } else {
                this.version_id="";
                this.version_time = "您的系统已经是最新版本";
                this.sys_name="";
              }
            },   
        token:null
      },

    ]
    
    $falcon.trigger("on_event",this.events);
  },

  watch: {},

  computed: {},
  methods: {
    async onPageShow() {
      let res = await fota.start();
      console.log("fota onPageShow:"+"StartFota res=" + res);
      res=kv.getItem("cur_version");
      if(res!="undefined"){
        this.version_id = "V" + res.substr(0, 5);
        this.version_time = res.substr(6, 8);
        res=kv.getItem("model");
        if(res!="undefined"){
          this.sys_name = res;
        }
      }
    },
    async onPageHide() {
      let res = await fota.stop();
      console.log("fota onPageHide:"+"StopFota res=" + res);
    },
    onBack() {
      this.$page.finish();
    },

    // 开始版本检测 versionCheck
    check_fun() {
      let res = fota.versionCheck();
      console.log("fota check_fun:"+"VersionCheck res" + res);
    },

    // 开始下载数据
    check_pull() {
      let res = fota.download();
      console.log("fota check_pull:"+"Fota Download res=" + res);
    },


  },
  destroyed() {
    this.$page.off("show", this.onPageShow);
    this.$page.off("hide", this.onPageHide);
    $falcon.trigger("off_event",this.events);
  },
};
</script>
  
<style lang="less" scoped>
@import "base.less";
.fota_box {
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
.info_box {
  justify-content: center;
  align-items: center;
  flex-direction: row;
  margin: 20px;
}
</style>
  