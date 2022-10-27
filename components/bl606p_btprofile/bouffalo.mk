include $(COMPONENT_PATH)/../ble_common.mk

#Component Makefile
include_dirs := include/bluetooth \
                src \
				
srcs_dirs := src \

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
bt_profile_srcs := src/sdp.c \
					
ifeq ($(CONFIG_BT_A2DP),1)
bt_profile_srcs += src/a2dp.c \
                   src/avdtp.c
endif

ifeq ($(CONFIG_BT_AVRCP),1)
bt_profile_srcs += src/avrcp.c \
                   src/avctp.c
endif
				
ifeq ($(CONFIG_BT_HFP),1)
bt_profile_srcs += src/hfp_hf.c \
                   src/rfcomm.c \
                   src/at.c
endif

COMPONENT_ADD_INCLUDEDIRS := $(include_dirs)

COMPONENT_SRCS := $(bt_profile_srcs)

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := $(srcs_dirs)