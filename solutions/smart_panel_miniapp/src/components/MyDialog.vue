<template>
    <fl-popup id="popup-id0" v-model="dlgShow" :handle-close="handlePopupClose" @dismiss="dismiss" :system="system">
      <div id="div-id1" class="fl-dialog-wrapper" :style="dialogStyle">
        <div id="div-id2" class="fl-dialog-title">
          <slot id="slot-id3" name="title">
            <text id="txt-id4" :class="'fl-dialog-title-text' + (isCenter ? ' fl-dialog-title-text-center' : '')">{{ title }}</text>
          </slot>
        </div>
        <div id="div-id5" class="fl-dialog-content">
          <slot>
            <text id="txt-id6" :class="'fl-dialog-content-text' + (isContentCenter ? ' fl-dialog-content-text-center' : '')">{{content}}</text>
          </slot>
        </div>
        <div id="div-id7" :class="'fl-dialog-footer' + (isCenter ? ' fl-dialog-footer-center' : '')">
          <slot id="id8" name="footer">
            <fl-button id="but-id9" class="fl-dialog-btn fl-dialog-btn-cancel" @click="cancelClick" v-if="isShowCancel">{{
              cancelText
            }}</fl-button>
            <fl-button
              id="but-id10"
              class="fl-dialog-btn fl-dialog-btn-confirm"
              type="primary"
              @click="confirmClick"
              v-if="isShowConfirm"
              >{{ confirmText }}</fl-button
            >
          </slot>
        </div>
      </div>
    </fl-popup>
  </template>

  <script>
  import { FlPopup, FlButton } from "falcon-ui";
  
  export default {
    components: { FlPopup, FlButton },
    model: {
      prop: "show",
      event: "change"
    },
    props: {
      system: {
        type: [Boolean, String],
        default: false
      },
      show: {
        type: [Boolean, String],
        default: false
      },
      title: {
        type: String,
        default: ""
      },
      content: {
        type: String,
        default: "内容",
      },
      center: {
        type: [Boolean, String],
        default: false
      },
      contentCenter: {
        type: [Boolean, String],
        default: false
      },
      handleClose: {
        type: Function
      },
      cancelText: {
        type: String,
        default: "取消"
      },
      confirmText: {
        type: String,
        default: "确认"
      },
      showCancel: {
        type: [Boolean, String],
        default: true
      },
      showConfirm: {
        type: [Boolean, String],
        default: true
      },
      handleCancelClick: {
        type: Function
      },
      handleConfirmClick: {
        type: Function
      },
      dialogStyle: {
        type: Object,
      },
    },
    data() {
      return {
        dlgShow: this.show
      };
    },
    watch: {
      show(val, old) {
        this.dlgShow = val;
      },
      dlgShow(val, old) {
        this.$emit("change", val);
        this.$emit(val ? "open" : "close");
      }
    },
    computed: {
      isCenter() {
        return this.center === true || this.center === "true";
      },
      isContentCenter() {
        return this.contentCenter === true || this.contentCenter === "true";
      },
      isShowCancel() {
        return this.showCancel === true || this.showCancel === "true";
      },
      isShowConfirm() {
        return this.showConfirm === true || this.showConfirm === "true";
      }
    },
    methods: {
      open() {
        if (!this.show || this.show === "false") {
          this.$emit("change", true);
        }
      },
      async close() {
        const ret = this.handleClose ? await this.handleClose() : false;
        if (ret !== true) {
          this.$emit("change", false);
        }
      },
      dismiss() {
        // 退出页面或者应用时被容器关闭了.仅限系统弹窗
        this.$emit("close");
      },
      handlePopupClose() {
        return true;
      },
      async cancelClick() {
        let ret = this.handleCancelClick ? await this.handleCancelClick() : false;
        if (ret !== true) {
          this.close();
        }
      },
      async confirmClick() {
        let ret = this.handleConfirmClick ? await this.handleConfirmClick() : false;
        if (ret !== true) {
          this.close();
        }
      }
    }
  };
  </script>
  
  <style lang="less" scoped>
  @type    : 'component';
  @element : 'dialog';
  
  
  .fl-dialog-wrapper {
    background-color: #232930;
    width: 95%;
    border-radius: 20px;
  }
  .fl-dialog-title {
    padding: 32px 32px 0;
  }
  .fl-dialog-title-text {
    color: #606266;
    font-size: 36px;
  }
  .fl-dialog-title-text-center {
    text-align: center;
  }
  .fl-dialog-content-text-center {
    text-align: center;
  }
  .fl-dialog-content {
    justify-content: center;
    padding: 32px;
    flex-grow: 1;
    &-text {
      color: #606266;
      font-size: 28px;
    }
  }
  .fl-dialog-footer {
    flex-direction: row;
    justify-content: flex-end;
    padding: 0 32px 32px;
  }
  .fl-dialog-footer-center {
    justify-content: center;
  }
  .fl-dialog-btn {
    padding: 0 20px;
    min-width: 80px;
    border-radius: 16px;
  }
  .fl-dialog-btn-cancel {
    margin-right: 20px;
    width: 326px;
  }
  .fl-dialog-btn-confirm {
    width: 326px;
  }
  
  </style>
  