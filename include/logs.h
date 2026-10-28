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

#ifndef LOGS_H_
#define LOGS_H_

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "main.h"

#define LOG_MESSAGE_MAX_LENGTH  1024

typedef enum LogLevel
{
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_PATCH,
  LOG_MEMORY,
  LOG_USERSCRIPT,
  LOG_GAME,
  LOG_CONFIG
} LogLevel;

#if LOGGING

extern void Log(LogLevel level, const char *fileName, uint32 lineNumber, u8string message, ...) __attribute__ ((format(printf, 4, 5)));

#define LOG(type, s, ...) \
  Log(type, __FILE_NAME__, __LINE__, s __VA_OPT__(,) __VA_ARGS__)

#define DEBUG_LOG(s, ...) LOG(LOG_DEBUG,   s __VA_OPT__(,) __VA_ARGS__)
#define INFO_LOG(s, ...)  LOG(LOG_INFO,    s __VA_OPT__(,) __VA_ARGS__)
#define WARN_LOG(s, ...)  LOG(LOG_WARNING, s __VA_OPT__(,) __VA_ARGS__)
#define ERROR_LOG(s, ...) LOG(LOG_ERROR,   s __VA_OPT__(,) __VA_ARGS__)

#if PATCH_LOGGING
#define PATCH_LOG(s, ...) LOG(LOG_PATCH,   s __VA_OPT__(,) __VA_ARGS__)
#else
#define PATCH_LOG(s, ...)
#endif

#if MEMORY_LOGGING
#define MEM_LOG(s, ...) LOG(LOG_MEMORY,   s __VA_OPT__(,) __VA_ARGS__)
#else
#define MEM_LOG(s, ...)
#endif

#if USERSCRIPT_LOGGING
#define USERSCRIPT_LOG(s, ...) LOG(LOG_USERSCRIPT,   s __VA_OPT__(,) __VA_ARGS__)
#else
#define USERSCRIPT_LOG(s, ...)
#endif

#if GAME_LOGGING
#define GAME_LOG(s, ...) LOG(LOG_GAME,   s __VA_OPT__(,) __VA_ARGS__)
#else
#define GAME_LOG(s, ...)
#endif

#if CONFIG_LOGGING
#define CONFIG_LOG(s, ...) LOG(LOG_CONFIG,   s __VA_OPT__(,) __VA_ARGS__)
#else
#define CONFIG_LOG(s, ...)
#endif

#else

#define LOG(type, s, ...)
#define DEBUG_LOG(s, ...)
#define INFO_LOG(s, ...)
#define WARN_LOG(s, ...)
#define ERROR_LOG(s, ...)
#define PATCH_LOG(s, ...)
#define MEM_LOG(s, ...)
#define USERSCRIPT_LOG(s, ...)
#define GAME_LOG(s, ...)
#define CONFIG_LOG(s, ...)

#endif

#endif /* LOGS_H_ */