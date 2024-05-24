<template>
  <div class="page" @click="back">
    <div class="pagediv" :style="{left:x1+'px'}">
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
    <div class="pagediv" :style="{left:x2+'px'}">
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
  </div>
</template>

<script>

export default {
  name: "index",
  data() {
    return {
      intervalId: null,
      interval: 26,

      x:0,//0->480 循环
      x1:0,
      x2:-480,
      speed:3,
    }
  },
  mounted() {
    this.intervalId=setInterval(() => {
      this.x+=this.speed;
      if (this.x>=480) {
        this.x = 0;
      }
      this.x1=this.x;
      this.x2=this.x-480;
    }, this.interval);
  },
  computed: {
    style() {
      return {
        borderWidth: this.$page.loadOptions.border,
        opacity: this.$page.loadOptions.opacity,
        borderRadius: this.$page.loadOptions.radius,
        boxShadow: this.$page.loadOptions.shadow ? '3px 3px 5px red' : ''
      };
    }
  },
  methods: {
    back(e) {
      clearInterval(this.intervalId);
      this.intervalId = null;

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
  position: relative;
  overflow: hidden;
}

.pagediv {
  width: 100%;
  height: 100%;
  background-color: black;
  flex-direction: row;
  flex-wrap: wrap;
  position: absolute;
}

.card {
  width: 152px;
  height: 152px;
  margin: 3px;
  border-color: rgb(255, 0, 0);
  background-color: white;
  align-items: center;
  justify-content: flex-end;
}

.card_title {
  font-size: 24px;
  color: black;
}
</style>
