  .set noreorder

#include "pspimport.s"
  IMPORT_START "IoFileMgrForUser", 0x40010000
  IMPORT_FUNC	 "IoFileMgrForUser", 0x810C4BC3, sceIoClose
  IMPORT_FUNC	 "IoFileMgrForUser", 0x109F50BC, sceIoOpen
  IMPORT_FUNC	 "IoFileMgrForUser", 0x6A638D83, sceIoRead
  IMPORT_FUNC	 "IoFileMgrForUser", 0x42EC03AC, sceIoWrite
  IMPORT_FUNC	 "IoFileMgrForUser", 0x27EB27B8, sceIoLseek
  IMPORT_FUNC	 "IoFileMgrForUser", 0x68963324, sceIoLseek32
  IMPORT_FUNC	 "IoFileMgrForUser", 0xB29DDF9C, sceIoDopen
  IMPORT_FUNC	 "IoFileMgrForUser", 0xE3EB004C, sceIoDread
  IMPORT_FUNC	 "IoFileMgrForUser", 0xEB092469, sceIoDclose
  IMPORT_FUNC	 "IoFileMgrForUser", 0xF27A9C51, sceIoRemove
  IMPORT_FUNC	 "IoFileMgrForUser", 0x06A70004, sceIoMkdir
  IMPORT_FUNC	 "IoFileMgrForUser", 0x1117C65F, sceIoRmdir
  IMPORT_FUNC	 "IoFileMgrForUser", 0x55F4717D, sceIoChdir
  IMPORT_FUNC	 "IoFileMgrForUser", 0xACE946E8, sceIoGetstat
  IMPORT_FUNC	 "IoFileMgrForUser", 0x779103A0, sceIoRename
  IMPORT_FUNC	 "IoFileMgrForUser", 0x54F5FB11, sceIoDevctl

  IMPORT_START "LoadExecForUser", 0x40010000
  IMPORT_FUNC	 "LoadExecForUser", 0x05572A5F, sceKernelExitGame

  IMPORT_START "ThreadManForUser", 0x40010000
  IMPORT_FUNC	 "ThreadManForUser", 0x82BC5777, sceKernelGetSystemTimeWide
  IMPORT_FUNC	 "ThreadManForUser", 0x369ED59D, sceKernelGetSystemTimeLow
  IMPORT_FUNC	 "ThreadManForUser", 0x1FB15A32, sceKernelSetEventFlag
  IMPORT_FUNC	 "ThreadManForUser", 0x55C20A00, sceKernelCreateEventFlag
  IMPORT_FUNC	 "ThreadManForUser", 0xEF9E4C70, sceKernelDeleteEventFlag
	IMPORT_FUNC	 "ThreadManForUser", 0x446D8DE6, sceKernelCreateThread
	IMPORT_FUNC	 "ThreadManForUser", 0xF475845D, sceKernelStartThread
	IMPORT_FUNC	 "ThreadManForUser", 0x809CE29B, sceKernelExitDeleteThread
  IMPORT_FUNC	 "ThreadManForUser", 0xCEADEB47, sceKernelDelayThread
  IMPORT_FUNC	 "ThreadManForUser", 0x9ACE131E, sceKernelSleepThread

  IMPORT_START "StdioForUser", 0x40010000
  IMPORT_FUNC	 "StdioForUser", 0xA6BAB2E9, sceKernelStdout

  IMPORT_START "UtilsForUser", 0x40010000
  IMPORT_FUNC	 "UtilsForUser", 0x79D1C3FA, sceKernelDcacheWritebackAll
  IMPORT_FUNC	 "UtilsForUser", 0x27CC57F0, sceKernelLibcTime

  IMPORT_START "ModuleMgrForUser", 0x40010000
  IMPORT_FUNC  "ModuleMgrForUser", 0x644395E2, sceKernelGetModuleIdList
  IMPORT_FUNC	 "ModuleMgrForUser", 0x748CBED9, sceKernelQueryModuleInfo

  IMPORT_START "sceRtc", 0x40010000
  IMPORT_FUNC	 "sceRtc", 0xE7C27D1B, sceRtcGetCurrentClockLocalTime

  IMPORT_START "SysMemUserForUser", 0x40010000
  IMPORT_FUNC	 "SysMemUserForUser", 0x9D9A5BA1, sceKernelGetBlockHeadAddr
  IMPORT_FUNC	 "SysMemUserForUser", 0x237DBD4F, sceKernelAllocPartitionMemory
  IMPORT_FUNC	 "SysMemUserForUser", 0xB6D61D02, sceKernelFreePartitionMemory