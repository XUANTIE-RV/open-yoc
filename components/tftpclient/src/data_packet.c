#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tftp/packet.h"

/*Creates a data packet from raw buffer*/
int create_data_packet_from_raw(char *buffer, int blen, packet *pckt)
{
    int n1, n2, i;
    //char *data;
    if (blen >= 2) {

        n1 = (int)buffer[0];
        n2 = (int)buffer[1];

        if (n1 < 0)
            n1 = 256 + n1;
        if (n2 < 0)
            n2 = 256 + n2;

        pckt->block_number = 256 * n1 + n2;
        pckt->data_buffer  = (char *)malloc(sizeof(char) * (blen - 1));
        for (i = 0; i < blen - 2; i++) {
            pckt->data_buffer[i] = buffer[i + 2];
        }
        pckt->data_buffer[blen - 2] = '\0';
        return 1;
    }
    return -1;
}

/* Copies the fields into buffer */
void copy_data_header_to_buffer(int block_number, char *data_buffer, int data_size, char *buffer)
{
    int i     = 0;
    buffer[0] = '\0';
    buffer[1] = '\3';
    buffer[2] = (char)(block_number / 256);
    buffer[3] = (char)(block_number % 256);
    for (i = 0; i < data_size; i++) {
        buffer[4 + i] = data_buffer[i];
    }
}

/*Some error messages*/

void print_data_wait_timeout(int block_number)
{
    printf("Timeout. resending the data packet with block number%d\n", block_number);
}

//void print_data_send_error(int block_number){
//	printf("Unable to send data packet with block_number %d\n", block_number);
//	exit(1);
//}

void print_data_blck_mismatch_error(int bnum, int e_bnum)
{
    printf("Data packet of block number %d received, %d expected\n", bnum, e_bnum);
}
