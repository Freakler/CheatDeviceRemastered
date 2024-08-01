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

#ifndef __MAIN_H__
#define __MAIN_H__


/** FEATURE-FLAGS *****************************************************************************************************/

//#define DEBUG // Debug mode  ("Debug" watermark, extra monitor, lang stuff, options etc)
//#define PREVIEW // Preview mode ("Preview" watermark, WIP cheats & custom spawn-teleport etc)

//#define LOG // logging to logfile
//#define MEMLOG // logging bad memory access to logfile
//#define PATCHLOG // debug logging for detected patch locations only
//#define USERSCRIPTLOG // debug logging for userscripts only

#define MEMCHECK // memory bounds check (faster memory operations if disabled but crash on out-of-bounds access)

#define NAMERESOLV // (~10KB) name resolver system for translating hashes (also remove minIni from makefile if off)

#define FREECAM // (~22KB)
#define EDITORS // (~105KB)
#define USERSCRIPTS // (~19KB)
#define HEXEDITOR // (~30KB)
#define CONFIG // (~8KB)
#define HEXMARKERS // (~4KB)
#define LANG

//#define ACHIEVEMENTS // (~4KB) discontinued
//#define SAVEDITOR // (?KB) removed

//#define MEMORY  // display memory usage on screen (LCS US v3.00 only)
//#define GAMELOG // display developer logs on screen + to file (LCS US v3.00 only)


/**********************************************************************************************************************/

enum {
  OFF, // 0
  ON   // 1
};

typedef struct {
  char *path;
  short cat;
  char type;
  char LC; // for Liberty City Stories
  char VC; // for Vice City Stories
  char SP; // for Singleplayer
  char MP; // for Multiplayer
  short conf_id; // previously: char *configname;    0x1xxx = Cheat, 0x2xxx = Category, 0x3xxx = Setting, 0x4xxx = Editor
  char def_stat;
  void *value;
  char *msg1;
  char *msg2;
  char *desc;
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

void setDefaultColors();
void saveCurrentColorsToConfig();

int menu_draw(const Menu_pack *menu_list, int menu_max);
int menu_ctrl(const Menu_pack *menu_list, int menu_max);
int menu_setDefaults(const Menu_pack *menu_list, int menu_max /*, char *config*/ );
int menu_apply(const Menu_pack *menu_list, int menu_max);
int menu_check(const Menu_pack *menu_list, int menu_max);

int menu_toggleCheat(void *value, int bool, int param );

void *category_toggle(int type, int cat, int set);

int module_stop(int argc, char *argv[]);
int module_start(SceSize argc, void* argp);

extern const Menu_pack main_menu[];
extern int menu_size;

/////////////////////////////////////////////

int editor_create();
int editor_draw();
int editor_ctrl();

int usercheats_create();
int usercheats_draw();
int usercheats_ctrl();

#ifdef USERSCRIPTS
int userscripts_create();
int userscripts_draw();
int userscripts_ctrl();
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
int freecam_draw();
int freecam_ctrl();
#endif

#ifdef HEXEDITOR  
void add_adr_to_history(int address);
int address_create();
int address_draw();
int address_ctrl();

int editbyte_create();
int editbyte_draw();
int editbyte_ctrl();

int hexeditor_create(int hexadr, int hexmode, int lowbound, int highbound, const char *infostring);
int hexeditor_draw();
int hexeditor_ctrl();
#endif

#ifdef HEXMARKERS
void hex_marker_add(u32 address);
int hex_marker_check(u32 address);
void hex_marker_addx(u32 address, int size);
void hex_marker_remove(u32 address);
void hex_marker_clear();
#endif

extern int LCS;

#endif
