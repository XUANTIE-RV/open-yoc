<template>
  <div class="wrapper">
    <div class="header">
      <back />
    </div>
    <text class="greeting">Module -- jsapi hello </text>

    <div class="content">
      <text class="jump-message" @click="joinPath()">joinPath sync test</text>
      <text class="jump-message" @click="readFile()">readFile async test</text>
      <text class="jump-message" @click="scanWiFi()">scanWiFi publish test</text>
    </div>

    <div>
      <text class="message">输出信息: {{ outputMessage }}</text>
    </div>
  </div>
</template>

<script>
import back from "../../components/back.vue";
import { foo, fooWifi } from 'hello';

export default {
  components: { back },
  name: "hello",
  data() {
    return {
      outputMessage: "",
    };
  },
  methods: {
    // 同步调用示例
    joinPath() {
      const content = foo.joinPath('root', 'works', 'project');
      this.outputMessage = `joinPath result ${content}`;
    },

    // 异步调用示例
    async readFile() {
      const content = await foo.readFile('/some/path/of/file');
      //console.log(`readFile result ${content}`);
      this.outputMessage = `readFile result ${content}`;
    },

    // 消息推送
    scanWiFi() {
      fooWifi.on("scan_result", (result) => {
        this.outputMessage = result;
      });
      fooWifi.scanWifi();
    }
  },
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";

.wrapper {
  flex: 1;
}

.greeting {
  text-align: center;
  margin-top: 20px;
  font-size: 50px;
  color: #acccbd;
}

.content {
  // justify-content: left;
  flex-direction: row;
  flex-wrap: wrap;
}

.jump-message {
  margin: 5px;
  font-size: 24px;
  color: white;
  background-color: rgb(15, 29, 155);
  padding: 5px;
  border-radius: 3px;
}

.message {
  font-size: 20px;
  color: black;
  margin: 30px;
}

.header {
  flex-direction: row;
  // justify-content: between;
  margin: 10px;
  align-items: center;
}</style>
