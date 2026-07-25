#define NANOPRINTF_IMPLEMENTATION

#include "nanoprintf.h"

int	snprintf(char *__restrict buf, size_t size, const char *__restrict format, ...)
{
  return npf_snprintf(buf, size, format);
}

int	vsnprintf(char *__restrict buf, size_t size, const char *__restrict format, va_list va)
{
  return npf_vsnprintf(buf, size, format, va);
}