#define LOG_TAG  "TINYFS_WRITE_CACHE"

#include <string.h>
#include "tinyfs_write_cache.h"
#include "tinyfs_nvm.h"



static struct{
    uint32_t current_buf_offset;
    uint16_t idx;
    uint8_t buf[TINYFS_WRITE_CACHE_SIZE];
}tinyfs_wr_cache_env;

void tinyfs_nvm_write_through()
{
    if(tinyfs_wr_cache_env.idx)
    {
        if(tinyfs_wr_cache_env.idx < TINYFS_WRITE_CACHE_SIZE)
        {
            memset(&tinyfs_wr_cache_env.buf[tinyfs_wr_cache_env.idx],0xff,TINYFS_WRITE_CACHE_SIZE-tinyfs_wr_cache_env.idx);
        }
        nvm_program(tinyfs_wr_cache_env.current_buf_offset,tinyfs_wr_cache_env.buf,TINYFS_WRITE_CACHE_SIZE);
        tinyfs_wr_cache_env.idx = 0;
    }
}

void tinyfs_nvm_program(uint32_t offset,uint32_t length,uint8_t *buffer)
{
    while(length)
    {
        tinyfs_wr_cache_env.current_buf_offset = offset & ~(TINYFS_WRITE_CACHE_SIZE - 1);
        uint16_t buf_available = TINYFS_WRITE_CACHE_SIZE - tinyfs_wr_cache_env.idx;
        if(length<=buf_available)
        {
            memcpy(&tinyfs_wr_cache_env.buf[tinyfs_wr_cache_env.idx],buffer,length);
            tinyfs_wr_cache_env.idx += length;
            length = 0;
        }else{
            memcpy(&tinyfs_wr_cache_env.buf[tinyfs_wr_cache_env.idx],buffer,buf_available);
            length -= buf_available;
            buffer += buf_available;
            offset += buf_available;
            tinyfs_wr_cache_env.idx += buf_available;
        }
        if(tinyfs_wr_cache_env.idx == TINYFS_WRITE_CACHE_SIZE)
        {
            tinyfs_nvm_write_through();
        }
    }
}

void tinyfs_nvm_read_with_cache(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    uint32_t nvm_read_length; 
    uint8_t *cache_src = tinyfs_wr_cache_env.buf;
    if(offset+length>tinyfs_wr_cache_env.current_buf_offset&&offset+length<=tinyfs_wr_cache_env.current_buf_offset + tinyfs_wr_cache_env.idx)
    {
        if(offset<tinyfs_wr_cache_env.current_buf_offset)
        {
            nvm_read_length = length - ((offset+length)&(TINYFS_WRITE_CACHE_SIZE-1));
        }else
        {
            cache_src += offset & (TINYFS_WRITE_CACHE_SIZE-1);
            nvm_read_length = 0;
        }       
    }else
    {
        nvm_read_length = length;
    }
    if(nvm_read_length)
    {
        nvm_read(offset,buffer,nvm_read_length);
        buffer += nvm_read_length;
        length -= nvm_read_length;
    }
    if(length)
    {
        memcpy(buffer,cache_src,length);
    }
}

