<template>
  <div id="div-id0" class="key_container">
    <!-- 输入框部分 -->
    <div id="div-id1" class="input_div">
      <div id="div-id2" class="input_part_out">
        <div
          id="div-id3"
          class="input_part_in"
          style="flex-direction: column; align-items: center"
        >
          <div
            id="div-id4"
            class="input_part_info"
            style="
              width: 100%;
              height: 70%;
              flex-direction: row;
              justify-content: center;
            "
          >
            <!-- 这里可以放 icon -->
            <div
              id="div-id5"
              class="input_part_text"
              style="flex-direction: column; justify-content: center"
            >
              <text id="txt-id6" class="input_info_text">{{ labelName }}</text>
              <div id="div-id7" class="input_box">
                <input
                  id="input-id8"
                  :softInputEnable="softInputEnable"
                  :value="inputText"
                  :placeholder="placeholder"
                  placeholderColor="#777"
                  style="width: 100%; height: 100%"
                  class="input-text"
                  autofocus="false"
                  @input="ipaddInput"
                  @confirm="onSubmit_input"
                />
              </div>
            </div>
          </div>

          <div
            id="div-id9"
            class="input_part_menu"
            style="
              width: 100%;
              height: 30%;
              flex-direction: row;
              align-items: center;
              justify-content: space-between;
            "
          >
            <div
              id="div-id10"
              class="input_part_bt"
              @click="onSubmit"
              style="margin-left: 20px"
            >
              <text class="input_part_bt_text">连接</text>
            </div>
            <div
              id="div-id11"
              class="input_part_bt"
              @click="onClose"
              style="margin-right: 20px"
            >
              <text id="txt-id12" class="input_part_bt_text">返回</text>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>
  
<script>
export default {
  name: "YxpKeyboard",
  props: {
    placeholder: { type: String, default: " 请输入" },
    softInputEnable: { type: Boolean, default: false },
    minLimit: { type: Number, default: 0 },
    labelName: { type: String, default: "默认标签" },
  },
  data() {
    return {
      input_info: "RockEnergy",
      inputText: "",
      curChars: "",
    };
  },

  created() {},
  methods: {
    ipaddInput(e) {
      // 每次输入都会触发的事件
      this.inputText = e.value;
    },
    onSubmit_input: function (e) {
      this.inputText = e.value;
      this.$emit("submit", this.inputText);
      this.inputText = "";
      this.$emit("close");
    },
    onSubmit: function () {
      this.$emit("submit", this.inputText);
      this.inputText = "";
      this.$emit("close");
    },
    // 关闭键盘
    onClose() {
      this.inputText = "";
      this.$emit("close");
    },
  },
};
</script>
  
  <style lang="less" scoped>
@import "../styles/keyboard.less";

.input_part_b {
  color: red;
  font-size: 22px;
}
</style>