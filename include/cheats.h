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

#ifndef __CHEATS_H__
#define __CHEATS_H__

#include <psptypes.h>

#include "main.h"

extern bool PatchLCS(uaddr addr, uaddr text_addr);
extern bool PatchVCS(uaddr addr, uaddr text_addr);

#define MAKE_CALL(a, f) _sw(0x0C000000 | (((uaddr)(f) >> 2) & 0x03FFFFFF), a);
#define MAKE_JUMP(a, f) _sw(0x08000000 | (((uaddr)(f) & 0x0FFFFFFC) >> 2), a);
// Reverse a JAL call and extract the address it's jumping to
#define REV_JAL(a) (uaddr)( ((a + sizeof(uaddr)) & 0xF0000000) | ((_lw(a) & 0x3FFFFFF) << 2) )

#define HIJACK_FUNCTION(a, f, ptr) { \
  uaddr _func_ = a; \
  static uaddr patch_buffer[3]; \
  _sw(_lw(_func_), (uaddr)patch_buffer); \
  _sw(_lw(_func_ + sizeof(uaddr)), (uaddr)patch_buffer + 8);\
  MAKE_JUMP((uaddr)patch_buffer + sizeof(uaddr), _func_ + 8); \
  _sw(0x08000000 | (((uaddr)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + sizeof(uaddr)); \
  ptr = (void *)patch_buffer; \
}

#define REDIRECT_FUNCTION(a, f) { \
  uaddr _func_ = a; \
  _sw(0x08000000 | (((uaddr)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + sizeof(uaddr)); \
}

#define MAKE_DUMMY_FUNCTION(a, r) { \
  uaddr _func_ = a; \
  if (r == 0) { \
    _sw(0x03E00008, _func_); \
    _sw(0x00001021, _func_ + sizeof(uaddr)); \
  } else { \
    _sw(0x03E00008, _func_); \
    _sw(0x24020000 | r, _func_ + sizeof(uaddr)); \
  } \
}

/// Calls
extern SceInt64 sceKernelGetSystemTimeWidePatched(void);

extern uaddr (* GetPPLAYER)();
extern uaddr (* GetPOBJ)();
extern uaddr (* GetPCAR)();

extern void (* CWeather_ForceWeather)(int16 weather);
extern void (* CWeather_ForceWeatherNow)(int16 weather);
extern void (* CWeather_ReleaseWeather)(void);

extern void (* TaskCharWith)(uint32 ped, int val, uint32 object);
extern void (* WarpPedIntoVehicle)(int ped, int vehicle); // FUN_001b90ec_warpPedIntoVehicle(int ped, int veh) (has more attributes in VCS!!!!)
extern void (* WarpPedIntoVehicleAsPassenger)(int ped, int vehicle, int seat); // FUN_001b9470_warpPedIntoVehicleAsPassenger(int ped, int veh, int seat)
extern void (* RequestModel)(uint32 model_id, uint32 param_2); // , uint32 param_3, uint32 param_4, uint32 param_5);
extern int (* GiveWeaponAndAmmo)(int ped_obj, int param_2, uint32 ammo); // , uint32 param_4, uint32 param_5);
extern void (* SetActorSkinTo)(int ped_obj, const char *name); // name must be lower case!!
extern void (* LoadAllModelsNow)(char x);
extern void (* RefreshActorSkin)(int ped_obj);
extern void (* TaskDuckLCS)(int ped_obj);
extern void (* TaskDuckVCS)(int ped_obj, int duration, char x);
extern void (* TaskUnDuck)(int ped_obj, char x);

extern int (* SetWantedLevel)(int pplayer, int stars);
extern int (* SetMaxWantedLevel)(int stars);

extern int (* StartNewScript)(int ip);

extern void (* TankControl)(int handle);
extern void (* BlowupVehiclesInPath)(int handle);

extern void (* DoHoverSuspensionRatios)(int handle);

extern int (* IsPlayerOnAMission)(void);
extern void (* SetBridgeState)(int state);

extern int (* LoadStringFromGXT)(int gxt_adr,char *string,int param_3,int param_4,int param_5, int param_6,int param_7,int param_8);
extern int LoadStringFromGXT_patched(int gxt_adr,char *string, int param_3, int param_4, int param_5, int param_6,int param_7,int param_8);

extern int (* GetTEST)(); // for testing

/// Hijacks
extern int (* buttonsToAction)(void *a1);
extern int buttonsToActionPatched(void *a1);

extern void (* cWorldStream_Render)(void *this, int mode);
extern void cWorldStream_Render_Patched(void *this, int mode);

extern void (* FUN_002c22a0)(int param_1);
extern void FUN_002c22a0_patched(int param_1);

extern void (* FUN_002db0c0)(int param_1);
extern void FUN_002db0c0_patched(int param_1);

extern int (*_checkCustomTracksReady)(int param_1);
extern int _checkCustomTracksReady_patched(int param_1);

extern uint32 (*loadSplashScreen)(int* x,int y,int z, int a, int b);
extern uint32 loadSplashScreen_patched(int* x,int y,int z, int a, int b);

extern void (*Loadscreen)(char * string1, char * string2, char *txdname, uint32 param_4);
extern void Loadscreen_patched(char * string1, char * string2, char *txdname, uint32 param_4);

extern void (*DrawLoadingBar)(float param_1);
extern void DrawLoadingBar_patched(float param_1);

extern void (*UpdatePosition)(u32 handle);
extern void UpdatePosition_patched(u32 handle);

//#ifdef SWIM
extern int (*FUN_00109dac_CWaterLevel_GetWaterLevel)(float param_1,float param_2,float *param_3);
extern int (*FUN_000e7d70_CCam_IsTargetInWater)(int param_1);
extern int FUN_000e7d70_CCam_IsTargetInWater_patched(int param_1);
extern void (*FUN_001a8d9c_CPed_ProcessBuoyancy)(int param_1);
extern void *FUN_001a8d9c_CPed_ProcessBuoyancy_patched(int param_1);
//#endif

extern void (*FUN_000B0A18)(uaddr data);
extern void FUN_000B0A18_Patched(uaddr data);

/// Cheat functions
extern void *debug_monitor(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *debug_vars(int calltype, int keypress, bool defaultstatus, int defaultval UNUSED);

extern void *fps_toggle(int calltype, int keypress, bool defaultstatus);
extern void *mem_toggle(int calltype, int keypress, bool defaultstatus);
extern void *coords_toggle(int calltype, int keypress, bool defaultstatus);
extern void *speedometer_toggle(int calltype, int keypress, bool defaultstatus, int defaultval);

extern void *fps_cap(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *neverfalloffbike(int calltype, int keypress, bool defaultstatus);
extern void *skip_intros(int calltype, int keypress, bool defaultstatus);
extern void *teleporter(int calltype, int keypress, bool defaultstatus UNUSED, int defaultval);
extern void *godmode(int calltype, int keypress, bool defaultstatus);
extern void *warp_out_water(int calltype, int keypress, bool defaultstatus);
extern void *warp_out_water_veh(int calltype, int keypress, bool defaultstatus);
extern void *invisible(int calltype, int keypress, bool defaultstatus);
extern void *ignored(int calltype, int keypress, bool defaultstatus);
extern void *unlimited_ammo(int calltype, int keypress, bool defaultstatus);
extern void *heavy_player(int calltype, int keypress, bool defaultstatus);
extern void *aim_of_death(int calltype, int keypress, bool defaultstatus);
extern void *indestr_vehicle(int calltype, int keypress, bool defaultstatus);
extern void *lockdoors(int calltype, int keypress, bool defaultstatus);
extern void *hover_vehicle(int calltype, int keypress, bool defaultstatus);
extern void *heavy_vehicle(int calltype, int keypress, bool defaultstatus);
extern void *world_weather(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *world_gravity(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *vehicle_base_color(int calltype, int keypress, bool defaultstatus UNUSED);
extern void *vehicle_stripe_color(int calltype, int keypress, bool defaultstatus UNUSED);
extern void stepthroughwall();
extern void *traffic_density(int calltype, int keypress, bool defaultstatus);
extern void *traffic_freeze(int calltype, int keypress, bool defaultstatus);
extern void *peds_density(int calltype, int keypress, bool defaultstatus);
extern void *peds_killaiming(int calltype, int keypress, bool defaultstatus);
extern void *peds_freeze(int calltype, int keypress, bool defaultstatus);
extern void *max_health(int calltype, int keypress);
extern void *max_armor(int calltype, int keypress);
extern void *powerjump(int calltype, int keypress, bool defaultstatus);
extern void *mission_select(int calltype, int keypress, bool defaultstatus UNUSED, int defaultval);
// extern void *load_building(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *wanted_level(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *world_time(int calltype, int keypress, bool defaultstatus, int defaultval UNUSED);
extern void *world_realtimeclock(int calltype, int keypress, bool defaultstatus);
extern void *no_cheating_warning(int calltype, int keypress, bool defaultstatus UNUSED);
extern void *traffic_radiostation(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *world_waterlevel(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *world_waveheight(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *powerbrake(int calltype, int keypress, bool defaultstatus);
extern void *nitro(int calltype, int keypress, bool defaultstatus);
extern void *tank(int calltype, int keypress, bool defaultstatus);
extern void *driveonwater(int calltype, int keypress, bool defaultstatus);
extern void *automaticflipover(int calltype, int keypress, bool defaultstatus) ;
extern void *driveonwalls(int calltype, int keypress, bool defaultstatus);
extern void *unlimited_sprinting(int calltype, int keypress, bool defaultstatus);
extern void *unlimited_swimming(int calltype, int keypress, bool defaultstatus);
extern void *pickup_spawner(int calltype, int keypress, bool defaultstatus UNUSED, int defaultval);
extern void *player_model(int calltype, int keypress, bool defaultstatus UNUSED, int defaultval);
extern void *vehicle_spawner(int calltype, int keypress, bool defaultstatus UNUSED, int defaultval);
extern void *teleport_next(int calltype, int keypress);
extern void teleport_next_sub();
extern void teleport_marker();
extern void teleport_mission();
extern void kill_all_targets();
extern void freeze_all_targets();
extern void mission_selector();
extern void *down_button(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *up_button(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *markonmap(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *debug_loadscreens(int calltype, int keypress, bool defaultstatus);
extern void *random_loadscreens(int calltype, int keypress, bool defaultstatus);
extern void *disable_textures(int calltype, int keypress, bool defaultstatus);
extern void *touch_pedestrian(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *touch_vehicle(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *touch_object(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *camera_centered(int calltype, int keypress, bool defaultstatus);
extern void *fieldofview(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *camera_topdown(int calltype, int keypress, bool defaultstatus);
extern void *gamespeed(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *dev_flag(int calltype, int keypress, bool defaultstatus);
extern void *freeze_timers(int calltype, int keypress, bool defaultstatus);
extern void *onmission(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *world_liftcontrol(int calltype, int keypress, bool defaultstatus, int defaultval UNUSED);
extern void *walking_speed(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *pedwalking_speed(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *gather_spell(int calltype, int keypress, bool defaultstatus);
extern void *rocketboost(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *untouchable(int calltype, int keypress, bool defaultstatus);
extern void *bmxjumpheight(int calltype, int keypress, bool defaultstatus, int defaultval);
extern void *policechaseheli(int calltype, int keypress, bool defaultstatus, int defaultval UNUSED);
extern void *fake_swimming(int calltype, int keypress, bool defaultstatus, int defaultval UNUSED);


extern void bttncht_givehealth();
extern void bttncht_givearmor();
extern void bttncht_givemoney();
extern void bttncht_weaponset1();
extern void bttncht_weaponset2();
extern void bttncht_weaponset3();
extern void bttncht_carsonwater(); // LCS only
extern void bttncht_playcredits(); // LCS only
extern void *bttncht_randomplayer(int calltype, int keypress, bool defaultstatus UNUSED, int defaultval); // LCS only
extern void bttncht_unlockmult100();
extern void bttncht_perftraction();
extern void bttncht_blowupcars();
extern void bttncht_spawnrhino();
extern void bttncht_bubblehead(); // LCS only
extern void bttncht_pedsweapons();
extern void bttncht_pedsattack();
extern void bttncht_pedsriot();
extern void bttncht_agrodrivers();
extern void bttncht_traffichrome();


extern void *cdr_showpopular(int calltype, int keypress, bool defaultstatus);
extern void *cdr_collapsecats(int calltype, int keypress, bool defaultstatus);
extern void *cdr_alternativefont(int calltype, int keypress, bool defaultstatus);
extern void *cdr_autostartmenu(int calltype, int keypress, bool defaultstatus);
extern void *cdr_freezegameinmenu(int calltype, int keypress, bool defaultstatus);
extern void *cdr_uselegend(int calltype, int keypress, bool defaultstatus);
extern void *cdr_allowuiblocking(int calltype, int keypress, bool defaultstatus);
extern void *cdr_liveconfig(int calltype, int keypress, bool defaultstatus);
extern void *cdr_swapacceleration(int calltype, int keypress, bool defaultstatus);

#ifdef LANG
extern void updateLangStrings();
extern void *cdr_changelang(int calltype, int keypress, bool defaultstatus, int defaultval);
#endif

extern void load_defaults(const CDRMenuItem *menu_list, usize menu_max);
extern void exit_game();

#ifdef PREVIEW
extern void user_cheats(int calltype, int keypress, bool defaultstatus UNUSED);
#endif

#ifdef USERSCRIPTS
extern void user_scripts(int calltype, int keypress, bool defaultstatus UNUSED);
#endif

#ifdef FREECAM
extern void freecam(int calltype, int keypress, bool defaultstatus UNUSED);
#endif

#ifdef HEXEDITOR
extern void hexeditor();
extern void hexeditpplayer();
extern void hexeditpcar();
#endif

extern void *mp_test(int calltype, int keypress, bool defaultstatus);

#ifdef PREVIEW
extern void test_func();
extern void *test_switch(int calltype, int keypress, bool defaultstatus);
#endif

#ifdef GAME_LOGGING
extern void (*debugprint)(const u8string text, ...);
extern void debugprint_patched(const u8string text, ...);
extern void drawGameLog();
#endif

#ifdef MEMORY
// MEMORY
extern void drawMemoryUsage();
#endif

#endif
