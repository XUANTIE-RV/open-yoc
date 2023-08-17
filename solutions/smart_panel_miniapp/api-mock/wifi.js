/*
 * jsapi扩展mock
 */
//(事件，注册函数数组)
let eventFunc = {
  scan_result: [],
  disconnected: [],
  connected: []
};
//注册函数数组和记号数组一一对应
let eventToken = {
  scan_result: [],
  disconnected: [],
  connected: []
};

//(事件，返回值数组)
let eventRet = {

  scan_result:
    [{
      bssid: "9e:f6:21:64:9a:23",
      isEncrypt: false,
      rssi: -7,
      ssid: "wangchuan",
      status: "disconnected"
    },
    {
      bssid: "9e:f6:21:64:9a:23",
      isEncrypt: false,
      rssi: -7,
      ssid: "ZhangSan",
      status: "disconnected"
    }],
  disconnected:
  {
    reason: "connect_failed",
    bssid: "9e:f6:21:64:9a:23",
    ssid: "wangchuan",
  },
  connected:
  {
    reason: "connect_succeeded",
    bssid: "9e:f6:21:64:9a:23",
    ssid: "wangchuan"
  }
};

let wifiPassword =
{
  wangchuan: "12345678",
  ZhangSan: "87654321"
}
let wifiCfg = []


let wifiCtrl = {
  connected: false,
  ssid: ""
}

export default {
  on(event, callback) {
    console.log('mock wifi.on' + " event:" + event);
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
    console.log('mock wifi.off' + " event:" + event);
    let index = eventToken[event].indexOf(token)
    if (index != -1) {
      eventFunc[event].splice(index, 1);
      eventToken[event].splice(index, 1)
    }

  },

  scan() {
    console.log('mock wifi.scan');
    for (let i = 0; i < eventFunc.scan_result.length; i++) {
      eventFunc.scan_result[i](eventRet.scan_result);
    }
  },

  addConfig(obj) {
    console.log('mock wifi.addConfig');
    for (let i = 0; i < wifiCfg.length; i++) {
      if (wifiCfg[i].ssid == obj.ssid) {
        wifiCfg[i].psk = obj.psk;
        return;
      }
    }
    wifiCfg.push(obj);
  },

  removeConfig(ssid) {
    console.log('mock wifi.removeConfig');
    for (let i = 0; i < wifiCfg.length; i++) {
      if (wifiCfg[i].ssid == obj.ssid) {
        wifiCfg.splice(i, 1)
        return;
      }
    }

  },

  listConfig() {
    console.log('mock wifi.listConfig');
    return wifiCfg;
  },

  connect(ssid) {
    console.log('mock wifi.connect');
    for (let i = 0; i < wifiCfg.length; i++) {
      if (wifiCfg[i].ssid == ssid) {
        if (wifiCfg[i].psk == wifiPassword[ssid]) {
          wifiCtrl.connected = true;
          wifiCtrl.ssid = ssid;
          eventRet.connected.ssid = ssid;
          for (let i = 0; i < eventFunc.connected.length; i++) {
            eventFunc.connected[i](eventRet.connected);
          }
        }
        else {
          //连其他wifi失败，不会断开现在的wifi连接
          eventRet.disconnected.ssid = ssid;
          for (let i = 0; i < eventFunc.disconnected.length; i++) {
            eventFunc.disconnected[i](eventRet.disconnected);
          }

        }
      }
    }
  },

  disconnect() {
    console.log('mock wifi.disconnect');
    wifiCtrl.connected = false;
    eventRet.disconnected.ssid = wifiCtrl.ssid;
    for (let i = 0; i < eventFunc.disconnected.length; i++) {
      eventFunc.disconnected[i](eventRet.disconnected);
    }
  }
}
