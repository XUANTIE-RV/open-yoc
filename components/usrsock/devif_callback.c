/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     devif_callback.c
 * @brief    user sock callback functions
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdbool.h>
#include <soc.h>
#include "devif.h"
#include "usrsock.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct devif_callback_s g_cbprealloc[CONFIG_NET_NACTIVESOCKETS];
static struct devif_callback_s *g_cbfreelist = NULL;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: devif_callback_free
 *
 * Description:
 *   Return a callback container to the free list.
 *
 * Assumptions:
 *   This function is called with the network locked.
 *
 ****************************************************************************/

static void devif_callback_free(struct devif_callback_s *cb,
                                struct devif_callback_s **list)
{
    struct devif_callback_s *prev;
    struct devif_callback_s *curr;

    if (cb) {
        net_lock();

        /* Remove the callback structure from the data notification list if
         * it is supposed to be in the data notification list.
         */

        if (list) {
            /* Find the callback structure in the connection event list */

            for (prev = NULL, curr = *list;
                    curr && curr != cb;
                    prev = curr, curr = curr->nxtconn);

            /* Remove the structure from the connection event list */

            if (curr) {
                if (prev) {
                    prev->nxtconn = cb->nxtconn;
                } else {
                    *list = cb->nxtconn;
                }
            }
        }

        /* Put the structure into the free list */

        cb->nxtconn  = g_cbfreelist;
        cb->nxtdev   = NULL;
        g_cbfreelist = cb;
        net_unlock();
    }
}

/****************************************************************************
 * Name: devif_event_trigger
 *
 * Description:
 *   Return true if the current set of events should trigger a callback to
 *   occur.
 *
 * Input Parameters:
 *   events   - The set of events that has occurred.
 *   triggers - The set of events that will trigger a callback.
 *
 ****************************************************************************/

static bool devif_event_trigger(uint16_t events, uint16_t triggers)
{
    /* The events are divided into a set of individual bits that may be ORed
     * together PLUS a field that encodes a single poll event.
     *
     * First check if any of the individual event bits will trigger the
     * callback.
     */

    if ((events & triggers & ~DEVPOLL_MASK) != 0) {
        return true;
    }

    /* No... check the encoded device event. */

    if ((events & DEVPOLL_MASK) == (triggers & DEVPOLL_MASK)) {
        return true;
    }

    /* No.. this event set will not generate the callback */

    return false;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: devif_initialize
 *
 * Description:
 *   Perform initialization of the network device interface layer
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void devif_initialize(void)
{
    devif_callback_init();
}

/****************************************************************************
 * Name: devif_callback_init
 *
 * Description:
 *   Configure the pre-allocated callback structures into a free list.
 *
 * Assumptions:
 *   Called early in the initialization sequence so that no special
 *   protection is required.
 *
 ****************************************************************************/

void devif_callback_init(void)
{
    int i;

    for (i = 0; i < CONFIG_NET_NACTIVESOCKETS; i++) {
        g_cbprealloc[i].nxtconn = g_cbfreelist;
        g_cbfreelist = &g_cbprealloc[i];
    }
}

/****************************************************************************
 * Name: devif_callback_alloc
 *
 * Description:
 *   Allocate a callback container from the free list.
 *
 *   If dev is non-NULL, then this function verifies that the device
 *   reference is still  valid and that the device is still UP status.  If
 *   those conditions are not true, this function will fail to allocate the
 *   callback.
 *
 * Assumptions:
 *   This function is called with the network locked.
 *
 ****************************************************************************/

struct devif_callback_s *devif_callback_alloc(struct devif_callback_s **list)
{
    struct devif_callback_s *ret;

    /* Check  the head of the free list */

    net_lock();
    ret  = g_cbfreelist;
    if (ret) {
        /* Remove the next instance from the head of the free list */

        g_cbfreelist = ret->nxtconn;
        memset(ret, 0, sizeof(struct devif_callback_s));

        /* Add the newly allocated instance to the head of the device event
         * list.
         */


        /* Add the newly allocated instance to the head of the specified list */

        if (list)
        {
            ret->nxtconn = *list;
            *list = ret;
        }
    }

    net_unlock();
    return ret;
}

/****************************************************************************
 * Name: devif_conn_callback_free
 *
 * Description:
 *   Return a connection/port callback container to the free list.
 *
 *   This function is just a front-end for devif_callback_free().  If the
 *   dev argument is non-NULL, it will verify that the device reference is
 *   still valid before attempting to free the callback structure.  A
 *   non-NULL list pointer is assumed to be valid in any case.
 *
 *   The callback structure will be freed in any event.
 *
 * Assumptions:
 *   This function is called with the network locked.
 *
 ****************************************************************************/

void devif_conn_callback_free(struct devif_callback_s *cb,
                              struct devif_callback_s **list)
{
    devif_callback_free(cb, list);
}

/****************************************************************************
 * Name: devif_dev_callback_free
 *
 * Description:
 *   Return a device callback container to the free list.
 *
 *   This function is just a front-end for devif_callback_free().  If the
 *   de argument is non-NULL, it will verify that the device reference is
 *   still valid before attempting to free the callback structure.  It
 *   differs from devif_conn_callback_free in that connection/port-related
 *   connections are also associated with the device and, hence, also will
 *   not be valid if the device pointer is not valid.
 *
 *   The callback structure will be freed in any event.
 *
 * Assumptions:
 *   This function is called with the network locked.
 *
 ****************************************************************************/

void devif_dev_callback_free(struct devif_callback_s *cb)
{
    struct devif_callback_s **list;

    /* Check if the device pointer is still valid.  It could be invalid if, for
     * example, the device were unregistered between the time when the callback
     * was allocated and the time when the callback was freed.
     */

    list = NULL;

    /* Then free the callback */

    devif_callback_free(cb, list);
}

/****************************************************************************
 * Name: devif_conn_event
 *
 * Description:
 *   Execute a list of callbacks using the packet event chain.
 *
 * Input Parameters:
 *   dev - The network device state structure associated with the network
 *     device that initiated the callback event.
 *   pvconn - Holds a reference to the TCP connection structure or the UDP
 *     port structure.  May be NULL if the even is not related to a TCP
 *     connection or UDP port.
 *   flags - The bit set of events to be notified.
 *   list - The list to traverse in performing the notifications
 *
 * Returned Value:
 *   The updated flags as modified by the callback functions.
 *
 * Assumptions:
 *   This function is called with the network locked.
 *
 ****************************************************************************/

uint16_t devif_conn_event(void *pvconn, uint16_t flags,
                          struct devif_callback_s *list)
{
    struct devif_callback_s *next;

    /* Loop for each callback in the list and while there are still events
     * set in the flags set.
     */

    net_lock();
    while (list && flags) {
        /* Save the pointer to the next callback in the lists.  This is done
         * because the callback action might delete the entry pointed to by
         * list.
         */

        next = list->nxtconn;

        /* Check if this callback handles any of the events in the flag set */

        if (list->event != NULL && devif_event_trigger(flags, list->flags))
        {
            /* Yes.. perform the callback.  Actions perform by the callback
             * may delete the current list entry or add a new list entry to
             * beginning of the list (which will be ignored on this pass)
             */

            flags = list->event(pvconn, list->priv, flags);
        }

        /* Set up for the next time through the loop */

        list = next;
    }

    net_unlock();
    return flags;
}

