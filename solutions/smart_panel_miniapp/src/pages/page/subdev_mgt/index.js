import IndexComponent from './index.vue';

class PageSubdev extends $falcon.Page {
  /**
   * 构造函数,页面生命周期内只执行一次
   */
  constructor() {
    super();
  }

  /**
   * 页面生命周期:首次启动
   * @param {Object} options 页面启动参数
   */
  onLoad(options) {
    super.onLoad(options);
    this.setRootComponent(IndexComponent);
  }

  /**
   * 页面生命周期:页面重新进入
   * 其他应用或者系统通过$falcon.navTo()方法重新启动页面.可以通过这个回调拿到新启动的参数
   * @param {Object}} options 重新启动参数
   */
  onNewOptions(options) {
    super.onNewOptions(options);
    console.log("subdev_mgt onNewOptions:"+"enter mesh again");
  }

  /**
   * 页面生命周期:页面进入前台
   */
  onShow() {
    super.onShow();

    // 接口调用示例
    // testModule.mockMethod('test');
    // DemoApi.testApi({params:1});

    //onshow以后组件才创建,可以调用组件的方法
    // this.$root.sayHello();
    console.log("subdev_mgt onShow");
    // let res = await fota.start();
    // /*异步函数，需要await等待返回值*/
    //console.log("subdev_mgt onShow:"+"StartFota res=" + res);
  }
  
  /**
   * 页面生命周期:页面进入后台
   */
  onHide() {
    console.log("subdev_mgt onHide");
  }

  /**
   * 页面生命周期:页面卸载
   */
  async onUnload() {
    console.log("subdev_mgt onUnload");
  }

}
export default PageSubdev;