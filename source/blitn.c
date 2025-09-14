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
   
void AsciiToUnicode(const char* in, wchar_t* out) {
  while( *in != '\0' ) {
    *out++ = *in++;
  } *out = '\0';
}

void drawBox(float x, float y, float width, float height, u32 color) {
  int i;
  for( i = 0; i < MAX_BOXES; i++ ) {
    if( bool_box[i] == (x+y+width+height) ) break;
    if( bool_box[i] == 0 ) {
      bool_box[i] = x+y+width+height;
      xbox[i] = x;
      ybox[i] = y;
      wbox[i] = width;
      hbox[i] = height;
      boxcol[i] = color;
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

void drawString(char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color) {
  int i;
  for( i = 0; i < MAX_TEXTS; i++ ) {
    if( wideBuffer[i][0] == 0 ) {
      AsciiToUnicode(string, wideBuffer[i]);
      xpos[i] = x;
      ypos[i] = y;
      fontstyle[i] = style;
      fontshadow[i] = shadow;
      fontscale[i] = scale;
      fontcolor[i] = color;
      textorigin[i] = origin;
      break;
    }
  }
  
}

void drawStringLCS(char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color) {
  wchar_t str[MSGLENGTH] = L" ";
  AsciiToUnicode(string, str);
  
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

void drawStringVCS(char *string, int origin, short style, float scale, short shadow, float x, float y, u32 color) {
  float coords[4];
  
  wchar_t str[MSGLENGTH] = L" ";
  AsciiToUnicode(string, str);

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
void mymenurender_LCS_patched() {
  int i;
  float coordinates[4];
  float *ptr_coords;
  
  /// BOXes
  for( i = 0; i < MAX_BOXES; i++ ) {
    if( bool_box[i] != 0 ) {  
      ptr_coords = (float *)SetBoxCoords(xbox[i], ybox[i], xbox[i]+wbox[i], ybox[i]+hbox[i], coordinates);
      DrawRect(ptr_coords, &boxcol[i], 0x1);
    } else break;
  } memset(bool_box, 0, sizeof(bool_box));
  
  
  /// TEXTs
  for( i = 0; i < MAX_TEXTS; i++ ) {
    if( wideBuffer[i][0] != '\0' ) {
    SetFontStyle(fontstyle[i]);  
    SetScale_LCS(fontscale[i]/2, fontscale[i]);
    SetColor(&fontcolor[i]);
    // SetSlant(0.0f);    
    SetDropShadowPosition(fontshadow[i]);
    if( fontshadow[i] == 2 ) { // (fix) if shadow is enabled.. keep it from fading when for example vehicle/location text fades aways
      char rgba[] = { 0x00, 0x00, 0x00, 0xFF }; // todo allow rgb values (although LCS only! -> ?)
      SetDropColor(rgba);
    }
    
    /// origin: 0 = free positioning , 2 = center, 3 = left, 4 = right (uses X coordinate unlike VCS!)
    if( textorigin[i] == 2 ) { // CENTER
      SetRightJustifyOff();
      SetJustifyOff();  
      SetCentreOn(); 
      SetCentreSize(SCREEN_WIDTH); // todo I guess
    }
    
    if( textorigin[i] == 0 || textorigin[i] == 3 ) { // FREE or LEFT
      SetCentreOff();
      SetRightJustifyOff();
      SetJustifyOn();
      SetWrapx(SCREEN_WIDTH);
    }    
    
    if( textorigin[i] == 4 ) { // RIGHT
      SetCentreOff();
      SetJustifyOff();
      SetRightJustifyOn();
      SetRightJustifyWrap(0.0f);
    }
    
    /*if( textorigin[i] == 6 ) { // ALIGN_SCREENCENTER (did this to match VCS) TODO
      SetRightJustifyOff();
      SetJustifyOff();  
      SetCentreOn(); 
      SetCentreSize(SCREEN_WIDTH);
      xpos[i] = SCREEN_WIDTH / 2.0f; // this crashes LCS Pause Menu
    }*/
    
    SetPropOn(); // proportional
    SetBackgroundOff();  
    SetBackGroundOnlyTextOff();  

    PrintString_LCS(xpos[i], ypos[i], wideBuffer[i], NULL);
    
    } else break;
  } memset(wideBuffer, '\0', sizeof(wideBuffer[0][0])*MAX_TEXTS*MSGLENGTH); // clear my draw array because it has all been drawn
    
  mymenurender_LCS();
}

void (*mymenurender_VCS)(void);
void mymenurender_VCS_patched() { 
  int i;
  float coordinates[4];
  float *ptr_coords;
  float scalefix = 0.0f;
  float posfix = 2.0f; // a fix to better match position compared to LCS version
  float coords[4];

  /// BOXes
  for( i = 0; i < MAX_BOXES; i++ ) {
    if( bool_box[i] != 0 ) {  
      ptr_coords = (float *)SetBoxCoords(xbox[i], ybox[i], xbox[i]+wbox[i], ybox[i]+hbox[i], coordinates);
      DrawRect(ptr_coords, &boxcol[i], 0x1); // todo
    } else break;
  } memset(bool_box, 0, sizeof(bool_box));

  
  /// TEXTs
  for( i = 0; i < MAX_TEXTS; i++ ) {
    if( wideBuffer[i][0] != '\0' ) {
      SetFontStyle(fontstyle[i]);
    ResetFontStyling();
    coords[0] = 0.0f;
    coords[1] = SCREEN_HEIGHT;
    coords[2] = SCREEN_WIDTH;
    coords[3] = 0.0f;
    SetTextBounds(coords); // full screen 
    
    if( textorigin[i] == 4 ) { // right justified via coordinate  [CUSTOM]
      coords[2] = xpos[i];
      SetTextBounds(coords); // in combination with "SetTextOriginPoint" text can be right justified! I did this to match LCS
    }  
    
    if( textorigin[i] == 2 ) { // centered via coordinate [CUSTOM]
      coords[2] = xpos[i] * 2.0f;
      SetTextBounds(coords); // in combination with "SetTextOriginPoint" text can be centered! I did this to match LCS
    }  

    SetTextSpaceing(1); // set 0 to put whitespace between each char
    SetTextOriginPoint(textorigin[i]); 
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
    SetScale_VCS(scalefix+fontscale[i]); // SetScale_VCS(fontscale[i]); //WHY CRASH!?
      
    if( fontshadow[i] == 2 && fontstyle[i] != FONT_NAMES ) { // fake shadow (black shifted text) + disable on FONT_NAMES because those have shadow by default
      u32 rgba[] = { 0xBB000000 };
      SetColor(rgba);
      PrintString_VCS(wideBuffer[i], (int)(xpos[i]+1.0f), (int)(ypos[i]+1.0f+posfix));
    }
    
    SetColor(&fontcolor[i]);
    PrintString_VCS(wideBuffer[i], (int)xpos[i], (int)(ypos[i] + posfix));
    
    } else break;
  } memset(wideBuffer, '\0', sizeof(wideBuffer[0][0])*MAX_TEXTS*MSGLENGTH); // clear my draw array because it has all been drawn

  mymenurender_VCS();
}


int FindPatchLCS(u32 addr, u32 text_addr) {
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
    SetSlantRefPoint = (void*)(addr+0x14); // FUN_00250e1c_SetSlantRefPoint
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


int FindPatchVCS(u32 addr, u32 text_addr) {
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
  
  int gta_version = -1;
  
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