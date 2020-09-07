#ifndef _TFTP_CLIENT_H_
#include <stdio.h>

#define NETASCII 8      //Netascii mode
#define OCTET 9         //Octal mode  

#define RRQ 1           //A write request packet
#define WRQ 2           //A read request packet
#define DATA 3           //An acknowledgement packet
#define ACK 4          //A data packet
#define ERROR 5         //An error packet

typedef struct _tftp_file_opt {
    FILE *(*tftp_fopen)(const char *pathname, const char *mode);
    size_t (*tftp_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
    size_t (*tftp_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
    int (*tftp_fclose)(FILE *stream);
} tftp_file_opt_t;

int tftp_client_init(tftp_file_opt_t *file_opt);
int tftp_client_start(int opcode, char* filename, int mode, char *server_addr, char *port);

#endif
