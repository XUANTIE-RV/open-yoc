/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
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
#include "cis_log.h"

#if CIS_ENABLE_MEMORYTRACE
#define MEMTRACE_FREELIST_MAX       0

typedef struct MemoryEntry {
    struct MemoryEntry *next;
    const char *file;
    const char *function;
    int         lineno;
    size_t      size;
    int         count;
    uint32_t    data[1];
} memory_entry_t;

static memory_entry_t prv_memory_cissys_alloc_list = { NULL, "head", "cissys_alloc", 0, 0, 0};
static memory_entry_t prv_memory_free_list = { NULL, "head", "cissys_free", 0, 0, 0};
static uint32_t prv_memory_max = 0;

static memory_entry_t *prv_memory_find_previous(memory_entry_t *list, void *memory)
{
    while (NULL != list->next) {
        if (list->next->data == memory) {
            return list;
        }

        list = list->next;
    }

    return NULL;
}

static void prv_trace_add_free_list(memory_entry_t *remove, const char *file, const char *function, int lineno)
{

#if MEMTRACE_FREELIST_MAX <= 0
    cissys_free(remove);
#else
    remove->next = prv_memory_free_list.next;
    prv_memory_free_list.next = remove;
    remove->file = file;
    remove->function = function;
    remove->lineno = lineno;

    if (prv_memory_free_list.count < MEMTRACE_FREELIST_MAX) {
        ++prv_memory_free_list.count;
    } else if (NULL != remove->next) {
        while (NULL != remove->next->next) {
            remove = remove->next;
        }

        cissys_free(remove->next);
        remove->next = NULL;
    }

#endif//MEMTRACE_FREELIST_MAX
}

void *cis_trace_malloc(size_t size, const char *file, const char *function, int lineno)
{
    static int counter = 0;
    memory_entry_t *entry = (memory_entry_t *)cissys_malloc(size + sizeof(memory_entry_t));
    entry->next = prv_memory_cissys_alloc_list.next;
    prv_memory_cissys_alloc_list.next = entry;
    ++prv_memory_cissys_alloc_list.count;
    prv_memory_cissys_alloc_list.size += size;
    prv_memory_cissys_alloc_list.lineno = 1;

    entry->file = file;
    entry->function = function;
    entry->lineno = lineno;
    entry->size = size;
    entry->count = ++counter;

    if (prv_memory_cissys_alloc_list.size > prv_memory_max) {
        prv_memory_max = prv_memory_cissys_alloc_list.size;
    }


    return &(entry->data);
}

void cis_trace_free(void *mem, const char *file, const char *function, int lineno)
{
    if (NULL != mem) {
        memory_entry_t *entry = prv_memory_find_previous(&prv_memory_cissys_alloc_list, mem);

        if (NULL != entry) {
            memory_entry_t *remove = entry->next;
            entry->next = remove->next;
            --prv_memory_cissys_alloc_list.count;
            prv_memory_cissys_alloc_list.size -= remove->size;
            prv_memory_cissys_alloc_list.lineno = 1;
            prv_trace_add_free_list(remove, file, function, lineno);
        } else {
            CIS_LOG_PRINT("memory: cis_trace_free error (no cissys_alloc) %s, %d, %s\n", file, lineno, function);
            memory_entry_t *entry = prv_memory_find_previous(&prv_memory_free_list, mem);

            if (NULL != entry) {
                entry = entry->next;
                CIS_LOG_PRINT("memory: already frees at %s, %d, %s\n", entry->file, entry->lineno, entry->function);
            }
        }
    }
}

void trace_print(int loops, int level)
{
    static int counter = 0;

    if (0 == loops) {
        counter = 0;
    } else {
        ++counter;
    }

    if (0 == loops || (((counter % loops) == 0) && prv_memory_cissys_alloc_list.lineno)) {
        prv_memory_cissys_alloc_list.lineno = 0;

        if (1 == level) {
            size_t total = 0;
            int entries = 0;
            memory_entry_t *entry = prv_memory_cissys_alloc_list.next;

            while (NULL != entry) {
                CIS_LOG_PRINT("memory: #%d, %lu bytes, %s, %d, %s\n", entry->count, (unsigned long) entry->size, entry->file, entry->lineno, entry->function);
                ++entries;
                total += entry->size;
                entry = entry->next;
            }

            if (entries != prv_memory_cissys_alloc_list.count) {
                CIS_LOG_PRINT("memory: error %d entries != %d\n", prv_memory_cissys_alloc_list.count, entries);
            }

            if (total != prv_memory_cissys_alloc_list.size) {
                CIS_LOG_PRINT("memory: error %lu total bytes != %lu\n", (unsigned long) prv_memory_cissys_alloc_list.size, (unsigned long) total);
            }
        }

        CIS_LOG_PRINT("memory: %d entries, %lu total bytes\n", prv_memory_cissys_alloc_list.count, (unsigned long) prv_memory_cissys_alloc_list.size);
        CIS_LOG_PRINT("memory: %lu peak bytes\n", prv_memory_max);
    }
}

void trace_status(int *blocks, size_t *size)
{
    if (NULL != blocks) {
        *blocks = prv_memory_cissys_alloc_list.count;
    }

    if (NULL != size) {
        *size = prv_memory_cissys_alloc_list.size;
    }
}

#endif
