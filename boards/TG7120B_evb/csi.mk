##
 # Copyright (C) 2017-2019 Alibaba Group Holding Limited
##

ifneq ($(TARGETS_ROOT_PATH),)
INCDIR += -I$(TARGETS_ROOT_PATH)/include
else
INCDIR += -I$(BOARDDIR)/include
endif
CSRC += $(BOARDDIR)/board_init.c
