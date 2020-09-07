#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tftp/packet.h"

/*copies fields into buffer*/
void *copy_request_header_to_buffer(int opcode, char *filename, int mode, char *buffer)
{
    //Returns the raw header for read request

    int   cur_size = 0;
    char *mode_s   = NULL;

    if (mode == NETASCII) {
        mode_s = (char *)malloc(sizeof(char) * 9);
        strcpy(mode_s, "netascii");
    } else if (mode == OCTET) {
        mode_s = (char *)malloc(sizeof(char) * 6);
        strcpy(mode_s, "octet");
    }

    if (mode_s == NULL)
        return NULL;

    int i = 0, len = strlen(filename);

    buffer[0] = '\0';
    if (opcode == RRQ)
        buffer[1] = '\1';
    else if (opcode == WRQ)
        buffer[1] = '\2';
    for (i = 0; i < len; i++) {
        buffer[2 + i] = filename[i];
    }
    cur_size         = len + 2;
    buffer[cur_size] = '\0';
    cur_size++;
    len = strlen(mode_s);
    for (i = 0; i < len; i++) {
        buffer[cur_size] = mode_s[i];
        cur_size++;
    }
    buffer[cur_size++] = '\0';

    return NULL;
}

/*Some helper functions*/
int is_netascii(int mode)
{
    if (mode == NETASCII)
        return 1;
    return 0;
}

int is_octet(int mode)
{
    if (mode == OCTET)
        return 1;
    return 0;
}

/*Some error messages*/
//void print_read_send_error(){
//    printf("Unable to send read request\n");
//    exit(1);
//}

//void print_write_send_error(){
//    printf("Unable to send write request\n");
//    exit(1);
//}

void print_read_timeout()
{
    printf("Read request timed out\n");
}

void print_write_timeout()
{
    printf("Write request timed out\n");
}
