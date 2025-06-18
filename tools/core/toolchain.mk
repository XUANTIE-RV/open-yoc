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

ifeq ($(toolchain), llvm)
$(info [info]: make with llvm)
CROSS_COMPILE ?= llvm-
CC          := clang
CXX 		:= clang++
AS          := clang
AR          := $(CROSS_COMPILE)ar
OBJCOPY     := $(CROSS_COMPILE)objcopy
OBJDUMP		:= $(CROSS_COMPILE)objdump
else
$(info [info]: make with gcc)
CROSS_COMPILE ?= riscv64-unknown-elf-
CC          := $(CROSS_COMPILE)gcc
CXX 		:= $(CROSS_COMPILE)g++
AS          := $(CROSS_COMPILE)gcc
AR          := $(CROSS_COMPILE)ar
OBJCOPY     := $(CROSS_COMPILE)objcopy
OBJDUMP		:= $(CROSS_COMPILE)objdump
endif