<template>
  <div class="key_container">
    <!-- 输入框部分 -->
    <div class="input_div">
      <div class="input_part_out">
        <div class="input_part_in" style="flex-direction: column; align-items: center; ">
          <div class="input_part_info" style="width: 100%;height: 70%; flex-direction: row; justify-content: center; ">
            <!-- 这里可以放 icon -->
            <div class="input_part_text" style="flex-direction: column; justify-content: center;">
                <text class="input_info_text">{{ labelName }}</text>
                <div class="input_box">
                  <input
                    :value="inputText"
                    :placeholder="placeholder"
                    placeholderColor="#777"
                    style="width: 100%;height: 100%;"
                    class="input-text"
                    autofocus="true"
                  >
              </div>
            </div>
          </div>
          <div class="input_part_menu" style="width: 100%; height: 30%; flex-direction: row; align-items: center; justify-content: space-between;">
            <div class="input_part_bt" @click="onSubmit" style="margin-left: 20px;">
              <text class="input_part_bt_text" >连接</text>
            </div>
            <div class="input_part_bt" @click="onClose" style="margin-right: 20px;">
              <text class="input_part_bt_text" >返回</text>
            </div>
          </div>
        </div>
      </div>
    </div>


    <!-- 键盘部分 -->
    <div class="key_div">



        <div class="key-panel">
          <div v-for="(char,index) in curChars.slice(0,10)" :key="index" class="btn-key yxp-touch" @click="onJoinChar(char)">
            <text class="key-text">{{ char }}</text>
          </div>
          <div style="width: 15px;"></div>
          <div v-for="(char,index) in curChars.slice(10,19)" :key="index" class="btn-key yxp-touch" @click="onJoinChar(char)">
            <text class="key-text">{{ char }}</text>
          </div>
          <div class="btn-transparency1"></div>
          <div v-for="(char,index) in curChars.slice(19,26)" :key="index" class="btn-key yxp-touch" @click="onJoinChar(char)">
            <text class="key-text">{{ char }}</text>
          </div>

          <div v-for="(char,index) in curChars.slice(26,)" :key="index" class="btn-key yxp-touch" @click="onJoinChar(char)">
            <text class="key-text">{{ char }}</text>
          </div>
          <div class="btn-transparency"></div>


          <div class="menu_panel" >
            <div class="btn-del yxp-touch" @click="onDelChar">
              <image class="img" resize="contain" :src="require('../assets/img/backspace@2x.png')" />
            </div>
            <div class="btn-key btn-menu yxp-touch" :class="{'menu-active':curCharType==='CHAR_LOWER'}" @click="onSwitchChar('CHAR_LOWER')">
              <text class="key-text" :class="{'menu-text-active':curCharType==='CHAR_LOWER'}">abc</text>
            </div>

            <div class="btn-key btn-menu yxp-touch" :class="{'menu-active':curCharType==='CHAR_UPPER'}" @click="onSwitchChar('CHAR_UPPER')">
              <text class="key-text" :class="{'menu-text-active':curCharType==='CHAR_UPPER'}">ABC</text>
            </div>
            <div class="btn-key btn-menu yxp-touch" @click="onJoinChar(' ')">
              <image class="img" resize="contain" :src="require('../assets/img/slot@2x.png')" />
            </div>
            <div class="btn-key btn-menu yxp-touch" :class="{'menu-active':curCharType==='CHAR_DIGIT'}" @click="onSwitchChar('CHAR_DIGIT')">
              <text class="key-text" :class="{'menu-text-active':curCharType==='CHAR_DIGIT'}">123</text>
            </div>
            <div class="btn-key btn-menu yxp-touch" :class="{'menu-active':curCharType==='CHAR_MARK'}" @click="onSwitchChar('CHAR_MARK')">
              <text class="key-text" :class="{'menu-text-active':curCharType==='CHAR_MARK'}">!@#</text>
            </div>

            <div class="btn-check yxp-touch">
              <image resize="contain" style="width:36px;height:36px;opacity:0.35;" :style="{'opacity':lengthValid?0.35:1}" :src="require('../assets/img/check@2x.png')" />
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
    minLimit: { type: Number, default: 0 },
    labelName: { type: String, default: '默认标签'},
  },
  data() {
    return {
      input_info: "RockEnergy",
      inputText: "",
      curChars: "",
      curCharType: "CHAR_LOWER",
      CHAR_UPPER: "QWERTYUIOPASDFGHJKLZXCVBNM",
      CHAR_LOWER: "qwertyuiopasdfghjklzxcvbnm",
      CHAR_MARK: `!@#$%^&*-=_+\\|;':",./<>?~\``,
      CHAR_DIGIT: `1234567890`,
    };
  },
  computed: {
    lengthValid() {
      if (this.minLimit > 0) {
        if (this.inputText.length < this.minLimit) {
          return true;
        }
      }
      return false;
    },
  },
  created() {
    this.curChars = this[this.curCharType];
  },
  methods: {
    // 切换键盘类型
    onSwitchChar(charType) {
      this.curCharType = charType;
      this.curChars = this[charType];
      console.log("keyboard onSwitchChar:"+this.curChars);
    },
    // 输入字符
    onJoinChar(char) {
      this.inputText = this.inputText + char;
    },
    // 删除字符
    onDelChar() {
      this.inputText = this.inputText.slice(0, -1);
    },
    // 输入完毕提交
    onSubmit() {
      if (this.lengthValid === true) {
        console.log("keyboard onSubmit:"+"this.lengthValid=" + this.lengthValid);
        return;
      }
      this.$emit("submit", this.inputText);
      this.inputText = ""
      this.onClose();
    },
    // 关闭键盘
    onClose() {
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