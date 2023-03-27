#include <stdio.h>
#include <string.h>
#include <aos/cli.h>
#include <unistd.h>
#include "wifi_if.h"


#define TESTWIFI_EVENT(CMD) TESTWIFI_##CMD
#define HOST_CMD_GET_MAC 0
#define HOST_CMD_GET_IP 1
#define HOST_CMD_CONNECT 4
#define HOST_CMD_START_STA 5
#define HOST_CMD_STOP_STA 7
#define HOST_CMD_CLEAN_WIFI 8
#define HOST_CMD_CLEAN_ALL 10
#define IOT_SET_DEVICEINFO 11
#define IOT_GET_DEVICEINFO 12

static void TESTWIFI_connect(int32_t argc,char **argv)
{
    //Note 链接WIFI Note
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    if(argc < 3) {
        printf("%s input err\n",__func__);
        return ;
    }
    unsigned char data[512] = {0};
    int passwdlength = 0;
    unsigned long len;
    unsigned char bssid[6] = {0};
    unsigned int offset = 0;

    //SET STA
    data[0] = HOST_CMD_START_STA;
    Wifi_SendMsgData(data,1);
    usleep(10*1000);
    //START Con
    if(argc > 3) {
        passwdlength = strlen(argv[3]);
    }
    len = 1 + 3 + strlen(argv[2]) + passwdlength + 3 + sizeof(bssid);
    memset(data, 0, len);
    data[0] = HOST_CMD_CONNECT;
    data[offset + 1] = 1;
    data[offset + 2] = strlen(argv[2]) % 256;
    data[offset + 3] = strlen(argv[2]) / 256;
    memcpy(&data[offset + 4], argv[2], strlen(argv[2]));
    offset = offset + 3 + strlen(argv[2]);

    data[offset + 1] = 3;
    data[offset + 2] = passwdlength % 256;
    data[offset + 3] = passwdlength / 256;
    if(argc > 3) {
        memcpy(&data[offset + 4], argv[3], passwdlength);
    }
    offset = offset + 3 + passwdlength;
    offset += 1;
    Wifi_SendMsgData(data, offset);
#endif
}

static void TESTWIFI_disconnect(int32_t argc,char **argv)
{
    //Note unlink wifi Note
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    unsigned char data[64] = {0};
    data[0] = HOST_CMD_STOP_STA;
    Wifi_SendMsgData(data, 1);
#endif
}

static void TESTWIFI_reset(int32_t argc,char **argv)
{
    //Note 重置WIFI Note
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    unsigned char data[64] = {0};
    data[0] = HOST_CMD_CLEAN_WIFI;
    Wifi_SendMsgData(data, 1);
#endif
}

static void TESTWIFI_setFourElements(int32_t argc,char **argv)
{
    //Note 设置四元素 Note
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    if(argc < 6) {
        printf("%s input err\n",__func__);
        return ;
    }
    unsigned char data[512] = {0};
    unsigned short dn_length = 0;
    unsigned short ds_length = 0;
    unsigned short pk_length = 0;
    unsigned short ps_length = 0;
    unsigned short offset = 0;
    dn_length = (unsigned short)strlen(argv[2]);
    ds_length = (unsigned short)strlen(argv[3]);
    pk_length = (unsigned short)strlen(argv[4]);
    ps_length = (unsigned short)strlen(argv[5]);
    //ds
    data[0] = IOT_SET_DEVICEINFO;
    offset += 1;
    data[offset] = 1;
    offset += 1;
    data[offset] = (unsigned char)(dn_length % 256);
    data[offset + 1] = (unsigned char)(dn_length / 256);
    offset += 2;
    //dn
    memcpy(&data[offset],argv[2],dn_length);
    offset += dn_length;
    data[offset] = 2;
    offset += 1;
    data[offset] = (unsigned char)(ds_length % 256);
    data[offset + 1] = (unsigned char)(ds_length / 256);
    offset += 2;
    memcpy(&data[offset],argv[3],ds_length);
    offset += ds_length;
    //pk
    data[offset] = 3;
    offset += 1;
    data[offset] = (unsigned char)(pk_length % 256);
    data[offset + 1] = (unsigned char)(pk_length / 256);
    offset += 2;
    memcpy(&data[offset],argv[4],pk_length);
    offset += pk_length;
    //ps
    data[offset] = 4;
    offset += 1;
    data[offset] = (unsigned char)(ps_length % 256);
    data[offset + 1] = (unsigned char)(ps_length / 256);
    offset += 2;
    memcpy(&data[offset],argv[5],ps_length);
    offset += ps_length;
    Wifi_SendMsgData(data, offset);
#endif
}

static void TESTWIFI_getFourElements(int32_t argc,char **argv)
{
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    unsigned char data[64] = {0};
    data[0] = IOT_GET_DEVICEINFO;
    Wifi_SendMsgData(data, 1);
#endif
}

static void TESTWIFI_recovery(int32_t argc,char **argv)
{
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    //Note 恢复出厂设置 Note
    unsigned char data[64] = {0};
    data[0] = IOT_GET_DEVICEINFO;
    Wifi_SendMsgData(data, 1);
#endif
}

static void TESTWIFI_getmac(int32_t argc,char **argv)
{
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    unsigned char data[64] = {0};
    data[0] = HOST_CMD_GET_MAC;
    Wifi_SendMsgData(data, 1);
#endif
}

static void TESTWIFI_getip(int32_t argc,char **argv)
{
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    unsigned char data[64] = {0};
    data[0] = HOST_CMD_GET_IP;
    Wifi_SendMsgData(data, 1);
#endif
}
void TESTWIFI_eventrecv(uint32_t event_id, const void *data, void *context)
{
    unsigned char* tmp = (unsigned char *)data;
    unsigned char cmd = tmp[0];
    printf("TESTWIFI_eventrecv cmd [%02X] \n",cmd);
    switch(cmd) {
        case HOST_CMD_GET_MAC:
        {
            printf("%s mac: %02X:%02X:%02X:%02X:%02X:%02X \n",__func__,tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6]);
        }
        break;
        case HOST_CMD_GET_IP:
        {
            //printf("%s net_data: %s \n",__func__,&tmp[1]);
        }
        break;
        case IOT_GET_DEVICEINFO:
        {
            char dn[128] = {0};
            char ds[128] = {0};
            char pk[128] = {0};
            char ps[128] = {0};
            int raw_len = 0;
            int offset = 1;
            raw_len = (unsigned short int)tmp[offset + 1] | ((unsigned short int)tmp[offset + 2] << 8);
            offset += 3;
            strncpy(dn,(char *)&tmp[offset],raw_len);
            offset +=raw_len;
            printf("%s dn:%s \n",__func__,dn);

            raw_len = (unsigned short int)tmp[offset + 1] | ((unsigned short int)tmp[offset + 2] << 8);
            offset += 3;
            strncpy(ds,(char *)&tmp[offset],raw_len);
            offset +=raw_len;
            printf("%s ds:%s \n",__func__,ds);

            raw_len = (unsigned short int)tmp[offset + 1] | ((unsigned short int)tmp[offset + 2] << 8);
            offset += 3;
            strncpy(pk,(char *)&tmp[offset],raw_len);
            offset +=raw_len;
            printf("%s pk:%s \n",__func__,pk);

            raw_len = (unsigned short int)tmp[offset + 1] | ((unsigned short int)tmp[offset + 2] << 8);
            offset += 3;
            strncpy(ps,(char *)&tmp[offset],raw_len);
            offset +=raw_len;
            printf("%s ps:%s \n",__func__,ps);
        }
        break;
        default:
        break;
    }
}

void test_wifi_cmd_handler(int32_t argc, char **argv)
{
    if(argc < 2) {
        printf("please input : \n");
        printf("1.test_wifi_cmd connect SSID PASSWD\n");
        printf("2.test_wifi_cmd disconnect\n");
        printf("3.test_wifi_cmd reset\n");
        printf("4.test_wifi_cmd setFourElements dn ds pk ps\n");
        printf("5.test_wifi_cmd getFourElements\n");
        printf("6.test_wifi_cmd recovery \n");
        printf("7.test_wifi_cmd getmac\n");
        printf("8.test_wifi cmd getip");
        return ;
    }
#if (CONFIG_APP_HI3861_WIFI_SUPPORT == 1)
    Wifi_RecvSubscribe(TESTWIFI_eventrecv,NULL);
#endif
    if(strcmp(argv[1],"connect") == 0) {
        TESTWIFI_EVENT(connect)(argc,argv);
    } else if(strcmp(argv[1],"disconnect") == 0) {
        TESTWIFI_EVENT(disconnect)(argc,argv);
    } else if(strcmp(argv[1],"reset") == 0) {
        TESTWIFI_EVENT(reset)(argc,argv);
    } else if(strcmp(argv[1],"setFourElements") == 0) {
        TESTWIFI_EVENT(setFourElements)(argc,argv);
    } else if(strcmp(argv[1],"getFourElements") == 0) {
        TESTWIFI_EVENT(getFourElements)(argc,argv);
    } else if(strcmp(argv[1],"recovery") == 0) {
        TESTWIFI_EVENT(recovery)(argc,argv);
    } else if(strcmp(argv[1],"getmac") == 0) {
        TESTWIFI_EVENT(getmac)(argc,argv);
    } else if(strcmp(argv[1],"getip") == 0) {
        TESTWIFI_EVENT(getip)(argc,argv);
    }
}

ALIOS_CLI_CMD_REGISTER(test_wifi_cmd_handler, test_wifi_cmd, wifi cmd )