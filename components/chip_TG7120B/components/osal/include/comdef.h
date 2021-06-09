/**************************************************************************************************
  Filename:       comdef.h
  Revised:
  Revision:

  Description:    Type definitions and macros.



**************************************************************************************************/

#ifndef COMDEF_H
#define COMDEF_H

#ifdef __cplusplus
extern "C"
{
#endif



/*********************************************************************
 * INCLUDES
 */

/* HAL */

#include "types.h"


/*********************************************************************
 * Lint Keywords
 */
#define VOID (void)

#define NULL_OK
#define INP
#define OUTP
#define UNUSED
#define ONLY
#define READONLY
#define SHARED
#define KEEP
#define RELAX




/*********************************************************************
 * CONSTANTS
 */

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef GENERIC
#define GENERIC
#endif

/*** Generic Status Return Values ***/
#define SUCCESS                   0x00
#define FAILURE                   0x01
#define INVALIDPARAMETER          0x02
#define INVALID_TASK              0x03
#define MSG_BUFFER_NOT_AVAIL      0x04
#define INVALID_MSG_POINTER       0x05
#define INVALID_EVENT_ID          0x06
#define INVALID_INTERRUPT_ID      0x07
#define NO_TIMER_AVAIL            0x08
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define INVALID_MEM_SIZE          0x0B
#define NV_BAD_ITEM_LEN           0x0C

/*********************************************************************
 * TYPEDEFS
 */

// Generic Status return
typedef uint8 Status_t;

// Data types
typedef int32   int24;
typedef uint32  uint24;

/*********************************************************************
 * Global System Events
 */

#define SYS_EVENT_MSG               0x8000  // A message is waiting event

/*********************************************************************
 * Global Generic System Messages
 */

#define KEY_CHANGE                0xC0    // Key Events

// OSAL System Message IDs/Events Reserved for applications (user applications)
// 0xE0 – 0xFC

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/
/*********************************************************************
 * TYPEDEFS
 */

/* These attributes define sleep beheaver. The attributes can be changed
 * for each sleep cycle or when the device characteristic change.
 */
typedef struct {
    uint16 pwrmgr_task_state;
    uint16 pwrmgr_next_timeout;
    uint16 accumulated_sleep_time;
    uint8  pwrmgr_device;
} pwrmgr_attribute_t;

/* With PWRMGR_ALWAYS_ON selection, there is no power savings and the
 * device is most likely on mains power. The PWRMGR_BATTERY selection allows
 * the HAL sleep manager to enter SLEEP LITE state or SLEEP DEEP state.
 */
#define PWRMGR_ALWAYS_ON  0
#define PWRMGR_BATTERY    1

/* The PWRMGR_CONSERVE selection turns power savings on, all tasks have to
 * agree. The PWRMGR_HOLD selection turns power savings off.
 */
#define PWRMGR_CONSERVE 0
#define PWRMGR_HOLD     1


/*********************************************************************
 * GLOBAL VARIABLES
 */

/* This global variable stores the power management attributes.
 */
extern pwrmgr_attribute_t pwrmgr_attribute;

typedef void *osal_msg_q_t;

typedef unsigned short(*pTaskEventHandlerFn)(unsigned char task_id, unsigned short event);

typedef struct {
    uint8  event;
    uint8  status;
} osal_event_hdr_t;

/*********************************************************************
 * TYPEDEFS
 */

typedef struct {
    unsigned short len : 15;   // unsigned short len : 15;
    unsigned short inUse : 1;  // unsigned short inUse : 1;
} osalMemHdrHdr_t;

typedef union {
    halDataAlign_t alignDummy;
    uint32 val;
    osalMemHdrHdr_t hdr;
} osalMemHdr_t;


extern uint8 osal_memcmp(const void GENERIC *src1, const void GENERIC *src2, unsigned int len);

/*
 * Memory set
 */
extern void *osal_memset(void *dest, uint8 value, int len);

extern void osal_run_system(void);

extern uint32 osal_next_timeout(void);

extern uint8 osal_start_timerEx(uint8 task_id, uint16 event_id, uint32 timeout_value);
extern uint8 osal_stop_timerEx(uint8 task_id, uint16 event_id);

extern void osal_mem_set_heap(osalMemHdr_t *hdr, uint32 size);

extern uint8 osal_init_system(void);
extern void osal_pwrmgr_device(uint8 pwrmgr_device);
extern uint8 *osal_msg_receive(uint8 task_id);
extern uint8 osal_msg_deallocate(uint8 *msg_ptr);
extern void *osal_mem_alloc(uint16 size);
extern void *osal_memcpy(void *, const void GENERIC *, unsigned int);
extern void osal_bm_free(void *payload_ptr);

#ifdef __cplusplus
}
#endif

#endif /* COMDEF_H */
