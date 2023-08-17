/*
 * jsapi扩展mock
 */
//(事件，注册函数数组)
let eventFunc = {
  status: [],
};
//注册函数数组和记号数组一一对应
let eventToken = {
  status: [],
};

//(事件，返回值数组)
let eventRet = {
  status: { isOn: true }
};

let screenCtrl = {
  isOn: true,  // 屏幕显示状态 true: 亮屏；false:息屏
  isAutoBrightness: false, //  自动调整屏幕亮度  true:启用；false: 禁用
  brightness: 88, // 当前屏幕亮度
  isAutoOff: false,  //自动息屏   true:启用；false: 禁用
  autoOffTimeout: 30  // 自动息屏时长 （单位：秒）
}

let t = 0;
export default {
  on(event, callback) {
    console.log('mock screen.on' + " event:" + event);
    let token = 0;
    if (!eventFunc[event].includes(callback)) {
      eventFunc[event].push(callback);
      token = new Date().getTime();
      eventToken[event].push(token);
    }
    return token;
  },

  off(event, token) {
    console.log('mock screen.off' + " event:" + event);
    let index = eventToken[event].indexOf(token)
    if (index != -1) {
      eventFunc[event].splice(index, 1);
      eventToken[event].splice(index, 1)
    }
  },

  getInfo() {
    console.log('mock screen.getInfo');
    return new Promise((resolve, reject) => {
      resolve(screenCtrl);
    })
  },

  turnOn() {
    console.log('mock screen.turnOn');
    screenCtrl.isOn = true;
    for (let i = 0; i < eventFunc.status.length; i++) {
      eventRet.status.isOn = true;
      eventFunc.status[i](eventRet.status);
    }
    if (screen.isAutoOff) {
      clearTimeout(t);
      t = setTimeout(() => {
        screenCtrl.isOn = false;
        for (let i = 0; i < eventFunc.status.length; i++) {
          eventRet.status.isOn = false;
          eventFunc.status[i](eventRet.status);
        }
      }, screenCtrl.autoOffTimeout * 1000);
    }
  },

  turnOff() {
    console.log('mock screen.turnOff');
    screenCtrl.isOn = false;
    for (let i = 0; i < eventFunc.status.length; i++) {
      eventRet.status.isOn = false;
      eventFunc.status[i](eventRet.status);
    }
  },

  setAutoBrightness(isAuto) {
    console.log('mock screen.setAutoBrightness');
    screenCtrl.isAutoBrightness = isAuto
  },

  setBrightness(percent) {
    console.log('mock screen.setBrightness');
    screenCtrl.brightness = percent;
    return new Promise((resolve, reject) => {
      resolve("success");
    })
  },

  setAutoOff(isAuto) {
    console.log('mock screen.setAutoOff');
    screenCtrl.isAutoOff = isAuto;
    if (isAuto) {
      clearTimeout(t);
      t = setTimeout(() => {
        screenCtrl.isOn = false;
        for (let i = 0; i < eventFunc.status.length; i++) {
          eventRet.status.isOn = false;
          eventFunc.status[i](eventRet.status);
        }
      }, screenCtrl.autoOffTimeout * 1000);
    }
  },

  setAutoOffTimeout(time) {
    console.log('mock screen.setAutoOffTimeout');
    screenCtrl.autoOffTimeout = time;
    return new Promise((resolve, reject) => {
      resolve("success");
    })
  }
}
