/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

#include "lwip/apps/tftp_client.h"
#include "lwip/apps/tftp_server.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"

#if LWIP_UDP

/* Define a base directory for TFTP access
 * ATTENTION: This code does NOT check for sandboxing,
 * i.e. '..' in paths is not checked! */
#ifndef LWIP_TFTP_EXAMPLE_BASE_DIR
#define LWIP_TFTP_EXAMPLE_BASE_DIR ""
#endif

/* Define this to a file to get via tftp client */
#ifndef LWIP_TFTP_EXAMPLE_CLIENT_FILENAME
#define LWIP_TFTP_EXAMPLE_CLIENT_FILENAME "yoc.bin"
#endif

/* Define this to a server IP string */
#ifndef LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP
#define LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP "192.168.1.2"
#endif

static uint32_t flash_base_addr;
//static csi_spiflash_info_t info;


static void *
tftp_open_file(const char* fname, u8_t is_write)
{
  // snprintf(full_filename, sizeof(full_filename), "%s%s", LWIP_TFTP_EXAMPLE_BASE_DIR, fname);
  // full_filename[sizeof(full_filename)-1] = 0;

  // if (is_write) {
  //   //return (void*)fopen(full_filename, "wb");
  // } else {
  //   //return (void*)fopen(full_filename, "rb");
  // }
  
  
  return "spiflash_handle";
}

static void*
tftp_open(const char* fname, const char* mode, u8_t is_write)
{
  // int ret;
  // csi_spiflash_t *spiflash_handle = (csi_spiflash_t *)malloc(sizeof(csi_spiflash_t));


  // ret = csi_spiflash_spi_init(spiflash_handle, 0, NULL);
	// if (ret < 0) {
	// 	printf("spiflash init error, actual return value is %d\n", ret);
	// 	return NULL;
	// }

  // ret = csi_spiflash_get_flash_info(spiflash_handle, &info);
	// if (ret < 0) {
	// 	printf("spiflash get flash info error, value is %d\n", ret);
	// 	return ret;
	// }

  // return spiflash_handle;
  return "spiflash_handle";
}

static void
tftp_close(void* handle)
{
  // csi_spiflash_t *spiflash_handle = (csi_spiflash_t *)handle;
  // csi_spiflash_spi_uninit(spiflash_handle);
}

static int
tftp_read(void* handle, void* buf, int bytes)
{
  // csi_spiflash_t *spiflash_handle = (csi_spiflash_t *)handle;
  // ret = csi_spiflash_read(spiflash_handle, spiflash_args.offset, buf, spiflash_args.size);
	// if (ret < 0) {
	// 	printf("spiflash spi read error!\n");
	// 	return ret;
	// }
  return 0;
}

static int
tftp_write(void* handle, struct pbuf* p)
{
  // static int offset = 0;
  // static int sector = 0;

  // sector = ((p->tot_len - 1)/info.sector_size + 1);
  // int ret;
  // csi_spiflash_t *spiflash_handle = (csi_spiflash_t *)handle;
  // ret = csi_spiflash_erase(spiflash_handle, flash_base_addr + sector*info.sector_size, sector*info.sector_size);
	// if (ret != 0) {
	// 	printf("spiflash spi erase error!\n");
	// 	return ret;
	// }

  // while (p != NULL) {
  //   ret = csi_spiflash_program(spiflash_handle, flash_base_addr + offset, p->payload, p->len);
  //   if (ret < 0) {
  //     printf("spiflash spi program error!\n");
  //     return ret;
  //   }
  //   p = p->next;
  //   offset += p->len;
  // }

  return 0;
}

/* For TFTP client only */
static void
tftp_error(void* handle, int err, const char* msg, int size)
{
  char message[100];

  LWIP_UNUSED_ARG(handle);

  memset(message, 0, sizeof(message));
  MEMCPY(message, msg, LWIP_MIN(sizeof(message)-1, (size_t)size));

  printf("TFTP error: %d (%s)", err, message);
}

static const struct tftp_context tftp = {
  tftp_open,
  tftp_close,
  tftp_read,
  tftp_write,
  tftp_error
};

void
tftp_init(int mode, void *arg)
{
  switch (mode){
  case 1:{
    int ret;
    void *f;
    err_t err;
    ip_addr_t srv;
    struct tftp_info *info = (struct tftp_info *)arg;
    char *ptr;

    flash_base_addr = (uint32_t)strtol(info->flash_save_addr, &ptr, 16);
    printf("TFTP from server %s; Filename '%s'.\n", info->server_ip, info->file_name);
    printf("Load address: %#x\n",flash_base_addr);
    ret = ipaddr_aton(info->server_ip, &srv);
    LWIP_ASSERT("ipaddr_aton failed", ret == 1);

    err = tftp_init_client(&tftp);
    LWIP_ASSERT("tftp_init_client failed", err == ERR_OK);

    f = tftp_open_file(info->file_name, 1);
    LWIP_ASSERT("failed to create file", f != NULL);

    err = tftp_get(f, &srv, TFTP_PORT, info->file_name, TFTP_MODE_OCTET);
    LWIP_ASSERT("tftp_get failed", err == ERR_OK);
    break;
  }
  case 2:
    tftp_init_server(&tftp);
    break;
  default:
    printf("tftp mode err\n");
    break;
  }
}

#endif /* LWIP_UDP */
