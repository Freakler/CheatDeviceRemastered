/*  Glue functions for the minIni library, based on the C/C++ stdio library
 *
 *  Or better said: this file contains macros that maps the function interface
 *  used by minIni to the standard C/C++ file I/O functions.
 *
 *  Copyright (c) CompuPhase, 2008-2011
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *  use this file except in compliance with the License. You may obtain a copy
 *  of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 *
 *  Version: $Id: minGlue.h 31 2011-01-28 10:46:16Z thiadmer.riemersma $
 */

/* map required file I/O to the standard C library */
#include <pspiofilemgr.h>

int _puts(const char *s, SceUID stream);
char *_gets(char *s, int size, SceUID stream);

#define ini_openread(filename,file)   ((*(file) = sceIoOpen((filename),PSP_O_RDONLY, 0777)) >= 0)
#define ini_openwrite(filename,file)  ((*(file) = sceIoOpen((filename),PSP_O_WRONLY | PSP_O_CREAT, 0777)) >= 0)
#define ini_close(file)               sceIoClose(*(file))
#define ini_read(buffer,size,file)    _gets((buffer),(size),*(file))
#define ini_write(buffer,file)        _puts((buffer),*(file))
#define ini_rename(source,dest)       sceIoRename((source),(dest))
#define ini_remove(filename)          sceIoRemove(filename)
#define ini_rewind(file)              sceIoLseek32(*(file),0,PSP_SEEK_SET)
