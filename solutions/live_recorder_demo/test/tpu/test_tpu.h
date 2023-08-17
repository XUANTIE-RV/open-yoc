#ifndef __TEST_TPU_H__
#define __TEST_TPU_H__
#if (CONFIG_SUPPORT_TPU == 1)
#include "rtos_types.h"
#include "semaphore.h"
#include "drv/list.h"
#include "pthread.h"

struct cvi_kernel_work {
	pthread_t work_thread;
	sem_t task_wait_sem;
	sem_t done_wait_sem;
	dlist_t task_list;
	pthread_mutex_t task_list_lock;
	dlist_t done_list;
	pthread_mutex_t done_list_lock;
	int work_run;
};

struct cvi_tpu_device {
	// struct device *dev;
	// struct reset_control *rst_tdma;
	// struct reset_control *rst_tpu;
	// struct reset_control *rst_tpusys;
	// struct clk *clk_tpu_axi;
	// struct clk *clk_tpu_fab;
	// dev_t cdev_id;
	// struct cdev cdev;
	sem_t tdma_done;
	uintptr_t tdma_paddr;	//
	uintptr_t tiu_paddr;
	int tdma_irq;
	pthread_mutex_t dev_lock;
	pthread_mutex_t  close_lock;
	int use_count;
	int running_count;
	int suspend_count;
	int resume_count;
	void *private_data;
	struct cvi_kernel_work kernel_work;
	int tdma_irq_num;
};

void cvi_tpu_test(int32_t argc, char **argv);


#endif
#endif      /* __TEST_TPU_H__ */
