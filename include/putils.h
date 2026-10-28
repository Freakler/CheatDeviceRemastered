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

#ifndef __PUTILS_H__
#define __PUTILS_H__

#include "mctypes.h"

#define BitsByte (8)

#define KiBytes (1024)
#define KiB(x)  (usize)((x) * KiBytes)
#define MiBytes (KiBytes * KiBytes)
#define MiB(x)  (usize)((x) * MiBytes)
#define GiBytes (MiBytes * MiBytes)
#define GiB(x)  (usize)((x) * GiBytes)
#define TiBytes (GiBytes * GiBytes)
#define TiB(x)  (usize)((x) * TiBytes)
#define PiBytes (TiBytes * TiBytes)
#define PiB(x)  (usize)((x) * PiBytes)
#define EiBytes (EiBytes * EiBytes)
#define EiB(x)  (usize)((x) * EiBytes)
#define ZiBytes (ZiBytes * ZiBytes)
#define ZiB(x)  (usize)((x) * ZiBytes)
#define YiBytes (YiBytes * YiBytes)
#define YiB(x)  (usize)((x) * YiBytes)

#define ABS(x) ( (x) >= 0 ? (x) : -(x) )
#define FABS(x) ( (x) >= 0.0f ? (x) : -(x) )

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define CONV_DEG_RAD  (float)(M_PI / 180.f)
#define CONV_RAD_DEG  (float)(180.f / M_PI)
#define RAD(x)        (float)((x) * CONV_DEG_RAD)
#define DEG(x)        (float)((x) * CONV_RAD_DEG)

#define ARRAY_SIZE(x) ( sizeof(x) / sizeof(x[0]) )
#define UNUSED __attribute__((unused))

#define STR_NULL(s)   !(s)
#define STR_EMPTY(s)  (!(*(s)))
#define CLEAR_STR(s)  (s)[0] = '\0'
#define STR_STARTSWITH_CHAR(s, c) ((s)[0] == (c))
#define STR_ENDSWITH_CHAR(s, c) (((s))[strlen((s))-1] == (c))

#define PERCENT(x, p)   ((x) * ((p) / 100.0f))
#define BIT(base, num)  ((base) << (num))

#define STRINGIFY(x) #x
#define STR(x)       STRINGIFY(x)
#define CONCAT(x, y) x##y

#define BOOLSTRCUSTOM(boolean, falsestr, truestr) ((boolean) ? (truestr) : (falsestr))
#define BOOLSTR(boolean) BOOLSTRCUSTOM((boolean), "true", "false")

#define EPSILON 0.0001f
#define FLOATEQL(f1, f2) ( FABS(f1 - f2) < EPSILON )

#define MASK8(val)  (uint8  )( (val) & 0xFF               )
#define MASK16(val) (uint16 )( (val) & 0xFFFF             )
#define MASK32(val) (uint32 )( (val) & 0xFFFFFFFF         )
#define MASK64(val) (uint64 )( (val) & 0xFFFFFFFFFFFFFFFF )

#ifndef VALIDATE_SIZE
#define VALIDATE_SIZE(type, size) \
  static_assert(sizeof(type) == (size), #type "'s size is not " #size)
#endif

#ifndef VALIDATE_OFFSET
#define VALIDATE_OFFSET(struc, member, offset) \
  static_assert(offsetof(struc, member) == (offset), "Offset of " #member " in " #struc " is not " #offset)
#endif

extern bool doesFileExist(const u8string path);
extern bool doesDirExist(const u8string path);

extern usize countFilesInFolder(const u8string path);
extern usize countFoldersInFolder(const u8string path);

extern func_stat setFolderModificationDateNow(const u8string folder);
extern u8string getFolderModificationDate(const u8string folder, u8string out, usize size);

extern bool adrenalineCheck();

extern void clearICacheFor(uaddr address);

extern void writeShort(byte *address, short value);
extern void writeInteger(byte *address, int value);
extern void writeFloat(byte *address, float value);

extern u8string _fgets(u8string s, usize size, SceUID stream);

extern void makedirs(const u8string path);
extern uaddr getHighMemBound();

extern bool checkCoordinateInsideArea(float a, float b, float c, float x, float y, float z, float radius);
extern float distanceBetweenCoordinates3d(float x1, float y1, float z1, float x2, float y2, float z2);

extern u8string SizeToStrUnits(usize size, u8string outString, usize outStringSize);

#define SIZESTR(size) \
  SizeToStrUnits((size), (cchar[16]){ 0 }, 16)

extern bool stringEndsWith(const u8string path, const u8string extension);
extern usize getExtensionLength(const u8string path);

extern uint32 hash(const u8string key, usize len, uint32 seed);

#endif /* __PUTILS_H__ */