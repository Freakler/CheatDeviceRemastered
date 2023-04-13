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

#ifndef __BLITN_H__
#define __BLITN_H__


#define SCREEN_WIDTH 480.0f
#define SCREEN_HEIGHT 272.0f

/// my ugly text storeing thingy 
#define MAX_TEXTS 512  // number of texts that can be displayed at once
#define MSGLENGTH 128  // length messages can have
wchar_t wideBuffer[MAX_TEXTS][MSGLENGTH];
u32 fontcolor[MAX_TEXTS];
short fontstyle[MAX_TEXTS];
short fontshadow[MAX_TEXTS];
int textorigin[MAX_TEXTS];
float fontscale[MAX_TEXTS];
float xpos[MAX_TEXTS];
float ypos[MAX_TEXTS];

/// and boxes too
#define MAX_BOXES 128
short bool[MAX_BOXES];
float xbox[MAX_BOXES];
float ybox[MAX_BOXES];
float wbox[MAX_BOXES];
float hbox[MAX_BOXES];
u32 boxcol[MAX_BOXES];


#define FONT_NAMES 0   // used for LOCATION NAME, VEHICLE NAME, RADIO-STATION NAME, MENU TABs (italic on VCS)
#define FONT_DIALOG 1  // used for DIALOGS, in MESSAGE BOX etc
#define FONT_GTA 2     // used for "MISSION SUCCESS!", "WASTED!" etc and mainmenu categories

#define SHADOW_ON 2    // only works for LCS
#define SHADOW_OFF 0

#define ALIGN_FREE 0
#define ALIGN_CENTER 2
#define ALIGN_LEFT 3
#define ALIGN_RIGHT 4
#define ALIGN_SCREENCENTER 6

#define SIZE_LCS_SMALL 0.6f
#define SIZE_LCS_NORMAL 0.8f
#define SIZE_LCS_BIG 1.0f
#define SIZE_LCS_HUGE 1.2f

#define SIZE_VCS_SMALL 0.46f //0.5f
#define SIZE_VCS_NORMAL 0.6f
#define SIZE_VCS_BIG 0.8f 
#define SIZE_VCS_HUGE 1.0f 


#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a);

#define HIJACK_FUNCTION(a, f, ptr) { \
  u32 _func_ = a; \
  static u32 patch_buffer[3]; \
  _sw(_lw(_func_), (u32)patch_buffer); \
  _sw(_lw(_func_ + 4), (u32)patch_buffer + 8);\
  MAKE_JUMP((u32)patch_buffer + 4, _func_ + 8); \
  _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + 4); \
  ptr = (void *)patch_buffer; \
}


/// box
char *(*SetBoxColor)(char *param_1,char alpha,char red,char green,char blue);
float *(*SetBoxCoords)(float param_1, float param_2, float param_3, float param_4, float *param_5);
void (*DrawRect)(float *param_1,uint *param_2,char param_3);

      
/// string
void (*PrintString_LCS)(float X, float Y, wchar_t* text, int* unknown);
void (*PrintString_VCS)(wchar_t* text, int x, int y);
void (*SetRightJustifyOn)();
void (*SetRightJustifyOff)();
void (*SetCentreOff)();
void (*SetCentreOn)();
void (*SetJustifyOn)();
void (*SetJustifyOff)();
void (*SetColor)(u32* color);
void (*SetBackgroundColor)(u32* color);
void (*SetFontStyle)(short style);
void (*SetDropShadowPosition)(short bool);
void (*SetDropColor)(char* rgba);
void (*SetScale_LCS)(float width, float height);
void (*SetScale_VCS)(float scale);
void (*SetBackGroundOnlyTextOn)();
void (*SetBackGroundOnlyTextOff)();
void (*SetPropOn)();
void (*SetPropOff)();
void (*SetBackgroundOn)();
void (*SetBackgroundOff)();
void (*SetTextOriginPoint)(int origin);
void (*SetTextSpaceing)(int option);
void (*SetTextBounds)(float* array);
void (*ResetFontStyling)();
void (*SetRightJustifyWrap)(float x);
void (*SetWrapx)(float x);
void (*SetSlant)(float param_1);
void (*SetCentreSize)(float param_1);
void (*SetSlantRefPoint)(float param_1, float param_2);


/// colors (0xAABBGGRR)
#define RED     0xFF0000FF
#define GREEN   0xFF00FF00
#define BLUE    0xFFFF0000
#define CYAN    0xFFffff00
#define MAGENTA 0xFFff00ff
#define YELLOW  0xFF00ffff
#define AZURE   0xFFFF7F00
#define VIOLET  0xFFFF007F
#define ORANGE  0xFF007FFF
#define GREY    0xFF808080
#define LGREY   0xFFA0A0A0
#define WHITE   0xFFFFFFFF
#define BLACK   0xFF000000
#define CLEAR   0x00000000

#define ALPHABLACK  0x88000000 // used for help-box, map legend by game
#define ALPHAWHITE  0x99FFFFFF // 

#define LIGHTBLUE  0xFFFF9900

#define ORANGERED  0xFF0055FF
#define DARKGREEN  0xFF1BA000

/// original colors used in the ogs
#define CHDVC_YELLOW  0xFF66ffff
#define CHDVC_ORANGE  0xFF05AFFF
#define CHDVC_MAGENTA 0xFF5C00A2
#define CHDVC_AZURE   0xFFAEE223
#define CHDVC_GREEN   0xFF00FF00

#define LCS_HEALTH   0xFF00008E
#define VCS_HEALTH   0xFF8056BB // not 100% original
#define LCS_ARMOR    0xFFBE782A
#define VCS_ARMOR    0xFFCFD714 // not 100% original 


void AsciiToUnicode(const char* in, wchar_t* out);

void drawBox(float x, float y, float width, float height, u32 color);
void drawUiBox(float x, float y, float width, float height, float bordersize, u32 bordercolor, u32 bgcolor);

void drawString(char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color);
void drawStringLCS(char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color);
void drawStringVCS(char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color);

void cWorldStream_Render_LCS_Patched(void *this, int mode);
void cWorldStream_Render_VCS_Patched(void *this, int mode);

int FindPatchLCS(u32 addr, u32 text_addr);
int FindPatchVCS(u32 addr, u32 text_addr);

int initTextBlit(u32 text_addr, u32 text_size);

#endif
