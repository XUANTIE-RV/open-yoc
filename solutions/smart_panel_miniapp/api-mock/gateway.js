/*
 * jsapi扩展mock
 */
let devList =
  [
    //测试入网
    /*
    {
      dev_addr: "00:00:00:00:00:00",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "11:11:11:11:11:11",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "22:22:22:22:22:22",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    */
    //测试子设备控制
    
    {
      dev_addr: "00:00:00:00:00:00",
      isOpen: false,
      brightness: 10,
      inNet: true,
      //subDevMgmt
      dev_name: "light1",
      roomType: "LIVING_ROOM",
      deviceType: "OBJ_LIGHT",
    },
    {
      dev_addr: "11:11:11:11:11:11",
      isOpen: false,
      brightness: 10,
      inNet: true,
      //subDevMgmt
      dev_name: "screen1",
      roomType: "BED_ROOM",
      deviceType: "OBJ_SCREEN",
    },
    {
      dev_addr: "22:22:22:22:22:22",
      isOpen: false,
      brightness: 10,
      inNet: true,
      //subDevMgmt
      dev_name: "switch1",
      roomType: "BED_ROOM",
      deviceType: "OBJ_SWITCH",
    },
    
    {
      dev_addr: "33:33:33:33:33:33",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "44:44:44:44:44:44",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "55:55:55:55:55:55",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },

    {
      dev_addr: "66:66:66:66:66:66",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "77:77:77:77:77:77",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "88:88:88:88:88:88",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "99:99:99:99:99:99",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "AA:AA:AA:AA:AA:AA",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "BB:BB:BB:BB:BB:BB",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "CC:CC:CC:CC:CC:CC",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "DD:DD:DD:DD:DD:DD",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    },
    {
      dev_addr: "EE:EE:EE:EE:EE:EE",
      isOpen: false,
      brightness: 10,
      inNet: false,
      //subDevMgmt
      dev_name: "",
      roomType: "",
      deviceType: "",
    }
  ];

let eventFunc = {
  ProvShowDev: [],
  AddNodeRst: [],
  DelNodeRst: [],
  NodeStatusRpt: [],
  jsapi_miniapp_init_finish:[]

};
//注册函数数组和记号数组一一对应
let eventToken = {
  ProvShowDev: [],
  AddNodeRst: [],
  DelNodeRst: [],
  NodeStatusRpt: [],
  jsapi_miniapp_init_finish:[]
};

//(事件，返回值数组)
let eventRet = {
  AddNodeRst: { dev_addr: "AA:AA:AA:AA:AA:AA", rst_code: 0 },
  DelNodeRst: { dev_addr: "AA:AA:AA:AA:AA:AA", rst_code: 0 },
  NodeStatusRpt: { dev_addr: "AA:AA:AA:AA:AA:AA", dev_name: "ligh1", dev_state:1},
  jsapi_miniapp_init_finish:{}
};
let devDelTime=300;

export default {
  //测试疯狂点击首页按钮
  //devList,
  on(event, callback) {
    console.log('mock gateway.on' + " event:" + event);
    let token = 0;
    if (!eventFunc[event].includes(callback)) {
      eventFunc[event].push(callback);
      //根据callback生成token:1 函数自身有一个objectId 2函数的内容转为string 3根据绑定的时间生成token
      token = new Date().getTime()
      eventToken[event].push(token);
    }
    if (event == "jsapi_miniapp_init_finish")
    setTimeout(() => { callback();}, 1000)
    return token;
  },

  off(event, token) {
    console.log('mock gateway.off' + " event:" + event);
    let index = eventToken[event].indexOf(token)
    if (index != -1) {
      eventFunc[event].splice(index, 1);
      eventToken[event].splice(index, 1)
    }
  },

  subDevCtrl(request, mac) {
    console.log('mock gateway.subDevCtrl'+request.operands+' '+request.operator);
    let changeFlag=0;
    for (let i = 0; i < devList.length; i++) {
      if (devList[i].dev_name==request.operands) {
        if(request.operator=="ACT_OPEN"){
          if(!devList[i].isOpen) changeFlag=1;
          devList[i].isOpen=true;
        }
        else if(request.operator=="ACT_CLOSE"){
          if(devList[i].isOpen) changeFlag=1;
          devList[i].isOpen=false;

        }
        else if(request.operator=="ACT_BRIVALUE"){
          devList[i].brightness=request.value;
        }
        if(changeFlag){
          for (let j = 0; j < eventFunc.NodeStatusRpt.length; j++) {
            eventRet.NodeStatusRpt.dev_addr = mac;
            eventRet.NodeStatusRpt.dev_name = request.operands;
            eventRet.NodeStatusRpt.dev_state = devList[i].isOpen ? 1 : 0;
            eventFunc.NodeStatusRpt[j](eventRet.NodeStatusRpt);
          }
        } 
        break; 
      }
    }
  },

  grpDevCtrl(request, group) {
    console.log('mock gateway.grpDevCtrl');
    let changeFlag=0;
    for (let i = 0; i < devList.length; i++){
      if(devList[i].deviceType==group && devList[i].inNet){
        changeFlag=0;
        if(request.operate=="ACT_OPEN"){
          if(!devList[i].isOpen) changeFlag=1;
          devList[i].isOpen=true;
        }
        else if(request.operate=="ACT_CLOSE"){
          if(devList[i].isOpen) changeFlag=1;
          devList[i].isOpen=false;

        }
        else if(request.operate=="ACT_BRIVALUE"){
          devList[i].brightness=request.value;
        }
        if(changeFlag){
          for (let j = 0; j < eventFunc.NodeStatusRpt.length; j++) {
            eventRet.NodeStatusRpt.dev_addr = devList[i].dev_addr;
            eventRet.NodeStatusRpt.dev_name = devList[i].dev_name;
            eventRet.NodeStatusRpt.dev_state = devList[i].isOpen ? 1 : 0;
            eventFunc.NodeStatusRpt[j](eventRet.NodeStatusRpt);
          }
        } 
      }
    }
  },

  meshCtrl(request, mac) {
    console.log('mock gateway.addConfig' + request.operator + mac);
    if (request.operator == "ACT_DEVSHOW") {
      setTimeout(() => {
        let newDeviceList = []
        for (let i = 0; i < devList.length; i++) {
          if (!devList[i].inNet) {
            let newDevice = { dev_addr: devList[i].dev_addr };
            newDeviceList.push(newDevice); //push(对象的引用)，所以let必须放在快内，是一个新的对象
          }
        }
        for (let i = 0; i < eventFunc.ProvShowDev.length; i++) {
          eventFunc.ProvShowDev[i](newDeviceList);
        }
      }, 500);
    }
    else if (request.operator == "ACT_DEVADD") {
      if (mac == "FF:FF:FF:FF:FF:FF") {
        setTimeout(() => {
          for (let i = 0; i < devList.length; i++) {
            devList[i].inNet = true;
          }
        }, 500);
      }
      else {
        setTimeout(() => {
          for (let i = 0; i < devList.length; i++) {
            if (devList[i].dev_addr == mac) {
              devList[i].inNet = true;
              break;
            }
          }
          for (let i = 0; i < eventFunc.AddNodeRst.length; i++) {
            eventRet.AddNodeRst.dev_addr = mac;
            eventFunc.AddNodeRst[i](eventRet.AddNodeRst);
          }
        }, 500);
      }

    }
    else if (request.operator == "ACT_DEVDEL") {
      if (mac == "FF:FF:FF:FF:FF:FF") {
        setTimeout(() => {
          for (let i = 0; i < devList.length; i++) {
            devList[i].inNet = false;
          }
        }, 500);
      }
      else {
        for (let i = 0; i < devList.length; i++) {
          if (devList[i].dev_addr == mac) {
            devList[i].inNet = false;
            break;
          }
        }
        if(devDelTime==300)
          devDelTime=700;
        else
          devDelTime=300;
        setTimeout(() => {
          for (let i = 0; i < eventFunc.DelNodeRst.length; i++) {
            eventRet.DelNodeRst.dev_addr = mac;
            eventFunc.DelNodeRst[i](eventRet.DelNodeRst);
          }
        }, devDelTime);
      }
    }
  },

  subDevMgmt(request, mac) {
    console.log('mock gateway.removeConfig');
    for (let i = 0; i < devList.length; i++) {
      if (devList[i].dev_addr == mac) {
        devList[i].dev_name=request.operator;
        devList[i].roomType=request.roomType;
        devList[i].deviceType=request.deviceType;
        break;
      }
    }
  },

  nodeListGet(request, cb) {
    console.log('mock gateway.listConfig');
    let nodeList = []
    for (let i = 0; i < devList.length; i++) {
      if (devList[i].inNet) {
        let node = { dev_name: devList[i].dev_name, dev_addr: devList[i].dev_addr }
        nodeList.push(node)
      }
    };
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve(nodeList)
      }, 500);
    })
  }
}
