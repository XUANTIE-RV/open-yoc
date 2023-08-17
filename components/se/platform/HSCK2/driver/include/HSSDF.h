#ifndef _HSSDF_H_
#define _HSSDF_H_
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif
	
/*类型定义*/
#define CODESECT    const   
typedef char          	SGD_CHAR;
typedef int8_t        	SGD_INT8;
typedef int16_t       	SGD_INT16;
typedef int32_t       	SGD_INT32;
typedef int64_t       	SGD_INT64;
typedef unsigned char 	SGD_UCHAR, U8;
typedef uint8_t       	SGD_UINT8;
typedef uint16_t      	SGD_UINT16;
typedef uint32_t      	SGD_UINT32;
typedef uint64_t      	SGD_UINT64;
typedef unsigned int  	SGD_RV, U32;
typedef void*			SGD_OBJ;
typedef int32_t			SGD_BOOL;
typedef void*         	SGD_HANDLE;
typedef void           VOID;
typedef struct _UINT_64
{
	SGD_UINT32 uiHigh;
	SGD_UINT32 uiLow;
}INT64U;

/*错误代码标识*/
#define SDR_OK            		0x0						/*操作成功*/
#define SDR_BASE             	0x01000000				/*错误码基础值*/
#define SDR_COMMFAIL         	        (SDR_BASE + 0x00000001)	/*ͨ与设备通信失败*/
#define SDR_EXECUTION_ERROR             (SDR_BASE + 0x00000002) // 指令执行错误
#define SDR_UPDATEFM_LEN_ERROR			(SDR_BASE + 0x00000003)	/*更新固件长度错误*/
#define SDR_TIMEOUT		                (SDR_BASE + 0x00000004)	/*回二级boot超时*/
#define SDR_COMMOND_ERROR		        (SDR_BASE + 0x00000004)	/*指令错误*/
#define SDR_SPACE_ERROR                 (SDR_BASE + 0x00000005)	/*空间错误*/

/*延时*/
#define READ_OVERLAPPED_TIMEOUT     3000 

#ifdef __cplusplus
}
#endif


#endif
