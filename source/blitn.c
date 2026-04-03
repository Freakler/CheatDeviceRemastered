/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-2025, Freakler
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

/******************************************************************************************
You can display MAX_TEXTS messages with a length of MSGLENGTH characters!
Call "initTextBlit()" with the GTA modules' text_addr

Then blit text to screen via:
  drawString(char *string, short style, float scale, float x, float y, u32 color);

Draw Rectangles via:
  void drawBox(float x, float y, float width, float height, u32 color);
*******************************************************************************************/

#include <pspsdk.h>
#include <pspkernel.h>
#include <systemctrl.h>

#include <string.h>
#include <wchar.h>
#include <stdarg.h>

#include "blitn.h"

#ifdef LOG
 #include "utils.h"
#endif

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

static menu_blit_text main_blit_texts[MAX_TEXTS] = { };
static menu_blit_box main_blit_boxes[MAX_BOXES] = { };

static int gta_version = -1;

// Source: https://github.com/Sergeanur/GXT-compiler/blob/49ed88b3/tables/lcs_table.txt
// The unicode arrays must be sorted.
static const u16 LCS_UNICODES[] = {
  0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9,
  0x00AA, 0x00AB, 0x00AE, 0x00B0, 0x00B9, 0x00BB, 0x00BF, 0x00C0, 0x00C1,
  0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA,
  0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2, 0x00D3,
  0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC,
  0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5,
  0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE,
  0x00EF, 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
  0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF, 0x0152,
  0x0153, 0x0178, 0x02C6, 0x02DC, 0x2018, 0x2019, 0x201A, 0x201C, 0x201D,
  0x201E, 0x2039, 0x203A, 0x20AC, 0x2122, 0xFFFD
};
static const u8 LCS_GAMECODES[] = {
  0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, // ¡ ¢ £ ¤ ¥ ¦ § ¨ © ª,
  0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0x80, 0x81, 0x82, 0x83, // « ® ° ¹ » ¿ À Á Â Ã,
  0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, // Ä Å Æ Ç È É Ê Ë Ì Í,
  0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, // Î Ï Ð Ñ Ò Ó Ô Õ Ö ×,
  0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, // Ø Ù Ú Û Ü Ý Þ ß à á,
  0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, // â ã ä å æ ç è é ê ë,
  0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, // ì í î ï ð ñ ò ó ô õ,
  0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, // ö ÷ ø ù ú û ü ý þ ÿ,
  0xC5, 0xCD, 0xCE, 0xC3, 0xCA, 0xC6, 0xC7, 0xC1, 0xC8, 0xC9, // Œ œ Ÿ ˆ ˜ ‘ ’ ‚ “ ”,
  0xC2, 0xC4, 0xCC, 0xC0, 0xCB, 0xDF                          // „ ‹ › € ™ �
};

static const u16 VCS_UNICODES[] = {
  0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A9, 0x00AA,
  0x00AB, 0x00AE, 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B9, 0x00BA, 0x00BB,
  0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
  0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0,
  0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9,
  0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2,
  0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB,
  0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4,
  0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD,
  0x00FE, 0x00FF, 0x0152, 0x0153, 0x0160, 0x0161, 0x0178, 0x017D, 0x017E,
  0x0192, 0x02C6, 0x02DC, 0x0384, 0x03BC, 0x2022, 0x208D, 0x208E, 0x2122,
  0xFFFD
};
static const u8 VCS_GAMECODES[] = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA9, 0xAA, 0xAB, // ¡ ¢ £ ¤ ¥ ¦ § © ª «,
  0xAE, 0xB0, 0xB1, 0xB2, 0xB3, 0xB9, 0xBA, 0xBB, 0xBF, 0xC0, // ® ° ± ² ³ ¹ º » ¿ À,
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, // Á Â Ã Ä Å Æ Ç È É Ê,
  0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, // Ë Ì Í Î Ï Ð Ñ Ò Ó Ô,
  0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, // Õ Ö × Ø Ù Ú Û Ü Ý Þ,
  0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // ß à á â ã ä å æ ç è,
  0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, // é ê ë ì í î ï ð ñ ò,
  0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, // ó ô õ ö ÷ ø ù ú û ü,
  0xFD, 0xFE, 0xFF, 0x8C, 0x9C, 0x8A, 0x9A, 0x9F, 0x8E, 0x9E, // ý þ ÿ Œ œ Š š Ÿ Ž ž,
  0x83, 0x88, 0x98, 0xB4, 0xB5, 0x95, 0x8B, 0x9B, 0x99, 0x80  // ƒ ˆ ˜ ΄ μ • ₍ ₎ ™ �
};

static const u16 *UNICODES[] = { VCS_UNICODES, LCS_UNICODES };
static const u8 *GAMECODES[] = { VCS_GAMECODES, LCS_GAMECODES };
static const size_t CHARMAP_LENS[] = {
  sizeof(VCS_UNICODES) / sizeof(VCS_UNICODES[0]),
  sizeof(LCS_UNICODES) / sizeof(LCS_UNICODES[0])
};

// Using question mark because replacement character is missing in FONT_HEADING.
#define REPLACEMENT_CHAR 0x3F

// Binary search version
static wchar_t UnicodeToGameChar(uint32_t unicode, const u16 *unicodes,
         const u8 *gamecodes, const size_t charmap_len)
{
  if (unicode > 0xFFFF)
    return REPLACEMENT_CHAR;
  size_t low = 0;
  size_t high = charmap_len - 1;
  while (low <= high) {
    size_t mid = low + (high - low) / 2;

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
static inline wchar_t UnicodeToGameChar(uint32_t unicode, const u16 *unicodes, const u8 *gamecodes, const size_t charmap_len) {
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

static int GetUtf8CharLen(u8 byte)
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

static int IsCont(u8 byte)
{
  return (byte & 0xC0) == 0x80;
}

static void PutReplacementChar(wchar_t **out)
{
  *(*out)++ = REPLACEMENT_CHAR;
}

static void Utf8ToGameEncoding(const char *in, wchar_t *out)
{
  if (gta_version != 0 && gta_version != 1) {
    *out = '\0';
    return;
  }

  const u16 *unicodes = UNICODES[gta_version];
  const u8 *gamecodes = GAMECODES[gta_version];
  const size_t charmap_len = CHARMAP_LENS[gta_version];

  while (*in != '\0') {
    unsigned char c1 = (unsigned char)*in++;
    uint32_t unicode;

    int len = GetUtf8CharLen(c1);

    switch (len) {
    case 1: {
      *out++ = (wchar_t)c1;
      continue;
    }

    case 2: {
      if (*in == '\0' || !IsCont(*in)) {
        PutReplacementChar(&out);
        continue;
      }
      u8 c2 = (u8)*in++;
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
      u8 c2 = (u8)*in++;
          u8 c3 = (u8)*in++;

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
      u8 c2 = (u8)*in++;
      u8 c3 = (u8)*in++;
      u8 c4 = (u8)*in++;

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

void drawBox(float x, float y, float width, float height, u32 color) {
  int i;
  for( i = 0; i < MAX_BOXES; i++ ) {
    menu_blit_box* curr_box = &main_blit_boxes[i];

    if( curr_box->id == (x+y+width+height) ) break;

    if( curr_box->id == 0 ) {
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

void drawUiBox(float x, float y, float width, float height, float bordersize, u32 bordercolor, u32 bgcolor) {
  drawBox(x, y, width, height, bgcolor); // x, y, width, height, color

  drawBox(x-bordersize, y-bordersize, width+(bordersize*2), bordersize, bordercolor); // top
  drawBox(x-bordersize, y+height, width+(bordersize*2), bordersize, bordercolor); // bottom

  drawBox(x-bordersize, y, bordersize, height, bordercolor); // left
  drawBox(x+width, y, bordersize, height, bordercolor); // right
}

void drawString(const char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color) {
  int i;
  for( i = 0; i < MAX_TEXTS; i++ ) {
    menu_blit_text* curr_text = &main_blit_texts[i];

    if( curr_text->text[0] == '\0' ) {
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

void drawStringLCS(const char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color) {
  wchar_t str[MSGLENGTH] = L" ";
  Utf8ToGameEncoding(string, str);

  SetColor(&color);
  SetPropOn();
  SetBackgroundOff();
  switch( origin ) {
    case 0: SetRightJustifyOff(); break; // free
    case 2: SetCentreOn(); break; // center
    case 4: SetRightJustifyOn(); break; // right
  }
  SetRightJustifyWrap(0.0f);
  SetWrapx(SCREEN_WIDTH);
  SetFontStyle(style);
  SetScale_LCS(scale/2,scale); // eg: 0.4048,0.88

  PrintString_LCS(x,y, str, 0);
}

void drawStringVCS(const char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color) {
  float coords[4];

  wchar_t str[MSGLENGTH] = L" ";
  Utf8ToGameEncoding(string, str);

  SetFontStyle(style);
  ResetFontStyling();

  coords[0] = 0.0f;
  coords[1] = SCREEN_HEIGHT;
  coords[2] = SCREEN_WIDTH;
  coords[3] = 0.0f;
  SetTextBounds(coords); // full screen

  SetTextSpaceing(1);
  SetTextOriginPoint(origin); // free
  SetScale_VCS(scale);
  SetColor(&color);

  PrintString_VCS(str, (int)x, (int)y);
}

/*void cWorldStream_Render_LCS_Patched(void *this, int mode) {

  //int i;

  cWorldStream_Render(this, mode);

  if( mode == 2 ) {
    for( i = 0; i < MAX_TEXTS; i++ ) {
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

  //int i;

  cWorldStream_Render(this, mode);

  if(mode == 2) {
    for( i = 0; i < MAX_TEXTS; i++ ) {
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
  int i;
  float coordinates[4];
  float *ptr_coords;

  /// BOXes
  for( i = 0; i < MAX_BOXES; i++ )
  {
    menu_blit_box* curr_box = &main_blit_boxes[i];

    if( curr_box->id != 0 )
    {
      ptr_coords = (float *)SetBoxCoords(curr_box->pos.x, curr_box->pos.y, curr_box->pos.x + curr_box->size.x, curr_box->pos.y + curr_box->size.y, coordinates);
      DrawRect(ptr_coords, &curr_box->color, 0x1); // todo
      curr_box->id = 0; // Zero after drawing
      continue;
    }

    break;
  }

  /// TEXTs
  for( i = 0; i < MAX_TEXTS; i++ )
  {
    menu_blit_text* curr_text = &main_blit_texts[i];

    if( curr_text->text[0] != '\0' )
    {
      SetFontStyle(curr_text->style);
      SetScale_LCS(curr_text->scale / 2.0f, curr_text->scale);

      SetColor(&curr_text->color);
      SetSlant(0.0f);
      SetDropShadowPosition(curr_text->shadow);

      if( curr_text->shadow == SHADOW_ON ) { // (fix) if shadow is enabled.. keep it from fading when for example vehicle/location text fades aways
        char rgba[] = { 0x00, 0x00, 0x00, 0xFF }; // todo allow rgb values (although LCS only! -> ?)
        SetDropColor(rgba);
      }

      /// origin: 0 = free positioning , 2 = center, 3 = left, 4 = right (uses X coordinate unlike VCS!)
      switch ( curr_text->origin )
      {
        case ALIGN_CENTER:
          SetRightJustifyOff();
          SetJustifyOff();
          SetCentreOn();
          SetCentreSize(SCREEN_WIDTH); // todo I guess
          break;

        case ALIGN_FREE:
        case ALIGN_LEFT:
          SetCentreOff();
          SetRightJustifyOff();
          SetJustifyOn();
          SetWrapx(SCREEN_WIDTH);
          break;

        case ALIGN_RIGHT:
          SetCentreOff();
          SetJustifyOff();
          SetRightJustifyOn();
          SetRightJustifyWrap(0.0f);
          break;

        case ALIGN_SCREENCENTER:
          SetRightJustifyOff();
          SetJustifyOff();
          SetCentreOn();
          SetCentreSize(SCREEN_WIDTH);
          curr_text->pos.x = SCREEN_WIDTH / 2.0f;
          break;

        default:
          break;
      }

      SetPropOn(); // proportional
      SetBackgroundOff();
      SetBackGroundOnlyTextOff();

      PrintString_LCS(curr_text->pos.x, curr_text->pos.y, curr_text->text, NULL);

      curr_text->text[0] = '\0'; // Zero string after drawing
      continue;
    }

    break;
  }

  mymenurender_LCS();
}

void (*mymenurender_VCS)(void);
static void mymenurender_VCS_patched() {
  int i;
  float coordinates[4];
  float *ptr_coords;
  float posfix = 2.0f; // a fix to better match position compared to LCS version
  float coords[4];

  /// BOXes
  for( i = 0; i < MAX_BOXES; i++ )
  {
    menu_blit_box* curr_box = &main_blit_boxes[i];

    if( curr_box->id != 0 )
    {
      ptr_coords = (float *)SetBoxCoords(curr_box->pos.x, curr_box->pos.y, curr_box->pos.x + curr_box->size.x, curr_box->pos.y + curr_box->size.y, coordinates);
      DrawRect(ptr_coords, &curr_box->color, 0x1); // todo
      curr_box->id = 0; // Zero after drawing
      continue;
    }

    break;
  }

  /// TEXTs
  for( i = 0; i < MAX_TEXTS; i++ )
  {
    menu_blit_text* curr_text = &main_blit_texts[i];

    if( curr_text->text[0] != '\0' )
    {
      SetFontStyle(curr_text->style);
      ResetFontStyling();
      coords[0] = 0.0f;
      coords[1] = SCREEN_HEIGHT;
      coords[2] = SCREEN_WIDTH;
      coords[3] = 0.0f;

      switch ( curr_text->origin )
      {
        case ALIGN_RIGHT:
          coords[2] = curr_text->pos.x;
          break;

        case ALIGN_CENTER:
          coords[2] = curr_text->pos.x * 2.0f;
          break;

        default:
          break;
      }

      SetTextSpaceing(1); // set 0 to put whitespace between each char
      SetTextBounds(coords); // in combination with "SetTextOriginPoint" text can be centered! I did this to match LCS
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

      SetScale_VCS(curr_text->scale);

      if( curr_text->shadow == SHADOW_ON && curr_text->style != FONT_NAMES ) { // fake shadow (black shifted text) + disable on FONT_NAMES because those have shadow by default
        u32 rgba[] = { 0xBB000000 };
        SetColor(rgba);
        PrintString_VCS(curr_text->text, (int)(curr_text->pos.x + 1.0f), (int)(curr_text->pos.y + 1.0f + posfix));
      }

      SetColor(&curr_text->color);
      PrintString_VCS(curr_text->text, (int)curr_text->pos.x, (int)(curr_text->pos.y + posfix));

      curr_text->text[0] = '\0'; // Zero after drawing
      continue;
    }

    break;
  }

  mymenurender_VCS();
}


static int FindPatchLCS(u32 addr, u32 text_addr) {
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
  if( _lw(addr + 0x34) == 0x24C5FEE0 && _lw(addr + 0x24) == 0x00C02025 ) { // 0x001BEE84
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> mymenurender_LCS_patched", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, mymenurender_LCS_patched, mymenurender_LCS);
    return 1;
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
  if( _lw(addr - 0x4) == 0xA080001F && _lw(addr + 0x8) == 0xE48C0004 && _lw(addr + 0x10) == 0xE48D0008 && _lw(addr + 0x20) == 0xE48C000C ) { // 0x00250DF8
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetScale_LCS", addr-text_addr, addr);
    #endif
    SetScale_LCS = (void*)(addr);
    SetSlant = (void*)(addr+0x14); // FUN_00250e0c_SetSlant
    SetSlantRefPoint = (void*)(addr+0x24); // FUN_00250e1c_SetSlantRefPoint
    return 1;
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
  if( _lw(addr - 0x4) == 0xE48D0014 && _lw(addr) == 0x00802825 && _lw(addr + 0x4) == 0x90A40000 && _lw(addr + 0x10) == 0x90A70001 ) { // 0x00250E30
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetColor", addr-text_addr, addr);
    #endif
    SetColor = (void*)(addr);
    return 1;
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
  if( _lw(addr + 0xC) == 0xA0800018 && _lw(addr + 0x10) == 0x03E00008 && _lw(addr + 0x14) == 0xA0800019 && _lw(addr + 0x18) == 0x44807000 && _lw(addr + 0x1C) == 0x3C0443F0 ) { // 0x00250F14
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetJustify(s)", addr-text_addr, addr);
    #endif
    SetRightJustifyOff = (void*)(addr);
    SetRightJustifyOn  = (void*)(addr - 0x1C); //FUN_00250ef8_
    SetCentreOff       = (void*)(addr - 0x2C); //FUN_00250ee8_
    SetCentreOn        = (void*)(addr - 0x48); //FUN_00250ecc_
    SetJustifyOff      = (void*)(addr - 0x5C); //FUN_00250eb8_
    SetJustifyOn       = (void*)(addr - 0x78); //FUN_00250e9c_
    return 1;
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
  if( _lw(addr - 0x4) == 0xA0A4001B && _lw(addr + 0x8) == 0x03E00008 && _lw(addr + 0xC) == 0xA080001B && _lw(addr + 0x10) == 0x90850000 && _lw(addr + 0x1C) == 0xA0C50028 ) { // 0x00250FD0
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetBackgroundOff", addr-text_addr, addr);
    #endif
    SetBackgroundOff = (void*)(addr);
    return 1;
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
  if( _lw(addr - 0x4) == 0xA080001C && _lw(addr + 0x4) == 0x34040001 && _lw(addr + 0xC) == 0x03E00008 && _lw(addr + 0x10) == 0xA0A4001D ) { // 0x00251030
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetPropOn", addr-text_addr, addr);
    #endif
    SetPropOn = (void*)(addr);
    SetPropOff = (void*)(addr+14);
    return 1;
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
  if( _lw(addr) == 0x00042C00 && _lw(addr + 0xc) == 0x34060002 && _lw(addr + 0x10) == 0x14A60005 && _lw(addr + 0x18) == 0xA4850038 ) { // 0x00251054
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetFontStyle", addr-text_addr, addr);
    #endif
    SetFontStyle = (void*)(addr);
    return 1;
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
  if( _lw(addr - 0x3C) == 0x34060002 && _lw(addr + 0x10) == 0x00802825 &&  _lw(addr + 0xB4) == 0x00000000 ) { // 0x0025109c
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetDropShadowPosition & SetDropColor", addr-text_addr, addr);
    #endif
    SetDropShadowPosition = (void*)(addr);
    SetDropColor = (void*)(addr + 0x10);
    return 1;
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
  if( _lw(addr - 0x6C) == 0x0211A02B && _lw(addr - 0xD8) == 0x0211A02B ) { // 0x00252AD4
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> PrintString_LCS", addr-text_addr, addr);
    #endif
    PrintString_LCS = (void*)(addr);
    return 1;
  }

  /*************************************
   * ULUS-10041 v1.02 | 0x001A1660 | OK!
   * ULUS-10041 v1.05 | 0x001F9434 | OK!
   * ULUS-10041 v3.00 | 0x001F9378 | OK!
   * ULES-00182 v1.00 | 0x001F9380 | OK!
   * ULES-00182 v2.00 | 0x001F92C4 | OK!
   * ULES-00151 v1.05 | 0x001F9434 | OK!
   * ULES-00151 v2.00 | 0x001F9434 | OK!
   * ULES-00151 v3.00 | 0x001F9378 | OK!
   * ULES-00151 v4.00 | 0x001F9378 | OK!
   * ULJM-05255 v1.01 | 0x001FEFA8 | OK!
   * ULET-00361 v0.02 | 0x001F9378 | OK!
   * ULET-00362 v0.01 | 0x001F92C4 | OK!
   * ULUX-80146 v0.02 | 0x002846C0 | OK!
   * ULUX-80146 v0.03 | 0x001FF904 | OK!
   **************************************/
  if( (_lw(addr - 0x20) == 0x3C05C974 &&_lw(addr) == 0xE48C0000 && _lw(addr + 0x14) == 0x00801025) &&  // 0x001F9378
      (_lw(addr + 0x4C) == 0x27A50010 || _lw(addr + 0x34) == 0x00003825) ) {  // rest || ULUS v1.02
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetBoxCoords", addr-text_addr, addr);
    #endif
    SetBoxCoords = (void*)(addr);
    return 1;
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
  if( _lw(addr+0x20) == 0x02004025 && _lw(addr + 0x8) == 0x30C900FF && _lw(addr + 0x64) == 0x3404000C /*0x3404000A for VCS*/ ) {  // 0x002CFDD0
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> DrawRect", addr-text_addr, addr);
    #endif
    DrawRect = (void*)(addr);
    return 1;
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
  if( _lw(addr+0x4) == 0x3C0443F0 && _lw(addr + 0x44) == 0x3C0443F0 && _lw(addr + 0x68) == 0x00000000 ) {
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetRightJustifyWrap", addr-text_addr, addr);
    logPrintf("blitn: 0x%08X (0x%08X) -> SetWrapx", addr+0x40-text_addr, addr+0x40);
    #endif
    SetRightJustifyWrap = (void*)(addr); // 0x00250F2C
    SetWrapx = (void*)(addr+0x40); // 0x00250f6c
    SetCentreSize = (void*)(addr+0x80); // FUN_00250fac_SetCentreSize
    SetBackgroundOn = (void*)(addr+0x90); // FUN_00250fbc_SetBackgroundOn
    SetBackgroundOff = (void*)(addr+0xA4); // FUN_00250fd0_SetBackgroundOff
    SetBackgroundColor = (void*)(addr+0xB4); // FUN_00250fe0_SetBackgroundColor
    SetBackGroundOnlyTextOn = (void*)(addr+0xE0); // FUN_0025100c_SetBackGroundOnlyTextOn
    SetBackGroundOnlyTextOff = (void*)(addr+0xF4); // FUN_00251020_SetBackGroundOnlyTextOff
    return 1;
  }

  return 0;
}


static int FindPatchVCS(u32 addr, u32 text_addr) {
  /*************************************
   * ULUS-10160 v1.03 | 0x001325BC | OK!
   * ULES-00502 v1.02 | 0x001324E8 | OK!
   * ULES-00503 v1.02 | 0x001324E8 | OK!
   * ULJM-05297 v1.01 | 0x001C99F8 | OK!
   * ULET-00417 v0.06 | 0x0012A07C | OK!
   **************************************/
  if( _lw(addr - 0xB0) == 0x34060001 && _lw(addr + 0x98) == 0x28A60000  && _lw(addr + 0xD0) == 0x00003825 ) {
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> mymenurender_VCS_patched", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, mymenurender_VCS_patched, mymenurender_VCS); // 0x01325bc
    return 1;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x0030805C | OK!
   * ULES-00502 v1.02 | 0x0030843C | OK!
   * ULES-00503 v1.02 | 0x003082B4 | OK!
   * ULJM-05297 v1.01 | 0x000FCB5C | OK!
   * ULET-00417 v0.06 | 0x002FF87C | OK!
   **************************************/
  if( _lw(addr - 0x4) == 0x27BD0010 && _lw(addr) == 0x27BDFFE0 && _lw(addr + 0x4) == 0xAFB00014 && _lw(addr + 0x8) == 0x341000FF ) {
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> ResetFontStyling", addr-text_addr, addr);
    #endif
    ResetFontStyling = (void*)(addr); // 0x0030805C
    return 1;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x00308138 | OK!
   * ULES-00502 v1.02 | 0x00308518 | OK!
   * ULES-00503 v1.02 | 0x00308390 | OK!
   * ULJM-05297 v1.01 | 0x000FCC40 | OK!
   * ULET-00417 v0.06 | 0x002FF958 | OK!
   **************************************/
  if( _lw(addr + 0x30) == 0x00409825 && _lw(addr + 0x54) == 0x00404825 ) {
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> PrintString_VCS", addr-text_addr, addr);
    #endif
    PrintString_VCS = (void*)(addr); // 0x00308138
    return 1;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x00308498 | OK!
   * ULES-00502 v1.02 | 0x00308878 | OK!
   * ULES-00503 v1.02 | 0x003086F0 | OK!
   * ULJM-05297 v1.01 | 0x000FCCD8 | OK!
   * ULET-00417 v0.06 | 0x002FFCB8 | OK!
   **************************************/
  if( _lw(addr) == 0x03E00008 && _lw(addr + 0x8) == 0x03E00008 && _lw(addr + 0x20) == 0x90840003 ) {
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetFontStyle, SetColor, SetScale_VCS, ...", addr-text_addr, addr);
    #endif
    SetFontStyle = (void*)(addr); // 0x308498
    SetColor = (void*)(addr+0x10); // 0x3084A8
    SetTextBounds = (void*)(addr+0x3C); // 0x3084d4
    SetScale_VCS = (void*)(addr+0x88); // 0x308520
    SetTextSpaceing = (void*)(addr+0x130); // 0x3085c8
    SetTextOriginPoint = (void*)(addr+0x16C); // 0x308604
    return 1;
  }
  if( _lw(addr+0x20) == 0x00052880 && _lw(addr + 0x68) == 0x00052880 && _lw(addr + 0x6C) == 0x00852021 ) { // JP only: 0x000FCE10
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetTextSpaceing, SetTextOriginPoint (JP only)", addr-text_addr, addr);
    #endif
     SetTextSpaceing = (void*)(addr); // sub_000FCE10
     SetTextOriginPoint = (void*)(addr+0x48); // sub_000FCE58
    return 1;
  }

  /*************************************
   * ULUS-10160 v1.01 | 0x0019895C
   * ULUS-10160 v1.02 |
   * ULUS-10160 v1.03 | 0x001CDB1C
   * ULES-00502 v1.02 | 0x001CDEB0
   * ULES-00503 v1.02 | 0x001CDD78
   * ULJM-05297 v1.01 | 0x001E2118
   * ULET-00417 v0.06 | 0x001C3E14
   * ULET-00417 v0.07 |
   **************************************/
  if( (_lw(addr) == 0xE48C0000 && _lw(addr + 0x14) == 0x00801025 && _lw(addr - 0x24 ) == 0x00801025) && // also LCS !!
    (_lw(addr + 0x34 ) == 0x00003825 || _lw(addr + 0x4C ) == 0x00402025) ) { // rest || JP
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> SetBoxCoords", addr-text_addr, addr);
    #endif
    SetBoxCoords = (void*)(addr);
    return 1;
  }

  /*************************************
   * ULUS-10160 v1.03 | 0x002F1130 | OK!
   * ULES-00502 v1.02 | 0x002F1510 | OK!
   * ULES-00503 v1.02 | 0x002F1388 | OK!
   * ULJM-05297 v1.01 | 0x0032FEB4 | OK!
   * ULET-00417 v0.06 | 0x002EBC20 | OK!
   **************************************/
  if( _lw(addr+0x20) == 0x02004025 && _lw(addr+0x8) == 0x30C900FF && _lw(addr+0x64) == 0x3404000A /* 0x3404000C for LCS*/ ) {
    #ifdef LOG
    logPrintf("blitn: 0x%08X (0x%08X) -> DrawRect", addr-text_addr, addr);
    #endif
    DrawRect = (void*)(addr);
    return 1;
  }

  return 0;
}


int initTextBlit(u32 text_addr, u32 text_size) {
  #ifdef LOG
  logPrintf("\nblitn: start..");
  #endif

  gta_version = -1;

  u32 i;
  for( i = 0; i < text_size; i += 4 ) {
    u32 addr = text_addr + i;
    if ((gta_version == -1 || gta_version == 0) && FindPatchVCS(addr, text_addr)) {
      gta_version = 0;
      continue;
    }
    if ((gta_version == -1 || gta_version == 1) && FindPatchLCS(addr, text_addr)) {
      gta_version = 1;
      continue;
    }
  }

  if( gta_version == -1 ) {
    #ifdef LOG
    logPrintf("blitn: ..error!\n");
    #endif
    return -1; // error
  }

  #ifdef LOG
  logPrintf("blitn: ..success! (%s)\n", gta_version ? "LCS" : "VCS");
  #endif
  return 0;
}
