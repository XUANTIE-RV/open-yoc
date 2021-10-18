# YoC

## Introduction

"Yun", aka cloud, On Chip (YoC) is a basic software platform, based on AliOS Things. It provides developers with a unified low-level CSI interface. It also provides components optimized for bluetooth, WiFi, voice, vision applications and etc. One can run the integrated development environment (IDE), Jianchi CDK, and use system performance analysis tool to debug, develop, and deploy functional component building blocks. It facilitates SoC companies and developers to quickly deliver customized SDK, and significantly reduces time-to-market.


## RVB2601 Development Board

RVB2601 is a development board based on the T-Head eco-chip CH2601, onboard JTAG debugger, WiFi&BLE chip W800, audio ADC ES7210, audio DAC ES8156, 128x64 OLED screen, RGB tri-color indicator, user buttons. For specific development board hardware specifications and purchase information, please go to: [RVB2601 development board](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.14300867.0.0.681c1f9cxK233N&id=3886757103532519424&vendorId=3706716635429273600&module=4).


## Development Board Demo

### Get started quickly

Please refer to the RVB2601 development board user guide: [RVB2601 user guide](https://occ.t-head.cn/community/post/detail?spm=a2cl5.14300867.0.0.484c180fZPgL3q&id=3887117894232449024)


### Demo list

Download the code before using the demo. 

```bash
git clone git@github.com:T-head-Semi/yoc-open.git
```

Then go to the `solutions` directory, all the demos are presented there, and you can use them according to the `README.md` file under each demo.


- [ch2601_helloworld](https://github.com/T-head-Semi/yoc-open/tree/master/solutions/ch2601_helloworld) demonstrates the minimal system Helloworld example of the RVB2601 development board.

- [ch2601_gui_demo](https://github.com/T-head-Semi/yoc-open/tree/master/solutions/ch2601_gui_demo) provides a development demonstration of the TFT LCD screen.

- [ch2601_marquee_demo](https://github.com/T-head-Semi/yoc-open/tree/master/solutions/ch2601_marquee_demo) demonstrates the tri-color marquee effect by controlling an RGB LED.

- [ch2601_fota_demo](https://github.com/T-head-Semi/yoc-open/tree/master/solutions/ch2601_fota_demo) demonstrates FOTA upgrade. FOTA's cloud service is in [OCC](https://occ.t-head.cn/).

- [ch2601_player_demo](https://github.com/T-head-Semi/yoc-open/tree/master/solutions/ch2601_player_demo) demonstrates offline audio playback. The audio file is stored in the FLASH of the device.

- [ch2601_freertos_demo](https://github.com/T-head-Semi/yoc-open/tree/master/solutions/ch2601_freertos_demo) demonstrates FreeRTOS system functions based on E906.

  

## Resource Reference

- T-Head Open Chip Community: https://occ.t-head.cn/

- The yocbook reference manual: [yocbook](https://yoc.docs.t-head.cn/yocbook/Chapter1-YoC%E6%A6%82%E8%BF%B0/)

- RVM-CSI Hardware Abstraction Layer Standard Interface Manual: [RVM-CSI Manual](https://occ.t-head.cn/document?temp=CSI&slug=csi-chn)

- RVB2601 technical blog post: [RVB2601](https://occ.t-head.cn/vendor/detail/post?spm=a2cl5.14300867.0.0.681c1f9cxK233N&id=3886757103532519424&vendorId=3706716635429273600&module=4#sticky)