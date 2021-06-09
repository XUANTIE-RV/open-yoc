/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions an
 * limitations under the License.
 */

#ifndef __TEST_LOG_H__
#define __TEST_LOG_H__

#include <autotest.h>
#include <test_config.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

#define TEST_PRINT(format, ...) \
		do { \
			atserver_send("+LOG:"); \
			atserver_send(format, ##__VA_ARGS__); \
			}while(0)
 
#define TEST_PRINT_HEX(msg, data, data_len) \
	do{ \
		atserver_send("\r\n%s",msg);                 \
		atserver_send(">>");                \
		for(int i=0;i<data_len; i++){       \
			atserver_send("%02x", data[i]); \
		}                                   \
		atserver_send("<<\r\n");              \
	}while(0)

#ifdef CONFIG_TEST_INFO
#define TEST_PRINT_INFO(format, ...) TEST_PRINT(format, ##__VA_ARGS__)
#else
#define TEST_PRINT_INFO(format, ...)
#endif

#ifdef CONFIG_TEST_WARN
#define TEST_PRINT_WARN(format, ...) TEST_PRINT(format, ##__VA_ARGS__)
#else
#define TEST_PRINT_WARN(format, ...)
#endif

#define TEST_PRINT_ERROR(format, ...) TEST_PRINT(format, ##__VA_ARGS__)


#define PRINT_LINE \
		TEST_PRINT("==================================================\n")


extern volatile char _mc_name[MODULES_LENGTH];
extern volatile int _test_result;

/* test printf suite */


#ifdef CONFIG_TEST_INFO
#define TEST_CASE_TIPS(msg, ...) \
	do { \
		atserver_send("\r\n+LOG:"); \
		atserver_send("[INFO]-[%s]-[%s-%d]-[%s]-[", _mc_name, __FILENAME__, __LINE__, __func__); \
		atserver_send(msg, ##__VA_ARGS__); \
		atserver_send("]\r\n"); \
	}while(0)
#else
#define TEST_CASE_TIPS(msg, ...)
#endif


#define TEST_CASE_START() \
	do { \
		_test_result = 0; \
		atserver_send("\r\n+LOG:"); \
		atserver_send("[%s]-[%s]\r\n", _mc_name, "Starting test"); \
	}while(0)

#define TEST_CASE_READY() \
	do { \
		atserver_send("\r\n%s\r\n", "READY"); \
	}while(0)

#define TEST_CASE_RESULT() \
	do { \
		if (!_test_result) { \
			atserver_send("\r\n%s\r\n", "OK"); \
		} \
		else { \
			atserver_send("\r\n%s\r\n", "ERROR"); \
		} \
	}while(0)


#define TEST_CASE_ASSERT(expression, msg, ...) \
	do { \
		if (!(expression)) { \
			_test_result = 1; \
			atserver_send("\r\n+LOG:"); \
			atserver_send("[ERROR]-[%s]-[%s-%d]-[%s]-[", _mc_name, __FILENAME__, __LINE__, __func__); \
			atserver_send(msg, ##__VA_ARGS__); \
			atserver_send("]\r\n"); \
		} \
	}while(0)

#define TEST_CASE_ASSERT_QUIT(expression, msg, ...) \
	do { \
		if (!(expression)) { \
			_test_result = 1; \
			atserver_send("\r\n+LOG:"); \
			atserver_send("[ERROR]-[%s]-[%s-%d]-[%s]-[", _mc_name, __FILENAME__, __LINE__, __func__); \
			atserver_send(msg, ##__VA_ARGS__); \
			atserver_send("]\r\n"); \
			return -1; \
		} \
	}while(0)

#ifdef CONFIG_TEST_WARN
#define TEST_CASE_WARN(msg, ...) \
	do { \
		_test_result = 1; \
		atserver_send("\r\n+LOG:"); \
		atserver_send("[WARN]-[%s]-[%s-%d]-[%s]-[", _mc_name, __FILENAME__, __LINE__, __func__); \
		atserver_send(msg, ##__VA_ARGS__); \
		atserver_send("]\r\n"); \
	}while(0)

#define TEST_CASE_WARN_QUIT(msg, ...) \
	do { \
		_test_result = 1; \
		atserver_send("\r\n+LOG:"); \
		atserver_send("[WARN]-[%s]-[%s-%d]-[%s]-[", _mc_name, __FILENAME__, __LINE__, __func__); \
		atserver_send(msg, ##__VA_ARGS__); \
		atserver_send("]\r\n"); \
		return -2; \
	}while(0)
#else
#define TEST_CASE_WARN(msg, ...)
#define TEST_CASE_WARN_QUIT(msg, ...)
#endif

#endif
