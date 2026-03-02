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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <pspkerneltypes.h>
#include <pspiofilemgr.h>

#include "editor.h"

#define VERSION "v1.0h3" // displayed in title

/** FEATURE-FLAGS *****************************************************************************************************/

//#define LITE // 'Lite' version with less features for more memory on PSP-1000

// #define DEBUG // Debug mode  ("Debug" watermark, extra monitor, lang stuff, options etc)
// #define PREVIEW // Preview mode ("Preview" watermark, WIP cheats & custom spawn-teleport etc)

// #define LOG // logging to logfile
// #define MEMLOG // logging bad memory access to logfile
// #define PATCHLOG // debug logging for detected patch locations only
// #define USERSCRIPTLOG // debug logging for userscripts only

#define MEMCHECK // memory bounds check (faster memory operations if disabled but crash on out-of-bounds access)

#ifndef LITE

 #define NAMERESOLV // (~10KB) name resolver system for translating hashes (also remove minIni from makefile if off)

 #define FREECAM // (~22KB)
 #define EDITORS // (~105KB)
 #define USERSCRIPTS // (~19KB)
 #define HEXEDITOR // (~30KB)
 #define CONFIG // (~8KB)
 #define HEXMARKERS // (~4KB)
 #define LANG // (~6KB)
 
#endif

//#define ACHIEVEMENTS // (~4KB) discontinued
//#define SAVEDITOR // (?KB) removed

//#define MEMORY  // display memory usage on screen (LCS US v3.00 only)
//#define GAMELOG // display developer logs on screen + to file (LCS US v3.00 only)

#define SWIM
//#define SPECIAL_VEHICLES // todo -> LCS: spawn "HELI" (0xC6)  VCS: Plane crashes because of something else

/**********************************************************************************************************************/

extern int ADRENALINE;
extern int PPSSPP;
extern int LCS;
extern int VCS;

enum {
  OFF, // 0
  ON   // 1
};

typedef struct {
  const char *path;
  const short cat;
  const char type;
  const char LC; // for Liberty City Stories
  const char VC; // for Vice City Stories
  const char SP; // for Singleplayer
  const char MP; // for Multiplayer
  const short conf_id; // previously: char *configname;    0x1xxx = Cheat, 0x2xxx = Category, 0x3xxx = Setting, 0x4xxx = Editor
  const char def_stat;
  void *value;
  const char *msg1;
  const char *msg2;
  const char *desc;
} Menu_pack;


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

void draw();
void stopMenu();
void closeMenu();

void buttonInput();
void applyCheats();
void applyOnce();
void checkCheats();

int menu_draw(const Menu_pack *menu_list, int menu_max);
int menu_ctrl(const Menu_pack *menu_list, int menu_max);
int menu_setDefaults(const Menu_pack *menu_list, int menu_max /*, char *config*/ );
int menu_apply(const Menu_pack *menu_list, int menu_max);
int menu_check(const Menu_pack *menu_list, int menu_max);

void *category_toggle(int type, int cat, int set);

int module_stop(SceSize argc, void* argp);
int module_start(SceSize argc, void* argp);

extern const Menu_pack main_menu[];
extern const int menu_size;

/////////////////////////////////////////////

int editor_create(int mode, int toptions, const char *editortitle, const Editor_pack *editor_menu, int first_obj, int block_size, int blocks);

typedef struct userscript_entry
{
  char* path; // Allocated when needed
  int attr;
  int files_folders_count;
} userscript_entry;

int usercheats_create();
int usercheats_draw();
int usercheats_ctrl();

#ifdef USERSCRIPTS
int free_userscripts_array();
int userscripts_create();
#endif

/*
#ifdef SAVEDITOR
int saveselector_create();
int saveselector_draw();
int saveselector_ctrl();

int saveditor_create();
int saveditor_draw();
int saveditor_ctrl();
#endif
*/

#ifdef FREECAM
int freecam_create();
#endif

#ifdef HEXEDITOR
int address_create();
int hexeditor_create(int hexadr, int hexmode, int lowbound, int highbound, const char *infostring);

#ifdef HEXMARKERS
void hex_marker_addx(u32 address, int size);
void hex_marker_clear();
#endif

#endif

void free_alloc_mem_cdr();

extern int LCS;

#endif
