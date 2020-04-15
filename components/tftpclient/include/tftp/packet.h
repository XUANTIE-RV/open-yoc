#ifndef _TFTP_PACKET_H_

#include "tftp_client.h"

typedef struct packet_struct {
    int   opcode;       //Type of the packet
    char *filename;     //Filename -> Only if a WRQ or RRQ
    int   mode;         //mode -> Only if WRQ or RRQ
    int   block_number; //Block number -> For ACK and DATA packets
    int   error_code;   //Error code for ERROR packets
    char *err_message;  //Error message for ERROR packets
    char *data_buffer;  //Data buffer which contains a data
} packet;

/*functions*/

void init_packet(packet *); //Initialises a packet structure
int  strip_raw_packet(
     char *, int,
     packet
         *); //Takes the TFTP header + data and returns a pointer to the corresponding packet structure
char *create_raw_packet(packet *); //Takes the packet struct pointer and returns the string

/*Some other functions*/

int is_read_request(int);  //is read request ?
int is_write_request(int); //is write request ?
int is_data_packet(int);   //is data packet ?
int is_error_packet(int);  //is error packet ?
int is_ack_packet(int);    //is acknowledgement packet ?

/* data_packet.c */
void print_data_wait_timeout(int block_number);
//void print_data_send_error(int block_number);
void print_data_blck_mismatch_error(int bnum, int e_bnum);
void copy_data_header_to_buffer(int block_number, char *data_buffer, int data_size, char *buffer);
int  create_data_packet_from_raw(char *buffer, int blen, packet *pckt);

/* request_packet.c */
int   is_netascii(int mode);
int   is_octet(int mode);
void  print_read_timeout(void);
void *copy_request_header_to_buffer(int opcode, char *filename, int mode, char *buffer);

/* ack_packet.c */
void copy_ack_header_to_buffer(int block_num, char *buffer);
void print_ack_wait_timeout(int block_number);
//void print_ack_send_error(int block_number);
void print_ack_blck_mismatch_error(int bnum, int e_bnum);
int create_ack_packet_from_raw(char *buffer, int blen, packet *pckt);

/* error_packet.c */
/*
Contains the constants and definitions pertaining to error packets
*/
#define NOT_DEFINED 0
#define FILE_NOT_FOUND 1
#define ACCESS_VIOLATION 2
#define DISK_FULL 3
#define ILLEGAL_OPERATION 4
#define UNKNOWN_TID 5
#define FILE_EXISTS 6
#define NO_USER 7

int copy_err_header_to_buffer(int err_type, char *err_message, char *buffer);
int create_error_packet_from_raw(char *buffer, int blen, packet *pckt);
int find_error();

#endif
