#ifndef COMMON_H_
#define COMMON_H_
#include <stdint.h>
#include <stddef.h>

extern const unsigned char one_bits[];
/// Number of '1' bits in a byte
#define NB_ONE_BITS(byte)   (one_bits[(byte) & 0x0F] + one_bits[(byte) >> 4])

/// Get the number of elements within an array, give also number of rows in a 2-D array
#define ARRAY_LEN(array)   (sizeof((array))/sizeof((array)[0]))

/// Get the number of columns within a 2-D array
#define ARRAY_NB_COLUMNS(array)  (sizeof((array[0]))/sizeof((array)[0][0]))

/// Macro to get a structure from one of its structure field
#define CONTAINER_OF(ptr, type, member)    ((type *)( (char *)ptr - offsetof(type,member) ))

#define MIN(a,b) ((a)<(b)?(a):(b))

#define CEILING(dividend,divisor) ((dividend)/(divisor) + (((dividend)%(divisor))?1:0))

void *ll_malloc(uint32_t size);

void ll_free(void *data);

void rand_gen_by_word(uint8_t *ptr,uint8_t word_num);

void memcpy32(uint32_t *dest, const uint32_t *src, uint32_t size_word);

int32_t time_diff(uint32_t time0,uint32_t time1);

uint8_t count_trailing_zeros(uint32_t x);

uint8_t count_leading_zeros(uint32_t x);
#endif
