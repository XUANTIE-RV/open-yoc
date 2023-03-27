<template>
  <div style="background-color:white;">
    <scroller class="wrapper" scroll-direction="vertical" over-scroll="50px">
      <div class="button" v-for="(v, i) in cases" v-bind:key="i" @click="openCase(v.page)">
        <text class="text">{{v.title}}</text>
      </div>
    </scroller>
  </div>
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
        {title:'jsapi hello', page:'hello'}
      ]
    }
  },
  methods: {
    openCase(v) {
      $falcon.navTo(v);
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
}

.button {
  margin: 5px;
  padding: 20px;
  background-color: #444;
}
.text {
  color: white;
  font-size: 20px;
}
</style>
