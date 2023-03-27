
#ifndef __XZ_DEC_H
#define __XZ_DEC_H

// will malloc in heap
#ifndef CONFIG_DICT_MAX_SIZE
// (1 << 15)
#define DICT_MAX_SIZE (0x8000)
#else
#define DICT_MAX_SIZE CONFIG_DICT_MAX_SIZE
#endif

#ifndef CONFIG_XZ_OUT_BUFSIZE
#define XZ_OUT_BUFSIZE 4096
#else
#define XZ_OUT_BUFSIZE CONFIG_XZ_OUT_BUFSIZE
#endif

typedef int (*data_write_fun)(unsigned long write_addr, void *data, size_t size, void *ext_info);
typedef int (*data_read_fun)(unsigned long read_addr, void *buffer, size_t size, void *ext_info);

int xz_img_dec_with_write(uint8_t* img_addr, uint32_t img_size, uint8_t* write_addr, data_write_fun fun, void *ext_info);

/**
 * @brief xz解压函数，可支持 内存/Flash 解压到 内存/Flash
 * @param[in] src 源数据地址
 * @param[in] src_size 源数据长度
 * @param[in] read_func 源数据读取函数，传入NULL表示从内存获取
 * @param[in] dst 目的地址
 * @param[out] out_size 解压之后的数据长度
 * @param[in] write_func 解压之后的数据写入函数，传入NULL表示直接写到内存
 * @param[in] ext_info Flash设备信息，如设备类型等
 * @return 0 : success; other: failed
 */
int xz_decompress(uint8_t* src, uint32_t src_size, data_read_fun read_func,
                 uint8_t* dst, uint32_t *out_size, data_write_fun write_func,
                 void *ext_info);

#endif
