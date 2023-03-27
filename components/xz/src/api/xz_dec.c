
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "xz.h"
#include "xz_dec.h"

static uint8_t out[XZ_OUT_BUFSIZE];
struct xz_dec *xz_decoder = NULL;

#define RAM_CODE_SECTION(func)  __attribute__((section(".__sram.code."#func)))  func
int RAM_CODE_SECTION(xz_img_dec_with_write)(uint8_t* img_addr, uint32_t img_size, uint8_t* write_addr, data_write_fun fun, void *ext_info);
int RAM_CODE_SECTION(xz_decompress)(uint8_t* src, uint32_t src_size, data_read_fun read_func, uint8_t* dst, uint32_t *out_size, data_write_fun write_func, void *ext_info);

static int xz_img_dec_init(void)
{
    if(!xz_decoder) {
        xz_crc32_init();
#ifdef XZ_USE_CRC64
        xz_crc64_init();
#endif
        /*Support up to 32 KB dictionary. The actually needed memory
         * is allocated once the headers have been parsed */
        xz_decoder = xz_dec_init(XZ_DYNALLOC, DICT_MAX_SIZE);
        if (xz_decoder == NULL) {
            printf("xz decoder init faild\r\n");
            return -1;;
        }
    }
    return 0;
}

int xz_img_dec_with_write(uint8_t* img_addr, uint32_t img_size, uint8_t* write_addr, data_write_fun fun, void *ext_info)
{
    if (xz_img_dec_init()) {
        return -1;
    }
    if(!xz_decoder) {
        printf("call init first\r\n");
        return -1;
    }

    if(!img_size || !fun) {
        return -1;
    }
    struct xz_buf b;
    enum xz_ret ret;

    b.in = img_addr;
    b.in_pos = 0;
    b.in_size = 0;
    b.out = out;
    b.out_pos = 0;
    b.out_size = XZ_OUT_BUFSIZE;

    while (true) {
        if (b.in_pos == b.in_size) {
            b.in = img_addr;
            b.in_size = img_size < XZ_OUT_BUFSIZE ? img_size : XZ_OUT_BUFSIZE;
            img_addr += b.in_size;
            img_size -= b.in_size;
            b.in_pos = 0;
        }

        ret = xz_dec_run(xz_decoder, &b);
        if (b.out_pos == sizeof(out)) {
            if(fun((unsigned long)write_addr, out, b.out_pos, ext_info) < 0) {
                printf("write faild\r\n");
                goto error;
            }
            write_addr += b.out_pos;
            b.out_pos = 0;
        }

        if (ret == XZ_OK)
            continue;

        if(b.out_pos && fun((unsigned long)write_addr,out, b.out_pos, ext_info) < 0) {
            printf("write faild\r\n");
            goto error;
        }

        if(!img_size) {
            ret = XZ_STREAM_END;
        }

        switch (ret) {
        case XZ_STREAM_END:
            xz_dec_end(xz_decoder);
		    xz_decoder = NULL;
            return 0;

        case XZ_MEM_ERROR:
            printf("Memory allocation failed\n");
            goto error;

        case XZ_MEMLIMIT_ERROR:
            printf("Memory usage limit reached\n");
            goto error;

        case XZ_FORMAT_ERROR:
            printf("Not a .xz file\n");
            goto error;

        case XZ_OPTIONS_ERROR:
            printf("Unsupported options in the .xz headers\n");
            goto error;

        case XZ_DATA_ERROR:
        case XZ_BUF_ERROR:
            printf("File is corrupt %d\n",ret);
            goto error;

        default:
            printf("Bug!\n");
            goto error;
        }
    }
error:
    xz_dec_end(xz_decoder);
	xz_decoder = NULL;
    return -1;
}

int xz_decompress(uint8_t* src, uint32_t src_size, data_read_fun read_func,
                 uint8_t* dst, uint32_t *out_size, data_write_fun write_func,
                 void *ext_info)
{
    uint32_t olen;
    uint8_t *in_buffer = NULL;

    if (xz_img_dec_init()) {
        return -1;
    }
    if(!xz_decoder) {
        printf("call init first\r\n");
        return -1;
    }

    if(!src_size) {
        return -EINVAL;
    }
    struct xz_buf b;
    enum xz_ret ret;
    size_t read_size = src_size < XZ_OUT_BUFSIZE ? src_size : XZ_OUT_BUFSIZE;

    in_buffer = src;
    if (read_func) {
        in_buffer = malloc(XZ_OUT_BUFSIZE);
        if (!in_buffer) {
            return -ENOMEM;
        }
    }

    b.in = in_buffer;
    b.in_pos = 0;
    b.in_size = 0;
    b.out = out;
    b.out_pos = 0;
    b.out_size = XZ_OUT_BUFSIZE;

    olen = 0;

    while (true) {
        if (b.in_pos == b.in_size) {
            read_size = src_size < XZ_OUT_BUFSIZE ? src_size : XZ_OUT_BUFSIZE;
            if (read_func) {
                if (read_func((unsigned long)src, in_buffer, read_size, ext_info) < 0) {
                    goto error;
                }
                b.in = in_buffer;
            } else {
                b.in = src;
            }
            b.in_size = read_size;
            src += b.in_size;
            src_size -= b.in_size;
            b.in_pos = 0;
        }

        ret = xz_dec_run(xz_decoder, &b);
        if (b.out_pos == sizeof(out)) {
            if (write_func) {
                if(write_func((unsigned long)dst, out, b.out_pos, ext_info) < 0) {
                    printf("write faild\r\n");
                    goto error;
                }
            } else {
                memcpy(dst, out, b.out_pos);
            }
            dst += b.out_pos;
            olen += b.out_pos;
            b.out_pos = 0;
        }

        if (ret == XZ_OK)
            continue;

        if(b.out_pos) {
            if (write_func) {
                if (write_func((unsigned long)dst, out, b.out_pos, ext_info) < 0) {
                    printf("write faild\r\n");
                    goto error;
                }
            } else {
                memcpy(dst, out, b.out_pos);
            }
            olen += b.out_pos;
        }

        if(!src_size) {
            ret = XZ_STREAM_END;
        }

        switch (ret) {
        case XZ_STREAM_END:
            xz_dec_end(xz_decoder);
		    xz_decoder = NULL;
            if (out_size) *out_size = olen;
            return 0;

        case XZ_MEM_ERROR:
            printf("Memory allocation failed\n");
            goto error;

        case XZ_MEMLIMIT_ERROR:
            printf("Memory usage limit reached\n");
            goto error;

        case XZ_FORMAT_ERROR:
            printf("Not a .xz file\n");
            goto error;

        case XZ_OPTIONS_ERROR:
            printf("Unsupported options in the .xz headers\n");
            goto error;

        case XZ_DATA_ERROR:
        case XZ_BUF_ERROR:
            printf("File is corrupt %d\n",ret);
            goto error;

        default:
            printf("Bug! ret:%d\n", ret);
            goto error;
        }
    }
error:
    if (read_func) {
        if (in_buffer) free(in_buffer);
    }
    xz_dec_end(xz_decoder);
	xz_decoder = NULL;
    return -1;
}