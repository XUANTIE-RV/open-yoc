<template>
    <div class="background" v-bind:style="{backgroundImage:'url(' + bg + ')'}">
        <div class="top_label_panel">
            <fl-icon name="back" class="nav-back" @click="onBack" />
            <text class="top_label_text">子设备控制</text>
        </div>

        <div style="border-width: 1px; border-color: ;"></div>
        <div class="voice_box" style=" flex-direction: column;">
            <text class="text sub_num">{{ sub_num }}</text>
            <div class="box1">
                <scroller class="scroller">
                    <div class="scroller-v-item" v-for="(v, i) in node_list" v-bind:key="i" style="border-width: 1px; border-color: burlywood;" >
                        <text class="text" style=" margin-left: 28px;" @click="openSetSub(v)" >{{ v }}</text>
                        <text class="text" style=" position: absolute; margin-left: 250px; border-radius: 5px; border-width: 1px; border-color: antiquewhite; padding: 2px;" @click="del_subNode(v)" >退网</text>
                    </div>
                </scroller>
            </div>
        </div>


    </div>
</template>
<script>
import { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch, FlDialog, FlRadio } from "falcon-ui";
import gw from "gateway";
export default {
    components: { FlIcon, FlPopup, FlCheckbox, FlSeekbar, FlSwitch, FlDialog, FlRadio },

    data() {
        return {
            show: false,
            sub_num:"在线设备: 0",
            bg: require('../../../images/state_icons/bg_light_gray.png?base64'),
            node_list:[],

            devname: [
                { label: "light1", value: "light1", macAdd: "" },
                { label: "light2", value: "light2", macAdd: "" },
            ],
            roomtype: [
                { label: "客厅", value: "LIVING_ROOM" },
                { label: "卧室", value: "BED_ROOM" },
            ],
            devType: [
                { label: "照明灯", value: "OBJ_LIGHT" },
            ],
            operatedMac:""
        };
    },
    created() {
        gw.nodeListGet().then((props) => {
            this.sub_num = "在线设备: " + (props.length);
            this.node_list.push(props[i].dev_addr);
        });
        this.$page.on("show", this.onPageShow);
        this.$page.on("hide", this.onPageHide);
    },

    destroyed() {
    this.$page.off("show", this.onPageShow);
    this.$page.off("hide", this.onPageHide);
    },

    watch: {

    },

    computed: {

    },

    methods: {
        onPageShow(){
            gw.nodeListGet().then((props) => {
                this.sub_num = "在线设备: " + (props.length);
                this.node_list.push(props[i].dev_addr);
            });
        },
        onPageHide(){

        },
        arrayLength1() {
            return this.node_list.length;
        },

        onBack() {
            this.$page.finish();
        },
    },
};
</script>
  
<style lang="less" scoped>
@import "base.less";
.voice_box{
    height: 450px;
    width: 480px;
    // border-color: greenyellow;
    border-width: 1px;

    justify-content: center;
    align-items: center;
    
}
.circle_box{
    height: 200px;
    width: 200px;
    justify-content: center;
    align-items: center;
    flex-direction: column;

    border-color: white;
    border-width: 5px;
    border-radius: 100px;
}
.voice_btn{
    font-size: 24px;
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

.box1{
  border-width: 5px;
  border-color: antiquewhite;
  position: absolute;
  border-radius: 10px ;
  top: 50px;
  left: 30px;
  width: 70%;
  height: 80%;
}
.btn1{
  justify-content: center;
  align-items: center;
  border-width: 3px;
  border-color:antiquewhite;
  position: absolute;
  border-radius: 10px ;
  top: 45px;
  left: 375px;
  width: 20%;
  height: 15%;
}

.btn2{
  justify-content: center;
  align-items: center;
  border-width: 3px;
  border-color: antiquewhite;
  position: absolute;
  border-radius: 10px ;
  top: 120px;
  left: 375px;
  width: 20%;
  height: 15%;
}

.sub_num{
    position: absolute;
    top: 10px;
    left: 300px;
}

.dev_state{
    position: absolute;
    top: 10px;
    left: 10px;
}
</style>
  