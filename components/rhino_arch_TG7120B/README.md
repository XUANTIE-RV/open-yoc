## 概述
arch for rhino-kernel with task switch patch for TG7120B 
supported CPU code here. It includes CPU architecture's shcedule codes.

```sh
.
├── include
│   ├── k_types.h
│   └── port.h
├── package.yaml
├── README.md
└── src
    ├── arm
    │   ├── m0
    │   ├── m3
    │   └── m4
    ├── csky
    │   ├── 802
    │   ├── 803
    │   ├── 803ef
    │   ├── 804
    │   ├── 805
    │   ├── 807
    │   ├── 810
    │   ├── ck610
    │   └── ck610f
    └── riscv
        ├── rv32_16gpr
        ├── rv32_32gpr
        ├── rv32f_32gpr
        └── rv32fd_32gpr
```
## 组件安装

```bash
yoc init
yoc install rhino_arch_TG7120B
```

## 运行资源

不同方案下资源消耗参考

| Solution              | Code size/Bytes | Ro data/Bytes | RW Data/Bytes | BSS/Bytes |
| ---------------------------- | --------------- | ------------- | ------------- | --------- |
| genie_mesh_light_ctl  | 260             | 0             | 0             | 0         |
| genie_mesh_node_ctl   | 260             | 0             | 0             | 0         |
| genie_mesh_switches   | 260             | 0             | 0             | 0         |

## 依赖资源
无