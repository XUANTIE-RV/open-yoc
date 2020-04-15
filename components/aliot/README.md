# 概述

阿里云对接层，对接os和ssl

MCU系统相关HAL

- HAL_Malloc	对应标准C库中的malloc(), 按入参长度开辟一片可用内存, 并返回首地址
- HAL_Free	    对应标准C库中的free(), 将入参指针所指向的内存空间释放
- HAL_Printf	对应标准C库中的printf(), 根据入参格式字符串将字符文本显示到终端，如果用户无需在串口上进行调试，该函数可以为空
- HAL_Snprintf	类似printf, 但输出的结果不再是显示到终端, 而是存入指定的缓冲区内存
- HAL_UptimeMs	返回一个uint64_t类型的数值, 表达设备启动后到当前时间点过去的毫秒数
- HAL_SleepMs	按照指定入参的数值, 睡眠相应的毫秒, 比如参数是10, 那么就会睡眠10毫秒

OS相关即接口
- HAL_MutexCreate	创建一个互斥锁, 返回值可以传递给HAL_MutexLock/Unlock
- HAL_MutexDestroy	销毁一个互斥锁, 这个锁由入参标识
- HAL_MutexLock	申请互斥锁, 如果当前该锁由其它线程持有, 则当前线程睡眠, 否则继续
- HAL_MutexUnlock	释放互斥锁, 此后当前在该锁上睡眠的其它线程将取得锁并往下执行
- HAL_SemaphoreCreate	创建一个信号量, 返回值可以传递给HAL_SemaphorePost/Wait
- HAL_SemaphoreDestroy	销毁一个信号量, 这个信号量由入参标识
- HAL_SemaphorePost	在指定的计数信号量上做自增操作, 解除其它线程的等待
- HAL_SemaphoreWait	在指定的计数信号量上等待并做自减操作
- HAL_ThreadCreate	根据配置参数创建thread