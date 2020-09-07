#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
//#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <lwip/netdb.h>
#include "tftp/packet.h"

#define TIMEOUT 10 //Default timeout
#define MAX_TRIES 5
#define BUF_SIZE 516
#define DATA_SIZE 512
#define SRV_PORT "69"
#define CLI_PORT "0"

//static int            request_mode;  //The mode of connection - read or write
static unsigned short source_tid;    //The source tid or source port number
static unsigned short dest_tid = 69; //The destination tid or destination port number

static int send_packet(int fd, char *buffer, int size, struct sockaddr *addr, int slen);
static int recv_packet(int fd, char *buffer, struct sockaddr *addr, int *slen);
static void finish(FILE *f, int s);
static int  invalid_port(char *port);
//void print_error(const char *);			//prints an error and exits
#define print_error(x)                                                                             \
    do {                                                                                           \
        fprintf(stderr, "[E][tftp]%s\n", x);                                                       \
        return -1;                                                                                 \
    } while (0);

//static void      print_buffer(char *, int);                   //print the buffer . For Testing purposes
static in_port_t get_in_port(struct sockaddr_storage *);      //get port number from sockaddr *
//int       ports_match(unsigned short, unsigned short); //check if two port numbers match

static tftp_file_opt_t *g_tftp_client_file_opt = NULL;
int tftp_client_init(tftp_file_opt_t *file_opt)
{
    g_tftp_client_file_opt = file_opt;
    return 0;
}

static char *errmsgs[] = {
        "Error Occured",          "File not found", "Access violation",    "Disk full",
        "Illegal TFTP Operation", "Unknown TID",    "File Already Exists", "No user"};

int tftp_client_start(int opcode, char *filename, int mode, char *server_addr, char *port)
{

    //Called in main() -> starts the client.
    //Starts the client according to the request type and request mode
    //A connection can be started with a WRQ or an RRQ

    /*Data size*/
    int data_size = 0;

    /*result value*/
    int rv;
    int rv1;

    /*address family*/
    //int addr_family;

    /*extra pointers*/
    struct sockaddr *otr;

    /*Timeout purposes*/
    int timeout_tries  = MAX_TRIES;
    int err_blck_tries = MAX_TRIES;

    /*socket declarations*/
    int                     me = 0;
    struct sockaddr_storage other_addr, my_addr;
    struct addrinfo         hints, *res, *p;
    int                     other_addrlen;
    int                     reclength;

    /*buffer declarations*/
    char send_buffer[BUF_SIZE];
    char recv_buffer[BUF_SIZE];

    /*File tp read from or write to*/
    FILE *fp = NULL;

    /*File statistics*/
    //struct stat file_buffer;

    /*The expected block number*/
    int expected_blck_num;

    /*timeval struct for timeouts*/
    struct timeval timeout;
    timeout.tv_sec  = TIMEOUT;
    timeout.tv_usec = 0;

    /*actual filename*/
    char *temp_filename, *temp;

    /*packet structure initializations*/
    packet *pckt;
    pckt = (packet *)malloc(sizeof(packet));
    init_packet(pckt);

    /*Error messages*/
    if (g_tftp_client_file_opt == NULL) {
        print_error("call tftp_client_init first");
        return -1;
    }

    if (is_read_request(opcode) || is_write_request(opcode)) {

#if 0 /* get模式 判断文件存在 */  
    if(is_read_request(opcode)){
        rv1 = stat(filename, &file_buffer);
        if(rv1 == 0){
            finish(NULL, me);
            print_error("File already exits");
        }

    }
#endif
        temp_filename = filename;
        for (rv = 0; rv < strlen(filename); rv++) {
            if (filename[rv] == '\0')
                break;
            if (filename[rv] == '/')
                temp_filename = filename + rv + 1;
        }

        if (*temp_filename == '\0') {
            print_error("Invalid filename");
        }

        if (is_read_request(opcode)) {
            temp          = temp_filename;
            temp_filename = filename;
            filename      = temp;
        }

        /*Networking aspect starts here*/

        //Configure the socket for timeouts
        //setsockopt(me, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(struct timeval)  );

        memset(&hints, 0, sizeof(hints)); //Initialize hints to zero
        hints.ai_family   = AF_UNSPEC;    //Handle any family of sockets
        hints.ai_socktype = SOCK_DGRAM;   //Socket is of Datagram type

        if (port != NULL) {
            if (invalid_port(port))
                print_error("Invalid port");
            if ((rv = getaddrinfo(server_addr, port, &hints, &res)) != 0) {
                fprintf(stderr, "getaddrinfo:\t%d\n", rv);
                return -1;
            }
        } else {
            if ((rv = getaddrinfo(server_addr, SRV_PORT, &hints, &res)) != 0) {
                fprintf(stderr, "getaddrinfo:\t%d\n", rv);
                return -1;
            }
        }

        if (res == NULL) {
            print_error("Failed to fetch valid interfaces");
        }

        memcpy(&other_addr, res->ai_addr, res->ai_addrlen);
        other_addrlen = res->ai_addrlen;
        //addr_family   = res->ai_family;
        //freeaddrinfo(res);
        otr = (struct sockaddr *)&other_addr;

#if 0
    //Try to bind the socket to a random port
    //Fill up hints
    memset( &hints, 0 , sizeof( hints ) );                      //Fill in hints with 0s
    hints.ai_family = addr_family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
    //listout all addresses
    if( (rv = getaddrinfo(NULL, CLI_PORT, &hints, &res)) != 0 ){        //do dns lookups and fetch all the possible interfaces
        fprintf(stderr, "getaddrinfo:\t%d\n", rv );   //so we can bind
        return -1;
    }
#endif

        //Try to create socket and bind to a one address
        for (p = res; p != NULL; p = p->ai_next) {

            if ((me = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
                perror("socket");
                continue;
            }
#if 0
        if( bind(me, p->ai_addr, p->ai_addrlen ) < 0 ){
            perror("bind");
            continue;
        }
#endif
            //addr_family = p->ai_family;
            break;
        }

        if (p == NULL) {
            fprintf(stderr, "Failed to bind\n");
            return -1;
        }

        source_tid = get_in_port(&my_addr); //save source tid

        printf("Socket created successfully\n");
        //printf("Socket bound to port %hu\n", source_tid);

        //Configure socket for timeouts
        //default timeout TIMEOUT
        if (setsockopt(me, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) ==
            -1) {
            perror("setsockopt timeout");
            finish(NULL, me);
            return -1;
        }

        memcpy(&my_addr, p->ai_addr, p->ai_addrlen);
        freeaddrinfo(res);

        reclength = -1;

        if (is_read_request(opcode)) {
            //handle the operations needed to be done for a read request

            expected_blck_num = 1;
            copy_request_header_to_buffer(opcode, temp_filename, mode,
                                          send_buffer); //Raw header now in buffer
            do {

                reclength     = -1;
                timeout_tries = MAX_TRIES; //tries for timeout
                while (reclength == -1 && timeout_tries > 0) {
                    if (send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen) == -1) {
                        finish(NULL, me);
                        perror("sendto");
                        return -1;
                    }
                    reclength = recv_packet(me, recv_buffer, otr, &other_addrlen);
                    if (reclength == -1) {
                        print_read_timeout();
                    }
                    timeout_tries--;
                }

                //If maximum timeout tries reached
                if (timeout_tries == 0) {
                    break;
                }

                //Strip the raw packet from buffer and put the values in pckt structure
                if (!strip_raw_packet(recv_buffer, reclength, pckt)) {
                    copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                              send_buffer);
                    send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                    finish(NULL, me);
                    print_error("Malformed packet encountered. Exiting");
                }

                //if not datapacket, then it maybe error packet or some other packet
                //if error packet print error message and exit
                //otherwise send ILLEGAL_OPERATION error to server and exit
                if (!is_data_packet(pckt->opcode)) {

                    if (is_error_packet(pckt->opcode)) {
                        finish(NULL, me);
                        print_error(pckt->err_message);
                    } else {
                        copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                                  send_buffer);
                        send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                        finish(NULL, me);
                        print_error("Unexpected packet encountered");
                    }
                }
                err_blck_tries--; //decrement the block number error tries
            } while (expected_blck_num != pckt->block_number && err_blck_tries > 0);

            if (err_blck_tries == 0 || timeout_tries == 0) {
                copy_err_header_to_buffer(NOT_DEFINED, "Maximum Tries reached", send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(NULL, me);
                print_error("Maximum tries reached");
            }

            dest_tid = get_in_port(&other_addr); //set the destination port as chosen by the server

            //Connection established
            //Yay!

            data_size = reclength - 4; //First four bytes are headers
            if (is_netascii(mode)) {
                fp = fopen(filename, "w");
            } else if (is_octet(mode)) {
                fp = fopen(filename, "wb");
            }

            if (fp == NULL) {
                rv1 = find_error();
                copy_err_header_to_buffer(rv1, errmsgs[rv1], send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(NULL, me);
                perror("fopen");
                return -1;
            }

            size_t wret = g_tftp_client_file_opt->tftp_fwrite(pckt->data_buffer, 1, data_size, fp);
            //fwrite treates error and eof as the same thing
            //so we need to call tftp_ferror() specifically to know if
            //an error has occured
            if (wret == 0) {
                //error is set
                rv1 = find_error();
                copy_err_header_to_buffer(rv1, errmsgs[rv1], send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(fp, me);
                perror("fwrite");
                return -1;
            }
            copy_ack_header_to_buffer(expected_blck_num, send_buffer);

            rv = 0;
            while (data_size == DATA_SIZE) {

                reclength = -1;
                if (!rv) {
                    err_blck_tries = MAX_TRIES;
                } else {
                    //previous receive fetched a duplicate packet
                    //so decrement err_blck_tries
                    err_blck_tries--;
                    rv = 0;
                }

                //error block tries maximum
                if (err_blck_tries == 0)
                    break;

                timeout_tries = MAX_TRIES;
                while (reclength == -1 && timeout_tries > 0) {
                    //As it keeps timing out keep trying
                    if (send_packet(me, send_buffer, 4, otr, other_addrlen) == -1) {
                        finish(fp, me);
                        perror("sendto");
                        return -1;
                    }

                    printf("ACK sent: Block number = %d\n", pckt->block_number);

                    reclength = recv_packet(me, recv_buffer, otr, &other_addrlen);
                    if (reclength == -1) {
                        print_ack_wait_timeout(expected_blck_num);
                    }
                    timeout_tries--;
                }

                if (timeout_tries == 0)
                    break;

                if (dest_tid != get_in_port(&other_addr)) {
                    //Send error packet with errtype UNKNOWN_TID
                    //since the destination ids dont match
                    //send error packet and drop the current packet as it might be
                    //from some other service and retransmit the previous packet
                    copy_err_header_to_buffer(UNKNOWN_TID, errmsgs[UNKNOWN_TID], recv_buffer);
                    if (send_packet(me, recv_buffer, BUF_SIZE, otr, other_addrlen) == -1) {
                        perror("sendto");
                        finish(fp, me);
                        return -1;
                    }
                    rv = 1;
                    continue; //skip this packet and retransmit
                }

                //Need to send an error packet
                if (!strip_raw_packet(recv_buffer, reclength, pckt)) {
                    copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                              send_buffer);
                    send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                    finish(fp, me);
                    print_error("Malformed packet encountered. Exiting");
                }

                //If not data packet and do the same thing
                //done above
                if (!is_data_packet(pckt->opcode)) {
                    if (is_error_packet(pckt->opcode)) {
                        if (pckt->error_code == UNKNOWN_TID) {
                            //Unknown tid error
                            //skip current packet and retransmit previous
                            continue;
                        }
                        print_error(pckt->err_message);
                    } else {
                        copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                                  send_buffer);
                        send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                        finish(fp, me);
                        print_error("Unexpected packet encountered");
                    }
                }

                //If packet block numbers dont match then
                //dont increment the expected_blck_num
                //and dont copy new headers into send_buffer so that
                //the previous packet is resent
                if (pckt->block_number == (expected_blck_num + 1) % 65536) {
                    data_size = reclength - 4;
                    //fprintf(fp, "%s", pckt->data_buffer  );
                    size_t wret = g_tftp_client_file_opt->tftp_fwrite(pckt->data_buffer, 1, data_size, fp);
                    if (wret == 0) {
                        rv1 = find_error();
                        copy_err_header_to_buffer(rv1, errmsgs[rv1], send_buffer);
                        send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                        perror("fwrite");
                        finish(fp, me);
                        return -1;
                    }
                    expected_blck_num = (expected_blck_num + 1) % 65536;
                    printf("Data packet of size %d and block number %d received\n", data_size,
                           pckt->block_number);
                    copy_ack_header_to_buffer(expected_blck_num, send_buffer);
                } else {
                    rv = 1;
                    printf("%d received %d expected\n", pckt->block_number, expected_blck_num);
                }
            }

            //Max tries reached
            if (timeout_tries == 0 || err_blck_tries == 0) {
                copy_err_header_to_buffer(NOT_DEFINED, "Maximum tries reached", send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(fp, me);
                print_error("Maximum tries reached");
            }

            //The final ack
            printf("Sending the final ACK\n");
            copy_ack_header_to_buffer(pckt->block_number, send_buffer);
            if (send_packet(me, send_buffer, 4, otr, other_addrlen) == -1) {
                finish(fp, me);
                print_error("Unable to send packet");
            }
        } else {
            //handle the operations needed to be done for a write request
            expected_blck_num = 0;
            reclength         = -1;
            data_size         = DATA_SIZE;

            printf("Write request \n");

            err_blck_tries = MAX_TRIES;
            copy_request_header_to_buffer(opcode, temp_filename, mode, send_buffer);
            do {

                timeout_tries = MAX_TRIES;
                while (reclength == -1 && timeout_tries > 0) {
                    if (send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen) == -1) {
                        //Unable to send for some reason
                        finish(NULL, me);
                        perror("sendto");
                        return -1;
                    }
                    reclength = recv_packet(me, recv_buffer, otr, &other_addrlen);
                    if (reclength == -1) {
                        print_data_wait_timeout(expected_blck_num);
                    }
                    timeout_tries--;
                }

                if (timeout_tries == 0)
                    break;

                if (!strip_raw_packet(recv_buffer, reclength, pckt)) {
                    copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                              send_buffer);
                    send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                    finish(fp, me);
                    print_error(
                        "Malformed Packet"); //Malformed packet , so send error packet and exit
                }

                if (!is_ack_packet(pckt->opcode)) {
                    if (is_error_packet(pckt->opcode)) {
                        finish(NULL, me);
                        print_error(pckt->err_message); //Error packet received
                    }
                    copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                              send_buffer);
                    send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                    finish(NULL, me);
                    print_error("Unexpected packet received"); //Not an expected packet
                }

                err_blck_tries--;
            } while ((pckt->block_number != expected_blck_num) && err_blck_tries > 0);

            //Max timeouts
            if (timeout_tries == 0 || err_blck_tries == 0) {
                copy_err_header_to_buffer(NOT_DEFINED, "Maximum tries reached", send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(NULL, me);
                print_error("Maximum tries reached");
            }

            dest_tid = get_in_port(&other_addr); //Save destination port number

            //intialize file pointer
            if (is_netascii(mode))
                fp = g_tftp_client_file_opt->tftp_fopen(filename, "r");
            else if (is_octet(mode))
                fp = g_tftp_client_file_opt->tftp_fopen(filename, "rb");

            if (fp == NULL) {
                //Unable to open file for some reason
                rv1 = find_error();
                copy_err_header_to_buffer(rv1, errmsgs[rv1], send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(fp, me);
                perror("fopen");
                return -1;
            }

            //Now the connection is established
            //Start sending the data
            //printf("Connection established\n");

            data_size = g_tftp_client_file_opt->tftp_fread(recv_buffer, 1, DATA_SIZE, fp);
            // printf("%d\n", data_size);
            if (data_size == 0) {
                //Some file read error occured
                rv1 = find_error();
                copy_err_header_to_buffer(rv1, errmsgs[rv1], send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(fp, me);
                perror(NULL);
                return -1;
            }
            expected_blck_num++;
            copy_data_header_to_buffer(expected_blck_num, recv_buffer, data_size,
                                       send_buffer); //copy data header to buffer
            rv = 0;
            while (data_size > 0) {
                //Read the data in blocks of DATA_SIZE bytes
                //Send the data packet
                //Receive the acks
                reclength = -1;

                if (!rv) {
                    err_blck_tries = MAX_TRIES;
                } else {
                    err_blck_tries--;
                    rv = 0;
                }

                if (err_blck_tries == 0)
                    break;

                timeout_tries = MAX_TRIES;
                while (reclength == -1 && timeout_tries > 0) {
                    //as it keeps timing out keep trying
                    if (send_packet(me, send_buffer, data_size + 4, otr, other_addrlen) == -1) {
                        finish(fp, me);
                        perror("sendto");
                        return -1;
                    }
                    //printf("Sent no.%d %d\n", expected_blck_num, data_size);
                    reclength = recv_packet(me, recv_buffer, otr, &other_addrlen);
                    if (reclength == -1) {
                        print_data_wait_timeout(expected_blck_num);
                    }
                    timeout_tries--;
                }

                //Max tries reached
                if (timeout_tries == 0)
                    break;

                if (dest_tid != get_in_port(&other_addr)) {
                    //Send error packet with errtype UNKNOWN_TID
                    //skip current and resend previous packets
                    copy_err_header_to_buffer(UNKNOWN_TID, errmsgs[UNKNOWN_TID], recv_buffer);
                    if (send_packet(me, recv_buffer, BUF_SIZE, otr, other_addrlen) == -1) {
                        perror("sento");
                        finish(fp, me);
                        return -1;
                    }
                    rv = 1;
                    continue;
                }

                //Some error checking
                //check if port numbers match
                //check if its a valid packet
                //check if the packet is the expected one
                //check if block numbers match etc

                if (!strip_raw_packet(recv_buffer, reclength, pckt)) {
                    //Send error packet to server and exit
                    copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                              send_buffer);
                    send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                    finish(fp, me);
                    print_error("Malformed Packet received");
                }

                if (!is_ack_packet(pckt->opcode)) {

                    if (is_error_packet(pckt->opcode)) {
                        if (pckt->error_code == UNKNOWN_TID) {
                            //unknown tid error from server
                            //dont quit just discard packet and resend data packet
                            continue;
                        } else {
                            finish(fp, me);
                            print_error(pckt->err_message);
                        }
                    } else {
                        //Unexpected packet
                        //Consider it as illegal operation
                        copy_err_header_to_buffer(ILLEGAL_OPERATION, errmsgs[ILLEGAL_OPERATION],
                                                  send_buffer);
                        send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                        finish(fp, me);
                        print_error("Unexpected packet encountered");
                    }
                }

                if (pckt->block_number == expected_blck_num) {
                    
                    if (pckt->block_number % 100 == 0) {
                        printf("Rec ack no.%d\n", pckt->block_number);
                    }

                    if (data_size < DATA_SIZE) //End of write request
                        break;

                    expected_blck_num = (expected_blck_num + 1) % 65536;
                    data_size         = g_tftp_client_file_opt->tftp_fread(recv_buffer, 1, DATA_SIZE, fp);
                    // printf("%d\n", data_size);
                    if (data_size == 0) {
                        //Some file read error occured
                        rv1 = find_error();
                        copy_err_header_to_buffer(rv1, errmsgs[rv1], send_buffer);
                        send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                        finish(fp, me);
                        perror(NULL);
                        return -1;
                    }
                    //data for the next block
                    copy_data_header_to_buffer(expected_blck_num, recv_buffer, data_size,
                                               send_buffer);
                } else {
                    printf("%d received %d expected\n", pckt->block_number, expected_blck_num);
                }
            }

            //max tries reached
            if (timeout_tries == 0 || err_blck_tries == 0) {
                copy_err_header_to_buffer(NOT_DEFINED, "Maximum tries reached", send_buffer);
                send_packet(me, send_buffer, BUF_SIZE, otr, other_addrlen);
                finish(fp, me);
                print_error("Maximum tries reached");
            }
        }

        finish(fp, me);

    } else
        printf("Invalid request\n");

    return 0;
}

/*Additional functions for error printing and stuff*/

//void print_error(const char *s){
//	fprintf(stderr, "%s\n", s);
//	return -1;
//}

/*calls sento()*/
static int send_packet(int fd, char *buffer, int size, struct sockaddr *addr, int slen)
{
    return sendto(fd, buffer, size, 0, addr, slen);
}

/*calls recvfrom()*/
static int recv_packet(int fd, char *buffer, struct sockaddr *addr, int *slen)
{
    return recvfrom(fd, buffer, BUF_SIZE, 0, addr, (unsigned int *)slen);
}

#if 0
/*Print Buffer -- for testing purpose*/
static void print_buffer(char *buffer, int length)
{
    int i = 0;
    for (i = 0; i < length; i++)
        printf("%c", buffer[i]);
}
#endif

/*Get port number from sockstorage structure*/
static unsigned short get_in_port(struct sockaddr_storage *ss)
{
    if (ss->ss_family == AF_INET) {
        return ntohs(((struct sockaddr_in *)ss)->sin_port);
    }

    return 0;
    //return ntohs(((struct sockaddr_in6*)ss)->sin6_port);
}

/*Close the file and socket if open*/
static void finish(FILE *f, int s)
{
    if (f != NULL)
        g_tftp_client_file_opt->tftp_fclose(f);

    close(s);
}

static int invalid_port(char *port)
{
    int i = atoi(port);
    return ((i > 65535) || (i <= 0));
}
