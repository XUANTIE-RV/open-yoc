/*
 * Copyright (C) 2018-2021 Alibaba Group Holding Limited
 */

#ifndef __HASH_H__
#define __HASH_H__

#include <aos/aos.h>

__BEGIN_DECLS__

typedef struct _hash_t       hash_t;
typedef struct _hash_entry_t hash_entry_t;
typedef struct _hash_iter_t  hash_iter_t;

struct _hash_t {
    size_t             size;
    size_t             keys_nb;
    hash_entry_t       **entries;
};

struct _hash_entry_t {
    char               *key;
    void               *value;
    hash_entry_t       *next;
};

struct _hash_iter_t {
    hash_t             *table;
    hash_entry_t       *entry;
    int                idx;
};

/**
 * @brief  init the hash-table
 * @param  [in] size : table size
 * @return 0/-1
 */
int hash_init(hash_t *hash, size_t size);

/**
 * @brief  uninit the hash table
 * @param  [in] table
 * @return
 */
void hash_uninit(hash_t *table);

/**
 * @brief  add a key-value pair to the hash-table
 * @param  [in] table
 * @param  [in] key
 * @param  [in] data : value of the key is a pointer, may be need free by the caller after free
 * @return 0/-1
 */
int hash_set(hash_t *table, const char *key, void *data);

/**
 * @brief  get value from hash table by the key
 * @param  [in] table
 * @param  [in] key
 * @return
 */
void *hash_get(hash_t *table, const char *key);

/**
 * @brief  whether the key is valid in hash-table
 * @param  [in] table
 * @param  [in] key
 * @param  [in] valid
 * @return
 */
void *hash_get2(hash_t *table, const char *key, int *valid);

/**
 * @brief  whether the key is exsit in the hash-table
 * @param  [in] table
 * @param  [in] key
 * @return 0/1
 */
int hash_key_is_valid(hash_t *table, const char *key);

/**
 * @brief  init the hash-iterator
 * @param  [in] table hash-table
 * @param  [in] iter iterator
 * @return 0/-1
 */
int hash_iter_init(hash_t *table, hash_iter_t *iter);

/**
 * @brief  deinit the hash-iterator
 * @param  [in] iter iterator
 * @return
 */
void hash_iter_uninit(hash_iter_t *iter);

/**
 * @brief  foreach entry of the hash-table
 * @param  [in] iter
 * @return NULL on error
 */
const char *hash_iter_foreach(hash_iter_t *iter);

/**
 * @brief  get entry value from iterator
 * @param  [in] iter
 * @return NULL on error
 */
void *hash_iter_get(hash_iter_t *iter);

/**
 * @brief  set entry value to the iterator
 * @param  [in] iter
 * @param  [in] data
 * @return 0/-1
 */
int hash_iter_set(hash_iter_t *iter, const void *data);

/**
 * @brief  delete key-value pair from a hash table by the key
 * @param  [in] table
 * @param  [in] key
 * @return 0/-1
 */
int hash_del(hash_t *table, const char *key);

/**
 * @brief  get key count from the hash table
 * @param  [in] table
 * @return -1 on error
 */
int hash_count(hash_t *table);

__END_DECLS__

#endif /* __HASH_H__ */

