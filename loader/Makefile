TARGET = cdr_loader
OBJS = main.o

WARNFLAGS = -Wall -Wextra
OPTMFLAGS = -Os -mtune=mips32r2

CFLAGS = -Os -G0 -fno-pic -mtune=mips32r2 $(WARNFLAGS)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LDFLAGS = -nostartfiles -nodefaultlibs

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBS = -lpspsystemctrl_user

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak