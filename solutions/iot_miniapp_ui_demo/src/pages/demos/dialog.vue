<template>
  <demo-scroller>
    <div class="main-wrapper">
      <div class="section">
        <text class="section-title">基础用法</text>
        <div class="demo-block demo-row">
          <fl-button
            class="btn1"
            @click="showDialog(1)"
            >基础样式</fl-button
          >
          <fl-button
            class="btn1"
            @click="showDialog(2)"
            >输入框</fl-button
          >
          <fl-button
            class="btn1"
            @click="showDialog(3)"
            >自定义内容</fl-button
          >
        </div>
      </div>
    </div>
    <fl-dialog
      v-model="show1"
      title="基础样式"
      center
      contentCenter
      :dialogStyle="{width: '720px', height: '400px'}"
      :showConfirm="false"
      cancelText="知道了"
      content="这里是详细内容描述"
    >
    </fl-dialog>

    <fl-dialog
      v-model="show2"
      title="输入框"
      center
      contentCenter
      content=""
      :dialogStyle="{width: '720px', height: '400px'}"
      :showCancel="false">
      <input class="show2-input" placeholder="请输入文本" placeholderColor="rgba(255, 255, 255, 0.4)">
    </fl-dialog>

    <fl-dialog
      v-model="show3"
      title="自定义内容"
      center
      contentCenter
      :dialogStyle="{width: '720px', height: '400px'}"
      :showCancel="false"
      confirmText="确定"
    >
      <fl-seekbar v-model="seekbarValue" v-bind="seekbarCfg" useBlockStyle :length="672" />
    </fl-dialog>

  </demo-scroller>
</template>
<script>
import { FlDialog, FlSeekbar } from "falcon-ui";
import { FlButton } from "falcon-ui";

export default {
  name: "Dialog",
  components: { FlDialog, FlButton, FlSeekbar },
  data() {
    return {
      show1: false,
      show2: false,
      show3: false,
      seekbarValue: 36,
      seekbarCfg: { min: 0, max: 100 },
    };
  },
  mounted() {},
  methods: {
    showDialog(index) {
      this["show" + index] = true;
    },
  }
};
</script>
<style lang="less" scoped>
@import "base.less";
.btn1 {
  width: 326px;
}
.show2-input {
  width: 610px;
  height: 101px;
  border-radius: @radius-medium;
  background-color: @background-color;
  position: absolute;
  padding-left: 12px;
  left: 55px;
  top: 56px;
  color: @white;
}
</style>
