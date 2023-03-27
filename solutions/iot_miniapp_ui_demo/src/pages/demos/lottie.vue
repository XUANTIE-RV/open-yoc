<template>
  <demo-scroller>
    <div class="main-wrapper">
      <div class="section" v-for="(item, index) in LOTTIES_FILE" :key="index">
        <div style="flex-direction: row;justify-content:space-between;width:100%;padding-top:20px;">
          <text class="section-title">动画{{ index + 1 }}</text>
          <div style="flex-direction: row;align-self:flex-end">
          <fl-button type="info" :textStyle="{fontSize:'20px'}" @click="startLottie(index)" style="margin:0 20px">启动动画</fl-button>
          <fl-button type="info" :textStyle="{fontSize:'20px'}" @click="stopLottie(index)" style="margin-right:20px;">结束动画</fl-button>
          </div>
        </div>

        <lottie
          class="lottie"
          ref="lottie"
          :key="'lottie' + index"
          style="width:400px;height:280px"
          :lottieFile="item"
          :loopCount="0"
          :autoplay="false"
        />
      </div>
    </div>
  </demo-scroller>
</template>
<script>
import { FlButton } from "falcon-ui";

const LOTTIES_FILE = [
// "assets/lotties/lottie.json",
   "assets/lotties/lottie2.json",
   "assets/lotties/lottie3.json",
// "assets/lotties/lottie4.json",
   "assets/lotties/lottie5.json",
   "assets/lotties/lottie6.json",
]

export default {
  components: { FlButton },
  data() {
    return {
    }
  },
  created() {
    this.LOTTIES_FILE = LOTTIES_FILE
  },
  methods: {
    startLottie(index) {
      for (let i = 0; i < LOTTIES_FILE.length; i++) {
        this.$refs.lottie[i].cancel();
      }
      this.$refs.lottie[index].play();
    },
    stopLottie(index) {
      this.$refs.lottie[index].cancel();
    }
  }
};
</script>

<style lang="less" scoped>
@import "base.less";
.lottie{
  margin-top:10px;
}
.main-wrapper{
  margin-bottom:0;
}
</style>
