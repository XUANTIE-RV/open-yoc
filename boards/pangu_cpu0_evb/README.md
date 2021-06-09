# 概述

`pangu_cpu0_evb` board配置

configs目录下有带tee的config.yaml配置文件

configs2目录下有不带tee的config.yaml配置文件

通过修改package.yaml文件里的export域来切换tee和非tee配置。

带tee配置：
```yaml
export:
  - dest: "<SOLUTION_PATH>/generated/data"
    source:
      - "bootimgs/boot"
      - "bootimgs/boot-notee"
      - "bootimgs/tee"
      - "bootimgs/cpu1"
      - "bootimgs/cpu2"
      - "bootimgs/lpm"
      - "bootimgs/kp"
      - "configs/config.yaml"
```

不带tee的配置：
```yaml
export:
  - dest: "<SOLUTION_PATH>/generated/data"
    source:
      - "bootimgs/boot"
      - "bootimgs/boot-notee"
      - "bootimgs/tee"
      - "bootimgs/cpu1"
      - "bootimgs/cpu2"
      - "bootimgs/lpm"
      - "bootimgs/kp"
      - "configs2/config.yaml"
```