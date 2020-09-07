#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tftp/packet.h"

/*creates packet structure from raw buffer*/
int create_error_packet_from_raw(char *buffer, int blen, packet *pckt)
{
    if (blen < 3)
        return 0;
    pckt->error_code  = (int)buffer[1];
    pckt->err_message = (char *)malloc(sizeof(char) * (blen - 2));
    strcpy(pckt->err_message, buffer + 2);
    return 1;
}

/*finds the tftp error type based on errno*/
int find_error()
{
    //Assumes errno is set and returns the
    //corresponding error packet type
    switch (errno) {
        case EACCES:
            return ACCESS_VIOLATION;
        case ENOMEM:
            return DISK_FULL;
        case ENOSPC:
            return DISK_FULL;
        case ENOENT:
            return FILE_NOT_FOUND;
        default:
            return NOT_DEFINED;
    }
}

/*copies the fields into buffer*/
int copy_err_header_to_buffer(int err_type, char *err_message, char *buffer)
{
    if (!(err_type >= NOT_DEFINED && err_type <= NO_USER))
        return 0;
    if (strlen(err_message) >= 512)
        return 0;
    buffer[0] = '\0';
    buffer[1] = '\5';
    buffer[2] = '\0';
    buffer[3] = (char)err_type;
    strcpy(buffer + 4, err_message);
    return 1;
}
