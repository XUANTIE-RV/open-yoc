##
 # Copyright (C) 2017-2024 Alibaba Group Holding Limited
 #
 # SPDX-License-Identifier: Apache-2.0
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

CUR_COMP_NAME := realt_perf
CUR_COMP_DIR := $(TOP_DIR)/components/$(CUR_COMP_NAME)
# $(info come to $(CUR_COMP_DIR))

ifeq ($(COLLECT_BUILD_FILES), 0)
CFLAGS += -DCONFIG_PROFILING_PERF=1
CFLAGS += -DCONFIG_PERF_BACKTRACE_USE_FP=1 -fno-omit-frame-pointer
else
$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/include $(CUR_COMP_DIR)/adapter/include

$(CUR_COMP_NAME)_l_path := $(CUR_COMP_DIR)/libs/$(PERF_LIBS_PRE_PATH)/$(CPU)
$(CUR_COMP_NAME)_ext_libs := realt_perf_prebuild
endif
