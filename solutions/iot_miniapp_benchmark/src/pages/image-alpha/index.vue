<template>
  <div class="page" :style="pagestyle" @click="back">
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
    <img class="image" :style="style" resize="stretch" :src="require('../../images/img.png')" />
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
      let s;
      if (this.$page.loadOptions.scale1) {
        s = {
          width: '130px',
          height: '130px',
          margin: '6px',
        };
      } else if (this.$page.loadOptions.scale2) {
        s = {
          width: '158px',
          height: '158px',
          margin: '1px',
        };
      } else {
        s = {
          width: '152px',
          height: '152px',
          margin: '3px',
        };
      }
      return s;
    },
    pagestyle () {
      return {
        opacity: this.$page.loadOptions.opacity
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

.image {
}

</style>
