#ifndef COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_MBOX_IPC_MEM_H_
#define COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_MBOX_IPC_MEM_H_

void drv_ipc_mem_init(void);
void *drv_ipc_mem_alloc(int *len);
void drv_ipc_mem_free(void *p);
int drv_ipc_mem_use_cache(void);

#endif /* COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_MBOX_IPC_MEM_H_ */
