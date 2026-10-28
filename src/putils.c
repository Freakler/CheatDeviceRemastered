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

#include <pspkernel.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <psprtc.h>

#include "putils.h"
#include "main.h"
#include "logs.h"

extern u8string file_log;
extern const u8string basefolder;

// Check if file exists
bool doesFileExist(const u8string path) {
  SceIoStat stat; memset(&stat, 0, sizeof(SceIoStat));

  if ( sceIoGetstat(path, &stat) < 0 ) {
    DEBUG_LOG("doesFileExist('%s') -> no", path);
    return false;
  }

  DEBUG_LOG("doesFileExist('%s') -> %s", path, FIO_SO_ISREG(stat.st_attr) ? "yes" : "no");

  return FIO_SO_ISREG(stat.st_attr);
}

// Check if dir exists
bool doesDirExist(const u8string path) {
  SceIoStat stat; memset(&stat, 0, sizeof(SceIoStat));

  if ( sceIoGetstat(path, &stat) < 0 ) {
    DEBUG_LOG("doesDirExist('%s') -> no", path);
    return false;
  }

    DEBUG_LOG("doesDirExist('%s') -> %s", path, FIO_SO_ISDIR(stat.st_attr) ? "yes" : "no");

  return FIO_SO_ISDIR(stat.st_attr);
}

usize countFilesInFolder(const u8string path) {
  SceIoDirent d_dir; memset(&d_dir, 0, sizeof(SceIoDirent)); // prevents a crash
  usize file_count = 0;

  SceUID fd = sceIoDopen(path);
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

usize countFoldersInFolder(const u8string path) {
  usize folder_count = 0;
  SceUID fd;
  SceIoDirent d_dir; memset(&d_dir,0,sizeof(SceIoDirent)); // prevents a crash
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
func_stat setFolderModificationDateNow(const u8string folder) {
  DEBUG_LOG("setFolderModificationDateNow('%s')", folder);

  int ret;
  SceIoStat d_stat; memset(&d_stat, 0, sizeof(SceIoStat));

  ret = sceIoGetstat(folder, &d_stat);
  DEBUG_LOG("sceIoGetstat() returned %i", ret);
  DEBUG_LOG("%04u-%02u-%02u  %02d:%02d:%02d", d_stat.sce_st_mtime.year, d_stat.sce_st_mtime.month, d_stat.sce_st_mtime.day, d_stat.sce_st_mtime.hour, d_stat.sce_st_mtime.minute, d_stat.sce_st_mtime.second );

  ScePspDateTime timetest;
  sceRtcGetCurrentClockLocalTime(&timetest); // https://github.com/pspdev/pspsdk/blob/master/src/rtc/psprtc.h

  d_stat.sce_st_mtime.year   = timetest.year;
  d_stat.sce_st_mtime.month  = timetest.month;
  d_stat.sce_st_mtime.day    = timetest.day;
  d_stat.sce_st_mtime.hour   = timetest.hour;
  d_stat.sce_st_mtime.minute = timetest.minute;
  d_stat.sce_st_mtime.second = timetest.second;

  DEBUG_LOG("new: %04u-%02u-%02u  %02d:%02d:%02d", d_stat.sce_st_mtime.year, d_stat.sce_st_mtime.month, d_stat.sce_st_mtime.day, d_stat.sce_st_mtime.hour, d_stat.sce_st_mtime.minute, d_stat.sce_st_mtime.second );

  ret = sceIoChstat(folder, &d_stat, 0x0020); // 0x0001 is passed for file mode, 0x0008 for creation time, 0x0020 modify time
  DEBUG_LOG("sceIoChstat() returned %i", ret);

  return ret;
}

u8string getFolderModificationDate(const u8string folder, u8string out, usize size) {
  DEBUG_LOG("getFolderModificationDate('%s')", folder);

  SceIoStat d_stat; memset(&d_stat, 0, sizeof(SceIoStat));
  int ret = sceIoGetstat(folder, &d_stat);
  DEBUG_LOG("sceIoGetstat() returned %i", ret);
  if( ret >= 0 )
    snprintf(out, size, "%04u-%02u-%02u  %02d:%02d:%02d", d_stat.sce_st_mtime.year, d_stat.sce_st_mtime.month, d_stat.sce_st_mtime.day, d_stat.sce_st_mtime.hour, d_stat.sce_st_mtime.minute, d_stat.sce_st_mtime.second );
  else
    strncpy(out, "error", size);
  return out;
}
#endif

bool adrenalineCheck()
{
  return doesFileExist("flash1:/config.adrenaline");
}

void clearICacheFor(uaddr address) {
  /// https://wololo.net/talk/viewtopic.php?t=11656
  /// https://www.romhacking.net/forum/index.php?topic=25734.0
  /// http://www.ethernut.de/en/documents/arm-inline-asm.html
  /// https://wiki.osdev.org/Inline_Assembly
  /// asm(code : output operand list : input operand list : clobber list);

  //__asm__("li $t0,0x08A0E898\n"); //this works.. but i want to store "address"

  // DEBUG_LOG("clearICacheFor(0x%08X)", address);

  __asm__ volatile ( // volatile so compiler won't mess with this
    "cache 8, 0(%0)\n"
    :
    : "r"(address)
    : "memory"
  );
}


void writeShort(byte *address, short value) { // because of memory alignment
  memcpy(address, &value, sizeof(value));
}

void writeInteger(byte *address, int value) { // because of memory alignment
  memcpy(address, &value, sizeof(value));
}

void writeFloat(byte *address, float value) { // because of memory alignment
  memcpy(address, &value, sizeof(value));
}

u8string _fgets(u8string s, usize size, SceUID stream) {
  int read = sceIoRead(stream, s, size - 1);
  if( read <= 0 )
    return NULL;

  array_uidx i = 0;
  while( i < read ) {
    if( s[i++] == '\n' )
      break;
  }
  s[i] = '\0';
  if(read - i)
    sceIoLseek32(stream, -(read - i), PSP_SEEK_CUR);
  return s;
}

void makedirs(const u8string path) { // recursively create path
  u8string sep = strrchr(path, '/');
  if( sep != NULL ) {
    *sep = 0;
    makedirs(path);
    *sep = '/';
  }
  if( sceIoMkdir(path, 0777) < 0 ) {
    ERROR_LOG("Unable to create '%s'", path);
  }
}

/*void FillVRAM(u32 color){
  //sceDisplaySetMode(0, 480, 272);
  //sceDisplaySetFrameBuf((void*)0x44000000,512,1,0);
  //sceDisplaySetFrameBufferInternal(0,(void*)0x44000000,512,1,0);
  for( uaddr i = 0x44000000; i < 0x44100000; i += sizeof(uaddr) ) {
    (((u32 *)i)[0]) = color;
  }
} */

uaddr getHighMemBound() { // thx Acid_Snake :)
  // PSP Fat -> ~24 MB
  // PSP Slim/Go/Street (~54 MB with High Memory Layout)

  /***************************************************************************************
                Normal          High Memory
  Adrenaline    0x09FC0000      0x0BBC0000      -> you can always access the higher memory in Adrenaline up to 0x0C000000 without crashing.. (although the games wont use it)
  ARK-4         0x09FC0000      0x0B2BF700      -> you can always access the higher memory here too. (4.20.69 r151)
  ME 2.3        0x09FC0000      0x09FC0000      -> feature not working? (access yes, available to game no)
  PRO-C2        0x09FC0000      0x09FC0000      -> feature not working? (access yes, available to game no)
  PPSSPP (64)   0x0C000000
  PPSSPP (93)   0x0DD00000

  ***************************************************************************************/

  // in PRO-C2 CFW basically plugins load first and extra ram is enabled second
  // https://github.com/MrColdbird/procfw/blob/d9435ff4af6ba60466b46019393f6bbd1fc72ac0/SystemControl/mediasync_patch.c#L152

  // For high memory support there are always two patches:
  //  One to make the memory read/write for user
  //  One to force it into p2 (the user partition)


  SceUID block = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "test", PSP_SMEM_High, 0x100, NULL);
  if ( block < 0 ) return 0; // Shouldn't fail but we never know...

  uaddr address = (uaddr)sceKernelGetBlockHeadAddr(block) + 0x100; // highest address is not calculated precisely this way either but it gives a rough idea
  sceKernelFreePartitionMemory(block);

  /// manual adjustment for non-highmem
  if( address == 0x09FC0000 )
    address = 0x0A000000;

  /// manual adjustment for high-mem
  if( address >= 0x0B000000 )
    address = 0x0C000000; // SHOULD be the same for all CFWs

  return address;
}

bool checkCoordinateInsideArea(float a, float b, float c, float x, float y, float z, float radius) {
  const float xdist = a - x;
  const float ydist = b - y;
  const float zdist = c - z;
  const float dist2 = ( xdist * xdist + ydist * ydist + zdist * zdist );

  return dist2 <= (radius * radius);
}

float distanceBetweenCoordinates3d(float x1, float y1, float z1, float x2, float y2, float z2) {
  const float xdist = x2 - x1;
  const float ydist = y2 - y1;
  const float zdist = z2 - z1;
  const float dist2 = ( xdist * xdist + ydist * ydist + zdist * zdist );

  return sqrtf(dist2);
}

/**
 * Convert a raw size number to a size with byte units.
 * @param size Raw size.
 * @param outString Output string.
 * @param outStringSize Output string size.
 * @returns `outString` on success, `NULL` otherwise.
 */
u8string SizeToStrUnits(usize size, u8string outString, usize outStringSize)
{
  if ( STR_NULL(outString) || outStringSize == 0 ) return NULL;

  // Yes I'm using the correct metric unit (which is a power of 2)
  static const u8string sizeUnits[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };

  // Unrealistically large, probably negative
  if ( size > 4000000000 ) size = 0;

  float fsize = (float)size;
  uint8 exp = 0;

  while( fsize >= 1024.0f ) {
    fsize /= 1024.0f;
    exp++;
    if ( exp == 8 ) break; // Max unit reached
  }

  snprintf(outString, outStringSize, "%.*f %s", ( exp == 0 ) ? 0 : 2, fsize, sizeUnits[exp]);
  return outString;
}

// Check if filename (or path) ends with extension
bool stringEndsWith(const u8string string, const u8string endswith)
{
  if ( !string || !endswith || *string == '\0' || *endswith == '\0' ) return 0;

  return !strcasecmp(string + strlen(string) - strlen(endswith), endswith);
}

// Get extension length
usize getExtensionLength(const u8string path)
{
  if ( !path || *path == '\0' ) return 0;

  const u8string dot = strrchr(path, '.'); // Find last dot
  if ( !dot ) return 0;

  return strlen(path) - (usize)(dot - path);
}

// FUNC: MurmurHash3 hash function
uint32 hash(const u8string key, usize len, uint32 seed)
{
  uint32 c1 = 0xcc9e2d51;
  uint32 c2 = 0x1b873593;
  uint32 r1 = 15;
  uint32 r2 = 13;
  uint32 m = 5;
  uint32 n = 0xe6546b64;
  uint32 h = seed;
  uint32 k = 0;
  const uint8 *d = (const uint8 *)key;
  const uint32 *chunks = (const uint32 *)(d);
  const uint8 *tail = (const uint8 *)(d + (len / 4) * 4);

  usize l = len / 4;

  // Process the body (4-byte chunks)
  for (usize i = 0; i < l; ++i) {
    k = chunks[i];
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;

    h ^= k;
    h = (h << r2) | (h >> (32 - r2));
    h = h * m + n;
  }

  k = 0;

  // Process the tail (remaining bytes)
  switch (len & 3)
  {
    case 3:
      k ^= (tail[2] << 16);
      break;

    case 2:
      k ^= (tail[1] << 8);
      break;

    case 1: k ^= tail[0];
            k *= c1;
            k = (k << r1) | (k >> (32 - r1));
            k *= c2;
            h ^= k;
            break;
  }

  h ^= len;
  h ^= (h >> 16);
  h *= 0x85ebca6b;
  h ^= (h >> 13);
  h *= 0xc2b2ae35;
  h ^= (h >> 16);

  return h;
}

// TODO: Remove this (it's here cause of newlib math -_-)
// Don't take this off otherwise plugin bloats by like 20KB (annoying linker)
int errno;
int *__errno (void)
{
  return &errno;
}