TARGET = ../cheatdevice_remastered
OBJS = exports.o main.o cheats.o utils.o editor.o functions.o config.o blitn.o minIni.o lang.o

WARNFLAGS = -Wall
OPTMFLAGS = -Os -frename-registers -mtune=mips32r2 -ffunction-sections -fdata-sections 

CFLAGS = -G0 -fshort-wchar -fno-pic -mno-check-zero-division $(OPTMFLAGS) $(WARNFLAGS) -mpreferred-stack-boundary=4
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

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