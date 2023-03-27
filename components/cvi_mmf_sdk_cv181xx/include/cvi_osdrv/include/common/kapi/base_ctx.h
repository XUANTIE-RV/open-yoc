#ifndef __BASE_CTX_H__
#define __BASE_CTX_H__

#include <cvi_base_ctx.h>
#include <vb_uapi.h>

#include <queue.h>
#include "semaphore.h"
#include "k_atomic.h"

#define VIP_RT_PRIO		45

typedef uint32_t VB_POOL;
#ifdef __arm__
typedef uint32_t VB_BLK;
#else
typedef uint64_t VB_BLK;
#endif

#define FIFO_HEAD(name, type)						\
	struct name {							\
		struct type *fifo;					\
		int front, tail, capacity;				\
	}

#define FIFO_INIT(head, _capacity) do {						\
		if (_capacity > 0)						\
		(head)->fifo = malloc(sizeof(*(head)->fifo) * _capacity);	\
		else \
			(head)->fifo = NULL; \
		(head)->front = (head)->tail = -1;				\
		(head)->capacity = _capacity;					\
	} while (0)

#define FIFO_EXIT(head) do {						\
		(head)->front = (head)->tail = -1;			\
		(head)->capacity = 0; 					\
		if ((head)->fifo) 					\
			free((head)->fifo);				\
		(head)->fifo = NULL;					\
	} while (0)

#define FIFO_EMPTY(head)    ((head)->front == -1)

#define FIFO_FULL(head)     (((head)->front == ((head)->tail + 1))	\
			|| (((head)->front == 0) && ((head)->tail == ((head)->capacity - 1))))

#define FIFO_CAPACITY(head) ((head)->capacity)

#define FIFO_SIZE(head)     (FIFO_EMPTY(head) ?\
		0 : ((((head)->tail + (head)->capacity - (head)->front) % (head)->capacity) + 1))

#define FIFO_PUSH(head, elm) do {						\
		if (FIFO_EMPTY(head))						\
			(head)->front = (head)->tail = 0;			\
		else								\
			(head)->tail = ((head)->tail == (head)->capacity - 1)	\
					? 0 : (head)->tail + 1;			\
		(head)->fifo[(head)->tail] = elm;				\
	} while (0)

#define FIFO_POP(head, pelm) do {						\
		*(pelm) = (head)->fifo[(head)->front];				\
		if ((head)->front == (head)->tail)				\
			(head)->front = (head)->tail = -1;			\
		else								\
			(head)->front = ((head)->front == (head)->capacity - 1)	\
					? 0 : (head)->front + 1;		\
	} while (0)

#define FIFO_FOREACH(var, head, idx)					\
	for (idx = (head)->front, var = (head)->fifo[idx];		\
		idx < (head)->front + FIFO_SIZE(head);			\
		idx = idx + 1, var = (head)->fifo[idx % (head)->capacity])

#define FIFO_GET_FRONT(head, pelm) (*(pelm) = (head)->fifo[(head)->front])

#define FIFO_GET_TAIL(head, pelm) (*(pelm) = (head)->fifo[(head)->tail])

#ifndef TAILQ_FOREACH_SAFE
#define TAILQ_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = TAILQ_FIRST((head));				\
		(var) && ((tvar) = TAILQ_NEXT((var), field), 1);	\
		(var) = (tvar))
#endif

#define MO_TBL_SIZE 256

struct mlv_i_s {
	u8 mlv_i_level;
	u8 mlv_i_table[MO_TBL_SIZE];
};

struct mod_ctx_s {
	MOD_ID_E modID;
	u8 ctx_num;
	void *ctx_info;
};

/*
 *
 * frame_crop: for dis
 * s16OffsetXXX: equal to the offset member in VIDEO_FRAME_S.
 *               to show the invalid area in size.
 */
struct cvi_buffer {
	uint64_t phy_addr[NUM_OF_PLANES];
	size_t length[NUM_OF_PLANES];
	uint32_t stride[NUM_OF_PLANES];
	SIZE_S size;
	uint64_t u64PTS;
	uint8_t dev_num;
	PIXEL_FORMAT_E enPixelFormat;
	uint32_t frm_num;
	struct crop_size frame_crop;

	int16_t s16OffsetTop;
	int16_t s16OffsetBottom;
	int16_t s16OffsetLeft;
	int16_t s16OffsetRight;

	uint8_t  motion_lv;
	uint8_t  motion_table[MO_TBL_SIZE];

	uint32_t flags;
	uint32_t sequence;
	struct timeval timestamp;
};

/*
 * vb_pool: the pool this blk belongs to.
 * phy_addr: physical address of the blk.
 * vir_addr: virtual address of the blk.
 * usr_cnt: ref-count of the blk.
 * buf: the usage which define planes of buffer.
 * magic: magic number to avoid wrong reference.
 * mod_ids: the users of this blk. BIT(MOD_ID) will be set is MOD using.
 */
struct vb_s {
	VB_POOL vb_pool;
	uint64_t phy_addr;
	void *vir_addr;
	atomic_t usr_cnt;
	struct cvi_buffer buf;
	uint32_t magic;
	uint64_t mod_ids;
	CVI_BOOL external;
	//struct hlist_node node;
};

FIFO_HEAD(vbq, vb_s*);

/*
 * VB_REMAP_MODE_NONE: no remap.
 * VB_REMAP_MODE_NOCACHE: no cache remap.
 * VB_REMAP_MODE_CACHED: cache remap. flush cache is needed.
 */
enum vb_remap_mode {
	_VB_REMAP_MODE_NONE = 0,
	_VB_REMAP_MODE_NOCACHE = 1,
	_VB_REMAP_MODE_CACHED = 2,
	_VB_REMAP_MODE_BUTT
};

typedef int32_t(*vb_acquire_fp)(MMF_CHN_S);

struct vb_req {
	STAILQ_ENTRY(vb_req) stailq;
	vb_acquire_fp fp;
	MMF_CHN_S chn;
	VB_POOL VbPool;
};

STAILQ_HEAD(vb_req_q, vb_req);

struct vb_pool {
	VB_POOL poolID;
	int16_t ownerID;
	uint64_t memBase;
	uint64_t memBaseAlign;
	void *vmemBase;
	struct vbq freeList;
	struct vb_req_q reqQ;
	uint32_t blk_size;
	uint32_t blk_cnt;
	enum vb_remap_mode remap_mode;
	CVI_BOOL bIsCommPool;
	uint32_t u32FreeBlkCnt;
	uint32_t u32MinFreeBlkCnt;
	char acPoolName[VB_POOL_NAME_LEN];
	struct pthread_mutex lock;
	struct pthread_mutex reqQ_lock;
};

struct snap_s {
	TAILQ_ENTRY(snap_s) tailq;

	//pthread_cond_t cond;
	pthread_cond_t cond_queue;
	MMF_CHN_S chn;
	VB_BLK blk;
};


/*
 * lock: lock for waitq/workq.
 * waitq: the queue of VB_BLK waiting to be done. For TDM modules, such as VPSS.
 * workq: the queue of VB_BLK module is working on.
 * dlock: lock for doneq.
 * doneq: the queue of VB_BLK to be taken. Size decided by u32Depth.
 * sem: sem to notify waitq is updated.
 * snap_jobs: the req to get frame for this chn.
 */
struct vb_jobs_t {
	struct pthread_mutex lock;
	struct vbq waitq;
	struct vbq workq;
	struct pthread_mutex dlock;
	struct vbq doneq;
	sem_t sem;
	TAILQ_HEAD(snap_q, snap_s) snap_jobs;
	uint8_t inited;
};

struct vi_jobs_ctx {
	struct vb_jobs_t vb_jobs[VI_MAX_CHN_NUM];
	struct vbq extchn_doneq[VI_MAX_EXT_CHN_NUM];
};

struct vpss_jobs_ctx {
	struct vb_jobs_t ins[VPSS_MAX_GRP_NUM];
	struct vb_jobs_t outs[VPSS_MAX_GRP_NUM][VPSS_MAX_CHN_NUM];
};

struct vo_jobs_ctx {
	struct vb_jobs_t vb_jobs;
};

typedef struct ptread_sem {
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	unsigned int cnt;
} ptread_sem_s;

struct cvi_venc_vb_ctx {
	CVI_BOOL enable_bind_mode;
	CVI_BOOL currBindMode;
	pthread_t thread;
	struct vb_jobs_t vb_jobs;
	CVI_BOOL pause;

	ptread_sem_s sem_vb;
};
extern struct cvi_venc_vb_ctx	venc_vb_ctx[VENC_MAX_CHN_NUM];

struct cvi_vdec_vb_ctx {
	CVI_BOOL enable_bind_mode;
	CVI_BOOL currBindMode;
	pthread_t thread;
	struct vb_jobs_t vb_jobs;
	CVI_BOOL pause;
};
extern struct cvi_vdec_vb_ctx	vdec_vb_ctx[VENC_MAX_CHN_NUM];

int base_set_mod_ctx(struct mod_ctx_s *ctx_s);
int32_t base_get_frame_info(PIXEL_FORMAT_E fmt, SIZE_S size, struct cvi_buffer *buf, u64 mem_base, u8 align);
CVI_S32 base_get_chn_buffer(MMF_CHN_S chn, VB_BLK *blk, CVI_S32 timeout_ms);
CVI_S32 base_fill_videoframe2buffer(MMF_CHN_S chn, const VIDEO_FRAME_INFO_S *pstVideoFrame,
	struct cvi_buffer *buf);
// jobs related api
void base_mod_jobs_init(MMF_CHN_S chn, enum CHN_TYPE_E chn_type,
		uint8_t waitq_depth, uint8_t workq_depth, uint8_t doneq_depth);
void base_mod_jobs_exit(MMF_CHN_S chn, enum CHN_TYPE_E chn_type);
struct cvi_buffer *base_mod_jobs_enque_work(MMF_CHN_S chn, enum CHN_TYPE_E chn_type);
bool base_mod_jobs_waitq_empty(MMF_CHN_S chn, enum CHN_TYPE_E chn_type);
bool base_mod_jobs_workq_empty(MMF_CHN_S chn, enum CHN_TYPE_E chn_type);
VB_BLK base_mod_jobs_waitq_pop(MMF_CHN_S chn, enum CHN_TYPE_E chn_type);
VB_BLK base_mod_jobs_workq_pop(MMF_CHN_S chn, enum CHN_TYPE_E chn_type);
struct vb_jobs_t *base_get_jobs_by_chn(MMF_CHN_S chn, enum CHN_TYPE_E chn_type);
CVI_U32 get_diff_in_us(struct timespec t1, struct timespec t2);

#endif  /* __CVI_BASE_CTX_H__ */
