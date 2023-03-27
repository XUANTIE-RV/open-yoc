<template>
  <div class="page" @click="back">
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
    <div class="card" :style="style">
      <text class="card_title">{{$page.loadOptions.text}}</text>
    </div>
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
    style() {
      return {
        borderRadius: this.$page.loadOptions.radius,
        boxShadow: this.$page.loadOptions.shadow ? '3px 3px 5px red' : '',
        backgroundImage: 'url(' + require('../../images/img.png') + ')',
      };
    }
  },
  methods: {
    back(e) {
      this.$page.finish();
    }
  },
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";

.page {
  width: 100%;
  height: 100%;
  background-color: black;
  flex-direction: row;
  flex-wrap: wrap;
}

.card {
  width: 152px;
  height: 152px;
  margin: 3px;
  align-items: center;
  justify-content: flex-end;
}

.card_title {
  font-size: 24px;
  color: black;
}
</style>
