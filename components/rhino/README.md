# 概述

rhino是AliOS Things提供的最小内核功能集，其中主要内容包括：
	任务(task)，任务管理和调度；
	定时器(timer)；
	工作队列(workqueue)； 
	系统时钟(system tick)； 
	信号量(semphore)； 
	互斥机制(mutex)； 
    消息队列(queue, buf queue)；
	环形缓冲池(ring buffer)； 
	事件机制(event)； 
	内存管理(memory management)；
	空闲任务(idle task) 

# 特性
- 支持完善的内核基本组件及特性；
- 支持多核、多任务、用户态内核态隔离等架构；
- 支持完善的任务维测、内存维测、异常处理机制。

# 目录
├── include
	├── k_api.h			  	# rhino统一对外头文件			
	├── k_default_config.h  # 内核默认配置
	├──
├── k_task.c			  	# 任务管理调度			
├── k_sem.c				  	# 信号量
├── k_task_sem.c          	# 任务私有信号量
├── k_cfs.c               	# 任务cfs公平调度
├── k_mm.c                	# 内存管理
├── k_mm_blk.c            	# 小内存block
├── k_mm_debug.c          	# 内存维测
├── k_dyn_mem_proc.c      	# 内存回收机制
├── k_mutex.c             	# 互斥mutex
├── k_buf_queue.c         	# 消息队列（含消息拷贝）
├── k_queue.c             	# 消息队列（不含消息拷贝）
├── k_time.c              	# 系统时间
├── k_timer.c             	# 软件定时器
├── k_workqueue.c         	# 工作队列
├── k_event.c             	# 事件机制
├── k_ringbuf.c           	# 队列管理
├── k_idle.c              	# idle任务 
├── k_tick.c              	# tick定时处理
├── k_obj.c               	# 内核对象
├── k_sys.c               	# 系统初始化、运行           
├── k_pend.c              	# 系统阻塞管理
├── k_sched.c             	# 内核内部调度
├── k_err.c               	# 错误处理
├── k_spin_lock.c         	# 锁（For SMP）
├── k_stats.c             	# 系统状态
├── package.yaml
└── README.md

# 依赖
无

# 接口使用

- 内核统一对外引用头文件：
#include "k_api.h"

- 内核对外默认配置：
k_default_config.h
该配置优先级低于board目录下的k_config.h配置

- 内核配置、裁剪说明（k_config.h）
参考：
https://help.aliyun.com/document_detail/161110.html?spm=a2c4g.11186623.6.626.39f0577beFSOOz

- 用户使用说明：
rhino为最小内核功能集，原则上用户在使用内核接口时，统一使用aos对外接口：
#include "aos/kernel.h"
参考：
https://help.aliyun.com/document_detail/161052.html?spm=a2c4g.11186623.6.564.64761447JWFEf9

- CPU架构支持说明
参考：
https://help.aliyun.com/document_detail/161108.html?spm=a2c4g.11186623.6.624.4d171447fgtZxl

- 板级移植指导：
参考：
https://help.aliyun.com/document_detail/161109.html?spm=a2c4g.11186623.6.625.d150500fMM4qrX


