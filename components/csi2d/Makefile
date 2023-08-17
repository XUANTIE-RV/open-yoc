CSOURCE := $(wildcard src/vector/*.c)
CSOURCE += $(wildcard src/sw/*.c)

# CSOURCE := $(wildcard src/*.S)

CPPSOURCE := $(wildcard src/*.cpp)

COBJS := $(patsubst %.c,%.c.o,$(CSOURCE))
CPPOBJS := $(patsubst %.cpp,%.cpp.o,$(CPPSOURCE))
OBJS := $(COBJS) $(CPPOBJS)
INC := inc

CC := /home/abing/Xuantie-900-gcc-linux-5.10.4-glibc-x86_64-V2.6.1/bin/riscv64-unknown-linux-gnu-gcc
CXX := /home/abing/Xuantie-900-gcc-linux-5.10.4-glibc-x86_64-V2.6.1/bin/riscv64-unknown-linux-gnu-g++

CFLAGS := -g -I$(INC) -D_GNU_SOURCE -O3 -mcpu=c906fdv -fPIC 
CPPFLAGS := -g -I$(INC) -D_GNU_SOURCE -O3 -mcpu=c906fdv -fPIC 

LDFLAGS := -lpthread -std=gnu99

all:csi2d

csi2d:$(OBJS)
	$(CC) -shared -o lib$@.so $^ $(LDFLAGS)

%.c.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.cpp.o:%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

clean:
	find . -name "*.o" | xargs rm -rf