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

# components sources
ALL_INCS      := $(foreach mod,$(COMPONENTS),$($(mod)_incs))
ALL_C_SRCS    := $(foreach mod,$(COMPONENTS),$($(mod)_c_srcs))
ALL_CPP_SRCS  := $(foreach mod,$(COMPONENTS),$($(mod)_cpp_srcs))
ALL_CXX_SRCS  := $(foreach mod,$(COMPONENTS),$($(mod)_cxx_srcs))
ALL_CC_SRCS   := $(foreach mod,$(COMPONENTS),$($(mod)_cc_srcs))
ALL_ASM_SRCS  := $(foreach mod,$(COMPONENTS),$($(mod)_asm_srcs))
ALL_L_PATH    := $(foreach mod,$(COMPONENTS),$($(mod)_l_path))
ALL_EXT_LIBS  := $(foreach mod,$(COMPONENTS),$($(mod)_ext_libs))

ALL_OBJS := $(foreach src,$(ALL_C_SRCS),$(call get_build_path_func,$(src:.c=.o)))
ALL_OBJS += $(foreach src,$(ALL_CPP_SRCS),$(call get_build_path_func,$(src:.cpp=.o)))
ALL_OBJS += $(foreach src,$(ALL_CXX_SRCS),$(call get_build_path_func,$(src:.cxx=.o)))
ALL_OBJS += $(foreach src,$(ALL_CC_SRCS),$(call get_build_path_func,$(src:.cc=.o)))
ALL_OBJS += $(foreach src,$(ALL_ASM_SRCS),$(call get_build_path_func,$(src:.S=.o)))
