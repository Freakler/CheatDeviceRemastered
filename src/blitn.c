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

/******************************************************************************************
You can display MAX_TEXTS messages with a length of MSGLENGTH characters!
Call "initTextBlit()" with the GTA modules' text_addr

Then blit text to screen via:
  drawString(short style, float scale, float x, float y, CRGBA color, u8string string);

Draw Rectangles via:
  void drawBox(float x, float y, float width, float height, CRGBA color);
*******************************************************************************************/

#include <pspsdk.h>
#include <pspkernel.h>
#include <systemctrl.h>

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>

#include "blitn.h"
#include "logs.h"
#include "putils.h"
#include "cheats.h"
#include "mctypes.h"
#include "lang.h"

/***********
~w~ = white
~b~ = blue
~y~ = yellow

~h~ = bright

~n~ = linebreak
~k~ =

~PDCWE~   // ~w~Use~h~ ~k~ ~PDCWE~ ~w~to change weapons when on foot.
~PDSPR~
~CVEIW~
~ANS~
~VEEE~
~PDFW~    // ~w~To throw an ~h~explosive weapon~w~ press~h~ ~k~ ~PDFW~. ~w~The longer you hold~h~ ~k~ ~PDFW~ ~w~the further the weapon wil
~TGSUB~
~VEWEP~   // ~w~To perform a ~h~drive-by~w~, hold~h~ ~k~ ~VELL~ ~w~or~h~ ~k~ ~VELR~ ~w~and fire using~h~ ~k~ ~VEWEP~.
************/

/* FUNCTIONS ****************************************************************************/

void (*CFont_SetScaleLCS)(float xScale, float yScale);
void (*CFont_SetScaleVCS)(float scale);
void (*CFont_SetSlantRefPoint)(float x, float y);
void (*CFont_SetSlant)(float s);
void (*CFont_SetJustifyOn)(void);
void (*CFont_SetJustifyOff)(void);
void (*CFont_SetRightJustifyOn)(void);
void (*CFont_SetRightJustifyOff)(void);
void (*CFont_SetCentreOn)(void);
void (*CFont_SetCentreOff)(void);
void (*CFont_SetWrapx)(float x);
void (*CFont_SetCentreSize)(float s);
void (*CFont_SetBackgroundOn)(void);
void (*CFont_SetBackgroundOff)(void);
void (*CFont_SetBackGroundOnlyTextOn)(void);
void (*CFont_SetBackGroundOnlyTextOff)(void);
void (*CFont_SetPropOn)(void);
void (*CFont_SetPropOff)(void);
void (*CFont_SetFontStyle)(int16 style);
void (*CFont_SetRightJustifyWrap)(float wrap);
void (*CFont_SetAlphaFade)(float fade);
void (*CFont_SetDropShadowPosition)(int16 pos);
void (*CFont_SetBackgroundColor)(CRGBA *col);
void (*CFont_SetColor)(CRGBA *col);
void (*CFont_SetDropColor)(CRGBA *col);
void (*CFont_PrintStringLCS)(float x, float y, wchar *s, int *unk);
void (*CFont_PrintStringVCS)(wchar *text, int x, int y);

void (*ResetFontStyling)();
void (*SetTextOriginPoint)(int origin);
void (*SetTextSpaceing)(int option);
void (*SetTextBounds)(CRect* bounds);

void (*CSprite2d_DrawRect_2)(const CRect *r, const CRGBA *col, bool screenUnits);

/* FUNCTIONS ****************************************************************************/

static menu_blit_text main_blit_texts[MAX_TEXTS]  = {0};
static menu_blit_box  main_blit_boxes[MAX_BOXES]  = {0};

static int gta_version = -1;

// It must be sorted by (unicode, gamecode) for binary search.
static const uint16 LCS_UNICODES[] = {
  0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8,
  0x00A9, 0x00AA, 0x00AB, 0x00AE, 0x00B0, 0x00B4, 0x00BB, 0x00BF,
  0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
  0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
  0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
  0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
  0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
  0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
  0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
  0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
  0x0152, 0x0153, 0x0178, 0x02C6, 0x02DC, 0x2018, 0x2019, 0x201A,
  0x201C, 0x201D, 0x201E, 0x2039, 0x203A, 0x20AC, 0x2122, 0xFFFD
};

static const uint8 LCS_GAMECODES[] = {
  0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, // ¡ ¢ £ ¤ ¥ ¦ § ¨
  0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, // © ª « ® ° ´ » ¿
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, // À Á Â Ã Ä Å Æ Ç
  0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, // È É Ê Ë Ì Í Î Ï
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, // Ð Ñ Ò Ó Ô Õ Ö ×
  0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, // Ø Ù Ú Û Ü Ý Þ ß
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, // à á â ã ä å æ ç
  0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, // è é ê ë ì í î ï
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, // ð ñ ò ó ô õ ö ÷
  0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, // ø ù ú û ü ý þ ÿ
  0xC5, 0xCD, 0xCE, 0xC3, 0xCA, 0xC6, 0xC7, 0xC1, // Œ œ Ÿ ˆ ˜ ‘ ’ ‚
  0xC8, 0xC9, 0xC2, 0xC4, 0xCC, 0xC0, 0xCB, 0xDF  // “ ” „ ‹ › € ™ �
};

static const uint16 VCS_UNICODES[] = {
  0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A9, 0x00AA, 0x00AB,
  0x00AE, 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B9, 0x00BA, 0x00BB, 0x00BF, 0x00C0,
  0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA,
  0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4,
  0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE,
  0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8,
  0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F0, 0x00F1, 0x00F2,
  0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC,
  0x00FD, 0x00FE, 0x00FF, 0x0152, 0x0153, 0x0160, 0x0161, 0x0178, 0x017D, 0x017E,
  0x0192, 0x02C6, 0x02DC, 0x0384, 0x03BC, 0x2022, 0x208D, 0x208E, 0x2122, 0xFFFD
};

static const uint8 VCS_GAMECODES[] = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA9, 0xAA, 0xAB, // ¡ ¢ £ ¤ ¥ ¦ § © ª «
  0xAE, 0xB0, 0xB1, 0xB2, 0xB3, 0xB9, 0xBA, 0xBB, 0xBF, 0xC0, // ® ° ± ² ³ ¹ º » ¿ À
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, // Á Â Ã Ä Å Æ Ç È É Ê
  0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, // Ë Ì Í Î Ï Ð Ñ Ò Ó Ô
  0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, // Õ Ö × Ø Ù Ú Û Ü Ý Þ
  0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // ß à á â ã ä å æ ç è
  0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, // é ê ë ì í î ï ð ñ ò
  0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, // ó ô õ ö ÷ ø ù ú û ü
  0xFD, 0xFE, 0xFF, 0x8C, 0x9C, 0x8A, 0x9A, 0x9F, 0x8E, 0x9E, // ý þ ÿ Œ œ Š š Ÿ Ž ž
  0x83, 0x88, 0x98, 0xB4, 0xB5, 0x95, 0x8B, 0x9B, 0x99, 0x80  // ƒ ˆ ˜ ΄ μ • ₍ ₎ ™ �
};

static const uint16 *UNICODES[] = { VCS_UNICODES, LCS_UNICODES };
static const uint8 *GAMECODES[] = { VCS_GAMECODES, LCS_GAMECODES };
static const array_size CHARMAP_LENS[] = {
  ARRAY_SIZE(VCS_UNICODES),
  ARRAY_SIZE(LCS_UNICODES),
};

// Using question mark because replacement character is missing in FONT_HEADING.
#define REPLACEMENT_CHAR '?'

// Binary search version
static wchar UnicodeToGameChar(uint32 unicode, const uint16 *unicodes,
         const uint8 *gamecodes, const array_size charmap_len)
{
  if (unicode > 0xFFFF)
    return REPLACEMENT_CHAR;
    
  array_size low = 0;
  array_size high = charmap_len - 1;
  while (low <= high) {
    array_uidx mid = low + (high - low) / 2;

    if (unicodes[mid] == unicode)
      return gamecodes[mid];

    if (unicodes[mid] < unicode)
      low = mid + 1;

    else
      high = mid - 1;
  }

  return REPLACEMENT_CHAR;
}

// Linear version
/*
static inline wchar UnicodeToGameChar(uint32_t unicode, const uint16 *unicodes, const uint8 *gamecodes, const size_t charmap_len) {
  if (unicode > 0xFFFF)
    return REPLACEMENT_CHAR;

  for (size_t i = 0; i < charmap_len; i++) {
    if (unicodes[i] == unicode)
      return gamecodes[i];
    if (unicodes[i] > unicode)
      break;
  }
  return REPLACEMENT_CHAR;
}*/

static usize GetUtf8CharLen(uint8 byte)
{
  if ((byte & 0x80) == 0x00)
    return 1;
  if ((byte & 0xE0) == 0xC0)
    return 2;
  if ((byte & 0xF0) == 0xE0)
    return 3;
  if ((byte & 0xF8) == 0xF0)
    return 4;

  return 0;
}

static bool IsCont(uint8 byte)
{
  return (byte & 0xC0) == 0x80;
}

static void PutReplacementChar(wstring *out)
{
  *(*out)++ = REPLACEMENT_CHAR;
}

void Utf8ToGameEncoding(u8string in, wstring out)
{
  if (gta_version != 0 && gta_version != 1) {
    *out = '\0';
    return;
  }

  const uint16 *unicodes = UNICODES[gta_version];
  const uint8 *gamecodes = GAMECODES[gta_version];
  const usize charmap_len = CHARMAP_LENS[gta_version];

  while (*in != '\0') {
    uchar c1 = (uchar)*in++;
    uint32 unicode;

    usize len = GetUtf8CharLen(c1);

    switch (len) {
    case 1: {
      *out++ = (wchar)c1;
      continue;
    }

    case 2: {
      if (*in == '\0' || !IsCont(*in)) {
        PutReplacementChar(&out);
        continue;
      }
      uint8 c2 = (uint8)*in++;
      unicode = ((c1 & 0x1F) << 6) | (c2 & 0x3F);

      if (unicode < 0x80) {
        PutReplacementChar(&out);
        continue;
      }

      break;
    }

    case 3: {
      if (*in == '\0' || *(in + 1) == '\0' ||
          !IsCont(*in) || !IsCont(*(in + 1))) {
        PutReplacementChar(&out);
        continue;
      }
      uint8 c2 = (uint8)*in++;
      uint8 c3 = (uint8)*in++;

      unicode = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) |
          (c3 & 0x3F);

      if (unicode < 0x0800 ||
          (unicode >= 0xD800 && unicode <= 0xDFFF)) {
        PutReplacementChar(&out);
        continue;
      }

      break;
    }

    case 4: {
      if (*in == '\0' || *(in + 1) == '\0' ||  *(in + 2) == '\0' ||
          !IsCont(*in) || !IsCont(*(in + 1)) || !IsCont(*(in + 2))) {
        PutReplacementChar(&out);
        continue;
      }
      uint8 c2 = (uint8)*in++;
      uint8 c3 = (uint8)*in++;
      uint8 c4 = (uint8)*in++;

      unicode = ((c1 & 0x07) << 18) | ((c2 & 0x3F) << 12) |
          ((c3 & 0x3F) << 6) | (c4 & 0x3F);

      if (unicode < 0x010000 || unicode > 0x10FFFF) {
        PutReplacementChar(&out);
        continue;
      }

      break;
    }

    default:
      PutReplacementChar(&out);
      continue;
    }

    *out++ = UnicodeToGameChar(unicode, unicodes, gamecodes,
             charmap_len);
  }

  *out = '\0';
}

void drawBox(float x, float y, float width, float height, CRGBA color) {
  for( array_uidx i = 0; i < MAX_BOXES; i++ )
  {
    menu_blit_box* curr_box = &main_blit_boxes[i];
    if( curr_box->id == (x+y+width+height) ) break;

    if( curr_box->id == 0 )
    {
      curr_box->id = x+y+width+height;
      curr_box->pos.x = x;
      curr_box->pos.y = y;
      curr_box->size.x = width;
      curr_box->size.y = height;
      curr_box->color = color;
      break;
    }
  }
}

void drawUiBox(float x, float y, float width, float height, float bordersize, CRGBA bordercolor, CRGBA bgcolor) {
  drawBox(x, y, width, height, bgcolor); // x, y, width, height, color

  drawBox(x-bordersize, y-bordersize, width+(bordersize*2), bordersize, bordercolor); // top
  drawBox(x-bordersize, y+height, width+(bordersize*2), bordersize, bordercolor); // bottom

  drawBox(x-bordersize, y, bordersize, height, bordercolor); // left
  drawBox(x+width, y, bordersize, height, bordercolor); // right
}

void drawStringNoTVarg(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, u8string string) {

  for( array_uidx i = 0; i < MAX_TEXTS; i++ )
  {
    menu_blit_text* curr_text = &main_blit_texts[i];

    if( STR_EMPTY(curr_text->text) )
    {
      Utf8ToGameEncoding(string, curr_text->text);
      curr_text->pos.x = x;
      curr_text->pos.y = y;
      curr_text->style = style;
      curr_text->shadow = shadow;
      curr_text->scale = scale;
      curr_text->color = color;
      curr_text->origin = origin;
      break;
    }
  }

}

void drawStringNoT(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, const u8string string, ...)
{
  u8char finalstr[MAX_TEXT_SIZE];
  va_list args;
  va_start(args, string);
  vsnprintf(finalstr, MAX_TEXT_SIZE, string, args);
  va_end(args);

  drawStringNoTVarg(origin, style, scale, shadow, x, y, color, finalstr);
}

void drawStringNoVarg(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, const u8string string)
{
  drawStringNoTVarg(origin, style, scale, shadow, x, y, color, _t(string));
}

void drawString(int origin, int16 style, float scale, int16 shadow, float x, float y, CRGBA color, u8string string, ...) {

  string = _t(string); /* Translate text */

  u8char finalstr[MAX_TEXT_SIZE];
  va_list args;
  va_start(args, string);
  vsnprintf(finalstr, MAX_TEXT_SIZE, string, args);
  va_end(args);

  drawStringNoTVarg(origin, style, scale, shadow, x, y, color, finalstr);
}

void drawStringLCS(int origin, int16 style, float scale, float x, float y, CRGBA color, u8string string, ...) {
  wchar str[MSGLENGTH] = {' '};

  string = _t(string); /* Translate text */

  u8char finalstr[MAX_TEXT_SIZE];
  va_list args;
  va_start(args, string);
  vsnprintf(finalstr, MAX_TEXT_SIZE, string, args);
  va_end(args);

  Utf8ToGameEncoding(finalstr, str);

  CFont_SetColor(&color);
  CFont_SetPropOn();
  CFont_SetBackgroundOff();
  
  switch( origin )
  {
    case ALIGN_FREE:    CFont_SetRightJustifyOff(); break; // free
    case ALIGN_CENTER:  CFont_SetCentreOn();        break; // center
    case ALIGN_RIGHT:   CFont_SetRightJustifyOn();  break; // right
  }

  CFont_SetRightJustifyWrap(0.0f);
  CFont_SetWrapx(SCREEN_WIDTH);
  CFont_SetFontStyle(style);
  CFont_SetScaleLCS(scale / 2, scale); // eg: 0.4048,0.88

  CFont_PrintStringLCS(x, y, str, NULL);
}

void drawStringVCS(int origin, int16 style, float scale, float x, float y, CRGBA color, u8string string, ...) {
  wchar str[MSGLENGTH] = {' '};

  string = _t(string); /* Translate text */

  u8char finalstr[MAX_TEXT_SIZE];
  va_list args;
  va_start(args, string);
  vsnprintf(finalstr, MAX_TEXT_SIZE, string, args);
  va_end(args);

  Utf8ToGameEncoding(finalstr, str);

  CFont_SetFontStyle(style);
  ResetFontStyling();

  SetTextBounds(&(CRect){0.0f, SCREEN_HEIGHT, SCREEN_WIDTH, 0.0f}); // full screen

  SetTextSpaceing(1);
  SetTextOriginPoint(origin); // free
  CFont_SetScaleVCS(scale);
  CFont_SetColor(&color);

  CFont_PrintStringVCS(str, (int)x, (int)y);
}

/*void cWorldStream_Render_LCS_Patched(void *this, int mode) {

  cWorldStream_Render(this, mode);

  if( mode == 2 ) {
    for( int i = 0; i < MAX_TEXTS; i++ ) {
      if( wideBuffer[i][0] != '\0' ) {
        SetFontStyle(fontstyle[i]);
        SetColor(&fontcolor[i]);
        SetScale_LCS(fontscale[i]/2, fontscale[i]);
        SetRightJustifyOff();
        SetPropOn();
        SetBackgroundOff();

        PrintString_LCS(xpos[i], ypos[i], wideBuffer[i], NULL);
      } else break;
    } memset(wideBuffer, '\0', sizeof(wideBuffer[0][0])*MAX_TEXTS*MSGLENGTH); // clear my draw array because it has all been drawn
  }
}
void cWorldStream_Render_VCS_Patched(void *this, int mode) {

  cWorldStream_Render(this, mode);

  if(mode == 2) {
    for( int i = 0; i < MAX_TEXTS; i++ ) {
      if( wideBuffer[i][0] != '\0' ) {
        ResetFontStyling();
        SetFontStyle(fontstyle[i]);
        SetColor(&fontcolor[i]);
        SetScale_VCS(0.6f); //crashes when fontscale[i] and with different scales
        SetTextOriginPoint(0x0); //top left

        PrintString_VCS(wideBuffer[i], (int)xpos[i], (int)ypos[i]);
      } else break;
    } memset(wideBuffer, '\0', sizeof(wideBuffer[0][0])*MAX_TEXTS*MSGLENGTH); // clear my draw array because it has all been drawn

  }
}*/

void (*mymenurender_LCS)(void);
static void mymenurender_LCS_patched() {
  CRect boxRect = {0};

  /// BOXes
  for( uint32 i = 0; i < MAX_BOXES; i++ )
  {
    menu_blit_box* curr_box = &main_blit_boxes[i];

    if( curr_box->id != 0 )
    {
      CRect_CRect_2(curr_box->pos.x, curr_box->pos.y, curr_box->pos.x + curr_box->size.x, curr_box->pos.y + curr_box->size.y, &boxRect);
      CSprite2d_DrawRect_2(&boxRect, &curr_box->color, true);
      curr_box->id = 0; // Zero after drawing
      continue;
    }

    break;
  }

  /// TEXTs
  for( uint32 i = 0; i < MAX_TEXTS; i++ )
  {
    menu_blit_text* curr_text = &main_blit_texts[i];

    if( !STR_EMPTY(curr_text->text) )
    {
      CFont_SetFontStyle(curr_text->style);
      CFont_SetScaleLCS(curr_text->scale / 2.0f, curr_text->scale);

      CFont_SetColor(&curr_text->color);
      CFont_SetSlant(0.0f);
      CFont_SetDropShadowPosition(curr_text->shadow);

      if( curr_text->shadow == SHADOW_ON )// (fix) if shadow is enabled.. keep it from fading when for example vehicle/location text fades aways
        CFont_SetDropColor(&(CRGBA){ .a = 0xFF });

      /// origin: 0 = free positioning , 2 = center, 3 = left, 4 = right (uses X coordinate unlike VCS!)
      switch ( curr_text->origin )
      {
        case ALIGN_CENTER:
          CFont_SetRightJustifyOff();
          CFont_SetJustifyOff();
          CFont_SetCentreOn();
          CFont_SetCentreSize(SCREEN_WIDTH); // TODO:
          break;

        case ALIGN_FREE:
        case ALIGN_LEFT:
          CFont_SetCentreOff();
          CFont_SetRightJustifyOff();
          CFont_SetJustifyOn();
          CFont_SetWrapx(SCREEN_WIDTH);
          break;

        case ALIGN_RIGHT:
          CFont_SetCentreOff();
          CFont_SetJustifyOff();
          CFont_SetRightJustifyOn();
          CFont_SetRightJustifyWrap(0.0f);
          break;

        case ALIGN_SCREENCENTER:
          CFont_SetRightJustifyOff();
          CFont_SetJustifyOff();
          CFont_SetCentreOn();
          CFont_SetCentreSize(SCREEN_WIDTH);
          curr_text->pos.x = SCREEN_WIDTH / 2.0f;
          break;

        default:
          break;
      }

      CFont_SetPropOn(); // proportional
      CFont_SetBackgroundOff();
      CFont_SetBackGroundOnlyTextOff();

      CFont_PrintStringLCS(curr_text->pos.x, curr_text->pos.y, curr_text->text, NULL);

      CLEAR_STR(curr_text->text); // Zero string after drawing
      continue;
    }

    break;
  }

  mymenurender_LCS();
}

void (*mymenurender_VCS)(void);
static void mymenurender_VCS_patched() {
  const float posfix = 2.0f; // a fix to better match position compared to LCS version
  CRect boxRect = {0};

  /// BOXes
  for( array_uidx i = 0; i < MAX_BOXES; i++ )
  {
    menu_blit_box* curr_box = &main_blit_boxes[i];

    if( curr_box->id != 0 )
    {
      CRect_CRect_2(curr_box->pos.x, curr_box->pos.y, curr_box->pos.x + curr_box->size.x, curr_box->pos.y + curr_box->size.y, &boxRect);
      CSprite2d_DrawRect_2(&boxRect, &curr_box->color, true);
      curr_box->id = 0; // Zero after drawing
      continue;
    }

    break;
  }

  /// TEXTs
  for( array_uidx i = 0; i < MAX_TEXTS; i++ )
  {
    menu_blit_text* curr_text = &main_blit_texts[i];

    if( !STR_EMPTY(curr_text->text) )
    {
      CFont_SetFontStyle(curr_text->style);
      ResetFontStyling();

      CRect textBounds = {0.0f, SCREEN_HEIGHT, SCREEN_WIDTH, 0.0f};

      switch ( curr_text->origin )
      {
        case ALIGN_RIGHT:
          textBounds.right = curr_text->pos.x;
          break;

        case ALIGN_CENTER:
          textBounds.right = curr_text->pos.x * 2.0f;
          break;

        default:
          break;
      }

      SetTextSpaceing(1); // set 0 to put whitespace between each char
      SetTextBounds(&textBounds); // in combination with "SetTextOriginPoint" text can be centered! I did this to match LCS
      SetTextOriginPoint(curr_text->origin);

      /* * * * * * *
      //free
      0 = free positioning,
      1 = screen left
      2 = screen center,
      3 = screen left,
      4 = screen right,
      5 = screen left,
      6 = screen center,
      7 = screen left,

      //top
      8 = screen top with x,
      9 = screen top left,
      10 = screen top center,
      11 = screen top left,
      12 = screen top right,
      13 = screen top left
      14 = screen top center
      15 =

      //middle
      16 = with x
      17 =

      //top

      //bottom
      32 = bottom with x
      33 =
      * * * * * *  */

      CFont_SetScaleVCS(curr_text->scale);

      if( curr_text->shadow == SHADOW_ON && curr_text->style != FONT_NAMES ) { // fake shadow (black shifted text) + disable on FONT_NAMES because those have shadow by default
        CFont_SetColor(&(CRGBA){ .a = 0xBB });
        CFont_PrintStringVCS(curr_text->text, (int)(curr_text->pos.x + 1.0f), (int)(curr_text->pos.y + 1.0f + posfix));
      }

      CFont_SetColor(&curr_text->color);
      CFont_PrintStringVCS(curr_text->text, (int)curr_text->pos.x, (int)(curr_text->pos.y + posfix));

      CLEAR_STR(curr_text->text); // Zero after drawing
      continue;
    }

    break;
  }

  mymenurender_VCS();
}


static bool FindPatchLCS(uaddr addr, uaddr text_addr UNUSED) {
  /*************************************
   * ULUS-10041 v1.02 | 0x0010dfa8 | OK!
   * ULUS-10041 v1.05 | 0x001BEF40 | OK!
   * ULUS-10041 v3.00 | 0x001BEE84 | OK!
   * ULES-00182 v1.00 | 0x001BEE8C | OK!
   * ULES-00182 v2.00 | 0x001BEDD0 | OK!
   * ULES-00151 v1.05 | 0x001BEF40 | OK!
   * ULES-00151 v2.00 | 0x001BEF40 | OK!
   * ULES-00151 v3.00 | 0x001BEE84 | OK!
   * ULES-00151 v4.00 | 0x001BEE84 | OK!
   * ULJM-05255 v1.01 | 0x001C3FB8 | OK!
   * ULET-00361 v0.02 | 0x001BEE84 | OK!
   * ULET-00362 v0.01 | 0x001BEDD0 | OK!
   * ULUX-80146 v0.02 | 0x00077AA0 | OK!
   **************************************/
  if( !mymenurender_LCS && _lw(addr + 0x34) == 0x24C5FEE0 && _lw(addr + 0x24) == 0x00C02025 ) { // 0x001BEE84
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> mymenurender_LCS_patched", addr-text_addr, addr);
    HIJACK_FUNCTION(addr, mymenurender_LCS_patched, mymenurender_LCS);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x00250F28 | OK!
   * ULUS-10041 v3.00 | 0x00250DF8 | OK!
   * ULES-00182 v1.00 | 0x00250E74 | OK!
   * ULES-00182 v2.00 | 0x00250D44 | OK!
   * ULES-00151 v1.05 | 0x00250F28 | OK!
   * ULES-00151 v2.00 | 0x00250F28 | OK!
   * ULES-00151 v3.00 | 0x00250DF8 | OK!
   * ULES-00151 v4.00 | 0x00250DF8 | OK!
   * ULJM-05255 v1.01 | 0x00258F60 | OK!
   * ULET-00361 v0.02 | 0x00250DF8 | OK!
   * ULET-00362 v0.01 | 0x00250D44 | OK!
   * ULUX-80146 v0.02 | 0x001C3F10 | OK!
   **************************************/
  if( !CFont_SetScaleLCS && _lw(addr - 0x4) == 0xA080001F && _lw(addr + 0x8) == 0xE48C0004 && _lw(addr + 0x10) == 0xE48D0008 && _lw(addr + 0x20) == 0xE48C000C ) { // 0x00250DF8
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetScale", addr-text_addr, addr);
    CFont_SetScaleLCS       = (void(*)(float x, float y))(addr);
    CFont_SetSlant          = (void(*)(float s))(addr+0x14); // FUN_00250e0c_SetSlant
    CFont_SetSlantRefPoint  = (void(*)(float x, float y))(addr+0x24); // FUN_00250e1c_SetSlantRefPoint
    return true;
  }

  /*************************************  OK!
   * ULUS-10041 v1.05 | 0x00250F60 | OK!
   * ULUS-10041 v3.00 | 0x00250E30 | OK!
   * ULES-00182 v1.00 | 0x00250EAC | OK!
   * ULES-00182 v2.00 | 0x00250D7C | OK!
   * ULES-00151 v1.05 | 0x00250F60 | OK!
   * ULES-00151 v2.00 | 0x00250F60 | OK!
   * ULES-00151 v3.00 | 0x00250E30 | OK!
   * ULES-00151 v4.00 | 0x00250E30 | OK!
   * ULJM-05255 v1.01 | 0x00258F98 | OK!
   * ULET-00361 v0.02 | 0x00250E30 | OK!
   * ULET-00362 v0.01 | 0x00250D7C | OK!
   * ULUX-80146 v0.02 | 0x001C3F48 | OK!
   **************************************/
  if( !CFont_SetColor && _lw(addr - 0x4) == 0xE48D0014 && _lw(addr) == 0x00802825 && _lw(addr + 0x4) == 0x90A40000 && _lw(addr + 0x10) == 0x90A70001 ) { // 0x00250E30
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetColor", addr-text_addr, addr);
    CFont_SetColor = (void(*)(CRGBA *col))(addr);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x00251044 | OK!
   * ULUS-10041 v3.00 | 0x00250F14 | OK!
   * ULES-00182 v1.00 | 0x00250F90 | OK!
   * ULES-00182 v2.00 | 0x00250E60 | OK!
   * ULES-00151 v1.05 | 0x00251044 | OK!
   * ULES-00151 v2.00 | 0x00251044 | OK!
   * ULES-00151 v3.00 | 0x00250F14 | OK!
   * ULES-00151 v4.00 | 0x00250F14 | OK!
   * ULJM-05255 v1.01 | 0x0025907C | OK!
   * ULET-00361 v0.02 | 0x00250F14 | OK!
   * ULET-00362 v0.01 | 0x00250E60 | OK!
   * ULUX-80146 v0.02 | 0x001C402C | OK!
   **************************************/
  if( !CFont_SetRightJustifyOff && _lw(addr + 0xC) == 0xA0800018 && _lw(addr + 0x10) == 0x03E00008 && _lw(addr + 0x14) == 0xA0800019 && _lw(addr + 0x18) == 0x44807000 && _lw(addr + 0x1C) == 0x3C0443F0 ) { // 0x00250F14
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetJustify(s)", addr-text_addr, addr);
    CFont_SetRightJustifyOff = (void(*)(void))(addr);
    CFont_SetRightJustifyOn  = (void(*)(void))(addr - 0x1C); //FUN_00250ef8_
    CFont_SetCentreOff       = (void(*)(void))(addr - 0x2C); //FUN_00250ee8_
    CFont_SetCentreOn        = (void(*)(void))(addr - 0x48); //FUN_00250ecc_
    CFont_SetJustifyOff      = (void(*)(void))(addr - 0x5C); //FUN_00250eb8_
    CFont_SetJustifyOn       = (void(*)(void))(addr - 0x78); //FUN_00250e9c_
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x00251100 | OK!
   * ULUS-10041 v3.00 | 0x00250FD0 | OK!
   * ULES-00182 v1.00 | 0x0025104C | OK!
   * ULES-00182 v2.00 | 0x00250F1C | OK!
   * ULES-00151 v1.05 | 0x00251100 | OK!
   * ULES-00151 v2.00 | 0x00251100 | OK!
   * ULES-00151 v3.00 | 0x00250FD0 | OK!
   * ULES-00151 v4.00 | 0x00250FD0 | OK!
   * ULJM-05255 v1.01 | 0x00259138 | OK!
   * ULET-00361 v0.02 | 0x00250FD0 | OK!
   * ULET-00362 v0.01 | 0x00250F1C | OK!
   * ULUX-80146 v0.02 | 0x001C40E8 | OK!
   **************************************/
  if( !CFont_SetBackgroundOff && _lw(addr - 0x4) == 0xA0A4001B && _lw(addr + 0x8) == 0x03E00008 && _lw(addr + 0xC) == 0xA080001B && _lw(addr + 0x10) == 0x90850000 && _lw(addr + 0x1C) == 0xA0C50028 ) { // 0x00250FD0
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetBackgroundOff", addr-text_addr, addr);
    CFont_SetBackgroundOff = (void(*)(void))(addr);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x00251160 | OK!
   * ULUS-10041 v3.00 | 0x00251030 | OK!
   * ULES-00182 v1.00 | 0x002510AC | OK!
   * ULES-00182 v2.00 | 0x00250F7C | OK!
   * ULES-00151 v1.05 | 0x00251160 | OK!
   * ULES-00151 v2.00 | 0x00251160 | OK!
   * ULES-00151 v3.00 | 0x00251030 | OK!
   * ULES-00151 v4.00 | 0x00251030 | OK!
   * ULJM-05255 v1.01 | 0x00259198 | OK!
   * ULET-00361 v0.02 | 0x00251030 | OK!
   * ULET-00362 v0.01 | 0x00250F7C | OK!
   * ULUX-80146 v0.02 | 0x001C4148 | OK!
   **************************************/
  if( !CFont_SetPropOn && _lw(addr - 0x4) == 0xA080001C && _lw(addr + 0x4) == 0x34040001 && _lw(addr + 0xC) == 0x03E00008 && _lw(addr + 0x10) == 0xA0A4001D ) { // 0x00251030
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetPropOn", addr-text_addr, addr);
    CFont_SetPropOn   = (void(*)(void))(addr);
    CFont_SetPropOff  = (void(*)(void))(addr + 0x14);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x00251184 | OK!
   * ULUS-10041 v3.00 | 0x00251054 | OK!
   * ULES-00182 v1.00 | 0x002510D0 | OK!
   * ULES-00182 v2.00 | 0x00250FA0 | OK!
   * ULES-00151 v1.05 | 0x00251184 | OK!
   * ULES-00151 v2.00 | 0x00251184 | OK!
   * ULES-00151 v3.00 | 0x00251054 | OK!
   * ULES-00151 v4.00 | 0x00251054 | OK!
   * ULJM-05255 v1.01 | 0x002591BC | OK!
   * ULET-00361 v0.02 | 0x00251054 | OK!
   * ULET-00362 v0.01 | 0x00250FA0 | OK!
   * ULUX-80146 v0.02 | 0x001C416C | OK!
   **************************************/
  if( !CFont_SetFontStyle && _lw(addr) == 0x00042C00 && _lw(addr + 0xc) == 0x34060002 && _lw(addr + 0x10) == 0x14A60005 && _lw(addr + 0x18) == 0xA4850038 ) { // 0x00251054
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetFontStyle", addr-text_addr, addr);
    CFont_SetFontStyle = (void(*)(int16 style))(addr);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x002511CC | OK!
   * ULUS-10041 v3.00 | 0x0025109C | OK!
   * ULES-00182 v1.00 | 0x00251118 | OK!
   * ULES-00182 v2.00 | 0x00250FE8 | OK!
   * ULES-00151 v1.05 | 0x002511CC | OK!
   * ULES-00151 v2.00 | 0x002511CC | OK!
   * ULES-00151 v3.00 | 0x0025109C | OK!
   * ULES-00151 v4.00 | 0x0025109C | OK!
   * ULJM-05255 v1.01 | 0x00259204 | OK!
   * ULET-00361 v0.02 | 0x0025109C | OK!
   * ULET-00362 v0.01 | 0x00250FE8 | OK!
   * ULUX-80146 v0.02 | 0x001C41B4 | OK!
   **************************************/
  if( !CFont_SetDropShadowPosition && _lw(addr - 0x3C) == 0x34060002 && _lw(addr + 0x10) == 0x00802825 &&  _lw(addr + 0xB4) == 0x00000000 ) { // 0x0025109c
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetDropShadowPosition & CFont::SetDropColor", addr-text_addr, addr);
    CFont_SetDropShadowPosition = (void(*)(int16 pos))(addr);
    CFont_SetDropColor          = (void(*)(CRGBA *col))(addr + 0x10);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x00252C04 | OK!
   * ULUS-10041 v3.00 | 0x00252AD4 | OK!
   * ULES-00182 v1.00 | 0x00252B50 | OK!
   * ULES-00182 v2.00 | 0x00252A20 | OK!
   * ULES-00151 v1.05 | 0x00252C04 | OK!
   * ULES-00151 v2.00 | 0x00252C04 | OK!
   * ULES-00151 v3.00 | 0x00252AD4 | OK!
   * ULES-00151 v4.00 | 0x00252AD4 | OK!
   * ULJM-05255 v1.01 | 0x00259F60 | OK!
   * ULET-00361 v0.02 | 0x00252AD4 | OK!
   * ULET-00362 v0.01 | 0x00252A20 | OK!
   * ULUX-80146 v0.02 | 0x001C54B8 | OK!
   **************************************/
  if( !CFont_PrintStringLCS && _lw(addr - 0x6C) == 0x0211A02B && _lw(addr - 0xD8) == 0x0211A02B ) { // 0x00252AD4
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::PrintString", addr-text_addr, addr);
    CFont_PrintStringLCS = (void(*)(float x, float y, wchar *s, int* unk))(addr);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 | 0x002CFEE8 | OK!
   * ULUS-10041 v3.00 | 0x002CFDD0 | OK!
   * ULES-00182 v1.00 | 0x002CFE7C | OK!
   * ULES-00182 v2.00 | 0x002CFD64 | OK!
   * ULES-00151 v1.05 | 0x002CFF30 | OK!
   * ULES-00151 v2.00 | 0x002CFF30 | OK!
   * ULES-00151 v3.00 | 0x002CFE18 | OK!
   * ULES-00151 v4.00 | 0x002CFE18 | OK!
   * ULJM-05255 v1.01 | 0x002D7BD4 | OK!
   * ULET-00361 v0.02 | 0x002CFE18 | OK!
   * ULET-00362 v0.01 | 0x002CFD64 | OK!
   * ULUX-80146 v0.02 | 0x0017C10C | OK!
   **************************************/
  if( !CSprite2d_DrawRect_2 && _lw(addr+0x20) == 0x02004025 && _lw(addr + 0x8) == 0x30C900FF && _lw(addr + 0x64) == 0x3404000C /*0x3404000A for VCS*/ ) {  // 0x002CFDD0
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CSprite2d::DrawRect", addr-text_addr, addr);
    CSprite2d_DrawRect_2 = (void(*)(const CRect *r, const CRGBA *col, bool screenUnits))(addr);
    return true;
  }

  /*************************************
   * ULUS-10041 v1.05 |
   * ULUS-10041 v3.00 | 0x00250F2C | OK!
   * ULES-00182 v1.00 |
   * ULES-00182 v2.00 |
   * ULES-00151 v1.05 |
   * ULES-00151 v2.00 |
   * ULES-00151 v3.00 |
   * ULES-00151 v4.00 |
   * ULJM-05255 v1.01 |
   * ULET-00361 v0.02 |
   * ULET-00362 v0.01 |
   * ULUX-80146 v0.02 |
   **************************************/
  if( !CFont_SetRightJustifyWrap && _lw(addr+0x4) == 0x3C0443F0 && _lw(addr + 0x44) == 0x3C0443F0 && _lw(addr + 0x68) == 0x00000000 ) {
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetRightJustifyWrap", addr-text_addr, addr);
    DEBUG_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetWrapx", addr+0x40-text_addr, addr+0x40);
    CFont_SetRightJustifyWrap       = (void(*)(float wrap))(addr); // 0x00250F2C
    CFont_SetWrapx                  = (void(*)(float x))(addr+0x40); // 0x00250f6c
    CFont_SetCentreSize             = (void(*)(float s))(addr+0x80); // FUN_00250fac_SetCentreSize
    CFont_SetBackgroundOn           = (void(*)(void))(addr+0x90); // FUN_00250fbc_SetBackgroundOn
    CFont_SetBackgroundOff          = (void(*)(void))(addr+0xA4); // FUN_00250fd0_SetBackgroundOff
    CFont_SetBackgroundColor        = (void(*)(CRGBA *col))(addr+0xB4); // FUN_00250fe0_SetBackgroundColor
    CFont_SetBackGroundOnlyTextOn   = (void(*)(void))(addr+0xE0); // FUN_0025100c_SetBackGroundOnlyTextOn
    CFont_SetBackGroundOnlyTextOff  = (void(*)(void))(addr+0xF4); // FUN_00251020_SetBackGroundOnlyTextOff
    return true;
  }

  return false;
}


static bool FindPatchVCS(uaddr addr, uaddr text_addr UNUSED) {
  /*************************************
   * ULUS-10160 v1.03 | 0x001325BC | OK!
   * ULES-00502 v1.02 | 0x001324E8 | OK!
   * ULES-00503 v1.02 | 0x001324E8 | OK!
   * ULJM-05297 v1.01 | 0x001C99F8 | OK!
   * ULET-00417 v0.06 | 0x0012A07C | OK!
   **************************************/
  if( !mymenurender_VCS && _lw(addr - 0xB0) == 0x34060001 && _lw(addr + 0x98) == 0x28A60000  && _lw(addr + 0xD0) == 0x00003825 ) {
    PATCH_LOG("blitn: 0x%08X (0x%08X) -> mymenurender_VCS_patched", addr-text_addr, addr);
    HIJACK_FUNCTION(addr, mymenurender_VCS_patched, mymenurender_VCS); // 0x01325bc
    return true;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x0030805C | OK!
   * ULES-00502 v1.02 | 0x0030843C | OK!
   * ULES-00503 v1.02 | 0x003082B4 | OK!
   * ULJM-05297 v1.01 | 0x000FCB5C | OK!
   * ULET-00417 v0.06 | 0x002FF87C | OK!
   **************************************/
  if( !ResetFontStyling && _lw(addr - 0x4) == 0x27BD0010 && _lw(addr) == 0x27BDFFE0 && _lw(addr + 0x4) == 0xAFB00014 && _lw(addr + 0x8) == 0x341000FF ) {
    PATCH_LOG("blitn: 0x%08X (0x%08X) -> ResetFontStyling", addr-text_addr, addr);
    ResetFontStyling = (void*)(addr); // 0x0030805C
    return true;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x00308138 | OK!
   * ULES-00502 v1.02 | 0x00308518 | OK!
   * ULES-00503 v1.02 | 0x00308390 | OK!
   * ULJM-05297 v1.01 | 0x000FCC40 | OK!
   * ULET-00417 v0.06 | 0x002FF958 | OK!
   **************************************/
  if( !CFont_PrintStringVCS && _lw(addr + 0x30) == 0x00409825 && _lw(addr + 0x54) == 0x00404825 ) {
    PATCH_LOG("blitn: 0x%08X (0x%08X) -> CFont::PrintString", addr-text_addr, addr);
    CFont_PrintStringVCS = (void(*)(wchar *text, int x, int y))(addr); // 0x00308138
    return true;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x00308498 | OK!
   * ULES-00502 v1.02 | 0x00308878 | OK!
   * ULES-00503 v1.02 | 0x003086F0 | OK!
   * ULJM-05297 v1.01 | 0x000FCCD8 | OK!
   * ULET-00417 v0.06 | 0x002FFCB8 | OK!
   **************************************/
  if( !CFont_SetFontStyle && _lw(addr) == 0x03E00008 && _lw(addr + 0x8) == 0x03E00008 && _lw(addr + 0x20) == 0x90840003 ) {
    PATCH_LOG("blitn: 0x%08X (0x%08X) -> CFont::SetFontStyle, CFont::SetColor, CFont::SetScale, ...", addr-text_addr, addr);
    CFont_SetFontStyle  = (void(*)(int16 style))(addr); // 0x308498
    CFont_SetColor      = (void(*)(CRGBA *col))(addr+0x10); // 0x3084A8
    SetTextBounds       = (void(*)(CRect *bounds))(addr+0x3C); // 0x3084d4
    CFont_SetScaleVCS   = (void(*)(float scale))(addr+0x88); // 0x308520
    SetTextSpaceing     = (void(*)(int option))(addr+0x130); // 0x3085c8
    SetTextOriginPoint  = (void(*)(int origin))(addr+0x16C); // 0x308604
    return true;
  }
  if( !SetTextSpaceing && _lw(addr+0x20) == 0x00052880 && _lw(addr + 0x68) == 0x00052880 && _lw(addr + 0x6C) == 0x00852021 ) { // JP only: 0x000FCE10
    PATCH_LOG("blitn: 0x%08X (0x%08X) -> SetTextSpaceing, SetTextOriginPoint (JP only)", addr-text_addr, addr);
    SetTextSpaceing     = (void(*)(int option))(addr); // sub_000FCE10
    SetTextOriginPoint  = (void(*)(int origin))(addr+0x48); // sub_000FCE58
    return true;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x002F1130 | OK!
   * ULES-00502 v1.02 | 0x002F1510 | OK!
   * ULES-00503 v1.02 | 0x002F1388 | OK!
   * ULJM-05297 v1.01 | 0x0032FEB4 | OK!
   * ULET-00417 v0.06 | 0x002EBC20 | OK!
   **************************************/
  if( !CSprite2d_DrawRect_2 && _lw(addr+0x20) == 0x02004025 && _lw(addr+0x8) == 0x30C900FF && _lw(addr+0x64) == 0x3404000A /* 0x3404000C for LCS*/ ) {
    PATCH_LOG("blitn: 0x%08X (0x%08X) -> CSprite2d::DrawRect", addr-text_addr, addr);
    CSprite2d_DrawRect_2 = (void(*)(const CRect *r, const CRGBA *col, bool screenUnits))(addr);
    return true;
  }

  return false;
}


func_stat initTextBlit(uaddr text_addr, usize text_size) {
  DEBUG_LOG("Starting blitn...");

  gta_version = -1;

  for( uaddr i = 0; i < text_size; i += sizeof(uaddr) ) {
    uaddr addr = text_addr + i;
    if ((gta_version == -1 || gta_version == 0) && FindPatchVCS(addr, text_addr)) {
      gta_version = 0;
      continue;
    }
    if ((gta_version == -1 || gta_version == 1) && FindPatchLCS(addr, text_addr)) {
      gta_version = 1;
      continue;
    }
  }

  if( gta_version == -1 )
  {
    ERROR_LOG("Error!");
    return FUNC_ERROR; // error
  }

  DEBUG_LOG("Success! (%s)", gta_version ? "LCS" : "VCS");
  return FUNC_OK;
}
