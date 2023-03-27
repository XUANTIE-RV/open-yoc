import Back from './components/back.vue';

export class BasePage extends $falcon.Page {
  constructor() {
    super()
  }
  beforeVueInstantiate(Vue) {
    Vue.component('Back', Back);
  }
}