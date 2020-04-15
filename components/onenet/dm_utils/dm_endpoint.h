#ifndef DM_ENDPOINT_H
#define DM_ENDPOINT_H


#include <stddef.h>
#include <stdint.h>

/* mdy by zpr
typedef struct
{
	char szCMEI_IMEI[64]; 	//CMEI/IMEI
	char szIMSI[64];		//IMSI
	char szDMv[16]; 		//DM???
	char szAppKey[64]; 		//appkey
	char szPwd[64];			//pwd??
	int  nAddressFamily; 	//4?6  ipv4,ipv6
	char szSrvIP[32];		//server
	int  nSrvPort;   		//5683
	int  nLifetime;    		//300
	int  nBootstrap; 		//??booststrap 0,not use; other n,use;
	char szCMEI_IMEI2[64];	//CMEI/IMEI

} Options;*/


typedef struct {
    char szCMEI_IMEI2[16]; 	//CMEI/IMEI
    char szIMSI[16];		//IMSI
    char szDMv[16]; 		//DM???
    char szAppKey[17]; 		//appkey
    char szPwd[33];			//pwd??
    unsigned short  nLifetime;    		//300
    char szCMEI_IMEI[1];	//CMEI/IMEI

} Options;

extern int prv_getDmUpdateQueryLength(st_context_t *contextP, st_server_t *server);
extern bool dmSdkInit(void);
extern int prv_getDmUpdateQuery(st_context_t *contextP, st_server_t *server, char *buffer, size_t length);
//mdy by zpr extern int genDmUpdateEndpointName(char **data);
extern int genDmRegEndpointName(char **data, bool isReg);
#endif
