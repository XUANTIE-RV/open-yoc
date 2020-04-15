#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#define DEV_DSP  1 //DSP TO RISC
#define DEV_RISC 2 //RISC TO DSP

struct ring_buf_s
{
    int pcm_start;
    int pcm_end;
    int pcm_size;
    int pcm_wrptr; //存储地址
    int pcm_rdptr; //存储地址
    int init;
};

extern int ring_buf_size(int dev);
extern int ring_buf_space(int dev);
extern int ring_buf_fullness(int dev);
extern int ring_buf_wr_data(int dev, char *buf, int len);
extern int ring_buf_rd_data(int dev, char *buf, int len);
extern void ring_buf_init(int dev);
extern void ring_buf_reset(void);

#endif

