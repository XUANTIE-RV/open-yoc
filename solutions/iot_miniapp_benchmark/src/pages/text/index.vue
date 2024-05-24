<template>
  <div class="page" @click="back">
    <div class="pagediv" :style="{left:x1+'px'}">
      <text class="text" :style="textStyle">{{text}}</text>
    </div>
    <div class="pagediv" :style="{left:x2+'px'}">
      <text class="text" :style="textStyle">{{text}}</text>
    </div>
  </div>
</template>

<script>

export default {
  name: "index",
  data() {
    return {
      text: 'Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。',
      
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
    textStyle() {
      return {
        opacity: this.$page.loadOptions.opacity, 
        fontSize: this.$page.loadOptions.size, 
        textShadow: this.$page.loadOptions.shadow ? '3px 3px 5px red' : ''
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
  align-items: center;
  justify-content: center;
  position: absolute;
}

.text {
  color: white;
}

</style>
