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

L_MODULE := libiot_aliot

L_CFLAGS := -Wall

L_INCS += csi/csi_kernel/include
L_INCS += modules/yunio/aliot/src/sdk-impl
L_INCS += modules/yunio/aliot/src/sdk-impl/imports
L_INCS += modules/yunio/aliot/src/sdk-impl/exports

L_PRE_LIBS += libs/$(CONFIG_CHIP_CPU)/libiot_alicoap_psk.a
L_PRE_LIBS += libs/$(CONFIG_CHIP_CPU)/libiot_alicoap_dtls.a
L_PRE_LIBS += libs/$(CONFIG_CHIP_CPU)/libiot_alimqtt_tls.a

L_SRCS += \
   src/platform/os/yoc/HAL_OS_yoc.c \
   src/platform/os/yoc/HAL_TCP_yoc.c \
   src/platform/os/yoc/HAL_Crypt_yoc.c \
   src/platform/os/yoc/HAL_UDP_yoc.c \
   src/platform/ssl/mbedtls/HAL_DTLS_mbedtls.c \
   src/platform/ssl/mbedtls/HAL_TLS_mbedtls.c \
   src/platform/ssl/mbedtls/iotx_hmac.c

#iot#
L_SRCS += iot_alimqtt/iot_alimqtt_uservice.c
L_SRCS += iot_alimqtt/iot_alimqtt.c
L_SRCS += iot_alicoap/iot_alicoap_uservice.c
L_SRCS += iot_alicoap/iot_alicoap.c

L_SRCS += ../iot.c

include $(BUILD_MODULE)

