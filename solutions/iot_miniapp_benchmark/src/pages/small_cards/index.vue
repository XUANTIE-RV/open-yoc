<template>
  <div class="page" @click="back">
    <div class="card" :style="style" v-for="n in 100">
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
        borderWidth: this.$page.loadOptions.border,
        opacity: this.$page.loadOptions.opacity,
        borderRadius: this.$page.loadOptions.radius,
        boxShadow: this.$page.loadOptions.shadow ? '3px 3px 5px red' : ''
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
  width: 42px;
  height: 42px;
  margin: 3px;
  border-color: rgb(255, 0, 0);
  background-color: white;
  align-items: center;
  justify-content: flex-end;
}

.card_title {
  font-size: 30px;
  color: black;
}
</style>
