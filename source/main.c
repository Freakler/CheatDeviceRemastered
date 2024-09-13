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

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <systemctrl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <pspsysmem.h>

#include "main.h"
#include "utils.h"
#include "cheats.h"
#include "blitn.h"
#include "editor.h"
#include "config.h"
#include "functions.h"
#include "lang.h"

#ifdef NAMERESOLV
  #include "minIni.h"
#endif

PSP_MODULE_INFO("CheatDeviceRemastered", 0, 1, 0); // user


/// settings
int menuopendelay = 2000;  // wait ~2 seconds before showing menu & applying after spawning
short showoptions = 12;    // the number of options / lines that should be displayed at once
float row_spacing = 15.0f; // pixels between rows
u32 open_key      = PSP_CTRL_LTRIGGER | PSP_CTRL_UP;   // eat buttons mode
u32 open_key_alt  = PSP_CTRL_LTRIGGER | PSP_CTRL_DOWN; // don't eat mode
char welcomemsg[] = "~y~CheatDevice Remastered ~w~is now ready! Press ~h~L + UP ~w~to open the menu. Happy cheating!"; // LCS ~y~ = yellow | VCS ~y~ = violet

const char *basefolder  = "ms0:/PSP/PLUGINS/cheatdevice_remastered/";

char file_log[] = "log.txt";

#ifdef CONFIG
char file_config[] = "cheatdevice_remastered.cfg";
#endif

#ifdef NAMERESOLV
char file_names_lcs[] = "cheatdevice_names_lcs.ini";
char file_names_vcs[] = "cheatdevice_names_vcs.ini";
#endif

#ifdef USERSCRIPTS
char folder_scripts[] = "SCRIPTS/"; // inside basefolder
#endif

#ifdef PREVIEW
char folder_cheats[] = "CHEATS/"; // inside basefolder
char folder_textures[] = "TEXTURES/"; // inside basefolder
#endif

#ifdef LANG
char folder_translations[] = "TRANSLATIONS/"; // inside basefolder
#endif


/// flags (shouldn't be set here)
short flag_menu_start     = 0; // menu will be started automatically like the original (2 for don't-eat-keys mode)
short flag_menu_show      = 0; // menu is allowed to be openend / shown boolean (don't set here)
short flag_menu_running   = 0; // menu is currently running boolean (don't set here)
short flag_keys_disable   = 0; // keys should be menu exlusive (set by button-open-combo, don't set here)
short flag_coll_cats      = 0; // makes categories become collapseable
short flag_ui_blocking    = 0; // block UI elements like map & hud for a cleaner look when menu is displayed
short flag_use_legend     = 0; // display the legend box on the bottom of the screen 
short flag_use_cataltfont = 0; // makes categories use a different font to make them stick out
short flag_use_liveconfig = 0; // config will be written to everytime the menu is closed (#ifdef CONFIG)
short flag_draw_FPS       = 0; // draw FPS counter bool
short flag_draw_MEM       = 0; // draw memory usage (main free)
short flag_draw_DEBUG     = 0; // draw Debug monitor bool
short flag_draw_DBGVALS   = 0; // draw Debug values bool
short flag_draw_COORDS    = 0; // draw player coordinates bool
short flag_draw_SPEEDO    = 0; // draw speedometer and gear monitor bool
short flag_draw_welcomsg  = 0; // lets the welcome message appear after spawning the first time
short flag_customusic     = 0; // number of custom tracks found for custom music cheat
short flag_hudwashidden   = 0; // keeps track of previously hidden HUD (for re-enabling if necessary)
short flag_mapwashidden   = 0; // keeps track of previously hidden MAP (for re-enabling if necessary)
short flag_swapxr         = 0; // swap acceleration in special cheats (to match gta_remastered's X and R swap)


/// color definitions 
u32 COLOR_TEMP;
u32 COLOR_TEXT         = WHITE;
u32 COLOR_CURSOR       = CHDVC_MAGENTA - 0x77000000; // add alpha
u32 COLOR_TITLE        = WHITE;
u32 COLOR_CATEGORY     = WHITE;
u32 COLOR_DEBUG        = RED; 
u32 COLOR_VALUE        = CHDVC_GREEN;
u32 COLOR_CHEAT_ON     = CHDVC_YELLOW;
u32 COLOR_CHEAT_OFF    = 0; // set later because different for LCS/VCS
u32 COLOR_BACKGROUND   = ALPHABLACK;
u32 COLOR_SCROLLBAR    = CHDVC_MAGENTA - 0x33000000; // add alpha
u32 COLOR_FREECAM      = ORANGERED;
u32 COLOR_USERCHEATS   = DARKGREEN;
u32 COLOR_SAVEDITOR    = VIOLET;
u32 COLOR_HEX          = CHDVC_AZURE;
u32 COLOR_HEX_MARKER   = CHDVC_MAGENTA - 0x77000000; // add alpha
u32 COLOR_EDITOR       = GREEN;
u32 COLOR_FILES        = LIGHTBLUE;
u32 COLOR_UIBORDER     = BLACK;
u32 COLOR_UIBACKGROUND = ALPHABLACK;


/// textscales (initial values)
float SIZE_SMALL  = 1.00f;
float SIZE_NORMAL = 1.00f;
float SIZE_BIG    = 1.00f;
float SIZE_HUGE   = 1.00f;
float SIZE_HEXEDIT= 1.00f;


/// more globals
u32 mod_text_addr;
u32 mod_text_size;
u32 mod_data_size;
register int gp asm("gp");
static STMOD_HANDLER previous;
int ADRENALINE = 0, PPSSPP = 0, LCS = 0, VCS = 0;
char buffer[256];
#ifdef CONFIG
char config[128];
#endif
char category_index[CAT_COUNTER]; // 1 = visible/expanded/enabled (init by module_start & taken care of by menu_setDefaults)
SceCtrlData pad;
int hold_n = 0;
u32 old_buttons = 0, current_buttons = 0, pressed_buttons = 0, hold_buttons = 0, lx, ly, rx, ry;
float xstick, ystick, xstick_, ystick_;
int hex_adr = -1; // mem address for HexEditor


/// some externs 
extern u32 memory_low;
extern u32 memory_high;
extern int pplayer;
extern int pcar;        
extern int pobj;
extern int multiplayer;
extern char speed[];
extern char gear[];
extern char *lcs_garagenames[];          
extern char *vcs_garagenames[];
extern int pcar_id; 
extern float fps;
extern int memory_main_free;
extern const char *weather_lcs[];
extern const char *weather_vcs[];


/// main menu //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Menu_pack main_menu[] = {
  
  /// char *path                        cat           type               LC      VC      SP      MP     conf_id  def_stat  void *value        char *msg1                          char *msg2                              char *desc
  
  // // DEBUG // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
  
  #ifdef DEBUG
  {"Debug Monitor:"                   , CAT_DEBUG   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1F00 , OFF , debug_monitor        , ""                                 , ""                                   , "" },
  {"Display vars"                     , CAT_DEBUG   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1F01 , ON  , debug_vars           , ""                                 , ""                                   , "" },
  #ifdef LANG
  {"Update Language Strings"          , CAT_DEBUG   , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , update_lang          , "CROSS: Reset"                     , ""                                   , "Update Menu Language" },  // arg / index though?!
  #endif
  {"- - - - - - - - - - - - - - - - -", CAT_DEBUG   , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  {""                                 , CAT_DEBUG   , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  #endif
  
  
  // // WORK IN PROGRESS // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
  
  #ifdef ACHIEVEMENTS
  {"Reset Achievements"               , CAT_WIP     , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , TRUE  , 0x164B , -1  , achievements_reset   , ""                                 , ""                                   , "" },
  {""                                 , CAT_WIP     , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  #endif
  
  #ifdef PREVIEW
  {"Wave height:"                     , CAT_WIP     , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1A4A , OFF , world_waveheight     , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust height"          , "Adjust the oceans wave height."},
  {"Police Heli:"                     , CAT_WIP     , MENU_VALUE       , TRUE  , TRUE  , TRUE  , TRUE  , 0x1F63 , OFF , policechaseheli      , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust"                 , "" },
  {""                                 , CAT_WIP     , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  {"test_func()"                      , CAT_WIP     , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , test_func            , "CROSS: Execute"                   , ""                                   , "" },  
  {"test_switch()"                    , CAT_WIP     , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , test_switch          , "CROSS: Enable/Disable Cheat"      , ""                                   , "" },
  {""                                 , CAT_WIP     , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  {"Hex Editor"                       , CAT_ALIAS   , MENU_CDR_HEX     , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , hexeditor            , "CROSS: Open HexEditor"            , ""                                   , "" },
  {"User Cheats"                      , CAT_WIP     , MENU_CDR_USER    , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , user_cheats          , "CROSS: "                          , "TRIANGLE: "                         , "" },
  {"User Scripts"                     , CAT_WIP     , MENU_CDR_USCM    , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , user_scripts         , "CROSS: "                          , "TRIANGLE: "                         , "" },
  {"Start Mission: "                  , CAT_WIP     , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x1EF4 , -1  , mission_select       , "CROSS: Start Mission"             , ""                                   , "" },
  {"- - - - - - - - - - - - - - - - -", CAT_WIP     , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  {""                                 , CAT_WIP     , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  #endif
 
  // // ALIAS // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
  {"Infinite Health & Armor"          , CAT_ALIAS   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , godmode              , "CROSS: Enable/Disable Cheat"      , ""                                   , "You are immune to explosions, gunshots, fall damage, fire etc.." },
  {"Wanted Level:"                    , CAT_ALIAS   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0      , -1  , wanted_level         , "CROSS: Enable/Disable Cheat"      , "SQUARE: Set current as max"         , "Adjust your Wanted Level, lock it and set the Maximum Level possible." },
  {"Time:"                            , CAT_ALIAS   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , world_time           , "CROSS: Freeze/Unfreeze time"      , "LEFT/RIGHT: Adjust hour"            , "Adjust the Worlds time and freeze it completely." },
  {"Weather:"                         , CAT_ALIAS   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , world_weather        , "CROSS: Lock/Unlock weather"       , "LEFT/RIGHT: Select weather"         , "Adjust and lock the Weather so it won't ever change again." },
  {"Vehicle Spawner:"                 , CAT_ALIAS   , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0      , -1  , vehicle_spawner      , "CROSS/SQUARE: Spawn Vehicle"      , "LEFT/RIGHT: Adjust"                 , "Spawns selected Vehicle in front of you." },
  {"Teleport:"                        , CAT_ALIAS   , MENU_VALUE       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , teleporter           , "CROSS: Teleport to location"      , "LEFT/RIGHT: Select location"        , "Teleport yourself to a preset location on the map." },
  {"Hover Bike & Car"                 , CAT_ALIAS   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , hover_vehicle        , "CROSS: Enable/Disable Cheat"      , ""                                   , "Your vehicle hovers and can fly like a hovercraft." },
  {"Button Up:"                       , CAT_ALIAS   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0      , -1  , up_button            , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Select a Cheat to quick toggle via button when in-game!" },
  {"Button Down:"                     , CAT_ALIAS   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0      , -1  , down_button          , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Select a Cheat to quick toggle via button when in-game!" },
  {""                                 , CAT_ALIAS   , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  // // CHEATS // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 

  {"Player"                           , CAT_PLAYER  , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x245F , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"Infinite Health & Armor"          , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1A93 , OFF , godmode              , "CROSS: Enable/Disable Cheat"      , ""                                   , "You are immune to explosions, gunshots, fall damage, fire etc.." },
  {"Unlimited Ammo & No reloading"    , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x18AB , OFF , unlimited_ammo       , "CROSS: Enable/Disable Cheat"      , ""                                   , "All your Weapons have unlimited ammunition and no reloading needed." },
  {"Unlimited sprinting"              , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x11E0 , -1  , unlimited_sprinting  , "CROSS: Enable/Disable Cheat"      , ""                                   , "You are never getting tired from sprinting ever again." },
  {"Walking Speed:"                   , CAT_PLAYER  , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x192C , OFF , walking_speed        , "CROSS: Enable/Disable Cheat"      , "CIRCLE: Disable and reset"          , "Adjust your speed of walking, sprinting etc" },
  {"Invisible"                        , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x1D18 , OFF , invisible            , "CROSS: Enable/Disable Cheat"      , ""                                   , "You are invisible but can still be attacked." },
  {"Ignored by everyone"              , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x155A , OFF , ignored              , "CROSS: Enable/Disable Cheat"      , ""                                   , "Everyone including police and mission enemys will ignore you." },
  {"Wanted Level:"                    , CAT_PLAYER  , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x19DF , OFF , wanted_level         , "CROSS: Enable/Disable Cheat"      , "SQUARE: Set current level as max"   , "Adjust your Wanted Level, lock it and set the Maximum Level possible." },
  {"Heavy Player"                     , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x1F3B , OFF , heavy_player         , "CROSS: Enable/Disable Cheat"      , ""                                   , "You can push cars by running against them but are to heavy to jump!" },
  {"Aim of Death"                     , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x1BBC , OFF , aim_of_death         , "CROSS: Enable/Disable Cheat"      , ""                                   , "Aiming down at a target will instantly kill it." },
  {"Warp out of Water automatically"  , CAT_PLAYER  , MENU_SWITCH      , TRUE  , FALSE , TRUE  , TRUE  , 0x1A48 , OFF , warp_out_water       , "CROSS: Enable/Disable Cheat"      , ""                                   , "If you fall into the ocean you will be teleported out automatically." },
  {"Model:"                           , CAT_PLAYER  , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x1C87 , OFF , player_model         , "CROSS: Change now"                , "LEFT/RIGHT: Select model"           , "Change the actor skin / model. Beware: May break the game!" },
  {"Maximum Health:"                  , CAT_PLAYER  , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x1D7D , -1  , max_health           , "LEFT/RIGHT: Adjust multiplier"    , ""                                   , "Adjust the Multiplier value giving extra health! ('Health Plus')" },
  {"Maximum Armor:"                   , CAT_PLAYER  , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x178E , -1  , max_armor            , "LEFT/RIGHT: Adjust multiplier"    , ""                                   , "Adjust the Multiplier value giving extra armor! ('Armor Plus')" },
  {"Power Jump"                       , CAT_PLAYER  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1E4A , OFF , powerjump            , "CROSS: Enable/Disable Cheat"      , ""                                   , "Hold the jumping key to jump as high as you want." }, // "Super Jump" in VCS
  #ifdef SWIM
  {"Swimming"                         , CAT_PLAYER  , MENU_SWITCH      , TRUE  , FALSE , TRUE  , TRUE  , 0x173E , OFF , fake_swimming        , "CROSS: Enable/Disable Cheat"      , ""                                   , "You finally learned to swim! No more drowning. Experimental though!"},
  #endif
  {""                                 , CAT_PLAYER  , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  {"Current Vehicle"                  , CAT_VEHICL  , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x258B , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"Hover Bike & Car"                 , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1A4C , OFF , hover_vehicle        , "CROSS: Enable/Disable Cheat"      , ""                                   , "Your vehicle hovers and can fly like a hovercraft." },
  {"Indestructible Vehicle"           , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1F53 , OFF , indestr_vehicle      , "CROSS: Enable/Disable Cheat"      , ""                                   , "Your vehicle is immune to explosions, gunshots, fire etc.." },
  {"Lock Doors when inside"           , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x14AB , OFF , lockdoors            , "CROSS: Enable/Disable Cheat"      , ""                                   , "As soon as you enter a vehicle with doors they will be locked." },
  {"Never fall off Bike"              , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x188D , OFF , neverfalloffbike     , "CROSS: Enable/Disable Cheat"      , ""                                   , "Never fall off your bike when rolling backwards or hitting objects." },
  {"Power Brake"                      , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1EBD , OFF , powerbrake           , "CROSS: Enable/Disable Cheat"      , ""                                   , "Hitting the brakes will make your vehicle stop immediately." },
  {"Primary Color:"                   , CAT_VEHICL  , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x15D1 , -1  , vehicle_base_color   , "CROSS: Select Red/Green/Blue"     , "LEFT/RIGHT: Adjust color value"     , "Adjust your current vehicles primary color!" },
  {"Secondary Color:"                 , CAT_VEHICL  , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x19AC , -1  , vehicle_stripe_color , "CROSS: Select Red/Green/Blue"     , "LEFT/RIGHT: Adjust color value"     , "Adjust your current vehicles secondary color!" },
  {"Heavy Vehicle"                    , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x18D4 , OFF , heavy_vehicle        , "CROSS: Enable/Disable Cheat"      , ""                                   , "You can easily push other cars and world objects." },
  {"Automatic Flip Over"              , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1E07 , OFF , automaticflipover    , "CROSS: Enable/Disable Cheat"      , ""                                   , "Never end up stuck on your roof ever again." },
  {"Drive on walls"                   , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x165A , OFF , driveonwalls         , "CROSS: Enable/Disable Cheat"      , ""                                   , "Drive on walls! Good luck, its tricky ;)" },
  {"N.O.S boost"                      , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x19A9 , OFF , nitro                , "CROSS: Enable/Disable Cheat"      , ""                                   , "Press O to activate a 2 sec Nitro Boost for your vehicle." },
  {"Rocket boost:"                    , CAT_VEHICL  , MENU_VALUE       , TRUE  , TRUE  , TRUE  , TRUE  , 0x19A8 , OFF , rocketboost          , "LEFT/RIGHT: Adjust multiplier"    , "CIRCLE: Disable and reset"          , "Continuous additional boost to your vehicle forward speed." },
  {"Behave like Tank"                 , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x1489 , OFF , tank                 , "CROSS: Enable/Disable Cheat"      , "SQUARE: Toggle explode on touch"    , "Press O to fire cannon and make vehicles explode when run into!" },
  {"Warp out of Water automatically"  , CAT_VEHICL  , MENU_SWITCH      , TRUE  , FALSE , TRUE  , TRUE  , 0x1A49 , OFF , warp_out_water_veh   , "CROSS: Enable/Disable Cheat"      , ""                                   , "If you fall into the ocean you will be teleported out automatically." },
  {"Cars drive on Water"              , CAT_VEHICL  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x15F7 , OFF , driveonwater         , "CROSS: Enable/Disable Cheat"      , ""                                   , "Your car can drive on water." },
  {""                                 , CAT_VEHICL  , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  {"Teleporting"                      , CAT_TELEP   , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x2B73 , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"Teleport:"                        , CAT_TELEP   , MENU_VALUE       , TRUE  , TRUE  , TRUE  , TRUE  , 0x18FA , OFF , teleporter           , "CROSS: Teleport to location"      , "LEFT/RIGHT: Select Location"        , "Teleport yourself to a preset location on the map." },
  {"Teleport near marker"             , CAT_TELEP   , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1647 , -1  , teleport_marker      , "CROSS: Teleport"                  , ""                                   , "Teleport near the custom marker" },
  {"Step Through Wall"                , CAT_TELEP   , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , TRUE  , 0x195A , -1  , stepthroughwall      , "CROSS: Teleport one step forward" , ""                                   , "With this you can easily step through walls get inside buildings etc" },
  {""                                 , CAT_TELEP   , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  {"Vehicles"                         , CAT_TRFFIC  , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x215F , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , ""  },
  {"Disable Traffic"                  , CAT_TRFFIC  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x198C , OFF , traffic_density      , "CROSS: Enable/Disable Cheat"      , ""                                   , "Completely disable the worlds traffic! (Parked vehicles still appear)"  },
  {"Freeze Traffic"                   , CAT_TRFFIC  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x17EC , OFF , traffic_freeze       , "CROSS: Enable/Disable Cheat"      , ""                                   , "Completely freeze the worlds vehicles except your own!"  },
  {"Touch Vehicle to:"                , CAT_TRFFIC  , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1EC4 , OFF , touch_vehicle        , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Decide what should happen with a vehicle as soon as you touch." },
  {"Default RadioStation:"            , CAT_TRFFIC  , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1B98 , OFF , traffic_radiostation , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Select Station"         , "Select a Radio Station that will be played when entering a vehicle."  },
  {"BMX Jump Height:"                 , CAT_TRFFIC  , MENU_VALUE       , FALSE , TRUE  , TRUE  , TRUE  , 0x1BDE , OFF , bmxjumpheight        , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust multiplier"      , "Adjust the BMXs jump height." },
  {"Untouchable"                      , CAT_TRFFIC  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x15D3 , OFF , untouchable          , "CROSS: Enable/Disable Cheat"      , ""                                   , "Vehicles wont be able to get near you and will be pushed back." },
  {""                                 , CAT_TRFFIC  , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  {"Pedestrians"                      , CAT_PEDS    , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x28A4 , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , ""  },
  {"Disable Peds"                     , CAT_PEDS    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1770 , OFF , peds_density         , "CROSS: Enable/Disable Cheat"      , ""                                   , "Completely disable Pedestrians! (special ones might still appear)"  },
  {"Freeze Peds"                      , CAT_PEDS    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x191C , OFF , peds_freeze          , "CROSS: Enable/Disable Cheat"      , ""                                   , "Freeze Pedestrians current positions in the world."  },
  {"Touch Ped to:"                    , CAT_PEDS    , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x14BA , OFF , touch_pedestrian     , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Decide what should happen with a Ped as soon as you touch it." },
  {"Kill everyone aiming at you"      , CAT_PEDS    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1611 , OFF , peds_killaiming      , "CROSS: Enable/Disable Cheat"      , ""                                   , "Everyone aiming at you will be killed instantly!"  },
  {"Ped Walking Speed:"               , CAT_PEDS    , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x191A , OFF , pedwalking_speed     , "CROSS: Enable/Disable Cheat"      , "CIRCLE: Disable and reset"          , "Adjust the Pedestrians walking and sprinting speed." },
  {""                                 , CAT_PEDS    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  {"Missions"                         , CAT_MISSON  , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , FALSE , 0x235B , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , ""  },
  {"Mission Selector"                 , CAT_MISSON  , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1F98 , -1  , mission_selector     , "CROSS: Open Menu"                 , ""                                   , "Use the build-in level-skip menu to start missions." },
  {"Force OnMission bool:"            , CAT_MISSON  , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x17B5 , OFF , onmission            , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Control OnMission check. Handle with care!"  },
  {"Freeze Mission Timers"            , CAT_MISSON  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x16EE , OFF , freeze_timers        , "CROSS: Enable/Disable Cheat"      , ""                                   , "Freeze side missions on screen timers."  },
  {"Kill / destroy all targets"       , CAT_MISSON  , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x18BC , -1  , kill_all_targets     , "CROSS: Execute Order 66"          , ""                                   , "Kill all Targets marked red!" },
  {"Freeze all targets"               , CAT_MISSON  , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1C56 , -1  , freeze_all_targets   , "CROSS: Freeze Targets"            , ""                                   , "Freeze all Targets marked red!" },
  {"Teleport to next Objective"       , CAT_MISSON  , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x1762 , -1  , teleport_next        , "CROSS: Teleport"                  , ""                                   , "Teleport to next marked destination, target or object" },
  {"Teleport to next Mission"         , CAT_MISSON  , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1799 , -1  , teleport_mission     , "CROSS: Teleport"                  , ""                                   , "Teleport to next Story-Mission marker." },
  {""                                 , CAT_MISSON  , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  {"Map"                              , CAT_MAP     , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , FALSE , 0x2A11 , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , ""  },
  {"Mark on Map:"                     , CAT_MAP     , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x165F , OFF , markonmap            , "CROSS: Enable/Disable Cheat"      , "SQUARE: Cycle options"              , "Select what should be displayed on the map and radar." },
  {""                                 , CAT_MAP     , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  {"Camera"                           , CAT_CAMERA  , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x254B , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , ""  },
  {"Classic TopDown view"             , CAT_CAMERA  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1458 , OFF , camera_topdown       , "CROSS: Enable/Disable Cheat"      , ""                                   , "Experience the classic top-down camera angle!" },
  {"Always Centered behind Vehicle"   , CAT_CAMERA  , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1A3C , OFF , camera_centered      , "CROSS: Enable/Disable Cheat"      , ""                                   , "The camera will always be centered behind a vehicle." },
  {"FOV:"                             , CAT_CAMERA  , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1DC7 , OFF , fieldofview          , "CROSS: Enable/Disable Cheat"      , "CIRCLE: Disable and reset"          , "Adjust the Field of View for a better viewing angle." },
  {""                                 , CAT_CAMERA  , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  {"Button Cheats"                    , CAT_STCKCHT , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , FALSE , 0x2E5F , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , ""  },
  {"Give Health"                      , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x19CA , -1  , bttncht_givehealth   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Give Armor"                       , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1874 , -1  , bttncht_givearmor    , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Give 250k Dollars"                , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x198F , -1  , bttncht_givemoney    , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Weaponset 1"                      , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1AC1 , -1  , bttncht_weaponset1   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Weaponset 2"                      , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x16AC , -1  , bttncht_weaponset2   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Weaponset 3"                      , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x17EB , -1  , bttncht_weaponset3   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Spawn Rhino"                      , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x122B , -1  , bttncht_spawnrhino   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Cars drive on Water"              , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , FALSE , TRUE  , FALSE , 0x187A , -1  , bttncht_carsonwater  , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Blow up all nearby Vehicles"      , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x138D , -1  , bttncht_blowupcars   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Change Player Model to:"          , CAT_STCKCHT , MENU_VALUE       , TRUE  , FALSE , TRUE  , FALSE , 0x1D45 , OFF , bttncht_randomplayer , "CROSS: Trigger Stock Cheat"       , "LEFT/RIGHT: Adjust model"           , ""  },
  {"Perfect Traction & Jumping"       , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1654 , -1  , bttncht_perftraction , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Unlock 100% Multiplayer"          , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1D80 , -1  , bttncht_unlockmult100, "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Bubble Heads"                     , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , FALSE , TRUE  , FALSE , 0x146A , -1  , bttncht_bubblehead   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Play Credits"                     , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , FALSE , TRUE  , FALSE , 0x187C , -1  , bttncht_playcredits  , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Peds have Weapons"                , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x195C , -1  , bttncht_pedsweapons  , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Peds attack you"                  , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x135C , -1  , bttncht_pedsattack   , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Peds Riot"                        , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1C71 , -1  , bttncht_pedsriot     , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Aggressive Drivers"               , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x15F3 , -1  , bttncht_agrodrivers  , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {"Chrome Vehicles"                  , CAT_STCKCHT , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , FALSE , 0x1E4C , -1  , bttncht_traffichrome , "CROSS: Trigger Stock Cheat"       , ""                                   , ""  },
  {""                                 , CAT_STCKCHT , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  {"World"                            , CAT_WORLD   , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x280A , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"Time:"                            , CAT_WORLD   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x18AC , OFF , world_time           , "CROSS: Freeze/Unfreeze time"      , "LEFT/RIGHT: Adjust hour"            , "Adjust the Worlds time and freeze it completely." },
  {"Realtime Clock"                   , CAT_WORLD   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x19C1 , OFF , world_realtimeclock  , "CROSS: Enable/Disable Cheat"      , "SQUARE: Sync with System time"      , "A day will last 24 real hours!" },
  {"Weather:"                         , CAT_WORLD   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1E2F , OFF , world_weather        , "CROSS: Lock/Unlock weather"       , "LEFT/RIGHT: Select weather"         , "Adjust and lock the Weather so it won't ever change again."},
  {"Water Level:"                     , CAT_WORLD   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x173A , OFF , world_waterlevel     , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust height"          , "Adjust the oceans water level height."},
  {"Gravity:"                         , CAT_WORLD   , MENU_VALUE       , TRUE  , TRUE  , TRUE  , TRUE  , 0x115C , OFF , world_gravity        , "CROSS: Reverse-button On/Off"     , "LEFT/RIGHT: Adjust gravity"         , "Adjust gravity intensity and hold 'UP' button in-game to reverse it!" },
  {"Touch Object to:"                 , CAT_WORLD   , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x142B , OFF , touch_object         , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Decide what should happen to a world object when touching it." },
  {"Staunton Bridge Lift is:"         , CAT_WORLD   , MENU_VALSWITCH   , TRUE  , FALSE , TRUE  , TRUE  , 0x18CA , OFF , world_liftcontrol    , "CROSS: Force position"            , "LEFT/RIGHT: Adjust option"          , "Change & force Staunton Bridge's Lift position to a selected position." },
  {""                                 , CAT_WORLD   , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  {"Miscellaneous"                    , CAT_MISC    , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x2847 , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"Button Up:"                       , CAT_MISC    , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x1620 , OFF , up_button            , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Select a Cheat to quick toggle via button when in-game!" },
  {"Button Down:"                     , CAT_MISC    , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , FALSE , 0x176A , OFF , down_button          , "CROSS: Enable/Disable Cheat"      , "LEFT/RIGHT: Adjust option"          , "Select a Cheat to quick toggle via button when in-game!" },
  {"Pickup Spawner:"                  , CAT_MISC    , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x1501 , OFF , pickup_spawner       , "CROSS: Create one-time pickup"    , "SQUARE: Create reappearing"         , "Spawns selected Pickup in front of you." },
  {"Vehicle Spawner:"                 , CAT_MISC    , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0x18FF , OFF , vehicle_spawner      , "CROSS/SQUARE: Spawn Vehicle"      , "LEFT/RIGHT: Adjust"                 , "Spawns selected Vehicle in front of you." },
  {"Reset Button Cheats history"      , CAT_MISC    , MENU_VALUE       , TRUE  , TRUE  , TRUE  , FALSE , 0      , -1  , no_cheating_warning  , "CROSS: Reset"                     , ""                                   , "Remove all evidence of stock Cheats ever being used. Safe saving!" },
  {"Vehicle Speedometer:"             , CAT_MISC    , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1507 , OFF , speedometer_toggle   , "CROSS: Toggle Speedometer"        , "SQUARE: Switch calculation"         , "Display a Speed'O'Meter when inside a Vehicle to monitor speed." },
  {"Display FPS"                      , CAT_MISC    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x179A , OFF , fps_toggle           , "CROSS: Toggle Display"            , ""                                   , "Display the Games' Frames Per Second" },
  {"Display Memory Usage"             , CAT_MISC    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x179B , OFF , mem_toggle           , "CROSS: Toggle Display"            , ""                                   , "Display the Games' free main memory" },
  {"Display Coordinates"              , CAT_MISC    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1814 , OFF , coords_toggle        , "CROSS: Toggle Coordinates"        , ""                                   , "Display your current xyz coordinates in the world." },
  {"Gather Spell"                     , CAT_MISC    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1E35 , OFF , gather_spell         , "CROSS: Enable/Disable Cheat"      , ""                                   , "Gather everything!! (There will be LAG.. but its worth it!)"  },
  {""                                 , CAT_DUMMY   , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  {"Multiplayer"                      , CAT_MULTI   , MENU_CATEGORY    , TRUE  , TRUE  , FALSE , TRUE  , 0x2D2A , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"test"                             , CAT_MULTI   , MENU_SWITCH      , TRUE  , TRUE  , FALSE , TRUE  , 0x181C , OFF , mp_test              , "CROSS: Enable/Disable Cheat"      , ""                                   , "" },
  {""                                 , CAT_DUMMY   , MENU_DUMMY       , TRUE  , TRUE  , FALSE , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  {"Game Options"                     , CAT_GAME    , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x2542 , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"Developer Flag"                   , CAT_GAME    , MENU_SWITCH      , TRUE  , FALSE , TRUE  , TRUE  , 0x14C0 , OFF , dev_flag             , "CROSS: Enable/Disable Cheat"      , ""                                   , "Spawn at Debug area on New Game & Start Multiplayer alone" },
  {"Gamespeed:"                       , CAT_GAME    , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x1965 , OFF , gamespeed            , "CROSS: Enable/Disable Cheat"      , "CIRCLE: Disable and reset"          , "Adjust the games speed." },
  {"Skip Intro Movies"                , CAT_GAME    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x1DD1 , OFF , skip_intros          , "CROSS: Enable/Disable Cheat"      , ""                                   , "Skip Rockstar Logo and Intro Movie when starting the game." },
  {"Debug Messages on Loadscreen"     , CAT_GAME    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x184C , OFF , debug_loadscreens    , "CROSS: Enable/Disable"            , ""                                   , "Re-enable some of Rockstars debug messages on the Loadscreen." },
  {"Random Loadscreens"               , CAT_GAME    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x18D3 , OFF , random_loadscreens   , "CROSS: Enable/Disable"            , ""                                   , "Real random Loadscreens including Multiplayer ones!" },
  {"Disable World Textures"           , CAT_GAME    , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x17CF , OFF , disable_textures     , "CROSS: Enable/Disable"            , ""                                   , "Stop World Textures from being applied to models." },
  {"Limit FPS to:"                    , CAT_GAME    , MENU_VALSWITCH   , TRUE  , TRUE  , TRUE  , TRUE  , 0x16F0 , OFF , fps_cap              , "CROSS: Toggle FPS Limit"          , ""                                   , "Limit the Games' Frames Per Second and enable 60 FPS!" },
  {""                                 , CAT_GAME    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  {"CheatDevice Options"              , CAT_CHDEV   , MENU_CATEGORY    , TRUE  , TRUE  , TRUE  , TRUE  , 0x285E , OFF , category_toggle      , "CROSS: Show/Hide Category"        , ""                                   , "" },
  {"Autostart CheatDevice Menu"       , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x38F4 , OFF , cdr_autostartmenu    , "CROSS: Enable/Disable"            , ""                                   , "Enabling will automatically start the CheatDevice after spawning." },
  #ifdef CONFIG
  {"Autosave Settings to Config"      , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x3E81 , OFF , cdr_liveconfig       , "CROSS: Enable/Disable"            , ""                                   , "All settings will be auto-saved to config when closing the menu." },
  #endif
  {"Collapsible Categories"           , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x365A , ON  , cdr_collapsecats     , "CROSS: Enable/Disable"            , ""                                   , "Choose between collapsible or always expanded categories." },
  {"Show popular cheats on top"       , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x3C42 , ON  , cdr_showpopular      , "CROSS: Enable/Disable"            , ""                                   , "Enabling will show a preset selection of popular Cheats on top." },
  {"Hide some UserInterfaces in Menu" , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x3881 , ON  , cdr_allowuiblocking  , "CROSS: Enable/Disable"            , ""                                   , "Allow Games's UserInterface elements to be blocked in some menus etc" },
  {"Freeze Game when in Menu"         , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , FALSE , 0x3F9E , OFF , cdr_freezegameinmenu , "CROSS: Enable/Disable"            , ""                                   , "Freeze the Game when the CheatDevice Menu is in use." },
  {"Hide Button Legend & Info"        , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x365B , OFF , cdr_uselegend        , "CROSS: Enable/Disable"            , ""                                   , "Hide the button legend allowing for more space and menu displayed!" },
  {"Alternative font for Categories"  , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x3846 , OFF , cdr_alternativefont  , "CROSS: Enable/Disable"            , ""                                   , "Use an alternative font for categories just like the main menu does." },
  {"Swap X with R for acceleration"   , CAT_CHDEV   , MENU_SWITCH      , TRUE  , TRUE  , TRUE  , TRUE  , 0x7F92 , OFF , cdr_swapacceleration , "CROSS: Enable/Disable"            , ""                                   , "Activate this if you use gta_remastered's X and R swapped controls." },
  
  #ifdef LANG
  {"Menu Language:"                   , CAT_CHDEV   , MENU_VALUE       , TRUE  , TRUE  , TRUE  , TRUE  , 0x1FB9 , OFF , cdr_changelang       , "CROSS: Change Menu Language"      , "LEFT/RIGHT: Select Language"        , "Change Cheat Device Remastered's Language for your native one!"},
  #endif

  {""                                 , CAT_MAIN    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 

  #ifdef USERSCRIPTS
  {"User Scripts"                     , CAT_MAIN    , MENU_CDR_USCM    , TRUE  , TRUE  , TRUE  , FALSE , 0      , -1  , user_scripts         , "CROSS: Open UserScripts"          , "TRIANGLE: Open Location"            , "Basic GTA Scripting Language Interpreter. See Readme on how to use" },
  {""                                 , CAT_MAIN    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , FALSE , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  #endif
  
  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //   
  
  #ifdef HEXEDITOR
  {"Hex Editor"                       , CAT_MAIN    , MENU_CDR_HEX     , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , hexeditor            , "CROSS: Open HexEditor"            , ""                                   , "A fully fledged HexEditor to directly monitor and work in memory!" },
  {"Hex Edit 'pplayer'"               , CAT_MAIN    , MENU_CDR_HEX     , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , hexeditpplayer       , "CROSS: Open HexEditor"            , ""                                   , "Open Object in Hex Editor" },
  {"Hex Edit 'pcar'"                  , CAT_MAIN    , MENU_CDR_HEX     , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , hexeditpcar          , "CROSS: Open HexEditor"            , ""                                   , "Open Object in Hex Editor" },
  {""                                 , CAT_MAIN    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  #endif
  
  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
  
  #ifdef FREECAM
  {"Free Camera"                      , CAT_MAIN    , MENU_CDR_FREECAM , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , freecam              , "CROSS: Free Camera Mode"          , "TRIANGLE: Open in HexEditor"        , "Unbind the camera to freely explore the world!" },
  {""                                 , CAT_MAIN    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  #endif
  
  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //   
  
  #ifdef EDITORS
  {"People Objects Editor"            , CAT_MAIN    , MENU_CDR_EDITOR  , TRUE  , TRUE  , TRUE  , TRUE  , 0x4D7E , -1  , editor_pedobj        , "CROSS: Open Editor"               , ""                                   , "Edit Pedestrian objects" },
  {"Vehicle Objects Editor"           , CAT_MAIN    , MENU_CDR_EDITOR  , TRUE  , TRUE  , TRUE  , TRUE  , 0x46AA , -1  , editor_vehicleobj    , "CROSS: Open Editor"               , ""                                   , "Edit Vehicle objects" },
  {"World Objects Editor"             , CAT_MAIN    , MENU_CDR_EDITOR  , TRUE  , TRUE  , TRUE  , TRUE  , 0x48AB , -1  , editor_worldobj      , "CROSS: Open Editor"               , ""                                   , "Edit World objects" },
  {"Business Objects Editor"          , CAT_MAIN    , MENU_CDR_EDITOR  , FALSE , TRUE  , TRUE  , TRUE  , 0x4EF2 , -1  , editor_businessobj   , "CROSS: Open Editor"               , ""                                   , "Edit Business objects" },
  {"Pickups Editor"                   , CAT_MAIN    , MENU_CDR_EDITOR  , TRUE  , TRUE  , TRUE  , TRUE  , 0x4DD7 , -1  , editor_pickups       , "CROSS: Open Editor"               , ""                                   , "Edit Pickups objects" },
  {"Mapicons Editor"                  , CAT_MAIN    , MENU_CDR_EDITOR  , TRUE  , TRUE  , TRUE  , TRUE  , 0x4814 , -1  , editor_mapicons      , "CROSS: Open Editor"               , ""                                   , "Edit Mapicon objects" },
  {"Parked Vehicles Editor"           , CAT_MAIN    , MENU_CDR_EDITOR  , TRUE  , TRUE  , TRUE  , FALSE , 0x47F1 , -1  , editor_vehspawns     , "CROSS: Open Editor"               , ""                                   , "Edit the parked world vehicles values and create new spawns." },
  {"Garage Editor"                    , CAT_MAIN    , MENU_CDR_EDITOR  , TRUE  , TRUE  , TRUE  , FALSE , 0x4D66 , -1  , editor_garage        , "CROSS: Open Editor"               , ""                                   , "Edit your stored Garage Vehicles. Garage must be closed!" },
  {""                                 , CAT_MAIN    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //   
  
  {"IDEs"                             , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x437E , -1  , editor_ide           , "CROSS: Open Editor"               , ""                                   , "Edit 'ItemDefinitions' of Peds, Vehicles, Objects, etc..." },
  {""                                 , CAT_MAIN    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  
  {"Buildings.ipl"                    , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x45AB , -1  , editor_buildingsipl  , "CROSS: Open Editor"               , ""                                   , "Edit 'ItemPlacement' of Buildings" },
  {"Treadables.ipl"                   , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , FALSE , TRUE  , TRUE  , 0x44D7 , -1  , editor_treadablesipl , "CROSS: Open Editor"               , ""                                   , "Edit 'ItemPlacement' of Roads, Grounds etc" },
  {"Dummys.ipl"                       , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x4D78 , -1  , editor_dummysipl     , "CROSS: Open Editor"               , ""                                   , "Edit 'ItemPlacement' of Doors, Objects etc" },
  {"Carcols.dat"                      , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x411E , -1  , editor_carcolsdat    , "CROSS: Open Editor"               , ""                                   , "Edit Vehicle color presets." },
  {"Pedcols.dat"                      , CAT_MAIN    , MENU_CDR_FILES   , FALSE , TRUE  , TRUE  , TRUE  , 0x465F , -1  , editor_pedcolsdat    , "CROSS: Open Editor"               , ""                                   , "Edit Pedestrian color presets." },
  {"Handling.cfg"                     , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x4632 , -1  , editor_handlingcfg   , "CROSS: Open Editor"               , ""                                   , "Adjust a Vehicles' handling behavior." },
  {"Particle.cfg"                     , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x4624 , -1  , editor_particlecfg   , "CROSS: Open Editor"               , ""                                   , "Adjust particle color, behaviour, timings etc" },
  {"Pedstats.dat"                     , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x4DB4 , -1  , editor_pedstatsdat   , "CROSS: Open Editor"               , ""                                   , "Edit Pedestrian's behaviour, abilities and statistics." },
  {"Weapon.dat"                       , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x41A2 , -1  , editor_weapondat     , "CROSS: Open Editor"               , ""                                   , "Edit Weapon's behaviour, abilities and animations." },
  {"Timecyc.dat"                      , CAT_MAIN    , MENU_CDR_FILES   , TRUE  , TRUE  , TRUE  , TRUE  , 0x4143 , -1  , editor_timecycdat    , "CROSS: Open Editor"               , ""                                   , "Edit World's colors per hour and weather" },
  {""                                 , CAT_MAIN    , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },
  #endif
  
  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
  
  #ifdef CONFIG
  {"Save Settings to Config"          , CAT_MAIN    , MENU_CONFIG      , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , save_config          , "CROSS: Save settings to config"   , ""                                   , "Save your current preferences to the config file." },
  {"Load Settings from Config"        , CAT_MAIN    , MENU_CONFIG      , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , load_config          , "CROSS: Reload config"             , ""                                   , "Load custom saved settings from the config file." },
  #endif
  {"Restore default Settings"         , CAT_MAIN    , MENU_CONFIG      , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , load_defaults        , "CROSS: Reset cheats to default"   , ""                                   , "Reset all menu cheats to their default disabled values." },
  
  {""                                 , CAT_DUMMY   , MENU_DUMMY       , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , NULL                 , NULL                               , NULL                                 , NULL },  
  {"Exit Game"                        , CAT_MAIN    , MENU_FUNCTION    , TRUE  , TRUE  , TRUE  , TRUE  , 0      , -1  , exit_game            , "CROSS: Exit game"                 , ""                                   , "Exit the game and return to the main menu." },
  
  {"",0,0,-1,-1,-1,-1,-1,-1,NULL,NULL,NULL,NULL}
}; int menu_size = (sizeof(main_menu)/sizeof(Menu_pack))-1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawLegendBox(int rows, u32 color) {
  drawBox(15.0f, 266.0f - (rows * 12.0f), 450.0f, 4.0f + (rows * 12.0f), color);
}

void drawLegendMessage(char *str, int side, int pos, u32 color) { // side: 0 = LEFT, 1 = RIGHT & pos: 0 = bottom, 1 = second last, 2 = "first text when three rows" etc
  drawString(str, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, side ? 240.0f : 20.0f, 256.0f - (pos * 12.0f), color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if defined(PREVIEW) || defined(USERSCRIPTS)
char extension[5];
char filename[64];

#define META_DESC_SIZE 90  // max length
#define META_DATE_SIZE 32  // max length
#define META_CATG_SIZE 32  // max length
#define META_VERS_SIZE 32  // max length
#define META_AUTH_SIZE 32  // max length

char meta_author[META_AUTH_SIZE];
char meta_version[META_VERS_SIZE];
char meta_category[META_CATG_SIZE];
char meta_date[META_DATE_SIZE];
char meta_description[META_DESC_SIZE];

void resetMeta() {  // clear
  memset(meta_author, 0, sizeof(meta_author)); 
  memset(meta_version, 0, sizeof(meta_version));
  memset(meta_category, 0, sizeof(meta_category));
  memset(meta_date, 0, sizeof(meta_date));
  memset(meta_description, 0, sizeof(meta_description));
}
#endif


/********************************************************************************************************
*
* User Cheats
*
*********************************************************************************************************/

#ifdef PREVIEW
short flag_usercheats = 0;

float usercheat_row_spacing = 15.f;  // pixels between rows

int usercheat_toptions;     // top menu lines
int usercheat_options;      // available cheats in txt (set later)
int usercheat_showoptions;  // the number of value rows that should be displayed at once (0 to 2 possible)  
int usercheat_selector;     // cursor in top menu bool 
int usercheat_selected_val; // selected option in values number
int usercheat_top;          // the values option on top when there are more options than usercheat_showoptions
int usercheat_draw_lower;   // some have no data in slots -> those can be disabled here

int usercheat_waittime = 0;    // for faster scrolling (delay until meta data will be read)
int usercheat_waitdelay = 400; // 1000 is about 1 sec

int readtxtneeded = 1;    // if 1 force txt read
int readfolderneeded = 1; // if 1 filename of current txt will be read + total files
int cur_dirno = 0;        // no of txt files in folder 
int usercheat_dirno = 0;  // current no of txt file in folder 


#define CHEATSPERTXT 128  // max cheats per txt possible
#define CHEATNAMELGT 60   // max name length of cheat 
#define CHEATLINELGT 256  // max line length in txt

char cheatnames[CHEATSPERTXT][CHEATNAMELGT]; // cheat names in current txt


int usercheats_create() {
  #ifdef LOG
  logPrintf("[INFO] %i: usercheats_create()", getGametime());
  #endif
  
  resetMeta(); // because used for other scripts too
  
  usercheat_toptions = 1;   // always 1 for now
  usercheat_selector = 1;   // cursor in txt selector (top menu) by default
  usercheat_draw_lower = 0; // disable drawing of lower menu by default
  usercheat_options = 0; 
  usercheat_showoptions = (flag_use_legend ? 12 : 14) - usercheat_toptions;
  usercheat_selected_val = 0;
  usercheat_top = 0;
  
  readtxtneeded = 1;    // now
  readfolderneeded = 1; // now
  
  flag_usercheats = 1;  // only set here!
  return 0;
}

int usercheats_draw() {
  int i;
  float x, y;
  char local_buff[256];
  char buffer_top[128]; // top option
  char buffer_top2[16]; // top option right
  
  snprintf(buffer, sizeof(buffer), "%s%s%s/", basefolder, folder_cheats, (LCS ? "LCS" : "VCS")); // "../CHEATS/xCS/"
  
  /// draw title  
  drawString("User Cheats", ALIGN_FREE, FONT_DIALOG, SIZE_BIG, SHADOW_OFF, 8.0f, 5.0f, COLOR_USERCHEATS);
  
  /// read in filename, extension & total files
  if( readfolderneeded ) {
    SceUID fd;
    SceIoDirent d_dir;
    memset(&d_dir,0,sizeof(SceIoDirent));
    fd = sceIoDopen(buffer); // cheats folder
    cur_dirno = 0;
    if( fd >= 0 ) {
      while( (sceIoDread(fd, &d_dir) > 0) )  {
        if( FIO_SO_ISDIR(d_dir.d_stat.st_attr) || FIO_S_ISDIR(d_dir.d_stat.st_mode) ) { // handle sub dirs
          if( d_dir.d_name[0] == '.' ) 
            continue;
        } else {
          if( d_dir.d_name[0] == '.' )
            continue;
          if( cur_dirno == usercheat_dirno ) { // this is the selected file
            memcpy(extension, d_dir.d_name + strlen(d_dir.d_name) - 4, 5);
            strcpy(filename, d_dir.d_name); // save filename
          }
        }
        cur_dirno++; // count all files in folder for later
      }
      sceIoDclose(fd);
    
    } else { // no cheats folder 
      drawString("No user cheats folder found!", ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
      sprintf(local_buff, "Create '%s' and place .txt there", buffer);
      drawString(local_buff, ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + usercheat_row_spacing, COLOR_TEXT);
      return -1;
    }
    
    if( cur_dirno <= 0 ) { // folder found but empty
      drawString("No user cheat files found!", ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
      sprintf(local_buff, "Place .txt files to '%s'", buffer);
      drawString(local_buff, ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + usercheat_row_spacing, COLOR_TEXT);
      return -1;
    }
    readfolderneeded = 0;
  }
  
  x = 40.0f; // horizontal menu start
  y = 35.0f; // vertical menu start
  
  /// draw UI 
  if( usercheat_toptions > 0 ) {
    float topmenuheight = 20.0f;
    drawUiBox(x-5.0f, y-2.0f, 410.0f, topmenuheight, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // x, y, width, height, border, color, color  
  }
  drawUiBox(x-5.0f, y-2.0f, 410.0f, flag_use_legend ? 194.0f : 224.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // main
  
  /// draw the top menu
  snprintf(buffer_top, sizeof(buffer_top), "File: %s", filename);
  snprintf(buffer_top2, sizeof(buffer_top2), "%d of %d", usercheat_dirno+1, cur_dirno);
  if(usercheat_selector == 1) { // cursor in top menu (txt select)
    drawString(buffer_top, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CHEAT_ON);
    drawString(buffer_top2, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+400.0f, y, COLOR_CHEAT_ON);
  } else { // cursor in lower menu (cheat select)
    drawString(buffer_top, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CATEGORY);
    drawString(buffer_top2, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+400.0f, y, COLOR_CATEGORY);
  }
  
  /// error checks of selected file
  if( !stricmp(extension, ".txt") ) { // check for txt
    
    /// read in ALL cheat names and CURRENT selected cheat's meta data
    if( readtxtneeded || (usercheat_waittime > 0 && getGametime() > usercheat_waittime) ) {
      
      resetMeta();
      usercheat_waittime = 0;

      sprintf(local_buff, "%s%s", buffer, filename);
      SceUID file = sceIoOpen(local_buff, PSP_O_RDONLY, 0777);
      if( file >= 0 ) {
        
        int found_cheats_counter = 0;
        char readbuf[CHEATLINELGT]; // CHEATLINELGT is max line length
        
        /// read in line by line /////////////////////////////////////////////////////////////
        while( _fgets(readbuf, CHEATLINELGT, file) && found_cheats_counter < CHEATSPERTXT ) { // read txt line by line
        
          char *linehandle = readbuf; // create ptr to work with
          
          /// remove preceding whitespaces and tabs
          while( (*linehandle == ' ') || (*linehandle == '\t') ) 
            ++linehandle;
          
          /// check #
          if( linehandle[0] == '#' ) { // check first char is #
            
            /// check "#cheat"
            linehandle = strstr(linehandle, "#cheat");
            if( linehandle != NULL ) { // "#cheat" found
              linehandle += 6; // skip "#cheat" (this is where name begins
              
              /// remove preceding whitespaces and tabs
              while( (*linehandle == ' ') || (*linehandle == '\t') ) 
                ++linehandle;
          
              /// save cheat names to array
              memset(cheatnames[found_cheats_counter], 0, sizeof(cheatnames[found_cheats_counter])); // clear
              strncpy(cheatnames[found_cheats_counter], linehandle, CHEATNAMELGT);
                
                /// if name too long add dots
                cheatnames[found_cheats_counter][CHEATNAMELGT-3] = '.';
                cheatnames[found_cheats_counter][CHEATNAMELGT-2] = '.';
                cheatnames[found_cheats_counter][CHEATNAMELGT-1] = '\0';
                
                /// exclude possible comments in title
                linehandle = strstr(cheatnames[found_cheats_counter], "//");
                if( linehandle != NULL ) 
                  linehandle[0] = '\0';
                
              found_cheats_counter++;
              continue;
            }
            continue;
          }
          
          
          /// check comment
          if( (usercheat_selected_val+1 == found_cheats_counter) && linehandle[0] == '/' && linehandle[1] == '/' ) { // check first char is # 
            
            /// skip all comment slashes
            while( (*linehandle == '/') ) 
              ++linehandle;
            
            char *temphandle = linehandle;
            
            /// check "Author:" comment
            temphandle = strstr(linehandle, "Author:");
            if( temphandle != NULL ) {
              strncpy(meta_author, temphandle+=7, META_AUTH_SIZE);
              meta_category[META_AUTH_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Version:" comment
            temphandle = strstr(linehandle, "Version:");
            if( temphandle != NULL ) {
              strncpy(meta_version, temphandle+=8, META_VERS_SIZE);
              meta_category[META_VERS_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Category:" comment
            temphandle = strstr(linehandle, "Category:");
            if( temphandle != NULL ) {
              strncpy(meta_category, temphandle+=9, META_CATG_SIZE);
              meta_category[META_CATG_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Date:" comment
            temphandle = strstr(linehandle, "Date:");
            if( temphandle != NULL ) {
              strncpy(meta_date, temphandle+=5, META_DATE_SIZE);
              meta_date[META_DATE_SIZE-1] = '\0';
              continue;
            }
            temphandle = strstr(linehandle, "Time:");
            if( temphandle != NULL ) {
              strncpy(meta_date, temphandle+=5, META_DATE_SIZE);
              meta_date[META_DATE_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Description:" comment
            temphandle = strstr(linehandle, "Description:");
            if( temphandle != NULL ) {
              strncpy(meta_description, temphandle+=12, META_DESC_SIZE);
              meta_description[META_DESC_SIZE-3] = '.';
              meta_description[META_DESC_SIZE-2] = '.';
              meta_description[META_DESC_SIZE-1] = '\0';
              continue;
            }
            
          }  
        
        
        }
        /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 
        
        usercheat_options = found_cheats_counter; // read in no of cheats in txt
        if( found_cheats_counter > 0)
          usercheat_draw_lower = 1; // allow lower menu to be accessed
        
        usercheat_showoptions = (flag_use_legend ? 12 : 14) - usercheat_toptions; // reset
        
        if ( usercheat_showoptions > usercheat_options ) 
          usercheat_showoptions = usercheat_options; // adjust showoptions (there can't be more showoptions than options itself)
        
        
        ////////////////////////////////////////////////////////////////////
        
        sceIoClose(file);
        readtxtneeded = 0;
        
      } else {
        usercheat_selector = 1; // jump cursor back to top menu
        usercheat_draw_lower = 0;
        drawString("Could not open file!", ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
        readfolderneeded = 1; // re-trigger folder read
        //usercheat_dirno = 0; // reset to first found
      } 
      
    }
    
    if( usercheat_waittime > 0 && usercheat_options <= 0) { // in delay window
      usercheat_draw_lower = 0;
      drawString("Loading..", ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
      
    } else if( usercheat_options <= 0 ) {
      usercheat_draw_lower = 0;
      drawString("No cheats found in this .txt", ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
    }
  
  } else {
    usercheat_selector = 1; // jump cursor back to top menu
    usercheat_draw_lower = 0;
    drawString("Not a txt file!", ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
  }
  

  /// draw Scrollbar
  if( (usercheat_options > usercheat_showoptions) && usercheat_draw_lower ) { // draw only if there are more options than can be displayed
    float scrollbar_x = 455.0f;
    float scrollbar_y = 33.0f + ( usercheat_toptions ? ((usercheat_row_spacing * usercheat_toptions) + 8.0f) : 0.0f);
    float scrollbar_bg_width = 5.0f;
    float scrollbar_bg_height = (flag_use_legend ? 194.0f : 224.0f) - ( usercheat_toptions ? ((usercheat_row_spacing * usercheat_toptions) + 8.0f) : 0.0f);
    float scrollbar_cursor_height = scrollbar_bg_height * ((float)usercheat_showoptions/(float)(usercheat_options-1));  // 1.0 all visible and 0.2 when 20% visible  
    float scrollbar_cursor_y = scrollbar_y + ((scrollbar_bg_height - scrollbar_cursor_height) / 100.0f * (((float)usercheat_top) * 100.0f / ((float)(usercheat_options-usercheat_showoptions))) ); // scroll only when entries move (like it should be)

    if( usercheat_top + usercheat_showoptions >= usercheat_options-1 ) // when last item is visible -> cursor must be at bottom
      scrollbar_bg_height -= 0.01f; // fix because for blit bug when two rectangles on same position
    
    drawBox(scrollbar_x, scrollbar_y, scrollbar_bg_width, scrollbar_bg_height, COLOR_BACKGROUND); // background 
    drawBox(scrollbar_x, scrollbar_cursor_y, scrollbar_bg_width, scrollbar_cursor_height, COLOR_SCROLLBAR); // cursor 
  }  
  
  
  /// draw the lower Menu
  if( usercheat_toptions ) 
    y += (usercheat_row_spacing * usercheat_toptions) + 8.0f; // vertical menu start
  
  if( usercheat_draw_lower ) {

    if ( usercheat_selected_val < usercheat_top ) // for scrolling up with selection  
      usercheat_top = usercheat_selected_val; 
    
    for( i = usercheat_top; i < usercheat_showoptions + usercheat_top; i++, y += usercheat_row_spacing ) {
      
      ///draw number
      #ifdef DEBUG
      if( flag_draw_DBGVALS ) {
        snprintf(buffer, sizeof(buffer), "%d", i+1);
        drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x-10.0f, y+2.0f, RED);
      }
      #endif
      
      /// draw cursor
      if ( i == usercheat_selected_val && usercheat_selector == 0) {
        drawBox(x-5.0f, y, 410.0f, 16.0f, COLOR_CURSOR); // float x, float y, float width, float height, u32 color
      }
  
      /// draw cheat name  
      if( 0 ) { // if cheat "ON" or "OFF" - TODO obviously
        COLOR_TEMP = COLOR_CHEAT_ON;
        drawString("ON", ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+24.0f, y, COLOR_TEMP);
      } else {
        COLOR_TEMP = COLOR_CHEAT_OFF;
        drawString("OFF", ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+24.0f, y, COLOR_TEMP);
      }
      snprintf(buffer, sizeof(buffer), "%s", cheatnames[i]); 
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+30.0f, y, COLOR_TEMP);      
    }
    
  } 
  

  #ifdef DEBUG
  if( flag_draw_DBGVALS ) {  
    snprintf(buffer, sizeof(buffer), "usercheat_dirno = %d", usercheat_dirno );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 60.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "cur_dirno = %d", cur_dirno );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 80.0f, RED);
  
  
    snprintf(buffer, sizeof(buffer), "usercheat_selector = %d", usercheat_selector );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 120.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "usercheat_top = %d", usercheat_top );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 140.0f, RED);
        
    snprintf(buffer, sizeof(buffer), "usercheat_selected_val = %i", usercheat_selected_val );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 160.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "usercheat_options = %i", usercheat_options );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 180.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "usercheat_showoptions = %i", usercheat_showoptions );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 200.0f, RED);

    snprintf(buffer, sizeof(buffer), "usercheat_draw_lower = %i", usercheat_draw_lower );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 220.0f, RED);
  }
  #endif
  
  
  /// draw Legend box
  if( flag_use_legend ) {
    drawLegendBox(3, COLOR_BACKGROUND);
    
    if( usercheat_selector == 1 ) { // cursor in top menu (txt select)
      drawLegendMessage("L+UP/DOWN: Toggle Menu",      0, 2, COLOR_TEXT); // left side, first row
      drawLegendMessage("UP/DOWN: Navigate Menu",      1, 2, COLOR_TEXT); // right side, first row
      drawLegendMessage("LEFT/RIGHT: Navigate files",  0, 1, COLOR_TEXT); // left side, second row
      drawLegendMessage("TRIANGLE: Open in Editor",    1, 1, COLOR_TEXT); // right side, second row
      drawLegendMessage("CROSS: Enable/Disable Cheat", 0, 0, COLOR_TEXT); // left side, third row
      drawLegendMessage("CIRCLE: Exit to Menu",        1, 0, COLOR_TEXT); // right side, third row
    
    } else { /// draw Cheat data instead
      snprintf(buffer, sizeof(buffer), "Author: %s", meta_author );
      drawLegendMessage(buffer, 0, 2, COLOR_TEXT); // left side, first row
      
      snprintf(buffer, sizeof(buffer), "Version: %s", meta_version );
      drawLegendMessage(buffer, 1, 2, COLOR_TEXT); // right side, first row
      
      snprintf(buffer, sizeof(buffer), "Category: %s", meta_category );
      drawLegendMessage(buffer, 0, 1, COLOR_TEXT); // left side, second row
      
      snprintf(buffer, sizeof(buffer), "Date: %s", meta_date );
      drawLegendMessage(buffer, 1, 1, COLOR_TEXT); // right side, second row
      
      snprintf(buffer, sizeof(buffer), "Description: %s", meta_description );
      drawLegendMessage(buffer, 0, 0, COLOR_TEXT); // left side, third row
    }
    
  }

  return 0;
}

int usercheats_ctrl() {
  if( usercheat_selector ) { // cursor in top menu (txt select)
    
    if( hold_buttons & PSP_CTRL_DOWN ) {
      if( usercheat_draw_lower ) {
        usercheat_selector = 0; // switch to values
        //readtxtneeded = 1;
        usercheat_waittime = getGametime() + usercheat_waitdelay;
        resetMeta();
      }
    }
    
    if(  hold_buttons & PSP_CTRL_LEFT ) { // previous txt
      if( usercheat_dirno > 0 ) {
        usercheat_dirno--;
        usercheat_options = 0;
        readfolderneeded = 1; // trigger read folder again
        //readtxtneeded = 1;
        usercheat_waittime = getGametime() + usercheat_waitdelay;
        resetMeta();
      }
    }
  
    if( hold_buttons & PSP_CTRL_RIGHT ) { // next txt
      if( usercheat_dirno < cur_dirno-1 ) {
        usercheat_dirno++;
        usercheat_options = 0;
        readfolderneeded = 1; // trigger read folder again
        //readtxtneeded = 1;
        usercheat_waittime = getGametime() + usercheat_waitdelay;
        resetMeta();
      }
    }
    
  } else { // cursor in lower menu (cheat select)
    
    if( hold_buttons & PSP_CTRL_UP ) {
      if( usercheat_selected_val <= 0 ) {
        if( usercheat_toptions )
          usercheat_selector = 1; // switch to top menu
      } else usercheat_selected_val -= 1;  
      
      if( usercheat_top > 1 ) { // scroll
        if( usercheat_selected_val == 1 ) 
          usercheat_top--;
      }
    
      //readtxtneeded = 1;
      usercheat_waittime = getGametime() + usercheat_waitdelay;
      resetMeta();
    }   
      
    if( hold_buttons & PSP_CTRL_DOWN ) {
      if( usercheat_showoptions == 1 ) { // handling 1 option only
        if( usercheat_selected_val >= usercheat_options-1 ) 
          usercheat_selected_val = usercheat_options-1; // selection = 0;
        else 
          usercheat_selected_val += 1;
        
        usercheat_top = usercheat_selected_val;
        
      } else {
        if( usercheat_selected_val >= usercheat_options-1 ) 
          usercheat_selected_val = usercheat_options-1; // selection = 0;
        
        else {
          usercheat_selected_val += 1;
          
          if( usercheat_top + usercheat_showoptions < usercheat_options ) { // scroll
            if( usercheat_selected_val >= usercheat_top + usercheat_showoptions-1 ) 
              usercheat_top++; 
          }
        }  
        if( usercheat_top + usercheat_showoptions < usercheat_options ) { // scroll
          if( usercheat_selected_val == usercheat_top + usercheat_showoptions-1 ) 
            usercheat_top++; 
        }
      }
      
      //readtxtneeded = 1;
      usercheat_waittime = getGametime() + usercheat_waitdelay; // about 0.5 sec
      resetMeta();
      
    }
      
    
    if( pressed_buttons & PSP_CTRL_CROSS ) { // toggle cheat
    
      ////////////////////////////////////////////////////////////
      
      /// read in txt (cheat x)
      
      //if on / off
      
        /// get code and add to executor
      
      //else
        
        /// remove code from executor
        
        /// exec #off section
      
      
      ////////////////////////////////////////////////////////////
    }
    
  }
  
  if( pressed_buttons & PSP_CTRL_CIRCLE ) // exit
    flag_usercheats = 0;

  return 0;  
}
#endif




/********************************************************************************************************
*
* User Scripts
*
*********************************************************************************************************/
#ifdef USERSCRIPTS
short flag_userscripts = 0;

float userscript_row_spacing = 15.0f; // pixels between rows

int userscript_toptions;     // top menu lines
int userscript_options;      // available cheats in txt (set later)
int userscript_showoptions;  // the number of value rows that should be displayed at once (0 to 2 possible)  
int userscript_selector;     // cursor in top menu bool 
int userscript_selected_val; // selected option in values number
int userscript_top;          // the values option on top when there are more options than usercheat_showoptions
int userscript_draw_lower;   // some have no data in slots -> those can be disabled here

int userscript_waittime = 0;   // for faster scrolling (delay until meta data will be read)
int userscript_waitdelay = 400; // 1000 is about 1 sec

int userscript_readtxtneeded = 1; // if 1 
//int userscript_readfolderneeded = 1; // if 1 filename of current txt will be read + total files

char currentfile[64]; // current txt
char currentexten[5]; // 

char script_subfldrs[128]; // contains additional path eg "/WIP/vehicles/" and will be merged to with script_folder
char script_workfldr[256]; // into this one

#define SCRIPTNAMELGT 60  // max name length of cheat 
#define SCRIPTLINELGT 256 // max line length in txt

#define CSTGXTS 32    // max custom strings possible (ALSO CHANGE IN cheats.c!!)
#define CSTGXTLGT 256 // max custom string length (ALSO CHANGE IN cheats.c!!)

ushort custom_gxts[CSTGXTS][CSTGXTLGT];

#define SCRIPT_SIZE 16384   // maximum script size

#define SUPPORT_LABEL 256   // maximum supported Labels
#define MAX_LABEL_LENGTH 32 // max length of a Label
  

int userscripts_create() {
  #ifdef LOG
  logPrintf("[INFO] %i: userscript_create()", getGametime());
  #endif

  resetMeta(); // because used for other cheats too
  
  userscript_toptions = 0;   // always 0 for now
  userscript_selector = 0;   // cursor in txt selector (top menu) by default
  userscript_draw_lower = 0; // disable drawing of lower menu by default
  userscript_options = 0; 
  userscript_showoptions = (flag_use_legend ? 12 : 14) - userscript_toptions;
  userscript_selected_val = 0;
  userscript_top = 0;
  
  userscript_readtxtneeded = 1; // now
  //userscript_readfolderneeded = 1; // now
  
  sprintf(script_workfldr, "%s%s%s/%s%s", basefolder, folder_scripts, (LCS ? "LCS" : "VCS"), script_subfldrs, (script_subfldrs[0] == 0x00) ? "" : "/");
  
  flag_userscripts = 1; // only set here!
  return 0;
}

int userscripts_draw() { // this is the worst code and I'm not proud of it
  
  int i;
  float x, y;
  char buffer[256];

  /// draw title  
  drawString(translate_string("User Scripts"), ALIGN_FREE, FONT_DIALOG, SIZE_BIG, SHADOW_OFF, 8.0f, 5.0f, COLOR_USERCHEATS);
  
  /// draw folder  
  if( script_subfldrs[0] != 0x00 ) {
    snprintf(buffer, sizeof(buffer), "/%s", script_subfldrs);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 120.0f, 7.0f, COLOR_TEXT);
  }
  
  /// check folder and files
  userscript_options = countFilesInFolder(script_workfldr) + countFoldersInFolder(script_workfldr); // with folder support
  
    // no longer needed as we auto-create the path and folder
    /* if( userscript_options == 0x80010002 ) { // no folder (doesn't work like this anymore since + countFolder... anyways)
      drawString("No user scripts folder found!", ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
      snprintf(buffer, sizeof(buffer), "Create '%s%s%s/' and place .txt there", basefolder, folder_scripts, (LCS ? "LCS" : "VCS"));
      drawString(buffer, ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + userscript_row_spacing, COLOR_TEXT);
      return -1; // no cheats folder
    } */
    
    if( userscript_options == 0 ) { // folder found but empty
      drawString(translate_string("No UserScript files found!"), ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_TEXT);
      snprintf(buffer, sizeof(buffer), translate_string("Place .txt files to '%s%s%s/'"), basefolder, folder_scripts, LCS ? "LCS" : "VCS");
      drawString(buffer, ALIGN_CENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + userscript_row_spacing, COLOR_TEXT);
      return -1; // no cheats folder
    }

  x = 40.0f; // horizontal menu start
  y = 35.0f; // vertical menu start
  
  /// draw UI 
  drawUiBox(x-5.0f, y-2.0f, 410.0f, flag_use_legend ? 190.0f : 224.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // main
    
  
  /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 
  
  if( userscript_options > 0 )
    userscript_draw_lower = 1;
  
  userscript_showoptions = (flag_use_legend ? 12 : 14) - userscript_toptions; // reset
        
  if ( userscript_showoptions > userscript_options ) 
    userscript_showoptions = userscript_options; // adjust showoptions (there can't be more showoptions than options itself)
        

  /// draw Scrollbar
  if( (userscript_options > userscript_showoptions) && userscript_draw_lower ) { // draw only if there are more options than can be displayed
    float scrollbar_x = 455.0f;
    float scrollbar_y = 33.0f + ( userscript_toptions ? ((userscript_row_spacing * userscript_toptions) + 8.0f) : 0.0f);
    float scrollbar_bg_width = 5.0f;
    float scrollbar_bg_height = (flag_use_legend ? 190.0f : 224.0f) - ( userscript_toptions ? ((userscript_row_spacing * userscript_toptions) + 8.0f) : 0.0f);
    float scrollbar_cursor_height = scrollbar_bg_height * ((float)userscript_showoptions/(float)(userscript_options-1)); // 1.0 wenn alles sichtbar  0.2 bei 20% sichtbar  
    float scrollbar_cursor_y = scrollbar_y + ((scrollbar_bg_height - scrollbar_cursor_height) / 100.0f * (((float)userscript_top) * 100.0f / ((float)(userscript_options-userscript_showoptions))) ); /// scroll only when entries move (like it should be)

    if( userscript_top + userscript_showoptions >= userscript_options-1 ) // when last item is visible -> cursor must be at bottom
      scrollbar_bg_height -= 0.01f; // fix because of blit bug when two rectangles on same position
    
    drawBox(scrollbar_x, scrollbar_y, scrollbar_bg_width, scrollbar_bg_height, COLOR_BACKGROUND); // background 
    drawBox(scrollbar_x, scrollbar_cursor_y, scrollbar_bg_width, scrollbar_cursor_height, COLOR_SCROLLBAR); // cursor 
  }  

  int counter = 0;  
      
  /// draw the lower Menu
  if(userscript_draw_lower) {

    if ( userscript_selected_val < userscript_top ) 
      userscript_top = userscript_selected_val;  // for scrolling up with selection  
     
    SceIoDirent d_dir;
    memset(&d_dir,0,sizeof(SceIoDirent)); // prevents a crash
    SceUID fd = sceIoDopen(script_workfldr);
    
    if(fd >= 0)  {
      while((sceIoDread(fd, &d_dir) > 0) && counter <= userscript_top) { // skip (auto-scroll)
        counter++;
      }
      
      for(i = userscript_top; i < userscript_showoptions + userscript_top; i++, y += userscript_row_spacing) {
    //  do { // skip folders (old)
    //    sceIoDread(fd, &d_dir);
    //  } while( FIO_SO_ISDIR(d_dir.d_stat.st_attr) );
        do { // skip "." entries
          sceIoDread(fd, &d_dir);
        } while(d_dir.d_name[0] == '.');
      
        memcpy(extension, d_dir.d_name + strlen(d_dir.d_name) - 4, 5);
        strcpy(filename, d_dir.d_name); // save filename
            
        #ifdef DEBUG
        if( flag_draw_DBGVALS ) {
          ///draw number
          snprintf(buffer, sizeof(buffer), "%d", i+1);
          drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x-10.0f, y+2.0f, RED);
        }
        #endif
                
        if ( i == userscript_selected_val && userscript_selector == 0) {
          /// save stuff
          strcpy(currentfile, filename);
          strcpy(currentexten, extension);
          
          /// draw cursor
          drawBox(x-5.0f, y, 410.0f, 16.0f, COLOR_CURSOR); // float x, float y, float width, float height, u32 color
        }
    
        /// draw name  
        COLOR_TEMP = COLOR_CHEAT_OFF;
        if( stricmp(extension, ".txt") ) {
           
          if( d_dir.d_stat.st_attr & FIO_SO_IFDIR ) { // check folder
            
            snprintf(buffer, sizeof(buffer), "%s%s", script_workfldr, filename);
            if( countFilesInFolder(buffer) > 0 || (countFoldersInFolder(buffer) > 0) )
              COLOR_TEMP = WHITE;
            
            else // folder is empty
              COLOR_TEMP = GREY;
            
            sprintf(filename, "%s/", filename); // add slash because looks nicer
            
          } else   
            COLOR_TEMP = GREY; // not a valid txt file (grey out)
          
        } else
          filename[strlen(filename)-4] = '\0'; // remove .txt from displayed name
        
        snprintf(buffer, sizeof(buffer), "%s", filename); 
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_TEMP);      
      }
    
      sceIoDclose(fd);
    }
    
  } 
  
  /// error checks of selected file
  if( !stricmp(currentexten, ".txt") ) { // check for txt
  
    /// read in meta data
    if( userscript_readtxtneeded || (userscript_waittime > 0 && getGametime() > userscript_waittime) ) {
      
      resetMeta();
      userscript_waittime = 0;
      
      snprintf(buffer, sizeof(buffer), "%s%s", script_workfldr, currentfile);
      SceUID file = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
      if(file >= 0) {
        
        char readbuf[SCRIPTLINELGT]; // SCRIPTLINELGT is max line length
        int linelimiter = 10; // only search for tags in these first lines (to minimize lag)
        
        /// read in line by line /////////////////////////////////////////////////////////////////
        while( _fgets(readbuf, SCRIPTLINELGT, file) && linelimiter-- > 0 ) { // read txt line by line
          
          char *linehandle = readbuf; // create ptr to work with
          
          /// remove preceding whitespaces and tabs
          while( (*linehandle == ' ') || (*linehandle == '\t') ) 
            ++linehandle;
          
          /// check comment
          if( (linehandle[0] == '/' && linehandle[1] == '/') || linehandle[0] == '#' ) { // check first char is #
            
            /// skip all comment slashes
            while( (*linehandle == '/') || (*linehandle == '#') ) 
              ++linehandle;
            
            char *temphandle = linehandle;
            
            /// check "Author:" comment
            temphandle = strstr(linehandle, "Author:");
            if( temphandle != NULL ) {
              strncpy(meta_author, temphandle+=7, META_AUTH_SIZE);
              meta_category[META_AUTH_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Version:" comment
            temphandle = strstr(linehandle, "Version:");
            if( temphandle != NULL ) {
              strncpy(meta_version, temphandle+=8, META_VERS_SIZE);
              meta_category[META_VERS_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Category:" comment
            temphandle = strstr(linehandle, "Category:");
            if( temphandle != NULL ) {
              strncpy(meta_category, temphandle+=9, META_CATG_SIZE);
              meta_category[META_CATG_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Date:" comment
            temphandle = strstr(linehandle, "Date:");
            if( temphandle != NULL ) {
              strncpy(meta_date, temphandle+=5, META_DATE_SIZE);
              meta_date[META_DATE_SIZE-1] = '\0';
              continue;
            }
            temphandle = strstr(linehandle, "Time:");
            if( temphandle != NULL ) {
              strncpy(meta_date, temphandle+=5, META_DATE_SIZE);
              meta_date[META_DATE_SIZE-1] = '\0';
              continue;
            }
            
            /// check "Description:" comment
            temphandle = strstr(linehandle, "Description:");
            if( temphandle != NULL ) {
              strncpy(meta_description, temphandle+=12, META_DESC_SIZE);
              meta_description[META_DESC_SIZE-3] = '.';
              meta_description[META_DESC_SIZE-2] = '.';
              meta_description[META_DESC_SIZE-1] = '\0';
              continue;
            }
            
          }
        }
        
        //sprintf(meta_description, "time = %i", getGametime());

        sceIoClose(file);
        userscript_readtxtneeded = 0;
        
      } else { //could not open file
        //userscript_readfolderneeded = 1; // re-trigger folder read
      } 
      
    }
    
  }
  
  
  #ifdef DEBUG
  if( flag_draw_DBGVALS ) {      
    snprintf(buffer, sizeof(buffer), "countFilesInFolder = %d", countFilesInFolder(script_workfldr) );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 30.0f, RED);
  
    snprintf(buffer, sizeof(buffer), "countFoldersInFolder = %d", countFoldersInFolder(script_workfldr) );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 40.0f, RED);
    
    
    snprintf(buffer, sizeof(buffer), "userscript_waittime = %d", userscript_waittime );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 55.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "gametime = %d", getGametime() );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 70.0f, RED);

    
    snprintf(buffer, sizeof(buffer), "counter = %d", counter );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 120.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "userscript_top = %d", userscript_top );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 140.0f, RED);
        
    snprintf(buffer, sizeof(buffer), "userscript_selected_val = %i", userscript_selected_val );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 160.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "userscript_options = %i", userscript_options );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 180.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "userscript_showoptions = %i", userscript_showoptions );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 200.0f, RED);

    //snprintf(buffer, sizeof(buffer), "userscript_draw_lower = %i", userscript_draw_lower );
    //drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 220.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "scripts_folder = %s%s%s/", basefolder, folder_scripts, (LCS ? "LCS" : "VCS")); // "../SCRIPTS/xCS/"  
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 220.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "script_workfldr = %s", script_workfldr );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 235.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "currentfile = %s", currentfile );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 250.0f, RED);
  }
  #endif
  
  
  /// draw Legend box
  if( flag_use_legend ) {
    drawLegendBox(3, COLOR_BACKGROUND);
    
    /// draw button legend
    //drawLegendMessage("L+UP/DOWN: Toggle Menu",      0, 2, COLOR_TEXT); // left side, first row
    //drawLegendMessage("UP/DOWN: Navigate Menu",      1, 2, COLOR_TEXT); // right side, first row
    //drawLegendMessage("LEFT/RIGHT: Navigate files",  0, 1, COLOR_TEXT); // left side, second row
    //drawLegendMessage("TRIANGLE: Open in Editor",    1, 1, COLOR_TEXT); // right side, second row
    //drawLegendMessage("CROSS: Enable/Disable Cheat", 0, 0, COLOR_TEXT); // left side, third row
    //drawLegendMessage("CIRCLE: Exit to Menu",        1, 0, COLOR_TEXT); // right side, third row
    
    
    /// draw data instead
    snprintf(buffer, sizeof(buffer), "%s%s", script_workfldr, currentfile);
    //drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 440.0f, 10.0f, RED);
    if( doesFileExist(buffer) == 1 ) { // its a file
      if( !stricmp(currentexten, ".txt") ) { // and its a txt
        snprintf(buffer, sizeof(buffer), translate_string("Author: %s"), meta_author );
        drawLegendMessage(buffer, 0, 2, COLOR_TEXT); // left side, first row
          
        snprintf(buffer, sizeof(buffer), translate_string("Version: %s"), meta_version );
        drawLegendMessage(buffer, 1, 2, COLOR_TEXT); // right side, first row
          
        snprintf(buffer, sizeof(buffer), translate_string("Category: %s"), meta_category );
        drawLegendMessage(buffer, 0, 1, COLOR_TEXT); // left side, second row
          
        snprintf(buffer, sizeof(buffer), translate_string("Date: %s"), meta_date );
        drawLegendMessage(buffer, 1, 1, COLOR_TEXT); // right side, second row
          
        snprintf(buffer, sizeof(buffer), translate_string("Description: %s"), meta_description );
        drawLegendMessage(buffer, 0, 0, COLOR_TEXT); // left side, third row
          
      } else drawLegendMessage(translate_string("This is not a valid .txt file!"), 0, 2, COLOR_TEXT); // left side, first row
    
    } else { //its a folder
      if( countFilesInFolder(buffer) > 0 || (countFoldersInFolder(buffer) > 0)) {
        drawLegendMessage(translate_string("CROSS: Enter Folder"),    1, 2, COLOR_TEXT); // right side, first row
        drawLegendMessage(translate_string("TRIANGLE: Leave Folder"), 1, 1, COLOR_TEXT); // right side, second row
        drawLegendMessage(translate_string("CIRCLE: Exit to Menu"),   1, 0, COLOR_TEXT); // right side, third row
          
      } else {
        drawLegendMessage(translate_string("This folder is empty!"),  0, 2, COLOR_TEXT); // left side, first row
      }
    }
  }
  
  return 0;
}

int userscripts_ctrl() {
  if( hold_buttons & PSP_CTRL_UP ) {
    if( userscript_selected_val <= 0 ) {
    } else userscript_selected_val -= 1;  
    
    if( userscript_top > 1 ) { // scroll
      if( userscript_selected_val == 1 ) 
        userscript_top--;
    }

    //userscript_readtxtneeded = 1; // instant!
    userscript_waittime = getGametime() + userscript_waitdelay;
    resetMeta();
  }   
    
  if( hold_buttons & PSP_CTRL_DOWN ) {
    if( userscript_showoptions == 1 ) { // handling 1 option only
      if( userscript_selected_val >= userscript_options-1 ) 
        userscript_selected_val = userscript_options-1; // selection = 0;
      else 
        userscript_selected_val += 1;
      
      userscript_top = userscript_selected_val;
      
    } else {
      if ( userscript_selected_val >= userscript_options-1 ) 
        userscript_selected_val = userscript_options-1; // selection = 0;
      
      else {
        userscript_selected_val += 1;
        
        if( userscript_top + userscript_showoptions < userscript_options ) { // scroll
          if( userscript_selected_val >= userscript_top + userscript_showoptions-1 ) 
            userscript_top++; 
        }
      }  
      if( userscript_top + userscript_showoptions < userscript_options ) { // scroll
        if( userscript_selected_val == userscript_top + userscript_showoptions-1 ) 
          userscript_top++; 
      }
    }
    
    // userscript_readtxtneeded = 1;
    userscript_waittime = getGametime() + userscript_waitdelay; // about 0.5 sec
    resetMeta();
  }
  
  if( pressed_buttons & PSP_CTRL_TRIANGLE ) {
    snprintf(buffer, sizeof(buffer), "%s%s%s/", basefolder, folder_scripts, (LCS ? "LCS" : "VCS")); // "../CHEATS/xCS/"
    if( strcmp(buffer, script_workfldr) != 0 || script_subfldrs[0] != 0x00) { // we are not yet back in root..
  
      /// remove last folder from working path string 
      int i; 
      for(i = strlen(script_subfldrs); i >= 0; i--) {
        if( script_subfldrs[i] == '/' ) {
          script_subfldrs[i] = 0x00;
          break;
        } else script_subfldrs[i] = 0x00;
      }
      sprintf(script_workfldr, "%s%s%s", buffer, script_subfldrs, (script_subfldrs[0] == 0x00) ? "" : "/");

    } else { // reset to be save
      memset(script_subfldrs, 0, sizeof(script_subfldrs)); // clear
      sprintf(script_workfldr, "%s", buffer); 
    }
  
    userscript_selected_val = 0;
    userscript_top = 0;
  }
    
    
    if( pressed_buttons & PSP_CTRL_CROSS ) { // toggle cheat
      snprintf(buffer, sizeof(buffer), "%s%s", script_workfldr, currentfile);
      if( doesFileExist(buffer) != 1 && (countFilesInFolder(buffer) > 0 || (countFoldersInFolder(buffer) > 0)) ) {  // check folder + not empty
        sprintf(script_subfldrs, "%s%s%s", script_subfldrs, (script_subfldrs[0] == 0x00) ? "" : "/", currentfile);
        sprintf(script_workfldr, "%s%s%s/%s/", basefolder, folder_scripts, (LCS ? "LCS" : "VCS"), script_subfldrs);
        
        userscript_selected_val = 0;
        userscript_top = 0;
        
        return 0;
      }
      
      /***************************************************************
      The following code was more of a PoC and just continued on..
      
      Its neither good nor should it be executed in this function. It 
      really is very work in progress and contains bugs most probably.
      
      Yes, it somehow works but please do not take it as an example as 
      this is not how it should be done! :D
      ****************************************************************/  
      if( !stricmp(currentexten, ".txt") ) {
        closeMenu();  
        
        #if defined(LOG) || defined(USERSCRIPTLOG)
        logPrintf("UserScript: %s\n", currentfile);
        #endif
        
        // // // // // // // SETTINGS // // // // // // // // // // // // // 
        extern int global_MainScriptSize;
        extern int global_ScriptSpace;
        extern int global_LargestMissionScriptSize;

        int placeholder = 0xDEADB000; // base (+1 for each)
        int customtextcounter = 0;
       
        
        /************
        - inside script space
        - at the very end minus SCRIPT_SIZE !
        - 
        ***************/
        
        /// relative offset where script will be placed to MAIN = 0 (important for calculating label jumps!)
        int roff = getInt(global_MainScriptSize + (LCS ? 0 : gp)) + getInt(global_LargestMissionScriptSize + (LCS ? 0 : gp)) - SCRIPT_SIZE; 
        //logPrintf("roff: 0x%08X", roff);
        
        /// physical address where script will be placed
        int addr = getInt(global_ScriptSpace + (LCS ? 0 : gp)) + roff; 
        //logPrintf("addr: 0x%08X", addr);
        

        // // // // // // // // // // // // // // // // // // // // // // //
        
        snprintf(buffer, sizeof(buffer), "%s%s", script_workfldr, currentfile);
        SceUID file = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
        if( file >= 0 ) {
          
          CustomScriptClear(addr, SCRIPT_SIZE); // zero script area in ScriptSpace
          
          int i, j, k, opcode = -1;
          u8 script[SCRIPT_SIZE]; // todo size
          for(i = 0; i < SCRIPT_SIZE; i++) // poorly memset
            script[i] = 0;
          
          int pos = 0, line = 0, tempint;
          float tempflt;
          char tempbuf[16];
          char readbuf[SCRIPTLINELGT]; // SCRIPTLINELGT is max line length
        
          char label_ch_arr[SUPPORT_LABEL][MAX_LABEL_LENGTH];
          for(i = 0; i < SUPPORT_LABEL; i++) // poorly memset label_ch_arr
            label_ch_arr[i][0] = 0;
          int label_pos_arr[SUPPORT_LABEL] = {0};
          int label_pos_cur = 0;  
          char unk_label[SUPPORT_LABEL][MAX_LABEL_LENGTH]; // save unknown labels (placeholder = placeholderbase + array offset)
          for(i = 0; i < SUPPORT_LABEL; i++) // poorly memset unk_label
            unk_label[i][0] = 0;
          int unk_label_pos_cur = 0;
          int lastif = -1, opcodessinceif = -1;
          //int blockcomment = 0;
          
          /// read in line by line /////////////////////////////////////////////////////////////////
          while( _fgets(readbuf, SCRIPTLINELGT, file) ) { // read txt line by line
            line++;
            
            char *linehandle = readbuf; //create ptr to work with
          
            /// remove preceding whitespaces and tabs
            while( (*linehandle == ' ') || (*linehandle == '\t') ) 
              ++linehandle;
            
            
            /// skip comment lines
            if( linehandle[0] == '/' && linehandle[1] == '/' ) 
              continue;
            
            /// block comments (needs more checks.. maybe later)
            /*if( linehandle[0] == '/' && linehandle[1] == '*' ) { // start
              blockcomment = 1;
              continue;
            }
            if( linehandle[0] == '*' && linehandle[1] == '/' ) { // end
              blockcomment = 0;
              linehandle+=2; // continue after
            }
            if( blockcomment ) // we are inside a commented out block
              continue;
            */

            /// error exit - script too big
            if( pos >= SCRIPT_SIZE ) {
              snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Script exceeds currently supported size! Sorry"));
              setTimedTextbox(buffer, 7.00f); //
              sceIoClose(file);
              goto scripterror_exit;
            }  
                
            /// clean string
            for(i = 0; i < SCRIPTLINELGT-1; i++) {
              
              /// replace tabs
              if( readbuf[i] == '\t' ) 
                readbuf[i] = ' ';
              
              /// remove following comments 
              if( readbuf[i] == '/' && readbuf[i+1] == '/' ) 
                readbuf[i] = 0;
              
              /// remove new line            
              if( readbuf[i] == '\r' || readbuf[i] == '\n') { 
                readbuf[i] = 0;
              }
              
            }
             
            /// check type of row
            if( linehandle[4] == ':' && linehandle[5] == ' ' ) { // operation (eg: "024C: request_model 172")            
              opcodessinceif++;
              int counter = -1, isfuncall = -1;
              char *saveptr;
              char *token = strtok_r(linehandle, " ", &saveptr); // get the first token
              while( token != NULL ) { // walk through other tokens
                // // // // // // // // // // // // // // // 
                
                /// opcode
                if( counter == -1) {
                  token[4] = 0; // zero out ":"
                  sprintf(tempbuf, "0x%s", token);
                  opcode = strtol(tempbuf, NULL, 0);
                  
                  #if defined(LOG) || defined(USERSCRIPTLOG)
                  logPrintf("opcode: 0x%04X", opcode);
                  #endif  
                  
                  script[pos++] = opcode % 0x100;
                  script[pos++] = opcode >> 8;
                  
                  /// special case (for later)
                  if( opcode == 0x05AE || opcode == 0x05AF || // LCS
                    opcode == 0x037A || opcode == 0x037B ) { // VCS
                    isfuncall = 3; // first 3 vals without identifier (like 06 for int XX XX XX XX)
                  } else isfuncall = 0;
                }
                
                /// rest are args or text
                else {
                  
                  if( token[0] != '\0' ) {
                    
                    #if defined(LOG) || defined(USERSCRIPTLOG)
                    logPrintf("arg_%i: '%s'", counter, token);
                    #endif  
                    
                    if( token[strlen(token)-1] == ')' ) { // "x(y,z)" eg.: $3922(38@,10) 

                      /******
                      0@    -> 6C
                      14@   -> 7A
                      $400  -> E7 90
                      $1414 -> EB 86 
                      $1446 -> EB A6
                      $4147 -> F6 33
                      
                      inner 
                      0@ -> 00
                      1@ -> 01
                      ...
                      
                      0 -> 00
                      5 -> 05
                      ...
                      ********/
                      
                      for( i = 0; i < strlen(token); i++ ) {
                        if( token[i] == '(' ) token[i] = ' ';
                        if( token[i] == ')' ) token[i] = 0;
                        if( token[i] == ',' ) token[i] = ' ';
                      } //logPrintf("token '%s' found ", token);
                      
                      char *saveptrsub = NULL;
                      int subcounter = 0, tempint = 0;
                      char *utoken = strtok_r(token, " ", &saveptrsub); // get the first sub token
                      while( utoken != NULL ) { // walk through other sub tokens
                        //logPrintf("utoken '%s'", utoken);
                        
                        if( utoken[strlen(utoken)-1] == '@' ) { // "..@" local variable (eg: 0@ 1@ 2@)                         
                          
                          utoken[strlen(utoken)-1] = 0; // remove the @
                          tempint = strtol(utoken, NULL, 0); // convert 
                          
                          if( subcounter == 0 ) { // OUTSIDE  X@( , )
                            script[pos++] = (u8)tempint + (LCS ? 0x6C : 0x6D); // because 0@ is 0xC (for LCS)
                          } else { // INSIDE  (X@, )
                            script[pos++] = (u8)tempint;
                          }
                          
                        } else if( utoken[0] == '$' ) { // "$..." global variables (eg: $4028)
                          utoken++; // remove the $
                          tempint = strtol(utoken, NULL, 0); // convert

                          int adr = (int)&tempint; //fugly
                      
                          //eg LCS: $1414  -> 0x0586 -> EB 86 in script  
                          script[pos++] = (*(unsigned char*)(adr+1)) + 0xE6; // vcs todo
                          script[pos++] = *(unsigned char*)adr; //
                          
                        } else { //must be simple int then
                          tempint = strtol(utoken, NULL, 0); // convert 
                          script[pos++] = (u8)tempint;
                        }
                        
                        subcounter++;
                        utoken = strtok_r(NULL, " ", &saveptrsub);
                      }
                      
                      //snprintf(buffer, sizeof(buffer), "~r~Error: '%s' in line %i not yet supported!", token, line);
                      //setTimedTextbox(buffer, 7.00f); //
                      //sceIoClose(file);
                      //goto scripterror_exit;
                    
                    } else if( token[0] == '$' ) { // "$...." global variables (eg: $4028, $PLAYER_CHAR)
                      
                      /** LCS *** (PSP) *******************
                      533=PLAYER_SKIN    0x215  -> 0x15CE
                      536=PLAYER_CHAR    0x218  -> 0x18CE
                      540=PLAYER_ACTOR   0x21C  -> 0x1CCE
                      560=ONMISSION      0x230  -> 0x30CE
                      
                      500          0x1F4  -> 0xF4CD
                      512          0x200  -> 0x00CE
                      4028         0xFBC  -> 0xBCDB
                      ************************************/
                      
                      /** VCS *** (PSP) *******************
                      782=PLAYER_CHAR   0x30E  -> 0x0ED0 
                      789=ONMISSION     0x315  -> 0x15D0  D0 15
                      
                      500          0x1F4  -> 0xF4CE  CE F4
                      ************************************/
                      token++; //remove the $
                      if( token[0] == '_') 
                        token++;
                      
                      int adr = 0;

                      if( strcmp(token, "PLAYER_SKIN") == 0 ) { // LCS only
                        tempint = 533;
                  
                      } else if( strcmp(token, "PLAYER_CHAR") == 0 ) {
                        tempint = (LCS ? 536 : 782);
                        if( VCS && mod_text_size == 0x00377D30 ) tempint = 759; // for ULUS v1.01
                        if( LCS && mod_text_size == 0x00320A34 ) tempint = 534; // for ULUS v1.02
                  
                      } else if( strcmp(token, "PLAYER_ACTOR") == 0 ) { // LCS only
                        tempint = 540;
                  
                      } else if( strcmp(token, "ONMISSION") == 0 ) {
                        tempint = (LCS ? 560 : 789);
                        if( VCS && mod_text_size == 0x00377D30 ) tempint = 766; // for ULUS v1.01
                  if( LCS && mod_text_size == 0x00320A34 ) tempint = 558; // for ULUS v1.02
                  
                      } else {
                        tempint = strtol(token, NULL, 0); // convert 
                      }
                      //logPrintf("-> %i", tempint);
                      
                      adr = (int)&tempint; //fugly
                      
                      //eg LCS: $500  -> 0x01F4  -> 0xF4CD in script  
                      script[pos++] = (*(unsigned char*)(adr+1)) + (LCS ? 0xCC : 0xCD);  // 0x01 + 0xCD = 0xCE
                      script[pos++] = *(unsigned char*)adr;                // 0xF4  
                      
                      
                    } else if ( token[0] == '#' ) { //"#...."  alias
                      
                      /// TODO
                      //eg: " 024C: request_model #SCRIPT_SALCHAIR "
                      
                      snprintf(buffer, sizeof(buffer), translate_string("~r~Error: '%s' in line %i not yet supported!"), token, line);
                      setTimedTextbox(buffer, 7.00f); //
                      sceIoClose(file);
                      goto scripterror_exit;
                      
                    } else if ( token[0] == '@' ) { // "@....." (eg: @RAYS4_15305 )
                      token++; // remove the @
                      
                      //logPrintf("Label = '%s'", token);
                      
                        /// error 
                        if ( VCS && strlen(token) < 3 ) { // labels with less than 3 chars sometimes crash the game
                          snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Label '%s' in line %i needs to be 3 or more chars!"), token, line);
                          setTimedTextbox(buffer, 7.00f); //
                          sceIoClose(file);
                          goto scripterror_exit;
                        }
                      
                      int adr = 0, tempint = 0;

                      /// if already in saved label array
                      for(i = 0; i < SUPPORT_LABEL; i++) { // loop saved labels
                        if ( strcmp(label_ch_arr[i], token) == 0) {
                          //tempint = 0 - label_pos_arr[i];   // write saved address from array (old.. negative offset like mission script works)
                          tempint = roff + label_pos_arr[i];   // write saved address from array
                          //logPrintf("%s found in array at position %i", token, i);
                        } 
                      }
                      if( tempint == 0 ) { // not found
                      
                        /// check if this is a script name in MAIN maybe //////////////////// experimental ////////////////
                        int address = 0;
                        int main_size = getInt(global_MainScriptSize + (LCS ? 0 : gp)); 
                        int script_space = getInt(global_ScriptSpace + (LCS ? 0 : gp)); 
                        for(address = script_space; address < script_space + main_size; address++) {
                          if( *(u8*)address == token[0] && *(u8*)(address+1) == token[1] ) { // first because faster
                            //logPrintf("'%s' vs '%s'", token, getString(address, 0));
                            if( strcmp(token, getString(address, 0)) == 0 ) {
                              #if defined(LOG) || defined(USERSCRIPTLOG)
                              logPrintf("string '%s' found @ 0x%08X in mainscript", token, address - script_space);
                              #endif
                              
                              tempint = address - script_space - 2; // 2 is opcode length
                              if( VCS ) tempint--; // vcs has additional 0xA as string identifier
                              
                              goto skip_found;
                            }
                          }
                        }
                        /////////////////////////////////////////////////////////////////////////////////////////////////
  
                        tempint = placeholder + unk_label_pos_cur; // write incremented placeholder
            
                        /// check if label already in unknown array maybe
                        int in_unk_arr = 0;
                        for(i = 0; i < SUPPORT_LABEL; i++) { // loop unknown labels
                          if ( strcmp(unk_label[i], token) == 0) {
                            in_unk_arr = 1;
                            tempint = placeholder + i; // overwrite placeholder position 
                            //logPrintf("%s found in array at position %i", token, i);
                            break;
                          } 
                        }
                      
                        /// save label in unknown array (if not in already)
                        if( !in_unk_arr && (unk_label_pos_cur < SUPPORT_LABEL) ) {
                          sprintf(unk_label[unk_label_pos_cur], "%s", token);
                          unk_label_pos_cur++;
                        }
                      }
                      //logPrintf("write 0x%08X", tempint);  
                      
                      skip_found:
                      
                        adr = (int)&tempint; // fugglyyy
                  
                        /// write to script
                        script[pos++] = 0x06; // integer (4 Bytes)
                        script[pos++] = *(unsigned char*)adr;
                        script[pos++] = *(unsigned char*)(adr+1);
                        script[pos++] = *(unsigned char*)(adr+2);
                        script[pos++] = *(unsigned char*)(adr+3);
                    
                    } else if( token[strlen(token)-1] == '@' ) { // "....@" local variable (eg: 0@ 1@ 2@ ... decimal!)
                      
                      /** LCS ***
                      0x0C = 0@
                      0x0D = 1@
                      0x0E = 2@
                      ...
                      ***********/
                      token[strlen(token)-1] = 0; // remove the @
                      tempint = strtol(token, NULL, 0); // convert 
                      script[pos++] = tempint + (LCS ? 0xC : 0xD); // because 0@ is 0xC (for LCS)
                                          
                 
                    } else if( token[0] == '\'' ) { // "'xyz'" string
                      token++; // remove ' at the beginning
                      
                      char identifier[CSTGXTLGT]; // the games GXT String OR full Custom Text
                      memset(identifier, 0, sizeof(identifier)); // clear
                                            
                      if( token[strlen(token)-1] != '\'' ) { // fugly incoming again
                        sprintf(identifier, "%s", token);
                        token = strtok_r(saveptr, "\'", &saveptr); // custom text can have whitespaces.
                        sprintf(identifier, "%s %s", identifier, token);
                      } else 
                        strncpy(identifier, token, strlen(token)-1);
                      
                      #if defined(LOG) || defined(USERSCRIPTLOG)
                      logPrintf("String found: '%s'", identifier);
                      #endif
                      
                      extern u32 ptr_gxtloadadr;
                      if( strlen(identifier) > 7 || (getShort(LoadStringFromGXT(getInt(ptr_gxtloadadr + (LCS ? 0 : gp)), identifier, 2, 0xFF, 0xFF, 0, 0x00FF0000, 0x00FFFFFF)) == 0x0000) ) { // not found in GXT

                        #if defined(LOG) || defined(USERSCRIPTLOG)
                        logPrintf("not found in GXT.."); // could be custom string or special string at this point
                        #endif
                      
                        /// ignore special strings in the following opcodes (like special models eg "MAR_01") that are not in GXT
                        if( // LCS
                          opcode == 0x0161 ||  // set_zone_ped_info
                          opcode == 0x0241 ||  // load_special_character
                          opcode == 0x02E9 ||  // load_cutscene
                          opcode == 0x02EB ||  // set_cutscene_anim
                          opcode == 0x02F8 ||  // load_special_model
                          opcode == 0x0357 ||  // undress_char
                          opcode == 0x03A9 ||  // script_name
                          opcode == 0x03D4 ||  // load_mission_audio
                          opcode == 0x0505 ||  // is_player_wearing
                          opcode == 0x0663 ||  // store_player_outfit
                          
                          // VCS
                          opcode == 0x0159 ||  // load_special_character
                          opcode == 0x01D1 ||  // load_special_model
                          opcode == 0x020F ||  // undress_char
                          opcode == 0x0238 ||  // script_name
                          opcode == 0x0257 ||  // load_mission_audio
                          opcode == 0x0318 ||  // is_player_wearing
                          opcode == 0x035B     // is_player_in_info_zone
                          
                          //opcode == 0x0006
                          
                          ) { 
                          #if defined(LOG) || defined(USERSCRIPTLOG)
                          logPrintf("-> special game string! don't touch");
                          #endif
                          // do nothing
                          
                        } else { // custom string!
                          if( customtextcounter < CSTGXTS ) { // check max supported custom strings
                            
                            /// save string as ushort in array
                            int ctr = 0;
                            memset(custom_gxts[customtextcounter], 0, CSTGXTLGT);
                            while(identifier[ctr] != 0x00) {
                              custom_gxts[customtextcounter][ctr] = identifier[ctr];
                              ctr++;
                            }
                            #if defined(LOG) || defined(USERSCRIPTLOG)
                            logPrintf("-> custom string! %d (location 0x%08X)", customtextcounter, &custom_gxts[customtextcounter]);
                            #endif
                            
                            /// replace token with CUSTOM GXT LABEL
                            sprintf(identifier, "CUST_%02d", customtextcounter); // see cheats.c (name has to match obviously)
                            
                            customtextcounter++;
                          } else {
                            snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Custom strings limit of %i reached."), CSTGXTS);
                            setTimedTextbox(buffer, 7.00f); //
                            sceIoClose(file);
                            goto scripterror_exit;
                          }
                        }
                      }
                      
                      #if defined(LOG) || defined(USERSCRIPTLOG)
                      logPrintf("");
                      #endif
                      
                      /// write GXT name to script
                      if( VCS ) script[pos++] = 0x0A;
                      
                      int ctr = 0;
                      while( identifier[ctr] != 0x00 ) { //'\''
                        script[pos++] = identifier[ctr];
                        ctr++;

                        if( ctr > 7 ) { // a gxt id string can't be longer than 7 chars!!
                          snprintf(buffer, sizeof(buffer), translate_string("~r~Error: String '%s' in line %i too long!"), identifier, line);
                          setTimedTextbox(buffer, 7.00f);
                          sceIoClose(file);
                          goto scripterror_exit;
                        }
                      } 
                      if( LCS ) {
                        while( ctr < 8 ) {
                          script[pos++] = 0x00; // odd? + 00 
                          ctr++;
                        }
                      } else {
                        script[pos++] = 0x00;
                      }
                      
            
                    } else if( isdigit(token[0]) || isdigit(token[1]) ) { /// "number"
                      int adr = 0;                
                      
                      /****************************************
                      01  0             int
                      02  0.0           float
                      03  XX            float (uppest byte)    2.0   = 03 40        0x40000000
                      04  XX XX         float (upper half)    1.0    = 04 80 3F     0x3F800000
                      05  XX XX XX      float               1337.5   = 05 30 A7 44  0x44A73000
                      06  XX XX XX XX   int
                      07  XX            int  -128 to 127
                      08  XX XX         int 128 to ___
                      09  XX XX XX XX   float               3.14152  = 09 AA 0E 49 40  0x40490EAA
                      
                      0A  String VCS only?!?
                      ****************************************/
                      
                      /// contains "." then FLOAT
                      if(strstr(token, ".") != NULL) {
                        tempflt = atof(token); // convert 
                        adr = (int)&tempflt; // fugly more
                        if( *(unsigned char*)(adr) != 0 ) { // 4 Bytes needed
                          script[pos++] = 0x09; //
                          script[pos++] = *(unsigned char*)adr;
                          script[pos++] = *(unsigned char*)(adr+1);
                          script[pos++] = *(unsigned char*)(adr+2);
                          script[pos++] = *(unsigned char*)(adr+3);
                          
                        } else if( *(unsigned char*)(adr+1) != 0 ) { // 3 Bytes needed
                          script[pos++] = 0x05; //
                          script[pos++] = *(unsigned char*)(adr+1);
                          script[pos++] = *(unsigned char*)(adr+2);
                          script[pos++] = *(unsigned char*)(adr+3);
                          
                        } else if( *(unsigned char*)(adr+2) != 0 ) { // 2 Bytes needed
                          script[pos++] = 0x04; //
                          script[pos++] = *(unsigned char*)(adr+2);
                          script[pos++] = *(unsigned char*)(adr+3);
                        
                        } else if( *(unsigned char*)(adr+3) != 0 ) { // 1 Byte needed
                          script[pos++] = 0x03;
                          script[pos++] = *(unsigned char*)(adr+3);
                        
                        } else {  //0 Byte needed because value is 0.0
                          script[pos++] = 0x02;
                        }


                      /// else INTEGER
                      } else { 
                        tempint = strtol(token, NULL, 0); // convert
                        //logPrintf("tempint: '%i' 0x%08X", tempint, tempint);
                        
                        /// special case
                        if( isfuncall > 0 ) { // no identifier (like 08 for int XX XX) for 3 first args in function call opcodes
                          script[pos++] = (u8)tempint;
                          isfuncall--;
                        
                        /// normal 
                        } else {
                          adr = (int)&tempint; // fugly
                          
                          ///most common hardcoded for now (TODO!)
                          if( tempint == 0 ) {
                            script[pos++] = 0x01;
                          } else if( tempint == 1 ) {
                            script[pos++] = 0x07;
                            script[pos++] = 0x01; 
                          } else if( tempint == -1 ) {
                            script[pos++] = 0x07;
                            script[pos++] = 0xFF; 
                          } else {
                            
                            /******************************************************************
                            Examples                              Sanny:
                            0319: set_total_unique_jumps_to 0     // 19 03 01
                            0319: set_total_unique_jumps_to 1     // 19 03 07 01
                            0319: set_total_unique_jumps_to 2     // 19 03 07 02 
                            0319: set_total_unique_jumps_to 126   // 19 03 07 7E 
                            0319: set_total_unique_jumps_to 127   // 19 03 07 7F
                            0319: set_total_unique_jumps_to 128   // 19 03 08 80 00
                            0319: set_total_unique_jumps_to 129   // 19 03 08 81 00
                            0319: set_total_unique_jumps_to 254   // 19 03 08 FE 00 
                            0319: set_total_unique_jumps_to 255   // 19 03 08 FF 00          !
                            0319: set_total_unique_jumps_to 256   // 19 03 08 00 01
                            0319: set_total_unique_jumps_to 80000 // 19 03 06 80 38 01 00
                            0319: set_total_unique_jumps_to -1    // 19 03 07 FF             !
                            0319: set_total_unique_jumps_to -1000 // 19 03 08 18 FC 
                            ******************************************************************/  
                            
                            /// TODO!
                            //if( *(unsigned char*)(adr+2) != 0 ) { //4 Bytes needed
                              script[pos++] = 0x06; //
                              script[pos++] = *(unsigned char*)adr;
                              script[pos++] = *(unsigned char*)(adr+1);
                              script[pos++] = *(unsigned char*)(adr+2);
                              script[pos++] = *(unsigned char*)(adr+3);
                              
                            /*} else if( *(unsigned char*)(adr+1) != 0 ) { //2 Bytes needed
                              script[pos++] = 0x08; //
                              script[pos++] = *(unsigned char*)adr;
                              script[pos++] = *(unsigned char*)(adr+1);
                            
                            } else if( *(unsigned char*)adr != 0 ) { //1 Byte needed        eg 172 = 0xAC but sanny uses 08 ?
                              script[pos++] = 0x07; //
                              script[pos++] = *(unsigned char*)adr;
                            
                            } else { //1 Byte needed
                              script[pos++] = 0x01; //0 Byte needed because value is 0
                            }*/
                          
                          }
                        }
                      }
                      
  
                    } else { /// "keyword" (eg: and, or, radius, from, to, model, visibility.. etc)
                      
                      // most are to be ignored

                      /// constants though                      
                      if( strcmp(token, "TIMERA") == 0 ) {
                        script[pos++] = 0x0A; //
                      } else if( strcmp(token, "TIMERB") == 0 ) {
                        script[pos++] = 0x0B; //
                      } 
                      
                      /// "if and" & "if or"
                      if( opcode == 0x00DB || opcode == 0x0078 ) { // LCS & VCS 
                        opcodessinceif = -1;
                        if( strcmp(token, "and") == 0 ) {
                          script[pos++] = 0x07; // int  -128 to 127
                          script[pos++] = 0x00; // 0x00 + number of and conditions (eg 0x4 = 4)
                          #if defined(LOG) || defined(USERSCRIPTLOG)
                          logPrintf("Number of conditions will be adjusted later!");
                          #endif
                        } else if( strcmp(token, "or") == 0 ) {
                          script[pos++] = 0x07; // int  -128 to 127
                          script[pos++] = 0x14; // 0x14 + number of or conditions (eg 0x16 = 2, 0x17 = 3 ..)
                          #if defined(LOG) || defined(USERSCRIPTLOG)
                          logPrintf("Number of conditions will be adjusted later!");
                          #endif
                        }
                        lastif = pos-1;
                      }
                      if( lastif != -1 ) { // if "lastif" is not -1 then previously there was an "if and" or "if or" opcode which still needs its conditions parameter set!
                        if( opcode == 0x004C || opcode == 0x004D || opcode == 0x0021 || opcode == 0x0022 ) { // set it once we reached "goto_if_false" or "goto_if_true"
                          script[lastif] = script[lastif] + opcodessinceif; // add
                          #if defined(LOG) || defined(USERSCRIPTLOG)
                          logPrintf("Last 'if' has %d conditions and is now set!", opcodessinceif);
                          #endif
                          lastif = -1; // reset
                        }
                      }
                 
                      if( VCS && opcode == 0x0482 ) { // VCS "building_swap_for_model"
                        if( strcmp(token, "enable") == 0 ) {
                          break; // ignore "enable 1"
                        }
                      }
                      
                      // more? todo!
                      
                    }
                  } else {
                    //logPrintf("empty arg string");
                  }
                }
                
                // // // // // // // // // // // // // // // 
                token = strtok_r(NULL, " ", &saveptr);
                counter++;
              }
              
              
              /**************
              C0 04   07 02 
              BB 03   09 8B D4 8C 44   09 0C 96 8C C4   09 B4 C8 64 41   04 48 42   07 FD   07 FF 
              68 03   09 8B D4 8C 44   09 0C 96 8C C4   09 B4 C8 64 41   04 48 42   07 FE   01 
              D0 03   09 8B D4 8C 44   09 0C 96 8C C4   09 B4 C8 64 41 
              4E 00
                
                04C0: set_area_visible 2 
                03BB: swap_nearest_building_model 1126.642 -1124.689 14.299 radius 50.0 from #WAREHOUSE3Z to #IZ_VIN 
                0368: set_visibility_of_closest_object_of_type 1126.642 -1124.689 14.299 radius 50.0 model #LODEHOUSE3Z visibility 0 
                03D0: load_scene 1126.642 -1124.689 14.299 
                004E: terminate_this_script 


              4C 02   08 AC 00 
              90 03 
              54 00   CE 18   0C   0D   0E 
              75 01   CE 18   0F 
              D3 02   0C 0D 0E 10 
              A5 00   08 AC 00   0C 0D 10 11 
              7A 01   11 0F 
              6E 03   CE 18   11 
              4E 00 
              
                024C: request_model 172 
                0390: load_all_models_now 
                0054: get_player_coordinates $PLAYER_CHAR store_to 0@ 1@ 2@ 
                0175: get_player_heading $PLAYER_CHAR store_to 3@ 
                02D3: get_ground_z_for_3d_coord 0@ 1@ 2@ store_to 4@ 
                00A5: create_car 172 at 0@ 1@ 4@ store_to 5@ 
                017A: set_car_heading 5@ to 3@ 
                036E: put_player $PLAYER_CHAR in_car 5@ 
                004E: terminate_this_script
                
              **************/
              
              #if defined(LOG) || defined(USERSCRIPTLOG)
              logPrintf("--------------------------------------");
              #endif  
              
            } else if( linehandle[0] == ':') { // label (eg ":DONS4_12258")
              linehandle++; // skip the ":"

              char *token = strtok(strtok(linehandle, "  "), " "); // get rid of whitespace and tabs
              
              /// error check: SUPPORT_LABEL count
              if( label_pos_cur >= SUPPORT_LABEL ) { //label matches
                snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Too many labels. (%i is max)"), SUPPORT_LABEL);
                setTimedTextbox(buffer, 7.00f); //
                sceIoClose(file);
                goto scripterror_exit;
              }
              
              
              /// error check: label-name already in array
              for( k = 0; k < label_pos_cur; k++ ) { // loop found labels array
                #if defined(LOG) || defined(USERSCRIPTLOG)
                logPrintf("comparing for doubles: '%s' with '%s'", token, label_ch_arr[k]);
                #endif  
                if( strcmp(token, label_ch_arr[k]) == 0 ) { // label matches
                  snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Found already used label '%s' in line %i?"), token, line);
                  setTimedTextbox(buffer, 7.00f); //
                  sceIoClose(file);
                  goto scripterror_exit;
                }
                
              }
              
              /// save label-name in array and corresponding placeholder in second array
              if( label_pos_cur < SUPPORT_LABEL ) {
                sprintf(label_ch_arr[label_pos_cur], "%s", token);
                
                if( pos == 0 ) { // itsafix! mission script can't start with a :label -> write a NOP so label is at position 2 (- FE FF FF FF)
                  script[pos++] = 0x00; //
                  script[pos++] = 0x00; //
                }
                
                label_pos_arr[label_pos_cur] = pos; // save offset in array
                label_pos_cur++;
              }


            /// the following can only happen when opcode is missing (write Opcodes in Sanny is disabled)
            } else if( linehandle[0] == '$') { // variable assign opcode (eg "$544 = -144.4412")
              
              // enable Write Opcodes in Sanny
              // 04 00   DB E7   07 01   //$4071 = 1 
              
              snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Missing opcode in line %i? (%s)"), line, linehandle); // $... 
              setTimedTextbox(buffer, 7.00f); //
              sceIoClose(file);
              goto scripterror_exit;
              
              
            } else if( linehandle[strlen(linehandle)-1] == '@' ) { // calculation opcode (eg "78@ == 1")
              
              // enable Write Opcodes in Sanny
              // 39 00   2F     07 02    //35@ == 2   
              
              snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Missing opcode in line %i? (%s)"), line, linehandle); // $... 
              setTimedTextbox(buffer, 7.00f); //
              sceIoClose(file);
              goto scripterror_exit;
            
            
            } else { 
              
              //can be empty row!
              
              
              //can be a comment row!
              
              
              //can be (eg: 33@ = 1)
                //TODO
              
                /*snprintf(buffer, sizeof(buffer), "~r~Error: Missing opcode in line %i? (%s)", line, linehandle); //$... 
                setTimedTextbox(buffer, 7.00f); //
                sceIoClose(file);
                goto scripterror_exit;*/
                
              
              /// but also custom calls.. (this was a test, works for PS2 scm but not PSP)
              /*if( linehandle[0] == '!' ) {
                linehandle++;
                
                ///patch the debug menu scroll for LCS
                if( LCS && strcmp(linehandle, "patchDBGMENU") == 0 ) {
                  
                  
                  int address = 0;
                  int main_size = getInt(global_MainScriptSize + (LCS ? 0 : gp)); 
                  int script_space = getInt(global_ScriptSpace + (LCS ? 0 : gp)); 
                  for(address = script_space; address < script_space + main_size; address++) {
                    if( *(u8*)(address) == 0x4D &&
                      *(u8*)(address+15) == 0x0F &&
                      *(u8*)(address+31) == 0x07 &&
                      *(u8*)(address+52) == 0x0F ) {
                      
                      // *(u8*)(address+10) = 0x08;
                      // *(u8*)(address+0x24) = 0x08;
                        
                        
                      logPrintf("found @ 0x%08X in mainscript", address - script_space);
                      //snprintf(buffer, sizeof(buffer), "found @ 0x%08X in mainscript", address - script_space);
                      //setTimedTextbox(buffer, 7.00f); //
                      //break;
                    }
                  }

                  
                }
                //setTimedTextbox("custom call found", 7.00f); //      
              }*/

            }

          }

          sceIoClose(file);


          #if defined(LOG) || defined(USERSCRIPTLOG)
          logPrintf("\nLABELs: %i/%i", label_pos_cur, SUPPORT_LABEL);
          for( i = 0; i < SUPPORT_LABEL; i++ ) {
            if( strlen(label_ch_arr[i]) == 0 ) break;
            ///logPrintf("label_ch_arr[%i]: '%s' (pos: %i -> 0x%08X)", i, label_ch_arr[i], label_pos_arr[i], 0 - label_pos_arr[i]); // negative offsets in mission script
            logPrintf("label_ch_arr[%i]: '%s' (pos: %i -> 0x%08X)", i, label_ch_arr[i], label_pos_arr[i], roff + label_pos_arr[i]); // now positive jump offest
          }
          logPrintf("\nUNKNOWN LABELs: %i/%i", unk_label_pos_cur, SUPPORT_LABEL);
          for( i = 0; i < SUPPORT_LABEL; i++ ) {
            if( strlen(unk_label[i]) == 0 ) break;
            logPrintf("unk_label[%i]: '%s' (0x%08X)", i, unk_label[i], placeholder + i);
          }
          #endif  
        
          
          /// now fix placeholder for jumps                  
          for(i = 0; i < pos - 4; i++) { // loop script
            
            if( script[i] == 0x06) { // int with 4 Bytes
              
              for(j = 0; j < unk_label_pos_cur; j++) { // loop unknown labels array
                
                int tempint = placeholder + j;
                int adr = (int)&tempint; // fugly
                
                if( (script[i+1] == *(unsigned char*)adr) &&
                  (script[i+2] == *(unsigned char*)(adr+1)) && 
                  (script[i+3] == *(unsigned char*)(adr+2)) &&
                  (script[i+4] == *(unsigned char*)(adr+3)) ) {
                  
                  #if defined(LOG) || defined(USERSCRIPTLOG)
                  logPrintf("\nfound placeholder at script position %i", i);
                  #endif  
                  
                  /// replace placeholder
                  for(k = 0; k < label_pos_cur; k++ ) { // loop found labels array
                    #if defined(LOG) || defined(USERSCRIPTLOG)
                    logPrintf("comparing: '%s' with '%s'", unk_label[j], label_ch_arr[k]);
                    #endif  
                    
                    if( strcmp(unk_label[j], label_ch_arr[k]) == 0 ) { // label matches
                      #if defined(LOG) || defined(USERSCRIPTLOG)
                      //logPrintf("labels match: '%s' (to be replaced with; 0x%08X)", unk_label[j], 0 - label_pos_arr[k]);
                      logPrintf("labels match: '%s' (to be replaced with; 0x%08X)", unk_label[j], roff + label_pos_arr[k]);
                      #endif  
                      
                      //tempint = 0 - label_pos_arr[k]; // (old.. negative offset like mission script works)
                      tempint = roff + label_pos_arr[k];
                      adr = (int)&tempint; //more fugly
                
                      script[i+1] = *(unsigned char*)adr;
                      script[i+2] = *(unsigned char*)(adr+1);
                      script[i+3] = *(unsigned char*)(adr+2);
                      script[i+4] = *(unsigned char*)(adr+3);
                      break;
                    }
                  }  
                  
                  /// error goto label not found in Labels
                  if( tempint == placeholder + j ) { // if tempint not overwriten (ugly)
                    snprintf(buffer, sizeof(buffer), translate_string("~r~Error: Label '%s' not found?!!"), unk_label[j]);
                    setTimedTextbox(buffer, 7.00f); //
                    goto scripterror_exit;
                  }
                  i += 4;
                  break;
                }
              }
            }
          }
          
          #if defined(LOG) || defined(USERSCRIPTLOG)
          logPrintf("\nscript size: %i/%i", pos, SCRIPT_SIZE);
          #endif  
          
          CustomScriptPlace(script, addr, pos); // "addr" physical address (must be in script space though), "pos" is the length/size
          
          #if defined(LOG) || defined(USERSCRIPTLOG)
          u8 tempbuffffff[128]; // quick temp printout (which also crashes for bigger scripts)
          memset(&tempbuffffff, 0, sizeof(tempbuffffff)); 
          for( i = 0; i < (pos < 128 ? pos : 128); i++ ) // otherwise crash of emulator
            snprintf(tempbuffffff, sizeof(tempbuffffff), "%s %02X", tempbuffffff, script[i]);
          logPrintf("\nSCRIPT:%s\n\n", tempbuffffff);
          #endif
          
          CustomScriptExecute(addr);
        }
        
        // // // // // // // // // // // // // // // // // // // // 
        //snprintf(buffer, sizeof(buffer), "Success: '%s' executed!", currentfile);
        //setTimedTextbox(buffer, 7.00f); //
      }
      
      ////////////////////////////////////////////////////////////
    }
    
    if( pressed_buttons & PSP_CTRL_CIRCLE ) { // exit
      flag_userscripts = 0;
    }
  
  scripterror_exit:
  return 0;  
}
#endif





/********************************************************************************************************
*
* Editor(s)
*
*********************************************************************************************************/
#ifdef EDITORS
short flag_editor = 0;
char editor_titlebuffer[64];
const Editor_pack *editor_curmenu;

int editor_menumode  = -1;
int editor_toptions  = -1;
int editor_firstobj  = -1; // first object 
int editor_lastobj   = -1; // last object 
int editor_blocksize = -1; // size of object
int editor_blocks    = -1; // blocks available

/// editor position(s) saved globally (init with 0)
int editor_pedobj_current        = 0;
int editor_vehicleobj_current    = 0;
int editor_worldobj_current      = 0;
int editor_businessobj_current   = 0;
int editor_pickup_current        = 0;
int editor_mapicon_current       = 0;
int editor_vehiclespawn_current  = 0;
int editor_garage_current        = 0;
int editor_garageslot_current    = 0;
int editor_vehicle_current       = 0; // used for multiple
int editor_buildingsIPL_current  = 0;
int editor_treadablesIPL_current = 0;
int editor_dummysIPL_current     = 0;
int editor_carcolsDAT_current    = 0;
int editor_pedcolsDAT_current    = 0;
int editor_particleCFG_current   = 0;
int editor_pedstatsDAT_current   = 0;
int editor_weaponDAT_current     = 0;
int editor_timecycDAT_current    = 0;
int editor_ide_current           = 0; // 453 = LCS: "plnt_pipepart01" aka ocean-pipe
int editor_temp_blocksize        = 0; // special

int editor_wasused = 0; // if > 0 at least one value was changed

float editor_row_spacing = 15.f; // pixels between rows

int editor_block_current;
int editor_base_adr;
int editor_options;       // available options (set later)
int editor_showoptions;   // the number of value rows that should be displayed at once (0 to 2 possible)  
int editor_draw_lower;    // some have no data in slots -> those can be disabled here
int editor_selector;      // is 1 if cursor in topmenu --- 0 when in values
int editor_selection_top; // selected topmenu option
int editor_selection_val; // selected option in values number
int editor_top;           // the values option on top when there are more options than editor_showoptions
int editor_cur_type;      // value type
int editor_trigger;       // triggers something like teleporting for pickups / set current car for vehicle editors etc

int waittime = 0;


void optionAdjust() {
  editor_options = 0;
  editor_showoptions = (flag_use_legend ? 12 : 14) - editor_toptions; // the number of value rows that should be displayed at once (0 to 2 possible)  
  
  while( editor_curmenu[editor_options].name != NULL ) 
    editor_options++; // calculate number of editor_options
    
  if ( editor_showoptions > editor_options ) 
    editor_showoptions = editor_options; // adjust editor_showoptions (there can't be more "editor_showoptions" than "editor_options" itself)    
}

int editor_create(int mode, int toptions, const char *editortitle, Editor_pack *editor_menu, int first_obj, int block_size, int blocks) {
  #ifdef LOG
  logPrintf("[INFO] %i: editor_create() ..'%s'", getGametime(), editortitle);
  #endif
  
  sprintf(editor_titlebuffer, "Editor - %s", translate_string(editortitle));
    
  editor_menumode = mode; 
  editor_toptions = toptions;
  editor_curmenu = editor_menu;
  editor_blocksize = block_size; 
  editor_blocks = blocks; 
  editor_firstobj = first_obj; 
  editor_lastobj = editor_firstobj + (editor_blocks * editor_blocksize); 
  editor_base_adr = 0;
  editor_draw_lower = 0;
  editor_selector = 1;  
  editor_selection_top = editor_toptions-1;
  editor_selection_val = 0;
  editor_top = 0;
  editor_cur_type = 0;
  editor_trigger = 0;
  editor_wasused = 0;
  
  optionAdjust();
  
  flag_editor = 1; // only set here!
  
  return 0;
}

int editor_draw() {
  int i, str_pos;
  char buffer[128];
  char buffer_top0[128]; // top option
  char buffer_top1[128]; // top option 2
  
  void *(* func)();
  const char *val;
  
  /// color sample box
  float box_w = 200.0f;
  float box_h = 40.0f;
  u32 cur_color = 0;
  

  /// draw title
  drawString(editor_titlebuffer, ALIGN_FREE, FONT_DIALOG, SIZE_BIG, SHADOW_OFF, 8.0f, 5.0f, COLOR_EDITOR);
  
  /// draw the upper Menu    
  float x = 40.0f; // horizontal menu start
  float y = 35.0f; // vertical menu start
    
  /// draw UI 
  if( editor_toptions > 0 ) {
    float topmenuheight = 20.0f;
    if( editor_menumode == EDITOR_GARAGE)
      topmenuheight = 35.0f;
    drawUiBox(x-5.0f, y-2.0f, 410.0f, topmenuheight, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // x, y, width, height, border, color, color  
  } drawUiBox(x-5.0f, y-2.0f, 410.0f, flag_use_legend ? 194.0f : 224.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // main
    
  //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//+//+//+//
  
  switch(editor_menumode) {   
    case EDITOR_PEDOBJ: 
      editor_block_current = editor_pedobj_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getPedObjectIsActive(editor_base_adr) ) {
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i    %s: %s"), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("ID"), getPedID(editor_base_adr), translate_string("Name"), getModelNameViaID(getPedID(editor_base_adr), waittime)); // block menu(getPedModelByID was replaced)
        editor_draw_lower = 1; // there is a PED here 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("Slot"), editor_block_current+1, editor_blocks); // block menu  
        editor_draw_lower = 0;  // no active PED -> not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
        
    case EDITOR_VEHICLEOBJ: 
      editor_block_current = editor_vehicleobj_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getVehicleObjectIsActive(editor_base_adr) ){
        sprintf(buffer_top0, translate_string("%s: %i/%i      %s: %i    %s: "), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("ID"), getVehicleID(editor_base_adr), translate_string("Name")); // block menu
        
        sprintf(buffer_top1, "%s", getRealVehicleNameViaID(getVehicleID(editor_base_adr))); // buffer_top1 kurz zweckentfremden!
        if( buffer_top1[0] == '\0' ) // some vehicles don't have translations..
          sprintf(buffer_top1, "%s", getGxtIdentifierForVehicleViaID(getVehicleID(editor_base_adr))); // ..use the GXT identifier-name then
        sprintf(buffer_top0, "%s%s", buffer_top0, buffer_top1);
        
        editor_draw_lower = 1; // there is a Vehicle here 
      } else { 
        sprintf(buffer_top0, translate_string("%s: %i/%i"), translate_string("Slot"), editor_block_current+1, editor_blocks ); // block menu  
        editor_draw_lower = 0; // no active Vehicle here -> not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
    
    case EDITOR_WORLDOBJ: 
      editor_block_current = editor_worldobj_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getWorldObjectIsActive(editor_base_adr) ){ 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i    %s: %s"), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("ID"), getShort(editor_base_adr+0x58), translate_string("Name"), getModelNameViaID(getShort(editor_base_adr+0x58), waittime) ); // block menu  
        editor_draw_lower = 1; // there is a obj here 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("Slot"), editor_block_current+1, editor_blocks ); // block menu  
        editor_draw_lower = 0; // no active obj here -> not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
      
    case EDITOR_BUSINESSOBJ: 
      editor_block_current = editor_businessobj_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getWorldObjectIsActive(editor_base_adr) ){ 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i    %s: %s"), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("ID"), getShort(editor_base_adr+0x58), translate_string("Name"), getModelNameViaID(getShort(editor_base_adr+0x58), waittime) ); // block menu  
        editor_draw_lower = 1; // there is a obj here 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("Slot"), editor_block_current+1, editor_blocks ); // block menu  
        editor_draw_lower = 0; // no active obj here -> not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
      
    case EDITOR_PICKUPS: 
      editor_block_current = editor_pickup_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getPickupIsActive(editor_base_adr) ){ // getPickupIsCollectable() alternative
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i    %s: %s"), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("ID"), getPickupID(editor_base_adr), translate_string("Name"), getPickupNameByID(getPickupID(editor_base_adr))); // block menu
        editor_draw_lower = 1; 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("Slot"), editor_block_current+1, editor_blocks ); // block menu  
        editor_draw_lower = 0; // not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
      
    case EDITOR_MAPICONS: 
      editor_block_current = editor_mapicon_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getMapiconIsActive(editor_base_adr) ){
        if( getMapiconID(editor_base_adr) == 0 ) // objective (destination, enemy, etc)
          snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %s   %s: %s"), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("Icon"), getMapiconNameByID(getMapiconID(editor_base_adr)), translate_string("Type"), getMapiconTypeName(editor_base_adr) ); //block menu  
        else
          snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %s"), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("Icon"), getMapiconNameByID(getMapiconID(editor_base_adr)) ); // block menu  
        editor_draw_lower = 1; 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("Slot"), editor_block_current+1, editor_blocks ); // block menu  
        editor_draw_lower = 0; // not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;

    case EDITOR_STATS: 
      editor_block_current = 0; // not used
      break;
    
    case EDITOR_GARAGE: 
      // garage doesn't use editor_block_current but 2 globals directly
      editor_base_adr = editor_firstobj + (editor_garage_current * editor_blocksize * 4) + (editor_garageslot_current * editor_blocksize); // calc base address
    
      snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i"), translate_string("Slot"), editor_garageslot_current+1 );
      if( getGarageVehicleSlotIsActive(editor_base_adr) ){ // vehicle id for detecting if slot is used
        ///print vehicle name
        /*snprintf(buffer_top1, sizeof(buffer_top1), "%s", getRealVehicleNameViaID(getShort(editor_base_adr))); // buffer_top1 kurz zweckentfremden!
        if( buffer_top1[0] == '\0' ) // some vehicles don't have translations..
          snprintf(buffer_top1, sizeof(buffer_top1), "%s", getGxtIdentifierForVehicleViaID(getShort(editor_base_adr))); // ..use the GXT identifier-name then
        snprintf(buffer_top0, sizeof(buffer_top0), "%s     '%s'", buffer_top0, buffer_top1);*/
        editor_draw_lower = 1; // allowed
    
      } else { 
        editor_draw_lower = 0; // no active -> not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      
      snprintf(buffer_top1, sizeof(buffer_top1), translate_string("%s: %s"), translate_string("Garage"), LCS ? lcs_garagenames[editor_garage_current] : vcs_garagenames[editor_garage_current]);
      break;
        
    case EDITOR_VEHWORLDSPAWNS: 
      editor_block_current = editor_vehiclespawn_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getVehicleWorldSpawnSlotIsActive(editor_base_adr) ){ // getPickupIsCollectable() alternative
        sprintf(buffer_top0, translate_string("%s: %i/%i      %s: %i"), translate_string("Slot"), editor_block_current+1, editor_blocks, translate_string("ID"), getInt(editor_base_adr)); // block menu
        
        ///add vehicle name
        sprintf(buffer_top1, "%s", getRealVehicleNameViaID(getInt(editor_base_adr))); // buffer_top1 kurz zweckentfremden!
        if( buffer_top1[0] == '\0' ) // some vehicles don't have translations..
          sprintf(buffer_top1, "%s", getGxtIdentifierForVehicleViaID(getInt(editor_base_adr))); // ..use the GXT identifier-name then
        sprintf(buffer_top0, translate_string("%s    %s: %s"), buffer_top0, translate_string("Name"), buffer_top1);
        
        ///add custom created indicator
        if( isCustomParkedVehicleSpawnViaSlot(editor_block_current) )
          sprintf(buffer_top0, translate_string("%s    (%s)"), buffer_top0, translate_string("custom"));
          
        editor_draw_lower = 1; 
      } else { 
        sprintf(buffer_top0, translate_string("%s: %i/%i"), translate_string("Slot"), editor_block_current+1, editor_blocks ); // block menu  
        editor_draw_lower = 0; // not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
    

    //********//********//********//********//********//********//********//********//********//********//********//********//********//
    
    case EDITOR_IDE:
      editor_block_current = editor_ide_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      
      if( getInt(editor_base_adr) ){ // there is a pointer here
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: '%s'   %s: %s"), translate_string("No."), editor_block_current, editor_blocks-1, translate_string("Type"), getIdeTypeName(getByte(getInt(editor_base_adr)+0x10)), translate_string("Name"), getModelNameViaHash(getInt(getInt(editor_base_adr)+0x8), waittime)); //block menu  
        editor_draw_lower = 1; 
    
        editor_base_adr = getInt(editor_base_adr);
        char ide_type = getByte(editor_base_adr+0x10);
        editor_temp_blocksize = getIdeSlotSizeByType(ide_type); // hardcoded though
        if( ide_type == 0x01 ) // obj
          editor_curmenu = LCS ? lcs_ide_obj_menu : vcs_ide_obj_menu;
        else if( ide_type == 0x03 ) // tobj
          editor_curmenu = LCS ? lcs_ide_tobj_menu : vcs_ide_tobj_menu;
        else if( ide_type == 0x04 ) // weap
          editor_curmenu = LCS ? lcs_ide_weap_menu : vcs_ide_weap_menu;
        else if( ide_type == 0x05 ) { // hier
          //editor_curmenu = LCS ? lcs_ide_hier_menu : vcs_ide_hier_menu; // empty
          editor_curmenu = LCS ? lcs_ide_menu : vcs_ide_menu; //don't replace - use base
        } else if( ide_type == 0x06 ) // cars
          editor_curmenu = LCS ? lcs_ide_cars_menu : vcs_ide_cars_menu;
        else if( ide_type == 0x07 ) // ped
          editor_curmenu = LCS ? lcs_ide_ped_menu : vcs_ide_ped_menu;
          
        optionAdjust(); // "re-adjust"  

      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("No."), editor_block_current, editor_blocks-1 ); // block menu  
        editor_draw_lower = 0; // not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
        editor_temp_blocksize = 0x4; // size of pointer
      }
      break;
      
      
    case EDITOR_HANDLINGCFG: // multiple vehicles share the same handling slot!
        editor_block_current = editor_vehicle_current;
        
        if( editor_block_current < getFirstIdeOfType(MODELINFO_VEHICLE) )
          editor_block_current = getFirstIdeOfType(MODELINFO_VEHICLE);
      
        if( editor_block_current > getLastIdeOfType(MODELINFO_VEHICLE) )
          editor_block_current = getLastIdeOfType(MODELINFO_VEHICLE);
        
        editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
          
        if( getInt(editor_base_adr) && getByte( getInt(editor_base_adr) + 0x10 ) == MODELINFO_VEHICLE ) { // there is a pointer here AND its vehicle type
          snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i    %s: %s"), translate_string("ID"), editor_block_current, translate_string("GXT"), getGxtIdentifierForVehicleViaID(editor_block_current)); 
          snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s    %s: %s"), buffer_top0, translate_string("Name"), getRealVehicleNameViaID(editor_block_current)); // fix  
          editor_temp_blocksize = LCS ? 0xF0 : 0xE0; // todo (var_handlingcfgslotsize)
          editor_base_adr = getAddressOfHandlingSlotForID(editor_block_current);
          editor_draw_lower = 1;
        } else { 
          snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("No."), editor_block_current, editor_blocks ); // block menu  
          editor_draw_lower = 0; // not allowed to draw lower menu
          editor_selector = 1; // don't allow going to down_menu
          editor_temp_blocksize = 0x4; // back to size of pointer
          editor_block_current++;
        }
      break;
      
    case EDITOR_BUILDINGSIPL:
      editor_block_current = editor_buildingsIPL_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getShort(editor_base_adr+0x58) ){
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i   %s: %s"), translate_string("No."), editor_block_current, editor_blocks-1, translate_string("ID"), getShort(editor_base_adr+0x58), translate_string("Name"), getModelNameViaID(getShort(editor_base_adr+0x58), waittime)); //block menu  
        editor_draw_lower = 1; 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("No."), editor_block_current, editor_blocks-1 ); // block menu  
        editor_draw_lower = 0; // not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
    
    case EDITOR_TREADABLESIPL:
      editor_block_current = editor_treadablesIPL_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getShort(editor_base_adr+0x58) ){
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i   %s: %s"), translate_string("No."), editor_block_current, editor_blocks-1, translate_string("ID"), getShort(editor_base_adr+0x58), translate_string("Name"), getModelNameViaID(getShort(editor_base_adr+0x58), waittime)); //block menu  
        editor_draw_lower = 1; 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("No."), editor_block_current, editor_blocks-1 ); // block menu  
        editor_draw_lower = 0; // not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
    
    case EDITOR_DUMMYSIPL:
      editor_block_current = editor_dummysIPL_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      if( getShort(editor_base_adr+0x58) ){
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i   %s: %s"), translate_string("No."), editor_block_current, editor_blocks-1, translate_string("ID"), getShort(editor_base_adr+0x58), translate_string("Name"), getModelNameViaID(getShort(editor_base_adr+0x58), waittime)); //block menu  
        editor_draw_lower = 1; 
      } else { 
        snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("No."), editor_block_current, editor_blocks-1 ); // block menu  
        editor_draw_lower = 0; // not allowed to draw lower menu
        editor_selector = 1; // don't allow going to down_menu
      }
      break;
      
    case EDITOR_CARCOLSDAT:
      editor_block_current = editor_carcolsDAT_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("Color"), editor_block_current, editor_blocks-1); // block menu  
      editor_draw_lower = 1; 
      
      /// draw color box
      if( LCS ) cur_color = getInt(editor_base_adr);
      if( VCS ) {
        cur_color = getByte(editor_base_adr);
        cur_color += getByte(editor_base_adr+0x1) * 0x100;
        cur_color += getByte(editor_base_adr+0x2) * 0x10000;
        cur_color += 0xFF000000; // add alpha
      }
      drawBox((SCREEN_WIDTH/2) - (box_w/2), 200.0f-box_h, box_w, box_h, cur_color); // x, y, width, height, color
      break;
  
    case EDITOR_PEDCOLSDAT: //VCS only
      editor_block_current = editor_pedcolsDAT_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("Color"), editor_block_current, editor_blocks-1); // block menu  
      editor_draw_lower = 1; 
      
      /// draw color box
      cur_color = getByte(editor_base_adr);
      cur_color += getByte(editor_base_adr+0x1) * 0x100;
      cur_color += getByte(editor_base_adr+0x2) * 0x10000;
      cur_color += 0xFF000000; // add alpha
      drawBox((SCREEN_WIDTH/2) - (box_w/2), 200.0f-box_h, box_w, box_h, cur_color); // x, y, width, height, color
      break;

    case EDITOR_PARTICLECFG:
      editor_block_current = editor_particleCFG_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("No."), editor_block_current, editor_blocks-1); // block menu  
      editor_draw_lower = 1; 
      break;
      
    case EDITOR_PEDSTATSDAT:
      editor_block_current = editor_pedstatsDAT_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i"), translate_string("No."), editor_block_current, editor_blocks-1); // block menu  
      editor_draw_lower = 1; 
      break;
      
    case EDITOR_WEAPONDAT:
      editor_block_current = editor_weaponDAT_current;
      editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); // calc base address
      snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %i/%i      %s: %i   %s: %s"), translate_string("No."), editor_block_current, editor_blocks-1, translate_string("ID"), getShort(editor_base_adr+0x60), translate_string("Name"), (getShort(editor_base_adr+0x60) > 0) ? getModelNameViaID(getShort(editor_base_adr+0x60), waittime) : "Unarmed"); //block menu  
      editor_draw_lower = 1; 
      break;
     
  case EDITOR_TIMECYCDAT:
      editor_block_current = editor_timecycDAT_current;
    int weather = editor_block_current / 24;
    int time = editor_block_current % 24;
    editor_base_adr = editor_firstobj + (8 * time) + weather;
      snprintf(buffer_top0, sizeof(buffer_top0), translate_string("%s: %s   %s: %02i:00"), translate_string("Weather"), translate_string( LCS ? weather_lcs[weather] : weather_vcs[weather] ), translate_string("Time"), time); //block menu  
      editor_draw_lower = 1; 
    editor_temp_blocksize = 0x2bf0; // LCS & VCS! todo?
      break;
      
    default: break;
  }
  //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//+//+//+//

  
  /// draw block/object selection menu line(s) #ugly
  if( editor_selector == 1 ) { // in topmenu
    if ( editor_selection_top == 0 ) {
      if( editor_toptions == 1 ) {
        drawString(buffer_top0, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CHEAT_ON);
      }
      if( editor_toptions == 2 ) {
        drawString(buffer_top1, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CHEAT_OFF);
        drawString(buffer_top0, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y+editor_row_spacing, COLOR_CHEAT_ON);
      }
          
    } else {
      if( editor_toptions == 1 ) {
        drawString(buffer_top0, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CHEAT_OFF);
      }
      if( editor_toptions == 2 ) {
        drawString(buffer_top1, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CHEAT_ON);
        drawString(buffer_top0, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y+editor_row_spacing, COLOR_CHEAT_OFF);
      }
    }
  } else {
    if( editor_toptions == 1 ) 
      drawString(buffer_top0, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CHEAT_OFF);
    if( editor_toptions == 2 ) {
      drawString(buffer_top1, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_CHEAT_OFF);
      drawString(buffer_top0, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y+editor_row_spacing, COLOR_CHEAT_OFF);
    }
  }  
  
  /// draw Scrollbar
  if( (editor_options > editor_showoptions) && editor_draw_lower ) { // draw only if there are more options than can be displayed
    float scrollbar_x = 455.0f;
    float scrollbar_y = 33.0f + ( editor_toptions ? ((editor_row_spacing * editor_toptions) + 8.0f) : 0.0f);
    float scrollbar_bg_width = 5.0f;
    float scrollbar_bg_height = (flag_use_legend ? 194.0f : 224.0f) - ( editor_toptions ? ((editor_row_spacing * editor_toptions) + 8.0f) : 0.0f);
    float scrollbar_cursor_height = scrollbar_bg_height * ((float)editor_showoptions/(float)(editor_options-1));  //1.0 wenn alles sichtbar   0.2 bei 20% sichtbar  
    float scrollbar_cursor_y = scrollbar_y + ((scrollbar_bg_height - scrollbar_cursor_height) / 100.0f * (((float)editor_top) * 100.0f / ((float)(editor_options-editor_showoptions))) ); // scroll only when entries move (like it should be)

    if( editor_top+editor_showoptions >= editor_options-1 ) // when last item is visible -> cursor must be at bottom
      scrollbar_bg_height -= 0.01f; // fix because of blit bug when two rectangles on same position
    
    drawBox(scrollbar_x, scrollbar_y, scrollbar_bg_width, scrollbar_bg_height, COLOR_BACKGROUND); //background 
    drawBox(scrollbar_x, scrollbar_cursor_y, scrollbar_bg_width, scrollbar_cursor_height, COLOR_SCROLLBAR); //cursor 
  }  
    
  /// draw the lower Menu      
  if(editor_toptions) 
    y += (editor_row_spacing*editor_toptions) + 8; // adjusted y for start of values here (depending on topmenu items)  
  
  if(editor_draw_lower) {
  
    if ( editor_selection_val < editor_top ) 
      editor_top = editor_selection_val;  // for scrolling up with selection  
    
    /// work
    for(i=editor_top; i < editor_showoptions+editor_top; i++, y += editor_row_spacing) {
      if ( i == editor_selection_val && editor_selector == 0) {
        COLOR_TEMP = COLOR_CHEAT_ON;
        editor_cur_type = editor_curmenu[editor_selection_val].type;
      } else {
        COLOR_TEMP = COLOR_CHEAT_OFF;
        if( editor_curmenu[i].edit_bool == FALSE ) 
          COLOR_TEMP = LGREY; // COLOR_TEXT;
        if( editor_curmenu[i].type == TYPE_DUMMY )
          COLOR_TEMP = COLOR_TEXT;
        
        if( i == editor_top && editor_curmenu[i-1].name != NULL ) // lower alpha for first row if not first menu entry
          COLOR_TEMP = COLOR_TEMP - 0x55000000; 
          
        if( i == editor_showoptions+editor_top-1 && editor_curmenu[i+1].name != NULL ) // lower alpha for last row if not last menu entry
          COLOR_TEMP = COLOR_TEMP - 0x55000000; 
      }
      
      #ifdef DEBUG
      if( flag_draw_DBGVALS ) {
        ///draw offset
        if( editor_curmenu[i].type != TYPE_DUMMY ) {
          snprintf(buffer, sizeof(buffer), "0x%X", editor_curmenu[i].address );
          drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x-5.0f, y+2.0f, RED);
        }
      }
      #endif
        
      ///Name selected
      drawString(translate_string(editor_curmenu[i].name), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_TEMP);
      
      if( editor_curmenu[i].value != 0 ) { // use the function to take care of everything    
        func = editor_curmenu[i].value;
        val = func(FUNC_GET_STRING, 0, editor_base_adr, editor_curmenu[i].address+editor_base_adr, (int)editor_curmenu[i].steps);
        snprintf(buffer, sizeof(buffer), "%s%s", val, editor_curmenu[i].postfix );
        
      } else { // plain value editing 
        int adr = editor_curmenu[i].address+editor_base_adr;
        char pre = editor_curmenu[i].precision;    
        char* fix = editor_curmenu[i].postfix;    
        switch( editor_curmenu[i].type ) {
          case TYPE_BYTE:
            snprintf(buffer, sizeof(buffer), pre ? "%i%s" : "0x%02X%s", *(unsigned char*)(adr), fix);
            break;
          
          case TYPE_BYTE_AS_FLT:
            snprintf(buffer, sizeof(buffer), "%.*f%s", pre, (float)(*(unsigned char*)(adr)) / 10, fix); // used in timecycle for example
            break;
            
          case TYPE_SHORT:
            snprintf(buffer, sizeof(buffer), pre ? "%i%s" : "0x%X%s", *(short*)(adr), fix);
            break;
            
          case TYPE_INTEGER:
            snprintf(buffer, sizeof(buffer), pre ? "%i%s" : "0x%X%s", *(int*)(adr), fix);
            break;
          
          case TYPE_FLOAT:
            snprintf(buffer, sizeof(buffer), "%.*f%s", pre, *(float*)(adr), fix); // argument-supplied precision
            break;
            
          case TYPE_BOOL:
            /*if( *(unsigned char*)(adr) == editor_curmenu[i].max ) { // max must be the true value
              snprintf(buffer, sizeof(buffer), "TRUE");
            } else if(*(unsigned char*)(adr) == editor_curmenu[i].min ) { // min must be the false value
              snprintf(buffer, sizeof(buffer), "FALSE");
            } else snprintf(buffer, sizeof(buffer), "ERROR");*/
      if( *(unsigned char*)(adr) == 1 ) {
              snprintf(buffer, sizeof(buffer), translate_string("TRUE"));
            } else if(*(unsigned char*)(adr) == 0 ) {
              snprintf(buffer, sizeof(buffer), translate_string("FALSE"));
            } else snprintf(buffer, sizeof(buffer), translate_string("ERROR"));
            break;
            
          case TYPE_BIT:
            if( *(char*)(adr) & (1 << pre) ) {
              snprintf(buffer, sizeof(buffer), translate_string("TRUE")); // bit is set
            } else snprintf(buffer, sizeof(buffer), translate_string("FALSE"));
            break;
            
          case TYPE_NIBBLE_LOW:  // eg: 0xE6  -> 6  0110
            snprintf(buffer, sizeof(buffer), pre ? "%i%s" : "0x%X%s", *(unsigned char*)(adr) & 0xF, fix);
            break;
            
          case TYPE_NIBBLE_HIGH: // eg: 0xE6  -> E  1110
            snprintf(buffer, sizeof(buffer), pre ? "%i%s" : "0x%X%s", *(unsigned char*)(adr) >> 4, fix);
            break;
            
          case TYPE_STRING: 
            snprintf(buffer, sizeof(buffer), "'");
            for( str_pos = 0; str_pos < pre && getByte(adr+str_pos) != 0x00; str_pos++ ) {
              snprintf(buffer, sizeof(buffer), "%s%c", buffer, *(char*)(adr+str_pos));
            } snprintf(buffer, sizeof(buffer), "%s'%s", buffer, fix);
            break;
          
          case TYPE_DUMMY: 
            snprintf(buffer, sizeof(buffer), "%s", fix);
            break;
        }
      }

      drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 440.0f, y, COLOR_TEMP); // align right!!
  
      if(!editor_toptions) // no top menu so this is a small fix to fill the ui box nicer
        y += 1.0f; 

    }  
    
  } else { // not allowed to draw
    
    //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//+//+//+//
    switch( editor_menumode ) {
      case EDITOR_GARAGE: // on VCS the vehicles are loaded into the garage on spawn!!!!! (TODO - can this be checked elsewhere?)
        drawString(translate_string("No vehicle found or garage open!"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_TEXT);
        if( VCS ) drawString(translate_string("Info: Vehicles are loaded on spawn! Open & close garage once."), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y+30, COLOR_TEXT);
        break;

      case EDITOR_PEDOBJ:
      case EDITOR_VEHICLEOBJ: 
      case EDITOR_WORLDOBJ: 
      case EDITOR_BUSINESSOBJ: 
      case EDITOR_PICKUPS: 
      case EDITOR_VEHWORLDSPAWNS: 
      case EDITOR_BUILDINGSIPL: 
      case EDITOR_TREADABLESIPL: 
      case EDITOR_DUMMYSIPL:
      case EDITOR_MAPICONS: 
      case EDITOR_HANDLINGCFG: 
        drawString(translate_string("No active object in this slot!"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_TEXT);
        break;
        
      case EDITOR_IDE: 
        drawString(translate_string("No IDE here!"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, COLOR_TEXT);
        break;
                    
      default: break;
    }
    //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//+//+//+//
    
    editor_selection_val = 0; // to be sure
  }  
    
    
  #ifdef DEBUG
  if( flag_draw_DBGVALS ) {    
    snprintf(buffer, sizeof(buffer), "editor_selector = %d", editor_selector );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 80.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "editor_top = %d", editor_top );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 100.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "editor_selection_top = %d", editor_selection_top );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 120.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "editor_base_adr = 0x%08X", editor_base_adr );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 140.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "editor_wasused = %i", editor_wasused );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 160.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "editor_selection_val = %i", editor_selection_val );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 180.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "editor_options = %i", editor_options );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 200.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "editor_toptions = %i", editor_toptions );
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 220.0f, RED);
  }
  #endif
    
    
  /// draw Legend   
  if( flag_use_legend ) {
    drawLegendBox(2, COLOR_BACKGROUND); // only 2 lines in small legend mode
    
    /* if( !flag_small_legend) {
      drawLegendMessage("UP/DOWN: Select Entry", 0, 2, COLOR_TEXT); // left side, first row
      drawLegendMessage("LEFT/RIGHT: Change Value", 1, 2, COLOR_TEXT); // right side, first row
    } */
  
    //+//+// OPTION //+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//
    if( editor_menumode == EDITOR_VEHICLEOBJ ) {
      if( editor_selector && pcar && editor_base_adr != pcar ) {  
        drawLegendMessage(translate_string("SELECT: Current Vehicle"), 0, 0, COLOR_TEXT); // left side, third row
      }
      if(editor_selector && !pcar && editor_base_adr != getObjectsTouchedObjectAddress(pobj) && getObjectsTouchedObjectAddress(pobj) >= editor_firstobj && getObjectsTouchedObjectAddress(pobj) <= editor_lastobj ) {  
        drawLegendMessage(translate_string("SELECT: Touched Vehicle"), 0, 0, COLOR_TEXT); // left side, third row
      }
      
      if( editor_selector 
        && getFloat(editor_base_adr) != 0.0f    // coordinate is set
        && getShort(editor_base_adr+0x62) != -1 //
        && getInt(editor_base_adr+0x1C) == 0    //
        && editor_base_adr != pcar ) {          // no need to teleport to own coordinates
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
      
    }
    if( editor_menumode == EDITOR_PEDOBJ  ) {
      if( editor_selector && pplayer && editor_base_adr != pplayer ) {
        drawLegendMessage(translate_string("SELECT: Player Object"), 0, 0, COLOR_TEXT); // left side, third row
      }
      
      if( editor_selector 
        && getFloat(editor_base_adr+0x30) != 0.0f // coordinate is set
        && getInt(editor_base_adr+0x40) != 0      //
        && getByte(editor_base_adr+0x43) == 0x09  //
        && editor_base_adr != pplayer ) {         // no need to teleport to own coordinates
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }      
    }
    if( editor_menumode == EDITOR_GARAGE  ) {
      if(editor_selector && getShort(editor_base_adr) != 0 && getFloat(editor_base_adr+0x4) != 0.0f) { // detect saved vehicle
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
      if( editor_selector && getShort(editor_base_adr) == 0 && editor_garageslot_current == 0 ) {  
        drawLegendMessage(translate_string("SELECT: Generate Vehicle"), 0, 0, COLOR_TEXT); // left side, third row
      }
    }
    if( editor_menumode == EDITOR_WORLDOBJ  ) {
      if( editor_selector && getFloat(editor_base_adr+0x30) != 0.0f && getWorldObjectIsActive(editor_base_adr) ) {
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
      if( editor_selector && editor_base_adr != getObjectsTouchedObjectAddress(pobj) && getObjectsTouchedObjectAddress(pobj) >= editor_firstobj && getObjectsTouchedObjectAddress(pobj) <= editor_lastobj ) {
        drawLegendMessage(translate_string("SELECT: Touched Object"), 0, 0, COLOR_TEXT); // left side, third row
      }
    }
    if( editor_menumode == EDITOR_BUSINESSOBJ  ) {
      if( editor_selector && getFloat(editor_base_adr+0x30) != 0.0f ) {
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
    }
    if( editor_menumode == EDITOR_PICKUPS  ) {
      if( editor_selector && getFloat(editor_base_adr) != 0 && getPickupIsActive(editor_base_adr) ) {
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
    }
    if( editor_menumode == EDITOR_MAPICONS  ) {
      if( editor_selector && getMapiconIsActive(editor_base_adr) && (getFloat(editor_base_adr+(LCS ? 0x14 : 0x18)) != 0.0f || getMapiconType(editor_base_adr) < 4) ) {
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
    }
    if( editor_menumode == EDITOR_HANDLINGCFG  ) {
      if( editor_selector && pcar && pcar_id != editor_vehicle_current) {
        drawLegendMessage(translate_string("SELECT: Set current vehicle"), 0, 1, COLOR_TEXT); // left side, second row
      }
    }
    if( editor_menumode == EDITOR_VEHWORLDSPAWNS  ) {
      if( editor_selector && getFloat(editor_base_adr+(0xC)) != 0 ) {
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
      if( editor_selector && pcar) {
        drawLegendMessage(translate_string("SELECT: Overwrite Vehicle"), 0, 0, COLOR_TEXT); // left side, third row
      }
    }
    if( editor_menumode == EDITOR_BUILDINGSIPL || editor_menumode == EDITOR_TREADABLESIPL || editor_menumode == EDITOR_DUMMYSIPL ) {
      if( editor_selector && getFloat(editor_base_adr+(0x38)) != 0 ) {
        drawLegendMessage(translate_string("SQUARE: Teleport there"), 0, 1, COLOR_TEXT); // left side, second row
      }
      
      if( editor_selector && editor_base_adr != getObjectsTouchedObjectAddress(pobj) && getObjectsTouchedObjectAddress(pobj) >= editor_firstobj && getObjectsTouchedObjectAddress(pobj) <= editor_lastobj ) {  
        drawLegendMessage(translate_string("SELECT: Touched Entity"), 0, 0, COLOR_TEXT); // left side, third row
      }
    }
    if( editor_menumode == EDITOR_WEAPONDAT  ) {
      if( editor_selector ) {  
        drawLegendMessage(translate_string("SELECT: Current Weapon"), 0, 0, COLOR_TEXT); // left side, third row
      }
    }
  if( editor_menumode == EDITOR_TIMECYCDAT  ) {
      if( editor_selector ) {  
        drawLegendMessage(translate_string("SELECT: Current Cycle"), 0, 0, COLOR_TEXT); // left side, third row
      }
    }
    //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//+//+//+//
    #ifdef HEXEDITOR
    if( editor_curmenu[editor_selection_val].address >= 0) {
      drawLegendMessage(translate_string("TRIANGLE: Open in HexEditor"), 1, 1, COLOR_TEXT); // right side, second row
    }
    #endif
    drawLegendMessage(translate_string("CIRCLE: Exit to Menu"), 1, 0, COLOR_TEXT); // right side, third row
  }

  return 0;
}

int editor_ctrl() {
  #ifdef HEXEDITOR
  int editor_mode = 0; // default 
  #endif
  
  void (* func)();
  static int keypress;
  
  if( editor_selector ) { // top menu
  
    if( hold_buttons & PSP_CTRL_DOWN ) {
      if( editor_selection_top <= 0 ) {
        if(editor_draw_lower) editor_selector = 0; // switch to values
        editor_selection_top = 0; // to be sure
      } else editor_selection_top -= 1;  
    }
    
    if( hold_buttons & PSP_CTRL_UP ) {
      if( editor_toptions > 1 ) {
        if ( editor_selection_top >= 1 ) {
          editor_selection_top = 1;
        } else editor_selection_top += 1;
      }
    }
    
    if( pressed_buttons & PSP_CTRL_SQUARE ) {
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
      if( (editor_menumode == EDITOR_PEDOBJ) && getFloat(editor_base_adr+0x30) != 0 && getPedObjectIsActive(editor_base_adr) && editor_base_adr != pplayer ) {
        editor_trigger = 1; // trigger teleport
      }
      if( (editor_menumode == EDITOR_VEHICLEOBJ) && getFloat(editor_base_adr+0x30) != 0 && getVehicleObjectIsActive(editor_base_adr) && editor_base_adr != pcar ) { 
        editor_trigger = 1; // trigger teleport
      }
      if( (editor_menumode == EDITOR_WORLDOBJ) && getFloat(editor_base_adr+0x30) != 0 && getWorldObjectIsActive(editor_base_adr) ) { 
        editor_trigger = 1; // trigger teleport
      }
      if( (editor_menumode == EDITOR_BUSINESSOBJ) && getFloat(editor_base_adr+0x30) != 0 ) { 
        editor_trigger = 1; // trigger teleport
      }
      if( (editor_menumode == EDITOR_PICKUPS) && getFloat(editor_base_adr) != 0 && getPickupIsActive(editor_base_adr) ) { 
        editor_trigger = 1; // trigger teleport
      }
      if( (editor_menumode == EDITOR_MAPICONS) && getMapiconIsActive(editor_base_adr) && (getFloat(editor_base_adr+(LCS ? 0x14 : 0x18)) != 0 || getMapiconType(editor_base_adr) < 4) ) { 
        editor_trigger = 1; // trigger teleport
      }
      if( editor_menumode == EDITOR_GARAGE && getFloat(editor_base_adr+0x4) != 0 ) {
        editor_trigger = 1; // trigger teleport
      }
      if( editor_menumode == EDITOR_VEHWORLDSPAWNS && getFloat(editor_base_adr+0x4) != 0 ) { // z coordinate is 0.00f for boats so we check x
        editor_trigger = 1; // trigger teleport
      }
      if( (editor_menumode == EDITOR_BUILDINGSIPL || editor_menumode == EDITOR_TREADABLESIPL || editor_menumode == EDITOR_DUMMYSIPL) && getFloat(editor_base_adr+0x30) != 0 ) { 
        editor_trigger = 1; // trigger teleport
      }
      
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
    }
    
    if( pressed_buttons & PSP_CTRL_SELECT ) {
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
      if( (editor_menumode == EDITOR_PEDOBJ) && pplayer && editor_base_adr != pplayer ) {
        int tadr = -1, pos = 0;
        for( tadr = editor_firstobj; tadr <= editor_lastobj; pos++, tadr += editor_blocksize ) {
          if( tadr == pplayer ) 
            editor_block_current = pos;
        }
      }
      
      if( editor_menumode == EDITOR_VEHICLEOBJ ) {
        if( pcar && editor_base_adr != pcar ) { // inside car & its not yet selected
          int tadr = -1, pos = 0;
          for( tadr = editor_firstobj; tadr <= editor_lastobj; pos++, tadr += editor_blocksize) {
            if( tadr == pcar ) 
              editor_block_current = pos;
          }
        }
        if( !pcar && editor_base_adr != getObjectsTouchedObjectAddress(pobj) && getObjectsTouchedObjectAddress(pobj) >= editor_firstobj && getObjectsTouchedObjectAddress(pobj) <= editor_lastobj) { //
          int tadr = -1, pos = 0;
          for( tadr = editor_firstobj; tadr <= editor_lastobj; pos++, tadr += editor_blocksize) {
            if( tadr == getObjectsTouchedObjectAddress(pobj) ) 
              editor_block_current = pos;
          }
        }
      } 
      
      if( ((editor_menumode == EDITOR_WORLDOBJ) || (editor_menumode == EDITOR_BUILDINGSIPL)) && getObjectsTouchedObjectAddress(pobj) != 0 ) {
        int tadr = -1, pos = 0;
        for( tadr = editor_firstobj; tadr <= editor_lastobj; pos++, tadr += editor_blocksize ) {
          if( tadr == getObjectsTouchedObjectAddress(pobj) ) 
            editor_block_current = pos;
        }        
      }
      
      
      if( editor_menumode == EDITOR_HANDLINGCFG ) {
        if( pcar ) editor_trigger = 1; // trigger teleport
      }
      
      if( editor_menumode == EDITOR_WEAPONDAT ) {
        editor_block_current = getPedsCurrentWeapon(pplayer);
      }
    
      if( editor_menumode == EDITOR_TIMECYCDAT ) {
        editor_block_current = getWeather() * 24 + getClockHours();
      }
      
      if( editor_menumode == EDITOR_GARAGE && getShort(editor_base_adr) == 0 && editor_garageslot_current == 0 ) {
        if( LCS && editor_garage_current == 0 ) // Portland
          createGarageVehicle(editor_base_adr, 0xAC, 1158.56f, -270.16f, 17.10f, 0.01f, 0.99f, 0.002f, 0x00, 0x00, 0xA, 0xFF, 0xFF);
          
        else if( LCS && editor_garage_current == 1 ) // Staunton
          createGarageVehicle(editor_base_adr, 0xAC, 297.59f, -427.37f, 26.20f, -0.01f, -0.99f, -0.002f, 0x00, 0x00, 0xA, 0xFF, 0xFF);
          
        else if( LCS && editor_garage_current == 2 ) // SSV
          createGarageVehicle(editor_base_adr, 0xAC, -789.25f, 289.82f, 48.60f, 0.32f, 0.94f, -0.001f, 0x00, 0x00, 0xA, 0xFF, 0xFF);
        
        else if( VCS && editor_garage_current == 0 ) // 101 Bayshore Ave
          createGarageVehicle(editor_base_adr, 0xBB, -835.40f, -1196.73f, 11.30f, 0.94f, 0.31f, -0.04f, 0x00, 0x00, 0x9, 0xFF, 0xFF);

        else if( VCS && editor_garage_current == 1 ) // The Compound
          createGarageVehicle(editor_base_adr, 0xBB, -1104.85f, 361.10f, 10.90f, 0.99f, -0.007f, -0.001f, 0x00, 0x00, 0x9, 0xFF, 0xFF);  

        else if( VCS && editor_garage_current == 2 ) // Clymenus Suite
          createGarageVehicle(editor_base_adr, 0xBB, 262.48f, -142.74f, 11.40f, 0.90f, -0.42f, 0.01f, 0x00, 0x00, 0x9, 0xFF, 0xFF);      
      }
      
      if( editor_menumode == EDITOR_VEHWORLDSPAWNS ) {
        if( pcar )
          createParkedVehicleSpawnViaSlot( editor_block_current, pcar_id, getFloat(pcar+0x30), getFloat(pcar+0x34), getFloat(pcar+0x38), (180.0f / M_PI) * getFloat(pplayer + (LCS ? 0x4E0 : 0x8D0)) + ((getFloat(pplayer + (LCS ? 0x4E0 : 0x8D0)) < 0.00f) ?  360.0f : 0.0f),  lcs_getVehicleColorBase(pcar), lcs_getVehicleColorStripe(pcar), 0, 0);
      }
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
    }
    
    if( (current_buttons & PSP_CTRL_RTRIGGER) == 0 ) { // fast scroll button not presssed
        
      if( hold_buttons & PSP_CTRL_LEFT ) {
        //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
        if( editor_menumode == EDITOR_GARAGE ) {
          if( editor_selection_top == 1 ) { // garage select
            if( editor_garage_current > 0 )
              editor_garage_current--;
          } 
          if( editor_selection_top == 0 ) { // slot select
            if( editor_garageslot_current > 0 )
              editor_garageslot_current--;
          }
        } else {
        //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
          if(editor_block_current > 0) 
            editor_block_current--; // default for editor_blocks
        }
        waittime = getGametime() + 500; // about 0.5 sec
      }
      
      if( hold_buttons & PSP_CTRL_RIGHT ) {
        //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
        if( editor_menumode == EDITOR_GARAGE ) {
          if( editor_selection_top == 1 ) { //garage select
            if( editor_garage_current < 2 )
              editor_garage_current++;
          } 
          if( editor_selection_top == 0 ) { //slot select
            if( editor_garageslot_current < 3 )
              editor_garageslot_current++;
          }
        } else {
        //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
          if( editor_block_current+1 < editor_blocks ) 
            editor_block_current++; //default for editor_blocks
        }
        waittime = getGametime() + 500; //about 0.5 sec
      }
    }
    
  } else {
    
      /// Navigation UP ////////////////////////////    
      if( hold_buttons & PSP_CTRL_UP ) {
        if( editor_selection_val <= 0 ) {
          if( editor_toptions ) editor_selector = 1; // switch to top menu
        } else editor_selection_val -= 1;

        while( editor_curmenu[editor_selection_val].address == -1 && editor_selection_val > 0) //skip dummys
          editor_selection_val -= 1;        
        
        if ( editor_top > 1 ) { // scroll
          if ( editor_selection_val == 1 ) editor_top--;
        }  
      }   
    
      /// Navigation DOWN ////////////////////////////
      if( hold_buttons & PSP_CTRL_DOWN ) {
        if( editor_showoptions == 1 ) { // handling 1 option only
          if( editor_selection_val >= editor_options-1 ) 
            editor_selection_val = editor_options-1; // selection = 0;
          else 
            editor_selection_val += 1;
          
          editor_top = editor_selection_val;
          
        } else {
          if ( editor_selection_val >= editor_options-1 ) 
            editor_selection_val = editor_options-1; // selection = 0;
          
          else {
            editor_selection_val += 1;
            
            while( editor_curmenu[editor_selection_val].address == -1 ) // skip dummys
              editor_selection_val += 1;
              
            if ( editor_top + editor_showoptions < editor_options ) { // scroll
              if ( editor_selection_val >= editor_top+editor_showoptions-1 ) 
                editor_top++; 
            }
          }  
          if ( editor_top + editor_showoptions < editor_options ) { // scroll
            if ( editor_selection_val == editor_top + editor_showoptions-1 ) 
              editor_top++; 
          }
        }
      }
      
    
      /// left right depending on value editing or function (if allowed)
      if( editor_curmenu[editor_selection_val].edit_bool == TRUE ) {
        
        if( editor_curmenu[editor_selection_val].value != 0 ) {
          keypress = 0;
          if( hold_buttons & PSP_CTRL_LEFT )  
            keypress = PSP_CTRL_LEFT;
          if( hold_buttons & PSP_CTRL_RIGHT ) 
            keypress = PSP_CTRL_RIGHT;
          func = (void *)(editor_curmenu[editor_selection_val].value);
          func(FUNC_CHANGE_VALUE, keypress, editor_base_adr, editor_curmenu[editor_selection_val].address+editor_base_adr, (int)editor_curmenu[editor_selection_val].steps);
          editor_wasused++;
          
        } else {
          
          char c, c_new;
          int adr = editor_curmenu[editor_selection_val].address+editor_base_adr;
          char pre = editor_curmenu[editor_selection_val].precision;
      
          if( (current_buttons & PSP_CTRL_RTRIGGER) == 0 ) { // not R Trigger
            if( hold_buttons & PSP_CTRL_LEFT ) {          
              switch( editor_cur_type ) {
                case TYPE_BYTE: case TYPE_BYTE_AS_FLT:
                  *(char*)(adr)-=editor_curmenu[editor_selection_val].steps;
                  break;
                  
                case TYPE_SHORT:
                  *(short*)(adr)-=editor_curmenu[editor_selection_val].steps;
                  break;
                  
                case TYPE_INTEGER:
                  *(int*)(adr)-=editor_curmenu[editor_selection_val].steps;
                  break;
                  
                case TYPE_FLOAT:
                  *(float*)(adr)-=editor_curmenu[editor_selection_val].steps;
                  break;
                  
                case TYPE_BIT:        
                  if( *(char*)(adr) & (1 << pre) ) { // BIT TRUE
                    *(char*)(adr) &= ~(1 << pre); // flip bit OFF at index .precision
                  } else { // BIT FALSE
                    *(char*)(adr) |= (1 << pre); // flip bit ON at index .precision
                  }  
                  break;
                  
                case TYPE_BOOL:
                  *(char*)(adr) = 1 - *(char*)(adr);
                  break;
                  
                case TYPE_NIBBLE_LOW: // eg: 0xE6 & 0xF    -> 0x6    = 0x 0000 0110
                  c = getByte(adr);
                  if( (c & 0xF) > 0 ) { //0x6 > 0x0
                    c_new = (c & 0xF) - editor_curmenu[editor_selection_val].steps;
                    *(char*)(adr) = (c & 0xF0) | (c_new & 0xF); // write low quartet
                  }
                  break;
                  
                case TYPE_NIBBLE_HIGH: // eg: 0xE6 >> 4    -> 0xE    = 0x 0000 1110
                  c = getByte(editor_curmenu[editor_selection_val].address+editor_base_adr);
                  if( ((c >> 4) & 0xF) > 0 ) { //0xE6 -> 0x?E -> 0x0E    >     0x0
                     c_new = (c >> 4) - editor_curmenu[editor_selection_val].steps;
                     *(char*)(adr) = (c & 0x0F) | ((c_new & 0xF) << 4); // write high quartet
                  }
                  break;
              }
              editor_wasused++;
            }
            
            if( hold_buttons & PSP_CTRL_RIGHT ) {
              switch( editor_cur_type ) {
                case TYPE_BYTE: case TYPE_BYTE_AS_FLT:
                  *(char*)(adr)+=editor_curmenu[editor_selection_val].steps;
                  break;
                  
                case TYPE_SHORT:
                  *(short*)(adr)+=editor_curmenu[editor_selection_val].steps;
                  break;
                    
                case TYPE_INTEGER:
                  *(int*)(adr)+=editor_curmenu[editor_selection_val].steps;
                  break;
                  
                case TYPE_FLOAT:
                  *(float*)(adr)+=editor_curmenu[editor_selection_val].steps;
                  break;
                  
                case TYPE_BIT:        
                  if( *(char*)(adr) & (1 << pre) ) { // BIT TRUE
                    *(char*)(adr) &= ~(1 << pre); // flip bit OFF at index .precision
                  } else { //BIT FALSE
                    *(char*)(adr) |= (1 << pre); // flip bit ON at index .precision
                  }  
                  break;  
                  
                case TYPE_BOOL:
                  *(char*)(adr) = 1 - *(char*)(adr);
          break;  
                  
                case TYPE_NIBBLE_LOW: // eg: 0xE6 & 0xF    -> 0x6    = 0x 0000 0110
                  c = getByte(adr);
                  if( (c & 0xF) < 0xF ) { //0x6 < 0xF
                    c_new = (c & 0xF) + editor_curmenu[editor_selection_val].steps;
                    *(char*)(adr) = (c & 0xF0) | (c_new & 0xF); // write low quartet
                  }
                  break;
                  
                case TYPE_NIBBLE_HIGH: // eg: 0xE6 >> 4    -> 0xE    = 0x 0000 1110
                  c = getByte(adr);
                  if( ((c >> 4) & 0xF) < 0xF ) { // 0xE6 -> 0x?E -> 0x0E    <     0xF    
                     c_new = (c >> 4) + editor_curmenu[editor_selection_val].steps;
                     *(char*)(adr) = (c & 0x0F) | ((c_new & 0xF) << 4); // write high quartet
                  }
                  break;
              }
              editor_wasused++;
            }
      
          }
          
          if( hold_buttons & PSP_CTRL_CROSS ) {
            switch( editor_cur_type ) {
              case TYPE_BOOL:
                *(unsigned char*)(adr) = 1 - *(unsigned char*)(adr);
                editor_wasused++;
                break;
              
              case TYPE_BIT:
                if( *(char*)(adr) & (1 << pre) ) { // BIT TRUE
                  *(char*)(adr) &= ~(1 << pre); // flip bit OFF at index .precision
                } else { // BIT FALSE
                  *(char*)(adr) |= (1 << pre); // flip bit ON at index .precision
                }  
                editor_wasused++;
                break;  
            }
          }
        }
      }

  }
  
  
  if( hold_buttons & PSP_CTRL_RTRIGGER ) { // fast scrolling
  
    if( hold_buttons & PSP_CTRL_LEFT ) {
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
      switch( editor_menumode ) {
        case EDITOR_GARAGE: 
          if( editor_selection_top == 1 ) { // garage
            if( editor_garage_current > 0 )
              editor_garage_current--;
            
          } else if( editor_selection_top == 0 ) { // slot
            if( editor_garageslot_current > 0 )
              editor_garageslot_current--;
          } break;
            
        default:   
          if( editor_selector ) { // fast scroll when in top menu
            if( editor_block_current >= 10 ) 
              editor_block_current-=10;
            else 
              editor_block_current = 0;
            
          } else {
            if( editor_block_current > 0 ) // trigger in lower menu
              editor_block_current--;
          }
          
          break;
      }
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
      waittime = getGametime() + 500; // about 0.5 sec
    }
  
    if( hold_buttons & PSP_CTRL_RIGHT ) {
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
      switch( editor_menumode ) {
        case EDITOR_GARAGE: 
          if( editor_selection_top == 1 ) { // garage
            if( editor_garage_current < 2 )
              editor_garage_current++;
            
          } else if( editor_selection_top == 0 ) { // slot
            if( editor_garageslot_current < 3 )
              editor_garageslot_current++;
          } break;
          
        default: 
          if( editor_selector ) { // fast scroll when in top menu
            if( editor_block_current+1 < editor_blocks-9 ) 
              editor_block_current+=10;
            else
              editor_block_current = editor_blocks-1;
            
          } else {
            if( editor_block_current+1 < editor_blocks ) 
              editor_block_current++;
          }          
          break;
      }
      //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
      waittime = getGametime() + 500; // about 0.5 sec
    }
  }

  #ifdef HEXEDITOR
  if( pressed_buttons & PSP_CTRL_TRIANGLE && editor_curmenu[editor_selection_val].address >= 0 ) {
    //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
    switch( editor_menumode ) {
      case EDITOR_GARAGE: 
        snprintf(buffer, sizeof(buffer), "Garage Data (%s - Slot: %i)", LCS ? lcs_garagenames[editor_garage_current] : vcs_garagenames[editor_garage_current], editor_garageslot_current+1);
        break;
        
      case EDITOR_PEDOBJ: 
        snprintf(buffer, sizeof(buffer), "Pedestrian Object %i/%i", editor_block_current+1, editor_blocks);
        break;
          
      case EDITOR_VEHICLEOBJ: 
        snprintf(buffer, sizeof(buffer), "Vehicle Object %i/%i", editor_block_current+1, editor_blocks);
        break;
        
      case EDITOR_WORLDOBJ: 
        snprintf(buffer, sizeof(buffer), "World Object %i/%i", editor_block_current+1, editor_blocks);
        break;
        
      case EDITOR_BUSINESSOBJ: 
        snprintf(buffer, sizeof(buffer), "Business Object %i/%i", editor_block_current+1, editor_blocks);
        break;
      
      case EDITOR_PICKUPS: 
        snprintf(buffer, sizeof(buffer), "Pickup %i/%i", editor_block_current+1, editor_blocks);
        break;
      
      case EDITOR_MAPICONS: 
        snprintf(buffer, sizeof(buffer), "Mapicon %i/%i", editor_block_current+1, editor_blocks);
        break;
      
      case EDITOR_VEHWORLDSPAWNS: 
        snprintf(buffer, sizeof(buffer), "Parked Vehicle Spawn %i/%i", editor_block_current+1, editor_blocks);
        break;
      
      /// /// /// /// /// 
      
      case EDITOR_HANDLINGCFG: 
        //snprintf(buffer, sizeof(buffer), "handling.cfg (ID: %i  Name: %s)", lcs_vehicles[editor_block_current].id, lcs_vehicles[editor_block_current].name); //editor_block_current, editor_blocks
        // if( LCS )
          // snprintf(buffer, sizeof(buffer), "handling.cfg (Slot %i)", lcs_vehicles[editor_block_current].hndlng_no); //alternative
        snprintf(buffer, sizeof(buffer), "Handling.cfg");
        break;
      
      case EDITOR_BUILDINGSIPL:
        snprintf(buffer, sizeof(buffer), "Buildings.ipl %i/%i", editor_block_current+1, editor_blocks);
        break;
        
      case EDITOR_TREADABLESIPL: 
        snprintf(buffer, sizeof(buffer), "Treadables.ipl %i/%i", editor_block_current+1, editor_blocks);
        break;
        
      case EDITOR_DUMMYSIPL: 
        snprintf(buffer, sizeof(buffer), "Dummys.ipl %i/%i", editor_block_current+1, editor_blocks);
        break;
      
      case EDITOR_CARCOLSDAT: 
        snprintf(buffer, sizeof(buffer), "Carcols.dat %i/%i", editor_block_current+1, editor_blocks);
        break;
      
      case EDITOR_PEDCOLSDAT: 
        snprintf(buffer, sizeof(buffer), "Pedcols.dat %i/%i", editor_block_current+1, editor_blocks);
        break;
        
      case EDITOR_IDE: 
        //editor_base_adr = editor_firstobj + (editor_block_current * editor_blocksize); //hex to ptr instead
        //editor_temp_blocksize = 0x4;
        snprintf(buffer, sizeof(buffer), "IDEs %i/%i", editor_block_current, editor_blocks-1);
        break;
        
      case EDITOR_PARTICLECFG: 
        snprintf(buffer, sizeof(buffer), "Particle.cfg %i/%i", editor_block_current+1, editor_blocks);
        break;
        
      case EDITOR_PEDSTATSDAT: 
        snprintf(buffer, sizeof(buffer), "Pedstats.dat %i/%i", editor_block_current+1, editor_blocks);
        break;
        
      case EDITOR_WEAPONDAT: 
        snprintf(buffer, sizeof(buffer), "Weapon.dat %i/%i", editor_block_current+1, editor_blocks);
        break;
    
      case EDITOR_TIMECYCDAT: 
        snprintf(buffer, sizeof(buffer), "Timecyc.dat - %s / %02i:00 - %s", LCS ? weather_lcs[editor_block_current / 24] : weather_vcs[editor_block_current / 24], editor_block_current % 24, editor_curmenu[editor_selection_val].name);
        break;
                
      default: 
        memset(&buffer[0], 0, sizeof(buffer));
        break;
    }
    //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//
    
    editor_mode = 1;
    editor_wasused++; // we can't really know but..
    
    #ifdef HEXMARKERS
    hex_marker_clear();
    
    if( !editor_selector ) { // we are in lower menu - add marker to hex location
      int markerlength = 1; // default
      switch( editor_cur_type ) {
        case TYPE_SHORT: 
          markerlength = 2;
          break;
            
        case TYPE_INTEGER: case TYPE_FLOAT:
          markerlength = 4;
          break;  
      }
      hex_marker_addx(editor_base_adr + (editor_selector ? 0 : editor_curmenu[editor_selection_val].address), markerlength);
    }

    #endif
    
    if( editor_menumode == EDITOR_IDE || editor_menumode == EDITOR_HANDLINGCFG || editor_menumode == EDITOR_TIMECYCDAT ) {
      hexeditor_create(editor_base_adr + (editor_selector ? 0 : editor_curmenu[editor_selection_val].address), editor_mode, editor_base_adr, editor_base_adr + editor_temp_blocksize, buffer);
    } else {
      hexeditor_create(editor_base_adr + (editor_selector ? 0 : editor_curmenu[editor_selection_val].address), editor_mode, editor_base_adr, editor_base_adr + editor_blocksize, buffer);
    }
  }
  #endif
  
  //+//+// SAVE BACK LAST VIEWED OBJECT //+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//+//+//+//
  switch( editor_menumode ) { // write back editor_block_current to its global value
      
    case EDITOR_PEDOBJ: 
      editor_pedobj_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x30), getFloat(editor_base_adr+0x34), getFloat(editor_base_adr+0x38)+0.5f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
    
    
    case EDITOR_VEHICLEOBJ: 
      editor_vehicleobj_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x30), getFloat(editor_base_adr+0x34), getFloat(editor_base_adr+0x38)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
      
    case EDITOR_WORLDOBJ: 
      editor_worldobj_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x30), getFloat(editor_base_adr+0x34), getFloat(editor_base_adr+0x38)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
      
    case EDITOR_BUSINESSOBJ: 
      editor_businessobj_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x30), getFloat(editor_base_adr+0x34), getFloat(editor_base_adr+0x38)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
    
    case EDITOR_PICKUPS: 
      editor_pickup_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr), getFloat(editor_base_adr+0x4), getFloat(editor_base_adr+0x8) + 1.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
    
    case EDITOR_MAPICONS: 
      editor_mapicon_current = editor_block_current;
      if( editor_trigger ) {
        int slot = getMapiconLinkedObjectSlotNumber(editor_base_adr);
        int base = -1;
        
        extern int vehicles_base;
        extern int vehicles_max;
        extern int peds_base;
        extern int peds_max;
        extern int worldobjs_base;
        extern int worldobjs_max;
        extern u32 var_pedobjsize;
        extern u32 var_vehobjsize;
        extern u32 var_wldobjsize;
        
        if( getMapiconType(editor_base_adr) == 1 ) { // use coordinates from VEHICLE object via slot number
          if( slot >= 0 && slot <= vehicles_max ) {
            base = vehicles_base + (var_vehobjsize * slot);            
            if( getVehicleObjectIsActive(base) )
              teleport(getFloat(base+0x30), getFloat(base+0x34), getFloat(base+0x38)+2.0f);
          }
          
        } else if( getMapiconType(editor_base_adr) == 2 ) { // use coordinates from PEDESTRIAN object via slot number
          if( slot >= 0 && slot <= peds_max ) {
            base = peds_base + (var_pedobjsize * slot);
            if( getPedObjectIsActive(base) ) 
              teleport(getFloat(base+0x30), getFloat(base+0x34), getFloat(base+0x38)+0.5f);
          }
        
        } else if( getMapiconType(editor_base_adr) == 3 ) { // use coordinates from WORLD object via slot number
          if( slot >= 0 && slot <= worldobjs_max ) {
            base = worldobjs_base + (var_wldobjsize * slot);
            if( getWorldObjectIsActive(base) )
              teleport(getFloat(base+0x30), getFloat(base+0x34), getFloat(base+0x38)+0.5f);
          }
        
        } else { // use coordinates in blip slot
          teleport(getFloat(editor_base_adr+(LCS ? 0xC : 0x10)), getFloat(editor_base_adr+(LCS ? 0x10 : 0x14)), getFloat(editor_base_adr+(LCS ? 0x14 : 0x18)) + 2.00f);
        }
        //closeMenu(); // close menugetFloat(editor_base_adr+)
        editor_trigger = 0; 
      }
      break;
      
    case EDITOR_GARAGE: 
      //editor_garageslot_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x4), getFloat(editor_base_adr+0x8), getFloat(editor_base_adr+0xC)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
      
    case EDITOR_HANDLINGCFG: 
      //if( LCS ) 
      //  editor_vehicle_current = lcs_vehicles[editor_block_current].id;
      //if( VCS )
        editor_vehicle_current = editor_block_current;
      
      if( editor_trigger ) {
        if( pcar ) 
          editor_vehicle_current = pcar_id;

        editor_trigger = 0; 
      }
      break;
    
    
    case EDITOR_VEHWORLDSPAWNS: 
      editor_vehiclespawn_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x4), getFloat(editor_base_adr+0x8), getFloat(editor_base_adr+0xC)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
    
    case EDITOR_BUILDINGSIPL:
      editor_buildingsIPL_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x30), getFloat(editor_base_adr+0x34), getFloat(editor_base_adr+0x38)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
    case EDITOR_TREADABLESIPL:
      editor_treadablesIPL_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x30), getFloat(editor_base_adr+0x34), getFloat(editor_base_adr+0x38)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
    case EDITOR_DUMMYSIPL:
      editor_dummysIPL_current = editor_block_current;
      if( editor_trigger ) {
        teleport(getFloat(editor_base_adr+0x30), getFloat(editor_base_adr+0x34), getFloat(editor_base_adr+0x38)+2.00f);
        //closeMenu(); // close menu
        editor_trigger = 0; 
      }
      break;
      
    case EDITOR_CARCOLSDAT:
      editor_carcolsDAT_current = editor_block_current;
      break;
      
    case EDITOR_PEDCOLSDAT:
      editor_pedcolsDAT_current = editor_block_current;
      break;
    
    case EDITOR_IDE:
      editor_ide_current = editor_block_current;
      break;
    
    
    case EDITOR_PARTICLECFG:
      editor_particleCFG_current = editor_block_current;
      break;
      
    case EDITOR_PEDSTATSDAT:
      editor_pedstatsDAT_current = editor_block_current;
      break;
      
    case EDITOR_WEAPONDAT:
      editor_weaponDAT_current = editor_block_current;
      break;
    
    case EDITOR_TIMECYCDAT:
      editor_timecycDAT_current = editor_block_current;
      break;
    
    
    default:
      flag_editor = 0; // just to be sure
      break;
  }
  //+//+//+//+//+/+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+//+////+//+//+//+//+/+//+//+//+//+//+//+//+//  
        
  if( pressed_buttons & PSP_CTRL_CIRCLE )
    flag_editor = 0;
  
  return 0;  
}
#endif




/********************************************************************************************************
*
* Free Camera 
*
*********************************************************************************************************/
#ifdef FREECAM
short flag_freecam = 0;

float camera_x, camera_y, camera_z; // camera xyz position in world
float fov;
float cam_coord_x1, cam_coord_y1, cam_coord_z1;
float cam_coord_x2, cam_coord_y2, cam_coord_z2;
float radius, inclination, azimuth; 
float movespeed = 0.40, turnspeed = 0.05;
int printinfo = 1;
int protofix = 0, protofix2 = 0; // todo make dynamic?!
extern u32 global_camera;

int freecam_create() {
  #ifdef LOG
  logPrintf("[INFO] %i: freecam_create()", getGametime());
  #endif
  
  if( LCS && (mod_text_size == 0x0031F854 || mod_text_size == 0x00320A34) ) // ULUX v0.02 & ULUS v1.02
    protofix = -0x10;
  
  if( VCS && mod_text_size == 0x00377D30 ) // ULUS v1.01
    protofix2 = 0x4;
  
  flag_freecam = 1; // only set here!
  printinfo = 1;
  setByte(pplayer + (LCS ? 0x560 : 0x550) + protofix2, 1); // unbind camera from player 
  setFloat(global_camera + (LCS ? 0xCC : 0x798), 4.0f); // unbind camera from vehicle (by setting camera mode to "4")
  return 0;
}

int freecam_draw() {
  float player_x = getFloat(pplayer+0x30);
  float player_y = getFloat(pplayer+0x34);
  float player_z = getFloat(pplayer+0x38);
  
  /// get camera values
  if( getByte(global_camera + (LCS ? 0x6B+protofix : 0x81A) ) == 0x1) { // cutscene
    camera_x = getFloat(global_camera + (LCS ? 0x9C0+protofix : 0x7F0));
    camera_y = getFloat(global_camera + (LCS ? 0x9C4+protofix : 0x7F4));
    camera_z = getFloat(global_camera + (LCS ? 0x9C8+protofix : 0x7F8));
  } else {
    camera_x = getFloat(global_camera + (LCS ? 0x340+protofix : 0x90));
    camera_y = getFloat(global_camera + (LCS ? 0x344+protofix : 0x94));
    camera_z = getFloat(global_camera + (LCS ? 0x348+protofix : 0x98));
  }
  fov = getFloat(global_camera + (LCS ? 0x254+protofix : 0x198));
  cam_coord_x1 = getFloat(global_camera + (LCS ? 0x330+protofix : 0x80));
  cam_coord_y1 = getFloat(global_camera + (LCS ? 0x334+protofix : 0x84));
  cam_coord_z1 = getFloat(global_camera + (LCS ? 0x338+protofix : 0x88));
  

  /// calculate
  radius = 1.00; // root( a^2 + b^2 + c^2 )
  inclination = acos(cam_coord_z1 / radius); // theta
  azimuth = atan(cam_coord_y1 / cam_coord_x1); // phi

  azimuth     -= (xstick_ * turnspeed);  
  inclination += (ystick_ * turnspeed);
  if( inclination < 0.20 ) inclination = 0.20; // set bound
  if( inclination > 3.00 ) inclination = 3.00; // set bound

  /// camera turning via keys combo
  if( current_buttons & PSP_CTRL_CROSS ) { 
    if( current_buttons & PSP_CTRL_UP ) inclination -= turnspeed;   // cam up
    if( current_buttons & PSP_CTRL_DOWN ) inclination += turnspeed; // cam down
    if( current_buttons & PSP_CTRL_LEFT ) azimuth += turnspeed;     // cam left
    if( current_buttons & PSP_CTRL_RIGHT ) azimuth -= turnspeed;    // cam right
  }

  /// calc new values 
  float back_x1 = cam_coord_x1; // for ulgy fix
  float back_y1 = cam_coord_y1;
  
  cam_coord_x1 = radius * sin(inclination) * cos(azimuth); // x  needs fix
  cam_coord_y1 = radius * sin(inclination) * sin(azimuth); // y  needs fix
  cam_coord_z1 = radius * cos(inclination);                // z  OK
  cam_coord_x2 = radius * cos(inclination) * cos(azimuth); // x2  needs fix
  cam_coord_y2 = radius * cos(inclination) * sin(azimuth); // y2  needs fix
  cam_coord_z2 = radius * sin(inclination);                // z2  OK    
    
  /// ugly quick fix TODO  
  if( back_x1 > 0.00 && back_y1 > 0.00 ) { // direction 1
    cam_coord_x2 = -cam_coord_x2;
    cam_coord_y2 = -cam_coord_y2;
  } else if( back_x1 < 0.00 && back_y1 > 0.00 ) { // direction 2
    cam_coord_x1 = -cam_coord_x1;
    cam_coord_y1 = -cam_coord_y1;
  } else if( back_x1 < 0.00 && back_y1 < 0.00 ) { // direction 3
    cam_coord_x1 = -cam_coord_x1;
    cam_coord_y1 = -cam_coord_y1;
  } else if( back_x1 > 0.00 && back_y1 < 0.00 ) { // direction 4
    cam_coord_x2 = -cam_coord_x2;
    cam_coord_y2 = -cam_coord_y2;
  }
    
    if( current_buttons & PSP_CTRL_SQUARE ) {
      if( current_buttons & PSP_CTRL_UP ) 
        camera_z = camera_z + movespeed; // camera up      
      if( current_buttons & PSP_CTRL_DOWN ) 
        camera_z = camera_z - movespeed; // camera down
      
      if( current_buttons & PSP_CTRL_LEFT ) 
        if( fov <= 149.5f ) 
          fov += 0.5; // zoom out
      if( current_buttons & PSP_CTRL_RIGHT ) 
        if( fov >= 0.5f ) 
          fov -= 0.5; // zoom in
    }
    
      if( !(current_buttons & PSP_CTRL_SQUARE) && !(current_buttons & PSP_CTRL_CROSS) ) {
        if( current_buttons & PSP_CTRL_UP ) { // forward
          camera_x = camera_x + (cam_coord_x1 * movespeed);
          camera_y = camera_y + (cam_coord_y1 * movespeed);
          camera_z = camera_z + (cam_coord_z1 * movespeed);
        }
        
        if( current_buttons & PSP_CTRL_DOWN ) { // backwards
          camera_x = camera_x - (cam_coord_x1  * movespeed);
          camera_y = camera_y - (cam_coord_y1  * movespeed);
          camera_z = camera_z - (cam_coord_z1  * movespeed);
        }
            
        if( current_buttons & PSP_CTRL_LEFT ) { // to left
          camera_x = camera_x - (cam_coord_y1  * movespeed);
          camera_y = camera_y + (cam_coord_x1  * movespeed);
        }
        
        if( current_buttons & PSP_CTRL_RIGHT ) { // to right
          camera_x = camera_x + (cam_coord_y1  * movespeed);
          camera_y = camera_y - (cam_coord_x1  * movespeed);
        }
      }
      
    if( current_buttons & PSP_CTRL_LTRIGGER ) { // adjust movement speed
      if( current_buttons & PSP_CTRL_CROSS ) {
        if( turnspeed > 0.001 ) turnspeed -= 0.001;
      } else {  
        if( movespeed > 0.01 ) movespeed -= 0.01;
      }
    }
        
    if( current_buttons & PSP_CTRL_RTRIGGER ) { // adjust movement speed
      if( current_buttons & PSP_CTRL_CROSS ) {
        if( turnspeed < 0.3 ) turnspeed += 0.001;      
      } else {
        if( movespeed < 5.00 ) movespeed += 0.01;
      }
    }    
    
  /////////////////////////////////////////////////////////////////////////////////////
  
  /// write back new values
  if( getByte(global_camera + (LCS ? 0x6B+protofix : 0x81A)) == 0x1 ) { // cutscene
    setFloat(global_camera + (LCS ? 0x9C0+protofix : 0x7F0), camera_x);
    setFloat(global_camera + (LCS ? 0x9C4+protofix : 0x7F4), camera_y);
    setFloat(global_camera + (LCS ? 0x9C8+protofix : 0x7F8), camera_z);
  } else {
    setFloat(global_camera + (LCS ? 0x340+protofix : 0x90), camera_x);
    setFloat(global_camera + (LCS ? 0x344+protofix : 0x94), camera_y);
    setFloat(global_camera + (LCS ? 0x348+protofix : 0x98), camera_z);
  }
  
  setFloat(global_camera + (LCS ? 0x254+protofix : 0x198), fov);
  
  setFloat(global_camera + (LCS ? 0x330+protofix : 0x80), cam_coord_x1);
  setFloat(global_camera + (LCS ? 0x334+protofix : 0x84), cam_coord_y1);
  setFloat(global_camera + (LCS ? 0x338+protofix : 0x88), cam_coord_z1);
  
  setFloat(global_camera + (LCS ? 0x360+protofix : 0xD0), cam_coord_x2);
  setFloat(global_camera + (LCS ? 0x364+protofix : 0xD4), cam_coord_y2);
  setFloat(global_camera + (LCS ? 0x368+protofix : 0xD8), cam_coord_z2);
  
  
  /////////////////////////////////////////////////////////////////////////////////////
  
  if( pressed_buttons & PSP_CTRL_SELECT ) {
    teleport(camera_x, camera_y, camera_z);
  }  
  
  /////////////////////////////////////////////////////////////////////////////////////
  
  if( printinfo ) {
    
    drawString(translate_string("Free Camera"), ALIGN_FREE, FONT_DIALOG, SIZE_BIG, SHADOW_OFF, 8.0f, 5.0f, COLOR_FREECAM);
    
    drawString(translate_string("Player position:"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 40.0f, COLOR_TEXT);
      snprintf(buffer, sizeof(buffer), "x: %.2f", player_x);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 55.0f, COLOR_VALUE);
      snprintf(buffer, sizeof(buffer), "y: %.2f", player_y);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 70.0f, COLOR_VALUE);
      snprintf(buffer, sizeof(buffer), "z: %.2f", player_z);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 85.0f, COLOR_VALUE);
      
      
    drawString(translate_string("Camera position:"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 120.0f, COLOR_TEXT);
      snprintf(buffer, sizeof(buffer), "x: %.2f", camera_x); 
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 135.0f, COLOR_VALUE);
      snprintf(buffer, sizeof(buffer), "y: %.2f", camera_y);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 150.0f, COLOR_VALUE);
      snprintf(buffer, sizeof(buffer), "z: %.2f", camera_z);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 165.0f, COLOR_VALUE);
    

    /*snprintf(buffer, sizeof(buffer), "cam_coord_x1 = %.2f", cam_coord_x1); //D0
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 160.0f, 20.0f, COLOR_VALUE);
    snprintf(buffer, sizeof(buffer), "cam_coord_y1 = %.2f", cam_coord_y1); //D4
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 160.0f, 40.0f, COLOR_VALUE);   
    snprintf(buffer, sizeof(buffer), "cam_coord_z1 = %.2f", cam_coord_z1); //D8
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 160.0f, 60.0f, COLOR_VALUE);
    
    snprintf(buffer, sizeof(buffer), "cam_coord_x2 = %.2f", cam_coord_x2); //E8
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 160.0f, 90.0f, COLOR_VALUE);
    snprintf(buffer, sizeof(buffer), "cam_coord_y2 = %.2f", cam_coord_y2); //EC
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 160.0f, 110.0f, COLOR_VALUE);
    snprintf(buffer, sizeof(buffer), "cam_coord_z2 = %.2f", cam_coord_z2); //F0
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 160.0f, 130.0f, COLOR_VALUE);
    */
    
    snprintf(buffer, sizeof(buffer), translate_string("cutscene: %X"), getByte( global_camera + (LCS ? 0x6B+protofix : 0x81A)) );
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 30.0f, GREY);
    
    snprintf(buffer, sizeof(buffer), translate_string("camera: %X"), getByte( global_camera + (LCS ? 0x64+protofix : 0x816)) );
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 50.0f, GREY);
    

    snprintf(buffer, sizeof(buffer), translate_string("FOV: %.2f"), fov);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 100.0f, GREY);
    
    snprintf(buffer, sizeof(buffer), translate_string("movespeed: %.2f"), movespeed);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 125.0f, GREY);
    snprintf(buffer, sizeof(buffer), translate_string("turnspeed: %.3f"), turnspeed);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 140.0f, GREY);
      
    snprintf(buffer, sizeof(buffer), translate_string("radius: %.2f"), radius);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 170.0f, GREY);
    snprintf(buffer, sizeof(buffer), translate_string("inclination: %.2f"), inclination);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 185.0f, GREY);
    snprintf(buffer, sizeof(buffer), translate_string("azimuth: %.2f"), azimuth);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 200.0f, GREY);
      

    if( flag_use_legend ) {
      drawLegendBox(3, COLOR_BACKGROUND);
      drawLegendMessage(translate_string("D-PAD: Move Camera position"),    0, 2, COLOR_TEXT); // left side, first row
      drawLegendMessage(translate_string("TRIANGLE: Show/Hide text"),       1, 2, COLOR_TEXT); // right side, first row
      drawLegendMessage(translate_string("CROSS + D-PAD: Turn Camera"),     0, 1, COLOR_TEXT); // left side, second row
      drawLegendMessage(translate_string("L/R (+ CROSS): Adjust Speed(s)"), 1, 1, COLOR_TEXT); // right side, second row
      drawLegendMessage(translate_string("SQUARE + D-PAD: Height & FOV"),   0, 0, COLOR_TEXT); // left side, third row
      drawLegendMessage(translate_string("CIRCLE: Exit to Menu"),           1, 0, COLOR_TEXT); // right side, third row
    }
  }

  return 0;
}

int freecam_ctrl() {
  
  if( pressed_buttons & PSP_CTRL_CIRCLE ) {
    flag_freecam = 0;
    setByte(pplayer + (LCS ? 0x560 : 0x550) + protofix2, 0); // rebind camera to player
    setFloat(global_camera + (LCS ? 0xCC : 0x798 ), 2.0f); // rebind camera to vehicle
  }

  if( pressed_buttons & PSP_CTRL_TRIANGLE ) {
    printinfo = 1 - printinfo;
  }  
  
  return 0;  
}
#endif



/********************************************************************************************************
*
* Enter Address for HexEditor
*
*********************************************************************************************************/
#ifdef HEXEDITOR
short flag_address = 0;
int tempaddress = -1; // stores the temp address which may or may not be hex_addr later
int addresspos = -1;  // position of cursor to digit to be adjusted

#define ADR_HISTORY_SIZE 32
int adr_history[ADR_HISTORY_SIZE];
int history_position = 0;

void add_adr_to_history(int address) { // new addresses added to beginning.. rest shifted
  int i;
  if( address != adr_history[0] ) { // only add address if its not the first already
    for( i = ADR_HISTORY_SIZE-1; i > 0; i-- ) {
      adr_history[i] = adr_history[i-1];
    } adr_history[0] = address;
  }
  //remove duplicates when shifting ? todo?
}

int address_create() {
  #ifdef LOG
  logPrintf("[INFO] %i: address_create()", getGametime());
  #endif
  
  flag_address = 1; // only set here!
  addresspos = 0;
  tempaddress = hex_adr;
  return 0;
}

int address_draw() {
  drawUiBox(120.0f, 82.0f, 240.0f,  22.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // header (x, y, width, height, border, color, color)  
  drawUiBox(120.0f, 82.0f, 240.0f, (tempaddress > mod_text_addr) ? (LCS ? 130.0f : 175.0f) : (LCS ? 85.0f : 130.0f), 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // main
  
  drawString(translate_string("Enter Address"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 200.0f, 85.0f, COLOR_TITLE);
  //drawBox(197.0f, 100.0f, 100.0f, 1.0f, COLOR_TITLE); // x, y, width, height, color
  
  /// draw physical address
  unsigned int i = 0, x = 320, y = 130, temp = tempaddress; // x = 290
  drawString(translate_string("Physical:"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-170), (float)y, COLOR_TEXT);
  while( temp ) {
    snprintf(buffer, sizeof(buffer), "%X", temp % 0x10);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-=10), (float)y, COLOR_TEXT);
    if( i == addresspos ) { //cursor
      drawString("v", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+2, y-15, COLOR_TEXT);
      drawString("^", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+1, y+15, COLOR_TEXT);
    } temp /= 0x10; i++;
  } drawString("0x 0", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-=30), (float)y, COLOR_TEXT);
  
  
  /// draw text_addr depending address
  if( tempaddress > mod_text_addr ) {
    i = 0, x = 320, y += 45, temp = tempaddress - mod_text_addr;
    drawString("text_addr:", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-170), (float)y, YELLOW);
    while( temp ) {
      snprintf(buffer, sizeof(buffer), "%X", temp % 0x10); 
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-=10), (float)y, YELLOW);
      if( i == addresspos ) { // cursor
        drawString("v", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+2, y-15, YELLOW);
        drawString("^", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+1, y+15, YELLOW);
      } temp /= 0x10; i++;
    } drawString("0x", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-=20), (float)y, YELLOW);
  }
  
    
  /// draw $gp depending address
  if( VCS ) {
    i = 0, x = 320, y += 45, temp = tempaddress - gp;
    drawString(translate_string("To $gp:"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-170), (float)y, ORANGE);
    while( temp ) {
      snprintf(buffer, sizeof(buffer), "%X", temp % 0x10); 
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-=10), (float)y, ORANGE);
      if( i == addresspos ) { // cursor
        drawString("v", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+2, y-15, ORANGE);
        drawString("^", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+1, y+15, ORANGE);
      } temp /= 0x10; i++;
    } drawString("0x", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)(x-=20), (float)y, ORANGE);
  }
  
  
  #ifdef DEBUG
  if( flag_draw_DBGVALS ) {
    /// draw info
    snprintf(buffer, sizeof(buffer), "addresspos = %i", addresspos);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 100.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "phy: 0x%08X", tempaddress);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 140.0f, RED);
      
    snprintf(buffer, sizeof(buffer), "mod: 0x%08X", tempaddress - mod_text_addr);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 160.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "$gp: 0x%08X", tempaddress  - gp);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 350.0f, 180.0f, RED);
  
      
    /// draw history
    y = 20.0f;
    snprintf(buffer, sizeof(buffer), "ADR_HISTORY_SIZE = %i", ADR_HISTORY_SIZE);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, y+=15, RED);
    snprintf(buffer, sizeof(buffer), "history_position = %i", history_position);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, y+=15, RED);
    for( i = 0; i < ADR_HISTORY_SIZE && i < 12 && adr_history[i] != 0; i++ ) { // max of about 12
      snprintf(buffer, sizeof(buffer), "%i - 0x%08X", i, adr_history[i]);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 30.0f, y+=15, RED);
    }
  }  
  #endif
  
  //drawString("CROSS: Open Address", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 100.0f, 200.0f, COLOR_TEXT);
  //drawString("CIRCLE: Abort", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 300.0f, 200.0f, COLOR_TEXT);

  return 0;
}

int address_ctrl() {
  if( hold_buttons & PSP_CTRL_LEFT ) {
    if( addresspos < 6 ) 
      addresspos++;
  }
  
  if( hold_buttons & PSP_CTRL_RIGHT ) {
    if( addresspos > 0 ) 
      addresspos--;
  }
  
  if( hold_buttons & PSP_CTRL_DOWN ) {
    tempaddress -= pow(0x10, addresspos); // addresspos 0: -= 0x1, 1: -= 0x10, 2: -= 0x100, ....
    if( tempaddress <= memory_low ) 
      tempaddress = memory_low;
    history_position = 0;
  }
  
  if( hold_buttons & PSP_CTRL_UP ) {
    tempaddress += pow(0x10, addresspos); // addresspos 0: += 0x1, 1: += 0x10, 2: += 0x100, ....
  if ( tempaddress >= memory_high ) 
      tempaddress = memory_high;
    history_position = 0;
  }
  
  if( pressed_buttons & PSP_CTRL_SQUARE ) {
    tempaddress = mod_text_addr + mod_text_size + mod_data_size;
    history_position = 0;
  }
  
  if( pressed_buttons & PSP_CTRL_SELECT ) { // ultra secret backdoor lul
    //category_index[CAT_DEBUG] = DEBUG = 1 - DEBUG; // outdated - but works for 1.0e and below ;)
  }
  
  if( pressed_buttons & PSP_CTRL_LTRIGGER ) {
    if( history_position == 0 && adr_history[0] != 0 ) // make tempaddr histroy[0]
      add_adr_to_history(tempaddress);
    if( history_position < ADR_HISTORY_SIZE-1 && adr_history[history_position+1] != 0 )
      tempaddress = adr_history[++history_position];
  }
  
  if( pressed_buttons & PSP_CTRL_RTRIGGER ) {
    if( history_position > 0 ) 
      tempaddress = adr_history[--history_position];    
  }
    
  if( pressed_buttons & PSP_CTRL_CROSS ) {  
    flag_address = 0;
    history_position = 0;
    add_adr_to_history(tempaddress);
    hex_adr = tempaddress;
    hexeditor_create(hex_adr, 0, memory_low, memory_high, "");
  }
  if( pressed_buttons & PSP_CTRL_CIRCLE ) {
    history_position = 0;
    flag_address = 0;
  }
    
  return 0;  
}




/********************************************************************************************************
*
* Edit Byte for HexEditor
*
*********************************************************************************************************/
short flag_editbyte = 0;
unsigned char editbyte_current = -1;
unsigned char editbyte_pos = -1;

int editbyte_create() {
  flag_editbyte = 1; // only set here!
  editbyte_current = getByte(hex_adr);
  editbyte_pos = 1;
  return 0;
}

int editbyte_draw() {
  drawUiBox(180.0f, 97.0f, 120.0f, 22.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // header (x, y, width, height, border, color, color)
  drawUiBox(180.0f, 97.0f, 120.0f, 75.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // main
  
  drawString(translate_string("Edit Byte"), ALIGN_SCREENCENTER, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 0.0F, 100.0F, COLOR_TITLE);
  
  int x = 225, y = 140;

  snprintf(buffer, sizeof(buffer), "0x%02X", editbyte_current);
  drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, (float)x, (float)y, COLOR_TEXT);

  if( LCS ) // graphical fix
    x += 2; 

  drawString("v", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+(editbyte_pos*10)+14, y-15, COLOR_TEXT);
  drawString("^", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x+(editbyte_pos*10)+14, y+15, COLOR_TEXT);

  return 0;
}

int editbyte_ctrl() {
  if( hold_buttons & PSP_CTRL_LEFT ) editbyte_pos = 0;
  if( hold_buttons & PSP_CTRL_RIGHT ) editbyte_pos = 1;

  if( hold_buttons & PSP_CTRL_DOWN ) {
    if( editbyte_pos == 0 ) {
      if( editbyte_current < 0x10 ) 
        editbyte_current = 0x00;
      else editbyte_current -= 0x10;
    } else {
      if( editbyte_current > 0x00 ) 
        editbyte_current -= 0x1;
    }  
  }
  if( hold_buttons & PSP_CTRL_UP ) {
    if( editbyte_pos == 0 ) {
      if( editbyte_current >= 0xF0 ) 
        editbyte_current = 0xFF;
      else editbyte_current += 0x10;
    } else {
      if( editbyte_current < 0xFF ) 
        editbyte_current += 0x1;
    }
  }
  if( pressed_buttons & PSP_CTRL_CROSS ) {  
    setByte(hex_adr, editbyte_current);
    clearICacheFor(hex_adr); // for PPSSPP
    flag_editbyte = 0;
  }
  if( pressed_buttons & PSP_CTRL_CIRCLE ) {  
    flag_editbyte = 0;
  }
    
  return 0;  
}




/********************************************************************************************************
*
* Hex Editor
*
* (uses around 500 strings drawn!)
*
* TODO: mode 1 with address higher than low bound and not ending with 0x....0
*
*********************************************************************************************************/
short flag_hexeditor = 0;

/// settings
int hexeditor_maxlines   = 14;      // the maximum number of lines that can be displayed - considering max text possible by the game is 512 we are pretty maxed out already
int hexeditor_colordelay = 3000000; // delay for changed value color (3 seconds)

/// set & used by functions
int hexeditor_mode;      // 0 = use full memory location, 1 = start from 0x00000000 ( = lower bound)
int hexeditor_lowbound;  // lowest memory address allowed to be displayed
int hexeditor_highbound; // highest memory address allowed to be displayed
char hexeditor_infobuffer[64];            // holds the additional string info of location displayed next to Headline
unsigned char hexeditor_memory[14][0x10]; // copy of displayed hex memory for comparison [lines][colums]
int hexeditor_memtime[14][0x10];          // holds the time of hex value for color stuff [lines][colums]
int hexeditor_lines;   // current number of lines that are displayed
int hexeditor_address; // the addresses displayed on the left on mode 0 (actual memory address)
int hexeditor_showadr; // the addresses displayed on the left on mode 1 (0x0000000 = starting address)
int hexeditor_browse_x; // cursor x
int hexeditor_browse_y; // cursor y

//"hex_adr" is physical address cursor is at

int hex_addressmode = 0; // 0 = physical address, module load address, global pointer register


#ifdef HEXMARKERS
#define MARKERS 128

int hex_markers[MARKERS];
  
void hex_marker_add(u32 address) {
  int i;
  for( i = 0; i < MARKERS; i++ ) {
    if( hex_markers[i] == address ) 
      return;

    if( hex_markers[i] == 0 ) { // found free spot
      hex_markers[i] = address;
      return;
    }
  }

  for( i = MARKERS-1; i > 0; i-- ) { // array full -> move by 1
    hex_markers[i] = hex_markers[i-1]; 
  } hex_markers[0] = address;
}

void hex_marker_addx(u32 address, int size) {
  int i;
  for( i = 0; i < size; i++ )
    hex_marker_add(address+i);
}

int hex_marker_check(u32 address) {
  int i;
  for( i = 0; i < MARKERS; i++ ) {
    if( hex_markers[i] == address ) {
      return 1;
    }
  }
  return 0;
}

void hex_marker_remove(u32 address) {
  int i;
  for( i = 0; i < MARKERS; i++ ) {
    if( hex_markers[i] == address ) {
      hex_markers[i] = 0;
      return;
    }
  }
}  

void hex_marker_clear() {
  int i;
  for( i = 0; i < MARKERS; i++ ) 
    hex_markers[i] = 0;
}  

#endif
  
  
int hexeditor_create(int address, int mode, int low, int high, const char *infostring) { // init HexEditor
  #ifdef LOG
  logPrintf("[INFO] %i: hexeditor_create(%s)", getGametime(), infostring);
  #endif
  
  if( address >= memory_low && address <= memory_high ) 
    hex_adr = address;
  else return -1;
  
  hexeditor_mode = mode;
  hexeditor_lowbound = low;
  hexeditor_highbound = high;
  snprintf(hexeditor_infobuffer, sizeof(hexeditor_infobuffer), "%s", translate_string(infostring));
  
  hexeditor_lines = (hexeditor_highbound - hexeditor_lowbound) / 0x10;
  if( ((hexeditor_highbound - hexeditor_lowbound) % 0x10) > 0 )
    hexeditor_lines++;
  if( hexeditor_lines > hexeditor_maxlines )
    hexeditor_lines = hexeditor_maxlines;
  
  /** hexeditor_mode **********************************************
  
  = 0 shows normal physical address 

  = 1 sets "low" = 0x00000000 and scrolls to address
  
    if low != 0x0*****0 (eg: 0x0****C or even ODD like 0x0****3) 
    everything is shifted. (Potentially dangerous when ODD though)
  
    if the block (low to high) fits to one page and address is not 
    0x000000 no scrolling. Sets cursor x/y instead!
    
  *****************************************************************/  
  
  if( hexeditor_mode == 0 ) { // address mode
    hexeditor_address = (hex_adr/0x10)*0x10; // get rid of last hex 0xX
    hexeditor_browse_x = hex_adr - hexeditor_address;
    hexeditor_browse_y = 0;
  }
  
  if( hexeditor_mode == 1 ) { // block mode
    hexeditor_showadr = 0x00000000;
    
    if( hexeditor_lines < hexeditor_maxlines ) { // whole block fits to page! -> set cursor
      hexeditor_address = hexeditor_lowbound;
      hexeditor_browse_x = (hex_adr - hexeditor_lowbound) % 0x10;
      hexeditor_browse_y = (hex_adr - hexeditor_lowbound) / 0x10;
      
    } else { // block is bigger than what can be displayed at once
      if( (((hex_adr - hexeditor_lowbound) / 0x10) + 2) < hexeditor_maxlines ) { // value's offset "can be seen" without scrolling (with buffer though, it ignores the last 2 lines) -> set cursor
        hexeditor_address = hexeditor_lowbound;
        hexeditor_browse_x = (hex_adr - hexeditor_lowbound) % 0x10;
        hexeditor_browse_y = (hex_adr - hexeditor_lowbound) / 0x10;
        
      } else { // value's offset is further down in the block -> "scroll"
        hexeditor_address = (hex_adr/0x10)*0x10; // get rid of last hex 0xX
        hexeditor_showadr = ((address-low)/0x10)*0x10;
        hexeditor_browse_x = hex_adr - hexeditor_address;
        hexeditor_browse_y = 0;
      }
    }
  }

  memset(hexeditor_memory, 0, sizeof(hexeditor_memory[0][0]) * hexeditor_lines * 0x10);
  memset(hexeditor_memtime, 0, sizeof(hexeditor_memtime[0][0]) * hexeditor_lines * 0x10);
  
  /// high bound handling
  if( hexeditor_address >= hexeditor_highbound-(hexeditor_lines*0x10) ) {
    if( hexeditor_mode == 0 )
      hexeditor_address = hexeditor_highbound-(hexeditor_lines*0x10); 
  }
  
  flag_hexeditor = 1; // only set here!
  return 0;
}

int hexeditor_draw() {
  
  /// settings
  int hex_spaceing   = 18;
  int ascii_spaceing = 6;
  int line_spaceing  = (flag_use_legend) ? 12 : 14;
  int vstart      = 50;  // vertical start of first line
  int hstarthex   = 80;  // horizontal start of hex values
  int hstartascii = 375;  // horizontal start of ascii
  
  int i, y, x1, x2;
  int counter, scounter; // lines & bytes per line
  u32 SOMECOLOR;
  unsigned char current = 0x00;
  int current2 = 0;
  
  
    hex_adr = (hexeditor_address+(hexeditor_browse_y*0x10)+(hexeditor_browse_x*0x01)); // update address depending on cursor position
    
    /// draw Title
    drawString(translate_string("HexEditor"), ALIGN_FREE, FONT_DIALOG, SIZE_BIG, SHADOW_OFF, 8.0f, 5.0f, COLOR_HEX);
    
    
    /// draw "infostring"
    drawString(hexeditor_infobuffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 100.0f, 8.0f, COLOR_TEXT);
    
  
    /// if arrays are empty (cleared via navigating for example) fill them with the new values once
    if( hexeditor_memtime[0][0] == 0 ) { 
      counter=0;
      while( counter < hexeditor_lines ){
        scounter=0;
        while( scounter < 0x10 ) {
          hexeditor_memory[counter][scounter] = *((unsigned char*)((hexeditor_address+(counter*0x10))+scounter));
          hexeditor_memtime[counter][scounter] = sceKernelGetSystemTimeLow() - hexeditor_colordelay * 2;
          scounter++;
        } counter++;
      }
    } 
    
    
    y = vstart, x1 = hstarthex, x2=hstartascii;
    
    /// draw UI box
    drawUiBox( 5.0f, y-2.0f, 470.0f, (flag_use_legend) ? 171.0f : 199.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); //main  //x, y, width, height, border, color, color

    /// draw current address
    COLOR_TEMP = RED;
    if( hex_addressmode == 1 ) {
      snprintf(buffer, sizeof(buffer), "%X", hex_adr - mod_text_addr);
      COLOR_TEMP = YELLOW;
    } else if( hex_addressmode == 2 ) {
      snprintf(buffer, sizeof(buffer), "%X", hex_adr - gp);
      COLOR_TEMP = ORANGE;
    } else snprintf(buffer, sizeof(buffer), "%X", hex_adr);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 8, y-line_spaceing-8, COLOR_TEMP);
    
    /// draw 0 to F legend
    for( i = 0x0; i <= 0xF; i++, x1+=hex_spaceing ) {
      snprintf(buffer, sizeof(buffer), "%X", i);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x1+5, y-line_spaceing-8, COLOR_TEXT); // *16
    } x1 = hstarthex;
    
    /// draw "ASCII" text
    drawString("ASCII", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x2, y-line_spaceing-8, COLOR_TEXT);
    
    counter = 0;
    while( counter < hexeditor_lines ){
          
      /// print address (left)
      if( hexeditor_mode ) {  // for editor mode 1 and 2
        snprintf(buffer, sizeof(buffer), "0x%08X", (hexeditor_showadr+(counter*0x10)) ); // draw 0x000000+ 
  
     } else {
        if( hex_addressmode == 1 ) 
          snprintf(buffer, sizeof(buffer), "0x%08X", (hexeditor_address+(counter*0x10)) - mod_text_addr ); // display real address "0x%08X" [mod_text_addr]
        else if( hex_addressmode == 2 ) 
          snprintf(buffer, sizeof(buffer), "0x%08X", (hexeditor_address+(counter*0x10)) - gp ); // display real address "0x%08X" [gp register]
        else 
          snprintf(buffer, sizeof(buffer), "0x%08X", (hexeditor_address+(counter*0x10)) ); // display physical address "0x%08X"
         
     } drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_HEXEDIT, SHADOW_OFF, 8, y, COLOR_TEXT);
      
      
    /// print hex & ascii
    scounter = 0;
    while( scounter < 0x10 && ((hexeditor_address + ( (counter*0x10) + scounter ) ) < hexeditor_highbound ) ) {
      current = *((unsigned char*)((hexeditor_address+(counter*0x10))+scounter));
      
      if( (hexeditor_address % 0x10) == 0x0 && (scounter == 0x0 || scounter == 0x4 || scounter == 0x8 || scounter == 0xC) ) // before && is fix for odd first byte in hexeditor_mode 1 -> disable pointer detection
        current2 = *((int*)((hexeditor_address+(counter*0x10))+scounter)); // for pointer check;
         
      if( hexeditor_memory[counter][scounter] != current ) 
        hexeditor_memtime[counter][scounter] = sceKernelGetSystemTimeLow();
            
        /// color for currently selected hex
        if( counter == hexeditor_browse_y && scounter == hexeditor_browse_x ) {
          SOMECOLOR = RED; // its the selected value #color_adjust
        } else {
          
          /// check if the value has changed (for color adjustment)
          if( (sceKernelGetSystemTimeLow() <= hexeditor_memtime[counter][scounter] + hexeditor_colordelay) ) {
            SOMECOLOR = WHITE;
          
          /// check if matches pplayer address
          } else if( current2 == pplayer ) {
            SOMECOLOR = CYAN;
          
          /// check if matches pcar address
          } else if( pcar && current2 == pcar ) {
            SOMECOLOR = BLUE;
          
          } else if( pcar && current2 == pcar ) {
            SOMECOLOR = BLUE;
            
          /// check if pointer (for different color)
          } else if( current2 > memory_low && current2 < memory_high) {
            
            if( current2 >= mod_text_addr && current2 <= mod_text_addr + mod_text_size ) //pointer to TEXT area
              SOMECOLOR = ORANGE;
            else
              SOMECOLOR = YELLOW;
          
          /// normal
          } else 
            SOMECOLOR = COLOR_VALUE;
        }  


      #ifdef HEXMARKERS
      /// draw markers
      for( i = 0; i < MARKERS; i++ ) {
        if( hex_markers[i] == ((hexeditor_address+(counter*0x10))+scounter)) {
          drawBox(x1-2.0f, y + (LCS ? 0 : 1.5f), 18, 11, COLOR_HEX_MARKER); // on hex
          //drawBox(x2-2, y, 8, 11, COLOR_HEX_MARKER); // on ascii
          break;
        }
      }
      #endif

      /// draw hex      
      snprintf(buffer, sizeof(buffer), "%02X", current );
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_HEXEDIT, SHADOW_OFF, x1, y, SOMECOLOR); // todo color
      
      hexeditor_memory[counter][scounter] = current; // save same or new value in memory array
      
      if( (current <= 0x20) || (current >= 0x7E) ) 
        current = '.'; //replace none exisiting (+ FIX -> displaying 0x7E on LCS crashes game!)
        
      /// color for currently selected ascii
      if( counter == hexeditor_browse_y && scounter == hexeditor_browse_x ) {
        SOMECOLOR = RED; // its the selected value (color_adjust)
      } else SOMECOLOR = COLOR_TEXT; // normal
      
      ///draw Ascii
      snprintf(buffer, sizeof(buffer), "%c", current);
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_HEXEDIT, SHADOW_OFF, x2, y, SOMECOLOR); //draw if in bound
      
      scounter++;
      x1 += hex_spaceing; // hex spacing
      x2 += ascii_spaceing; // ascii spacing  
    } 
    y += line_spaceing;
    x2 = hstartascii;
    x1 = hstarthex;
    counter++;
  } 
    
  ////////////////////////////////////////////////  
  
  /// current values
  if( flag_use_legend ) {
    drawString(translate_string("short:"),   ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF,  50.0f, 222.0f, COLOR_TEXT);
    drawString(translate_string("integer:"), ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF,  50.0f, 233.0f, COLOR_TEXT);
    drawString(translate_string("float:"),   ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF,  50.0f, 244.0f, COLOR_TEXT);
    drawString(translate_string("address:"), ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF,  50.0f, 255.0f, COLOR_TEXT);
  } else {
    drawString(translate_string("short:"),   ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF,  40.0f, 255.0f, COLOR_TEXT);
    drawString(translate_string("integer:"), ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 150.0f, 255.0f, COLOR_TEXT);
    drawString(translate_string("float:"),   ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 260.0f, 255.0f, COLOR_TEXT);
    drawString(translate_string("address:"), ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 400.0f, 255.0f, COLOR_TEXT);
  } 

  switch( hex_adr % 0x10 ) {
    case 0x0: case 0x4: case 0x8: case 0xC:
      snprintf(buffer, sizeof(buffer), "%d", *((short*)(hex_adr)) );
      if( flag_use_legend ) 
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 56.0f, 222.0f, RED);
      else     
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 45.0f, 255.0f, RED);
      
      snprintf(buffer, sizeof(buffer), "%d", *((int*)(hex_adr)) );
      if( flag_use_legend ) 
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 56.0f, 233.0f, RED);
      else     
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 155.0f, 255.0f, RED);
      
      snprintf(buffer, sizeof(buffer), "%f", *((float*)(hex_adr)));
      if( flag_use_legend ) 
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 56.0f, 244.0f, RED);
      else
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 265.0f, 255.0f, RED);
      
      
      if( *((int*)(hex_adr)) > memory_low && *((int*)(hex_adr)) < memory_high) { // link to accessible memory
        snprintf(buffer, sizeof(buffer), "0x%08X", *((int*)(hex_adr)) );
        if( flag_use_legend ) 
          drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 56.0f, 255.0f, RED);
        else
          drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 405.0f, 255.0f, RED);
      }
      break;
      
    case 0x2: case 0x6: case 0xA: case 0xE:
      snprintf(buffer, sizeof(buffer), "%d", *((short*)(hex_adr)) );
      if( flag_use_legend ) 
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 56.0f, 222.0f, RED);
      else     
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 45.0f, 255.0f, RED);
      break;
  }

  
  /// button legend
  if( flag_use_legend && !flag_draw_DBGVALS ) {
    #ifndef DEBUG
    drawBox(320, 220, 155, 50, COLOR_BACKGROUND);
    drawString(translate_string("CROSS: Edit selected Byte"),  ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 325.0f, 222.0f, COLOR_TEXT);
    drawString(translate_string("SQUARE: Set Byte to 0x00"),   ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 325.0f, 233.0f, COLOR_TEXT);
    drawString(translate_string("TRIANGLE: Open Address"),     ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 325.0f, 244.0f, COLOR_TEXT);
    drawString(translate_string("CIRCLE: Exit HexEditor"),     ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 325.0f, 255.0f, COLOR_TEXT);
    
    drawBox(160, 220, 155, 50, COLOR_BACKGROUND);
    drawString(translate_string("R + SQUARE: Zero 4 Bytes"),   ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 165.0f, 233.0f, COLOR_TEXT);
    drawString(translate_string("R + TRIANGLE: Teleport xyz"), ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 165.0f, 244.0f, COLOR_TEXT);
    drawString(translate_string("SELECT: Cycle Baseadr."),     ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 165.0f, 255.0f, COLOR_TEXT);
    #ifdef HEXMARKERS
    drawString(translate_string("R + CROSS: Mark selected"),   ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 165.0f, 222.0f, COLOR_TEXT);
    #endif
   #endif
  }
  
  
  #ifdef DEBUG
  if( flag_draw_DBGVALS ) {
    snprintf(buffer, sizeof(buffer), "hexeditor_lines = %i", hexeditor_lines );
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 250.0f, 222.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "hexeditor_address = 0x%08X", hexeditor_address );
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 250.0f, 233.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "hexeditor_showadr = 0x%08X", hexeditor_showadr );
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 250.0f, 244.0f, RED);
            
    snprintf(buffer, sizeof(buffer), "hex_adr = 0x%08X", hex_adr );
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 250.0f, 255.0f, RED);

    /*int u, e = hstartascii, r = 80;
    for(u = 0; u < MARKERS; u++, r+=20) {
      snprintf(buffer, sizeof(buffer), "%d: 0x%08X", u, hex_markers[u] );
      drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, e, r, ORANGE);
    }*/
  }
  #endif
  
  return 0;
  
}

int hexeditor_ctrl() {
    if( hold_buttons & PSP_CTRL_DOWN ) {
      if( hexeditor_browse_y == hexeditor_lines-1 && (hexeditor_address+(hexeditor_lines*0x10) < hexeditor_highbound) ) {
        hexeditor_address+=0x10;
        hexeditor_showadr+=0x10;
        memset(hexeditor_memory, 0, sizeof(hexeditor_memory[0][0]) * hexeditor_lines * 0x10);
        memset(hexeditor_memtime, 0, sizeof(hexeditor_memtime[0][0]) * hexeditor_lines * 0x10);        
        while( (hexeditor_address + (hexeditor_browse_y*0x10) + hexeditor_browse_x) >= hexeditor_highbound ) // bound_handling
          hexeditor_browse_x--; 
        if ( hexeditor_browse_x < -1 ) { // bound error handling 
          hexeditor_browse_x = 0; 
          hexeditor_browse_y--; 
        } 
          
      } else {
        if( hexeditor_browse_y != hexeditor_lines-1 ) {
          hexeditor_browse_y++;
          while( (hexeditor_address + (hexeditor_browse_y*0x10) + hexeditor_browse_x) >= hexeditor_highbound ) 
            hexeditor_browse_x--; // bound_handling
          if ( hexeditor_browse_x < -1 ) { // bound error handling
            hexeditor_browse_x = 0; hexeditor_browse_y--; 
          } 
        }
      }
    }
    
    if( hold_buttons & PSP_CTRL_UP ) { 
      if( hexeditor_browse_y == 0 && hexeditor_address+hexeditor_browse_y > hexeditor_lowbound ) {
        hexeditor_address-=0x10;
        hexeditor_showadr-=0x10;
        memset(hexeditor_memory, 0, sizeof(hexeditor_memory[0][0]) * hexeditor_lines * 0x10);
        memset(hexeditor_memtime, 0, sizeof(hexeditor_memtime[0][0]) * hexeditor_lines * 0x10);
      } else {
        if( hexeditor_address+hexeditor_browse_y > hexeditor_lowbound ) 
          hexeditor_browse_y--;
      }
    }
    
    if( hold_buttons & PSP_CTRL_LEFT ) {
        if( hexeditor_browse_x == 0 ) {
          hexeditor_browse_x = 0x10-1;
          while( (hexeditor_address + (hexeditor_browse_y*0x10) + hexeditor_browse_x) >= hexeditor_highbound ) // bound handling
            hexeditor_browse_x--; 
          if( hexeditor_browse_x < -1 ) { // bound error handling
            hexeditor_browse_x = 0; 
            hexeditor_browse_y--; 
          } 
        } else hexeditor_browse_x--;
        
        
    }
    if( hold_buttons & PSP_CTRL_RIGHT ) {
      if( (hexeditor_address + (hexeditor_browse_y*0x10) + hexeditor_browse_x+1) < hexeditor_highbound ){ // only allow go right when there is a value next to it (bound handling)
        if( hexeditor_browse_x == 0x10-1 ) 
          hexeditor_browse_x = 0;
        else hexeditor_browse_x++;
      } else hexeditor_browse_x = 0;
    }
    
    
    if( pressed_buttons & PSP_CTRL_SELECT ) {
      if( hex_addressmode < (LCS ? 1 : 2) ) 
        hex_addressmode++;
      else hex_addressmode = 0;
    }
    
    #ifdef HEXMARKERS
    if( pressed_buttons & PSP_CTRL_CROSS ) {
      if( current_buttons & PSP_CTRL_RTRIGGER ) { // mark
        if( hex_marker_check(hex_adr) ) 
          hex_marker_remove(hex_adr);
        else hex_marker_add(hex_adr);
        //setTimedTextbox("Test in hex! delete", 3);
      } else editbyte_create(); // edit byte
    }
    #endif
    
    if( hold_buttons & PSP_CTRL_SQUARE ) { // set zero
      if( current_buttons & PSP_CTRL_RTRIGGER ) {
        setByte(hex_adr+0x0, 0x00);
        setByte(hex_adr+0x1, 0x00);
        setByte(hex_adr+0x2, 0x00);
        setByte(hex_adr+0x3, 0x00);
      } else setByte(hex_adr, 0x00); // set zero
      clearICacheFor(hex_adr); // for PPSSPP
    }
    
    
    /// open location
    if( pressed_buttons & PSP_CTRL_TRIANGLE ) {      
      
      if( current_buttons & PSP_CTRL_RTRIGGER ) { // teleport to location
        if( hex_adr % 0x4 == 0x0 )
          teleport( getFloat(hex_adr), getFloat(hex_adr+0x4), getFloat(hex_adr+0x8) ); // Note: no value check!
      
      } else { // open pointer location
        if( hex_adr % 0x4 == 0x0 ) {
          if( getInt(hex_adr) > memory_low && getInt(hex_adr) < memory_high) { // pointer to accessible memory
            
            /* old version (drop mode 1)
            hex_adr = getInt(hex_adr);
            hexeditor_address=(hex_adr/0x10)*0x10; //get rid of last hex 0xX
            hexeditor_browse_x = hex_adr-hexeditor_address;
            hexeditor_browse_y = 0;
            hexeditor_mode = 0;
            
            hexeditor_lowbound = memory_low;
            hexeditor_highbound = memory_high;*/
            
            memset(hexeditor_memory, 0, sizeof(hexeditor_memory[0][0]) * hexeditor_lines * 0x10);
            memset(hexeditor_memtime, 0, sizeof(hexeditor_memtime[0][0]) * hexeditor_lines * 0x10);
            
            add_adr_to_history(hex_adr); // add address to history array
            sprintf(hexeditor_infobuffer, "> via offset at 0x%08X", hex_adr);
            hexeditor_create(getInt(hex_adr), 0, memory_low, memory_high, hexeditor_infobuffer);
          }
        }
      
      }
    }
    
  if( pressed_buttons & PSP_CTRL_CIRCLE ) {  
    flag_hexeditor = 0;
  }
      
  return 0;  
}
#endif


/// These functions are 'original display settings -save' and only reset if enabled there!
void hideHUD() {
  if( getDisplaySettingsToggleHUD() ) {
    setDisplaySettingsToggleHUD(OFF);
    flag_hudwashidden = 1;
  }
}

void showHUD() { // only show if it was enabled before
  if( flag_hudwashidden == 1 ) {
    setDisplaySettingsToggleHUD(ON); // re-enable
    flag_hudwashidden = 0;
  }
}

void hideRadar() {
  if( getDisplaySettingsToggleRadar() ) {
    setDisplaySettingsToggleRadar(OFF);
    flag_mapwashidden = 1;
  }
}

void showRadar() { // only show if it was enabled before
  if( flag_mapwashidden == 1 ) {
    setDisplaySettingsToggleRadar(ON); // re-enable
    flag_mapwashidden = 0;
  }
}



void draw() { // called by hijacked game function
  flag_menu_show = 1; // menu is allowed
  
  /// add 'watermark'
  #ifdef DEBUG
  drawString("DEBUG", ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 478.0f, 0.0f, RED);
  #endif
  
  /// add 'watermark'
  #ifdef PREVIEW
  drawString("PREVIEW", ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 478.0f, 0.0f, WHITE);
  #endif
  

  /// draw welcome message in textbox (only when menu autostart is disabled!)
  if( flag_draw_welcomsg ) {
    #ifdef LOG
    logPrintf("[INFO] %i: drawing welcome message", getGametime());
    #endif  
    setTimedTextbox(translate_string(welcomemsg), 7.00f);
    flag_draw_welcomsg = 0;
  }

  /// draw Menu and everything that uses button input
  if( flag_menu_running == 1 && flag_menu_show == 1 ) {
    
    /// reset hidden UI elements (for other 'menus')
    if( ( 
      #ifdef FREECAM
      !flag_freecam && 
      #endif
      
      #ifdef PREVIEW
      !flag_usercheats && 
      #endif
      
      #ifdef HEXEDITOR
      !flag_hexeditor && 
      #endif
      
      #ifdef EDITORS
      !flag_editor &&
      #endif
      
      #ifdef USERSCRIPTS
      !flag_userscripts &&
      #endif
      
      1 ) // in case all are disabled
      
      #ifdef HEXEDITOR
      || flag_editbyte 
      #endif 
      
      ) {
        
      showHUD();
      showRadar();
    }
    
    /// decide menu to draw
    if( 0 ) { // never true (because ifdefs..)
    
    #ifdef HEXEDITOR
    } else if( flag_address ) // enter address dialog
      address_draw();
      
    else if( flag_editbyte ) // editbyte dialog
      editbyte_draw();
      
    else if( flag_hexeditor ) { // hex editor
      if( flag_ui_blocking ) {
        blockTextBox();
        blockDialogs();
        //blockLocationText(); //TODO
        hideHUD();
        hideRadar();
      } hexeditor_draw();
    #endif
    
    #ifdef FREECAM
    } else if( flag_freecam ) { // freecam
      if( flag_ui_blocking ) {
        blockTextBox();
        blockDialogs();
        //blockLocationText();
        hideHUD();
        hideRadar();
      } freecam_draw();
    #endif
    
    #ifdef EDITORS
    } else if( flag_editor ) { // editors
      if( flag_ui_blocking ) {
        blockTextBox();
        blockDialogs();
        //blockLocationText();
        hideHUD();
        hideRadar();
      } editor_draw();
    #endif
    
    #ifdef PREVIEW
    } else if( flag_usercheats ) { // usercheats
      if( flag_ui_blocking ) {
        blockTextBox();
        blockDialogs();
        //blockLocationText();
        hideHUD();
        hideRadar();
      } usercheats_draw();
    #endif
    
    #ifdef USERSCRIPTS
    } else if( flag_userscripts ) { // userscripts
      if( flag_ui_blocking ) {
        blockTextBox();
        blockDialogs();
        //blockLocationText();
        hideHUD();
        hideRadar();
      } userscripts_draw();
    #endif
    
    } else { // Main Menu
      if( flag_ui_blocking ) {
        blockTextBox();
        blockDialogs();
        //blockLocationText();
        hideRadar();
      } menu_draw(main_menu, menu_size);
    }  
        
  } else { // reset hidden UI elements for good (if they were hidden)
    showHUD();
    showRadar();
  }


  /// SpeedOmeter (when enabled)
  if( pcar && flag_draw_SPEEDO == 1 && flag_menu_running == 0 && !isHudDisabledWhileCutsceneCamera() && flag_draw_DEBUG == 0 && isDialogShowing() == 0 ) {
    float speedo_x = 15.0f; //default (left bound to screen)
    
    if( getDisplaySettingsToggleRadar() ) //minimap true
      speedo_x = multiplayer ? 120.0f : 90.0f; //fix for higher minimap in mp
    
    drawString(speed, ALIGN_FREE, FONT_NAMES, SIZE_BIG, SHADOW_ON, speedo_x, 225.0f, WHITE);
    drawString( gear, ALIGN_FREE, FONT_NAMES, SIZE_BIG, SHADOW_ON, speedo_x, 245.0f, WHITE);
  }
  
  
  /// free Memory (when enabled)
  if( flag_draw_MEM == 1 && flag_menu_running == 0 && flag_draw_DEBUG == 0 && isTextboxShowing() == 0 ) {
  getSizeString(buffer, memory_main_free);
    drawString(translate_string("MEM:"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 8.0f, 5.0f, WHITE);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 48.0f, 5.0f, (memory_main_free < 150*1000 ? (memory_main_free < 100*1000 ? (memory_main_free < 50*1000 ? RED : ORANGE) : YELLOW) : GREEN) );
  }
  
  
  /// FPS indicator (when enabled)
  if( flag_draw_FPS == 1 && flag_menu_running == 0 && flag_draw_DEBUG == 0 && isTextboxShowing() == 0 ) {
    snprintf(buffer, sizeof(buffer), "%.0f", fps);
    drawString(translate_string("FPS:"), ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 8.0f, 5.0f + (flag_draw_MEM ? row_spacing : 0), WHITE);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 43.0f, 5.0f + (flag_draw_MEM ? row_spacing : 0), (fps < 30.0f ? (fps < 20.0f ? (fps < 15.0f ? RED : ORANGE) : YELLOW) : GREEN) );
  }
  
  
  /// Config saveing
  #ifdef DEBUG
  extern int saveing;
  if( saveing )
    drawString("Saving config..", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 18.0f, 15.0f, WHITE);
  #endif
  
  
  /// Coordinates (when enabled)
  if( flag_draw_COORDS == 1 && flag_draw_DEBUG == 0 ) {
    if( pplayer > 0 ) {
      snprintf(buffer, sizeof(buffer), "x: %.2f", getFloat(pplayer+0x30));
      drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 470.0f, 100.0f, COLOR_VALUE);
      snprintf(buffer, sizeof(buffer), "y: %.2f", getFloat(pplayer+0x34));
      drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 470.0f, 120.0f, COLOR_VALUE);
      snprintf(buffer, sizeof(buffer), "z: %.2f", getFloat(pplayer+0x38));
      drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 470.0f, 140.0f, COLOR_VALUE);
      snprintf(buffer, sizeof(buffer), "%.0f", getPedFacingDirectionInDegree(pplayer) );
      drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 470.0f, 165.0f, COLOR_VALUE);
     /* if( pcar ) { // beacause og CD
         snprintf(buffer, sizeof(buffer), "%.0f", getVehicleSpeed(pcar) * 100 );
         drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 470.0f, 190.0f, COLOR_VALUE);
     } */
    }
  }
  
}

void closeMenu() { // can be called by cheat (see teleport)
  #ifdef LOG
  logPrintf("[INFO] %i: closeMenu() called", getGametime());
  #endif
  flag_menu_running = 0; // trigger closing cheat device menu
  flag_keys_disable = 0; // free keys to work in-game again
  
  #ifdef CONFIG
  if( flag_use_liveconfig ) {
    //setTimedTextbox("Autosaving..", 3.00f); // 4 of 4 locations
    save_config(main_menu, menu_size);
  }
  #endif
}

void stopMenu() { // called by hijacked game function
  flag_menu_show = 0; // menu is not allowed
          
  if( flag_menu_running ) { // menu is open
    #ifdef LOG
    logPrintf("[INFO] %i: stopMenu() called", getGametime());
    #endif  
    flag_menu_running = 0; // stop and exit menu thread (if running)
    flag_keys_disable = 0; // reset to be able to use keys when menu closed
    flag_menu_start = 1;   // restart flag since the running menu was interrupted
    
    #ifdef CONFIG
    if( flag_use_liveconfig ) {
      //setTimedTextbox("Autosaving..", 3.00f); // 3 of 4 locations
      save_config(main_menu, menu_size);
    }
    #endif
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Rx Rsrv[0] // for readability
#define Ry Rsrv[1] // right stick support

/**
current_buttons = "currently pressed"
pressed_buttons = "on button down"
hold_buttons    = for scrolling
**/

void buttonInput() { // called by hijacked game function
  sceCtrlPeekBufferPositive(&pad, 1);
    
  /// Left Analog //////////////////////////////////////////
  xstick = (float)(pad.Lx - 128) / (float)128; 
  ystick = (float)(pad.Ly - 128) / (float)128;
  
  if( xstick < 0.25f && xstick > -0.25f ) 
    xstick = 0.00f; // fix
  
  if( ystick < 0.25f && ystick > -0.25f ) 
    ystick = 0.00f; // fix
  
  lx = pad.Lx - 128;
  ly = pad.Ly - 128;
    

  /// Right Analog ////////////////////////////////////////
  xstick_ = (float)(pad.Rx - 128) / (float)128; 
  ystick_ = (float)(pad.Ry - 128) / (float)128;
  
  if( xstick_ < 0.25f && xstick_ > -0.25f ) 
    xstick_ = 0.00f; // fix
  
  if( ystick_ < 0.25f && ystick_ > -0.25f ) 
    ystick_ = 0.00f; // fix
  
  if( xstick_ <= -0.99f && ystick_ <= -0.99f ) {
    xstick_ = 0.00f; // fix
    ystick_ = 0.00f; // fix
  }
  rx = pad.Rx - 128;
  ry = pad.Ry - 128;

  old_buttons = current_buttons;
  current_buttons = pad.Buttons;
  pressed_buttons = current_buttons & ~old_buttons;
  hold_buttons = pressed_buttons;

  if( old_buttons & current_buttons ) {
    if( hold_n >= 10 ) {
      hold_buttons = current_buttons;
      hold_n = 8;
    } hold_n++;
  } else hold_n = 0;

  if( flag_menu_running ) { // menu is open
                
    /// get input for control
    if( 0 ) { } // never true (because ifdefs..)

    #ifdef HEXEDITOR
    else if( flag_address ) // enter address dialog
      address_ctrl();
    
    else if( flag_editbyte ) // edit byte dialog
      editbyte_ctrl();
    
    else if( flag_hexeditor ) // hex editor
      hexeditor_ctrl();
    #endif
    
    #ifdef FREECAM  
    else if( flag_freecam ) // freecam
      freecam_ctrl();
    #endif  
      
    #ifdef EDITORS  
    else if( flag_editor ) // editor
      editor_ctrl();
    #endif
    
    #ifdef PREVIEW    
    else if( flag_usercheats ) // usercheats
      usercheats_ctrl();
    #endif
    
    #ifdef USERSCRIPTS
    else if( flag_userscripts ) // userscripts
      userscripts_ctrl();
    #endif
    
    else  // main menu
      menu_ctrl(main_menu, menu_size); 

    
    if( ((current_buttons & open_key ) == open_key) && current_buttons != old_buttons ) {
      #ifdef LOG
      logPrintf("[INFO] %i: closing menu via L + UP", getGametime());
      #endif  
      flag_menu_running = 0; // stop menu
      flag_keys_disable = 0; // set keys work in-game & for menu
      
      #ifdef CONFIG
      if( flag_use_liveconfig ) {
        //setTimedTextbox("Autosaving..", 3.00f); // 1 of 4 locations
        save_config(main_menu, menu_size);
      }
      #endif
    }
  
    if( ((current_buttons & open_key_alt ) == open_key_alt) && current_buttons != old_buttons ) {
      #ifdef LOG
      logPrintf("[INFO] %i: closing menu via L + DOWN", getGametime());
      #endif    
      flag_menu_running = 0; // stop menu
      flag_keys_disable = 0; // set keys work in-game & for menu
      
      #ifdef CONFIG
      if( flag_use_liveconfig ) {
        //setTimedTextbox("Autosaving..", 3.00f); // 2 of 4 locations
        save_config(main_menu, menu_size);
      }
      #endif
    }
        
  } else { // menu is closed
      
    if( flag_menu_show ) { // menu is allowed to be openend at this point
                
      /// Normal Mode ( L + UP )
      if( flag_menu_start == 1 || (((current_buttons & open_key ) == open_key) && current_buttons != old_buttons )) { //) && (pressed_buttons &
        #ifdef LOG
        logPrintf("[INFO] %i: starting menu", getGametime());
        #endif
        flag_menu_start = 0;   // reset
        flag_keys_disable = 1; // set keys exlusive for menu
        flag_menu_running = 1; // start menu
      }
          
      /// Don't Eat Keys Mode ( L + DOWN )
      if( flag_menu_start == 2 || (((current_buttons & open_key_alt ) == open_key_alt) && current_buttons != old_buttons )) {
        #ifdef LOG
        logPrintf("[INFO] %i: starting menu (without key eating)", getGametime());
        #endif
        flag_menu_start = 0;   // reset
        flag_keys_disable = 0; // set keys work in-game & for menu
        flag_menu_running = 1; // start menu
      }
          
    }
  }
      
}


void checkCheats() { // called by hijacked game function
  menu_check(main_menu, menu_size); // FUNC_CHECK
}

void applyCheats() { // called by hijacked game function
  menu_apply(main_menu, menu_size); // FUNC_GET_STATUS & FUNC_APPLY
  
  /// make Dodo flyable
  if( LCS && pcar_id == 0xA4 && flag_menu_running == 0) {
    if( current_buttons & (flag_swapxr ? PSP_CTRL_RTRIGGER : PSP_CTRL_CROSS) ) {  // thrust
      setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+(LCS?0x70:0x140)) + getFloat(pcar+0x10) * (getVehicleSpeed(pcar) + 5.0f) * 0.001 ); 
      setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+(LCS?0x74:0x144)) + getFloat(pcar+0x14) * (getVehicleSpeed(pcar) + 5.0f) * 0.001 );
      setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+(LCS?0x78:0x148)) + getFloat(pcar+0x18) * (getVehicleSpeed(pcar) + 5.0f) * 0.002 );
      
    } else if(current_buttons & PSP_CTRL_SQUARE ) { // brake/reverse
      setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+(LCS?0x70:0x140)) - getFloat(pcar+0x10) * 0.002 );
      setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+(LCS?0x74:0x144)) - getFloat(pcar+0x14) * 0.002 );
      setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+(LCS?0x78:0x148)) - getFloat(pcar+0x18) * 0.005 );
    }  
  }
  
  #ifdef SPECIAL_VEHICLES
  // temporary controls (todo create custom physics like for dodo)
  if( pcar && ((LCS && (pcar_id == 0xC8 || pcar_id == 0xC9)) || (VCS && pcar_id == 0x118)) ){ // AEROPL & (Dead)DODO
    hover_vehicle(FUNC_APPLY, -1, -1); // apply hover cheat in loop
  }
  #endif
}

void applyOnce() { //called by hijacked game function
  #ifdef LOG
  logPrintf("[INFO] %i: applyOnce()", getGametime());
  #endif
  
  #ifdef PREVIEW
  if( LCS ) {
    teleport(918, 194, 5); // Portland ferry area ocean ramp
  }
  //if( VCS ) {
  //  createParkedVehicleSpawnViaSlot(65, 187, -807.20f, -1187.13f, 12.0f, 300.0f,  -1,  -1,  0,  0); // Deluxo in front of Safehouse
  //  teleport(-811, -1179.2, 14); // Bayshore
  //}
  #endif
  
  /// make RC Cars drivable (this sets "model flag 2" in handling.cfg to "NO_DOORS")
  if( LCS ) {
    setByte(getAddressOfHandlingSlotForID(0xC7) + 0xD0, 16); // HELI (tiny inv. heli)
    setByte(getAddressOfHandlingSlotForID(0xA9) + 0xD0, 16); // RCBANDIT
    setByte(getAddressOfHandlingSlotForID(0xD3) + 0xD0, 16); // RCGOBLIN
    setByte(getAddressOfHandlingSlotForID(0xD4) + 0xD0, 16); // RCRAIDER
    setByte(getAddressOfHandlingSlotForID(201) + 0xD0, 16); // "DeadDodo"
  }
  
  /// disable map legend by default
  setMapLegendToggle(OFF);
  
  /// remove ocean pipe's collision(s)
  if( LCS ) {
    removeIPLObjectsCollision(0x1C5); // disables ocean blockades by removing object collision (inside buildings.ipl id: 453)
  }
  
  /// fix invisible garage door in Staunton
  
  
  /// heli height limit (also planes in VCS)
  if( !PPSSPP ) setHeliHeightLimit(996.0f); // 996.0f = 0x4479 (original is 0x42A0 = 80.0f)  addr_heliheight
  // -> for PPSSPP this is set in Patch function(s) so that its being set as early as possible so that PPSSPP doesn't crash (for some strange jit bug reason)
  
  ///Custom Vehicle Spawns in World
  if( LCS ) {
    //Portland 000 - 063:   19, 31
    //Staunton 064 - 135:   81, 89, 104, 109, 120
    //Shoresid 136 - 177:   147, 153, 161, 165, 175
    //178 - 194 (side mission vehicles?)
    
    //north is right is 0 degree https://sites.google.com/site/trigbookprojecttttttttttttt/4-2-degrees-and-radians
    
    /***
    019 (only allowed to spawn between 07:00 to 18:00)
    031 (only allowed to spawn between 18:00 to 07:00) @ night
    081 (only allowed to spawn between 07:00 to 00:00)
    089  always
    147 (only allowed to spawn between 07:00 to 18:00)
    153 (only allowed to spawn between 18:00 to 07:00) @ night
    161  always
    165  always
    ***/
                                 //slot   id      x           y         z    deg    col1  col2 alarm door_lock
    createParkedVehicleSpawnViaSlot( 19, 0xCD,  1152.0f,  -253.0f,  18.0f, 340.0f,   0,   0,    0,    0); // black PCJ600 in front of Portland Savehouse
    createParkedVehicleSpawnViaSlot( 89, 0xCD,   287.2f,  -441.0f,  26.0f, 180.0f,   0,   0,    0,    0); // black PCJ600 in front of Staunton Savehouse
    createParkedVehicleSpawnViaSlot(161, 0xCD,  -822.3f,   296.8f,  40.0f,  30.0f,   0,   0,    0,    0); // black PCJ600 in front of Shoreside Savehouse
    
    createParkedVehicleSpawnViaSlot( 81, 0xD5,   341.0f, -1111.0f,  77.0f,  28.0f,  -1,  -1,    0,    0); // Hunter on Landing Pad in Staunton
    createParkedVehicleSpawnViaSlot(147, 0xD6, -1558.0f,  -909.0f,  15.0f, 190.0f,  -1,  -1,    0,    0); // Maverick on Landing Pad in SSV
    createParkedVehicleSpawnViaSlot(161, 0xA4,  -908.0f,  -751.0f,  12.0f, 154.0f,  -1,  -1,    0,    0); // Dodo in airport hangar in SSV
  
    
    /// fix Slot 111 (Bobcat with wrong y coordinate)
    u32 addr = getVehicleWorldSpawnSlotAddress(110);
    if( getInt(addr) == 0x99 && getInt(addr+0x8) == 0x4405BF5C) { // make sure its a bobcat with the exact wrong coord
      setFloat(addr+0x8, getFloat(addr+0x8) * -1.0f); // make y negative  (534.99 -> -534.99)
    }
  }
  
  if( VCS ) {
    
    /***
    64  always
    88  always
    156 always
    ***/
                                  //slot   id      x           y         z     deg    col1  col2 alarm door_lock
    createParkedVehicleSpawnViaSlot(  64, 0xFC,  -800.5f,  -1186.8f,  11.0f, 290.0f,  -1,  -1,    0,    0); // Streetfighter in front of 101 Bayshore Safehouse
    createParkedVehicleSpawnViaSlot(  88, 0xFC, -1073.2f,    345.0f,  11.0f, 110.0f,  -1,  -1,    0,    0); // Streetfighter in front of Compound Safehouse
    createParkedVehicleSpawnViaSlot( 156, 0xFC,   262.8f,   -161.8f,  11.5f, 190.0f,  -1,  -1,    0,    0); // Streetfighter in front of Clymens Suit Safehouse
    
    createParkedVehicleSpawnViaSlot( 101, 260,  -469.5f,    1124.0f,  65.0f, 200.0f,  -1,  -1,    0,    0); // VNC Maverick on VCN building Landing Pad
    
  }
  
  ///Custom pickup spawns in world
  //if( LCS )
    //spawnPickup(0x10F, 0x3, 0xA, float x, float y, float z); // teargas in ssv pool like in alpha

  #ifdef LOG
  logPrintf("[INFO] %i: applyOnce() done!", getGametime());
  #endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *category_toggle(int type, int cat, int set) {
  static char buffer[8];
  if( type == FUNC_GET_STRING ) { // add indicator if category hidden
    if( !flag_coll_cats ) 
    return ""; // categories can't be expanded/collapsed -> don't show any indicator
  
    snprintf(buffer, sizeof(buffer), !category_index[cat] ? " +" : " -");
    return (void *)buffer;
  }
  
  if( type == FUNC_CHANGE_VALUE )
    category_index[cat] = 1 - category_index[cat];
  
  if( type == FUNC_SET )
    category_index[cat] = set;
  
  return NULL;
}

int checkMenuEntryAllowedToBeDisplayed(const Menu_pack *menu_list, int entry) {
  if( (menu_list[entry].type != MENU_CATEGORY && !category_index[menu_list[entry].cat]) || 
    (LCS && menu_list[entry].LC == FALSE) || 
    (VCS && menu_list[entry].VC == FALSE) || 
    (!multiplayer && menu_list[entry].SP == FALSE) || 
    (multiplayer && menu_list[entry].MP == FALSE) ) {
      return 0; // nope
    }
  return 1; // ok
}  


int menu_sel;    // currently/last selected option
int toption = 0; // the option on top when there are more options than showoptions
int hidden = 0;  // skipping menu draw with disabled entries

int menu_draw(const Menu_pack *menu_list, int menu_max) {
  static int status;
  const char *val;
  static u32 color;
  static int i;
  static float x, y;
  void *(* surrent_get)();
  
  /// title
  snprintf(buffer, sizeof(buffer), translate_string("CheatDevice Remastered %s by Freakler"), VERSION);
  drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_ON, 8.0f, 5.0f, COLOR_TITLE);
  
  /// GameID & version
  //snprintf(buffer, sizeof(buffer), "%s v%s", buf_titleid, buf_version);
  //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 8.0f, 15.0f, LGREY);
    
  /// fps counter (in menu)
  #ifdef DEBUG
  if( flag_draw_FPS ) {
    snprintf(buffer, sizeof(buffer), "FPS: %.0f", fps);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 420.0f, 85.0f, WHITE);
  }
  #endif
  
  /// menu
  x = 65.0f; // horizontal menu draw start
  y = flag_use_legend ? 28.0f : 30.0f; // vertical menu draw start
  hidden = 0;
  
  /// ui box
  // drawUiBox(x-50.0f, y-5.0f, 340.0f, flag_use_legend ? 190.0f : 235.0f, 2.0f, COLOR_UIBORDER, COLOR_UIBACKGROUND); // main  //x, y, width, height, border, color, color
  
  if( menu_sel > menu_max ) 
    menu_sel = 0; // fix for switching between Menus or other out of bounds bug  
  
  if( menu_sel < toption ) 
    toption = menu_sel; // for scrolling up with selection
  
  for( i = toption; i < menu_max; i++ ) { // skip all unallowed from toption to next available on the way scrolling down
    if( checkMenuEntryAllowedToBeDisplayed(menu_list, i) == 0 ) {
      toption = i;
    } else break;
  }

  for( i = toption; i < showoptions+toption+hidden && i <= menu_max; i++, y += row_spacing ) {
    if( checkMenuEntryAllowedToBeDisplayed(menu_list, i) == 0 ) { // do not display
      y -= row_spacing;
      hidden++;
      
    } else { // OK to display
    
      val = ""; // set & reset value string
      surrent_get = menu_list[i].value;
      
      /// draw cursor
      if( i == menu_sel ) 
        drawBox(x-50.0f, y, 340.0f, 16.0f, COLOR_CURSOR); // adjust for vcs
      
      /// draw cheat
      switch( menu_list[i].type )  { 
          
        case MENU_CDR_USER: case MENU_CDR_USCM: 
          color = COLOR_USERCHEATS;
          break;
        
        #ifdef FREECAM        
        case MENU_CDR_FREECAM: 
          color = COLOR_FREECAM;
          break;
        #endif
        
        case MENU_CDR_HEX: 
          color = COLOR_HEX;
          break;
          
        case MENU_CDR_EDITOR: 
          color = COLOR_EDITOR;
          val = surrent_get(FUNC_GET_STRING);
          break;
          
        case MENU_CDR_FILES: 
          color = COLOR_FILES;
          val = surrent_get(FUNC_GET_STRING);
          break;
                  
        case MENU_CATEGORY: 
          color = COLOR_CATEGORY;
          val = surrent_get(FUNC_GET_STRING, menu_list[i].cat);
          break;
          
        case MENU_FUNCTION: case MENU_CONFIG: 
          color = COLOR_CHEAT_OFF;
          break;
          
        case MENU_VALUE: // value list
          if( surrent_get(FUNC_GET_STATUS) ) { // if cheat "ON"
            color = COLOR_CHEAT_ON;
          } else {
            color = COLOR_CHEAT_OFF;
          }
          val = surrent_get(FUNC_GET_STRING);
          break;

        case MENU_SWITCH: // ON/OFF
          if( surrent_get(FUNC_GET_STATUS) ) { // if cheat "ON"
            status = 1;
            color = COLOR_CHEAT_ON;
            drawString("ON", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x-25, y, color);
          } else {
            status = 0;
            color = COLOR_CHEAT_OFF;
            drawString("OFF", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x-30, y, color);
          } 
          break;
          
        case MENU_VALSWITCH: // ON/OFF + values
          if( surrent_get(FUNC_GET_STATUS) ) {
            status = 1;
            color = COLOR_CHEAT_ON;
            drawString("ON", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x-25, y, color);
          } else {
            status = 0;
            color = COLOR_CHEAT_OFF;
            drawString("OFF", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x-30, y, color);
          }
          val = surrent_get(FUNC_GET_STRING);
          break;
          
        default:
          color = COLOR_TEXT;
      }
      
      /// draw menu
      //if( menu_list[i].type != MENU_DUMMY ) {
        memset(&buffer[0], 0, sizeof(buffer));
        
        char* buffer_menu_path = menu_list[i].path;

        if ( buffer_menu_path[strlen(buffer_menu_path)-1] == ':')
          snprintf(buffer, sizeof(buffer), "%s %s", translate_string(menu_list[i].path), translate_string(val));
        else
          snprintf(buffer, sizeof(buffer), "%s%s", translate_string(menu_list[i].path), translate_string(val));


        if( flag_use_cataltfont && menu_list[i].type == MENU_CATEGORY ) // alternative font
          drawString(buffer, ALIGN_FREE, FONT_GTA, LCS ? 0.9 : 0.33, SHADOW_ON, x, y, color);
        else
          drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, x, y, color);
        
        #ifdef DEBUG
        if( flag_draw_DBGVALS ) {
          snprintf(buffer, sizeof(buffer), "%i", i);
          drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 5, y, RED);
        }
        #endif
      //}
      
      /// draw message
      if( flag_use_legend ) {
        if( i == menu_sel ) {
          drawLegendMessage(translate_string(menu_list[i].msg1), 0, 1, COLOR_TEXT); // left side, second row
          drawLegendMessage(translate_string(menu_list[i].msg2), 1, 1, COLOR_TEXT); // right side, second row

          if (strlen(menu_list[i].desc) > 0) {
            char desc_formatted[128];
            snprintf(desc_formatted, sizeof(desc_formatted), "> %s", translate_string(menu_list[i].desc));

            drawLegendMessage(desc_formatted, 0, 0, COLOR_TEXT); // left side, third row
          }
        }
      }
      
    }
  }
  
  /// msg
  if( flag_use_legend ) {
    drawLegendBox(3, COLOR_BACKGROUND);
    drawLegendMessage(translate_string("L+UP/DOWN: Toggle Menu"), 0, 2, COLOR_TEXT);
    drawLegendMessage(translate_string("UP/DOWN: Navigate Cheats"), 1, 2, COLOR_TEXT);
  }
  
  /// draw debug stuff (when enabled)
  #ifdef DEBUG //menu specific for debugging
  if( flag_draw_DBGVALS ) {
    snprintf(buffer, sizeof(buffer), "toption: %i", toption);
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 100.0f, RED);
    snprintf(buffer, sizeof(buffer), "menu_sel: %i", menu_sel);
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 120.0f, RED);
    snprintf(buffer, sizeof(buffer), "hidden: %i", hidden);
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 140.0f, RED);
    snprintf(buffer, sizeof(buffer), "menu_size: %i", menu_size);
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 160.0f, RED);
    
    snprintf(buffer, sizeof(buffer), "mod_text_addr: 0x%08X", mod_text_addr);
    drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 460.0f, 252.0f, RED);
  }
  #endif

  return 0;
}


int menu_ctrl(const Menu_pack *menu_list, int menu_max) { // for blocked buttons check "buttonsToActionPatched()" in cheats.c
  static int keypress;
  int last_sel = 0;

  if( !(current_buttons & PSP_CTRL_LTRIGGER) ) { // fix unwanted scrolling with open menu combo
      
    /// Navigation UP ////////////////////////////////////////
    if( hold_buttons & PSP_CTRL_UP && !(current_buttons & PSP_CTRL_RTRIGGER) ) {
      #ifdef DEBUG
      drawString("hold_buttons PSP_CTRL_UP", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 140.0f, 50.0f, RED);
      #endif
    
      last_sel = menu_sel;
        
      if( menu_sel <= 0 ) {
        menu_sel = 0;
        toption = 0; // scroll
      } else menu_sel--;
    
      /// skip dummy & disabled on the way up
      while( checkMenuEntryAllowedToBeDisplayed(menu_list, menu_sel) == 0 || menu_list[menu_sel].type == MENU_DUMMY || (!flag_coll_cats ? menu_list[menu_sel].type == MENU_CATEGORY : 0) ) {
        menu_sel--;
                   
      if( menu_sel < 0 ) { // go back to last working if only disabled on top of cheatslist
        menu_sel = last_sel; 
        break;
      }
          
      if( toption > 1 && menu_sel == 1 ) // scroll up for each dummy / disabled entry
        toption--; 
      }

      if( toption > 1 && menu_sel == 1 ) // scroll up one more time
        toption--; 
      
    }
  
    /// Navigation DOWN //////////////////////////////////////   
    if( (hold_buttons & PSP_CTRL_DOWN) && !(current_buttons & PSP_CTRL_RTRIGGER) ) {
      #ifdef DEBUG
      drawString("hold_buttons PSP_CTRL_DOWN", ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 140.0f, 50.0f, RED);
      #endif
    
      last_sel = menu_sel;
    
      if( menu_sel < menu_max-1 ) 
        menu_sel++;
      }
    
    //////////////////////////////////////////////////////////////////
    
    /// category scrolling
    if( current_buttons & PSP_CTRL_RTRIGGER ) {
      
      if( pressed_buttons & PSP_CTRL_UP ) { 
        last_sel = menu_sel;
        do {
          do { 
            menu_sel--;
          } while( checkMenuEntryAllowedToBeDisplayed(menu_list, menu_sel) == 0 || menu_list[menu_sel].type == MENU_DUMMY );
          if( menu_sel < 0 ) { // go back to last working
            menu_sel = last_sel; 
            break;
          }
        } while( menu_list[menu_sel].type != MENU_CATEGORY );
      }
      
      if( pressed_buttons & PSP_CTRL_DOWN ) {
        last_sel = menu_sel;
        do {
          if( menu_sel < menu_max-1 ) 
            menu_sel++;
          if( menu_sel >= menu_max-1 ) { // go back to last working
            menu_sel = last_sel;
            break;
          }
        } while( menu_list[menu_sel].type != MENU_CATEGORY );
    toption = menu_sel; // set always on top
      }

    }
    
  }
  
  //////////////////////////////////////////////////////////////////////
    
    /// skip dummy & disabled on the way down (out of navigation-if to handle hidden/disabled on top of menu struct)
    while( checkMenuEntryAllowedToBeDisplayed(menu_list, menu_sel) == 0 || menu_list[menu_sel].type == MENU_DUMMY || (!flag_coll_cats ? menu_list[menu_sel].type == MENU_CATEGORY : 0) ) {
      menu_sel++;
      
      if( menu_sel > menu_max-1 ) { // go back to last working if only disabled on bottom of cheatslist
        menu_sel = last_sel; 
        break;
      }
          
      if( toption+showoptions+hidden < menu_max ) { // scroll down for each dummy / disabled entry
        if( menu_sel >= toption+showoptions+hidden-1 ) 
          toption++; 
      }
    }
    if( toption+showoptions+hidden < menu_max ) { // scroll down one more time
      if( menu_sel >= toption+showoptions+hidden-1 ) 
        toption++;
    }

    
    /// buttons 
    keypress = -2;
    if( menu_list[menu_sel].type == MENU_VALUE || menu_list[menu_sel].type == MENU_VALSWITCH ) { // left & right
      if( hold_buttons & PSP_CTRL_LEFT )  keypress = PSP_CTRL_LEFT;
      if( hold_buttons & PSP_CTRL_RIGHT ) keypress = PSP_CTRL_RIGHT;
    }
    
    if( pressed_buttons & PSP_CTRL_CROSS ) // works everywhere
      keypress = PSP_CTRL_CROSS;
    
    
    if( menu_list[menu_sel].type != MENU_FUNCTION 
     && menu_list[menu_sel].type != MENU_CATEGORY
     && menu_list[menu_sel].type != MENU_CONFIG
     && menu_list[menu_sel].type != MENU_CDR_HEX
     && menu_list[menu_sel].type != MENU_CDR_EDITOR
     && menu_list[menu_sel].type != MENU_CDR_FILES ) { // other buttons only work for none of these types
      if( pressed_buttons & PSP_CTRL_CIRCLE ) 
        keypress = PSP_CTRL_CIRCLE; 
      if( pressed_buttons & PSP_CTRL_SQUARE ) 
        keypress = PSP_CTRL_SQUARE;
      if( pressed_buttons & PSP_CTRL_TRIANGLE ) 
        keypress = PSP_CTRL_TRIANGLE;
    }

    if( keypress > -2 ) {
      void (* func)();
      switch( menu_list[menu_sel].type ) {
        
        case MENU_FUNCTION: case MENU_CDR_HEX: case MENU_CDR_EDITOR: case MENU_CDR_FILES:
          func = (void *)(menu_list[menu_sel].value);
          func();
          break;
          
        case MENU_VALUE: case MENU_SWITCH: case MENU_VALSWITCH: case MENU_CDR_FREECAM: case MENU_CDR_USER: case MENU_CDR_USCM: 
          func = (void *)(menu_list[menu_sel].value);
          func( FUNC_CHANGE_VALUE, keypress );
          break;
          
        case MENU_CATEGORY:
          func = (void *)(menu_list[menu_sel].value);
          func( FUNC_CHANGE_VALUE, menu_list[menu_sel].cat );
          break;
            
        case MENU_CONFIG:
          func = (void *)(menu_list[menu_sel].value);
          func( menu_list, menu_max );
          break;
      }
    }
  
  return 0;
}


int menu_check(const Menu_pack *menu_list, int menu_max) {
  static int i;
  void *(* surrent_get)();
  
  for( i=0; i < menu_max; i++ ) {
    if( menu_list[i].cat != CAT_ALIAS && (((LCS && menu_list[i].LC == TRUE) || (VCS && menu_list[i].VC == TRUE)) && ((!multiplayer && menu_list[i].SP == TRUE) || (multiplayer && menu_list[i].MP == TRUE))) ) { // do not check anything for copies of already exisitng cheat functions & disabled
      surrent_get = menu_list[i].value;
      switch( menu_list[i].type )  {
        case MENU_VALUE:     // value list
        case MENU_SWITCH:    // ON/OFF
        case MENU_VALSWITCH: // ON/OFF + value
          surrent_get(FUNC_CHECK);
          break;
      }
    }
  }
  return 0;
}


int menu_apply(const Menu_pack *menu_list, int menu_max) {
  static int i;
  void *(* surrent_get)();
  
  for( i=0; i < menu_max; i++ ) {
    if( menu_list[i].cat != CAT_ALIAS && (((LCS && menu_list[i].LC == TRUE) || (VCS && menu_list[i].VC == TRUE)) && ((!multiplayer && menu_list[i].SP == TRUE) || (multiplayer && menu_list[i].MP == TRUE))) ) { // do not apply anything for copies of already exisitng cheat functions & disabled
      surrent_get = menu_list[i].value;
      switch( menu_list[i].type )  {
        case MENU_VALUE:  // value list
        case MENU_SWITCH:  // ON/OFF
        case MENU_VALSWITCH:  // ON/OFF + value
          if( surrent_get(FUNC_GET_STATUS) ) 
            surrent_get(FUNC_APPLY);
          break;
      }
    }
  }
  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NAMERESOLV
int hash_to_check = -1;
int last_hash = -1;
char hashbuffer[32];
int name_resolver_status = -1;

int name_resolver(SceSize args, void *argp) {
  #ifdef LOG
  logPrintf("[INFO] name_resolver Thread started");
  #endif
  
  char hashstr[16];  
  name_resolver_status = 1;
  
  while( 1 ) {
    if( hash_to_check != last_hash ) {
      sprintf(hashstr, "0x%08X", hash_to_check); //0x%08X
      snprintf(buffer, sizeof(buffer), "%s%s", basefolder, LCS ? file_names_lcs: file_names_vcs); //ini file
      
      ini_gets( LCS ? "NAMES_LCS" : "NAMES_VCS", hashstr, "unknown", hashbuffer, 32, buffer); //
      
      #ifdef LOG
      logPrintf("[INFO] %i: name_resolver(0x%08X) loaded '%s'", getGametime(), hash_to_check, hashbuffer);
      #endif
      
      last_hash = hash_to_check;
    }
    sceKernelDelayThread(100*1000); // 100ms
  }
  name_resolver_status = 0;
  sceKernelExitDeleteThread(0);
  return 0;
}
#endif

int patch() {
  #ifdef LOG
  logPrintf("[INFO] found 'GTA3' module! mod_text_addr = 0x%08X, text_size = 0x%08X, data_size = 0x%08X", mod_text_addr, mod_text_size, mod_data_size);
  #endif

  /// blacklist
  if( mod_text_size == 0x00386750 && mod_data_size == 0x0001F7E0) { // ULJM-05297_v1.01
    #ifdef LOG
    logPrintf("[ERROR] unsupported game version");
    #endif
    return -1; //exit out
    
  } else if( mod_text_size == 0x0033388C && mod_data_size == 0x0002FEB0) { // ULJM-05255_v1.01
    #ifdef LOG
    logPrintf("[ERROR] unsupported game version");
    #endif
    return -1; //exit out
    
  }
  
  /***************************************************************** 
  * - USA ----------
  * ULUS-10041_v1.02  text_size = 0x00320A34, data_size = 0x0002DBE4
  * ULUS-10041_v1.05  text_size = 0x0032BEDC, data_size = 0x0002E0F8
  * ULUS-10041_v3.00  text_size = 0x0032BFC4, data_size = 0x0002E110
  * 
  * - GER ----------
  * ULES-00182_v1.00 text_size = 0x0032BE8C, data_size = 0x0002E0F8
  * ULES-00182_v2.00 text_size = 0x0032BF74, data_size = 0x0002E110
  * 
  * - EU -----------
  * ULES-00151_v1.05  text_size = 0x0032BF6C, data_size = 0x0002E0F8
  * ULES-00151_v2.00  text_size = 0x0032BF6C, data_size = 0x0002E0F8
  * ULES-00151_v3.00  text_size = 0x0032C044, data_size = 0x0002E110
  * ULES-00151_v4.00  text_size = 0x0032C044, data_size = 0x0002E110
  * 
  * - JP -----------
  * ULJM-05255_v1.01  text_size = 0x0033388C, data_size = 0x0002FEB0
  * 
  * - PROTOTYPE ----
  * ULUX-80146_v0.02  text_size = 0x0031F854, data_size = 0x0002DEC8
  * ULUX-80146_v0.03  text_size = 0x0033205C, data_size = 0x0002E1E0
  * ULET-00361_v0.02  text_size = 0x0032C044, data_size = 0x0002E110 (EU)
  * ULET-00362_v0.01  text_size = 0x0032BF74, data_size = 0x0002E110 (GER)
  * 
  *****************************************************************
  * 
  * - USA ----------
  * ULUS-10160_v1.01  text_size = 0x00377D30, data_size = 0x00020E34
  * ULUS-10160_v1.02  text_size = 
  * ULUS-10160_v1.03  text_size = 0x003864DC, data_size = 0x0001F85C
  * 
  * - EU -----------
  * ULES-00502_v1.02  text_size = 0x003868BC, data_size = 0x0001F85C
  * 
  * - GER ----------
  * ULES-00503_v1.02  text_size = 0x0038673C, data_size = 0x0001F85C
  * 
  * - JP -----------
  * ULJM-05297_v1.01  text_size = 0x00386750, data_size = 0x0001F7E0
  * 
  * - PROTOTYPE ----
  * ULET-00417_v0.06  text_size = 0x0038664C, data_size = 0x0001F86C
  * ULET-00417_v0.07  text_size = 0x003868BC, data_size = 0x0001F85C
  *****************************************************************/
  
  #ifdef LOG
  logPrintf("\n> searching & patching EBOOT..");
  #endif
  u32 i;
  int lcs_counter = 0, vcs_counter = 0;
  for( i = 0; i < mod_text_size; i += 4 ) {
    u32 addr = mod_text_addr + i;
    
    // first hit decides on version -> yes, potentially dangerous
    if( (LCS == 1 || (LCS == VCS)) && PatchLCS(addr, mod_text_addr) ) {
      lcs_counter++;
      LCS = 1;
      continue;
    }
    if( (VCS == 1 || (LCS == VCS)) && PatchVCS(addr, mod_text_addr) ) {
      vcs_counter++;
      VCS = 1;
      continue;
    }
  }
  
  /// error check
  #ifdef LOG
  logPrintf("[INFO] %i LCS & %i VCS locations found", lcs_counter, vcs_counter);
  #endif
  if( lcs_counter > 0 && vcs_counter > 0 ) { // error check
    #ifdef LOG
    logPrintf("[ERROR] both counters found something");
    #endif
    return -1;
  }
  if( LCS == VCS ) { // error check
    #ifdef LOG
    logPrintf("[ERROR] LCS == VCS");
    #endif  
    return -1;
  }
  
  
  /// set default cheat text color
  #ifdef LOG
  logPrintf("\n> setting default color..");
  #endif  
  COLOR_CHEAT_OFF = LCS ? CHDVC_ORANGE : CHDVC_AZURE;

  
  /// set default start address for HexEditor
  #ifdef HEXEDITOR  
  #ifdef LOG
  logPrintf("\n> setting default hexeditor address..");
  #endif  
  hex_adr = mod_text_addr + mod_text_size + mod_data_size;
  add_adr_to_history(mod_text_addr); //
  #endif
  
  
  /// config read/create & default values
  #ifdef LOG
  logPrintf("\n> setting menu and cheat defaults.. (plus config)");
  #endif  
  load_defaults(main_menu, menu_size); // to be save
  #ifdef CONFIG
  if( doesFileExist(config) ) 
    load_config(main_menu, menu_size); // load config
  #endif
  
  
  /// decide draw "welcome helpbox"
  if( flag_menu_start == 0 ) 
    flag_draw_welcomsg = 1;
  
  
  /// set text draw sizes
  SIZE_SMALL    = LCS ? SIZE_LCS_SMALL  : SIZE_VCS_SMALL;
  SIZE_HEXEDIT  = LCS ? SIZE_LCS_SMALL  : 0.5f;
  SIZE_NORMAL   = LCS ? SIZE_LCS_NORMAL : SIZE_VCS_NORMAL;
  SIZE_BIG      = LCS ? SIZE_LCS_BIG    : SIZE_VCS_BIG;
  SIZE_HUGE     = LCS ? SIZE_LCS_HUGE   : SIZE_VCS_HUGE;


  /// (temporary) move everything from old location "ms0:/CDR/.." to "ms0:/PSP/PLGUINS/cheatdevice_remastered/.."
  /*if( doesDirExist("ms0:/CDR/") ) { //old folder found -> should be moved 
    if( !doesDirExist(scripts_folder) ) {  //but only if nothing found in new location (ms0:/PSP/PLUGINS/cheatdevice_remastered/SCRIPTS/)
      //sceIoMvdir("ms0:/CDR/", "ms0:/PSP/PLUGINS/cheatdevice_remastered/");
    }
  }*/


  /// create sub folders for the noobs
  #ifdef PREVIEW
  snprintf(buffer, sizeof(buffer), "%s%s%s", basefolder, folder_cheats, LCS ? "LCS" : "VCS"); // "../CHEATS/xCS"
  sceIoMkdir(buffer, 0777);
  #endif
  
  #ifdef USERSCRIPTS
  snprintf(buffer, sizeof(buffer), "%s%s%s", basefolder, folder_scripts, LCS ? "LCS" : "VCS"); // "../SCRIPTS/xCS"
  makedirs(buffer);
  #endif 
  
  #ifdef LANG
  snprintf(buffer, sizeof(buffer), "%s%s", basefolder, folder_translations); // "../TRANSLATIONS/"
  makedirs(buffer);
  #endif   
  
  //snprintf(buffer, sizeof(buffer), "%s%s", basefolder, folder_textures); // "../TEXTURES/"
  //sceIoMkdir(buffer, 0777);
  

  /// create name resolver thread (if .ini found)
  #ifdef NAMERESOLV
  snprintf(buffer, sizeof(buffer), "%s%s", basefolder, LCS ? file_names_lcs: file_names_vcs);
  if( doesFileExist(buffer) ) {
    
    #ifdef LOG
    logPrintf("\n[INFO] %s found! Starting name_resolver thread", buffer);
    #endif  
    
    SceUID thid = sceKernelCreateThread("name_resolver", name_resolver, 0x18, 0x1000, PSP_THREAD_ATTR_USER, NULL);
    if( thid < 0 ) {
      #ifdef LOG
      logPrintf("[INFO] Error, could not create thread\n");
      #endif  
      sceKernelSleepThread();
    }
    sceKernelStartThread(thid, 0, NULL);
  }
  #endif
  
  /// intialize random number generator (for real random loadscreens cheat in the first place)
  time_t t; // for real rand() #loadscreens
  srand((unsigned) time(&t));
  
  
  /// read TitleID & version from PARAM.SFO - todo
  /*#ifdef LOG
    logPrintf("[INFO] Reading PARAM.SFO\n");
  #endif  
  if( doesFileExist("disc0:/PSP_GAME/PARAM.SFO") ) {
    //https://github.com/PSP-Archive/TNmenu/blob/main/utils.c#L200
    //https://github.com/DaveeFTW/Chronoswitch/blob/master/src/main.c#L53
    //https://github.com/TheOfficialFloW/Adrenaline/blob/e0fef64b5c7514398532f93176f210bc4c5f4a08/cef/systemctrl/adrenaline.c#L70
  }*/
  
  
  /// PREVIEW ONLY STUFF
  #ifdef PREVIEW
  skip_intros(FUNC_SET, -1, ON); //skip intro movies
  cdr_autostartmenu(FUNC_SET, -1, ON); //autostart on
  #endif
  

  #ifdef LOG
  logPrintf("\n> Setup all done! Starting game..\n");
  #endif  
  
  return 0; //success
}


static void CheckModules() { // PPSSPP only
  SceUID modules[10];
  int count = 0;
  if( sceKernelGetModuleIdList(modules, sizeof(modules), &count) >= 0 ) {
    int i;
    SceKernelModuleInfo info;
    for( i = 0; i < count; ++i ) {
      info.size = sizeof(SceKernelModuleInfo);
      if (sceKernelQueryModuleInfo(modules[i], &info) < 0) {
        continue;
      }
      if (strcmp(info.name, "GTA3") == 0) {
        mod_text_addr = info.text_addr;
        mod_text_size = info.text_size;
        mod_data_size = info.data_size;
    
        /// with this approach the game continues to run when patch() is still in progress
    
        // SKIP INTRO MOVIES /////////// for PPSSPP /// TEMP ///
        #ifdef PREVIEW
        if( mod_text_size == 0x0031F854 ) { // ULUX 
          MAKE_DUMMY_FUNCTION(mod_text_addr + 0x00076000, 0);
          clearICacheFor(mod_text_addr + 0x00076000);
          clearICacheFor(mod_text_addr + 0x00076004);
        }
        if( mod_text_size == 0x0032BFC4 ) { // LCS US v3.00
          MAKE_DUMMY_FUNCTION(mod_text_addr + 0x001BCFD0, 0);
          clearICacheFor(mod_text_addr + 0x001BCFD0);
          clearICacheFor(mod_text_addr + 0x001BCFD4);
        }
        #endif
        ////////////////////////////////////////////////////////
      
        sceKernelDelayThread(10000); // small delay to fix blackscreen for LCS (mostly slow android devices)
  
        initTextBlit(mod_text_addr, mod_text_size);  // see blit.c (HAS ITS OWN SEARCHING LOOP!)  
    
        int ret = patch();
        if( ret != 0 ) // patching returned error
          return;

        sceKernelDcacheWritebackAll();
        return;
      }
    }
  }
}


int OnModuleStart(SceModule2 *mod) {
  char *modname = mod->modname;
  
  if( strcmp(modname, "GTA3") == 0 ) {
    mod_text_addr = mod->text_addr;
    mod_text_size = mod->text_size;
    mod_data_size = mod->data_size;

    initTextBlit(mod_text_addr, mod_text_size); // see blit.c (HAS ITS OWN SEARCHING LOOP!)
    int ret = patch();
    if( ret != 0 ) // patching returned error
      return -1;
    sceKernelDcacheWritebackAll();
  }

  if( !previous )
    return 0;

  return previous(mod);
}


int module_start(SceSize argc, void* argp) {
  int i;
  sceCtrlPeekBufferPositive(&pad, 1);

  /// remove old log file (if there is one)
  snprintf(buffer, sizeof(buffer), "%s%s", basefolder, file_log);
  sceIoRemove(buffer); // delete old logfile
  
  /// create basefolder to be save (for logfile this early)
  makedirs((char*)basefolder); // recursively create "ms0:/PSP/PLUGINS/cheatdevice_remastered/"

  /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
  
  #ifdef LOG
  logPrintf("[INFO] Starting CheatDeviceRemastered %s", VERSION);
  logPrintf("[INFO] argc: %i, argp: %s", argc, argp);
  #endif

  /// config init
  #ifdef CONFIG
  sprintf(config, "%s%s", basefolder, file_config); // always use the basefolder path
  #ifdef LOG
  if( doesFileExist(config)) 
    logPrintf("[INFO] Config .ini found! (%s)", config);
  #endif
  #endif
  
  
  /// init categories array
  for(i = 0; i < CAT_COUNTER; i++) 
    category_index[i] = 1;


  /// check available memory (high memory layout)                                         
  memory_high = getHighMemBound();
  // memory_high = 0x0C000000;
  #ifdef LOG
  logPrintf("[INFO] sceKernelTotalFreeMemSize = %i bytes", sceKernelTotalFreeMemSize() );
  logPrintf("[INFO] sceKernelGetBlockHeadAddr() = 0x%08X", memory_high);
  #endif
  
   
  /// check PPSSPP
  if( sceIoDevctl("kemulator:", 0x00000003, NULL, 0, NULL, 0) == 0 ) {
    PPSSPP = 1;
    #ifdef LOG
    logPrintf("[INFO] PPSSPP detected!");
    #endif
  }


  /// check Adrenaline eCFW
  if( !PPSSPP && adrenalineCheck() ) {
    ADRENALINE = 1;
    #ifdef LOG
    logPrintf("[INFO] Adrenaline eCFW detected");
    #endif
  }
  
  
  /// startup key combos
  #ifdef CONFIG
  if( pad.Buttons & PSP_CTRL_RTRIGGER ) { //delete config .ini file
    sceIoRemove(config);
    #ifdef LOG
    logPrintf("> R-TRIGGER: '%s' deleted!", config);
    #endif
  }
  #endif
  
  
  if( PPSSPP ) 
    CheckModules(); // scan the modules using normal/official syscalls (https://github.com/hrydgard/ppsspp/pull/13335#issuecomment-689026242)
  else // PSP
    previous = sctrlHENSetStartModuleHandler(OnModuleStart); 
  
  return 0;
}
