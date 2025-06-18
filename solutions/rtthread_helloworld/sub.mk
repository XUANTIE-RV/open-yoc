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

CUR_COMP_NAME := $(notdir $(patsubst %/,%,$(shell pwd)))
CUR_COMP_DIR := $(shell pwd)

$(CUR_COMP_NAME)_incs := $(CUR_COMP_DIR)/app/include

$(CUR_COMP_NAME)_c_srcs := $(wildcard $(CUR_COMP_DIR)/app/src/*.c) \
                           $(wildcard $(CUR_COMP_DIR)/app/src/testcases/utest/*.c) \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/atomic_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/event_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/hooklist_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/irq_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/mailbox_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/mem_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/memheap_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/messagequeue_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/mtsafe_kprint_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/mutex_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/sched_mtx_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/sched_sem_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/sched_thread_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/sched_timed_mtx_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/sched_timed_sem_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/semaphore_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/signal_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/slab_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/thread_tc.c \
                           $(CUR_COMP_DIR)/app/src/testcases/kernel/timer_tc.c
