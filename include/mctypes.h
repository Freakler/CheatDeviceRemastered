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

/* 
 * (M)ore types for C
 */

#ifndef MCTYPES_H_
#define MCTYPES_H_

#include <stdbool.h>            // For bool, true, false...
#include <sys/types.h>          // For more types...
#include <pspkerneltypes.h>     // For PSP types
#include <inttypes.h>           // Format specifiers

#ifndef ON
#define ON    true
#endif

#ifndef OFF
#define OFF   false
#endif

#ifndef YES
#define YES   true
#endif

#ifndef NO
#define NO    false
#endif

#ifndef FUNC_ERROR
#define FUNC_ERROR -1
#endif

#ifndef FUNC_OK
#define FUNC_OK     0
#endif

// 8-bit
#ifndef __int8_defined
typedef int8_t    int8;
#define __int8_defined
#endif

#ifndef __uint8_defined
typedef uint8_t   uint8;
#define __uint8_defined
#endif

#ifndef __byte_defined
typedef uint8_t   byte;
#define __byte_defined
#endif

#ifndef __uchar_defined
typedef uint8_t   uchar;
#define __uchar_defined
#endif

#ifndef __bool8_defined
typedef uint8_t   bool8;
#define __bool8_defined
#endif

// 16-bit 
#ifndef __int16_defined
typedef int16_t   int16;
#define __int16_defined
#endif

#ifndef __uint16_defined
typedef uint16_t  uint16;
#define __uint16_defined
#endif

#ifndef __ushort_defined
typedef uint16_t  ushort;
#define __ushort_defined
#endif

#ifndef __bool16_defined
typedef uint16_t  bool16;
#define __bool16_defined
#endif

#ifndef __float16_defined
typedef uint16_t  float16;
#define __float16_defined
#endif

#ifndef __array_size_defined
typedef uint16_t  array_size;
#define __array_size_defined
#endif

#ifndef __array_idx_defined
typedef int16_t   array_idx;
#define __array_idx_defined
#endif

#ifndef __array_uidx_defined
typedef uint16_t  array_uidx;
#define __array_uidx_defined
#endif

// 32-bit
#ifndef __uint32_defined
typedef uint32_t  uint32;
#define __uint32_defined
#endif

#ifndef __int32_defined
typedef int32_t   int32;
#define __int32_defined
#endif

#ifndef __bool32_defined
typedef uint32_t  bool32;
#define __bool32_defined
#endif

#ifndef __func_stat_defined
typedef int32_t   func_stat;
#define __func_stat_defined
#endif

#ifndef __float32_defined
typedef float     float32;
#define __float32_defined
#endif

#ifndef __size_defined
typedef size_t    usize;
#define __size_defined
#endif

// 64-bit
#ifndef __uint64_defined
typedef uint64_t  uint64;
#define __uint64_defined
#endif

#ifndef __int64_defined
typedef int64_t   int64;
#define __int64_defined
#endif

// 128-bit
#ifndef __uint128_defined
typedef struct { uint64_t u[2]; } uint128;
#define __uint128_defined
#endif

#ifndef __int128_defined
typedef struct { int64_t i[2]; }  int128;
#define __int128_defined
#endif

// 256-bit
#ifndef __uint256_defined
typedef struct { uint64_t u[4]; } uint256;
#define __uint256_defined
#endif

#ifndef __int256_defined
typedef struct { int64_t i[4]; }  int256;
#define __int256_defined
#endif

// Strings
#ifndef __u8char_defined
typedef char      u8char;
#define __u8char_defined
#endif

#ifndef __u8string_defined
typedef char *    u8string;
#define __u8string_defined
#endif

#ifndef __cchar_defined
typedef char      cchar;
#define __cchar_defined
#endif

#ifndef __cstring_defined
typedef char *    cstring;
#define __cstring_defined
#endif

#ifndef __wchar_defined
typedef uint16_t  wchar;
#define __wchar_defined
#endif

#ifndef __wstring_defined
typedef uint16_t *wstring;
#define __wstring_defined
#endif

// Pointers and addresses
#ifndef __uaddr_defined
typedef uintptr_t uaddr;
#define __uaddr_defined
#endif

#ifndef __iaddr_defined
typedef intptr_t  iaddr;
#define __iaddr_defined
#endif

#ifndef __uintptr_defined
typedef uintptr_t uintptr;
#define __uintptr_defined
#endif

#ifndef __intptr_defined
typedef intptr_t  intptr;
#define __intptr_defined
#endif

#ifndef __ptrdiff_defined
typedef ptrdiff_t ptrdiff;
#define __ptrdiff_defined
#endif

// Vectors and Matrices
#ifndef __vector2d_defined
typedef ScePspFVector2  Vector2D;
#define __vector2d_defined
#endif

#ifndef __vector2dint_defined
typedef ScePspIVector2  Vector2DInt;
#define __vector2dint_defined
#endif

#ifndef __vector2dshort_defined
typedef ScePspSVector2  Vector2DShort;
#define __vector2dshort_defined
#endif

#ifndef __vector2dchar_defined
typedef struct { int8 x, y; } Vector2DChar;
#define __vector2dchar_defined
#endif

#ifndef __vector3d_defined
typedef ScePspFVector3  Vector3D;
#define __vector3d_defined
#endif

#ifndef __vector3dint_defined
typedef ScePspIVector3  Vector3DInt;
#define __vector3dint_defined
#endif

#ifndef __vector3dshort_defined
typedef ScePspSVector3  Vector3DShort;
#define __vector3dshort_defined
#endif

#ifndef __vector3dchar_defined
typedef struct { int8 x, y, z; } Vector3DChar;
#define __vector3dchar_defined
#endif

#ifndef __vector4d_defined
typedef ScePspFVector4  Vector4D;
#define __vector4d_defined
#endif

#ifndef __vector4dint_defined
typedef ScePspIVector4  Vector4DInt;
#define __vector4dint_defined
#endif

#ifndef __vector4dshort_defined
typedef ScePspSVector4  Vector4DShort;
#define __vector4dshort_defined
#endif

#ifndef __vector4dchar_defined
typedef struct { int8 x, y, z, w; } Vector4DChar;
#define __vector4dchar_defined
#endif

#ifndef __matrix2d_defined
typedef ScePspFMatrix2  Matrix2D;
#define __matrix2d_defined
#endif

#ifndef __matrix3d_defined
typedef ScePspFMatrix3  Matrix3D;
#define __matrix3d_defined
#endif

#ifndef __matrix4d_defined
typedef ScePspFMatrix4  Matrix4D;
#define __matrix4d_defined
#endif

#ifndef ALIGN
#define ALIGN(x)  __attribute__((aligned(x)))
#endif

#ifndef UNUSED
#define UNUSED    __attribute__((unused))
#endif

#endif /* MCTYPES_H */