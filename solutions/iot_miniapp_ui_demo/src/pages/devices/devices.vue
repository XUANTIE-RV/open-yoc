<template>
  <div class="page">
    <div class="tab-list">
      <fl-icon name="back" class="nav-back" @click="$page.finish()" />
    </div>

    <scroller class="cell-list">
      <div
        v-for="(item, index) in cells" :key="index"
        class="cell-list-item"
        @click="onClick(item)"
      >
        <div class="cell-left-side">
          <!--
          <img v-if="item.signal==0" style="width: 36px; height: 36px;" :src="require(`../assets/ued/wifi-signal-0.png`)" />
          <img v-if="item.signal==1" style="width: 36px; height: 36px;" :src="require(`../assets/ued/wifi-signal-1.png`)" />
          <img v-if="item.signal==2" style="width: 36px; height: 36px;" :src="require(`../assets/ued/wifi-signal-2.png`)" />
          <img v-if="item.signal==3" style="width: 36px; height: 36px;" :src="require(`../assets/ued/wifi-signal-3.png`)" />
          -->
          <text class="cell-label-text">{{item.label}}</text>
        </div>
        <div class="cell-right-side">
          <fl-icon name="right" class="nav-right" />
        </div>
      </div>
    </scroller>

  </div>
</template>
<script>
import { FlButton, FlIcon } from "falcon-ui";

export default {
  components: { FlButton, FlIcon },
  name: "devices",
  data() {
    return {
      cells: [
        {label: '新手引导', page: 'GuidePage'},
        {label: 'Wifi配网', page: 'WifiPage'},
      ]
    }
  },
  methods: {
    onClick(item) {
      console.log(item)
    }
  }
};
</script>
<style lang="less" scoped>
@import "base.less";
.nav-back {
  line-height: 48px;
  margin-right: 20px;
}
.cell-list {
  margin: 15px 30px 15px 30px;
  row-gap: 10px;
}

.cell-list-item {
  height: 90px;
  width: 100%;
  border-radius: 20px;
  background-color: #343F50;
  margin-bottom: 18px;

  flex-direction: row;
  justify-content: space-between;
  align-items: center;
}
.cell-left-side {
  margin-left: 30px;
  flex-direction: row;
  column-gap: 25px;
}
.cell-label-text {
  font-family: AlibabaPuHuiTiM;
  font-size: 28px;
  color: #FFFFFF;
  line-height: 32px;
}
.cell-right-side {
  margin-right: 30px;
  flex-direction: row;
  column-gap: 25px;
}

.nav-right {
  line-height: 48px;
  margin-right: 20px;

  font-size: 38px;
  color: @text-color;
  font-weight:bold;
}

</style>

