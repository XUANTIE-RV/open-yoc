<template>
  <div id="div-id0" class="page">
    <div id="div-id1" class="fl-demo">
      <scroller id="scroller-id2" class="scroller">
        <slot />
      </scroller>
    </div>
  </div>
</template>
<script>
import { FlIcon } from "falcon-ui";
export default {
  name: "DemoScroller",
  components: { FlIcon },
  props: {},
  data(){
    return {
      title:this.$page.$pageName
    };
  },
  methods:{
    onBack(){
      this.$page.finish();
    }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base";
.nav-wrap {
  display: flex;
  flex-direction: row;
  align-items: center;
  justify-content: flex-start;
  height: 82px;
}
.nav-back{
  line-height:50px;
}

.nav-title {
  font-size: 32px;
  color: #FFFFFF;
  font-weight: bold;
  flex: 1;
  text-align: left;
  line-height: 56px;
  margin-left:20px;
}
.fl-demo {
  flex: 1;
}
.scroller {
  display: flex;
  flex: 1;
  flex-direction:column;
  justify-content: flex-start;
}
</style>