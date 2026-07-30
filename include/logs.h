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

extern void Log(LogLevel level, const char *funcName, const char *message, ...);

#define LOG(type, s, ...) \
  Log(type, __func__, s __VA_OPT__(,) __VA_ARGS__)

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