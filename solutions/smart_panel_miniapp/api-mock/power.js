/*
 * jsapi扩展mock
 */
//(事件，注册函数数组)
let eventFunc = {
  batteryChange: [],
  charge: [],
  batteryLow: [],
  batteryEmergency: []
};
//注册函数数组和记号数组一一对应
let eventToken = {
  batteryChange: [],
  charge: [],
  batteryLow: [],
  batteryEmergency: []
};

//(事件，返回值数组)
let eventRet = {
  batteryChange:
    [{
      "bssid": "9e:f6:21:64:9a:23",
      "isEncrypt": false,
      "rssi": -7,
      "ssid": "wangchuan",
      "status": charge
    },
    {
      "bssid": "9e:f6:21:64:9a:23",
      "isEncrypt": false,
      "rssi": -7,
      "ssid": "ZhangSan",
      "status": charge
    }],
  charge:
  {

  },
  batteryLow: null,
  batteryEmergency: null
};

let powerCtrl={
    autoHibernate :false,   //闲置休眠开关 true: 启用；false:禁用
    hibernateTimeout:60, //闲置时长 （单位：秒）
    batteryPercent:80,   //当前剩余电量
    isCharging:false       //电池状态   true:正在充电中；false: 未充电
}
export default {
  on(event, callback) {
    console.log('mock power.on' + " event:" + event);
    let token = 0;
    if (!eventFunc[event].includes(callback)) {
      eventFunc[event].push(callback);
      token = new Date().getTime();
      eventToken[event].push(token);
    }
    return token;
  },

  off(event, token) {
    console.log('mock power.off' + " event:" + event);
    let index = eventToken[event].indexOf(token)
    if (index != -1) {
      eventFunc[event].splice(index, 1);
      eventToken[event].splice(index, 1)
    }
  },

  getInfo() {
    console.log('mock power.getInfo');
    return powerCtrl
  },

  setAutoHibernate(isOn) {
    console.log('mock power.setAutoHibernate');
  },

  setHibernateTime(time) {
    console.log('mock power.setHibernateTime');
  },

  shutdown() {
    console.log('mock power.shutdown');
  },

  reboot() {
    console.log('mock power.reboot');
  }
}
