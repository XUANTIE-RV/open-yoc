/*******************************************************************************
 *
 * Copyright (c) 2013 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
  *    Baijie & Longrong, China Mobile - Please refer to git log
 *
 *******************************************************************************/
#include "cis_internals.h"


cis_list_t *cis_list_add(cis_list_t *head,
                         cis_list_t *node)
{
    cis_list_t *target;

    if (NULL == head) {
        return node;
    }

    if (head->id > node->id) {
        node->next = head;
        return node;
    }

    target = head;

    while (NULL != target->next && target->next->id < node->id) {
        target = target->next;
    }

    node->next = target->next;
    target->next = node;

    return head;
}


cis_list_t *cis_list_find(cis_list_t *head,
                          cis_listid_t id)
{
    while (NULL != head && head->id < id) {
        head = head->next;
    }

    if (NULL != head && head->id == id) {
        return head;
    }

    return NULL;
}

cis_list_t *cis_list_find_u16(cis_list_t *head,
                              uint16_t id)
{
    while (NULL != head && (head->id & 0xFFFF) < id) {
        head = head->next;
    }

    if (NULL != head && (head->id & 0xFFFF) == id) {
        return head;
    }

    return NULL;
}


cis_list_t *cis_list_remove(cis_list_t *head,
                            cis_listid_t id,
                            cis_list_t **nodeP)
{
    cis_list_t *target;

    if (head == NULL) {
        if (nodeP) {
            *nodeP = NULL;
        }

        return NULL;
    }

    if (head->id == id) {
        if (nodeP) {
            *nodeP = head;
        }

        return head->next;
    }

    target = head;

    while (NULL != target->next && target->next->id < id) {
        target = target->next;
    }

    if (NULL != target->next && target->next->id == id) {
        if (nodeP) {
            *nodeP = target->next;
        }

        target->next = target->next->next;
    } else {
        if (nodeP) {
            *nodeP = NULL;
        }
    }

    return head;
}

cis_listid_t cis_list_newId(cis_list_t *head)
{
    cis_listid_t id;
    cis_list_t *target;

    id = 0;
    target = head;

    while (target != NULL && id == target->id) {
        id = target->id + 1;
        target = target->next;
    }

    return id;
}

void cis_list_free(cis_list_t *head)
{
    if (head != NULL) {
        cis_list_t *nextP;

        nextP = head->next;
        cis_free(head);
        cis_list_free(nextP);
    }
}


cis_listid_t   cis_list_count(cis_list_t *head)
{
    cis_listid_t count = 0;
    cis_list_t *target;

    target = head;

    while (target != NULL) {
        count++;
        target = target->next;
    }

    return count;
}