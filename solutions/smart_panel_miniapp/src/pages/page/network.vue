<template>
    <div id="bgr-id0" class="background" v-bind:style="{backgroundImage:'url(' + bg + ')'}">
        <div id="top-id1" class="top_label_panel">
            <fl-icon id="icon-id3" name="back" class="nav-back" @click="onBack" />
            <text id="txt-id4" class="top_label_text">网络设置</text>
        </div>

        <scroller id="scroller-id5" scroll-direction="vertical" class="network-scroller">
          <div id="div-id6" class="scroller-v-item" v-for="(v, i) in newwork_mode" v-bind:key="i" >
            <image id="img-id7" :src= v.src style="position: relative; width: 42px; height: 42px; margin-left: 28px;" />
            <text id="txt-id8" class="text" style=" margin-left: 16px;" @click="openCase(v.page)">{{ v.title }}</text>
            <!-- <image id="img-id9" :src="!isConnect ? require('../../images/menu_icons/ic_chiose_nor.png') : require('../../images/menu_icons/ic_chiose_press.png') " 
              style="position: absolute; width: 26px; height: 26px; margin-left: 426px;" />  -->
            <!-- <image id="img-id10" :src="isConnect ? require('../../images/menu_icons/ic_chiose_press.png') : require('../../images/menu_icons/ic_chiose_nor.png')" 
              style="position: absolute; width: 26px; height: 26px; margin-left: 426px;" @click= "modeSelect" /> -->

            <image id="img-id10" :src="v.isEnable ? require('../../images/menu_icons/ic_chiose_press.png') : require('../../images/menu_icons/ic_chiose_nor.png')" 
              style="position: absolute; width: 26px; height: 26px; margin-left: 426px;" @click= "modeSelect(v)" />

            <div id="nor-id11" style="position: absolute; top: 70px; margin: 0 0 0 80px; width: 370px; height: 2px; background-color: wheat;"></div>
          </div>

          <div id="div-id12" class = "other_set" style="height: 48px; width: 100%; background-color: rgba(22, 24, 32, 1); ">
            <text id="txt-id13" class="small_text" style="margin-left: 28px; color: rgba(129, 129, 130, 1); margin: 9px 0 9px 0;">{{ connected_info }}</text>
          </div>

          <!-- <div class="scroller-v-item" v-show="cur_net" @click="cur_wifi_info" >
            <text class="text" style=" margin-left: 28px;">{{ cur_wifi }}</text>
            <image :src= "require('../../images/menu_icons/ic_ secondarypages_go_nor.png')" style="position: absolute; width: 13px; height: 24px; margin-left: 439px;" />
            <div style="position: absolute; margin-left: 400px; width: 70px; height: 60px;  border-width: 1px; border-color: red;" @click="connect_wifi(v)"></div>
            <div style="position: absolute; top: 70px; margin: 0 0 0 28px; width: 424px; height: 2px; background-color: #191C24;"></div>
          </div> -->
          
          <div id="div-id14" class = "other_set" style="height: 48px; width: 100%; background-color: rgba(22, 24, 32, 1); ">
            <text id="div-id15" class="small_text" style="margin-left: 28px; color: rgba(129, 129, 130, 1); margin: 9px 0 9px 0;">其他网络</text>
          </div>


          <div id="div-id16" class="scroller-v-item" v-show="other_net" v-for="(v, i) in high_set" v-bind:key="i"  >
            <text id="txt-id17" class="text" style=" margin-left: 28px;">{{ v }}</text>
            <image id="img-id18" :src= "require('../../images/menu_icons/ic_ secondarypages_go_nor.png')" style="position: absolute; width: 13px; height: 24px; margin-left: 439px;" />
            <div id="div-id19" style="position: absolute; margin-left: 400px; width: 70px; height: 60px; " @click="connect_wifi(v)"></div>
            <div id="div-id20" style="position: absolute; top: 70px; margin: 0 0 0 28px; width: 424px; height: 2px; background-color: wheat;"></div>
          </div>
          <div id="nor-id21" style="height: 48px; width: 100%;"></div>
        </scroller>

        <yxp-keyboard 
          v-show="isShowKeyboard" 
          :softInputEnable = "inputEnable"
          :labelName = ssid
          :min-limit="0" 
          @close="isShowKeyboard=false, inputEnable=false" 
          @submit="onSubmit" />
    </div>
</template>

<script>
import { FlIcon, FlPopup, FlCheckbox } from "falcon-ui";
import wifi from "wifi";
import kv from 'system_kv';

export default {
    components: { FlIcon, FlPopup, FlCheckbox },
    data() {
        return {
          events:null,
          connected_info: "已连接: 无",
          wifi_list:[],
          isShowKeyboard:false,
          bg: require('../../images/state_icons/bg_light_gray.png'),
          isConnect: true,
          other_net: true,
          cur_net: true,
          tempConfig:"",
        };
    },
    created() {
        this.$page.on("show", this.onPageShow);
        this.$page.on("hide", this.onPageHide);
        console.log("network created");

        this.events=[
          //进入页面自动扫描wifi
          {
            module:"wifi",
            event:"scan_result",
            callback:
              (scanResult) => {
                while(this.arrayLength()>0){
                  this.wifi_list.splice(0, 1); // 移除元素
                }
                for (var i in scanResult) {
                  if (Math.abs(scanResult[i].rssi) > 100) {
                  } else if (scanResult[i].ssid === "") {
                  } else {
                    console.log("network created:"+scanResult[i].ssid + " " + scanResult[i].status);
                    this.wifi_list.push(scanResult[i].ssid)
                  }
                  console.log("network created:"+scanResult[i]);
                }
              },   
            token:null
          },
          {
            module:"$falcon",
            event:"wifi_change",
            callback:
              () => {
                // console.log("network created:"+"success!" + event.data);
                let res=kv.getItem("wifi_state");
                console.log("network change" + res);
                if(res === "connect"){
                  console.log("systemSet created:"+"connect");
                  this.connected_info = "已连接: " + kv.getItem('connected_info');
                }else{
                  console.log("systemSet created:"+"disconnect");
                  this.connected_info = "已连接: 无" ;
                }
              },
            token:null
          },
        ];
        $falcon.trigger("on_event",this.events);

        //进入页面自动扫描wifi
        wifi.scan()
        
        let res=kv.getItem("wifi_state");
        if(res == "connect"){
          console.log("systemSet created:"+"connect");
          this.connected_info = "已连接: " + kv.getItem('connected_info');
        }else{
          console.log("systemSet created:"+"disconnect");
          this.connected_info = "已连接: 无" ;
        }
    },
    watch: {

    },
    computed: {
      newwork_mode() {
        return [
          {src: require('../../images/menu_icons/ic_scense_nor.png'), title:'有线网络', isEnable: !this.isConnect, page:'wirednet'},
          {src: require('../../images/menu_icons/ic_scense_nor_wifi.png'), title:'无线网络', isEnable: this.isConnect},
        ]
      },
      high_set() {
        return this.wifi_list;
      }
      
    },

    methods: {
      openCase(v) {
          $falcon.navTo(v);
        },
      arrayLength() {
        return this.wifi_list.length;
      },
      async onPageShow(){

      },
      async onPageHide(){

      },
      connect_wifi(ssid){
        this.ssid = ssid
        this.isShowKeyboard = true;
        this.inputEnable = true;
        console.log("network connect_wifi:"+"connect_wifi=" + ssid);
      },

      onSubmit(psk) {
        // 1、添加WiFi配置
        this.inputEnable = false;
        this.isShowKeyboard = false;

        var temp = {ssid:this.ssid, psk:psk};
        this.tempConfig = temp;
        wifi.addConfig(temp)
        console.log("network onSubmit:"+"add wifi config success !");
        // 2、连接WiFi
        wifi.connect(this.ssid)

      },
      modeSelect(v){
        // v.isEnable = !v.isEnable
        this.isConnect = !this.isConnect
        if(this.isConnect){
          // 网络 开启
          this.other_net = true;
          this.cur_net = true;
          console.log("network modeSelect:"+"wifi open js");
          // $falcon.trigger('net',)
        }else{
          // 网络 关闭
          this.other_net = false;
          this.cur_net = false;
          wifi.disconnect();
          console.log("network modeSelect:"+"wifi close js");
        }
      },
      onBack() {
          console.log("network onBack");
          this.$page.finish();
      },
    },
    beforeDestroy() {
      $falcon.trigger("off_event",this.events);
    },
    destroyed() {
      this.$page.off("show", this.onPageShow);
      this.$page.off("hide", this.onPageHide);
    },
};
</script>
  
<style lang="less" scoped>
@import "base.less";
.network-scroller{
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
  height:80px;

}
.scroller-v-item {
  width: 100%;
  #scroll-item();
}
</style>
  