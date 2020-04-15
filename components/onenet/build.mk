##
# Copyright (C) 2016 YunOS Project. All rights reserved.
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

L_PATH:= $(call cur-dir)

include $(DEFINE_LOCAL)

L_MODULE:= libiot_onenet

L_CFLAGS:= -Wall

L_INCS += csi/csi_kernel/include
L_INCS:= \
		$(L_PATH)/ciscore \
		$(L_PATH)/ciscore/er-coap-13 \
		$(L_PATH)/ciscore/std_object

L_SRCS:= \
    dm_utils/dm_endpoint.c \
    dm_utils/j_aes.c \
    dm_utils/j_base64.c \
    ciscore/cis_block1.c \
    ciscore/cis_bootstrap.c \
    ciscore/cis_config.c \
    ciscore/cis_core.c \
    ciscore/cis_data.c \
    ciscore/cis_json.c \
    ciscore/cis_list.c \
    ciscore/cis_log.c \
    ciscore/cis_management.c \
    ciscore/cis_memtrace.c \
    ciscore/cis_objects.c \
    ciscore/cis_observe.c \
    ciscore/cis_packet.c \
    ciscore/cis_registration.c \
    ciscore/cis_tlv.c \
    ciscore/cis_transaction.c \
    ciscore/cis_uri.c \
    ciscore/cis_utils.c \
    ciscore/cis_discover.c \
    ciscore/er-coap-13/er-coap-13.c \
    platform/yoc/onenet_net.c \
    platform/yoc/onenet_sys.c \
    ciscore/std_object/std_object.c \
    ciscore/std_object/std_object_security.c

L_SRCS += yoc_iot/iot_onenet_api.c
L_SRCS += yoc_iot/iot_onenet.c
L_SRCS += ../iot.c

include $(BUILD_MODULE)
