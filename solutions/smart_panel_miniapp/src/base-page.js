// import DemoScroller from "./components/demo-scroller.vue";
// import YxpKeyboard from "./components/keyboard.vue";
import YxpKeyboard from "./components/keyboard_2.vue";
// import http from 'http';
// import smta from "smta";
// import power from "power";
// import screen from "screen";
// import kv from "system_kv";
// import voice from "voice";
// import fota from "fota";
// import wifi from "wifi";
import gw from "gateway";

export class BasePage extends $falcon.Page {
  constructor() {
    super()
  }
  beforeVueInstantiate(Vue) {
    // Vue.component('DemoScroller', DemoScroller);
    Vue.component('gw', gw);
    Vue.component('YxpKeyboard', YxpKeyboard);

    // vue.component('wifi', wifi);
  }
}
