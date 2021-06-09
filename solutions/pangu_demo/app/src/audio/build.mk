##
 # Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

L_PATH := $(call cur-dir)

include $(DEFINE_LOCAL)

L_MODULE := libaudiobin

L_SRCS += audio_res.c \
		  net_fail.mp3.c \
		  net_succ.mp3.c \
		  sorry.mp3.c \
		  sorry2.mp3.c \
		  starting.mp3.c \
		  hello.mp3.c \
		  ok.mp3.c \
		  net_cfg_conn.mp3.c \
		  net_cfg_start.mp3.c \
		  net_cfg_switch.mp3.c \
          net_cfg_config.mp3.c \
          net_cfg_fail.mp3.c \
          net_cfg_timeout.mp3.c \
		  play_err.mp3.c

L_INCS += include
include $(BUILD_MODULE)
