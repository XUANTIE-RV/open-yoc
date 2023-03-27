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
        {title:'图片缩小', page:'image', params: {scale1: true}},
        {title:'图片放大', page:'image', params: {scale2: true}},
        {title:'图片旋转', page:'image', params: {rotate: true}},
        {title:'半透图片', page:'image-alpha', params: {}},
        {title:'半透图片缩小', page:'image-alpha', params: {scale1: true}},
        {title:'半透图片放大', page:'image-alpha', params: {scale2: true}},
        {title:'文字', page:'text', params: {}},
        {title:'文字&阴影', page:'text', params: {shadow: true}},
        {title:'卡片', page:'cards', params: {}},
        {title:'圆角卡片', page:'cards', params: {radius: '20px'}},
        {title:'卡片阴影', page:'cards', params: {radius: '20px', shadow: true}},
        {title:'卡片&文本', page:'cards', params: {radius: '20px', text: '我是标题'}},
        {title:'图文', page:'image-cards', params: {text: '我是标题'}},
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
