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
} LogLevel;

#if LOGGING

extern void Log(LogLevel level, const char *funcName, const char *message, ...);

#define LOG(type, s, ...) \
  Log(type, __func__, s __VA_OPT__(,) __VA_ARGS__)

#define DEBUG_LOG(s, ...) LOG(LOG_DEBUG,   s __VA_OPT__(,) __VA_ARGS__)
#define INFO_LOG(s, ...)  LOG(LOG_INFO,    s __VA_OPT__(,) __VA_ARGS__)
#define WARN_LOG(s, ...)  LOG(LOG_WARNING, s __VA_OPT__(,) __VA_ARGS__)
#define ERROR_LOG(s, ...) LOG(LOG_ERROR,   s __VA_OPT__(,) __VA_ARGS__)

#else

#define LOG(type, s, ...)
#define DEBUG_LOG(s, ...)
#define INFO_LOG(s, ...)
#define WARN_LOG(s, ...)
#define ERROR_LOG(s, ...)

#endif

#endif /* LOGS_H_ */