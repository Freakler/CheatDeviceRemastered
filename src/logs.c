/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-, Freakler
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

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
#include "putils.h"

#if LOGGING

static u8string LogTypeToString[] =
{
  "DEBUG", "INFO ", "WARNG", "ERROR", "PATCH", "MEMRY", "USRSC", "GAME ", "CONFG"
};

void Log(LogLevel level, const char *fileName, uint32 lineNumber, u8string message, ...)
{
  if ( STR_EMPTY(message) ) return;

  u8char finalMessage[LOG_MESSAGE_MAX_LENGTH/2];
  u8char formattedMessage[LOG_MESSAGE_MAX_LENGTH];

  va_list args; va_start(args, message);
  vsnprintf(finalMessage, sizeof(finalMessage), message, args);
  va_end(args);

  snprintf(formattedMessage, sizeof(formattedMessage), "[%s] [%s:%lu, %s]: %s\n", PLUGIN_NAME, fileName, lineNumber, LogTypeToString[level], finalMessage);

  SceUID stdoutFileno = (level == LOG_ERROR) ? sceKernelStderr() : sceKernelStdout();
	sceIoWrite(stdoutFileno, formattedMessage, strlen(formattedMessage));
}

#endif