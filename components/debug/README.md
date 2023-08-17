## Contents

## Introduction
**debug** debug files of AliOS Things

### Features
- coredump and debug cmds support

### Directories

```sh
debug
├─ debug_backtrace   # show stack backtrace when panic
├─ debug_overview    # show task\heap\memory status when panic
├─ debug_panic       # debug core
├─ debug_dft_config  # debug default config
├─ debug_cpuusage    # cpuusage feature
├─ include/debug_api # debug api for same layer
```
### Dependencies

## Reference

## other

组件中依赖的相关宏有：
CONFIG_KERNEL_RHINO
SYSINFO_DEVICE_NAME
RHINO_CONFIG_SYS_STATS
RHINO_CONFIG_KOBJ_LIST
RHINO_CONFIG_CPU_NUM
DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE
RHINO_CONFIG_MM_DEBUG
RHINO_CONFIG_CPU_STACK_DOWN
DEBUG_LAST_WORD_ENABLE
CONFIG_VFS_LSOPEN
K_MM_STATISTIC
RHINO_CONFIG_MM_BLK
RHINO_CONFIG_MM_TLF
RHINO_CONFIG_BUF_QUEUE
RHINO_CONFIG_QUEUE
RHINO_CONFIG_SEM
AOS_COMP_CLI
RHINO_CONFIG_MM_TRACE_LVL
DEBUG_ULOG_FLUSH
AOS_COMP_ULOG
