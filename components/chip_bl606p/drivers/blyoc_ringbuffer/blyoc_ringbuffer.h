
#ifndef __BLYOC_RINGBUFFER__
#define __BLYOC_RINGBUFFER__

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *buffer;
    uint32_t length;
    uint32_t ridx;
    uint32_t widx;
} blyoc_ringbuf_t;

/**
 * @brief  create a ring buffer
 * @param[in] length length space of the ring buffer
 * @return  pointer to ring buffer
 */
int blyoc_ringbuffer_create(blyoc_ringbuf_t *ringbuffer, uint8_t *buffer, int length);

int blyoc_ringbuffer_reset(blyoc_ringbuf_t *ringbuffer);

/**
 * @brief   destroy the ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return  null
 */
void blyoc_ringbuffer_destroy(blyoc_ringbuf_t *buffer);

/**
 * @brief   read data from ring buffer.
 * @param[in] buffer pointer to the ring buffer
 * @param[in] target pointer to buffer for data to read from ring buffer
 * @param[in] amount amount of data items to read
 * @return  number of actual read data
 */
int blyoc_ringbuffer_read(blyoc_ringbuf_t *buffer, uint8_t *target, uint32_t amount);

/**
 * @brief   write data to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @param[in] data pointer to buffer for data to write to ring buffer
 * @param[in] length length of data items to write
 * @return  0 for success
 */
int blyoc_ringbuffer_write(blyoc_ringbuf_t *buffer, uint8_t *data, uint32_t length);

/**
 * @brief   is the ring buffer empty?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int blyoc_ringbuffer_empty(blyoc_ringbuf_t *buffer);
/**
 * @brief   is the ring buffer full?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int blyoc_ringbuffer_full(blyoc_ringbuf_t *buffer);
/**
 * @brief   available write space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return   number of write spcae
 */
int blyoc_ringbuffer_available_write_space(blyoc_ringbuf_t *buffer);
/**
 * @brief    available read space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return  number of read spcae
 */
int blyoc_ringbuffer_available_read_space(blyoc_ringbuf_t *buffer);

#define blyoc_ringbuffer_available_write_space(B) (\
        (B)->length - blyoc_ringbuffer_available_read_space(B))

#define blyoc_ringbuffer_full(B) (blyoc_ringbuffer_available_write_space(B) == 0)

#define blyoc_ringbuffer_empty(B) (blyoc_ringbuffer_available_read_space((B)) == 0)

#define blyoc_ringbuffer_clear(B) ((B)->widx = (B)->ridx = 0)

#ifdef __cplusplus
}
#endif

#endif

