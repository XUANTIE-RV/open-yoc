<template>
  <div class="page" :style="pagestyle" @click="back">
    <div class="pagediv" :style="{left:x1+'px'}">
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
    </div>
    <div class="pagediv" :style="{left:x2+'px'}" v-if="showdiv2">
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
      <img class="image" :style="style" resize="stretch" :src="require('../../images/img.jpg')" />
    </div>

  </div>
</template>

<script>

export default {
  name: "index",
  data() {
    return {
      style:{},

      showdiv2:false,
      intervalId: null,
      interval: 26,

      x:0,//0->480 循环
      x1:0,
      x2:-480,
      speed:3,

      range:35,

      widthMax:0,
      heightMax:0,
      widthMin:0,
      heightMin:0,
      width:0,
      height:0,

      rotateAngle:10,
    }
  },
  mounted() {
    let s;
    if (this.$page.loadOptions.scale1) {
      s = {
        width: '158px',
        height: '158px',
        margin: '1px',
        /*
        width: '130px',
        height: '130px',
        margin: '6px',
        */
      };
    } else if (this.$page.loadOptions.scale2) {
      s = {
        width: (158-this.range)+'px',
        height:(158-this.range)+'px',
        margin: '1px',
        /*
        width: '158px',
        height: '158px',
        margin: '1px',
        */
      };
    } else {
      s = {
        width: '152px',
        height: '152px',
        margin: '3px',
      };
    }
    if (this.$page.loadOptions.rotate) {
      s.transform = 'rotate(10deg)';
    }
    this.style=s;
    if(this.$page.loadOptions.scale1){
      this.widthMax=parseInt(this.style.width);
      this.heightMax=parseInt(this.style.height);
      this.widthMin=this.widthMax-this.range;
      this.heightMin=this.heightMax-this.range;
      this.width=this.widthMax;
      this.height=this.heightMax;
      this.intervalId=setInterval(() => {    
          this.width=this.width-1;
          this.height=this.height-1;
          if(this.width<this.widthMin){
            this.width=this.widthMax;
            this.height=this.heightMax;
            //this.width=this.widthMin;
            //this.height=this.heightMin;
          }
          let tempStyle={
              width :this.width+'px'+ '',
              height:this.height+'px'+ '',
              margin:this.style.margin,
          };
          this.style = tempStyle;                
      }, this.interval);
    }
    else if(this.$page.loadOptions.scale2){
      this.widthMin=parseInt(this.style.width);
      this.heightMin=parseInt(this.style.height);
      this.widthMax=this.widthMin+this.range;
      this.heightMax=this.heightMin+this.range;
      this.width=this.widthMin;
      this.height=this.heightMin;
      this.intervalId=setInterval(() => {    
          this.width=this.width+1;
          this.height=this.height+1;
          if(this.width>this.widthMax){
            this.width=this.widthMin;
            this.height=this.heightMin;
            //this.width=this.widthMax;
            //this.height=this.heightMax;
          }
          let tempStyle={
              width :this.width+'px'+ '',
              height:this.height+'px'+ '',
              margin:this.style.margin,
          };
          this.style = tempStyle;                
      }, this.interval);
    }
    else if(this.$page.loadOptions.rotate){
      this.intervalId=setInterval(() => {

        this.rotateAngle++;
        if(this.rotateAngle>=360){
          this.rotateAngle=0;
        }
        let tempStyle={
          width :this.style.width,
          height:this.style.height,
          margin:this.style.margin,
          transform : 'rotate('+this.rotateAngle+'deg)'
        };
        this.style = tempStyle; 
      }, this.interval);
    }
    else{
      this.showdiv2=true;
      this.intervalId=setInterval(() => {
        this.x+=this.speed;
        if (this.x>=480) {
          this.x = 0;
        }
        this.x1=this.x;
        this.x2=this.x-480;
      }, this.interval);
    }
  },
  computed: {
    pagestyle () {
      return {
        opacity: this.$page.loadOptions.opacity
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


.image {
}

</style>
