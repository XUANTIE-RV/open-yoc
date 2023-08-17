<template>
  <div id="bgr-id0" class="background" v-bind:style="{ backgroundImage: 'url(' + bg + ')' }">
    <div id="top-id1" class="top_label_panel" style="height: 70px">
      <fl-icon id="icon-id2" name="back" class="nav-back" @click="onBack" />
    </div>
    <text id="txt-id3" class="top_label_text" style="
                      position: absolute;
                      margin: 13px 0 417px 0;
                      font-size: 36px;
                      font-weight: bold;
                    ">音乐播放器</text>
    <text id="txt-id4" style="
                      align-self: center;
                      color: #ffffff;
                      position: absolute;
                      margin: 62px 0 388px 0;
                      font-size: 22px;
                    ">{{ music_name }}</text>
    <div id="div-id5" style="
                      position: absolute;
                      margin: 95px 115px 115px 135px;
                      width: 230px;
                      height: 230px;
                    ">
      <image id="img-id6" style="width: 230px; height: 230px"
        :src="require('../../../images/music_icons/music_cricle.png?base64')" />
    </div>

    <div id="div-id7" style="
                      position: absolute;
                      top: 320px;
                      width: 90%;
                      height: 78px;
                      flex-direction: row;
                      align-self: center;
                      justify-content: center;
                      align-items: center;
                    ">
      <text id="txt-id8" class="small_text" style="
                        margin-left: 1px;
                        margin-right: 1px;
                        width: 70px;
                        text-align: right;
                      ">{{ cur_musicT }}</text>
      <fl-seekbar id="seekbar-id9" v-model="musicTime" v-bind="seekbarCfg_time" v-bind:max="cur_musicTS"
        @change="musicTimeChanged" @click="musicTimeChanged" />
      <text id="txt-di10" class="small_text" style="margin-left: 1px; width: 70px">{{ cur_musicTS }}</text>
    </div>

    <image id="img-id11" :src="
      require('../../../images/music_icons/ic_music_previous_song_nor.png') " style="
                      width: 36px;
                      height: 36px;
                      position: absolute;
                      margin: 400px 320px 44px 124px;
                    " @click="prev" />

    <image id="img12" :src="
      isPlaying
        ? require('../../../images/music_icons/ic_music_stop_nor.png')
        : require('../../../images/music_icons/ic_music_play_nor.png')" style="
                      width: 36px;
                      height: 36px;
                      position: absolute;
                      margin: 400px 222px 44px 222px;
                    " @click="start_Play" />

    <image id="img13" :src="require('../../../images/music_icons/ic_music_next_song_nor.png')" style="
                      width: 36px;
                      height: 36px;
                      position: absolute;
                      margin: 400px 123px 44px 320px;
                    " @click="next" />

    <image id="img14" :src="
      require('../../../images/music_icons/ic_music_previous_song_vloum.png') " style="
                      width: 28px;
                      height: 28px;
                      position: absolute;
                      margin: 404px 34px 48px 418px;
                    " @click="volumeChange" />
    <div id="div-id15" class="music_section" v-show="isVolumeShow" style="
                      border-width: 1px;
                      border-color: 1px;
                      position: absolute;
                      margin: 120px 0 0 400px;
                    ">
      <fl-seekbar ref="mobile" v-model="value" v-bind="seekbarCfg" useBlockStyle direction="vertical" />
    </div>
  </div>
</template>
<script>
import { FlIcon, FlPopup, FlCheckbox, FlSeekbar } from "falcon-ui";
import { seekbar } from "falcon-ui/src/styles/theme.config";
import smta from "smta";
import kv from "system_kv";
var urlList = [];
export default {
  components: { FlIcon, FlPopup, FlCheckbox, FlSeekbar },

  data() {
    return {
      events:null,
      //非零值代表定时器允许更新；0不允许；初始化是允许的  timerID的处理全部移到smta事件中。
      //简化逻辑：next()：仅调用smta接口，在start事件中更新页面。
      timerID: 1, 

      bg: require("../../../images/state_icons/bg_light_gray.png?base64"),
      music_name: "name",
      isPlaying: false,
      cur_num: 0,
      musicTime: 0,
      cur_musicT: 0,
      cur_musicTS: 0,
      seekbarCfg_time: {
        length: 300,
        min: 0,
        activeColor: "#FFFFFF",
        disabledColor: "green",
        handleSize: "26px",
      },
      value: "",
      isVolumeShow: false,
      seekbarCfg: {
        min: 0,
        max: 100,
        blockLabel: "音量",
        length: "220px",
        blockWidth: "40px",
      },

    };
  },
  created() {
    urlList[0] = "file:///mnt/alibaba1.mp3";
    urlList[1] = "file:///mnt/alibaba2.mp3";
    urlList[2] = "file:///mnt/alibaba3.mp3";
    this.isPlaying = false
    this.$page.on("hide", this.onPageHide);
    this.$page.on("show", this.onPageShow);
    
    this.events=[
      //恢复播放
      {
        module:"smta",
        event:"resume",
        callback:   
          function () {
            console.log("music_play created:"+"PlayResume");
            // 先停止上一个定时器再开启下一个定时器.
            clearTimeout(this.timerId);
            this.timerId = 0;
            this.timerId=1;
            this.updateTime();
            this.isPlaying = true;
          }.bind(this),
        token:null
      },
      //停止播放
      {
        module:"smta",
        event:"stop",
        callback:   
          function () {
            console.log("music_play created:"+"PlayStop");
          }.bind(this),
        token:null
      },
      //播放暂停
      {
        module:"smta",
        event:"pause",
        callback:   
          function () {
            console.log("music_play created:"+"PlayPause");
            clearTimeout(this.timerId);
            this.timerId = 0;
            this.isPlaying = false;
          }.bind(this),
        token:null
      },
      //播放完成
      {
        module:"smta",
        event:"finish",
        callback:   
          function () {
            //真正歌曲放完时,this.timerId非零，可以进来

            //底层，播放时快进，会调用stop，但此时this.timerId为零进不来(musicTimeChanged中置0)
            //底层，播放时切歌，会调用stop，进得来，但无所谓了

            //语音停止播放时，this.timerId为零，进不来，所以需要再audio_stop中更新显示。
            if(this.timerId)
            {
              clearTimeout(this.timerId);
              this.timerId = 0;
              this.musicTime=  this.cur_musicTS;
              this.cur_musicT = this.cur_musicTS;
              console.log("music_play created:"+"PlayFinish");
              this.isPlaying = false;
              //底层，播放时切歌/快进，会调用stop；触发finish事件后，此处再次调用stop冲突
              //smta.stop();
            }
          }.bind(this),
        token:null
      },
      //开始播放
      {
        module:"smta",
        event:"start",
        callback:   
          async function () {
          // 先停止上一个定时器再开启下一个定时器.
          clearTimeout(this.timerId);
          this.timerId = 0;
          let info={
            url:smta.getUrl(),
            duration:smta.getDuration()
          };
          console.log("music_play created:"+"current play url " + info.url + " " + info.duration);
          //把首页的cur_num传递到音乐播放器页面，也可以通过kv
          if(info.url==="file:///mnt/alibaba1.mp3"){
              this.cur_num=0;
          }
          else if(info.url==="file:///mnt/alibaba2.mp3"){
              this.cur_num=1;
          }
          else if(info.url==="file:///mnt/alibaba3.mp3"){
              this.cur_num=2;
          }
          this.music_name=info.url.substring(info.url.lastIndexOf("/")+1,info.url.lastIndexOf(".mp3"))
          this.cur_musicTS = (info.duration / 1000).toFixed(0);
          this.isPlaying = true;
          this.timerId=1;
          this.updateTime();
        }.bind(this),
        token:null
      },
      //语音上一首 vs 按键上一首
      {
        module:"$falcon",
        event:"audio_prev",
        callback:   
          async () => {
          },
        token:null
      },
      //语音下一首 vs 按键下一首
      {
        module:"$falcon",
        event:"audio_next",
        callback:   
          async (e) => {
          },
        token:null
      },
      //语音暂停 vs 按键暂停
      {
        module:"$falcon",
        event:"audio_pause",
        callback:   
          async (e) => {
          },
        token:null
      },
      //语音恢复 vs 按键恢复
      {
        module:"$falcon",
        event:"audio_resume",
        callback:   
          async (e) => {
          },
        token:null
      },
      //语音停止
      {
        module:"$falcon",
        event:"audio_stop",
        callback:   
          async () => {
            clearTimeout(this.timerId);
            this.timerId = 0;
            this.musicTime=  this.cur_musicTS;
            this.cur_musicT = this.cur_musicTS;
            console.log("music_play created:"+"PlayFinish");
            this.isPlaying = false;
          },
        token:null
      },
    ]

    $falcon.trigger("on_event",this.events);
    //进入页面时会调用initPara两次，但是等到pageshow再调用，页面显示会有延迟
    //亮屏时只会在pageshow调用一次initPara
    this.initPara();
  },

  watch: {
    async value() {
      console.log("music_play watch:"+"set vol && " + this.value);
      await smta.setVol(this.value);
    },
  },

  computed: {},
  methods: {
    // 初始化页面参数
    initPara() {
      console.log("music_play initPara");
      let configs=smta.getStatus();
      console.log("music_play initPara:"+"current play status " + configs);
      switch (configs) {
        case 1:
          // 停止播放状态：息屏时会错过停止播放的事件
          let info2={
            url:smta.getUrl(),
            duration:smta.getDuration()
          };
          if(info2.url==="name"){
            this.music_name="name";
            this.cur_musicTS=0;
          }
          else{
            this.music_name=info2.url.substring(info2.url.lastIndexOf("/")+1,info2.url.lastIndexOf(".mp3"))
            this.cur_musicTS = (info2.duration / 1000).toFixed(0);
            //把首页的cur_num传递到音乐播放器页面，也可以通过kv
            if(info2.url==="file:///mnt/alibaba1.mp3"){
                this.cur_num=0;
            }
            else if(info2.url==="file:///mnt/alibaba2.mp3"){
                this.cur_num=1;
            }
            else if(info2.url==="file:///mnt/alibaba3.mp3"){
                this.cur_num=2;
            }
          }
          clearTimeout(this.timerId);
          this.timerId = 0;
          this.musicTime=  this.cur_musicTS;
          this.cur_musicT = this.cur_musicTS;
          this.isPlaying = false;
          break;
        case 2:
          // 正在播放状态.
          clearTimeout(this.timerId);
          this.timerId = 0;
          let info={
            url:smta.getUrl(),
            duration:smta.getDuration()
          };
          console.log("music_play initPara:"+"current play url " + info.url + " " + info.duration);
          //把首页的cur_num传递到音乐播放器页面，也可以通过kv
          if(info.url==="file:///mnt/alibaba1.mp3"){
              this.cur_num=0;
          }
          else if(info.url==="file:///mnt/alibaba2.mp3"){
              this.cur_num=1;
          }
          else if(info.url==="file:///mnt/alibaba3.mp3"){
              this.cur_num=2;
          }
          this.music_name=info.url.substring(info.url.lastIndexOf("/")+1,info.url.lastIndexOf(".mp3"))
          this.cur_musicTS = (info.duration / 1000).toFixed(0);
          this.isPlaying = true;
          this.timerId=1;
          this.updateTime();
          break;
        case 3:
          // 暂停播放状态
          let info1={
            url:smta.getUrl(),
            duration:smta.getDuration()
          };
          console.log("music_play initPara:"+"current play url " + info1.url + "\t" + info1.duration);
          //把首页的cur_num传递到音乐播放器页面，也可以通过kv
          if(info1.url==="file:///mnt/alibaba1.mp3"){
              this.cur_num=0;
          }
          else if(info1.url==="file:///mnt/alibaba2.mp3"){
              this.cur_num=1;
          }
          else if(info1.url==="file:///mnt/alibaba3.mp3"){
              this.cur_num=2;
          }
          this.music_name=info1.url.substring(info1.url.lastIndexOf("/")+1,info1.url.lastIndexOf(".mp3"))
          this.cur_musicTS = (info1.duration / 1000).toFixed(0);
          let time = smta.getCurTime();
          this.cur_musicT = (time / 1000).toFixed(0);
          this.musicTime = this.cur_musicT;
          this.isPlaying = false;
          clearTimeout(this.timerId);
          this.timerId=0;
          break;
        default:
          break;
      }
      let vol = smta.getVol();
      console.log("music_play initPara:"+"smta vol is $$" + vol);
      this.value = vol;
    },
    async onPageHide() {
      console.log("music_play onPageHide");
      clearTimeout(this.timerId);
      this.timerId = 0;
    },
    onPageShow() {
      console.log("music_play onPageShow");
      this.initPara();
    },
    async volumeChange() {
      this.isVolumeShow = !this.isVolumeShow;
      if (this.isVolumeShow) {
        let vol = smta.getVol();
        console.log("music_play volumeChange:"+"smta vol is " + vol);
        this.value = vol;
      }
    },
    onBack() {
      this.$page.finish();
    },
    prev() {
      if (this.cur_num == 0) {
        this.cur_num = 2;
      } else {
        this.cur_num = this.cur_num - 1;
      }
      smta.play(urlList[this.cur_num], 0);
    },
    async start_Play() {
      if (!this.isPlaying) {
        this.isPlaying = true;
        let configs=smta.getStatus();
        console.log("music_play start_Play:"+"current play status " + configs);
        switch (configs) {
          case 0:
            // 未知状态
            await smta.play(urlList[this.cur_num], 0);
            break;
          case 1:
            // 停止状态
            await smta.play(urlList[this.cur_num], 0);
            break;
          case 2:
            // 播放状态 
            await smta.play(urlList[this.cur_num], 0);
            break;
          case 3:
            // 暂停状态 -=> 恢复播放
            smta.resumeEnable(1);
            await smta.resume().then(async (configs) => {
              console.log("music_play start_Play:"+configs);
              if (configs === -1) {
                await smta.play(urlList[this.cur_num], 0);
              }
            })
            break;
          default:
            break;
        }
        
      } else if (this.isPlaying) {
        this.isPlaying = false;
        let configs=smta.getStatus();
        console.log("music_play start_Play:"+"current play status " + configs);
        switch (configs) {
          case 0:
            await smta.stop();
            break;
          case 1:
            await smta.pause();
            break;
          case 2:
            await smta.pause();
            break;
          case 3:
            await smta.pause();
            break;
          default:
            break;
        }
      }
    },
    async updateTime() {
      let time = smta.getCurTime();
      let info={
        url:smta.getUrl(),
        duration:smta.getDuration()
      };

      if(this.timerId)
      {
        this.cur_musicT = (time / 1000).toFixed(0);
        this.musicTime = this.cur_musicT;
        this.cur_musicTS = (info.duration / 1000).toFixed(0);

        // this.$refs.mobile.seekbarCfg.max = info.duration;
        // fl-seekbar.setAttribute(max,info.duration)

        this.timerId = setTimeout(() => {
          console.log("music_play updateTime:"+this.timerId);
          this.updateTime();
        }, 1000);
      }
    },
    async musicTimeChanged(e) {
      // 停止定时器
      clearTimeout(this.timerId);
      this.timerId = 0;

      console.log("music_play musicTimeChanged:"+"musice time is " + e);
      this.musicTime = e;
      await smta.seek(e * 1000);
    },

    next() {
      this.cur_num = (this.cur_num + 1) % 3;
      smta.play(urlList[this.cur_num], 0);
    },
  },
  beforeDestroy() {
    $falcon.trigger("off_event",this.events);
  },
  destroyed() {
    console.log("music_play destroyed");
    this.$page.off("hide", this.onPageHide);
    this.$page.off("show", this.onPageShow);
    clearTimeout(this.timerId);
  },
};
</script>

<style lang="less" scoped>
@import "base.less";
</style>
