#include <stdint.h>
#include <pspmoduleexport.h>

#define NULL ((void *) 0)

// https://www.psdevwiki.com/pspprxlibraries/syslib.html
#define NID_MODULE_START    0xD632ACDB
#define NID_MODULE_INFO     0xF01D73A7

extern void* module_start;
extern void* module_info;

static const uintptr_t __syslib_exports[4] __attribute__((section(".rodata.sceResident"))) = 
{
  NID_MODULE_START,
  NID_MODULE_INFO,
  (uintptr_t) &module_start,
  (uintptr_t) &module_info,
};

const struct _PspLibraryEntry __library_exports[1] __attribute__((section(".lib.ent"), used)) = 
{
  { NULL, 0x0000, 0x8000, 4, 1, 1, (void*)&__syslib_exports },
};
