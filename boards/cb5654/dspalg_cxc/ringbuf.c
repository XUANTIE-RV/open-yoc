#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dspalg_ringbuf.h"

#define SRAM_SIZE              0x8000 //32KB
#define SRAM_START_ADDR        0x20000000

#define SRAM_DSP_START_ADDR    SRAM_START_ADDR
#define SRAM_RISC_START_ADDR   (SRAM_START_ADDR + SRAM_SIZE/2)

#define RINGBUF_DSP_INFO_SIZE  0x400  //1KB
#define RINGBUF_DSP_SIZE       (SRAM_SIZE/2 - RINGBUF_DSP_INFO_SIZE)
#define RINGBUF_DSP_START_ADDR (SRAM_DSP_START_ADDR + RINGBUF_DSP_INFO_SIZE)

#define RINGBUF_RISC_INFO_SIZE  0x400  //1KB
#define RINGBUF_RISC_SIZE       (SRAM_SIZE/2 - RINGBUF_RISC_INFO_SIZE)
#define RINGBUF_RISC_START_ADDR (SRAM_RISC_START_ADDR + RINGBUF_RISC_INFO_SIZE)

#define RING_BUF_DEVICE(ptr) struct ring_buf_s *ptr;          \
    ptr = ring_buf_dev(dev);                                  \
    if (ptr == NULL || ptr->init == 0)                        \
    {                                                         \
        printf("ringbuf assert %s:%d\n", __FILE__, __LINE__); \
        return 0;                                             \
    }

static struct ring_buf_s *ring_buf_dsp = NULL;
static struct ring_buf_s *ring_buf_risc = NULL;

static struct ring_buf_s *ring_buf_dev(int dev)
{
    if (dev == DEV_DSP)
        return ring_buf_dsp;
    else if (dev == DEV_RISC)
        return ring_buf_risc;
    else
        return NULL;
}

static int ring_buf_wr_update(int dev, int val)
{
    RING_BUF_DEVICE(ptr);
    
    ptr->pcm_wrptr += val;
    if (ptr->pcm_wrptr >= ptr->pcm_end)
        ptr->pcm_wrptr -= ptr->pcm_size;

    return 0;
}

static int ring_buf_rd_update(int dev, int val)
{
    RING_BUF_DEVICE(ptr);
    
    ptr->pcm_rdptr += val;
    if (ptr->pcm_rdptr >= ptr->pcm_end)
        ptr->pcm_rdptr -= ptr->pcm_size;

    return 0;
}

static int ring_buf_wr_distance(int dev)
{
    RING_BUF_DEVICE(ptr);
    
    return ptr->pcm_end - ptr->pcm_wrptr;

    return 0;
}

static int ring_buf_rd_distance(int dev)
{
    RING_BUF_DEVICE(ptr);
    
    return ptr->pcm_end - ptr->pcm_rdptr;
}

void ring_buf_init(int dev)
{
    struct ring_buf_s *ptr;

    if (dev == DEV_DSP)
    {
        ring_buf_dsp = (struct ring_buf_s *)SRAM_DSP_START_ADDR;
        ptr = ring_buf_dsp;
        ptr->pcm_start = RINGBUF_DSP_START_ADDR;
        ptr->pcm_size = RINGBUF_DSP_SIZE;
    }
    else
    {
        ring_buf_risc = (struct ring_buf_s *)SRAM_RISC_START_ADDR;
        ptr = ring_buf_risc;
        ptr->pcm_start = RINGBUF_RISC_START_ADDR;
        ptr->pcm_size = RINGBUF_RISC_SIZE;
    }

    ptr->init = 0;
    ptr->pcm_end = ptr->pcm_start + ptr->pcm_size;
    ptr->pcm_wrptr = ptr->pcm_rdptr = ptr->pcm_start;
    ptr->init = 1;
    printf("ring_buf_init done, dev %d\n", dev);
}

int ring_buf_size(int dev)
{
    RING_BUF_DEVICE(ptr);

    return ptr->pcm_size;
}

int ring_buf_space(int dev)
{
    RING_BUF_DEVICE(ptr);
    
    return ptr->pcm_size - ring_buf_fullness(dev);
}

int ring_buf_fullness(int dev)
{
    int val;

    RING_BUF_DEVICE(ptr);

    val = ptr->pcm_wrptr - ptr->pcm_rdptr;
    if (val < 0)
        val += ptr->pcm_size;

    return val;
}

int ring_buf_wr_data(int dev, char *buf, int len)
{
    int len1, len2;
    
    RING_BUF_DEVICE(ptr);
   
    //printf("### %s %d, len %d wrptr 0x%x rdptr 0x%x, space %d\n", __func__, __LINE__, len, ptr->pcm_wrptr, ptr->pcm_rdptr, ring_buf_space(dev));
    if (ring_buf_space(dev) <= len)
        return 0;

    if (len <= ring_buf_wr_distance(dev))
    {
        memcpy((void *)ptr->pcm_wrptr, buf, len);
        ring_buf_wr_update(dev, len);
    }
    else
    {
        len1 = ring_buf_wr_distance(dev);
        len2 = len - len1;
        memcpy((void *)ptr->pcm_wrptr, buf, len1);
        ring_buf_wr_update(dev, len1);
        memcpy((void *)ptr->pcm_wrptr, buf + len1, len2);
        ring_buf_wr_update(dev, len2);
    }

    return len;
}

int ring_buf_rd_data(int dev, char *buf, int len)
{
    int len1, len2;

    RING_BUF_DEVICE(ptr);
    
    //printf("### %s %d, len %d wrptr 0x%x rdptr 0x%x, fullness %d\n", __func__, __LINE__, len, ptr->pcm_wrptr, ptr->pcm_rdptr, ring_buf_fullness(dev));
    if (ring_buf_fullness(dev) <= len)
        return 0;

    if (len <= ring_buf_rd_distance(dev))
    {
        memcpy(buf, (void *)ptr->pcm_rdptr, len);
        ring_buf_rd_update(dev, len);
    }
    else
    {
        len1 = ring_buf_rd_distance(dev);
        len2 = len - len1;
        memcpy(buf, (void *)ptr->pcm_rdptr, len1);
        ring_buf_rd_update(dev, len1);
        memcpy(buf + len1, (void *)ptr->pcm_rdptr, len2);
        ring_buf_rd_update(dev, len2);
    }

    return len;
}

void ring_buf_reset()
{
    memset((void *)SRAM_START_ADDR, 0, SRAM_SIZE);
}
