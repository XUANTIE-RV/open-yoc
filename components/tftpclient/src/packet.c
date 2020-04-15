#include "tftp/packet.h"
#include <stdio.h>

void init_packet(packet *pckt)
{
    //Initializes a packet structure

    pckt->opcode       = -1;
    pckt->mode         = -1;
    pckt->block_number = -1;
    pckt->error_code   = -1;
}

/*helper functions*/
int is_read_request(int opcode)
{
    return opcode == RRQ;
}

int is_write_request(int opcode)
{
    return opcode == WRQ;
}

int is_data_packet(int opcode)
{
    return opcode == DATA;
}

int is_error_packet(int opcode)
{
    return opcode == ERROR;
}

int is_ack_packet(int opcode)
{
    return (opcode == ACK);
}

/*creates the packet structure from buffer*/
/*Calls the corresponding functions for each type of packet*/
int strip_raw_packet(char *buffer, int blen, packet *pckt)
{
    int opcode;
    if (buffer[0] != '\0') {
        printf("Malformed packet\n");
        return 0;
    } else {
        if (!(blen > 1)) {
            printf("Malformed packet\n");
            return 0;
        }
        opcode = (int)buffer[1];
        switch (opcode) {
            case RRQ:
                //printf("Read request packet");
                break;
            case WRQ:
                //printf("Write request packet\n");
                break;
            case DATA:
                //printf("Data packet\n");
                pckt->opcode = DATA;
                return create_data_packet_from_raw(buffer + 2, blen - 2, pckt);
                break;
            case ACK:
                //printf("Acknowledgement packet\n");
                pckt->opcode = ACK;
                return create_ack_packet_from_raw(buffer + 2, blen - 2, pckt);
                break;
            case ERROR:
                //printf("Error packet\n");
                pckt->opcode = ERROR;
                return create_error_packet_from_raw(buffer + 2, blen - 2, pckt);
                break;
            default:
                //printf("Malformed packet\n");
                return 0;
        }
    }
    return -1;
}
