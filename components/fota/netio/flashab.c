/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <stdio.h>
#include <aos/kernel.h>
#include <yoc/netio.h>
#include <yoc/ota_ab.h>
#include <yoc/ota_ab_img.h>
#include <bootab.h>

#define TAG "fotaab"

#if (CONFIG_FOTA_BUFFER_SIZE < 1024)
#error "the fota buffer_size must be more than 1024B"
#endif

#ifndef __linux__
#include <aos/aos.h>
#include <yoc/partition.h>

static uint32_t get_checksum(uint8_t *data, uint32_t length)
{
    uint32_t cksum = 0;
    for (int i = 0; i < length; i++) {
        // if (i % 16 == 0) printf("\n");
        // printf("0x%02x ", *data);
        cksum += *data++;
    }
    // printf("\n");
    return cksum;
}

static int get_img_index(netio_t *io, size_t cur_offset)
{
    int i;
    size_t offsets[PACK_IMG_MAX_COUNT + 1];

    download_img_info_t *dl_img_info = &((pack_private_t *)io->priv)->dl_imgs_info;
    LOGD(TAG, "%s, cur_offset: %d", __func__, cur_offset);
    if (cur_offset == 0) {
        return 0;
    }
    for (i = 0; i < PACK_IMG_MAX_COUNT + 1; i++) {
        offsets[i] = ~0;
    }
    for (i = 0; i < dl_img_info->image_count; i++) {
        offsets[i] = dl_img_info->img_info[i].img_offset;
        LOGD(TAG, "offsets[%d]: %d", i, offsets[i]);
    }
    for (i = 0; i < dl_img_info->image_count; i++) {
        LOGD(TAG, "offsets[%d]: %d, offsets[%d]: %d", i, offsets[i], i+1, offsets[i+1]);
        if(cur_offset >= offsets[i] && cur_offset < offsets[i+1]) {
            LOGD(TAG, "Range Num: %d", i);
            return i;
        }
    }
    LOGE(TAG, "get img index error.");
    return -1;
}

int images_info_init(uint8_t *header_buffer, download_img_info_t *dl_img_info)
{
    int i;
    pack_header_v2_t *header = (pack_header_v2_t *)header_buffer;
    pack_header_imginfo_v2_t *imginfo = header->image_info;

    LOGD(TAG, "come to images info init.");
    if (header->magic != PACK_HEAD_MAGIC) {
        LOGE(TAG, "the image header is wrong.[0x%x]", header->magic);
        return -1;
    }
    if (header->image_count > IMG_MAX_COUNT) {
        LOGE(TAG, "the image count is overflow.");
        return -1;
    }
    memset(dl_img_info, 0, sizeof(download_img_info_t));
    dl_img_info->image_count = header->image_count;
    dl_img_info->head_size = header->head_size;
    dl_img_info->digest_type = header->digest_type;
    dl_img_info->signature_type = header->signature_type;
    memcpy(dl_img_info->md5sum, header->md5sum, sizeof(dl_img_info->md5sum));
    memcpy(dl_img_info->signature, header->signature, sizeof(dl_img_info->signature));
    LOGD(TAG, "image count is :%d", header->image_count);
    for (i = 0; i < header->image_count; i++) {
        LOGD(TAG, "-------> %s", imginfo->img_name);
        LOGD(TAG, "offset:%d", imginfo->offset);
        LOGD(TAG, "size:%d", imginfo->size);
        LOGD(TAG, "preload_size:%d", imginfo->preload_size);
        // snprintf(dl_img_info->img_info[i].partition_name, sizeof(dl_img_info->img_info[i].partition_name), "%s%s", imginfo->img_name, otaab_get_next_ab());
        strlcpy(dl_img_info->img_info[i].partition_name, imginfo->img_name, sizeof(dl_img_info->img_info[i].partition_name) - 1);
        strcat(dl_img_info->img_info[i].partition_name, otaab_get_next_ab());
        partition_t hdl = partition_open(dl_img_info->img_info[i].partition_name);
        if (hdl < 0) {
            LOGE(TAG, "open partition %s failed. maybe it is not existed", dl_img_info->img_info[i].partition_name);
            return -1;
        }
        partition_info_t *part_info = partition_info_get(hdl);
        if (part_info == NULL) {
            LOGE(TAG, "get %s info failed.", dl_img_info->img_info[i].partition_name);
            return -1;
        }
        if (imginfo->size > part_info->length) {
            LOGE(TAG, "imgsize overflow, [%s, %d > %d]", imginfo->img_name, imginfo->size, part_info->length);
            return -1;
        }
        dl_img_info->img_info[i].partition_handle = hdl;
        dl_img_info->img_info[i].partition_abs_addr = part_info->base_addr + part_info->start_addr;
        dl_img_info->img_info[i].partition_size = part_info->length;
        dl_img_info->img_info[i].write_size = 0;
        dl_img_info->img_info[i].read_size = 0;
        dl_img_info->img_info[i].img_offset = imginfo->offset;
        dl_img_info->img_info[i].img_size = imginfo->size;
        dl_img_info->img_info[i].preload_size = imginfo->preload_size;
        partition_close(hdl);
        imginfo++;
    }
    return 0;
}

static int flash_open(netio_t *io, const char *path)
{
    LOGD(TAG, "%s", __func__);
    if (sizeof(pack_header_v2_t) > OTA_AB_IMG_HEAD_SIZE) {
        LOGE(TAG, "the head size is not matched.");
        return -1;
    }
    partition_t handle = partition_open(B_ENVAB_NAME);

    if (handle >= 0) {
        partition_info_t *lp = partition_info_get(handle);
        aos_assert(lp);

        pack_private_t *pack_private = aos_zalloc(sizeof(pack_private_t));
        if (pack_private == NULL) {
            return -1;
        }
        pack_private->envab_handle = handle;

        io->size = lp->length;
        io->block_size = lp->erase_size;
        io->priv = (void *)pack_private;
        return 0;
    }

    return -1;
}

static int flash_close(netio_t *io)
{
    LOGD(TAG, "%s", __func__);
    pack_private_t *pack_private = (pack_private_t *)io->priv;
    if (pack_private) {
        partition_t handle = pack_private->envab_handle;
        partition_close(handle);
        aos_free(pack_private);
        pack_private = NULL;
    }
    return 0;
}

static int flash_read(netio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    LOGD(TAG, "%s", __func__);
    return 0;
}

static int fota_flash_write(const char *partition_name, int block_size, off_t off_set, uint8_t *data, int length)
{
    if (length == 0) {
        return 0;
    }
    LOGD(TAG, "fota write partition off_set:0x%lx length:0x%x", off_set, length);
    int ret = otaab_upgrade_partition_slice(partition_name, NULL, off_set, length, 0, (char *)data);
    if (ret < 0) {
        LOGE(TAG, "write partition failed. off_set:0x%x length:0x%x", off_set, length);
        return ret;
    }
    return length;
}

static int flash_write(netio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    int ret;
    int headsize = 0;
    pack_header_v2_t *header;
    pack_private_t *priv = (pack_private_t *)io->priv;
    download_img_info_t *dl_img_info = &priv->dl_imgs_info;

    LOGD(TAG, "\n%s, %d %d %d", __func__, io->size, io->offset, length);
    if (priv == NULL) {
        return -1;
    }
    if (io->size < sizeof(pack_header_v2_t)) {
        LOGE(TAG, "the images size is too small, %d", io->size);
        return -1;
    }
    if (io->offset == 0) {
        header = (pack_header_v2_t *)buffer;
        if (header->magic == PACK_HEAD_MAGIC) {
            LOGD(TAG, "i am the pack image.");
            if (length < sizeof(pack_header_v2_t)) {
                LOGE(TAG, "the first size %d is less than %d", length, sizeof(pack_header_v2_t));
                return -1;
            }
            LOGD(TAG, "head_version:%d, head_size:%d, checksum:0x%08x, count:%d, digest:%d, signature:%d",
                        header->head_version, header->head_size, header->head_checksum, header->image_count,
                        header->digest_type, header->signature_type);
            if (header->head_version != 2) {
                LOGE(TAG, "the head version is not matched.");
                return -1;
            }
            headsize = header->head_size;
            uint8_t *tempbuf = aos_zalloc(headsize);
            if (!tempbuf) {
                return -ENOMEM;
            }
            memcpy(tempbuf, buffer, headsize);
            ((pack_header_v2_t *)tempbuf)->head_checksum = 0;
            memset(((pack_header_v2_t *)tempbuf)->signature, 0, sizeof(((pack_header_v2_t *)tempbuf)->signature));
            uint32_t cksum = get_checksum((uint8_t *)tempbuf, headsize);
            aos_free(tempbuf);
            if (cksum != header->head_checksum) {
                LOGE(TAG, "the header checksum error.[0x%08x, 0x%08x]", header->head_checksum, cksum);
                return -1;
            }
            off_t offset = OTA_AB_IMG_INFO_OFFSET_GET(io->block_size);
            ret = partition_erase_size(priv->envab_handle, offset, sizeof(pack_header_v2_t));
            if (ret < 0) {
                return ret;
            }
            ret = partition_write(priv->envab_handle, offset, buffer, sizeof(pack_header_v2_t));
            if (ret < 0) {
                return ret;
            }
            ret = images_info_init(buffer, dl_img_info);
            if (ret < 0) {
                return ret;
            }
            LOGD(TAG, "parse packed image ok.");
        } else {
            LOGE(TAG, "the image is not a pack image.");
            return -1;
        }
    }
    int idx = get_img_index(io, io->offset);
    if (idx < 0) {
        LOGE(TAG, "flash write error.");
        return -1;
    }
    LOGD(TAG, "idx:%d, dl_img_info->img_info[%d].img_offset:%d", idx, idx, dl_img_info->img_info[idx].img_offset);
    if (dl_img_info->img_info[idx].partition_size - (io->offset - dl_img_info->img_info[idx].img_offset) < length) {
        length = dl_img_info->img_info[idx].partition_size - (io->offset - dl_img_info->img_info[idx].img_offset);
    }
    int real_to_write_len = length - headsize;
    if (dl_img_info->img_info[idx].write_size + real_to_write_len > dl_img_info->img_info[idx].img_size) {
        int leftsize = dl_img_info->img_info[idx].img_size - dl_img_info->img_info[idx].write_size;
        ret = fota_flash_write(dl_img_info->img_info[idx].partition_name, io->block_size, dl_img_info->img_info[idx].write_size, &buffer[headsize], leftsize);
        if (ret < 0) {
            LOGE(TAG, "write leftsize %d bytes failed", leftsize);
            return -1;
        }
        LOGD(TAG, "write leftsize %d bytes ok", leftsize);
        dl_img_info->img_info[idx].write_size += leftsize;
        int remainsize = real_to_write_len - leftsize;
        if (dl_img_info->img_info[idx + 1].partition_name[0] != 0) {
            ret = fota_flash_write(dl_img_info->img_info[idx + 1].partition_name, io->block_size, dl_img_info->img_info[idx + 1].write_size, &buffer[headsize + leftsize], remainsize);
            if (ret < 0) {
                LOGE(TAG, "write remainsize %d bytes failed", remainsize);
                return -1;
            }
            LOGD(TAG, "write remainsize %d bytes ok", remainsize);
            dl_img_info->img_info[idx + 1].write_size += remainsize;
        }
    } else {
        ret = fota_flash_write(dl_img_info->img_info[idx].partition_name, io->block_size, dl_img_info->img_info[idx].write_size,  &buffer[headsize], real_to_write_len);
        if (ret < 0) {
            LOGE(TAG, "write real_to_write_len %d bytes failed", real_to_write_len);
            return -1;
        }
        LOGD(TAG, "write real_to_write_len %d bytes ok", real_to_write_len);
        dl_img_info->img_info[idx].write_size += real_to_write_len;
    }

    io->offset += length;
    LOGD(TAG, "write %d ok.\n", length);
    return length;
}

static int flash_seek(netio_t *io, size_t offset, int whence)
{
    pack_private_t *priv = (pack_private_t *)io->priv;
    download_img_info_t *dl_img_info = &priv->dl_imgs_info;

    LOGD(TAG, "%s, io->block_size:%d, offset:%d", __func__, io->block_size, offset);
    if (offset > 0) {
        uint8_t *buffer = aos_zalloc(sizeof(pack_header_v2_t));
        if (buffer == NULL) {
            return -ENOMEM;
        }
        int ret = partition_read(priv->envab_handle, OTA_AB_IMG_INFO_OFFSET_GET(io->block_size), buffer, sizeof(pack_header_v2_t));
        if (ret < 0) {
            aos_free(buffer);
            return ret;
        }
        ret = images_info_init(buffer, dl_img_info);
        aos_free(buffer);
        if (ret < 0) {
            return ret;
        }
    }
    int idx = get_img_index(io, offset);
    if (idx < 0) {
        LOGE(TAG, "flash seek error.");
        return -1;
    }
    switch (whence) {
        case SEEK_SET:
            io->offset = offset;
            dl_img_info->img_info[idx].write_size = offset - dl_img_info->img_info[idx].img_offset;
            LOGD(TAG, "dl_img_info->img_info[%d].write_size:0x%x", idx, dl_img_info->img_info[idx].write_size);
            return 0;
        default:
            break;
    }
    return -1;
}
#else
typedef int partition_t;

static int flash_open(netio_t *io, const char *path)
{
    return 0;
}

static int flash_close(netio_t *io)
{
    return 0;
}

static int flash_read(netio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    return 0;
}

static int flash_write(netio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    return 0;
}

static int flash_seek(netio_t *io, size_t offset, int whence)
{
    switch (whence) {
        case SEEK_SET:
            io->offset = offset;
            return 0;
        case SEEK_CUR:
            io->offset += offset;
            return 0;
        case SEEK_END:
            io->offset = io->size - offset;
            return 0;
    }

    return -1;
}
#endif /* __linux__ */

const netio_cls_t flashab = {
    .name = "flashab",
    .open = flash_open,
    .close = flash_close,
    .write = flash_write,
    .read = flash_read,
    .seek = flash_seek,
};

int netio_register_flashab(void)
{
    return netio_register(&flashab);
}
#endif /*CONFIG_OTA_AB*/