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

#ifndef __BLITN_H__
#define __BLITN_H__

#include "mctypes.h"
#include "storiesrwsdk.h"

/// my ugly text storeing thingy
#define MAX_TEXTS 512  // number of texts that can be displayed at once
#define MSGLENGTH 128  // length messages can have

/// and boxes too
#define MAX_BOXES 128

#define SIZE_LCS_SMALL 0.6f
#define SIZE_LCS_NORMAL 0.8f
#define SIZE_LCS_BIG 1.0f
#define SIZE_LCS_HUGE 1.2f

#define SIZE_VCS_SMALL 0.46f //0.5f
#define SIZE_VCS_NORMAL 0.6f
#define SIZE_VCS_BIG 0.8f
#define SIZE_VCS_HUGE 1.0f

typedef struct menu_blit_text
{
  wchar   text[MSGLENGTH];
  CRGBA   color;
  int16   style;
  int16   shadow;
  int     origin;
  float   scale;
  Vector2D pos;
} menu_blit_text;

typedef struct menu_blit_box
{
  int16   id;
  CRGBA   color;
  Vector2D pos;
  Vector2D size;
} menu_blit_box;

/// colors (0xAABBGGRR)
#define RED           COLOR32(0xFF0000FF)
#define GREEN         COLOR32(0xFF00FF00)
#define BLUE          COLOR32(0xFFFF0000)
#define CYAN          COLOR32(0xFFffff00)
#define MAGENTA       COLOR32(0xFFff00ff)
#define YELLOW        COLOR32(0xFF00ffff)
#define AZURE         COLOR32(0xFFFF7F00)
#define VIOLET        COLOR32(0xFFFF007F)
#define ORANGE        COLOR32(0xFF007FFF)
#define GREY          COLOR32(0xFF808080)
#define LGREY         COLOR32(0xFFA0A0A0)
#define WHITE         COLOR32(0xFFFFFFFF)
#define BLACK         COLOR32(0xFF000000)
#define CLEAR         COLOR32(0x00000000)
#define ALPHABLACK    COLOR32(0x88000000) // used for help-box, map legend by game
#define ALPHAWHITE    COLOR32(0x99FFFFFF) //
#define LIGHTBLUE     COLOR32(0xFFFF9900)
#define ORANGERED     COLOR32(0xFF0055FF)
#define DARKGREEN     COLOR32(0xFF1BA000)

/// original colors used in the ogs
#define CHDVC_YELLOW  COLOR32(0xFF66ffff)
#define CHDVC_ORANGE  COLOR32(0xFF05AFFF)
#define CHDVC_MAGENTA COLOR32(0xFF5C00A2)
#define CHDVC_MAGENTA COLOR32(0xFF5C00A2)
#define CHDVC_AZURE   COLOR32(0xFFAEE223)
#define CHDVC_GREEN   COLOR32(0xFF00FF00)

#define LCS_HEALTH    COLOR32(0xFF00008E)
#define VCS_HEALTH    COLOR32(0xFF8056BB) // not 100% original
#define LCS_ARMOR     COLOR32(0xFFBE782A)
#define VCS_ARMOR     COLOR32(0xFFCFD714) // not 100% original

#define MAX_TEXT_SIZE 256

extern void drawBox(float x, float y, float width, float height, CRGBA color);
extern void drawUiBox(float x, float y, float width, float height, float bordersize, CRGBA bordercolor, CRGBA bgcolor);

extern void drawStringNoTVarg(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, u8string string);
extern void drawStringNoT(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, const u8string string, ...);
extern void drawStringNoVarg(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, const u8string string);
extern void drawString(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, u8string string, ...);
extern void drawStringLCS(int origin, int16 style, float scale, float x, float y, CRGBA color, u8string string, ...);
extern void drawStringVCS(int origin, int16 style, float scale, float x, float y, CRGBA color, u8string string, ...);

extern void Utf8ToGameEncoding(const u8string in, wstring out);

// extern void cWorldStream_Render_LCS_Patched(void *this, int mode);
// extern void cWorldStream_Render_VCS_Patched(void *this, int mode);

extern func_stat initTextBlit(uaddr text_addr, usize text_size);

#endif