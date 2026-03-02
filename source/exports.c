#include <pspmoduleexport.h>

#define NULL ((void *) 0)

// https://www.psdevwiki.com/pspprxlibraries/syslib.html
#define NID_MODULE_START    0xD632ACDB
#define NID_MODULE_STOP     0xCEE8593C
#define NID_MODULE_INFO     0xF01D73A7

extern void module_start;
extern void module_info;
extern void module_stop;

static const unsigned int __syslib_exports[6] __attribute__((section(".rodata.sceResident"))) = 
{
  NID_MODULE_START,
  NID_MODULE_STOP,
  NID_MODULE_INFO,
  (unsigned int) &module_start,
  (unsigned int) &module_stop,
  (unsigned int) &module_info,
};

const struct _PspLibraryEntry __library_exports[1] __attribute__((section(".lib.ent"), used)) = 
{
  { NULL, 0x0000, 0x8000, 4, 1, 2, &__syslib_exports },
};