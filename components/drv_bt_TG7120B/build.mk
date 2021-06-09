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

L_MODULE := libdrivers_bt

L_CFLAGS := -Wall

L_SRCS :=

L_INCS +=

L_PATH_BT := .

L_SRCS += \
			$(L_PATH_BT)/stack_main.c \
			$(L_PATH_BT)/controller/ll_patch.c \
			$(L_PATH_BT)/hci/hci_event.c \
			$(L_PATH_BT)/hci/hci.c \
			$(L_PATH_BT)/hci/hci_tl.c \
			$(L_PATH_BT)/hci/hci_data.c \
			$(L_PATH_BT)/ch6121_hci.c \
			$(L_PATH_BT)/hci_api.c \
			$(L_PATH_BT)/simple_data.c \
			$(L_PATH_BT)/bt_crypto.c \
			$(L_PATH_BT)/dtm/dtm_test.c

L_ASM_SRCS += $(L_PATH_BT)/P256-cortex-m0-ecdh-gcc.S

L_INCS +=   $(L_PATH)/$(L_PATH_BT)/hci\
			$(L_PATH)/$(L_PATH_BT)/controller \
			$(L_PATH)/$(L_PATH_BT)/include \
			$(L_PATH)/$(L_PATH_BT)/dtm

L_INCS +=   $(L_PATH)/../../../csi/csi_kernel/include $(L_PATH)/../../../csi/csi_kernel/rhino/core/include $(L_PATH)/../../../csi/csi_kernel/rhino/arch/include
L_INCS +=   $(L_PATH)/../../../kernel/protocols/bluetooth/bt_crypto/include

include $(BUILD_MODULE)
