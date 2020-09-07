#include "app_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <aos/cli.h>
#include <aos/log.h>
#include "tftp/tftp_client.h"

#define TAG "tftp"

extern uint32_t g_mic_data[];
extern uint32_t g_ref_data[];
extern uint32_t g_pcm_data[];
extern uint32_t g_mic_len;
extern uint32_t g_ref_len;
extern uint32_t g_pcm_len;

static char g_mime_name[64] = {0};

static uint8_t *get_rec_mem_addr(void)
{
    if (!strcmp(g_mime_name, "mic")) {
        return (uint8_t *)g_mic_data;
    } else if (!strcmp(g_mime_name, "ref")) {
        return (uint8_t *)g_ref_data;
    } else if (!strcmp(g_mime_name, "pcm")) {
        return (uint8_t *)g_pcm_data;
    }
    return NULL;
}

static uint32_t pcm_to_mem_size(void)
{
    if (!strcmp(g_mime_name, "mic")) {
        return g_mic_len;
    } else if (!strcmp(g_mime_name, "ref")) {
        return g_ref_len;
    } else if (!strcmp(g_mime_name, "pcm")) {
        return g_pcm_len;
    }
    return 0;
}

static int g_fread_pos = 0;
static FILE *tftp_fopen(const char *pathname, const char *mode)
{
    g_fread_pos = 0;
    LOGI(TAG, "open %s, %s, [%s, %d bytes]\n", pathname, mode, g_mime_name, pcm_to_mem_size());

    if (pcm_to_mem_size() <= 0) {
        return NULL;
    }
    return (FILE *)1;
}

static size_t tftp_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int bytes = size * nmemb;
    int left_bytes = pcm_to_mem_size() - g_fread_pos - 4; /* 少发4个字节，先绕过tftp没法发送512倍数数据量的最后一包 */

    if (left_bytes < bytes) {
        bytes     = left_bytes;
    }

    uint8_t *pcm_record = (uint8_t *)get_rec_mem_addr();
    if (pcm_record == NULL) {
        return 0;
    }
    memcpy(ptr, &pcm_record[g_fread_pos], bytes);

    g_fread_pos += bytes;

    return bytes;
#if 0
    static int file_size = 1025;
    int bytes = size * nmemb;
    printf("tftp read %d,%d\n", bytes, file_size);

    memset(ptr, '1', bytes);

    if (file_size < bytes) {
        bytes     = file_size;
        file_size = 1025;
    } else {
        file_size -= bytes;
    }

    return bytes;
#endif
}

static size_t tftp_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    LOGI(TAG, "write %d, %s\n", size * nmemb, g_mime_name);
    return 0;
}

static int tftp_fclose(FILE *stream)
{
    g_fread_pos = 0;
    LOGI(TAG, "close [%s]\n", g_mime_name);
    return 0;
}

tftp_file_opt_t g_tftp_file_opt = {
    tftp_fopen,
    tftp_fread,
    tftp_fwrite,
    tftp_fclose
};

static void print_help()
{
    printf("Usage:\n\nclient [-h] [type] [filepath] [hostname] [port]\n\n");
    printf("-h\tShows this usage instructions\n\n");
    printf("Type:\n\n");
    printf("-ra\tRequests a read in NETASCII mode\n");
    printf("-ro\tRequests a read in OCTET Mode\n");
    printf("-wa\tRequests a write in NETASCII mode\n");
    printf("-wo\tRequests a write in OCTET mode\n\n");
    printf("filepath\tPath to file to read or write\n");
    printf("hostname\tIP Address or hostname\n");
    printf("port\t\tOptional.Default is 69\n\n");
}

//  tftp -mic -wo mic.raw 192.168.1.101
static void cmd_app_func(char *wbuf, int wbuf_len, int argc, char **argv)
{

    int mode;
    int type;

    if (argc != 5 && argc != 3 && argc != 6)
        printf("Invalid arguments provided. Use -h for help");

    char *mime = argv[1];
    if (!strcmp(mime, "-mic")) {
        strncpy(g_mime_name, "mic", sizeof(g_mime_name) - 1);
    } else if (!strcmp(mime, "-ref")) {
        strncpy(g_mime_name, "ref", sizeof(g_mime_name) - 1);
    } else if (!strcmp(mime, "-pcm")) {
        strncpy(g_mime_name, "pcm", sizeof(g_mime_name) - 1);
    }

    char *tmode = argv[2];
    if (!strcmp(tmode, "-ra")) {
        type = RRQ;
        mode = NETASCII;
    } else if (!strcmp(tmode, "-ro")) {
        type = RRQ;
        mode = OCTET;
    } else if (!strcmp(tmode, "-wa")) {
        type = WRQ;
        mode = NETASCII;
    } else if (!strcmp(tmode, "-wo")) {
        type = WRQ;
        mode = OCTET;
    } else if (!strcmp(tmode, "-h")) {
        print_help();
        return;
    } else {
        printf("Invalid arguments provided. Use -h for help");
        return;
    }

    if (argc == 5) {
        tftp_client_start(type, argv[3], mode, argv[4], argv[5]);
    } else {
        tftp_client_start(type, argv[3], mode, argv[4], NULL);
    }
}

void cli_tftps_cmd(void)
{
    static const struct cli_command cmd_info = {"tftp", "tftps 0/1", cmd_app_func};

    aos_cli_register_command(&cmd_info);

    tftp_client_init(&g_tftp_file_opt);
}
