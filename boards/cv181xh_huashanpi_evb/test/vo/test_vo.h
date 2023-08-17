#ifndef __TEST_VO_H__
#define __TEST_VO_H__

#define APP_CHECK_RET(actual, fmt, arg...)								\
	do {												\
		if ((actual) != 0) {									\
			aos_cli_printf("[%d]:%s():ret=%d\n" fmt, __LINE__, __func__, actual, ## arg);	\
			return;										\
		}											\
	} while (0)

#endif
