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

L_MODULE := libtls

L_CFLAGS := -Wall 

L_SRCS_PATH := library

L_INCS += csi/csi_driver/include

L_SRCS += \
    $(L_SRCS_PATH)/aes.c \
    $(L_SRCS_PATH)/aesni.c \
    $(L_SRCS_PATH)/arc4.c \
    $(L_SRCS_PATH)/asn1parse.c \
    $(L_SRCS_PATH)/asn1write.c \
    $(L_SRCS_PATH)/base64.c \
    $(L_SRCS_PATH)/bignum.c \
    $(L_SRCS_PATH)/blowfish.c \
    $(L_SRCS_PATH)/camellia.c \
    $(L_SRCS_PATH)/ccm.c \
    $(L_SRCS_PATH)/certs.c \
    $(L_SRCS_PATH)/cipher.c \
    $(L_SRCS_PATH)/cipher_wrap.c \
    $(L_SRCS_PATH)/cmac.c \
    $(L_SRCS_PATH)/ctr_drbg.c \
    $(L_SRCS_PATH)/debug.c \
    $(L_SRCS_PATH)/des.c \
    $(L_SRCS_PATH)/dhm.c \
    $(L_SRCS_PATH)/ecdh.c \
    $(L_SRCS_PATH)/ecdsa.c \
    $(L_SRCS_PATH)/ecjpake.c \
    $(L_SRCS_PATH)/ecp.c \
    $(L_SRCS_PATH)/ecp_curves.c \
    $(L_SRCS_PATH)/entropy.c \
    $(L_SRCS_PATH)/error.c \
    $(L_SRCS_PATH)/gcm.c \
    $(L_SRCS_PATH)/havege.c \
    $(L_SRCS_PATH)/hmac_drbg.c \
    $(L_SRCS_PATH)/md2.c \
    $(L_SRCS_PATH)/md4.c \
    $(L_SRCS_PATH)/md5.c \
    $(L_SRCS_PATH)/md.c \
    $(L_SRCS_PATH)/md_wrap.c \
    $(L_SRCS_PATH)/memory_buffer_alloc.c \
    $(L_SRCS_PATH)/net_sockets.c \
    $(L_SRCS_PATH)/oid.c \
    $(L_SRCS_PATH)/padlock.c \
    $(L_SRCS_PATH)/pem.c \
    $(L_SRCS_PATH)/pk.c \
    $(L_SRCS_PATH)/pkcs11.c \
    $(L_SRCS_PATH)/pkcs12.c \
    $(L_SRCS_PATH)/pkcs5.c \
    $(L_SRCS_PATH)/pkparse.c \
    $(L_SRCS_PATH)/pk_wrap.c \
    $(L_SRCS_PATH)/pkwrite.c \
    $(L_SRCS_PATH)/platform.c \
    $(L_SRCS_PATH)/ripemd160.c \
    $(L_SRCS_PATH)/rsa.c \
    $(L_SRCS_PATH)/sha1.c \
    $(L_SRCS_PATH)/sha256.c \
    $(L_SRCS_PATH)/sha512.c \
    $(L_SRCS_PATH)/ssl_cache.c \
    $(L_SRCS_PATH)/ssl_ciphersuites.c \
    $(L_SRCS_PATH)/ssl_cli.c \
    $(L_SRCS_PATH)/ssl_cookie.c \
    $(L_SRCS_PATH)/ssl_srv.c \
    $(L_SRCS_PATH)/ssl_ticket.c \
    $(L_SRCS_PATH)/ssl_tls.c \
    $(L_SRCS_PATH)/threading.c \
    $(L_SRCS_PATH)/timing.c \
    $(L_SRCS_PATH)/version.c \
    $(L_SRCS_PATH)/version_features.c \
    $(L_SRCS_PATH)/x509.c \
    $(L_SRCS_PATH)/x509_create.c \
    $(L_SRCS_PATH)/x509_crl.c \
    $(L_SRCS_PATH)/x509_crt.c \
    $(L_SRCS_PATH)/x509_csr.c \
    $(L_SRCS_PATH)/x509write_crt.c \
    $(L_SRCS_PATH)/x509write_csr.c \
    $(L_SRCS_PATH)/entropy_poll.c \
    $(L_SRCS_PATH)/xtea.c \
    $(L_SRCS_PATH)/rsa_alt.c \
    $(L_SRCS_PATH)/sha1_alt.c \
    $(L_SRCS_PATH)/sha256_alt.c \
    $(L_SRCS_PATH)/aes_alt.c

include $(BUILD_MODULE)

