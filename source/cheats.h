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

#ifndef __CHEATS_H__
#define __CHEATS_H__

#include <psptypes.h>

#include "main.h"

int PatchLCS(u32 addr, u32 text_addr);
int PatchVCS(u32 addr, u32 text_addr);

#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);

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

#define REDIRECT_FUNCTION(a, f) { \
  u32 _func_ = a; \
  _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + 4); \
}

#define MAKE_DUMMY_FUNCTION(a, r) { \
  u32 _func_ = a; \
  if (r == 0) { \
    _sw(0x03E00008, _func_); \
    _sw(0x00001021, _func_ + 4); \
  } else { \
    _sw(0x03E00008, _func_); \
    _sw(0x24020000 | r, _func_ + 4); \
  } \
}


typedef struct {
  char* title;
  short id;
  char unlocked;
} achievement_pack;

/// Calls 
SceInt64 sceKernelGetSystemTimeWidePatched(void);

int (* GetPPLAYER)();
int (* GetPOBJ)();
int (* GetPCAR)();

void (* SetNextWeather)(short weathercode);
void (* SetWeatherNow)(short weathercode);
void (* ReleaseWeather)(void);

void (* TaskCharWith)(unsigned int ped, int val, unsigned int object);
void (* WarpPedIntoVehicle)(int ped, int vehicle); // FUN_001b90ec_warpPedIntoVehicle(int ped, int veh) (has more attributes in VCS!!!!)
void (* WarpPedIntoVehicleAsPassenger)(int ped, int vehicle, int seat); // FUN_001b9470_warpPedIntoVehicleAsPassenger(int ped, int veh, int seat)
void (* RequestModel)(unsigned int model_id, unsigned int param_2); // , unsigned int param_3, unsigned int param_4, unsigned int param_5);
int (* GiveWeaponAndAmmo)(int ped_obj, int param_2, unsigned int ammo); // , unsigned int param_4, unsigned int param_5);
void (* SetActorSkinTo)(int ped_obj, const char *name); // name must be lower case!!
void (* LoadAllModelsNow)(char x);
void (* RefreshActorSkin)(int ped_obj);
void (* TaskDuckLCS)(int ped_obj);
void (* TaskDuckVCS)(int ped_obj, int duration, char x);
void (* TaskUnDuck)(int ped_obj, char x);

int (* SetWantedLevel)(int pplayer, int stars);
int (* SetMaxWantedLevel)(int stars);

int (* StartNewScript)(int ip);

void (* TankControl)(int handle);
void (* BlowupVehiclesInPath)(int handle);

void (* DoHoverSuspensionRatios)(int handle);

int (* IsPlayerOnAMission)(void);
void (* SetBridgeState)(int state);

int (* LoadStringFromGXT)(int gxt_adr,char *string,int param_3,int param_4,int param_5, int param_6,int param_7,int param_8);
int LoadStringFromGXT_patched(int gxt_adr,char *string, int param_3, int param_4, int param_5, int param_6,int param_7,int param_8);

int (* GetTEST)(); // for testing

short cameraX(short *pad);
short cameraY(short *pad);
short aimX(short *pad);
short aimY(short *pad);


/// Hijacks
int (* buttonsToAction)(void *a1);
int buttonsToActionPatched(void *a1);

void (* cWorldStream_Render)(void *this, int mode);
void cWorldStream_Render_Patched(void *this, int mode);

void (* FUN_002c22a0)(int param_1);
void FUN_002c22a0_patched(int param_1);

void (* FUN_002db0c0)(int param_1);
void FUN_002db0c0_patched(int param_1);

int (*_checkCustomTracksReady)(int param_1);
int _checkCustomTracksReady_patched(int param_1);

unsigned int (*loadSplashScreen)(int* x,int y,int z, int a, int b);
unsigned int loadSplashScreen_patched(int* x,int y,int z, int a, int b);

void (*Loadscreen)(char * string1, char * string2, char *txdname, unsigned int param_4);
void Loadscreen_patched(char * string1, char * string2, char *txdname, unsigned int param_4);

void (*DrawLoadingBar)(float param_1);
void DrawLoadingBar_patched(float param_1);

void (*UpdatePosition)(int handle);
void UpdatePosition_patched(int handle);

//#ifdef SWIM
int (*FUN_00109dac_CWaterLevel_GetWaterLevel)(float param_1,float param_2,float *param_3);
int (*FUN_000e7d70_CCam_IsTargetInWater)(int param_1);
int FUN_000e7d70_CCam_IsTargetInWater_patched(int param_1);
void (*FUN_001a8d9c_CPed_ProcessBuoyancy)(int param_1);
void *FUN_001a8d9c_CPed_ProcessBuoyancy_patched(int param_1);
//#endif

/// Cheat functions 
void *debug_monitor(int calltype, int keypress, int defaultstatus, int defaultval);
void *debug_vars(int calltype, int keypress, int defaultstatus, int defaultval);

void *customtracks(int calltype, int keypress, int defaultstatus);

void *fps_toggle(int calltype, int keypress, int defaultstatus);
void *mem_toggle(int calltype, int keypress, int defaultstatus);
void *coords_toggle(int calltype, int keypress, int defaultstatus);
void *speedometer_toggle(int calltype, int keypress, int defaultstatus, int defaultval);

void *fps_cap(int calltype, int keypress, int defaultstatus, int defaultval);
void *neverfalloffbike(int calltype, int keypress, int defaultstatus);
void *playermodel(int calltype, int keypress, int defaultstatus);
void *skip_intros(int calltype, int keypress, int defaultstatus);
void *teleporter(int calltype, int keypress, int defaultstatus, int defaultval);
void *godmode(int calltype, int keypress, int defaultstatus);
void *warp_out_water(int calltype, int keypress, int defaultstatus);
void *warp_out_water_veh(int calltype, int keypress, int defaultstatus);
void *invisible(int calltype, int keypress, int defaultstatus);
void *ignored(int calltype, int keypress, int defaultstatus);
void *unlimited_ammo(int calltype, int keypress, int defaultstatus);
void *heavy_player(int calltype, int keypress, int defaultstatus);
void *aim_of_death(int calltype, int keypress, int defaultstatus);
void *indestr_vehicle(int calltype, int keypress, int defaultstatus);
void *lockdoors(int calltype, int keypress, int defaultstatus);
void *hover_vehicle(int calltype, int keypress, int defaultstatus);
void *heavy_vehicle(int calltype, int keypress, int defaultstatus);
void *world_weather(int calltype, int keypress, int defaultstatus, int defaultval);
void *world_gravity(int calltype, int keypress, int defaultstatus, int defaultval);
void *vehicle_base_color(int calltype, int keypress, int defaultstatus);
void *vehicle_stripe_color(int calltype, int keypress, int defaultstatus);
void stepthroughwall();
void *traffic_density(int calltype, int keypress, int defaultstatus);
void *traffic_freeze(int calltype, int keypress, int defaultstatus);
void *peds_density(int calltype, int keypress, int defaultstatus);
void *peds_killaiming(int calltype, int keypress, int defaultstatus);
void *peds_freeze(int calltype, int keypress, int defaultstatus);
void *max_health(int calltype, int keypress);
void *max_armor(int calltype, int keypress);
void *powerjump(int calltype, int keypress, int defaultstatus);
void *mission_select(int calltype, int keypress, int defaultstatus, int defaultval);
void *load_building(int calltype, int keypress, int defaultstatus, int defaultval);
void *wanted_level(int calltype, int keypress, int defaultstatus, int defaultval);
void *world_time(int calltype, int keypress, int defaultstatus, int defaultval);
void *world_realtimeclock(int calltype, int keypress, int defaultstatus);
void *no_cheating_warning(int calltype, int keypress, int defaultstatus);
void *traffic_radiostation(int calltype, int keypress, int defaultstatus, int defaultval);
void *world_waterlevel(int calltype, int keypress, int defaultstatus, int defaultval);
void *world_waveheight(int calltype, int keypress, int defaultstatus, int defaultval);
void *powerbrake(int calltype, int keypress, int defaultstatus);
void *nitro(int calltype, int keypress, int defaultstatus);
void *tank(int calltype, int keypress, int defaultstatus);
void *driveonwater(int calltype, int keypress, int defaultstatus);
void *automaticflipover(int calltype, int keypress, int defaultstatus) ;
void *driveonwalls(int calltype, int keypress, int defaultstatus);
void *unlimited_sprinting(int calltype, int keypress, int defaultstatus);
void *unlimited_swimming(int calltype, int keypress, int defaultstatus);
void *pickup_spawner(int calltype, int keypress, int defaultstatus, int defaultval);
void *player_model(int calltype, int keypress, int defaultstatus, int defaultval);
void *vehicle_spawner(int calltype, int keypress, int defaultstatus, int defaultval);
void *teleport_next(int calltype, int keypress);
void teleport_next_sub();
void teleport_marker();
void teleport_mission();
void kill_all_targets();
void freeze_all_targets();
void mission_selector();
void *down_button(int calltype, int keypress, int defaultstatus, int defaultval);
void *up_button(int calltype, int keypress, int defaultstatus, int defaultval);
void *markonmap(int calltype, int keypress, int defaultstatus, int defaultval);
void *debug_loadscreens(int calltype, int keypress, int defaultstatus);
void *random_loadscreens(int calltype, int keypress, int defaultstatus);
void *disable_textures(int calltype, int keypress, int defaultstatus);
void *touch_pedestrian(int calltype, int keypress, int defaultstatus, int defaultval);
void *touch_vehicle(int calltype, int keypress, int defaultstatus, int defaultval);
void *touch_object(int calltype, int keypress, int defaultstatus, int defaultval);
void *camera_centered(int calltype, int keypress, int defaultstatus);
void *fieldofview(int calltype, int keypress, int defaultstatus, int defaultval);
void *camera_topdown(int calltype, int keypress, int defaultstatus);
void *gamespeed(int calltype, int keypress, int defaultstatus, int defaultval);
void *dev_flag(int calltype, int keypress, int defaultstatus);
void *freeze_timers(int calltype, int keypress, int defaultstatus);
void *onmission(int calltype, int keypress, int defaultstatus, int defaultval);
void *world_liftcontrol(int calltype, int keypress, int defaultstatus, int defaultval);
void *walking_speed(int calltype, int keypress, int defaultstatus, int defaultval);
void *pedwalking_speed(int calltype, int keypress, int defaultstatus, int defaultval);
void *gather_spell(int calltype, int keypress, int defaultstatus);
void *rocketboost(int calltype, int keypress, int defaultstatus, int defaultval);
void *untouchable(int calltype, int keypress, int defaultstatus);
void *bmxjumpheight(int calltype, int keypress, int defaultstatus, int defaultval);
void *policechaseheli(int calltype, int keypress, int defaultstatus, int defaultval);
void *fake_swimming(int calltype, int keypress, int defaultstatus, int defaultval);


void bttncht_givehealth();
void bttncht_givearmor();
void bttncht_givemoney();
void bttncht_weaponset1();
void bttncht_weaponset2();
void bttncht_weaponset3();
void bttncht_carsonwater(); // LCS only
void bttncht_playcredits(); // LCS only
void *bttncht_randomplayer(int calltype, int keypress, int defaultstatus, int defaultval); // LCS only
void bttncht_unlockmult100();
void bttncht_perftraction();
void bttncht_blowupcars();
void bttncht_spawnrhino();
void bttncht_bubblehead(); // LCS only
void bttncht_pedsweapons();
void bttncht_pedsattack();
void bttncht_pedsriot();
void bttncht_agrodrivers();
void bttncht_traffichrome();


void *cdr_showpopular(int calltype, int keypress, int defaultstatus);
void *cdr_collapsecats(int calltype, int keypress, int defaultstatus);
void *cdr_alternativefont(int calltype, int keypress, int defaultstatus);
void *cdr_autostartmenu(int calltype, int keypress, int defaultstatus);
void *cdr_freezegameinmenu(int calltype, int keypress, int defaultstatus);
void *cdr_uselegend(int calltype, int keypress, int defaultstatus);
void *cdr_allowuiblocking(int calltype, int keypress, int defaultstatus);
void *cdr_liveconfig(int calltype, int keypress, int defaultstatus);
void *cdr_advancedconfig(int calltype, int keypress, int defaultstatus);
void *cdr_swapacceleration(int calltype, int keypress, int defaultstatus);

#ifdef LANG
void *cdr_changelang(int calltype, int keypress, int defaultstatus, int defaultval);
#endif

void load_defaults(const Menu_pack *menu_list, int menu_max);
void exit_game();

void user_cheats(int calltype, int keypress, int defaultstatus);
void user_scripts(int calltype, int keypress, int defaultstatus);
void savegame_editor();
void freecam(int calltype, int keypress, int defaultstatus);
void hexeditor();
void hexeditpplayer();
void hexeditpcar();

void *mp_test(int calltype, int keypress, int defaultstatus);

// ACHIEVMENTS
void achievements();
void achievements_reset();

// PREVIEW
void test_func();
void *test_switch(int calltype, int keypress, int defaultstatus);

// GAMELOG
void (*debugprint)(const char *text, ...); 
void debugprint_patched(const char *text, ...);
void drawGameLog();

// MEMORY
void drawMemoryUsage();

#endif
