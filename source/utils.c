/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-2023, Freakler
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <pspkernel.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <math.h>

#include "utils.h"
#include "main.h" // for LOG

extern char file_log[];
extern const char *basefolder;

int logPrintf(const char *text, ...) { 
  va_list list;
  char string[256];

  va_start(list, text);
  vsprintf(string, text, list);
  va_end(list);

  char buffer[128];
  snprintf(buffer, sizeof(buffer), "%s%s", basefolder, file_log);

  SceUID fd = sceIoOpen(buffer, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
  if( fd >= 0 ) {
    sceIoWrite(fd, string, strlen(string));
    sceIoWrite(fd, "\n", 1);
    sceIoClose(fd);
  }
   
  return 0;
}

int doesFileExist(const char* path) {
  SceUID dir = sceIoOpen(path, PSP_O_RDONLY, 0777);
  if( dir >= 0 ) {
    sceIoClose(dir);
    #ifdef LOG
    logPrintf("[INFO] doesFileExist('%s') -> yes", path);
    #endif   
    return 1; // true
  }
  #ifdef LOG
  logPrintf("[INFO] doesFileExist('%s') -> no", path);
  #endif 
  return 0; // false
} 

int doesDirExist(const char* path) {
  SceUID dir = sceIoDopen(path); 
  if( dir >= 0 ) {
    sceIoDclose(dir); 
    #ifdef LOG
    logPrintf("[INFO] doesDirExist('%s') -> yes", path);
    #endif 
  return 1; // true
  }
  #ifdef LOG
  logPrintf("[INFO] doesDirExist('%s') -> no", path);
  #endif 
  return 0; // false
} 

int countFilesInFolder(const char *path) {
  int file_count = 0;
  SceUID fd;
  SceIoDirent d_dir;
  memset(&d_dir,0,sizeof(SceIoDirent)); // prevents a crash
  fd = sceIoDopen(path);
  if( fd >= 0 )  {
    while( sceIoDread(fd, &d_dir) > 0 )  {
      if( FIO_SO_ISREG(d_dir.d_stat.st_attr) ) { // if entry is a regular file
        file_count++;
      }
    }
    sceIoDclose(fd);
  } else return fd;
  return file_count;
}  

int countFoldersInFolder(const char *path) {
  int folder_count = 0;
  SceUID fd;
  SceIoDirent d_dir;
  memset(&d_dir,0,sizeof(SceIoDirent)); // prevents a crash
  fd = sceIoDopen(path);
  if( fd >= 0 )  {
    while( sceIoDread(fd, &d_dir) > 0 )  {
      if( FIO_SO_ISDIR(d_dir.d_stat.st_attr) && d_dir.d_name[0] != '.' ) { // if entry is a regular file
        folder_count++;
      }
    }
    sceIoDclose(fd);
  } else return fd;
  return folder_count;
}
    
#ifdef SAVEDITOR
int setFolderModificationDateNow(const char* folder) {
  #ifdef LOG
  logPrintf("setFolderModificationDateNow('%s')", folder);
  #endif 
  
  int ret;
  SceIoStat d_stat;
  
  ret = sceIoGetstat(folder, &d_stat);
  #ifdef LOG
  logPrintf("sceIoGetstat() returned %i", ret);
  logPrintf("%04u-%02u-%02u  %02d:%02d:%02d", d_stat.st_mtime.year, d_stat.st_mtime.month, d_stat.st_mtime.day, d_stat.st_mtime.hour, d_stat.st_mtime.minute, d_stat.st_mtime.second );
  #endif     
  
  pspTime timetest;
  sceRtcGetCurrentClockLocalTime(&timetest); // https://github.com/pspdev/pspsdk/blob/master/src/rtc/psprtc.h
      
  d_stat.st_mtime.year   = timetest.year;
  d_stat.st_mtime.month  = timetest.month;
  d_stat.st_mtime.day    = timetest.day;
  d_stat.st_mtime.hour   = timetest.hour;
  d_stat.st_mtime.minute = timetest.minutes;
  d_stat.st_mtime.second = timetest.seconds;
  
  #ifdef LOG
  logPrintf("new: %04u-%02u-%02u  %02d:%02d:%02d", d_stat.st_mtime.year, d_stat.st_mtime.month, d_stat.st_mtime.day, d_stat.st_mtime.hour, d_stat.st_mtime.minute, d_stat.st_mtime.second );
  #endif 
  
  ret = sceIoChstat(folder, &d_stat, 0x0020); // 0x0001 is passed for file mode, 0x0008 for creation time, 0x0020 modify time
  #ifdef LOG
  logPrintf("sceIoChstat() returned %i", ret);
  #endif 
  
  return ret;
}

char *getFolderModificationDate(const char* folder) {
  #ifdef LOG
  logPrintf("getFolderModificationDate('%s')", folder);
  #endif 
  
  char *res = "error";
  SceIoStat d_stat;
  int ret = sceIoGetstat(folder, &d_stat);
  #ifdef LOG
  logPrintf("sceIoGetstat() returned %i", ret);
  #endif 
  if( ret >= 0 )
    sprintf(res, "%04u-%02u-%02u  %02d:%02d:%02d", d_stat.st_mtime.year, d_stat.st_mtime.month, d_stat.st_mtime.day, d_stat.st_mtime.hour, d_stat.st_mtime.minute, d_stat.st_mtime.second );
    
  return res;
}
#endif 

int adrenalineCheck() { 
  return doesFileExist("flash1:/config.adrenaline");
}

void clearICacheFor(u32 address) {
  /// https://wololo.net/talk/viewtopic.php?t=11656
  /// https://www.romhacking.net/forum/index.php?topic=25734.0
  /// http://www.ethernut.de/en/documents/arm-inline-asm.html
  /// https://wiki.osdev.org/Inline_Assembly
  /// asm(code : output operand list : input operand list : clobber list);
  
  //asm("li $t0,0x08A0E898\n"); //this works.. but i want to store "address"
  
  #ifdef LOG
  //logPrintf("[INFO] clearICacheFor(0x%08X)", address);
  #endif 
  
  asm("cache 8, 0($a0)\n"); // $t0
}


void writeShort(uint8_t *address, short value) { // because of memory alignment
  int adr = (int)&value;
  
  *(unsigned char*)(address + 0) = *(unsigned char*)adr;
  *(unsigned char*)(address + 1) = *(unsigned char*)(adr+1);
}

void writeInteger(uint8_t *address, int value) { // because of memory alignment
  //logPrintf("value: 0x%08X", value);
  
  int adr = (int)&value;
  //logPrintf("adr: '0x%08X'", adr);

  *(unsigned char*)(address + 0) = *(unsigned char*)adr;
  *(unsigned char*)(address + 1) = *(unsigned char*)(adr+1);
  *(unsigned char*)(address + 2) = *(unsigned char*)(adr+2);
  *(unsigned char*)(address + 3) = *(unsigned char*)(adr+3);
}

void writeFloat(uint8_t *address, float value) { // because of memory alignment
  //logPrintf("value: %f", value);
  
  int adr = (int)&value;
  //logPrintf("adr: '0x%08X'", adr);

  *(unsigned char*)(address + 0) = *(unsigned char*)adr;
  *(unsigned char*)(address + 1) = *(unsigned char*)(adr+1);
  *(unsigned char*)(address + 2) = *(unsigned char*)(adr+2);
  *(unsigned char*)(address + 3) = *(unsigned char*)(adr+3);
}


char *strtok_r(char *s, const char *delim, char **save_ptr) {
  char *end;
  if( s == NULL )
    s = *save_ptr;
  if( *s == '\0' ) {
    *save_ptr = s;
    return NULL;
  }
  // Scan leading delimiters.
  s += strspn (s, delim);
  if( *s == '\0' ) {
    *save_ptr = s;
    return NULL;
  }
  // Find the end of the token. 
  end = s + strcspn (s, delim);
  if( *end == '\0' ) {
    *save_ptr = end;
    return s;
  }
  // Terminate the token and make *SAVE_PTR point past it.  
  *end = '\0';
  *save_ptr = end + 1;
  return s;
}

char *_fgets(char *s, int size, SceUID stream) {
  int read = sceIoRead(stream, s, size - 1);
  if( read <= 0 )
    return NULL;
  int i = 0;
  while( i < read ) {
    if( s[i++] == '\n' )
      break;
  }
  s[i] = '\0';
  if(read - i)
    sceIoLseek32(stream, -(read - i), PSP_SEEK_CUR);
  return s;
}

void makedirs(char *path) { // recursively create path
  char *sep = strrchr(path, '/');
  if( sep != NULL ) {
    *sep = 0;
    makedirs(path);
    *sep = '/';
  }
  if( sceIoMkdir(path, 0777) && errno != EEXIST ) {
    //#ifdef LOG // path of logfile might not exist yet
    //logPrintf("error while trying to create '%s'\n%m\n", path); 
    //#endif
  }
}

/*void FillVRAM(u32 color){
  int i;
  //sceDisplaySetMode(0, 480, 272);
  //sceDisplaySetFrameBuf((void*)0x44000000,512,1,0);
  //sceDisplaySetFrameBufferInternal(0,(void*)0x44000000,512,1,0);
  for( i = 0x44000000; i < 0x44100000; i += 4 ) {
    (((u32 *)i)[0]) = color;
  }
} */

int getHighMemBound() { // thx Acid_Snake :)
  // PSP Fat -> ~24 MB 
  // PSP Slim/Go/Street (~54 MB with High Memory Layout)
  // Adrenaline -> 0x09FC0000 (crashes > 0x0C000000)
  // Adrenaline High Memory Layout -> 0x0BBC0000 (crashes > 0x0C000000)
  // PPSSPP (with "memory = 64") -> 0x0C000000
  // PPSSPP (with "memory = 93") -> 0x0DD00000
  
  SceUID block = sceKernelAllocPartitionMemory(2, "test", PSP_SMEM_High, 100, NULL);
  int address = (int)sceKernelGetBlockHeadAddr(block)+0x100; // still not perfect though
  sceKernelFreePartitionMemory(block);
  
  // sigh!
  if( address == 0x09FC0000 ) 
    address = 0x0A000000;

  if( adrenalineCheck() ) 
    address = 0x0C000000;
  
  return address;
}

int checkCoordinateInsideArea(float a, float b, float c, float x, float y, float z, float radius) {
  return (sqrt(pow(a-x, 2) + pow(b-y, 2) + pow(c-z, 2)) <= radius);
}

float distanceBetweenCoordinates3d(float x1, float y1, float z1, float x2, float y2, float z2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2) * 1.0);
}

void getSizeString(char string[16], uint64_t size) { 
  double double_size = (double)size;

  int i = 0;
  static char *units[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
  while( double_size >= 1024.0 ) {
    double_size /= 1024.0;
    i++;
  }
  snprintf(string, 16, "%.*f %s", (i == 0) ? 0 : 2, double_size, units[i]);
}

int fileEndsWithExtension(char *filename, const char* extension) {
  return strcmp(filename + strlen(filename) - strlen(extension), extension) == 0;
}