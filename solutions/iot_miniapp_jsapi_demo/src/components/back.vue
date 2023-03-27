<template>
  <div class="back" @click="back">
    <text class="back-text">返回</text>
  </div>
</template>

<script>
export default {
  name: "index",
  data() {
    return {};
  },
  methods: {
    back() {
      this.$page.finish();
    },
  },
};
</script>

<style lang="less" scoped>
@import "../styles/base.less";

.back {
  position: fixed;
  left: 20px;
  top: 20px;
  width: 60px;
  height: 40px;
  background-color: rgb(136, 136, 136, 0.6);
  align-items: center;
  justify-content: center;
}
.back-text {
  color: white;
  font-size: 20px;
}
</style>
