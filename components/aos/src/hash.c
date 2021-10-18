/*
 * Copyright (C) 2018-2021 Alibaba Group Holding Limited
 */

#include <aos/hash.h>

#define TAG                    "hash"

/**
 * @brief  init the hash-table
 * @param  [in] size : table size
 * @return 0/-1
 */
int hash_init(hash_t *hash, size_t size)
{
    CHECK_PARAM(hash && size, -1);
    memset(hash, 0, sizeof(hash_t));
    hash->entries = (hash_entry_t**)aos_zalloc(size * sizeof(hash_entry_t*));
    if (!hash->entries) {
        LOGE(TAG, "may be oom, size = %u", size);
        return -1;
    }

    hash->size = size;

    return 0;
}

/**
 * @brief  uninit the hash table
 * @param  [in] table
 * @return
 */
void hash_uninit(hash_t *table)
{
    int i;
    hash_entry_t *entry, *next;

    if (table) {
        for (i = 0; i < table->size; i++) {
            entry = table->entries[i];
            while (entry) {
                next = entry->next;
                aos_free(entry->key);
                aos_free(entry);
                entry = next;
            }
        }
        aos_free(table->entries);
    }
}

static unsigned _hash_key(hash_t *table, const char *key)
{
    int shift = 0;
    unsigned int hash = 0;

    if (table->size == 1)
        return 0;
    while (*key != '\0') {
        hash ^= ((int)*key++ << shift);
        shift += 4;
        if (shift > 24)
            shift = 0;
    }

    return hash % table->size;
}

/**
 * @brief  add a key-value pair to the hash-table
 * @param  [in] table
 * @param  [in] key
 * @param  [in] data : value of the key is a pointer, may be need free by the caller after free
 * @return 0/-1
 */
int hash_set(hash_t *table, const char *key, void *data)
{
    int idx = 0;
    hash_entry_t *entry = NULL;

    CHECK_PARAM(table && key, -1);
    idx = _hash_key(table, key);
    entry = table->entries[idx];
    while (entry) {
        if (strcmp(key, entry->key) == 0) {
            entry->value = data;
            return 0;
        }
        entry = entry->next;
    }

    /* allocate and fill a new entry */
    entry = (hash_entry_t *)aos_zalloc(sizeof(hash_entry_t));
    if (!entry) {
        LOGE(TAG, "may be oom");
        return -1;
    }
    entry->key = strdup(key);
    if (!entry->key) {
        aos_free(entry);
        LOGE(TAG, "may be oom");
        return -1;
    }
    entry->value        = data;
    entry->next         = table->entries[idx];
    table->entries[idx] = entry;
    table->keys_nb++;

    return 0;
}

/**
 * @brief  whether the key is valid in hash-table
 * @param  [in] table
 * @param  [in] key
 * @param  [in] valid
 * @return
 */
void *hash_get2(hash_t *table, const char *key, int *valid)
{
    int idx;
    void *value = NULL;
    hash_entry_t *entry;

    CHECK_PARAM(key, NULL);
    idx = _hash_key(table, key);
    entry = table->entries[idx];
    while (entry) {
        if (!strcmp(key, entry->key)) {
            if (valid)
                *valid = 1;
            value = entry->value;
            return value;
        }

        entry = entry->next;
    }

    if (valid)
        *valid = 0;

    return value;
}

/**
 * @brief  get value from hash table by the key
 * @param  [in] table
 * @param  [in] key
 * @return
 */
void *hash_get(hash_t *table, const char *key)
{
    return hash_get2(table, key, NULL);
}

/**
 * @brief  whether the key is exsit in the hash-table
 * @param  [in] table
 * @param  [in] key
 * @return 0/1
 */
int hash_key_is_valid(hash_t *table, const char *key)
{
    int idx;
    hash_entry_t *entry;

    CHECK_PARAM(key, 0);
    idx = _hash_key(table, key);
    entry = table->entries[idx];
    while (entry) {
        if (!strcmp(key, entry->key)) {
            return 1;
        }

        entry = entry->next;
    }

    return 0;
}

/**
 * @brief  delete key-value pair from a hash table by the key
 * @param  [in] table
 * @param  [in] key
 * @return 0/-1
 */
int hash_del(hash_t *table, const char *key)
{
    int idx;
    hash_entry_t *entry, *prev = NULL;

    CHECK_PARAM(key, -1);
    idx = _hash_key(table, key);
    entry = table->entries[idx];
    while (entry) {
        if (strcmp(key, entry->key) == 0) {
            aos_free(entry->key);
            if (prev)
                prev->next = entry->next;
            else
                table->entries[idx] = entry->next;
            aos_free(entry);
            table->keys_nb--;
            return 0;
        }
        prev  = entry;
        entry = entry->next;
    }

    return -1;
}

/**
 * @brief  init the hash-iterator
 * @param  [in] table hash-table
 * @param  [in] iter iterator
 * @return 0/-1
 */
int hash_iter_init(hash_t *table, hash_iter_t *iter)
{
    CHECK_PARAM(table && iter, -1);
    memset(iter, 0, sizeof(hash_iter_t));
    iter = (hash_iter_t *) malloc(sizeof(*iter));
    iter->table = table;

    return 0;
}

/**
 * @brief  deinit the hash-iterator
 * @param  [in] iter iterator
 * @return
 */
void hash_iter_uninit(hash_iter_t *iter)
{
    if (iter)
        memset(iter, 0, sizeof(hash_iter_t));
}

/**
 * @brief  foreach entry of the hash-table
 * @param  [in] iter
 * @return NULL on error
 */
const char *hash_iter_foreach(hash_iter_t *iter)
{
    CHECK_PARAM(iter && iter->table, NULL);
    hash_t *table       = iter->table;
    hash_entry_t *entry = iter->entry;

    if (entry) {
        entry = entry->next;
    } else {
        while (iter->idx < iter->table->size) {
            entry = table->entries[iter->idx];
            if (entry) {
                iter->idx++;
                break;
            }
            iter->idx++;
        }
    }

    if (!entry || (iter->idx > table->size)) {
        /* foreach over */
        return NULL;
    }
    iter->entry = entry;

    return entry->key;
}

/**
 * @brief  get entry value from iterator
 * @param  [in] iter
 * @return NULL on error
 */
void *hash_iter_get(hash_iter_t *iter)
{
    CHECK_PARAM(iter && iter->entry, NULL);
    return iter->entry->value;
}

/**
 * @brief  set entry value to the iterator
 * @param  [in] iter
 * @param  [in] data
 * @return 0/-1
 */
int hash_iter_set(hash_iter_t *iter, const void *data)
{
    CHECK_PARAM(iter && iter->entry, -1);
    iter->entry->value = (void*)data;

    return 0;
}

int hash_count(hash_t *table)
{
    return table->keys_nb;
}




