/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/* maxiao.maxiao
   define the interface and structure for MIT_RTOS
*/


#ifndef __MIT_RTOS_H__
#define __MIT_RTOS_H__

#ifdef __cplusplus 
extern "C"
{ 
#endif

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	kMitRtosRetSuccess = 0,

	kMitRtosRetKwsInitError = 20041,
}MitRtosRetValue;

/*定义MIT_RTOS模块返回的事件类型*/
typedef enum {
	MIT_RTOS_EVENT_NONE=0,
	MIT_RTOS_EVENT_WWV,
	MIT_RTOS_EVENT_WWVSTART,
	MIT_RTOS_EVENT_WWVEND,
	MIT_RTOS_EVENT_WWVDATA,
	MIT_RTOS_EVENT_WWVCONFIRM,
	MIT_RTOS_EVENT_WWVREJECT,
	MIT_RTOS_EVENT_VADSTART,
	MIT_RTOS_EVENT_VADTIMEOUT,
	MIT_RTOS_EVENT_VADEND,
	/*MIT_RTOS_EVENT_VPRRESULT,
	MIT_RTOS_EVENT_ASRPARTIALRESULT,
	MIT_RTOS_EVENT_ASRRESULT,
	MIT_RTOS_EVENT_ASRRERROR,
	MIT_RTOS_EVENT_DIALOGRESULT,*/
} mit_rtos_event_t;


//return the length got 需要注册，但未实际使用
typedef int (*MitRtosDataGet)(void * user_data, char * buffer, int len);
typedef int (*MitRtosDataOut)(void * user_data,  mit_rtos_event_t event, char * buffer, int len);
typedef int (*MitRtosEvent)(void * user_data, mit_rtos_event_t event, int dialog_finish);


/*定义MIT_RTOS模块 回调相关参数。 需要注册，但未实际使用*/
typedef struct  {
  MitRtosEvent on_event_callback;   //SDK返回事件的回调接口
  MitRtosDataGet need_data_callback;//SDK获取数据的回调接口
  MitRtosDataOut put_data_callback; //
  void *user_data;                  //用户私有数据，作为回调函数参数
}mit_rtos_listener;

/*定义MIT_RTOS模块 信号处理算法选择*/
typedef enum {
	kMitRtosAlgBypass = 0,
	kMitRtosAlg1MicFixed = 1,
	kMitRtosAlg2MicFloat = 2,
	kMitRtosAlg2MicFixed = 3,
	kMitRtosAlg2MicBssFloat = 4,
	kMitRtosAlg2Mic1AecFloat = 5,
	kMitRtosAlg2MicFloatThead2ed = 6,
}MitRtosAlgType;

/* 定义MIT_RTOS模块初始化配置信息*/
typedef struct {
	mit_rtos_listener * listener; //回调参数

	MitRtosAlgType alg_type; //模块算法选择。固定选择 kMitRtosAlg2MicFixed (3)
	
	char task_enable ;       //use new task to run SDK. reserved
	char kws_alwayson_enable;//唤醒检测保持运行。识别状态也进行唤醒检测。
	
	char fe_enable;          //是否使能AFE子模块
	char kws_enable;         //是否使能kws子模块
	char vad_enable;         //是否使能VAD子模块
	
	char wwv_enable;         //是否使能唤醒二次确认联动功能	0不需要二次确认、1需要二次确认(相关门限使用算法预置参数)
	short local_threshold;    //reserved 唤醒本地门限，    百分比 []0- 100]。引擎内部使用
	short wwv_threshold;      //reserved 唤醒二次确认门限，百分比[]0-100]. 实际kws引擎内部未使用，外层封装时控制

	char need_data_after_vad;//process时是否需要外传语音数据给调用方。 如果不需要保存算法处理后数据，则可以设置为0 关闭此功能。
} MitRtosConfig;

/*打印给定配置参数的相关信息。用于调试log查看
  参数：
 	 config: MIT_RTOS模块初始化配置参数
  返回值：
  	无
*/
void mit_rtos_config_print(MitRtosConfig * config);

/*定义VAD事件*/
typedef enum {
	kMitRotsVadResultInvalid = -1,//VAD检测结果无变化，即保持安静或者有声音
	kMitRotsVadResultStartPoint,  //检测到起点消息
	kMitRotsVadResultEndPoint,    //检测到尾点消息
	kMitRotsVadResultTimeout      //检测到静音超时消失
}MitRotsVadResult;

/*打印VAD事件的描述字符，用于log打印
  参数：
 	 vad_result: VAD事件
  返回值：
  	对应描述符信息。
*/
const char * mit_rtos_vadstate_get_string(MitRotsVadResult vad_result);

/*定义唤醒信息*/
typedef struct {
	int index;      //the index of all channels
	char * data;    //唤醒词切分数据缓存指针 [reserved]
	int length;     //唤醒词切分数据长度      [reserved]
	char word[24];  //唤醒词文本
	int confidence; //唤醒词打分
	short cc_confidence;//reserved
	char do_wwv;    //本次唤醒是否需要进行二次确认验证。输出到用户使用
}MitRtosDataKws;


/*定义MIT_RTOS模块检测到的消息内容*/
typedef struct MitRtosAudioResult_{
	MitRotsVadResult vad_state;  //当前VAD状态
	int data_kws_count;          //检测到的唤醒词个数。0 未检测到唤醒词; >0,检测到多个唤醒词
	MitRtosDataKws data_kws[3];  //检测到唤醒词时的具体唤醒信息
	//int kws_bestend;             //检测到唤醒数据的最佳尾点，用于截取唤醒语音。 1找到bestend，0未找到bestend。
	                             //kws_bestend=1后，可以一次性获取所有唤醒数据。 
} MitRtosAudioResult;

/*初始化MIT_RTOS模块
  参数：
  	config 初始化配置参数
  返回值：
  	成功返回0，失败返回负值。
*/
int mit_rtos_init(MitRtosConfig * config);

/*反初始化MIT_RTOS模块
  参数：
  	无
  返回值：
  	成功返回0，失败返回负值。
*/
int mit_rtos_uninit();

/*启动MIT_RTOS模块
  参数：
  	无
  返回值：
  	成功返回0，失败返回负值。
*/
int mit_rtos_start();

/*停止MIT_RTOS模块
  参数：
  	无
  返回值：
  	成功返回0，失败返回负值。
*/
int mit_rtos_stop();

/*向SDK输入语音数据，并获取算法处理后数据、事件消息。
  参数：
  	buffer：      输入语音数据地址
  	len：         'buffer'中数据长度，单位字节。要求每次数据长度是20ms，即2mic+1ref数据来说，是320*3*2=1920字节。
  	buffer_out： 算法处理后数据保存地址
  	len_out：     作为输入标识'buffer_out'的空间大小。 作为输出，标识算法实际写入'buffer_out'的数据长度。 单位字节
  	result_info：返回MIT_RTOS模块内的消息
  返回值：
  	失败返回负值
  	唤醒未成功，返回0
  	唤醒成功，返回0x01. 找到唤醒数据最佳尾点，返回0x02. 唤醒成功并同时得到bestend，则返回3(0x01|0x02). 4...10reserved
  	命中命令词，则返回正数（>=11）,无bestend概念。
  	if (ret == 0) {
		//not wake up
  	} else if (ret>0) {
  		//唤醒相关消息 主唤醒词唤醒成功/命令词成功/得到唤醒数据尾点
  		if (ret > 10) {
			//命令词命中
	  	} else {
	  		if (ret &0x01) {
				//主唤醒词唤醒成功。可以获取唤醒附加消息，但二次确认数据还未准备完毕
	  		}
	  		if (ret &0x02) {
				//主唤醒词二次确认数据准备完毕
	  		}

	  	}
  	} else {
		//出错
  	}
*/
mit_rtos_event_t mit_rtos_update_audio(const char * buffer, int len, char * buffer_out, int *len_out, MitRtosAudioResult * result_info);

/* reserved */int mit_rtos_get_audio(char * buffer_out, int length);
/* reserved */int mit_rtos_event_get_kws(char * buffer);

#define MIT_RTOS_VOICE_DATA_LENGTH_MS 20
#define MIT_RTOS_VOICE_DATA_LENGTH_BYTE (MIT_RTOS_VOICE_DATA_LENGTH_MS*16*2)

/*定义需要获取语音的内容。是KWS语音还是ASR语音*/
typedef enum {
	kMitRtosVoiceModeKws = 0,// kws voice data
	kMitRtosVoiceModeAsr = 1,// asr voice data
}MitRtosVoiceMode;


/*[reserved] define the state of voice_data get from mit_rtos*/
typedef enum {
	voice_data_flag_disable = 0,// data not available
	voice_data_flag_start ,// start of voice_section
	voice_data_flag_available,// available of voice_section
	voice_data_flag_end,	  // end of voice_section
	voice_data_flag_timeout,  // special for vad timeout
	voice_data_flag_error,	  // error of voice_section
}mit_rtos_voice_data_flag;

/*定义获取的语音是否有效*/
typedef enum {
	kVoiceDataInvalid	= 0,// invalid
	kVoiceDataAvailable = 1,// available
}MitRtosVoiceDataAvailable;

typedef struct {
	MitRtosVoiceMode mode;               //获取什么数据，kws/asr [in]
	MitRtosVoiceDataAvailable available; //本次获取的数据是否有效 [out]
	//mit_rtos_voice_data_flag flag; //本次数据是否检测到对应事件 [out-reserved]
	//int kws_data_leng_total;//kws所有数据长度[out]
	char *data;      //存放获取数据的地址，此空间是用户管理的 [in]
	int data_length; //作为输入，描述用户想要获取的数据长度，单位字节[in]。作为输出，标识用户实际获取到的数据长度，单位字节       [out]
	int data_end;    //是否是最后一段数据[out] //reserved
}mit_rtos_voice_data;

/*从SDK中获取对应状态的语音数据。
  比如唤醒成功时，获取唤醒切分数据，可以一次性全部获取完毕，也可以分批多次获取；
  识别状态，获取识别数据，由于识别数据是实时获取，因此只能分批次获取，知道获取所有识别数据后结束本次识别过程。
  参数：
  	voice_data： 描述获取数据的相关信息。
  返回值:
    成功返回 0. 数据信息存放在voice_data结构体中。
    失败返回负值。
*/
int mit_rtos_get_voice(mit_rtos_voice_data *voice_data);

#ifdef __cplusplus 
}
#endif
#endif
