/*
 * jsapi扩展mock
 */
let keyValue = []
export default {
  setItem(keyStr, valueStr) {
    console.log('mock kv.setItem '+keyStr+" "+valueStr);
    return new Promise((resolve, reject) => {

      for (let i = 0; i < keyValue.length; i++) {
        if (keyValue[i].key == keyStr) {
          keyValue[i].value = valueStr;
          resolve("success");
        }
      }
      keyValue.push({ key: keyStr, value: valueStr });
      resolve("success");
    })
  },

  getItem(keyStr) {
    console.log('mock kv.getItem '+keyStr);
    for (let i = 0; i < keyValue.length; i++) {
      if (keyValue[i].key == keyStr)
        return keyValue[i].value;
    }
    return "undefined";
  },

  removeItem(keyStr) {
    console.log('mock kv.removeItem '+keyStr);
    let index = keyValue.indexOf(key);
    if (index != -1)
      keyValue.splice(index, 1);

    return new Promise((resolve, reject) => {
      for (let i = 0; i < keyValue.length; i++) {
        if (keyValue[i].key == keyStr) {
          keyValue.splice(index, 1);
          resolve("success");
        }
      }
      resolve("failure");
    })
  },

  clear() {
    console.log('mock kv.clear');
    keyValue.splice(0, keyValue.length);
    return new Promise((resolve, reject) => {
      resolve("success");
    })
  }
}
