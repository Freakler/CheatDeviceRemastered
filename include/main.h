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

#ifndef __MAIN_H__
#define __MAIN_H__

#include "mctypes.h"
#include <pspiofilemgr.h>

#include "editor.h"

#define VERSION "v1.0h4" // displayed in title

/** FEATURE-FLAGS *****************************************************************************************************/

// #define DEBUG    // Debug mode  ("Debug" watermark, extra monitor, lang stuff, options etc)
// #define PREVIEW  // Preview mode ("Preview" watermark, WIP cheats & custom spawn-teleport etc)

#define LITE      OFF // 'Lite' version with less features for more memory on PSP-1000
#define LOGGING   ON  // General Logging (DEBUG, INFO, WARNING, ERROR)
#define MEMCHECK  ON  // memory bounds check (faster memory operations if disabled but crash on out-of-bounds access)

#if !LITE
 #define NAMERESOLV   // ( ~10  KB ) name resolver system for translating hashes
 #define FREECAM      // ( ~22  KB )
 #define EDITORS      // ( ~105 KB )
 #define USERSCRIPTS  // ( ~19  KB )
 #define HEXEDITOR    // ( ~30  KB )
 #define CONFIG       // ( ~8   KB )
 #define LANG         // ( ~6   KB )
//  #define SWIM
//  #define SPECIAL_VEHICLES // TODO: LCS: spawn "HELI" (0xC6)  VCS: Plane crashes because of something else
//  #define SAVEDITOR        // (  ? KB ) removed
//  #define MEMORY           // display memory usage on screen (LCS US v3.00 only)

#else
 #undef LOGGING
 #undef DEBUG
 #undef PREVIEW
#endif

#if LOGGING
 #define PATCH_LOGGING       ON  // debug logging for detected patch locations only
 #define MEMORY_LOGGING      ON  // logging bad memory access to console
 #define USERSCRIPT_LOGGING  OFF  // debug logging for userscripts only
 #define GAME_LOGGING        OFF  // display developer logs on screen + to file (LCS US v3.00 only)
 #define CONFIG_LOGGING      OFF
#endif

/**********************************************************************************************************************/

extern bool ADRENALINE, PPSSPP, LCS, VCS;

typedef struct {
  const u8string path;
  const uint8 cat;
  const uint8 type;
  const bool LC; // for Liberty City Stories
  const bool VC; // for Vice City Stories
  const bool SP; // for Singleplayer
  const bool MP; // for Multiplayer
  const uint16 conf_id; // previously: char *configname;    0x1xxx = Cheat, 0x2xxx = Category, 0x3xxx = Setting, 0x4xxx = Editor
  const int8 def_stat;
  void *value;
  const u8string msg1;
  const u8string msg2;
  const u8string desc;
} CDRMenuItem;


enum {
  FUNC_GET_STATUS,
  FUNC_GET_STRING,
  FUNC_APPLY,
  FUNC_CHECK,
  FUNC_GET_VALUE,
  FUNC_CHANGE_VALUE,
  FUNC_TOGGLE_CHEAT,
  FUNC_SET
};

enum {
  TYPE_DUMMY,
  TYPE_BIT,
  TYPE_NIBBLE_LOW,
  TYPE_NIBBLE_HIGH,
  TYPE_BYTE,
  TYPE_SHORT,
  TYPE_INTEGER,
  TYPE_FLOAT,
  TYPE_BOOL,
  TYPE_STRING,
  TYPE_BYTE_AS_FLT
};

enum {
  MENU_DUMMY,
  MENU_CATEGORY,
  MENU_FUNCTION,  // simple function call - no check calls no returns
  MENU_VALUE,     // list with options (left/right)
  MENU_SWITCH,    // simple ON/OFF switch
  MENU_VALSWITCH, // list with options (left/right) with ON/OFF switch

  MENU_CDR_FREECAM,
  MENU_CDR_USER,
  MENU_CDR_USCM,
  MENU_CDR_HEX,
  MENU_CDR_EDITOR,
  MENU_CDR_FILES,

  MENU_CONFIG     // special args
};

enum {
  /// special
  CAT_DEBUG,
  CAT_WIP,
  CAT_MAIN,
  CAT_MULTI,
  CAT_ALIAS,
  CAT_DUMMY,
  CAT_CHDEV,
  CAT_CATGRY,
  CAT_GAME,

  /// actual categories
  CAT_PLAYER,
  CAT_VEHICL,
  CAT_TELEP,
  CAT_SAVE,
  CAT_WORLD,
  CAT_CHEATS,
  CAT_CAMERA,
  CAT_MISSN,
  CAT_STCKCHT,
  CAT_MISSON,
  CAT_MAP,
  CAT_MISC,
  CAT_COLOR,
  CAT_TRFFIC,
  CAT_PEDS,

  CAT_COUNTER // my little counter (must be last)
};

extern void draw();
extern void stopMenu();
extern void closeMenu();

extern void buttonInput();
extern void applyCheats();
extern void applyOnce();
extern void checkCheats();

extern void menu_draw(const CDRMenuItem *menu_list, usize menu_max);
extern void menu_ctrl(const CDRMenuItem *menu_list, usize menu_max);
extern void menu_apply(const CDRMenuItem *menu_list, usize menu_max);
extern void menu_check(const CDRMenuItem *menu_list, usize menu_max);

extern void *category_toggle(int type, int cat, int set);

extern const CDRMenuItem main_menu[];
extern const array_size menu_size;

/////////////////////////////////////////////

extern void editor_create(uint8 mode, array_size toptions, const u8string editortitle, const Editor_pack *editor_menu, uaddr first_obj, usize block_size, usize blocks);

#ifdef PREVIEW
extern void usercheats_create();
extern void usercheats_draw();
extern void usercheats_ctrl();
#endif

#ifdef USERSCRIPTS

typedef struct userscript_entry
{
  u8string path; // Allocated when needed
  uint32 attr;
  usize files_folders_count;
} userscript_entry;

extern func_stat free_userscripts_array();
extern void userscripts_create();
#endif

#ifdef SAVEDITOR
extern void saveselector_create();
extern void saveselector_draw();
extern void saveselector_ctrl();

extern void saveditor_create();
extern void saveditor_draw();
extern void saveditor_ctrl();
#endif

#ifdef FREECAM
extern void freecam_create();
#endif

#ifdef HEXEDITOR
extern void address_create();
extern void hexeditor_create(uaddr hexadr, int hexmode, uaddr lowbound, uaddr highbound, const u8string infostring);
extern void hex_marker_addx(uaddr address, usize size);
extern void hex_marker_clear();

#endif

extern void free_alloc_mem_cdr();

extern bool LCS;

#endif
