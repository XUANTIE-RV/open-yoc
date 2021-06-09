#ifndef TINYFS_WRITE_CACHE_H_
#define TINYFS_WRITE_CACHE_H_
#include <stdint.h>

#define TINYFS_WRITE_CACHE_SIZE 256
void tinyfs_nvm_program(uint32_t offset,uint32_t length,uint8_t *buffer);
void tinyfs_nvm_read_with_cache(uint32_t offset, uint32_t length, uint8_t *buffer);
void tinyfs_nvm_write_through(void);

#endif
