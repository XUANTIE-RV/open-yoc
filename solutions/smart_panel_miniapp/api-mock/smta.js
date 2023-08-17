/*
 * jsapi扩展mock
 */
//(事件，注册函数数组)
let eventFunc = {
  error: [],
  start: [],
  pause: [],
  resume: [],
  finish: [],
  stop: [],
  vol_change: [],
  mute: []
};
//注册函数数组和记号数组一一对应
let eventToken = {
  error: [],
  start: [],
  pause: [],
  resume: [],
  finish: [],
  stop: [],
  vol_change: [],
  mute: []
};
//current play url file:///mnt/alibaba3.mp3	68205
let musicList = [
  {
    url: "file:///mnt/alibaba1.mp3",
    duration: 20000
  },
  {
    url: "file:///mnt/alibaba2.mp3",
    duration: 40000
  },
  {
    url: "file:///mnt/alibaba3.mp3",
    duration: 68205
  },
];
let onlineMusic={
  url:"",
  duration: 66000
};
let playCtrl = {
  curNum: 0,//0,1,2对应3个mp3 ,-1在放在线音乐
  curVolumn: 10,//当前音量，进入播放器的默认音量
  volumnBak: 10,//过去音量
  status: 0, //    UNKNOWN = 0,STOPED,PLAYING,PAUSED
  curTime: 0,//ms
  speed: 1, //0.5~1.5  1:正常播放，0.5:慢速播放
  curUrl:"",
}
//定时器，播放开始后，定时更新curTime；前台一秒更新一次时间，后台根据播放速度设置定时器的值
//0.5慢速时，相当于定时器过了500ms，音乐已经放了1000ms了。
let t = 0;

function updateMusicTime() {
  let duration=0;
  if(playCtrl.curNum==-1){
    duration=onlineMusic.duration;
  }
  else{
    duration=musicList[playCtrl.curNum].duration;
  }
  if (playCtrl.curTime + 1000 >= duration) {
    playCtrl.curTime += 1000;
    playCtrl.state = 1
    clearInterval(t);
    for (let i = 0; i < eventFunc.finish.length; i++) {
      eventFunc.stop[i]();
      eventFunc.finish[i]();
    }
  }
  else {
    playCtrl.curTime += 1000;
  }
}

export default {
  on(event, callback) {
    console.log('mock smta.on' + " event:" + event);
    let token = 0;
    if (!eventFunc[event].includes(callback)) {
      eventFunc[event].push(callback);
      //根据callback生成token:1 函数自身有一个objectId 2函数的内容转为string 3根据绑定的时间生成token
      token = new Date().getTime()
      eventToken[event].push(token);
    }
    return token;
  },

  off(event, token) {
    console.log('mock smta.off' + " event:" + event);
    let index = eventToken[event].indexOf(token)
    if (index != -1) {
      eventFunc[event].splice(index, 1);
      eventToken[event].splice(index, 1)
    }
  },

  play(url, type) {
    console.log('mock smta.play '+url);
    playCtrl.curUrl=url;
    let i=0;
    for(;i<musicList.length;i++){
      if(musicList[i].url==url){
        playCtrl.curNum=i;
        break;
      }
    }
    if(i==musicList.length){
      playCtrl.curNum=-1;
      onlineMusic.url=url;
    }
    playCtrl.status = 2;
    playCtrl.curTime=0;
    clearInterval(t);
    t = setInterval(updateMusicTime, 1000 * playCtrl.speed)
    for (let i = 0; i < eventFunc.start.length; i++) {
      eventFunc.start[i]();
    }
  },

  stop() {
    console.log('mock smta.stop');
    playCtrl.status = 1;
    playCtrl.curTime = 0;
    clearInterval(t);
    for (let i = 0; i < eventFunc.stop.length; i++) {
      eventFunc.stop[i]();
      eventFunc.finish[i]();
    }
  },

  pause() {
    console.log('mock smta.pause');
    playCtrl.status = 3;
    clearInterval(t);
    for (let i = 0; i < eventFunc.pause.length; i++) {
      eventFunc.pause[i]();
    }
  },

  resume() {
    console.log('mock smta.resume');
    playCtrl.status = 2;
    clearInterval(t);
    t = setInterval(updateMusicTime, 1000 * playCtrl.speed);
    for (let i = 0; i < eventFunc.resume.length; i++) {
      eventFunc.resume[i]();
    }
    return new Promise((resolve, reject) => {
      resolve(playCtrl.curTime)
    })
  },
  resumeEnable(enable) {
    console.log('mock smta.resumeEnable');
  },

  mute() {
    console.log('mock smta.mute');
    playCtrl.volumnBak = playCtrl.curVolumn;
    playCtrl.curVolumn = 0;
    return new Promise((resolve, reject) => {
      resolve(playCtrl.curVolumn)
    })
  },

  unmute() {
    console.log('mock smta.unmute');
    playCtrl.curVolumn = playCtrl.volumnBak;
    playCtrl.volumnBak = 0;
    return new Promise((resolve, reject) => {
      resolve(playCtrl.curVolumn)
    })
  },

  getVol() {
    console.log('mock smta.getVol');
    return playCtrl.curVolumn;
  },

  setVol(vol) {
    console.log('mock smta.setVol');
    playCtrl.curVolumn = vol;
    return new Promise((resolve, reject) => {
      resolve(playCtrl.curVolumn)
    })
  },

  seek(seek_time) {
    console.log('mock smta.seek');
    let duration=0;
    if(playCtrl.curNum==-1){
      duration=onlineMusic.duration;
    }
    else{
      duration=musicList[playCtrl.curNum].duration;
    }
    //进度条传递过来的值一定<=当前音乐的时长，但还是判断一下，不然定时器永远停不了了
    if (seek_time <= duration) {
      playCtrl.curTime = seek_time;
      playCtrl.status = 2;
      clearInterval(t);
      t = setInterval(updateMusicTime, 1000 * playCtrl.speed);
      for (let i = 0; i < eventFunc.start.length; i++) {
        eventFunc.start[i]();
      }
    }
  },

  setSpeed(speed) {
    console.log('mock smta.setSpeed');
    playCtrl.speed = speed;
    return new Promise((resolve, reject) => {
      resolve(playCtrl.speed);
    })
  },

  getSpeed() {
    console.log('mock smta.getSpeed');
    return playCtrl.speed;
  },

  getStatus() {
    console.log('mock smta.getStatus');
    return playCtrl.status;
  },

  getCurTime() {
    console.log('mock smta.getCurTime');
    return playCtrl.curTime;
  },

  getUrl() {
    console.log('mock smta.getUrl');
    if(playCtrl.curNum==-1){
      return onlineMusic.url;
    }
    else
    {
      return musicList[playCtrl.curNum].url;
    }
  },

  getDuration() {
    console.log('mock smta.getDuration');
    if(playCtrl.curNum==-1){
      return onlineMusic.duration;
    }
    else
    {
      return musicList[playCtrl.curNum].duration;
    }
  }
}
