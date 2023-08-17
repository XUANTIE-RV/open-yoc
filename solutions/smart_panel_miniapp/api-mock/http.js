/*
 * jsapi扩展mock
 */
export default {
  on(event, callback) {
    console.log('mock http.on' + " event:" + event);
  },

  off(event, token) {
    console.log('mock http.off' + " event:" + event);
  },

  request(obj) {
    console.log('mock http.request');
    return new Promise((resolve, reject) => {
      // some asynchronous operation
      if (true) {
        resolve("http request return");
      } else {
        reject("http request error");
      }
    })
  },

  download(obj) {
    console.log('mock http.download');
    return new Promise((resolve, reject) => {
      // some asynchronous operation
      if (true) {
        resolve("http download return");
      } else {
        reject("http download error");
      }
    })
  }
}
