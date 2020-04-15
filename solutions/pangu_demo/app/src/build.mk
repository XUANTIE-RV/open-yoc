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

L_MODULE := libmain

L_SRCS += init/cli_cmd.c
L_SRCS += init/init.c

L_SRCS += cmd_proc_baidu_asr.c \
			cmd_proc_music_url.c \
			cmd_proc_xunfei_music.c \
			cmd_proc_xunfei_tts.c \
			cmd_proc_xunfei_cmd.c \
			cmd_proc_text_cmd.c \
			nlp_json_parse.c \
			cli_auitalk.c \
			cmd_proc_xf_asr.c \
			cmd_proc_proxy_ws_asr.c \
			cmd_proc_xunfei_story.c \
			app_player.c

L_SRCS += app_net.c \
		  wifipair/pair_broadcast.c

L_SRCS += app_rtc.c

L_SRCS += app_main.c

L_SRCS += cop_fota.c
L_SRCS += app_button.c
L_SRCS += app_led.c
L_SRCS += app_led_demo.c

L_SRCS += iperf.c
L_SRCS += cli_apps.c

L_INCS += include
L_INCS += src/wifipair
L_INCS += src/wifipair/ap
L_INCS += src/audio

include $(BUILD_MODULE)

include src/audio/build.mk
include src/wifipair/ap/build.mk
