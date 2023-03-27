#ifndef _LDC_PLATFORM_H_
#define _LDC_PLATFORM_H_

#include "pthread.h"
#include "semaphore.h"
#include "rtos_types.h"
#include "ldc_uapi.h"
#include "aos/list.h"
#include <time.h>

#define VIP_MAX_PLANES 3

struct cvi_ldc_data {
	__u32 bytesperline[VIP_MAX_PLANES];
	__u32 sizeimage[VIP_MAX_PLANES];
	__u16 w;
	__u16 h;
};

struct cvi_ldc_job {
	enum cvi_ldc_op op;
	struct cvi_ldc_data cap_data, out_data;

	__u64 mesh_id_addr;
	__u32 bgcolor;
	dlist_t node;
	dlist_t task_list;
	bool sync_io;
	MOD_ID_E enModId;
	struct timespec hw_start_time;
};


struct cvi_ldc_vdev {
	char dev_name[64];
	u32 vid_caps;
	pthread_mutex_t mutex;
	sem_t dev_sem; //drv send a done sem to user
	sem_t hw_done_sem; //hw done a job
	sem_t sem_sbm; //vpp_sb_done
	sem_t job_sem; // user send a job to drv
	//struct clk *clk_sys[5];
	//struct clk *clk;
	void *shared_mem;
	char irq_name[64];
	unsigned int irq_num_ldc;
	pthread_t thread;
	//struct kthread_worker worker;
	//struct kthread_work work;
	dlist_t jobq;
	bool job_done;
	bool thread_created;
	u32 align;
};

int ldc_open(void);
int ldc_release(void);
long ldc_ioctl(unsigned int cmd, void *arg);
void ldc_isr(int irq, void *_dev);
int cvi_ldc_probe(void);
int cvi_ldc_remove(void);

#endif /* _LDC_PLATFORM_H_ */
