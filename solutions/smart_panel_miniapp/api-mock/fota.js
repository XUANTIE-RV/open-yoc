/*
 * jsapi扩展mock
 */
//(事件，注册函数数组)
let eventFunc = {
  ready: [],
  version: [],
  download: [],
  end: [],
  restart: [],
};
let eventToken = {
  ready: [],
  version: [],
  download: [],
  end: [],
  restart: [],
};
//(事件，返回值数组)
let eventRet = {
  ready: null,
  //下载成功后，curversion==newversion
  version:
  {
    code: 0,
    curversion: "1.0.0",
    newversion: "1.0.1",
    //newversion: "1.0.0",
    deviceid: "9527",
    model: "1234",
    local_changelog: "history log",
    changelog: "new log",
    msg: ""
  },
  //定时改变进度
  download:
  {
    code: 0,
    cur_size: 0,
    total_size: 100,
    percent: 0,
    speed: 10,
    msg: ""
  },
  end:
  {
    code: 0,
    msg: "download successfully"
  },
  restart:
  {
    code: 0,
    msg: "restart successfully"
  },
};

let versionCtrl = {
  curversion: "1.0.0",
  newversion: "1.0.1",
  updating: false,
  ready: false,
  start: false,
  versionChecked: false,
  state: "idle",
}
let t = setInterval(updateDownload, 1000)
clearInterval(t)

function updateDownload() {
  if (eventRet.download.cur_size == 90) {
    versionCtrl.state = "finish"
    clearInterval(t);
    for (let i = 0; i < eventFunc.end.length; i++) {
      eventFunc.end[i](eventRet.end);
    }
  }
  else {
    eventRet.download.cur_size += 10;
    eventRet.download.percent += 10;
    for (let i = 0; i < eventFunc.download.length; i++) {
      eventFunc.download[i](eventRet.download);
    }
  }
}

export default {
  on(event, callback) {
    console.log('mock fota.on' + " event:" + event);
    let token = 0;
    if (event == "ready"){
      versionCtrl.ready = true;
      setTimeout(() => {callback();}, 2000);
    }
    else if (!eventFunc[event].includes(callback)) {
      eventFunc[event].push(callback);
      token = new Date().getTime()
      eventToken[event].push(token);
    }
    return token;
  },

  off(event, token) {
    console.log('mock fota.off' + " event:" + event);
    let index = eventToken[event].indexOf(token)
    if (index != -1) {
      eventFunc[event].splice(index, 1)
      eventToken[event].splice(index, 1)
    }
  },

  start() {
    console.log('mock fota.start');
    if (!versionCtrl.ready) return;
    versionCtrl.start = true;
    versionCtrl.state = "start"
  },

  stop() {
    console.log('mock fota.stop');
    if (!versionCtrl.ready) return;
    versionCtrl.start = false;
    versionCtrl.state = "idle"
  },

  getState() {
    console.log('mock fota.getState');
    if (!versionCtrl.ready || !versionCtrl.start) return;
    return versionCtrl.state;
  },

  versionCheck() {
    console.log('mock fota.versionCheck');
    if (!versionCtrl.ready) return;
    versionCtrl.versionChecked = true;
    for (let i = 0; i < eventFunc.version.length; i++) {
      eventFunc.version[i](eventRet.version);
    }
  },

  download() {
    console.log('mock fota.download'+versionCtrl.ready+versionCtrl.start+versionCtrl.versionChecked);
    if (!versionCtrl.ready || !versionCtrl.start || !versionCtrl.versionChecked) return;
    versionCtrl.state = "download"
    eventRet.download.cur_size = 0;
    eventRet.download.percent = 0;
    t = setInterval(updateDownload, 1000);
  },

  restart(delayms) {
    console.log('mock fota.restart');
    if (!versionCtrl.ready) return;
    setTimeout(() => {
      for (let i = 0; i < eventFunc.restart.length; i++) {
        eventFunc.restart[i](eventRet.restart);
      }
    }, delayms);
  }
}
