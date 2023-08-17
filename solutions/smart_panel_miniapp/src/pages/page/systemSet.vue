<template>
    <div id="bgr-id0" class="background" v-bind:style="{backgroundImage:'url(' + bg + ')'}">
        <div id="bgr-id1" class="top_label_panel">
            <fl-icon id="icon-id0" name="back" class="nav-back" @click="onBack" />
            <text id="txt-id0" class="top_label_text">设置</text>
        </div>

        <scroller id="scroller-id0" scroll-direction="vertical" class="systemSet-scroller">
          <div id="div-id2" class="scroller-v-item" v-for="(v, i) in cases" v-bind:key="i" @click="openCase(v.page)">
            <image id="img-id0" :src= v.src style="position: relative; width: 42px; height: 42px; margin-left: 28px;" />
            <text id="txt-id1" class="text" style=" margin-left: 16px;">{{ v.title }}</text>
            <text id="txt-id2" class="text" style="position: absolute; margin-left: 350px; ">{{ v.state }}</text>
            <image id="img-id1" :src= "require('../../images/menu_icons/ic_ secondarypages_go_nor.png')" style="position: absolute; width: 13px; height: 24px; margin-left: 439px;" />
            <div style="position: absolute; top: 70px; margin: 0 0 0 80px; width: 370px; height: 2px; background-color: wheat;"></div>
          </div>
          <div class = "high_set" style="height: 48px; width: 100%; background-color: rgba(22, 24, 32, 1); justify-content: center; ">
            <text id="txt-id3" class="text" style="margin-left: 28px; color: rgba(129, 129, 130, 1);">高级设置</text>
          </div>
          <div class="scroller-v-item" v-for="(v, i) in high_set" v-bind:key="i" @click="openCase(v.page)">
            <image id="img-id2" :src= v.src style="position: relative; width: 42px; height: 42px; margin-left: 28px;" />
            <text id="txt-id4" class="text" style=" margin-left: 16px;">{{ v.title }}</text>
            <image id="img-id3" :src= "require('../../images/menu_icons/ic_ secondarypages_go_nor.png')" style="position: absolute; width: 13px; height: 24px; margin-left: 439px;" />
            <div style="position: absolute; top: 70px; margin: 0 0 0 80px; width: 370px; height: 2px; background-color: wheat;"></div>
          </div>
          <div id="nor-id7" style="height: 48px; width: 100%;"></div>
        </scroller>

    </div>
</template>
<script>
import { FlIcon, FlPopup } from "falcon-ui";
import smta from "smta";
import screen from 'screen';
import kv from 'system_kv';


export default {
    components: { FlIcon, FlPopup },
    computed: {
      cases() {
        return [
          {src: require('../../images/menu_icons/ic_setup_net_nor.png?base64'), title:'网络', state: this.stateTable.netState, page:'network'},
          {src: require('../../images/menu_icons/ic_setup_voice_nor.png?base64'), title:'声音', state: this.stateTable.volume + "%" , page:'sound'},
          {src: require('../../images/menu_icons/ic_setup_view_nor.png?base64'), title:'亮度与息屏', state: this.stateTable.screen + "%", page:'screen'},
          {src: require('../../images/menu_icons/ic_setup_language_nor.png?base64'), title:'语音助手', page:'voice'},
          {src: require('../../images/menu_icons/fotaUpgrade.png?base64'), title:'fota升级',page:'fota'},
        ]
      },
      high_set() {
        return [
          // {src: require('../../images/menu_icons/ic_setup_voice_nor.png?base64'), title:'模式选择',page:'voice'},
          {src: require('../../images/menu_icons/ic_setup_about_used_nor.png?base64'), title:'系统',page:'sys'}
        ]
      }

    },
    created() {
      this.$page.on("show", this.onShow);
      this.stateTable.volume = smta.getVol();
      
      screen.getInfo().then((res) => {
        this.stateTable.screen = res.brightness;
      });

      let res=kv.getItem("wifi_state");
      console.log("systemSet created:"+"wifi status is " + res)
      if(res === "connect"){
        console.log("systemSet created:"+"connect");
        this.stateTable.netState = "已连接"
      }else{
        console.log("systemSet created:"+"disconnect");
        this.stateTable.netState = "未连接"
      }

      this.events=[
        {
          module:"$falcon",
          event:"soundChange",
          callback:
            (e) => {
              console.log("systemSet created:"+"change"+e.data);
              this.stateTable.volume = e.data
            },   
          token:null
        },
        {
          module:"$falcon",
          event:"screenChange",
          callback:
            (e) => {
              console.log("systemSet created:"+"change"+e.data);
              this.stateTable.screen = e.data
            },   
          token:null
        },
        {
          module:"$falcon",
          event:"wifi_change",
          callback:
            (e) => {
              console.log("systemSet created:"+"change"+e.data);
              this.stateTable.netState = e.data
            },   
          token:null
        },
      ];
      
      $falcon.trigger("on_event",this.events);

    },
    destroyed() {
      this.$page.off("show", this.onShow);
      $falcon.trigger("off_event",this.events);
    },

    data() {
        return {
          events:null,
          bg: require('../../images/state_icons/bg_light_gray.png'),
          
          stateTable: {
            netState: "未连接",
            volume: "",
            screen: ""
          }
        };
    },

    methods: {
          onShow(){
          console.log("systemSet onShow:"+"updated");
          this.stateTable.volume = smta.getVol()
          screen.getInfo().then((res) => {
            this.stateTable.screen = res.brightness;
          });
 
          let res=kv.getItem("wifi_state");
          if(res == "connect"){
            console.log("systemSet onShow:"+"connect");
            this.stateTable.netState = "已连接"
          }else{
            console.log("systemSet onShow:"+"disconnect");
            this.stateTable.netState = "未连接"
          }
    },
        onBack() {
            this.$page.finish();
        },
        funSelected(index) {
            this.selectedIndex = index;
        },
        openCase(v) {
          $falcon.navTo(v, this.stateTable);
          console.log("systemSet openCase:"+this.stateTable.volume);
        },
    },
};
</script>
  
<style lang="less" scoped>
@import "base.less";
.section_1 {
    width: 407px;
    height: 180px;
    margin: 24px 0 0 37px;
}

.div_box {
    background-color: @btn-background-color;
    width: 180px;
    height: 180px;
    border-radius: @border-radius-normal;

    align-items: center;
}

.image {
    width: 50%;
    height: 50%;
    margin-top: 20px;
}

.section_2 {
    width: 407px;
    height: 180px;
    margin: 23px 0 0 37px;
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
.fun-item-selected {
  opacity: 1;
}
.fun_item{

}
.systemSet-scroller{
    width: 100%;
    height: 100%;
    // border-width: 1px;
    // border-color: red;
}

#scroll-item() {
  // align-items: flex-start;
  // align-content: center;
  align-items: center;
  justify-content: flex-start;
  flex-direction: row;
  background-color: transparent;
  height:80px;


}
.scroller-v-item {
  width: 100%;
  #scroll-item();
}
.button {
  margin: 5px;
  padding: 20px;
  background-color: #444;
}
</style>
  