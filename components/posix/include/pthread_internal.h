#ifndef PTHREAD_INTERNAL_H
#define PTHREAD_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <time.h>
#include <sys/time.h>
#include "k_api.h"
#include "pthread.h"

#define K_PTHREAD(thread)   ((ktask_t *)(thread))
#define K_MUTEX_P(mtx)      ((kmutex_t *)(mtx))
#define K_MUTEX_PP(mtx)     ((kmutex_t **)(mtx))
#define K_SEM_P(sem)        ((ksem_t *)(sem))
#define K_SEM_PP(sem)       ((ksem_t **)(sem))

typedef struct _pthread_cleanup {
    void                    *para;
    int                      cancel_type;
    struct _pthread_cleanup *prev;

    void (*cleanup_routine)(void *para);
} _pthread_cleanup_t;

typedef struct _pthread_tcb {
    unsigned int    magic;
    pthread_attr_t  attr;
    ktask_t        *tid;

    char            name[DEFAULT_THREAD_NAME_LEN];

    void *(*thread_entry)(void *para);
    void *thread_para;

    ksem_t *join_sem;

    unsigned char          cancel_state;
    volatile unsigned char cancel_type;
    volatile unsigned char canceled;

    _pthread_cleanup_t *cleanup;
    _pthread_environ_t *environ;

    void **tls;
    void  *return_value;
} _pthread_tcb_t;

RHINO_INLINE _pthread_tcb_t *_pthread_get_tcb(pthread_t thread)
{
    _pthread_tcb_t *ptcb;

    if (thread == NULL) {
        return NULL;
    }

    ptcb = (_pthread_tcb_t *)K_PTHREAD(thread)->user_info[PTHREAD_CONFIG_USER_INFO_POS];
    if (ptcb != NULL) {
        if (ptcb->magic != PTHREAD_MAGIC) {
            ptcb = NULL;
        }
    }

    return ptcb;
}

#ifdef __cplusplus
}
#endif

#endif