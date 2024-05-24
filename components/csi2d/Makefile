CSOURCE := $(wildcard src/vector/*.c)
CSOURCE += $(wildcard src/sw/*.c)
CSOURCE += $(wildcard test/*.c)

# CSOURCE := $(wildcard src/*.S)

CPPSOURCE := $(wildcard test/*.cpp)
CPPSOURCE += $(wildcard src/*.cpp)

COBJS := $(patsubst %.c,%.c.o,$(CSOURCE))
CPPOBJS := $(patsubst %.cpp,%.cpp.o,$(CPPSOURCE))
OBJS := $(COBJS) $(CPPOBJS)
INC := inc

CROSS_COMPILE ?= /iothome/huanglb/work/light_yocto/riscv-toolchain/bin/riscv64-unknown-linux-gnu-

CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++

# DEFINE := -DC910

CFLAGS := -g -I$(INC) -D_GNU_SOURCE -O3 -fPIC -mcmodel=medany -mabi=lp64d -march=rv64gcv0p7_xtheadc
CPPFLAGS := -g -I$(INC) -D_GNU_SOURCE -O3 -fPIC -mcmodel=medany -mabi=lp64d -march=rv64gcv0p7_xtheadc

CFLAGS += $(DEFINE)
CPPFLAGS += $(DEFINE)

LDFLAGS := -std=gnu99

all:csi2d bench

bench:$(OBJS)
	$(CXX) -static -o $@ $^ $(CFLAGS) $(LDFLAGS)

csi2d:$(OBJS)
	$(CC) -shared -o lib$@.so $^ $(LDFLAGS)

%.c.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.cpp.o:%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

clean:
	find . -name "*.o" | xargs rm -rf