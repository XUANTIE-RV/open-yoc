#ifndef __SWITCHES_QUEUE_H__
#define __SWITCHES_QUEUE_H__

#define QUEUE_MAX_MSG_SIZE (3)

typedef enum _queue_mesg_type_s
{
    QUEUE_MESG_TYPE_INPUT,
    QUEUE_MESG_TYPE_POWER
} queue_mesg_type_e;

typedef struct _queue_mesg_s
{
    queue_mesg_type_e type;
    uint8_t data;
} queue_mesg_t;

int queue_recv_data(queue_mesg_t *pdata);
int queue_send_data(queue_mesg_t *pdata);
int queue_init(void);

#endif
