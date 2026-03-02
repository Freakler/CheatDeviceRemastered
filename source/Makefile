TARGET = ../cheatdevice_remastered
OBJS = exports.o main.o cheats.o utils.o editor.o functions.o config.o blitn.o minIni.o lang.o
  
CFLAGS = -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -frename-registers
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_LARGE_MEMORY = 1
BUILD_PRX = 1
PRX_EXPORTS = exports.exp

#USE_KERNEL_LIBC = 1
#USE_KERNEL_LIBS = 1
USE_PSPSDK_LIBC = 1
USE_PSPSDK_LIBS = 1

#LIBS = -lpspsystemctrl_kernel -lm
LIBS = -lpspsystemctrl_user -lm -lpsprtc

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
