<!-- <template>
  <div
    :class="{
      'switch-outer': true,
      'switch-outer-active': isActive,
      'switch-outer-disable': disabled,
    }"
    v-on:touchstart="toggle()"
    v-on:touchend="stopCount"
    :style="switchStyle"
    :id="id"
  >
    <div
      id="div-id0"
      class="desc"
      style="position: absolute; width: 100%; height: 100%"
    >
      <image
        id="img-id1"
        class="switch_icon"
        resize="contain"
        :src="
          this.value
            ? require('../images/menu_icons/light_close48.png')
            : require('../images/menu_icons/light_open48.png')
        "
      />
      <text id="txt-id2" class="title">{{ title }}</text>
      <text id="txt-id4" class="text">{{
        isActive ? activeText : inactiveText
      }}</text>
    </div>
    <div
      id="div-id5"
      :class="isActive ? 'ball-active' : 'ball-inactive'"
      style="position: absolute; top: 5%; left: 88%"
    ></div>
  </div>
</template>
<script>
export default {
  name: "LightCrad",
  model: {
    // v-model双向数据绑定
    prop: "value",
    event: "change",
  },
  props: {
    id: {
      type: String,
      default: "id",
    },
    value: {
      // 是否选中
      type: [Boolean],
      default: false,
    },
    count: {
      // 长按计数
      type: [Number],
      default: 100,
    },
    title: {
      type: String,
      default: "标题",
    },
    activeText: {
      type: String,
      default: "已开启",
    },
    inactiveText: {
      type: String,
      default: "已关闭",
    },
    width: {
      type: [Number, String],
      default: "239px",
    },
    height: {
      type: [Number, String],
      default: "119px",
    },
    disabled: {
      type: Boolean,
      default: false,
    },
    beforeChange: {
      type: Function,
      default: null,
    },
  },
  data() {
    return {
      // 当前的value
      innerValue: this.value,
      intervalId: null,
      count: 0,
    };
  },
  computed: {
    isActive() {
      return this.innerValue == true;
    },
    switchStyle() {
      return {
        width: `${parseInt(this.width)}px`,
        height: `${parseInt(this.height)}px`,
      };
    },
  },
  methods: {
    incrementCount() {
      this.count++;
      if (this.count % 120 === 0) {
        this.count = 0;
      }
      if (this.count % 20 === 0) {
        console.log("LightCrad incrementCount:"+this.title);
        $falcon.trigger("set_light_value", {
          title: this.title,
          count: this.count,
        });
      }
    },
    // 通过点击触发事件
    toggle() {
      // 判断是否使能
      if (this.disabled) return;
      // 获得xxx的值
      let nextValue = this._getToggleValue();
      nextValue = this.beforeChange ? this.beforeChange(nextValue) : nextValue;
      this.innerValue = nextValue;
      this.intervalId = setInterval(this.incrementCount, 100);
    },
    stopCount() {
      this.count = 0;
      clearInterval(this.intervalId);
      this.intervalId = null;
    },
    _getToggleValue() {
      return !this.innerValue;
    },
  },
  watch: {
    // 对value的值进行监听
    value() {
      this.innerValue = this.value;
    },
    // 监听innervalue的值，触发change事件，带有额外参数
    innerValue() {
      this.$emit("change", this.innerValue);
    },
  },
};
</script> -->

<template>
  <div
    :class="{
      'switch-outer': true,
      'switch-outer-active': isActive,
      'switch-outer-disable': disabled,
    }"
    @click="toggle()"
    :style="switchStyle"
  >
    <div class="desc" style="position: absolute; width: 100%; height: 100%; ">
      
      <image class="switch_icon" resize="contain" :src="
        this.value ? require('../images/menu_icons/light_close48.png') 
        : require('../images/menu_icons/light_open48.png')
        " />
      <text class="title">{{ title }}</text>
      <text class="text">{{ isActive ? activeText : inactiveText }}</text>
    </div>
    <div :class="isActive ? 'ball-active' : 'ball-inactive'" style="position: absolute; top: 5%; left: 88%;"></div>
  </div>
</template>
<script>
export default {
  name: "LightCrad",
  model: {
    // v-model双向数据绑定
    prop: "value",
    event: "change",
  },
  props: {
    value: {
      // 是否选中
      type: [Boolean],
      default: false,
    },
    title: {
      type: String,
      default: "标题",
    },
    activeText: {
      type: String,
      default: "已开启",
    },
    inactiveText: {
      type: String,
      default: "已关闭",
    },
    width: {
      type: [Number, String],
      default: "239px",
    },
    height: {
      type: [Number, String],
      default: "119px",
    },
    disabled: {
      type: Boolean,
      default: false,
    },
    beforeChange: {
      type: Function,
      default: null,
    },
  },
  data() {
    return {
      // 当前的value
      innerValue: this.value,
    };
  },
  computed: {
    isActive() {
      return this.innerValue == true;
    },
    switchStyle() {
      return {
        width: `${parseInt(this.width)}px`,
        height: `${parseInt(this.height)}px`,
      };
    },
  },
  methods: {
    // 通过点击触发事件
    toggle() {
      console.log("LightCrad toggle:"+this.disabled+" "+this.value);
      // 判断是否使能
      if (this.disabled) return;
      // 获得xxx的值
      let nextValue = this._getToggleValue();
      nextValue = this.beforeChange ? this.beforeChange(nextValue) : nextValue;
      this.innerValue = nextValue;
    },
    _getToggleValue() {
      return !this.innerValue;
    },
  },
  watch: {
    // 对value的值进行监听
    value() {
      this.innerValue = this.value;
    },
    // 监听innervalue的值，触发change事件，带有额外参数
    innerValue() {
      this.$emit("change", this.innerValue);
    },
  },
};
</script>

<style lang="less" scoped>
@import "base.less";

.title {
  font-size: @font-size-title-small;
  color: @white;
  position: absolute;
  margin: 0 0 0 10px;
}

.text {
  font-size: @font-size-content-large;
  color: @white;
  align-self: center;
  margin: 120px 0 24px 0;
}
.switch_icon {
  position: absolute;
  height: 52px;
  width: 52px;
  margin: 41px 35px 41px 41px;
  // padding-bottom: 50px;
}
.switch-outer {
  // padding: 20px 20px 24px 24px;
  background-color: @btn-background-color;
  border-radius: @border-radius-normal;
  flex-direction: row;
  &-active {
    background-color: @primary;
  }
  &-disable {
    // opacity: @disabled-opacity;
  }
}

.ball-active {
  height: 16px;
  width: 16px;
  border-radius: 16px;
  background-color: @light-green;
}

.ball-inactive {
  height: 16px;
  width: 16px;
  border-radius: 16px;
  background-color: @card-background-color;
}
</style>
