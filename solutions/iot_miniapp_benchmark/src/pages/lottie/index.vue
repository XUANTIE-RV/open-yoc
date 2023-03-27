<template>
  <div class="page" @click="back">
    <lottie
      class="lottie"
      lottieFile="/assets/lotties/reading-nocontent.json"
      :autoPlay="true"
    />
    <lottie
      class="lottie"
      lottieFile="/assets/lotties/reading-nocontent.json"
      :autoPlay="true"
    />
    <lottie
      class="lottie"
      lottieFile="/assets/lotties/reading-nocontent.json"
      :autoPlay="true"
    />
    <lottie
      class="lottie"
      lottieFile="/assets/lotties/reading-nocontent.json"
      :autoPlay="true"
    />
    
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

.lottie {
  width: 200px;
  height: 200px;
  margin: 20px;
}

</style>
