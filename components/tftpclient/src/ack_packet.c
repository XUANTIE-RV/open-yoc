#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tftp/packet.h"

/*copies the fields into the buffer*/
void copy_ack_header_to_buffer(int block_num, char *buffer)
{
    buffer[0] = '\0';
    buffer[1] = '\4';
    buffer[2] = (char)(block_num / 256);
    buffer[3] = (char)(block_num % 256);
}

/*Copies the fields in raw packet into the packet structure*/
int create_ack_packet_from_raw(char *buffer, int blen, packet *pckt)
{
    //fills in the required fields in packet structure
    int n1, n2;
    if (blen != 2)
        return 0;
    n1 = (int)buffer[0];
    n2 = (int)buffer[1];

    if (n1 < 0)
        n1 = 256 + n1;
    if (n2 < 0)
        n2 = 256 + n2;

    pckt->block_number = 256 * n1 + n2;
    return 1;
}

/*Some error messages*/
void print_ack_wait_timeout(int block_number)
{
    printf("Timeout. Resending ACK with block number %d\n", block_number);
}

//void print_ack_send_error(int block_number){
//	printf("Unable to send ACK packet with block number %d\n", block_number);
//	exit(1);
//}

void print_ack_blck_mismatch_error(int bnum, int e_bnum)
{
    printf("ACK packet with block number %d received, %d expected\n", bnum, e_bnum);
}