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

L_MODULE := libiot_lwm2m

L_CFLAGS := -Wall

L_INCS := \
    ${L_PATH}/core \
    ${L_PATH}/platform \
    ${L_PATH}/std_object

L_SRCS := core/er-coap-13/er-coap-13.c \
        core/block1.c \
        core/bootstrap.c \
        core/data.c \
        core/discover.c \
        core/json.c \
        core/liblwm2m.c \
        core/list.c \
        core/management.c \
        core/objects.c \
        core/observe.c \
        core/packet.c \
        core/registration.c \
        core/tlv.c \
        core/transaction.c \
        core/uri.c \
        core/utils.c \
        platform/connection.c \
        platform/platform.c \
        std_object/object_access_control.c \
        std_object/object_connectivity_moni.c \
        std_object/object_connectivity_stat.c \
        std_object/object_device.c \
        std_object/object_firmware.c \
        std_object/object_location.c \
        std_object/object_security.c \
        std_object/object_server.c

L_SRCS += yoc_iot/iot_oceancon.c
L_SRCS += yoc_iot/iot_oceancon_object.c
L_SRCS += ../iot.c

include $(BUILD_MODULE)

