<template>
  <div
    id="bgr-id0"
    class="background"
    v-bind:style="{ backgroundImage: 'url(' + bg + ')' }"
  >
    <div id="div-id1" class="top_label_panel">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
      <text id="txt-id3" class="top_label_text">子设备管理</text>
    </div>

    <div id="div-id4" style="border-width: 1px; border-color: "></div>
    <div id="div-id5" class="voice_box" style="flex-direction: column">
      <text id="txt-id6" class="text dev_state">{{ state_text }}</text>
      <text id="txt-id7" class="text sub_num">在线设备: {{ sub_num }}</text>
      <div id="div-id8" class="box1">
        <scroller id="scroller-id9" class="scroller">
          <div
            id="div-id10"
            class="scroller-v-item"
            v-for="(v, i) in scan_node_list"
            :key="i"
            style="border-width: 1px; border-color: burlywood"
          >
            <text
              id="txt-id11"
              class="text"
              style="margin-left: 28px"
              >{{ v }}</text
            >
            <text
              id="txt-id12"
              class="text"
              style="
                position: absolute;
                margin-left: 250px;
                border-radius: 5px;
                border-width: 1px;
                border-color: antiquewhite;
                padding: 2px;
              "
              @click="add_subNode(v)"
              >入网</text
            >
          </div>

          <!-- <div class="scroller-v-item" v-for="(v, i) in node_list" v-bind:key="i" style="border-width: 1px; border-color: burlywood;" >
                    <text class="text" style=" margin-left: 28px;" @click="openSetSub(v)" >{{ v }}</text>
                    <text class="text" style=" position: absolute; margin-left: 250px; border-radius: 5px; border-width: 1px; border-color: antiquewhite; padding: 2px;" @click="del_subNode(v)" >退网</text>
                </div> -->

          <div
            id="div-id13"
            class="scroller-v-item"
            v-for="(v, i) in devList"
            v-bind:key="i"
            style="border-width: 1px; border-color: burlywood">
            <text
              id="txt-id14"
              class="text"
              style="margin-left: 28px"
              @click="openSetSub(v)"
              >{{ getDeviceName(v) }}</text>
            <text
              id="txt-id15"
              class="text"
              style="
              position: absolute;
              margin-left: 250px;
              border-radius: 5px;
              border-width: 1px;
              border-color: antiquewhite;
              padding: 2px;"
              @click="del_subNode(v)" >退网</text>
          </div>
        </scroller>
      </div>
      <div id="txt-id16" class="btn1" :style="{ backgroundColor: bgColor1 }" @click="fun_scan">
        <text id="txt-id17" class="text" style="">扫描</text>
      </div>

      <div id="div-id19" class="btn2" :style="{ backgroundColor: bgColor2 }" @click="fun2_list">
        <text id="txt-id18" class="text" style="">已入网</text>
      </div>
    </div>

    <fl-dialog
      v-model="show"
      :dialogStyle="{ width: '460px', height: '90%' }"
      :showCancel="false"
      :showConfirm="false"
      confirmText="确定"
      @close="subManage"
      :handleConfirmClick="mesh_confirm"
      center
    >
    <div style="width: 100%; height: 100%;">
      <div
        style="
          border-width: 1px;
          border-color: ;
          width: 100%;
          height: 80%;
          position: absolute;
          left: 10px;
          top: 0px;
          flex-direction: row;
          margin-left: 10px;
        "
      >
        <div
          style="
            border-width: 1px;
            border-left-color: black;
            width: 120px;
            height: 100%;
          "
        >
          <text>设备名字</text>
          <fl-radio
            :items="devname"
            v-model="radioOptAct"
            radioColor="blue"
            radioSize="22px"
            radioGapx="22px"
            direction="column"
            style="margin-left: 10px"
          />
        </div>
        <div
          style="
            border-width: 1px;
            border-left-color: black;
            width: 120px;
            height: 100%;
          "
        >
          <text>所属房间</text>
          <fl-radio
            :items="roomtype"
            v-model="radioRoomType"
            radioColor="blue"
            radioSize="22px"
            radioGapx="22px"
            direction="column"
            style="margin-left: 10px"
          />
        </div>
        <div
          style="border-width: 1px; border-color: ; width: 120px; height: 100%"
        >
          <text>设备类型</text>
          <fl-radio
            :items="devType"
            v-model="radioDevType"
            radioColor="blue"
            radioSize="22px"
            radioGapx="22px"
            direction="column"
            style="margin-left: 10px"
          />
        </div>

      </div>
      <div style="position: absolute; top: 320px; flex-direction: row; align-self: center;">
        <div slot="footer" style="margin-right: 20px;">
          <FlButton class="dialog-button" :textStyle="{fontSize: '20px'}" style="" @click="mesh_confirm()">确认</FlButton>
        </div> 
        <div slot="footer" style="margin-left: 30px;" >
          <FlButton class="dialog-button" :textStyle="{fontSize: '20px'}" style="" @click="close_confirm()">取消</FlButton>
        </div>
      </div>
    </div>
    </fl-dialog>


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
import {  FlIcon,  FlPopup,  FlCheckbox,  FlSeekbar,  FlSwitch, FlDialog,  FlRadio,  FlButton} from "falcon-ui";
import gw from "gateway";
import kv from "system_kv";
export default {
  components: {  FlIcon,
 FlPopup, FlCheckbox,  FlSeekbar, FlSwitch, FlDialog, FlRadio, FlButton},

  data() {
    return {
      events:null,

      bgColor1: '',
      bgColor2: '',
      msg: "正在入网请稍后",
      show_popup: false,
      transition: false,
      overlayTransition: false,
      position: "center",
      popContentClass: "demo-popup-content",
      show: false,
      state_text: "状态: 无动作",
      sub_num: 0,
      bg: require("../../../images/state_icons/bg_light_gray.png?base64"),
      scan_node_list: [], // 用来显示扫描得到的设备
      node_list: [], // 用来显示已经入网的设备
      devList: [], // 保存所有子设备信息，包含dev_name,dev_addr

      radioMsg: "选中：无",

      radioOptAct: "",
      radioRoomType: "",
      radioDevType: "",
      devname: [
        { label: "客厅灯", value: "light1", macAdd: "" },
        { label: "窗帘", value: "screen1", macAdd: "" },
        //底层不支持obj_switch，现在先把开关当成obj_light类型，后面再改
        { label: "开关", value: "switch1", macAdd: "" },
      ],
      roomtype: [
        { label: "客厅", value: "LIVING_ROOM" },
        { label: "卧室", value: "BED_ROOM" },
        // { label: "书房", value: "STUDY_ROOM" },
        // { label: "厨房", value: "KITCHEN" },
        // { label: "浴室", value: "SHOWER_ROOM"}
      ],
      devType: [
        { label: "照明灯", value: "OBJ_LIGHT" },
        { label: "窗帘", value: "OBJ_SCREEN" },
        { label: "开关", value: "OBJ_SWITCH" },
      ],
      operatedMac: "",
    };
  },
  created() {
    gw.nodeListGet().then((props) => {
      if (!props) {
        this.sub_num = 0;
      } else {
        this.sub_num = props.length;
      }
    });
    this.$page.on("show", this.onPageShow);
    this.$page.on("hide", this.onPageHide);
    this.events=[
      // 扫描
      {
        module:"gw",
        event:"ProvShowDev",
        callback:
          (props) => {
            while (this.arrayLength1() > 0) {
              this.scan_node_list.splice(0, 1); // 移除元素
              console.log("subdev_mgt fun_scan:"+ "1");
            }
            while (this.arrayLength2() > 0) {
              this.node_list.splice(0, 1); // 移除元素
              console.log("subdev_mgt fun_scan:"+ "2");
            }
            while (this.arrayLength3() > 0) {
              this.devList.splice(0, 1); // 移除元素
            }

            console.log("subdev_mgt fun_scan:"+ "ProvShowDev");
            for (var i in props) {
              console.log("subdev_mgt fun_scan:"+ props[i].dev_addr);
              this.scan_node_list.push(props[i].dev_addr);
            }
            this.state_text = "状态: 扫描完成!";
            this.show_popup = false;
          },   
        token:null
      },
      //入网
      {
        module:"gw",
        event:"AddNodeRst",
        callback:
          (addNodeResult) => {
            console.log("subdev_mgt add_subNode:" + addNodeResult.dev_addr + " " + addNodeResult.rst_code);
            const { rst_code, dev_addr } = addNodeResult;
            if (rst_code === 0) {
              console.log("subdev_mgt add_subNode:" + `dev add success ${dev_addr} result ${rst_code}`);
              this.state_text = "状态: 添加成功";
              this.sub_num += 1;
              this.show_popup = false;

              // const index = this.scan_node_list.findIndex((item) => item === addNodeResult.dev_addr);
              // if (index !== -1) {
              //  console.log("subdev_mgt add_subNode:" +"add " + scan_node_list[index]);
              //     this.scan_node_list.splice(index, 1);
              // }
              for (var i = 0; i < this.scan_node_list.length; i++) {
                console.log("subdev_mgt add_subNode:" +this.scan_node_list[i]+" vs "+addNodeResult.dev_addr);
                if (this.scan_node_list[i] === addNodeResult.dev_addr) {
                  console.log("subdev_mgt add_subNode:" +"add " + this.scan_node_list[i]);
                  this.scan_node_list.splice(i, 1);
                  i--; // 因为已经删除了一个元素，所以将 i 减一
                }
              }
              var updatedScanNodeList = this.scan_node_list; // 这里是为了触发vue的更新机制，也可以把变量声明成响应式属性
              this.scan_node_list = updatedScanNodeList;
            } else {
              this.state_text = "状态: 添加失败";
            }
          },   
        token:null
      },
      //退网
      {
        module:"gw",
        event:"DelNodeRst",
        callback:
          async () => {
            if(this.state_text == "状态: 删除设备..."){
              this.state_text = "状态: 删除成功";
              console.log("subdev_mgt del_subNode:"+"DelNodeRst first");
              this.sub_num = this.sub_num - 1;
              //删除设备后，如果直接调用fun2_list，会有两次弹窗  
              this.bgColor2 = "#646566";
              this.bgColor1 = "";
              while (this.arrayLength1() > 0) {
                this.scan_node_list.splice(0, 1); // 移除元素
              }
              while (this.arrayLength2() > 0) {
                this.node_list.splice(0, 1); // 移除元素
              }
              while (this.arrayLength3() > 0) {
                this.devList.splice(0, 1); // 移除元素
              }
              gw.nodeListGet().then((props) => {
                for (var i in props) {
                  console.log("subdev_mgt fun2_list:"+ props[i]);
                  this.devList.push(props[i]);
                }
                this.show_popup = false;
              });
              
            }
          },   
        token:null
      },
    ]
    
    $falcon.trigger("on_event",this.events);
  },
  beforeDestroy() {
    $falcon.trigger("off_event",this.events);
  },
  destroyed() {
    this.$page.off("show", this.onPageShow);
    this.$page.off("hide", this.onPageHide);

  },

  watch: {},

  computed: {},

  methods: {
    handlePopupClose(){
       return true;//关不掉
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

    getDeviceName(device) {
      const deviceMap = {
        light1: "客厅灯",
        screen1: "窗帘",
        switch1: "开关",
      };

      if (device.dev_name && deviceMap.hasOwnProperty(device.dev_name)) {
        return deviceMap[device.dev_name];
      } else {
        return device.dev_addr || device.dev_name;
      }
    },
    getScanDevic(device) {
      return device;
    },
    mesh_confirm() {
      console.log("subdev_mgt mesh_confirm:"+this.radioOptAct+" "+this.radioRoomType+" "+this.radioDevType);
 
      var operations = {
        operator: this.radioOptAct, //
        roomType: this.radioRoomType, // 配置子设备所属房间
        deviceType: this.radioDevType, // 设备类型
        dev_mac: this.operatedMac,
      };
      gw.subDevMgmt(operations, operations.dev_mac);
      this.fun2_list();
      this.show = false
    },
    close_confirm(){
      this.show = false
    },
    // 设置配置信息
    openSetSub(obj) {

      for (let i = 0; i < this.devList.length; i++) {
        let str = '';
        str += `index${i+1}`;
        console.log("test " + str);
        let res=kv.getItem(str);
        console.log("kv get " + res)
        const arr = res.split('/');
        console.log("split  " + arr);
        console.log("split  " + arr[0]);
        console.log("split  " + arr[2]);
        console.log("split  " + arr[3]);
        console.log("split  " + arr[4]);

        if(arr[2] === obj.dev_name){
          this.radioOptAct = arr[2];
          this.radioRoomType = arr[3];
          this.radioDevType = arr[4];

          this.show = true;
          this.operatedMac = "";
          console.log("subdev_mgt openSetSub:"+ obj.dev_name + " " + obj.dev_addr);
          this.operatedMac = obj.dev_addr;
        }

       
      }
      this.show = true;
      this.operatedMac = "";
      console.log("subdev_mgt openSetSub:"+ obj.dev_name + " " + obj.dev_addr);
      this.operatedMac = obj.dev_addr;
    },

    arrayLength1() {
      return this.scan_node_list.length;
    },

    arrayLength2() {
      return this.node_list.length;
    },
    arrayLength3() {
      return this.devList.length;
    },

    async onPageShow() {
    },
    async onPageHide() {},

    // 扫描子设备
    fun_scan() {
      this.msg="正在扫描请稍后";
      this.show_popup = true;
      // 首先清除列表元素
      while (this.arrayLength2() > 0) {
        this.node_list.splice(0, 1); // 移除元素
        console.log("subdev_mgt fun_scan:"+ "-1");
      }
      while (this.arrayLength1() > 0) {
        this.scan_node_list.splice(0, 1); // 移除元素
        console.log("subdev_mgt fun_scan:"+ "-2");
      }
      while (this.arrayLength3() > 0) {
        this.devList.splice(0, 1); // 移除元素
      }
      this.bgColor2 = "";
      this.bgColor1 = "#646566";
      // 扫描子设备
      this.state_text = "状态: 扫描中....";

      var operations = {
        operator: "ACT_DEVSHOW",
        value: 1,
        dev_mac: "",
      };
      operations.dev_mac = "FF:FF:FF:FF:FF:FF";
      console.log("subdev_mgt fun_scan:"+ operations);
      gw.meshCtrl(operations, operations.dev_mac);

    },

    // 返回子设备列表
    fun2_list() {
      if(this.state_text != "状态: 查询中...."){
        this.msg="正在查询已入网设备";
        this.show_popup = true;
        this.state_text = "状态: 查询中....";
        this.bgColor2 = "#646566";
        this.bgColor1 = "";
        while (this.arrayLength1() > 0) {
          this.scan_node_list.splice(0, 1); // 移除元素
        }
        while (this.arrayLength2() > 0) {
          this.node_list.splice(0, 1); // 移除元素
        }
        while (this.arrayLength3() > 0) {
          this.devList.splice(0, 1); // 移除元素
        }

        gw.nodeListGet().then((props) => {
          for (var i in props) {
            console.log("subdev_mgt fun2_list:"+ props[i]);
            this.devList.push(props[i]);
          }
          this.show_popup = false;
          this.state_text = "状态: 查询完成!";
        });
      }
    },

    // 添加设备
    add_subNode(mac) {
      this.state_text = "状态: 添加设备...";
      var operations = {
        // 添加子设备
        operator: "ACT_DEVADD",
        value: 1,
        dev_mac: "",
      };
      operations.dev_mac = mac;

      gw.meshCtrl(operations, operations.dev_mac);
      this.msg="正在入网请稍后";
      this.show_popup = true;
    },

    // 删除子设备
    del_subNode(obj) {
      this.state_text = "状态: 删除设备...";
      var operations = {
        operator: "ACT_DEVDEL",
        value: 1,
        dev_mac: "",
      };
      operations.dev_mac = obj.dev_addr;
      gw.meshCtrl(operations, operations.dev_mac);
      this.msg="正在退网请稍后";
      this.show_popup = true;
      //500ms和退网成功的事件先到者有效
      setTimeout(() => {
        if(this.state_text == "状态: 删除设备...")
        {
          this.state_text = "状态: 删除成功";
          console.log("subdev_mgt del_subNode:"+"500ms first");
          this.sub_num = this.sub_num - 1;
          //删除设备后，如果直接调用fun2_list，会有两次弹窗  
          this.bgColor2 = "#646566";
          this.bgColor1 = "";
          while (this.arrayLength1() > 0) {
            this.scan_node_list.splice(0, 1); // 移除元素
          }
          while (this.arrayLength2() > 0) {
            this.node_list.splice(0, 1); // 移除元素
          }
          while (this.arrayLength3() > 0) {
            this.devList.splice(0, 1); // 移除元素
          }
          gw.nodeListGet().then((props) => {
            for (var i in props) {
              console.log("subdev_mgt fun2_list:"+ props[i]);
              this.devList.push(props[i]);
            }
            this.show_popup = false;
          });
          
        }
      }, 500); 
    },
    onBack() {
      this.$page.finish();
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

.box1 {
  border-width: 5px;
  border-color: antiquewhite;
  position: absolute;
  border-radius: 10px;
  top: 50px;
  left: 30px;
  width: 70%;
  height: 80%;
}
.btn1 {
  justify-content: center;
  align-items: center;
  border-width: 3px;
  border-color: antiquewhite;
  position: absolute;
  border-radius: 10px;
  top: 45px;
  left: 375px;
  width: 20%;
  height: 15%;
}

.btn2 {
  justify-content: center;
  align-items: center;
  border-width: 3px;
  border-color: antiquewhite;
  position: absolute;
  border-radius: 10px;
  top: 120px;
  left: 375px;
  width: 20%;
  height: 15%;
}

.sub_num {
  position: absolute;
  top: 10px;
  left: 300px;
}

.dev_state {
  position: absolute;
  top: 10px;
  left: 10px;
}
</style>
  