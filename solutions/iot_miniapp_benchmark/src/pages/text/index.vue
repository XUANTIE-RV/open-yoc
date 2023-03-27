<template>
  <div class="page" @click="back">
    <text class="text" :style="textStyle">{{text}}</text>
  </div>
</template>

<script>

export default {
  name: "index",
  data() {
    return {
      text: 'Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。 Her hair was tied back in a neat bow. 她的头发扎在脑后打了个整齐的蝴蝶结。'
    }
  },
  mounted() {
  },
  computed: {
    textStyle() {
      if (this.$page.loadOptions.shadow) {
        return {
          textShadow: '3px 3px 5px red'
        };
      } else {
        return {};
      }
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
  align-items: center;
  justify-content: center;
}

.text {
  font-size: 24px;
  color: white;
}

</style>
