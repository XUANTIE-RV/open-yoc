#ifndef __TEST_VI_H__
#define _TEST_VI_H__
#if (CONFIG_APP_TEST == 1)
#define APP_CHECK_RET(actual, fmt, arg...)								\
	do {												\
		if ((actual) != 0) {									\
			aos_cli_printf("[%d]:%s():ret=%d\n" fmt, __LINE__, __func__, actual, ## arg);	\
			return;										\
		}											\
	} while (0)
#endif
#endif
