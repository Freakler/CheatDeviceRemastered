#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <pspsysmem.h>
#include <pspiofilemgr.h>
#include <pspstdio.h>

#include "logs.h"
#include "main.h"
#include "versioning.h"

#if LOGGING

static const char *LogTypeToString[] =
{
  "DEBUG", "INFO ", "WARNG", "ERROR", "PATCH", "MEMRY", "USRSC", "GAME "
};

void Log(LogLevel level, const char *funcName, const char *message, ...)
{
  if ( !message ) return;

  char finalMessage[LOG_MESSAGE_MAX_LENGTH/2];
  char formattedMessage[LOG_MESSAGE_MAX_LENGTH];

  va_list args; va_start(args, message);
  vsnprintf(finalMessage, sizeof(finalMessage), message, args);
  va_end(args);

  snprintf(formattedMessage, sizeof(formattedMessage), "[%s] [%s] [%s]: %s\n", PLUGIN_NAME, LogTypeToString[level], funcName, finalMessage);

  SceUID stdoutFileno = (level == LOG_ERROR) ? sceKernelStderr() : sceKernelStdout();
	sceIoWrite(stdoutFileno, formattedMessage, strlen(formattedMessage));
}

#else

void Log(LogLevel level, const char *funcName, const char *message, ...) { }

#endif