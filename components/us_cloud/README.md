# 概述

该组件是集成云知声在线ASR tts功能的组件，提供了create start acquist(送入录音) stop destroy等接口；

创建时用到的配置信息在当前目录下的res/config_file有示例，开发时请将其放入文件系统中，并记录下路径以及文件名，在RasrCreate时会用到该信息;
里面的appkey和seckey仅用于开发使用，商业化需要向云知声信息技术有限公司重新申请appkey和seckey,
获得授权
