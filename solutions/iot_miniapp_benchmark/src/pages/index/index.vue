<template>
  <scroller class="wrapper" scroll-direction="vertical" over-scroll="50px" over-fling="50px">
    <div class="container">
      <div class="button" v-for="(v, i) in cases" v-bind:key="i" @click="openCase(v)">
        <text class="text">{{v.title}}</text>
      </div>
    </div>
  </scroller>
</template>

<script>

export default {
  name: "index",
  data() {
    return {}
  },
  mounted() {
  },
  computed: {
    cases() {
      return [
        {title:'图片', page:'image', params: {}},
        {title:'图片+半透明', page:'image', params: {opacity: 0.5}},
        {title:'图片缩小', page:'image', params: {scale1: true}},
        {title:'图片放大', page:'image', params: {scale2: true}},
        {title:'图片旋转', page:'image', params: {rotate: true}},
        {title:'半透图片', page:'image-alpha', params: {}},
        {title:'半透图片+半透明', page:'image-alpha', params: {opacity: 0.5}},
        {title:'半透图片缩小', page:'image-alpha', params: {scale1: true}},
        {title:'半透图片放大', page:'image-alpha', params: {scale2: true}},
        {title:'文字小', page:'text', params: {size: 24}},
        {title:'文字中', page:'text', params: {size: 46}},
        {title:'文字大', page:'text', params: {size: 60}},
        {title:'文字小+半透明', page:'text', params: {size: 24, opacity: 0.5}},
        {title:'文字中+半透明', page:'text', params: {size: 46, opacity: 0.5}},
        {title:'文字大+半透明', page:'text', params: {size: 60, opacity: 0.5}},
        {title:'文字小+阴影', page:'text', params: {shadow: true, size: 24}},
        {title:'大卡片', page:'cards', params: {}},
        {title:'大圆角卡片', page:'cards', params: {radius: '20px'}},
        {title:'大圆', page:'cards', params: {radius: '76px'}},
        {title:'大圆角卡片+border', page:'cards', params: {radius: '20px', border: '2px'}},
        {title:'大圆角卡片+半透明', page:'cards', params: {radius: '20px', opacity: 0.5}},
        {title:'大圆角卡片+阴影', page:'cards', params: {radius: '20px', shadow: true}},
        {title:'卡片&文本', page:'cards', params: {radius: '20px', text: '我是标题'}},
        {title:'小卡片', page:'small_cards'},
        {title:'小圆角卡片', page:'small_cards', params: {radius: '5px'}},
        {title:'小圆', page:'small_cards', params: {radius: '21'}},
        {title:'小圆角卡片+border', page:'small_cards', params: {radius: '5px', border: '1px'}},
        {title:'小圆角卡片+半透明', page:'small_cards', params: {radius: '5px', opacity: 0.5}},
        {title:'小圆角卡片+阴影', page:'small_cards', params: {radius: '5px', shadow: true}},
        {title:'小圆角卡片+文字', page:'small_cards', params: {radius: '5px', text: 'A'}},
        {title:'半透图片+文字', page:'image-cards', params: {text: '我是标题'}},
        {title:'Lottie矢量动画', page:'lottie'},
      ]
    }
  },
  methods: {
    openCase(v) {
      $falcon.navTo(v.page, v.params);
    }
  },
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";

.wrapper {
  width: 100%;
  height: 100%;
  flex-direction: column;
  background-color: white;
}

.container {
  width: 100%;
  flex-direction: row;
  flex-wrap: wrap;
}

.button {
  margin: 5px;
  padding: 20px;
  background-color: #444;
}
.button:active {
  background-color: #888;
}
.text {
  color: white;
  font-size: 20px;
}
</style>
