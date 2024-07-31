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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pspctrl.h>
#include <systemctrl.h>
#include <math.h>
#include <malloc.h>
#include <psprtc.h>

#include "cheats.h"
#include "functions.h"
#include "main.h"
#include "editor.h"
#include "utils.h"
#include "blitn.h"

#ifdef LANG
#include "lang/lang.h"
#endif

register int gp asm("gp"); // for VCS
int gp_ = 0; // helper (also used in configs extern)

/// externs 
extern int LCS, VCS, PPSSPP, ADRENALINE;

extern int menuopendelay;
extern u32 mod_text_addr;
extern u32 mod_text_size;
extern u32 mod_data_size;

extern float SIZE_SMALL, SIZE_NORMAL, SIZE_BIG, SIZE_HUGE; // for drawString()

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

extern const teleports_pack lcs_teleports[];
extern int lcs_teleportersize;

extern const teleports_pack vcs_teleports[];
extern int vcs_teleportersize;

extern const pickups_pack lcs_pickups[]; 
extern int lcs_pickupsize;

extern const pickups_pack vcs_pickups[]; 
extern int vcs_pickupsize;

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

extern short showoptions;
extern char category_index[];

extern u32 current_buttons, pressed_buttons, hold_buttons;
extern float xstick, ystick, xstick_, ystick_;

extern short flag_menu_start;
extern short flag_menu_show;
extern short flag_menu_running;
extern short flag_keys_disable;  
extern short flag_draw_FPS;
extern short flag_draw_MEM;
extern short flag_draw_COORDS;
extern short flag_draw_SPEEDO;
extern short flag_draw_DEBUG;
extern short flag_use_liveconfig; // #ifdef CONFIG
extern short flag_customusic;
extern short flag_coll_cats;
extern short flag_freecam;
extern short flag_use_cataltfont;
extern short flag_ui_blocking;
extern short flag_use_legend;

extern u32 memory_low;
extern u32 memory_high;

extern char L,R,UP,DOWN,LEFT,RIGHT,CROSS,CIRCLE,SQUARE,TRIANGLE,NONE;
extern char key_to_pad;

extern const char *weather_lcs[];
extern const char *weather_vcs[];


/// globals
char retbuf[64]; // return string for cheats

int pplayer = 0;
int pcar = 0;
int pobj = 0;
int pcar_id = 0;
int pcar_type = 0;
int gametimer = 0;
int island = 0;
int language = 0;
int multiplayer = 0;

int peds_max = 0;
int peds_cur = 0;
int peds_base = 0;
int vehicles_max = 0;
int vehicles_cur = 0;
int vehicles_base = 0;
int worldobjs_max = 0;
int worldobjs_cur = 0;
int worldobjs_base = 0;
int businessobjs_max = 0;
int businessobjs_cur = 0;
int businessobjs_base = 0;

int garage_cur = 0;
int pickups_cur = 0;
int mapicons_cur = 0;
int vehspawns_cur = 0;

int dummysIPL_base = 0;
int handlingcfg_cur = 0;
int carcolsDAT_base = 0;
int pedcolsDAT_base = 0; // VCS only
int buildingsIPL_base = 0;
int treadablesIPL_base = 0;


/// later set by PatchXCS //////////////////////////
u32 addr_skipIntroMovie             = -1;
u32 addr_neverFallOffBike_rollback  = -1;
u32 addr_neverFallOffBike_hitobj    = -1;
u32 addr_worldgravity               = -1;
u32 addr_fpsCap                     = -1;
u32 addr_buttoncheat                = -1;
u32 addr_heliheight                 = -1;
#ifdef PREVIEW
u32 addr_policechaseheli_1          = -1;
u32 addr_policechaseheli_2          = -1;
u32 addr_policechaseheli_3          = -1;
#endif
u32 addr_randompedcheat             = -1;

u32 global_weather          = -1;
u32 global_trafficdensity   = -1;
u32 global_peddensity       = -1;
u32 global_freezegame       = -1;
u32 global_gametimer        = -1;
u32 global_timescale        = -1;
u32 global_maxhealthmult    = -1;
u32 global_maxarmormult     = -1;
u32 global_unlimtedsprint   = -1;
u32 global_ismultiplayer    = -1;
u32 global_clockmultiplier  = -1;
u32 global_cheatusedboolean = -1;
u32 global_cheatusedcounter = -1;
u32 global_maxwantedlevel   = -1;
u32 global_camera           = -1;
u32 global_hudincutscene    = -1;
u32 global_currentisland    = -1;
u32 global_systemlanguage   = -1;
u32 global_garagedata       = -1;

u32 ptr_pedestriansobj      = -1;
u32 ptr_vehiclesobj         = -1;
u32 ptr_worldobj            = -1;
u32 ptr_businessobj         = -1; // VCS only
u32 ptr_audioscriptobj      = -1;

u32 global_pickups          = -1;
u32 global_ptr_water        = -1;
u32 global_buttoninput      = -1;
u32 global_radioarea        = -1;
u32 global_custrackarea     = -1;
u32 global_radarblips       = -1;
u32 global_developerflag    = -1;
u32 global_freezetimers     = -1;

u32 global_ScriptSpace      = -1;
u32 global_MainScriptSize   = -1;
u32 global_LargestMissionScriptSize = -1;

u32 global_mp_1             = -1;
u32 global_mp_parameters    = -1;
u32 global_OnAMissionFlag   = -1;
u32 global_bridgeState      = -1;
u32 global_m_pVehicleName   = -1;
u32 global_m_pVehicleNameToPrint = -1;

u32 global_Wind             = -1;
u32 global_WindClipped      = -1;


u32 global_displaysettings  = -1; // VCS only
 u32 global_radarbool       = -1; // LCS only
 u32 global_hudbool         = -1; // LCS only
 u32 global_maplegendbool   = -1; // LCS only
 
 
u32 global_helpbox                = -1; // VCS only
 u32 global_helpbox_string        = -1; // LCS only
 u32 global_helpbox_timedisplayed = -1; // LCS only
 u32 global_helpbox_displaybool   = -1; // LCS only
 u32 global_helpbox_permboxbool   = -1; // LCS only
 u32 global_helpbox_duration      = -1; // LCS only
 u32 global_dialog_string         = -1; // LCS only
 
u32 global_bmxjumpmult      = -1; // VCS only

u32 var_garageslots         = -1;
u32 var_garageslotsize      = -1;

u32 var_pickupslots         = -1;
u32 var_pickupslotsize      = -1;

u32 var_radarblipslots      = -1;
u32 var_radarblipslotsize   = -1;
u32 var_radarblipspadding   = -1;

u32 var_pedobjsize          = -1;
u32 var_vehobjsize          = -1;
u32 var_wldobjsize          = -1;

u32 var_bsnobjsize          = -1;

u32 addr_vehiclesworldspawn        = -1;
u32 var_vehiclesworldspawnslots    = -1;
u32 var_vehiclesworldspawnslotsize = -1;


/// GAME.DTZ ////////////////////////
u32 ptr_buildingsIPL          = -1;
u32 buildingsIPL_max          = -1;
u32 var_buildingsIPLslotsize  = -1;

u32 ptr_treadablesIPL         = -1;
u32 treadablesIPL_max         = -1;
u32 var_treadablesIPLslotsize = -1;

u32 ptr_dummysIPL             = -1;
u32 dummysIPL_max             = -1;
u32 var_dummysIPLslotsize     = -1;

u32 ptr_handlingCFG           = -1;
u32 var_handlingcfgslots      = -1;
u32 var_handlingcfgslotsize   = -1;

u32 ptr_particleCFG           = -1;
u32 var_particleCFGslots      = -1;
u32 var_particleCFGslotsize   = -1;

u32 ptr_carcolsDAT            = -1;
u32 var_carcolsdatslots       = -1;
u32 var_carcolsdatslotsize    = -1;

u32 ptr_pedstatTable          = -1;
u32 var_pedstatDATslots       = -1;
u32 var_pedstatDATslotsize    = -1;

u32 ptr_weaponTable           = -1;
u32 var_weaponDATslots        = -1;
u32 var_weaponDATslotsize     = -1;


u32 ptr_timecycDAT            = -1;

u32 ptr_IDEs                  = -1;
u32 ptr_IDETable              = -1;

u32 ptr_gxtloadadr            = -1;
u32 ptr_radarIconList         = -1;

u32 var_radios                = -1;
u32 render                    = -1;

u32 ptr_memory_main           = -1;


/// savedataeditor - scraped since not working on emu and save space
#ifdef SAVEDITOR 
u32 savedatakey  = -1;
u32 titleid      = -1;
u32 saveprefix   = -1;
#endif

#ifdef PREVIEW 
/** Test-Area ***********************************************************************************************/
  
  u32 testoffs = -1; // offset address for testing only
  
  int test = 0;
  int runfunc = 0;
  int patchonce = 1; 
  int TESTVAL1 = -1;
  int TESTVAL2 = -1;
  int TESTVAL3 = -1;
 
  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  
  
  void (*FUN_0028a9b8_CHEAT_ShowCredits)(int param_1);
  void (*FUN_00008bd0)(int param_1);
  
  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  


  int (*findFreeVehicleSlot)();
  void (*FUN_000bc3d4)(int param_1);

  int (*FUN_000bc6d4_checkForGround)(float *param_1, char *param_2,int *param_3,char param_4,char param_5, char param_6, uint param_7, uint param_8, uint param_9, u32 param_10,char param_11);

  int (*FUN_000bc6d4_checkForGround)(float *param_1, char *param_2,int *param_3,char param_4,char param_5, char param_6, uint param_7, uint param_8, uint param_9, u32 param_10,char param_11);
  
  void (*FUN_002cedc0_spawnParkedVehicles)(int *param_1);
  //int (*FUN_002ce7ec)(int *param_1);
    
  ////////////////////////////////////////////
  
  int (*FUN_001621e8)(char *file); 
  int FUN_001621e8_patched(char *file) {
    logPrintf("FUN_002c302c_load: '%s'", file);
    return FUN_001621e8(file);
  }
  
  int (*redirectFile)(char* a); 
  int redirectFile_patched(char* a) {
    
    int ret;
    logPrintf("loaded: '%s'", a);
    
    ///if( doesFileExist(
    
    
    //in FUN_002c2fe4_loadFile ---> fp = FUN_002549a4 (scIoOpen like?)
    //According to FUN_000b41b4_strFileOpen func FUN_002c2fe4_loadFile supports ms0!!!

    
    if( strcmp("DISC0:/PSP_GAME/USRDIR/DATA/MAIN.SCM", a) == 0 ) {
      logPrintf("found scm!");  
      //ret = FUN_001621e8("ms0:LCS/DATA\\main.scm", "r", 1);
      return redirectFile("MS0:/LCS/DATA/MAIN.SCM");
      //ret = sceIoOpen("ms0:/LCS/DATA\\main.scm", 1, 0xFF); 
      //ret = (int)sceIoOpen("DISC0:/PSP_GAME/USRDIR/TEXT/GERMAN.GXT", PSP_O_RDONLY, 0777); 
      
    } else 
      return redirectFile(a);
      
    
    return ret;
  }
  
  //int (*testfunc2)(int param_1);
  //void (*testfunc3)(int param_1);
  
  void (*FUN_00009138_CAutomobile_DoHoverSuspensionRatios)(int param_1);

  ////////////////////////////////////////////  ////////////////////////////////////////////  ////////////////////////////////////////////
  
  uint (*testfunc2)(int param_1, uint param_2); 
  uint testfunc2_patched(int param_1, uint param_2) { //FUN_0016fcd0_CElementGroupModelInfo_GetNodeFromId
    int ret = testfunc2( param_1, param_2);
    logPrintf("ret = 0x%08X, param_1: 0x%08X, param_2: 0x%08X", ret, param_1, param_2);
    static int last = -1;
    if( ret == 0 ) {
      return last;
      //0x160 block
    }
    last = ret;
    return 0;
  }
  
  void (*testfunc)(int ide, int wheel_no, uint *vectors); 
  void testfunc_patched(int ide, int wheel_no, uint *vectors) { //FUN_0007243c_CVehicleModelInfo_GetWheelPosn
    //char buffer[256]; 
    //snprintf(buffer, sizeof(buffer), "z = %.2f, xstick = %.2f, ystick = %.2f", test[0], xstick, ystick);
    //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 20.0f, RED);
      
    logPrintf("ide: 0x%08X, wheel: %d, vectors: 0x%08X", ide, wheel_no, vectors);
  
    //static int last = -1;
    
    if( ide == getAddressOfIdeSlotForID(0xC8) ) { // Aeroplane!
      logPrintf("airtrain!!"); 
      *vectors = 0x3F800000;
      vectors[1] = 0x3F800000;
      vectors[2] = 0x3F800000;
      
      //return testfunc(getAddressOfIdeSlotForID(0xA4), wheel_no, vectors);

    } else {
      testfunc(ide, wheel_no, vectors);
    }
    //logPrintf("ret: 0x%08X", last);
  }
    
  void (*FUN_123)(int vehicle);


/*********************************************************************************************************/
#endif

int PatchLCS(u32 addr, u32 text_addr) { //Liberty City Stories
      
  #ifdef PREVIEW
  if( LCS && patchonce ) { // FOR TESTING ONLY
            
    ///MAKE_DUMMY_FUNCTION(text_addr + 0xea5c0, 0); //
    
    ///testfunc = (void*)(text_addr + 0x182ae0); //make function call-able
    
    ///HIJACK_FUNCTION(text_addr + 0x182ae0, testfunc_patched, testfunc); //replace funtion
    
    ///_sw(0x00000000, text_addr + 0x290ea4); //write to mem

  // // // // // // // // // // // // // // // // // // // // // // // // // // //

    
    /// Example ///// open trunk  /////////////////////////////////////
    // FUN_00008bd0 = (void*)(text_addr + 0x08bd0); // opcode - open trunk?
    
  
    /// Example ///// play Credits Cheat -> calling works //////////////
    // FUN_0028a9b8_CHEAT_ShowCredits = (void*)(text_addr + 0x28a9b8); 
    
  
    /// Example ///// give player an AK47 //////////////////////////////
    // FUN_001c6c28(0x114,1); //requestModel
    //  //not needed FUN_001c6f64(0); //something register model
    // FUN_0019b2f4(pplayer,0x1b,0x78); //GiveWeaponAndAmmo
    //  //not needed FUN_001c28e0(0x114); //something unloads model if not registered with FUN_001c6f64

    
    /// Example ///// loadscreen bar color (like alpha txd) ///////////
    ///red-ish loading bar
    // setByte(text_addr + 0x1bde30 + 0x00, 0x47 ); //R
    // setByte(text_addr + 0x1bde30 + 0x04, 0x0E ); //G
    // setByte(text_addr + 0x1bde30 + 0x08, 0x00 ); //B
    ///yellow background
    // setByte(text_addr + 0x1bddbc + 0x00, 0xB3 ); //R
    // setByte(text_addr + 0x1bddbc + 0x04, 0x8D ); //G
    // setByte(text_addr + 0x1bddbc + 0x08, 0x00 ); //B    
  
  // // // // // // // // // // // // // // // // // // // // // // // // // // //  
    
    /// GTA native additions (LCS 3.00 only)
    if( ADRENALINE ) { //ULUS-10041_v3.00
    
      /// Fix Stats
      //_sw(0x34050000 | 480, text_addr + 0x2DF9F0); //stats values
      //_sw(0x34050000 | 480, text_addr + 0x2DFC2C); //stats rating value
      //_sw(0x34040000 | 480, text_addr + 0x2DFC50); //stats rating name
      
      /// Multiplayer Character & Lightbeam
      /*
      _sh(0x441B, text_addr + 0xB4AB8); //lightbeam x (to 620.0)
      _sh(0x4348, text_addr + 0xB4AC0); //lightbeam y (to 200.0)
      _sh(0x41C0, text_addr + 0xB4AC8); //lightbeam (to 24.0)
      _sh(0x42C8, text_addr + 0xB4AD0); //lightbeam scale (to 100.0)
      _sh(0x437A, text_addr + 0xB4AD8); //lightbeam scale (to 250.0)
      _sh(0x3E2A, text_addr + 0xB4AE0); //lightbeam (to 0.1666..)
    
      //_sh(0xC2B4, text_addr + 0xb4C8C); //  [0xC2B40000 = -90.0f] to ???
      
      //_sh(0xBFF0, text_addr + 0xb4bbc); //character view angle [0xbf70a3d7 = -0.939999997616]
      //_sh(0xC014, text_addr + 0xb4bcc); //character view scale [0xc094cccd = -4.65000009537] (to -2.325 is twice the size)
      //_sh(0xBE4C, text_addr + 0xb4bdc); //character view angle [0xbdcca3d7 = -0.099921874702]
      */
      
      /******************************************************************
      cc 2a 64 8e   lw   param_1,offset DAT_00352acc_ResolutionWidth(s3)
        param1 == a0 !!!
        can be replaced with
      0x3404___ '...4' - li         $a0, 0x___
      
      Examples:      
      _sw(0x34040000 | 480, text_addr + 0x002DB7A8);  -> li $a0 (param_1), 480
      _sw(0x34050000 | 272, text_addr + 0x002db7b0);  -> li $a1 (param_2), 272
      _sw(0x34090000 | 480, text_addr + 0x00AAAAAA);  -> li $t1, 480
      _sw(0x340A0000 | 272, text_addr + 0x00AAAAAA);  -> li $t2, 272
      
      li 
        $a0  = 0x34040000 (param_1)
        $a1  = 0x34050000 (param_2)
        $a2  = 0x34060000 (param_3)
        $a3  = 0x34070000 (param_4)
        
        $t0  = 0x34080000 (param_5)
        $t1  = 0x34090000 ...
        $t2  = 0x340A0000
      
      **********************************/  
        
      /****************
      960 = C0 03 00 00 
      544 = 20 02 00 00
      480 = E0 01
      272 = 10 01

      960 = 0x44700000
      544 = 0x44080000
      480 = 0x43F00000
      272 = 0x43880000
      *****************/
    }
    
    
    //FUN_001a1174 = (void*)(text_addr + 0x1a1174); //setActorSkinTo
    //FUN_001c6f64 = (void*)(text_addr + 0x1c6f64); //loadAllModelsNow
    //FUN_001a1210 = (void*)(text_addr + 0x1a1210); //resfreshActorSkin

//  _sw(0x0C02b570, text_addr + 0x00ad86c); //mode 4
//  _sw(0x34040004, text_addr + 0xad500); //m

    //MAKE_DUMMY_FUNCTION(0x89C0FD0, 0); //for test
    
    //testoffs = text_addr + 0x0385da0; //can use here with  setVal(testoffs, xyz);
    
    //FUN_001621e8 = (void*)(text_addr + 0x2c302c);  //
    //HIJACK_FUNCTION(text_addr + 0x2c302c, FUN_001621e8_patched, FUN_001621e8); //replace funtion
    
    ///print loaded files from UMD



  //testfunc2 = (void*)(text_addr + 0x19a1a0); 
  
  
  //MAKE_DUMMY_FUNCTION(text_addr + 0x0020a0e8, 0); //FUN_0020a0e8_CPhysical_ApplyMoveForce        Only when on ground
  //MAKE_DUMMY_FUNCTION(text_addr + 0x0020a1e4, 0); //FUN_0020a1e4_CPhysical_ApplyTurnForce        Only when on ground

  //MAKE_DUMMY_FUNCTION(text_addr + 0xce30, 0); //FUN_0000ce30_CAutomobile_ProcessControl
  //MAKE_DUMMY_FUNCTION(text_addr + 0x9138, 0); //FUN_00009138_CAutomobile_DoHoverSuspensionRatios    drive on water
  //MAKE_DUMMY_FUNCTION(text_addr + 0xbfc8, 0); //FUN_0000bfc8_CAutomobile_ProcessBuoyancy



  /// fake swimming
  //HIJACK_FUNCTION(text_addr + 0xe7d70, FUN_000e7d70_CCam_IsTargetInWater_patched, FUN_000e7d70_CCam_IsTargetInWater); // MAKE_DUMMY_FUNCTION(text_addr + 0xe7d70, 0); // FUN_000e7d70_CCam_IsTargetInWater
  //FUN_00109dac_CWaterLevel_GetWaterLevel = (void*)(text_addr + 0x109dac); // needs to be called
  //HIJACK_FUNCTION(text_addr + 0x1a8d9c, FUN_001a8d9c_CPed_ProcessBuoyancy_patched, FUN_001a8d9c_CPed_ProcessBuoyancy);
  
  
  /// drive on water
  //FUN_00009138_CAutomobile_DoHoverSuspensionRatios = (void*)(text_addr + 0x9138);  //drive on water (without hover wheels!) -- call with pcar obj
    
  
  /// walk through water without effect
  //MAKE_DUMMY_FUNCTION(text_addr + 0x1a8d9c, 0); //FUN_001a8d9c_CPed_ProcessBuoyancy
  
  
  ////////////// /////////////// //////////// /////////// ///////////
  
  
  //testfunc2 = (void*)(text_addr + 0x16fcd0); //FUN_00....
  
//  HIJACK_FUNCTION(text_addr + 0x16fcd0, testfunc2_patched, testfunc2); //replace funtion
//  HIJACK_FUNCTION(text_addr + 0x7243c, testfunc_patched, testfunc); //replace funtion
  
  //testoffs = text_addr + 0x1a853c; //can use here with  setVal(testoffs, xyz);  
  
  //MAKE_DUMMY_FUNCTION(text_addr + 0x00ea37c, 0); //_camera_Using1stPersonWeaponMode
  
  //_sw(0x1200000C, text_addr + 0x0000FF30); //0x0000FF30: 0x1600000C '....' - bnez       $s0, loc_0000FF64
  //clearICacheFor(text_addr + 0x0000FF30);
  
  //setInt(text_addr + 0x0a23a4, 0);
  //clearICacheFor(text_addr + 0x0a23a4);
  
  //FUN_123 = (void*)(text_addr + 0x11bbb8); 
  
  
  //MAKE_DUMMY_FUNCTION(text_addr + 0x10e8b4, 0); // calc waves
  //MAKE_DUMMY_FUNCTION(text_addr + 0x2ae900, 0); // FUN_00

  
  
  
  // // // // // // // // // // // // // // // // // // // // // // // // // // //  
    
    #ifdef PATCHLOG
    logPrintf("[INFO] patchonce() LCS ran!");
    #endif
    patchonce = 0;
  } 
  #endif
  
  
  #ifdef GAMELOG
  static int patchlogonce = 1; 
  if( LCS && patchlogonce && mod_text_size == 0x0032BFC4 && mod_data_size == 0x0002E110 ) { // ULUS-10041_v3.00
  
    /// Debug Prints ////////////////////////////////////////////
    HIJACK_FUNCTION(text_addr + 0x2f6558, debugprint_patched, debugprint); // error in cpp
    HIJACK_FUNCTION(text_addr + 0x2f59bc, debugprint_patched, debugprint); // general? opcodes etc?! 134
    HIJACK_FUNCTION(text_addr + 0x2e2e48, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2d28d4, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2d1090, debugprint_patched, debugprint); // error mpeg
    HIJACK_FUNCTION(text_addr + 0x2cc578, debugprint_patched, debugprint); // multiplayer
    HIJACK_FUNCTION(text_addr + 0x2c2f2c, debugprint_patched, debugprint); // UMD File IO
    HIJACK_FUNCTION(text_addr + 0x2bfdb8, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2b3148, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2b9664, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2aee08, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2a12d8, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x29f0b4, debugprint_patched, debugprint); // memory error prints HEAP
    HIJACK_FUNCTION(text_addr + 0x299050, debugprint_patched, debugprint); // ped density
    HIJACK_FUNCTION(text_addr + 0x28a378, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x288400, debugprint_patched, debugprint); // CGameLogic
    HIJACK_FUNCTION(text_addr + 0x287dfc, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x27ccc0, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2566f8, debugprint_patched, debugprint); // entity 
    HIJACK_FUNCTION(text_addr + 0x2566cc, debugprint_patched, debugprint); // create xyz
    HIJACK_FUNCTION(text_addr + 0x255a24, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x229bec, debugprint_patched, debugprint); // FindNthNodeClosestToCoor / CheckObjectSpawnPosition
    HIJACK_FUNCTION(text_addr + 0x2297c8, debugprint_patched, debugprint); 
    HIJACK_FUNCTION(text_addr + 0x207768, debugprint_patched, debugprint); // StreamingHeap
    HIJACK_FUNCTION(text_addr + 0x203cc8, debugprint_patched, debugprint); // adhoc
    HIJACK_FUNCTION(text_addr + 0x2035ec, debugprint_patched, debugprint); // frame
    HIJACK_FUNCTION(text_addr + 0x1d15f4, debugprint_patched, debugprint); // car transfer / carjack
    HIJACK_FUNCTION(text_addr + 0x1d1380, debugprint_patched, debugprint); // volatile ram
    HIJACK_FUNCTION(text_addr + 0x1c15d8, debugprint_patched, debugprint); // anim, models "STREAM MODEL xyz"
    HIJACK_FUNCTION(text_addr + 0x1bc8cc, debugprint_patched, debugprint); // Files, Loadscreen etc
    HIJACK_FUNCTION(text_addr + 0x1bc098, debugprint_patched, debugprint); // cInterestZoneManager
    HIJACK_FUNCTION(text_addr + 0x19af68, debugprint_patched, debugprint); // weapon model
    HIJACK_FUNCTION(text_addr + 0x198dcc, debugprint_patched, debugprint); // ClearWaitState
    HIJACK_FUNCTION(text_addr + 0x193810, debugprint_patched, debugprint); // splash screen
    HIJACK_FUNCTION(text_addr + 0x1816b0, debugprint_patched, debugprint); // collision
    HIJACK_FUNCTION(text_addr + 0x17e524, debugprint_patched, debugprint); // "  anim %i: blend %f %f time %f spd %f anm %i, %i flags %i"
    HIJACK_FUNCTION(text_addr + 0x17e4f8, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x17b738, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x17149c, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x1708d4, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x16fecc, debugprint_patched, debugprint); // SCE Errors
    HIJACK_FUNCTION(text_addr + 0x16af14, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x162528, debugprint_patched, debugprint); // marker
    HIJACK_FUNCTION(text_addr + 0x152f0c, debugprint_patched, debugprint); // CTheScripts 
    HIJACK_FUNCTION(text_addr + 0x14bddc, debugprint_patched, debugprint); // cWorldStream
    HIJACK_FUNCTION(text_addr + 0x131774, debugprint_patched, debugprint); //media mp?
    HIJACK_FUNCTION(text_addr + 0x11bfb8, debugprint_patched, debugprint); // car transfer
    HIJACK_FUNCTION(text_addr + 0x119080, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x1157b8, debugprint_patched, debugprint); // pickups
    HIJACK_FUNCTION(text_addr + 0x109aac, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0xe635c, debugprint_patched, debugprint); //camera & fade screen
    HIJACK_FUNCTION(text_addr + 0xd0484, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0xb65d4, debugprint_patched, debugprint); // custom tracks
    HIJACK_FUNCTION(text_addr + 0xb4180, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0xb14a0, debugprint_patched, debugprint); // libwav
    HIJACK_FUNCTION(text_addr + 0xac6c0, debugprint_patched, debugprint); // savedata stuff
    HIJACK_FUNCTION(text_addr + 0xa7c34, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0xa2fc8, debugprint_patched, debugprint); // mp?
    HIJACK_FUNCTION(text_addr + 0x80810, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x74e38, debugprint_patched, debugprint); // heap info
    HIJACK_FUNCTION(text_addr + 0x6f1dc, debugprint_patched, debugprint); // "main: %i %i %i  streaming: %i %i  volatile: %i %i  kernel: %i  FPS: %.1f  GPU%%: %.1f  ptrnode: %i\n"
    HIJACK_FUNCTION(text_addr + 0x690d8, debugprint_patched, debugprint); // Adhoc
    HIJACK_FUNCTION(text_addr + 0x671a4, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x3f62c, debugprint_patched, debugprint); // audio stuff sound thread libwav
    HIJACK_FUNCTION(text_addr + 0x3f23c, debugprint_patched, debugprint); // audio stuff
    HIJACK_FUNCTION(text_addr + 0x38e64, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x35aac, debugprint_patched, debugprint); // cannot find with name
    HIJACK_FUNCTION(text_addr + 0x340cc, debugprint_patched, debugprint);
    HIJACK_FUNCTION(text_addr + 0x2786c, debugprint_patched, debugprint); // adhoc
  
    #ifdef PATCHLOG
    logPrintf("[INFO] patchlogonce() LCS ran!");
    #endif
    patchlogonce = 0;
  } 
  #endif
  
  /// /// /// ULTRA CRITICAL ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  /// 
  
  /*** cWorldStream_Render() *****************
  * ULUS v1.05 - 
  * ULUS v3.00 - 0x00150344 
  * ULES v1.00 - 
  * ULES v2.00 - 
  * ULES v3.00 - 
  * ULJM v1.01 - 
  * ULUX v0.02 - OK
  * 
  * Note:  */
  if( _lw(addr + 0x50) == 0x0080A025 && _lw(addr + 0x5C) == 0x02002025 && _lw(addr + 0x88) == 0x2C840001 ) { // LCS US 3.00 -> 0x00150344
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> cWorldStream_Render()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, cWorldStream_Render_Patched, cWorldStream_Render);
    return 1;
  }
  
  /// for stopMenu(); [in pause menu]
  if( (_lw(addr + 0xC) == 0x00808025  && _lw(addr + 0x10) == 0x341101DD && _lw(addr + 0x28) == 0xAFB20018) || (_lw(addr + 0x8) == 0x00808025  && _lw(addr + 0xC) == 0x340401DD && _lw(addr + 0x28) == 0xAFBF0018) ) { //0x002DB0C0 || 0x002433D0 for ulux
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> FUN_002db0c0()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, FUN_002db0c0_patched, FUN_002db0c0); //
    return 1;
  } // ULUX-002 OK but ugly (functions are quite different) todo?
  
  /// disable Button Input --> same as VCS
  if( _lw(addr + 0x8) == 0x00808025 && _lw(addr + 0x14) == 0x26050034 ) {  //not save
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> buttonsToAction()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, buttonsToActionPatched, buttonsToAction); // 0x00294E88
    return 1;
  }
  
  ///for FPS stuff
  if( _lw(addr - 0x70) == 0x3C043586  && _lw(addr + 0x18) == 0x00402025  && _lw(addr + 0x64) == 0x34040001 ) {  // LCS US 3.00 -> 0x002AF398  
    /*******************************************************************
     *  0x002AF378: 0x2C840002 '...,' - sltiu      $a0, $a0, 2
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_fpsCap", addr-0x20-text_addr, addr-0x20);
    #endif
    addr_fpsCap = addr-0x20; // 0x002AF378
    
    /*******************************************************************
     *  0x002AF398: 0x0C0C1EF1 '....' - jal        ThreadManForUser_82BC5777
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> sceKernelGetSystemTimeWide()", addr-text_addr, addr);
    #endif
    MAKE_CALL(addr, sceKernelGetSystemTimeWidePatched); //LCS US 3.00 -> 0x002AF398
    
    return 1;
  }
  
  
  
  /// /// /// CRITICAL ///  ///  ///  ///  ///  ///  ///  ///  ///  (called in sceKernelGetSystemTimeWidePatched) ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  /// 
  
  /// get pplayer function
  if( _lw(addr + 0x8) == 0x000429C0 && _lw(addr + 0xC) == 0x00A53021 && _lw(addr + 0x18) == 0x00A42023 && _lw(addr + 0x2C) == 0x8C820000 ) {  //0x1d18b0
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> GetPPLAYER()", addr-text_addr, addr);
    #endif
    GetPPLAYER = (void*)(addr); //get pplayer 
    return 1;
  }
  
  /// get pobj function 
  if( _lw(addr - 0x18) == 0x00000000 && _lw(addr + 0x8) == 0x000429C0 && _lw(addr + 0x24) == 0x00852021 && _lw(addr + 0x2C) == 0x10800006  ) {  //0x1d17b4
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> GetPCAR()", addr-text_addr, addr);
    #endif
    GetPCAR = (void*)(addr); // get pcar 
    return 1;
  }
  
  /// global gametimer
  if( _lw(addr - 0xC) == 0x02002825 && _lw(addr - 0x4) == 0x00001025 && _lw(addr + 0x14) == 0x02002025 ) { 
    /*******************************************************************
     *  0x001DB5EC: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x001DB5F0: 0x8C84A144 'D...' - lw         $a0, -24252($a0)
    *******************************************************************/
    global_gametimer = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> global_gametimer 0x%08X", global_gametimer-text_addr, global_gametimer, addr-text_addr); // 0x35A144
    #endif  
    
    return 1;
  }
  
  /// global timescale
  if(  _lw(addr) == 0x3C053F80 && _lw(addr + 0x18) == 0xE48C0010 && _lw(addr + 0x20) == 0x2405FFFF ) { // 0x000e5fe0
    /*******************************************************************
     * 0x000E5FEC: 0x3C050036 '6..<' - lui        $a1, 0x36
     * 0x000E5FF0: 0xE4ADA158 'X...' - swc1       $fpr13, -24232($a1)
    *******************************************************************/
    global_timescale = (_lh(addr+0xC) * 0x10000) + (int16_t)_lh(addr+0x10); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> global_timescale 0x%08X", global_timescale-text_addr, global_timescale, addr-text_addr); // DAT_0035a158_TimeScale
    #endif  
    
    return 1;
  }
  
  /// global_currentisland & global_systemlanguage [this function sets lots of globals -> weather, clock, cheatsused, gamespeed]
  if( _lw(addr + 0x20) == 0xAE040008  && _lw(addr + 0x18C) == 0xAE040070 ) {     
    /*******************************************************************
     *  0x000ACA2C: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x000ACA30: 0x8C84A46C 'l...' - lw         $a0, -23444($a0)
    *******************************************************************/
    global_currentisland = (_lh(addr+0x14) * 0x10000) + (int16_t)_lh(addr+0x18); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> global_currentisland", global_currentisland-text_addr, global_currentisland); // DAT_0035A46C
    #endif
    
    /*******************************************************************
     *  0x000ACA40: 0x3C050035 '5..<' - lui        $a1, 0x35
     *  0x000ACA48: 0x8CA45C9C '.\..' - lw         $a0, 23708($a1)
    *******************************************************************/
    global_systemlanguage = (_lh(addr+0x28) * 0x10000) + (int16_t)_lh(addr+0x30); //actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> global_systemlanguage", global_systemlanguage-text_addr, global_systemlanguage); // DAT_00355c9c
    #endif
    return 1;
  }
  
  /// globals pointers to objects
  if( _lw(addr + 0x2C) == 0x02409825 &&  _lw(addr + 0x6C) == 0x00000000 && _lw(addr + 0xF0) == 0x02402025 ) { //0x00037714
    /*******************************************************************
     *  0x00037718: 0x3C040035 '5..<' - lui        $a0, 0x35
     *  0x00037720: 0x8C90853C '<...' - lw         $s0, -31428($a0)
     *
     *  0x00037744: 0x24040CB0 '...$' - li         $a0, 3248
    *******************************************************************/
    ptr_pedestriansobj = (_lh(addr+0x4) * 0x10000) + (int16_t)_lh(addr+0xC); // actual address!
    var_pedobjsize = (int16_t)_lh(addr+0x30); // 0xCB0
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> ptr_pedestriansobj", ptr_pedestriansobj-text_addr, ptr_pedestriansobj); // DAT_0034853c
    logPrintf("var_pedobjsize = 0x%X", var_pedobjsize); //
    #endif
    
    /*******************************************************************
     *  0x00037798: 0x3C040035 '5..<' - lui        $a0, 0x35
     *  0x0003779C: 0x8C908540 '@...' - lw         $s0, -31424($a0)
     *
     *  0x00037814: 0x2631F920 ' .1&' - addiu      $s1, $s1, -1760
    *******************************************************************/
    ptr_vehiclesobj = (_lh(addr+0x84) * 0x10000) + (int16_t)_lh(addr+0x88); // actual address!
    var_vehobjsize = 0x10000 - _lh(addr+0x100); // 0x6E0
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> ptr_vehiclesobj", ptr_vehiclesobj-text_addr, ptr_vehiclesobj); // DAT_00348540
    logPrintf("var_vehobjsize = 0x%X", var_vehobjsize); //
    #endif
    
    /*******************************************************************
     *  0x00037818: 0x3C040035 '5..<' - lui        $a0, 0x35
     *  0x0003781C: 0x8C92854C 'L...' - lw         $s2, -31412($a0)
     *
     *  0x00037884: 0x2673FDE0 '..s&' - addiu      $s3, $s3, -544
    *******************************************************************/
    ptr_worldobj = (_lh(addr+0x104) * 0x10000) + (int16_t)_lh(addr+0x108); // actual address!
    var_wldobjsize = 0x10000 - _lh(addr+0x170); // 0x220
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> ptr_worldobj", ptr_worldobj-text_addr, ptr_worldobj); // DAT_0034854c
    logPrintf("var_wldobjsize = 0x%X", var_wldobjsize); //
    #endif
    
    return 1;
  }
  
  
  
  /// /// /// FOR CHEATS ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  /// 
  
  /// skip intro movies
  if( _lw(addr + 0x8) == 0x3C05005A && _lw(addr + 0x24) == 0x00002825  ) { 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_skipIntroMovie", addr-text_addr, addr);
    #endif
    addr_skipIntroMovie = addr; // 0x001BCFD0
    //MAKE_DUMMY_FUNCTION(0x89C0FD0, 0); //for test
    return 1;
  }

  /// never fall off bike.. when rolling backwards
  if( _lw(addr-0x18) == 0x45000012 && _lw(addr-0xC) == 0x3405002C && _lw(addr - 0x4) == 0x02403825 && _lw(addr + 0x8) == 0x8E0401FC ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_neverFallOffBike_rollback", addr-text_addr, addr);
    #endif
    addr_neverFallOffBike_rollback = addr; // 0x000DEF44
    return 1;
  }
  
  /// never fall off bike.. when hitting object
  if( _lw(addr - 0x4) == 0x34050027 && _lw(addr + 0x4) == 0x00004025 && _lw(addr + 0xB8) == 0x00000000 ) { // last one is #hacky
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_neverFallOffBike_hitobj", addr-text_addr, addr);
    #endif
    addr_neverFallOffBike_hitobj = addr; // 0x0023DBFC | ULUX -> 0x0020B2D8
    return 1;
  }
  
  /// world gravity
  if( _lw(addr - 0x18) == 0x30A50002 && _lw(addr) == 0x3C053C03 && _lw(addr + 0x4) == 0x34A5126F ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_worldgravity", addr-text_addr, addr);
    #endif
    addr_worldgravity = addr; // 0x0020A898
    return 1;
  }
  
  /// set weather function(s)
  if( _lw(addr + 0x1C) == 0x2404FFFF  && _lw(addr - 0x14) == 0x03E00008 && _lw(addr - 0x8) == 0x03E00008 && _lw(addr+0x14) == 0x03E00008 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetNextWeather(), SetWeatherNow(), ReleaseWeather()", addr-text_addr, addr);
    #endif
    SetNextWeather = (void*)(addr-0xC);  // 0x12EDB0
    SetWeatherNow  = (void*)(addr);      // 0x12edbc
    ReleaseWeather = (void*)(addr+0x1C); // 0x12EDD8
    /*******************************************************************
     *  
     *  
    *******************************************************************/
    global_weather = (_lh(addr+0x8) * 0x10000) + (int16_t)_lh(addr + 0xC); // actual address!  (only to READ current weather)
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_weather", global_weather-text_addr, global_weather); // 0x0035A484
    #endif
    return 1;
  }
  
  
  /// traffic density multiplier value
  if( _lw(addr - 0x10) == 0x34060001 && _lw(addr - 0x18) == 0x02002025 && _lw(addr+0x8) == 0x100000B7 && _lw(addr+0xC) == 0x00001025 ) { 
    /*******************************************************************
     *  
     *  
    *******************************************************************/
    global_trafficdensity = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_trafficdensity", global_trafficdensity-text_addr, global_trafficdensity); // 0x00347CDC
    #endif
    return 1;
  }
  
  /// ped density multiplier value
  if( _lw(addr + 0x38) == 0x00001025 && _lw(addr + 0x4C) == 0x00A62821 && _lw(addr + 0x84) == 0x00001025 ) { 
    /*******************************************************************
     *  
     *  
    *******************************************************************/
    global_peddensity = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_peddensity", global_peddensity-text_addr, global_peddensity); // 0x00352610
    #endif
    return 1;
  }
  
    
  /// global settings toggles (radar, hud and more)
  if( _lw(addr - 0x10) == 0x34040120 && _lw(addr - 0x3C) == 0x34040001 ){
    global_hudbool = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_hudbool", global_hudbool-text_addr, global_hudbool); // 0x00355C4A
    #endif
    
    global_radarbool = global_hudbool + 0x2; // hehe-hacky (actual address!)
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_radarbool", global_radarbool-text_addr, global_radarbool); // DAT_00355c4c_ui_radar
    #endif
    
    global_maplegendbool = global_hudbool + 0x23; // hehe-hacky (actual address!)
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_maplegendbool", global_maplegendbool-text_addr, global_maplegendbool); // DAT_00355c6d_mapLegendBool
    #endif
    return 1;
  }
  
  /// global max health & armor multiplier
  if( _lw(addr - 0x24) == 0x00001025 &&  _lw(addr - 0x14) == 0x02002025 && _lw(addr + 0x64) == 0x02002025 && _lw(addr + 0xF8) == 0x00408825  ) { 
    /*******************************************************************
     *  0x00192274: 0x3C050038 '8..<' - lui        $a1, 0x38
     *  0x0019228C: 0x24A55B10 '.[.$' - addiu      $a1, $a1, 23312
     *  0x00192294: 0x90850167 'g...' - lbu        $a1, 359($a0)
    *******************************************************************/
    global_maxhealthmult = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x18) + _lh(addr+0x20); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_maxhealthmult", global_maxhealthmult-text_addr, global_maxhealthmult); // 0x00385c77
    #endif
  
    /*******************************************************************
     *  0x001922EC: 0x3C050038 '8..<' - lui        $a1, 0x38
     *  0x00192304: 0x24A55B10 '.[.$' - addiu      $a1, $a1, 23312
     *  0x0019230C: 0x90850168 'h...' - lbu        $a1, 360($a0)
    *******************************************************************/
    #ifdef PREVIEW
    TESTVAL1 = _lh(addr);
    TESTVAL2 = _lh(addr+0x18);
    TESTVAL3 = _lh(addr+0x20);
    #endif
    
    global_maxarmormult = (_lh(addr+0x78) * 0x10000) + (int16_t)_lh(addr+0x90) + _lh(addr+0x98); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_maxarmormult", global_maxarmormult-text_addr, global_maxarmormult); // 0x00385c78
    #endif
    return 1;
  }
  
  
  
  /// global never get tired from sprinting & Multiplayer bool
  if( _lw(addr + 0x28) == 0x3C05C316 && _lw(addr + 0x84) == 0x3C063F00 && _lw(addr + 0xA8) == 0x3C0543FA ) { // 0x0013F37C (in LCS FUN_0013f37c, VCS FUN_001447a0)
    /*******************************************************************
     *  0x0013F37C: 0x3C050035 '5..<' - lui        $a1, 0x35
     *  0x0013F380: 0x90A52C7C '|,..' - lbu        $a1, 11388($a1)
    *******************************************************************/
    global_ismultiplayer = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_ismultiplayer", global_ismultiplayer-text_addr, global_ismultiplayer); // DAT_00352c7c_isMultiplayerActive
    #endif
  
    /*******************************************************************
     *  0x0013F3D8: 0x3C060038 '8..<' - lui        $a2, 0x38
     *  0x0013F3DC: 0x24C65B10 '.[.$' - addiu      $a2, $a2, 23312
     *  0x0013F3E0: 0x00A62821 '!(..' - addu       $a1, $a1, $a2
     *  0x0013F3E4: 0x90A50164 'd...' - lbu        $a1, 356($a1)
    *******************************************************************/
    global_unlimtedsprint = (_lh(addr+0x5C) * 0x10000) + (int16_t)_lh(addr+0x60) + _lh(addr+0x68); //actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_unlimtedsprint", global_unlimtedsprint-text_addr, global_unlimtedsprint); // DAT_00385c74_playerNeverGetsTired
    #endif
    return 1;
  }
  
  
  /// SetWantedLevel function
  if( _lw(addr + 0x64) == 0x00409025 && _lw(addr - 0xE0) == 0x340400CA ) { // 0x00140DF4
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetWantedLevel()", addr-text_addr, addr);
    #endif
    SetWantedLevel = (void*)(addr);
    return 1;
  } 
  
  /// SetMaxWantedLevel function + global
  if( _lw(addr + 0x10) == 0x2C850007 && _lw(addr + 0x90) == 0x34060005 ) { // 0x002C9570
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetMaxWantedLevel()", addr-text_addr, addr);
    #endif
    SetMaxWantedLevel = (void*)(addr);
    /*******************************************************************
     *  0x002C958C: 0x3C050035 '5..<' - lui        $a1, 0x35
     *  0x002C95B4: 0xACA64B9C '.K..' - sw         $a2, 19356($a1)
    *******************************************************************/
    global_maxwantedlevel = (_lh(addr+0x1C) * 0x10000) + (int16_t)_lh(addr+0x44); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_maxwantedlevel", global_maxwantedlevel-text_addr, global_maxwantedlevel); // DAT_00354b9c
    #endif
    return 1;
  } 
  
  
  /// global clock multiplier
  if( _lw(addr + 0x8) == 0x3404000C && _lw(addr - 0x14) == 0x00808025 && _lw(addr + 0xA0) == 0x00000000 ) { // 0x0003834C
    /*******************************************************************
     *  0x0003834C: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x00038350: 0xAC90A1A4 '....' - sw         $s0, -24156($a0)
    *******************************************************************/
    global_clockmultiplier = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_clockmultiplier", global_clockmultiplier-text_addr, global_clockmultiplier); // DAT_0035A1A4
    #endif
    return 1;
  }
  
  
  /// globals cheat used boolean & counter (from Cheat_WeaponSet1)
  if( _lw(addr - 0x4) == 0x34040120 && _lw(addr - 0x1C) == 0x34040115 && _lw(addr - 0x5C) == 0x00402025 ) {  // 0x000B8FDC / ULUX -> 0x001F8E70
    /*******************************************************************
     *  0x0028FB20: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x0028FB24: 0x3C040035 '5..<' - lui        $a0, 0x35
     *  0x0028FB28: 0x8CA6A38C '....' - lw         $a2, -23668($a1)
     *  0x0028FB2C: 0xA0902432 '2$..' - sb         $s0, 9266($a0)
    *******************************************************************/
    global_cheatusedboolean = (_lh(addr+0x4) * 0x10000) + (int16_t)_lh(addr+0xC); // actual address!
    global_cheatusedcounter = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_cheatusedboolean", global_cheatusedboolean-text_addr, global_cheatusedboolean); // DAT_00352432
    logPrintf("0x%08X (0x%08X) -> global_cheatusedcounter", global_cheatusedcounter-text_addr, global_cheatusedcounter); // DAT_0035A38C
    #endif
    return 1;
  }
  
  
  /// global_freezegame
  if( _lw(addr + 0xE0) == 0x3405009E && _lw(addr + 0x30) == 0x02002025 ) { // 0x000B8FDC / ULUX -> 0x0010B00C
    /*******************************************************************
     *  0x000B8FDC: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x000B8FE0: 0x9084A4F8 '....' - lbu        $a0, -23304($a0)
    *******************************************************************/
    global_freezegame = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> global_freezegame", global_freezegame-text_addr, global_freezegame); // DAT_0035A4F8
    #endif
    return 1;
  }
  
  
  /// globals for helpbox (via "FUN_00182e94_ClearSmallPrints")
  if( _lw(addr - 0x10) == 0x14C0FFF2 && _lw(addr + 0x5C) == 0x28A90100 ) { // US 3.00 -> 0x00182E94 / ULUX -> 0x001599BC
    /*******************************************************************
     *  0x00182EB4: 0x3C080065 'e..<' - lui        $t0, 0x65
     *  0x00182EC8: 0x25089DC0 '...%' - addiu      $t0, $t0, -25152
    *******************************************************************/
    global_helpbox_string = (_lh(addr+0x20) * 0x10000) + (int16_t)_lh(addr+0x34); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_helpbox_string", global_helpbox_string-text_addr, global_helpbox_string); // DAT_00649dc0
    #endif
    /*******************************************************************
     *  0x00182EFC: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x00182F00: 0xACA0A808 '....' - sw         $zr, -22520($a1)
    *******************************************************************/
    global_helpbox_timedisplayed = (_lh(addr+0x68) * 0x10000) + (int16_t)_lh(addr+0x6C); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_helpbox_timedisplayed", global_helpbox_timedisplayed-text_addr, global_helpbox_timedisplayed); // DAT_0035a808
    #endif
    /*******************************************************************
     *  0x00182F0C: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x00182F10: 0xACA0A810 '....' - sw         $zr, -22512($a1)
    *******************************************************************/
    global_helpbox_displaybool = (_lh(addr+0x78) * 0x10000) + (int16_t)_lh(addr+0x7C); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_helpbox_displaybool", global_helpbox_displaybool-text_addr, global_helpbox_displaybool); // DAT_0035a810
    #endif
    /*******************************************************************
     *  0x00182F1C: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x00182F20: 0xA0A0A815 '....' - sb         $zr, -22507($a1)
    *******************************************************************/
    global_helpbox_permboxbool = (_lh(addr+0x88) * 0x10000) + (int16_t)_lh(addr+0x8C); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_helpbox_permboxbool", global_helpbox_permboxbool-text_addr, global_helpbox_permboxbool); // DAT_0035a815
    #endif
    /*******************************************************************
     *  0x00182F5C: 0x3C060036 '6..<' - lui        $a2, 0x36
     *  0x00182F64: 0xE4CCA818 '....' - swc1       $fpr12, -22504($a2)
    *******************************************************************/
    global_helpbox_duration = (_lh(addr+0xC8) * 0x10000) + (int16_t)_lh(addr+0xD0); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_helpbox_duration", global_helpbox_duration-text_addr, global_helpbox_duration); // DAT_0035a818
    #endif
    
    
    /** BONUS *****************************************************************
     *  0x00182EC0: 0x3C040065 'e..<' - lui        $a0, 0x65
     *  0x00182ED4: 0x24849BC0 '...$' - addiu      $a0, $a0, -25664
    *******************************************************************/
    global_dialog_string = (_lh(addr+0x2C) * 0x10000) + (int16_t)_lh(addr+0x40); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_dialog_string", global_dialog_string-text_addr, global_dialog_string); // DAT_00649bc0
    #endif
    return 1;
  }
  
  
  /// global camera stuff
  if( _lw(addr - 0x18) == 0x108001E6 && _lw(addr - 0x28) == 0x30840001 && _lw(addr + 0xA0) == 0x34110006 ) { // 0x001E1818 / ULUX -> 0x00192A90
    /*******************************************************************
     *  0x001E1818: 0x3C040038 '8..<' - lui        $a0, 0x38
     *  0x001E1820: 0x2484F3A0 '...$' - addiu      $a0, $a0, -3168
    *******************************************************************/
    global_camera = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); //actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_camera", global_camera-text_addr, global_camera); // DAT_0037F3A0
    #endif
    return 1;
  }
  
  
  /** UNCENSOR GERMAN VERSION **************** https://www.eg.bucknell.edu/~csci320/mips_web/ ********************************
  *
  * If the flag is set to 0x0 like in the German version it will have the following effects on the gameplay:
  * - player can't kick peds on ground
  * - no head exploding with sniper when headshot
  * - dead peds don't drop money
  * - bloody tire tracks
  * - no bonecracking sound when running over peds
  * - people wasted stat missing in stats menu
  *
  * Other differences
  * - Rampages missing
  ****************************************************************************************************************************/
  if( _lw(addr+0x4) == 0xA4440082 && _lw(addr) == 0x34040002 && _lw(addr+0x8) == 0x34150002 ) { // 0x001BCE54 (in FUN_001BCDE8 called once at game startup)
    #ifdef PATCHLOG
    logPrintf("UNCENSOR GERMAN VERSION 1/2! (0x%08X)", addr-text_addr);
    #endif
    
    /// set "isUncut" flag to 0x1
    /*******************************************************************
     *  0xA2204C74  sb  $zr, 19572($s1)   ->  0xA2334C74  $s3, 19572($s1)   [$s3 holds 0x1]
     *******************************************************************/
    if( _lh(addr+0x2E) == 0xA220 ) _sh(0xA233, addr+0x2E); // 0x001BCE80 - just to be sure 
    if( _lh(addr+0x4E) == 0xA220 ) _sh(0xA233, addr+0x4E); // 0x001BCEA0 - just to be sure 
    if( _lh(addr+0xA2) == 0xA220 ) _sh(0xA233, addr+0xA2); // 0x001BCEF4
    
    /// set "isGerman" flag to 0x0 (set later in other function via language check like other game versions)
    /*******************************************************************
     *  $s3, 19574($s2) ->  $zr, 19574($s2)
     *******************************************************************/
    /*
    if( _lh(addr+0x26) == 0xA253 ) _sh(0xA240, addr+0x26); // just to be sure 
    if( _lh(addr+0x6E) == 0xA253 ) _sh(0xA240, addr+0x6E); // just to be sure 
    if( _lh(addr+0x86) == 0xA253 ) _sh(0xA240, addr+0x86); // just to be sure  
    if( _lh(addr+0xA6) == 0xA253 ) _sh(0xA240, addr+0xA6); // just to be sure 
    */
    
    return 1;
  } 
  if( _lw(addr) == 0x34040994 && _lw(addr-0xDC) == 0x34150001 && (_lw(addr+0x40) == _lw(addr+0x90)) ) {  //0x002D695C (in FUN_002D6838 called when ???)
    #ifdef PATCHLOG
    logPrintf("UNCENSOR GERMAN VERSION 2/2! (0x%08X)", addr-text_addr);
    #endif
    
    /*******************************************************************
     *  0xA2C04C74  sb  $zr, 19572($s6)   ->  0xA2D54C74  sb   $s5 19572 $s6          [$s5 holds 0x1]
     *******************************************************************/
    if( _lh(addr+0x42) == 0xA2C0 ) _sh(0xA2D5, addr+0x42); // 0x002D699C - set "isUncut" flag 0x1

    /// set "isGerman" 0x0 here -> set if language matches right after
    /*******************************************************************
     *  0xA0954C76 $s5, 19574($a0) -> $zr, 19574($a0)
     *******************************************************************/
    //if( _lh(addr+0x3E) == 0xA095 ) _sh(0xA080, addr+0x3E); // just to be sure 
    
    /*******************************************************************
     *  0xA2C04C74 sb   $zr, 19572($s6)   ->  0xA2D54C76 (create "isGerman" flag instead like other gameversions use)
     *******************************************************************/
    if( _lh(addr+0x92) == 0xA2C0 ) { // replace flag from "isGerman" to "isUncut" like the other game versions and enable
      _sh(0xA2D5, addr+0x92);
      _sh(_lh(addr+0x3C), addr+0x90); 
    }
    
    return 1;
  } 


  /// global_hudincutscene 
  if( _lw(addr + 0x7C) == 0x34120096  && _lw(addr + 0x60) == 0x34110118 && _lw(addr + 0x24) == 0x34130001  ) { 
    /*******************************************************************
     *  0x00163198: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x0016319C: 0x9084A214 '....' - lbu        $a0, -24044($a0)
    *******************************************************************/
    global_hudincutscene = (_lh(addr+0x3C) * 0x10000) + (int16_t)_lh(addr+0x40); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_hudincutscene", global_hudincutscene-text_addr, global_hudincutscene); // DAT_0035a214
    #endif
    return 1;
  } 
  

  /// global_ptr_water 
  if( _lw(addr + 0x118) == 0x90A50129 && _lw(addr + 0x40) == 0x3404000C ) { 
    /*******************************************************************
     *  0x00109ADC: 0x3C040033 '3..<' - lui        $a0, 0x33
     *  0x00109AE8: 0xAC851FE0 '....' - sw         $a1, 8160($a0)
    *******************************************************************/
    global_ptr_water = (_lh(addr+0x4) * 0x10000) + (int16_t)_lh(addr+0x10); // actual address!
    #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> global_ptr_water", global_ptr_water-text_addr, global_ptr_water); // DAT_00331fe0_toWater
    #endif
    return 1;
  } 
  
  
  /// global_buttoninput (used for to trigger stock cheats)
  if( _lw(addr + 0x10) == 0x3053FFFF && _lw(addr + 0x44) == 0x0093282A ) {  // 0x0025A5C4
    /*******************************************************************
     *  0x0025A5C4: 0x3C040038 '8..<' - lui        $a0, 0x38
     *  0x0025A5C8: 0x24955DA0 '.].$' - addiu      $s5, $a0, 23968
    *******************************************************************/
    global_buttoninput = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_buttoninput", global_buttoninput-text_addr, global_buttoninput); // 0x00385DA0
    #endif
    return 1;
  } 
  
  /// addr_buttoncheat (used for to trigger stock cheats)
  if( _lw(addr + 0x10) == 0x34050032 && _lw(addr - 0x24) == 0x34050031 ) { // 0x00292B20
    /*******************************************************************
     *  0x00292B20: 0x10800004 '....' - beqz       $a0, loc_00292B34
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_buttoncheat", addr-text_addr, addr);
    #endif
    addr_buttoncheat = addr; // 0x00292B20
    return 1;
  } 
  
  /// global_garagedata
  /*************************************
   * ULUS-10041 v1.05 | 0x001376B4 | OK!
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 0x0026D218 | OK!
   **************************************/ 
  if( _lw(addr - 0x20) == 0x2404FFFF && _lw(addr + 0x30) == 0x29040004 ) { //
    
    /*******************************************************************
     *  0x001376B4: 0x3C050063 'c..<' - lui        $a1, 0x63
     *  0x001376C0: 0x24A54A88 '.J.$' - addiu      $a1, $a1, 19080
    *******************************************************************/
    global_garagedata = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0xC); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X: 0x%08X -> global_garagedata", addr-text_addr, global_garagedata-text_addr); // 0x634A88
    #endif
    
    /*******************************************************************
     *  0x001376D4: 0x28E9000C '...(' - slti       $t1, $a3, 12
     *  0x001376EC: 0x24C6002C ',..$' - addiu      $a2, $a2, 44
     *******************************************************************/
    var_garageslots = *(char*)(addr+0x20); // 0xC = 12 slots (4 in each garage)
    var_garageslotsize = *(char*)(addr+0x38); // 0x2C
    #ifdef PATCHLOG
    logPrintf("var_garageslots = 0x%08X", var_garageslots); //
    logPrintf("var_garageslotsize = 0x%08X", var_garageslotsize); //
    #endif
    
    return 1;
  }
    
  /// LCS Pickups
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
  if( _lw(addr - 0x74) == 0x30870003 && _lw(addr + 0x44) == 0x00001025 &&  _lw(addr + 0x4) == 0x34050000  ) { // 0x0027E680
    /*******************************************************************
     *  0x0027E680: 0x3C020062 'b..<' - lui        $v0, 0x62
     *  0x0027E688: 0x24424980 '.IB$' - addiu      $v0, $v0, 18816
    *******************************************************************/
    global_pickups = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_pickups", global_pickups-text_addr, global_pickups); // 0x00624980
    #endif
    
    /*******************************************************************
     *  
     *  
    *******************************************************************/
    var_pickupslots = (int16_t)_lh(addr+0x34); // *(short*)(addr+0x34);
    var_pickupslotsize = (int16_t)_lh(addr+0x3C); // *(char*)(addr+0x3C); 
    #ifdef PATCHLOG
    logPrintf("var_pickupslots = 0x%08X", var_pickupslots); // 0x14F ? 0x150
    logPrintf("var_pickupslotsize = 0x%08X", var_pickupslotsize); // 0x60
    #endif
    return 1;
  }

  
  /// Custom Music
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
  if( _lw(addr + 0x2C) == 0x8C84000C && _lw(addr + 0x38) == 0x01094021 && _lw(addr + 0x64) == 0x00000000 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> _checkCustomTracksReady()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, _checkCustomTracksReady_patched, _checkCustomTracksReady); // sub_00034A28
    return 1;
  } // not for ULUX
  if( _lw(addr + 0x24) == 0x2A040038 && _lw(addr - 0x28) == 0x34050016 ) {  // 0x00296958
    /*******************************************************************
     *  0x00296958: 0x3C040039 '9..<' - lui        $a0, 0x39
     *  0x00296960: 0x2484B4F8 '...$' - addiu      $a0, $a0, -19208
    *******************************************************************/
    global_radioarea = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_radioarea", global_radioarea-text_addr, global_radioarea); // 0x0038B4F8
    #endif
    return 1;
  } 
  
  /*************************************
   * ULUS-10041 v1.05 | 0x000AD924 | OK!
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | too different!
   **************************************/ 
  if( _lw(addr + 0x4C) == 0x34060014 && _lw(addr + 0x74) == 0x34060160 ) { // 0x000AD924
    /*******************************************************************
     *  0x000AD924: 0x3C040035 '5..<' - lui        $a0, 0x35
     *  0x000AD928: 0x8C846F70 'po..' - lw         $a0, 28528($a0)
    *******************************************************************/
    global_custrackarea = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_custrackarea", global_custrackarea-text_addr, global_custrackarea); // DAT_00356f70_CustomTracksArea
    #endif
    return 1;
  } 
  
  
  /// LCS Radar Blips
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
  if( _lw(addr + 0x80) == 0x3C043F80 && _lw(addr + 0x3C) == 0x24420001 &&  _lw(addr + 0x14) == 0x01094021  ) { // FUN_00163e40
    /*******************************************************************
     *  0x00163E58: 0x3C090064 'd..<' - lui        $t1, 0x64
     *  0x00163E5C: 0x25296A20 ' j)%' - addiu      $t1, $t1, 27168
    *******************************************************************/
    global_radarblips = (_lh(addr+0x18) * 0x10000) + (int16_t)_lh(addr+0x1C); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_radarblips", global_radarblips-text_addr, global_radarblips); // DAT_00646A20
    #endif
    
    /*******************************************************************
     *  0x00163E6C: 0x2C48004B 'K.H,' - sltiu      $t0, $v0, 75
    *******************************************************************/
    var_radarblipslots = (int16_t)_lh(addr+0x2C);
    #ifdef PATCHLOG
    logPrintf("var_radarblipslots = 0x%08X", var_radarblipslots); //0x4B
    #endif
    
    /*******************************************************************
     *  0x00163E7C: 0x24420001 '..B$' - addiu      $v0, $v0, 1      dec 1 = bits: ....000001
     *  0x00163E8C: 0x00024180 '.A..' - sll        $t0, $v0, 6      shift bits to left by 6   = ..01000000 = 0x40 (64)
     *  0x00163E90: 0x00024900 '.I..' - sll        $t1, $v0, 4      shift bits to left by 4    =   ..010000 = 0x10 (16)
     *  0x00163E94: 0x01094021 '!@..' - addu       $t0, $t0, $t1    add t0 and t1 = 0x50 (80)
     *******************************************************************/
    var_radarblipslotsize = 0x50; // TODO
    #ifdef PATCHLOG
    logPrintf("var_radarblipslotsize = 0x%08X", var_radarblipslotsize); //0x50
    #endif
    return 1;
  }
  
  
  /// GAME.DTZ - FUN_000673bc_loadGAMEDTZ (contains pointers (& ptr to ptrs) to all files packed in the dtz) (GAME.DTZ is loaded to 0xC05380)
  /*************************************
   * ULUS-10041 v1.05 | 0x000673BC | OK!
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | too different!
   **************************************/ 
  if( _lw(addr + 0x98) == 0x3C044754 && _lw(addr + 0xC0) == 0x30A500FF ) {  // FUN_000673bc
    /*******************************************************************
     *  0x000673E4: 0x3C040038 '8..<' - lui        $a0, 0x38
     *  0x000673F0: 0x2484D640 '@..$' - addiu      $a0, $a0, -10688
    *******************************************************************/
    ptr_memory_main = (_lh(addr+0x28) * 0x10000) + (int16_t)_lh(addr+0x34);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_memory_main", ptr_memory_main-text_addr, ptr_memory_main); // DAT_0037d640
    #endif

    
     /*******************************************************************
     *  0x000675B8: 0x3C050035 '5..<' - lui        $a1, 0x35
     *  0x000675BC: 0xACA41F14 '....' - sw         $a0, 7956($a1)
    *******************************************************************/
    ptr_handlingCFG = (_lh(addr+0x1FC) * 0x10000) + (int16_t)_lh(addr+0x200);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_handlingCFG", ptr_handlingCFG-text_addr, ptr_handlingCFG); // DAT_00351f14_PTR_handling.cfg
    #endif
    var_handlingcfgslots     = 80; // where to find? TODO
    var_handlingcfgslotsize   = 0xF0; // where to find?
    
    
    /*******************************************************************
     *  0x0006749C: 0x3C050035 '5..<' - lui        $a1, 0x35
     *  0x000674A0: 0xACA48544 'D...' - sw         $a0, -31420($a1)
    *******************************************************************/
    ptr_buildingsIPL = (_lh(addr+0xE0) * 0x10000) + (int16_t)_lh(addr+0xE4);
    var_buildingsIPLslotsize = 0x60; //todo?!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_buildingsIPL", ptr_buildingsIPL-text_addr, ptr_buildingsIPL); // DAT_00348544_PTR_toPTRtoMapParts1
    #endif
    
    
    /*******************************************************************
     *  0x000674A8: 0x3C050035 '5..<' - lui        $a1, 0x35
     *  0x000674AC: 0xACA48548 'H...' - sw         $a0, -31416($a1)
    *******************************************************************/
    ptr_treadablesIPL = (_lh(addr+0xEC) * 0x10000) + (int16_t)_lh(addr+0xF0);
    var_treadablesIPLslotsize = 0x60; //todo?
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_treadablesIPL", ptr_treadablesIPL-text_addr, ptr_treadablesIPL); // DAT_00348548_PTR_toPTRtoMapParts2
    #endif
    
    
    /*******************************************************************
     *  0x000674B4: 0x3C050035 '5..<' - lui        $a1, 0x35
     *  0x000674B8: 0xACA48550 'P...' - sw         $a0, -31408($a1)
    *******************************************************************/
    ptr_dummysIPL = (_lh(addr+0xF8) * 0x10000) + (int16_t)_lh(addr+0xFC);
    var_dummysIPLslotsize = 0x60; //todo?
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_dummysIPL", ptr_dummysIPL-text_addr, ptr_dummysIPL); // DAT_00348550_PTR_toPTRtoMapParts3
    #endif
    
    
    /*******************************************************************
     *  0x000675F0: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x000675F4: 0xACA4A42C ',...' - sw         $a0, -23508($a1)
    *******************************************************************/
    ptr_particleCFG = (_lh(addr+0x234) * 0x10000) + (int16_t)_lh(addr+0x238);
    var_particleCFGslots   = 82; //where to find?
    var_particleCFGslotsize = 0x94; //where to find?
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_particleCFG", ptr_particleCFG-text_addr, ptr_particleCFG); // DAT_0035a42c_particle.cfg
    #endif
  
  
    /*******************************************************************
     *  0x000675D0: 0x3C040035 '5..<' - lui        $a0, 0x35
     *  0x000675D4: 0x8C841980 '....' - lw         $a0, 6528($a0)
    *******************************************************************/
    ptr_timecycDAT = (_lh(addr+0x214) * 0x10000) + (int16_t)_lh(addr+0x218);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_timecycDAT", ptr_timecycDAT-text_addr, ptr_timecycDAT); // PTR_DAT_00351980_timecyc.dat
    #endif
    
    // get more file locations here TODO

    return 1;
  }
  if( _lw(addr + 0x94) == 0x3C044754 && _lw(addr + 0xC4) == 0x30A500FF ) { // 0x00011E84 - ULUX ONLY VERSION (because DTZ func too different)
    #ifdef PATCHLOG
    logPrintf("ptr_buildingsIPL, ptr_treadablesIPL, ptr_dummysIPL, ptr_handlingCFG, ptr_particleCFG, ptr_timecycDAT, ptr_memory_main");
    #endif
    
    ptr_memory_main = (_lh(addr+0x24) * 0x10000) + (int16_t)_lh(addr+0x30);
  
    ptr_buildingsIPL = (_lh(addr+0xF8) * 0x10000) + (int16_t)_lh(addr+0xFC);
    var_buildingsIPLslotsize = 0x60; // todo?!
    
    ptr_treadablesIPL = (_lh(addr+0x104) * 0x10000) + (int16_t)_lh(addr+0x108);
    var_treadablesIPLslotsize = 0x60; // todo?
  
    ptr_dummysIPL = (_lh(addr+0x110) * 0x10000) + (int16_t)_lh(addr+0x114);
    var_dummysIPLslotsize = 0x60; // todo?
    
    ptr_handlingCFG = (_lh(addr+0x218) * 0x10000) + (int16_t)_lh(addr+0x21C);
    var_handlingcfgslots   = 80; // where to find?
    var_handlingcfgslotsize = 0xF0; // where to find?
    
    ptr_particleCFG = (_lh(addr+0x250) * 0x10000) + (int16_t)_lh(addr+0x254);
    var_particleCFGslots   = 82; // where to find?
    var_particleCFGslotsize = 0x94; // where to find?
    
    ptr_timecycDAT = (_lh(addr+0x230) * 0x10000) + (int16_t)_lh(addr+0x234);
  
    return 1;
  }
  
  /// carcols.dat (in DTZ but need to get global from other location)
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
  if(  _lw(addr + 0x24) == 0x2A64000B && _lw(addr - 0x3C) == 0x02A03025 ) { // 0x0000974C
    /*******************************************************************
     *  0x0000974C: 0x3C050033 '3..<' - lui        $a1, 0x33
     *  0x00009750: 0x8CA5FC50 'P...' - lw         $a1, -944($a1)
    *******************************************************************/
    ptr_carcolsDAT = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_carcolsDAT", ptr_carcolsDAT-text_addr, ptr_carcolsDAT); // DAT_0032fc50_PTR_carcols.dat
    #endif
    var_carcolsdatslots   = 128; //where to find? TODO
    var_carcolsdatslotsize   = 0x4; //where to find?
    
    return 1;
  }
  
  /// pedstat.dat (in DTZ but need to get globals from other location)
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
   if(  _lw(addr - 0x34) == 0x24A50088 && _lw(addr + 0x14) == 0x8C840000 ) { // 0x0017DBEC (inside FUN_0017db24)
    /*******************************************************************
     *  0x0017DBEC: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x0017DBF0: 0x8CA5A14C 'L...' - lw         $a1, -24244($a1)
    *******************************************************************/
    ptr_pedstatTable = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_pedstatTable", ptr_pedstatTable-text_addr, ptr_pedstatTable); // DAT_0035a14c_pedstats.dat
    #endif
    
    var_pedstatDATslots   = 42; // where to find? TODO
    var_pedstatDATslotsize = 0x34; // where to find?
    
    return 1;
  }
  
  /// IDEs (in DTZ but need to get globals from other location)
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
   if(  _lw(addr + 0x54) == 0x90850010 && _lw(addr + 0x6C) == 0x8CA50004 ) { // FUN_00224af4_IDE
    /*******************************************************************
     *  0x00224B00: 0x3C140036 '6..<' - lui        $s4, 0x36
     *  0x00224B18: 0xAE84A140 '@...' - sw         $a0, -24256($s4)
    *******************************************************************/
    ptr_IDEs = (_lh(addr+0xC) * 0x10000) + (int16_t)_lh(addr+0x24); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_IDEs", ptr_IDEs-text_addr, ptr_IDEs); // DAT_0035a140_VAR_IDEoffsets
    #endif
    
    
    /*******************************************************************
     *  0x00224B04: 0x3C130035 '5..<' - lui        $s3, 0x35
     *  0x00224B28: 0xAE6593BC '..e.' - sw         $a1, -27716($s3)
    *******************************************************************/
    ptr_IDETable = (_lh(addr+0x10) * 0x10000) + (int16_t)_lh(addr+0x34);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_IDETable", ptr_IDETable-text_addr, ptr_IDETable); //DAT_003493bc_PTR_IDETable
    #endif
    
    return 1;
  }
  
  
  /// world vehicle spawns
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
   if( _lw(addr + 0x40) == 0x01601825 && _lw(addr + 0x90) == 0x2442FFFF ) { // FUN_002cec38_checkSetVehicleWorldSpawn
    /*******************************************************************
     *  0x002CECA8: 0x3C020065 'e..<' - lui        $v0, 0x65
     *  0x002CECAC: 0x2442AAC0 '..B$' - addiu      $v0, $v0, -21824
    *******************************************************************/
    addr_vehiclesworldspawn = (_lh(addr+0x70) * 0x10000) + (int16_t)_lh(addr+0x74);
    
    /*******************************************************************
     *  0x002CEC70: 0x284300C3 '..C(' - slti       $v1, $v0, 195
    *******************************************************************/
    var_vehiclesworldspawnslots = (int16_t)_lh(addr+0x38);
    
    /*******************************************************************
     *    0x002CEC98: 0x00022100 '.!..' - sll        $a0, $v0, 4
     *    0x002CEC9C: 0x00821023 '#...' - subu       $v0, $a0, $v0
     *    0x002CECA0: 0x00021080 '....' - sll        $v0, $v0, 2
     *    0x002CECA4: 0x00442023 '# D.' - subu       $a0, $v0, $a0
    *******************************************************************/
    var_vehiclesworldspawnslotsize = 0x2C; //get from code is complicated here TODO
    
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_vehiclesworldspawn", addr_vehiclesworldspawn-text_addr, addr_vehiclesworldspawn); // 0x64aac0
    logPrintf("var_vehiclesworldspawnslots = 0x%08X", var_vehiclesworldspawnslots);
    logPrintf("var_vehiclesworldspawnslotsize = 0x%08X", var_vehiclesworldspawnslotsize); 
    #endif
    
    return 1;
  }
  
  
  /// for loadscreen TXDs replacement
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
   /* if( _lw(addr + 0x9C) == 0x26A5FFE0 && _lw(addr + 0xFC) == 0x00000000 ) { // FUN_001bd8dc_loadSplashScreen
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> loadSplashScreen()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, loadSplashScreen_patched, loadSplashScreen);
    return 1;
  } */

  /// loadStringFromGXT
  if(  _lw(addr + 0x44) == 0x92240022 && _lw(addr + 0x70) == 0x03E00008 ) { // FUN_0010fad4_loadStringFromGXT aka CTextGet
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> LoadStringFromGXT()", addr-text_addr, addr);
    #endif
    LoadStringFromGXT = (void*)(addr); // FUN_0010fad4_loadStringFromGXT
    HIJACK_FUNCTION(addr, LoadStringFromGXT_patched, LoadStringFromGXT);
    return 1;
  }
  if(  _lw(addr + 0x28) == 0x0040B025 && _lw(addr + 0x64) == 0x2408FFFF ) { // 0x00028EDC near "MP_SNEW"
    /*******************************************************************
     *  0x00028EDC: 0x3C140033 '3..<' - lui        $s4, 0x33
     *  0x00028EE0: 0x8E84207C '| ..' - lw         $a0, 8316($s4)
    *******************************************************************/
    ptr_gxtloadadr = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_gxtloadadr", ptr_gxtloadadr-text_addr, ptr_gxtloadadr); //DAT_0033207c_gxtLoadAdr
    #endif
    return 1;
  }
  
  /// radarIcon (for names)
  if(  _lw(addr + 0x20) == 0x00402025 && _lh(addr + 0x12) == 0x3C04 && _lh(addr + 0x4A) == 0x24A5 && _lw(addr + 0xC) == 0x00000000 ) { // FUN_00162bac_loadRadarIcons
    /*******************************************************************
     *  0x00162BD0: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x00162BD8: 0x2484A6C0 '...$' - addiu      $a0, $a0, -22848
    *******************************************************************/
    ptr_radarIconList = (_lh(addr+0x24) * 0x10000) + (int16_t)_lh(addr+0x2C); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_radarIconList", ptr_radarIconList-text_addr, ptr_radarIconList); // DAT_0035a6c0_radarIcon_0
    #endif
    return 1;
  }
  
  /// radio stations count
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 0x000B9164
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 0x000BBC70
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 | 
   * ULUX-80142 v0.02 | 0x0010B180
   **************************************/ 
  if(  _lh(addr - 0x4) == 0xFF && _lh(addr + 0x10) == 0xFF && _lw(addr + 0x24) == 0x00000000 && (_lw(addr) == _lw(addr + 0x14)) ) { // 0x000B9164
    /*******************************************************************
     *  0x000B9164: 0x2A44000B '..D*' - slti       $a0, $s2, 11
    *******************************************************************/
    var_radios = (int16_t)_lh(addr);
    var_radios -= 1;
    #ifdef PATCHLOG
    logPrintf("var_radios = 0x%08X", var_radios);
    #endif
    return 1;
  }
  
  /// render global (used for cWorldStreamRender)
  if(  _lw(addr + 0x20) == 0x34050002 && _lh(addr + 0x2A) == 0x1480 && _lw(addr - 0x34) == 0x00000000 && _lw(addr + 0x44) == 0x00000000  ) { // 0x001BEA9C
    /*******************************************************************
     *  0x001BEA9C: 0x3C110035 '5..<' - lui        $s1, 0x35
     *  0x001BEAA0: 0x8E247144 'Dq$.' - lw         $a0, 28996($s1)
    *******************************************************************/
    render = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> render", render-text_addr, render); //DAT_00357144_render
    #endif
    return 1;
  }
  
  
  /// savedata stuff (scraped since not working on emu and save space)
  /*************************************
   * ULUS-10041 v1.05 | OK!
   * ULUS-10041 v3.00 | OK!
   * ULES-00182 v1.00 | OK! own version below
   * ULES-00182 v2.00 | OK! own version below
   * ULES-00151 v1.05 | OK!
   * ULES-00151 v2.00 | OK!
   * ULES-00151 v3.00 | OK!
   * ULES-00151 v4.00 | OK!
   * ULJM-05255 v1.01 |
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 | 
   * ULUX-80142 v0.02 | OK! (doesn't use a key)
   **************************************/ 
  /* if( _lh(addr + 0x2C) == 0x004C  && _lh(addr - 0x8) == 0x0007  && _lh(addr - 0x1C) == 0x0005) { // 0xAEDD8
    
    if( _lh(addr + 0x98) == 0x0010 ) { // decryption key for later / patched Versions 
      *******************************************************************
       *  0x000AEE54: 0x3C050033 '3..<' - lui        $a1, 0x33
       *  0x000AEE78: 0x24A5017C '|..$' - addiu      $a1, $a1, 380
      *******************************************************************
      savedatakey = (_lh(addr+0x7C) * 0x10000) + (int16_t)_lh(addr+0xA0); 
      #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> savedatakey (later)", savedatakey-text_addr, savedatakey); // DAT_0033017c_SAVEKEY
      #endif
    }
    if( _lh(addr + 0x9C) == 0x0010 ) { // decryption key for initial versions (like US v1.05)
      *******************************************************************
       *  0x000AEEE8: 0x3C050033 '3..<' - lui        $a1, 0x33
       *  0x000AEF10: 0x24A5007C '|..$' - addiu      $a1, $a1, 124
      *******************************************************************
      savedatakey = (_lh(addr+0x7C) * 0x10000) + (int16_t)_lh(addr+0xA4); 
      #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> savedatakey (initial)", savedatakey-text_addr, savedatakey); 
      #endif
    }
    
    *******************************************************************
     *  0x000AEDD8: 0x3C040031 '1..<' - lui        $a0, 0x31
     *  0x000AEDDC: 0x2493DB7C '|..$' - addiu      $s3, $a0, -9348
    *******************************************************************
    titleid = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> titleid", titleid-text_addr, titleid); //s_ULUS10041_0030db7c
    #endif
    
    saveprefix = titleid + 0xC;
    
    return 1;
  }
  if( _lh(addr + 0x34) == 0x004C  && _lh(addr + 0x4) == 0x0007  && _lh(addr - 0x14) == 0x0005 ) { // German Version is too different  0x000AEE14
    
    if( _lh(addr + 0xA4) == 0x0010 ) { //decryption key for GER v1.00
      *******************************************************************
       *  0x000AEE98: 0x3C050033 '3..<' - lui        $a1, 0x33
       *  0x000AEEC0: 0x24A5003C '<..$' - addiu      $a1, $a1, 60
      *******************************************************************
      savedatakey = (_lh(addr+0x84) * 0x10000) + (int16_t)_lh(addr+0xAC); // 0x0033003C
      #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> savedatakey (GERMAN v1.00)", savedatakey-text_addr, savedatakey); 
      #endif
    }
    if( _lh(addr + 0xA0) == 0x0010 ) { //decryption key for GER v2.00
      *******************************************************************
       *  0x000AEE98: 0x3C050033 '3..<' - lui        $a1, 0x33
       *  0x000AEEC0: 0x24A5003C '<..$' - addiu      $a1, $a1, 60
      *******************************************************************
      savedatakey = (_lh(addr+0x84) * 0x10000) + (int16_t)_lh(addr+0xA8); // 0x0033003C
      #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> savedatakey (GERMAN v2.00)", savedatakey-text_addr, savedatakey); 
      #endif
    }
    
    *******************************************************************
     *  0x000AEE14: 0x3C040031 '1..<' - lui        $a0, 0x31
     *  0x000AEE1C: 0x2494DBBC '...$' - addiu      $s4, $a0, -9284
    *******************************************************************
    titleid = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); // 0x0030DBBC "ULES00182" v1.00 (0x30DAFC v2.00)
    
    #ifdef PATCHLOG 
    logPrintf("0x%08X (0x%08X) -> titleid (GERMAN)", titleid-text_addr, titleid); 
    #endif
    
    saveprefix = titleid + 0xC; //"S"
    
    return 1;
  } */

  
  /// global_developerflag (read from multiplayer menu function)
  if(  _lw(addr + 0x30) == 0x340800A0 && _lw(addr - 0xC) == 0x28440002 ) { // 0x0002E0A0
    /*******************************************************************
     *  0x0002E0A0: 0x3C040033 '3..<' - lui        $a0, 0x33
     *  0x0002E0A4: 0x9084010E '....' - lbu        $a0, 270($a0)
    *******************************************************************/
    global_developerflag = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_developerflag", global_developerflag-text_addr, global_developerflag); // DAT_0033010e_developerFlag
    #endif

    return 1;
  }
  
  /// global_freezetimers
  if( _lw(addr + 0x14) == 0x2406FFFE && _lw(addr + 0x34) == 0x02402025 /*_lw(addr + 0x14) == 0x2406FFFE && _lw(addr - 0x30) == 0x34050004*/ ) { // 0x00154400
    /*******************************************************************
     *  0x00154400: 0x3C040039 '9..<' - lui        $a0, 0x39
     *  0x00154404: 0x2484E388 '...$' - addiu      $a0, $a0, -7288
     *  0x00154408: 0xA080014D 'M...' - sb         $zr, 333($a0)
    *******************************************************************/
    global_freezetimers = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4) + (int16_t)_lh(addr+0x8); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_freezetimers", global_freezetimers-text_addr, global_freezetimers); // DAT_0038e4d5_freezeOnScreenTimer
    #endif
    return 1;
  }

  
  /// ped task function
  if( _lw(addr + 0x30) == 0x34050037  && _lw(addr + 0x4C) == 0x8E040254 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TaskCharWith()", addr-text_addr, addr);
    #endif
    
    /******************************
    * 0x8 = kill ped on foot
    * 0x9 = kill ped by any means
    * 0xA = flee ped on foot till safe 
    * 0xB = avoid ped on foot always
    * 0xC = go to ped on foot
    * 0xD = go to ped
    * 
    * 0x10 = leave vehicle
    * 0x11 = enter car as passenger
    * 0x12 = enter car as driver
    * 0x15 = destroy object
    * 0x16 = destroy vehicle
    * 
    * 0x1D = set leader
    * 
    * 0x29 = leave vehicle and flee
    * 0x2E = aim gun at ped
    * 0x32 = 
    */
    TaskCharWith = (void*)(addr);  // 0x8f0f0
    
    /******************************************************************************
      this function only works within apply not hooked func for drawing!!!
      AND from inside another void returning function!!! WTF!
    *******************************************************************************/

    return 1;
  }
  
  /// warp ped to vehicle
  if( _lw(addr + 0x10) == 0x34120012  && _lw(addr + 0x3C) == 0x34040001 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> WarpPedIntoVehicle()", addr-text_addr, addr);
    #endif
    WarpPedIntoVehicle = (void*)(addr); // 0x1b90ec
    return 1;
  }
  /// warp ped to vehicle as passenger
  if( _lw(addr + 0x14) == 0x34080012  && _lw(addr + 0x3C) == 0x34040001 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> WarpPedIntoVehicleAsPassenger()", addr-text_addr, addr);
    #endif
    WarpPedIntoVehicleAsPassenger = (void*)(addr); // 0x1b9470
    return 1;
  }
  
  /// request model
  if( _lw(addr + 0x14) == 0x28941324  && _lw(addr + 0x34) == 0x00102100 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> RequestModel()", addr-text_addr, addr);
    #endif
    RequestModel = (void*)(addr); // 0x1c6c28
    return 1;
  }
  /// GiveWeaponAndAmmo
  if( _lw(addr + 0x6C) == 0x269205A0  && _lw(addr + 0x38) == 0x00132140 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> GiveWeaponAndAmmo()", addr-text_addr, addr);
    #endif
    GiveWeaponAndAmmo = (void*)(addr); // 0x19b2f4
    return 1;
  }
  
  /// SetActorSkinTo (needs name to be lower case!)
  if( _lw(addr + 0x28) == 0x340500A1  && _lw(addr + 0x44) == 0x24840040 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetActorSkinTo()", addr-text_addr, addr);
    #endif
    SetActorSkinTo = (void*)(addr);  // 0x001a1174
    return 1;
  }
  /// LoadAllModelsNow
  if( (_lw(addr + 0x4) == 0x308400FF  && _lw(addr + 0x50) == 0x3C100002) /* || ULUX (func params diff) --> (_lw(addr + 0x4) == 0x308500FF  && _lw(addr + 0x44) == 0x0016B080) */ ) { // FUN_001c6f64_LoadAllRequestedModels
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> LoadAllModelsNow()", addr-text_addr, addr);
    #endif
    LoadAllModelsNow = (void*)(addr); // 0x001c6f64
    return 1;
  }
  /// RefreshActorSkin
  if( _lw(addr + 0x38) == 0xAE04034C  && _lw(addr + 0x10) == 0x2404FFFF ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> RefreshActorSkin()", addr-text_addr, addr);
    #endif
    RefreshActorSkin = (void*)(addr); // 0x001a1210
    return 1;
  }
  
  /// TaskDuckLCS
  if( _lw(addr + 0x18) == 0x34050099  && _lw(addr + 0x70) == 0x34840010 ) { // FUN_000835fc_taskDuck
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TaskDuckLCS()", addr-text_addr, addr);
    #endif
    TaskDuckLCS = (void*)(addr); // 0x000835fc
    return 1;
  }
  /// TaskUnDuck
  if( _lw(addr + 0x20) == 0x34050099  && _lw(addr + 0x64) == 0x3C06C080 ) { // FUN_000837d0_unduck
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TaskUnDuck()", addr-text_addr, addr);
    #endif
    TaskUnDuck = (void*)(addr); // 0x000837d0
    return 1;
  }
  
  
  /// for loadscreen debug text
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | 0x001BDBF8 | OK!
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | func too different for my patch
   **************************************/ 
   if( _lw(addr + 0x34) == 0x3C053E80 && _lw(addr + 0x18) == 0x3C044040 ) { // FUN_001bdbf8_DrawLoadingBar 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> DrawLoadingBar()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, DrawLoadingBar_patched, DrawLoadingBar); // ULUX OK!
    return 1;
  } 
  if( _lh(addr + 0x18) == 0x00FF && _lw(addr + 0x14) == 0x00C09825 ) { // FUN_001bde98_DrawLoadscreen 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> DrawLoadscreen()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, Loadscreen_patched, Loadscreen);
    return 1;
  }


  
  /// addr_heliheight
  if( _lh(addr) == 0x42A0 && _lh(addr +0x20) == 0x428C && _lw(addr + 0x14) == 0x00000000 ) {  // 0x0009F778
    /*******************************************************************
     *  0x0009F778: 0x3C0442A0 '.B.<' - lui        $a0, 0x42A0 (default 80.0f)
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_heliheight", addr-text_addr, addr);
    #endif
    addr_heliheight = addr; // 0x0009F778
    return 1;
  } 
  
  
  /// StartNewScript
  if( _lw(addr - 0xC) == 0x2402FFFF  && _lw(addr + 0x30) == 0x24C70001 && _lw(addr + 0x58) == 0x34040001 ) { // FUN_015415c
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> StartNewScript()", addr-text_addr, addr);
    #endif
    StartNewScript = (void*)(addr); // 0x15415c
    return 1;
  }
  /// globals: ScriptSpace & MainScriptSize
  if( _lw(addr - 0x34) == 0x26100008 && _lw(addr + 0x28) == 0x34040001 ) { // 0x000E0FA0
    /*******************************************************************
     *  0x000E0FA0: 0x3C100033 '3..<' - lui        $s0, 0x33
     *  0x000E0FA4: 0x8E054D7C '|M..' - lw         $a1, 19836($s0)
    *******************************************************************/
    global_ScriptSpace = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_ScriptSpace", global_ScriptSpace-text_addr, global_ScriptSpace); // DAT_00334d7c_ScriptSpace
    #endif
    
    /*******************************************************************
     *  0x000E0FA8: 0x3C110036 '6..<' - lui        $s1, 0x36
     *  0x000E0FAC: 0x8E27A4BC '..'.' - lw         $a3, -23364($s1)
    *******************************************************************/
    global_MainScriptSize = (_lh(addr+0x8) * 0x10000) + (int16_t)_lh(addr+0xC); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_MainScriptSize", global_MainScriptSize-text_addr, global_MainScriptSize); // DAT_0035a4bc_MainScriptSize
    #endif

    /// TODO ?
  if( mod_text_size == 0x0031F854 )
    global_LargestMissionScriptSize = global_MainScriptSize - 0x4;  // ULUX
  else
    global_LargestMissionScriptSize = global_MainScriptSize - 0x18; 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_LargestMissionScriptSize", global_LargestMissionScriptSize-text_addr, global_LargestMissionScriptSize); // DAT_0035a4a4_LargestMissionScriptSize
    #endif

    return 1;
  } // ULUX OK


  /// TankControl
  if( _lw(addr + 0x50) == 0x3C04BFB2  && _lw(addr + 0x78) == 0x3C04403C ) { // FUN_0001c7f8
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TankControl()", addr-text_addr, addr);
    #endif
    TankControl = (void*)(addr); //
    return 1;
  }
  /// BlowupVehiclesInPath
  if( _lw(addr + 0x38) == 0x3C053DCC  && _lw(addr + 0x58) == 0x30840004 ) { // FUN_00008900
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> BlowupVehiclesInPath()", addr-text_addr, addr);
    #endif
    BlowupVehiclesInPath = (void*)(addr); //
    return 1;
  }
  
  
  /// areas: multiplayer
  if(  _lw(addr + 0x10) == 0x2C850007 && _lw(addr + 0x1C) == 0x00042080 && _lw(addr - 0xC) == 0x00000000 ) { // 0x00171100
    /*******************************************************************
     *  0x00171100: 0x3C040038 '8..<' - lui        $a0, 0x38
     *  0x00171108: 0x2484D768 'h..$' - addiu      $a0, $a0, -10392
    *******************************************************************/
    global_mp_parameters = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_mp_parameters", global_mp_parameters-text_addr, global_mp_parameters); // DAT_0037d768
    #endif

    /*******************************************************************
     *  0x00171134: 0x3C040035 '5..<' - lui        $a0, 0x35
     *  0x00171138: 0x8C846DF8 '.m..' - lw         $a0, 28152($a0)
    *******************************************************************/
    global_mp_1 = (_lh(addr+0x34) * 0x10000) + (int16_t)_lh(addr+0x38); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_mp_1", global_mp_1-text_addr, global_mp_1); // DAT_00356df8
    #endif
    
    
    return 1;
  }
  
  

  /// IsPlayerOnAMission
  if( _lw(addr + 0x20) == 0x14860003 && _lw(addr + 0x2C) == 0x34020001  ) { // FUN_001541f0 _CTheScripts_IsPlayerOnAMission
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> IsPlayerOnAMission()", addr-text_addr, addr);
    #endif
    IsPlayerOnAMission = (void*)(addr); //
  
    /*******************************************************************
     *  0x001541F0: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x001541F4: 0x8C84A48C '....' - lw         $a0, -23412($a0)
    *******************************************************************/
    global_OnAMissionFlag = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_OnAMissionFlag", global_OnAMissionFlag-text_addr, global_OnAMissionFlag); // DAT_0035a48c_OnAMissionFlag
    #endif
    
    return 1;
  }
  
  /// SetBridgeState
  if( _lw(addr + 0x3C) == 0x3C06C3A5 && _lw(addr + 0x4) == 0x308400FF ) { // FUN_001615e4_SetBridgeState
    #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> SetBridgeState()", addr-text_addr, addr);
    #endif
    SetBridgeState = (void*)(addr); //
  
    /*******************************************************************
     *  0x001615EC: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x001615F8: 0xACA4A69C '....' - sw         $a0, -22884($a1)
    *******************************************************************/
    global_bridgeState = (_lh(addr+0x8) * 0x10000) + (int16_t)_lh(addr+0x14); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_bridgeState", global_bridgeState-text_addr, global_bridgeState); // DAT_0035a69c_bridgeState
    #endif
    
    return 1;
  }
  
  
  /// globals VehicleName
  if( _lw(addr - 0x14) == 0x34040004 && _lw(addr + 0x28) == 0x28850003 ) { // 0x00187400 (inside Draw())

    /*******************************************************************
     *  0x00187400: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x00187404: 0x8C84A81C '....' - lw         $a0, -22500($a0)
    *******************************************************************/
    global_m_pVehicleName = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_m_pVehicleName", global_m_pVehicleName-text_addr, global_m_pVehicleName); // DAT_0035a81c_m_pVehicleName
    #endif

    global_m_pVehicleNameToPrint = global_m_pVehicleName + 0x10; 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_m_pVehicleNameToPrint", global_m_pVehicleNameToPrint-text_addr, global_m_pVehicleNameToPrint); // DAT_0035a82c_m_pVehicleNameToPrint
    #endif
    
    return 1;
  }
  
  /// CalculateNewVelocity
  if( _lw(addr + 0x4) == 0x8C850194  && _lw(addr + 0x10) == 0x30A40001 ) { // FUN_001a88c4_UpdatePosition
  //if( _lw(addr + 0x60) == 0x3C044334  && _lw(addr + 0x2C) == 0x00808025 ) { // FUN_001a8368_CalculateNewVelocity
    #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> UpdatePosition()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, UpdatePosition_patched, UpdatePosition); //
    return 1;
  }
  
  
  /// weapon.dat (in DTZ but need to get globals from other location)
  /*************************************
   * ULUS-10041 v1.05 | 
   * ULUS-10041 v3.00 | ok
   * ULES-00182 v1.00 | 
   * ULES-00182 v2.00 | 
   * ULES-00151 v1.05 | 
   * ULES-00151 v2.00 | 
   * ULES-00151 v3.00 | 
   * ULES-00151 v4.00 | 
   * ULJM-05255 v1.01 | 
   * ULET-00361 v0.02 | 
   * ULET-00362 v0.01 |
   * ULUX-80142 v0.02 | 
   **************************************/ 
   if( _lw(addr + 0x18) == 0x10E00004 && _lw(addr + 0x4) == 0x000431C0 ) {  // FUN_0014997c
    /*******************************************************************
     *  0x0014998C: 0x3C050036 '6..<' - lui        $a1, 0x36
     *  0x00149990: 0x8CA5A660 '`...' - lw         $a1, -22944($a1)
    *******************************************************************/
    ptr_weaponTable = (_lh(addr+0x10) * 0x10000) + (int16_t)_lh(addr+0x14);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_weaponTable", ptr_weaponTable-text_addr, ptr_weaponTable); // DAT_0035a660_weapon.dat
    #endif
    
    var_weaponDATslots   = 37; // where to find? TODO
    var_weaponDATslotsize = 0x70;
    
    return 1;
  } 
  
  
  /// DoHoverSuspensionRatios
  if( _lw(addr + 0x60) == 0x84A40058  && _lw(addr + 0xB8) == 0x34160000 ) { // FUN_00009138
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> DoHoverSuspensionRatios()", addr-text_addr, addr);
    #endif
    DoHoverSuspensionRatios = (void*)(addr); //
    return 1;
  }
  
  #ifdef PREVIEW
  /// wind
  if( _lw(addr + 0x34) == 0x2A440015 && _lw(addr + 0x48) == 0x2A440014 ) { // 0x001307FC
  /*******************************************************************
     *  0x001307FC: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x0013081C: 0xE490A1F8 '....' - swc1       $fpr16, -24072($a0)
    *******************************************************************/
    global_Wind = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x20); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_Wind", global_Wind-text_addr, global_Wind); // DAT_0035a1f8_Wind
    #endif
  
    /*******************************************************************
     *  0x00130828: 0x3C040036 '6..<' - lui        $a0, 0x36
     *  0x0013082C: 0xE491A570 'p...' - swc1       $fpr17, -23184($a0)
    *******************************************************************/
    global_WindClipped = (_lh(addr+0x2C) * 0x10000) + (int16_t)_lh(addr+0x30); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_WindClipped", global_WindClipped-text_addr, global_WindClipped); // DAT_0035a570_WindClipped
    #endif
    /// nop out to stop game from setting it on its own (we take care of it in the cheat function)
  _sw(0x00000000, addr + 0x2c);
  _sw(0x00000000, addr + 0x30);
  
    return 1;
  }
  #endif
  
  /// addr_randompedcheat
  if( _lw(addr - 0x14) == 0x2A04006D ) {  // 0x00290928
    /*******************************************************************
     *  0x00290928: 0x1211FFDF '....' - beq        $s0, $s1, loc_002908A8
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_randompedcheat", addr-text_addr, addr);
    #endif
    addr_randompedcheat = addr; // 0
    return 1;
  } 
  
  #ifdef PREVIEW
  /// swimming
  if( _lw(addr + 0x18) == 0x3C04C5BB && _lw(addr + 0x5C) == 0x3C04C5BB ) { // FUN_000e7d70_CCam_IsTargetInWater 
    HIJACK_FUNCTION(addr, FUN_000e7d70_CCam_IsTargetInWater_patched, FUN_000e7d70_CCam_IsTargetInWater); // MAKE_DUMMY_FUNCTION(text_addr + 0xe7d70, 0);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> CCam_IsTargetInWater", addr-text_addr, addr); // 
    #endif
    return 1;
  }
  if( _lw(addr + 0x0) == 0x3C064500 && _lw(addr + 0x2C) == 0x340A0080 ) { // FUN_00109dac_CWaterLevel_GetWaterLevel
    FUN_00109dac_CWaterLevel_GetWaterLevel = (void*)(addr); // needs to be called in "ProcessBuoyancy"
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> CWaterLevel_GetWaterLevel", addr-text_addr, addr); // 
    #endif
    return 1;
  }
  if( _lw(addr + 0x4) == 0x3C063F8C && _lw(addr + 0x44) == 0x34050037 ) { // FUN_001a8d9c_CPed_ProcessBuoyancy
    HIJACK_FUNCTION(addr, FUN_001a8d9c_CPed_ProcessBuoyancy_patched, FUN_001a8d9c_CPed_ProcessBuoyancy);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> CPed_ProcessBuoyancy", addr-text_addr, addr); // 
    #endif
    return 1;
  }
  #endif
  
  return 0;
}


/*
void (*FUN_test)();
void FUN_test_patched() {
  
  /// TEXT
  u32 textcolor = WHITE;
  ResetFontStyling();
  SetFontStyle(1);
  SetColor(&textcolor);
  //SetScale_VCS(0.6f); // crashes when fontscale[i] and with different scales
  SetTextOriginPoint(0x0); // top left  
  wchar_t wbuf[128];
  AsciiToUnicode("Hello World! Lorem Ipsum", wbuf);
  PrintString_VCS(wbuf, (int)25.0f, (int)80.0f);
  
  
  /// BOX
  u32 color = CHDVC_MAGENTA;
  float coordinates[4];
  float *ptr_coords = (float *)SetBoxCoords(50.0f, 20.0f, 400.0f, 100.0f, coordinates);
  DrawRect(ptr_coords, &color, 0x1);

  /// TEXT 2
  u32 textcolor2 = CHDVC_ORANGE;
  ResetFontStyling();
  SetFontStyle(1);
  SetColor(&textcolor2);
  //SetScale_VCS(0.6f); // crashes when fontscale[i] and with different scales
  SetTextOriginPoint(0x0); // top left  
  wchar_t wbuf2[128];
  AsciiToUnicode("osidjv pijdf iojhs0iopsf", wbuf2);
  PrintString_VCS(wbuf2, (int)25.0f, (int)40.0f);
  
  /// BOX 2
  u32 color2 = WHITE;
  float coordinates2[4];
  float *ptr_coords2 = (float *)SetBoxCoords(100.0f, 50.0f, 350.0f, 130.0f, coordinates2);
  DrawRect(ptr_coords2, &color2, 0x1);
  
  FUN_test();
} */
   

void (*testfuncv)(int a, int b); 
void testfuncv_patched(int a, int b) {
  //char buffer[256]; 
  //snprintf(buffer, sizeof(buffer), "z = %.2f, xstick = %.2f, ystick = %.2f", test[0], xstick, ystick);
  //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 20.0f, RED);
      
  logPrintf("a: 0x%08X, b: 0x%08X", a, b);
}

int PatchVCS(u32 addr, u32 text_addr) { // Vice City Stories
  #ifdef PREVIEW
  if( VCS && patchonce ) { // FOR TESTING ONLY
    /// fullscreen loading bar size (Like LCS) ///////
    // setByte(text_addr + 0x13168c + 0x00, 0xD2 );
    // setByte(text_addr + 0x13168c + 0x01, 0x43 );
    // setByte(text_addr + 0x131620 + 0x00, 0xE1 );
    // setByte(text_addr + 0x13158c + 0x00, 0xE1 );
    // setByte(text_addr + 0x1314f0 + 0x00, 0xE3 );
    
    /// deactivate vehicle speed motion blur
    //MAKE_DUMMY_FUNCTION(text_addr + 0x021a6c0, 0); 
    
    /// change Police-heli (0x113 = Hunter, 0 = disabled)
    //setShort(text_addr + 0x02D4100, 0x113); // request model - addr_policechaseheli_1
    //setShort(text_addr + 0x01ed254, 0x113); // check model
    //setShort(text_addr + 0x01ed68c, 0x113); // call constructor
    
    /// drive on water
    //FUN_00009138_CAutomobile_DoHoverSuspensionRatios = (void*)(text_addr + 0x2f358);  //drive on water (without hover wheels!) -- call with pcar obj


  // // Samples // // // // // // // // // // // // // // // // // // // // // // // // //  
    
    //FUN_002cf610 = (void*)(text_addr + 0x2cf610); 
    //testfunc3 = (void*)(text_addr + 0x1a71f0); 
    
    
    //testfunc = (void*)(text_addr + 0x131490); 
    //TankControl = (void*)(text_addr + 0x02eb18); 
    //HIJACK_FUNCTION(text_addr + 0x115cd8, testfunc_patched, testfunc);
      
    //_sw(0x00000000, addr_neverFallOffBike_rollback);
    
    //_sw(0x460d603e, text_addr + 0x1674c0); // c.le.s f12,f13
    //clearICacheFor(text_addr + 0x1674c0);
    
    
  // // Test Area // // // // // // // // // // // // // // // // // // // // // // // // //
    
    //MAKE_DUMMY_FUNCTION(text_addr + 0x08c5f0, 0);
    
     
    
  // // // // // // // // // // // // // // // // // // // // // // // // // // //  
    
    #ifdef PATCHLOG
    logPrintf("[INFO] patchonce() VCS ran!");
    #endif
    patchonce = 0;
  }
  #endif


  /// /// /// ULTRA CRITICAL ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  /// 
  
  /// cWorldStream_Render
  /*************************************
   * ULUS-10160 v1.01 | 
   * ULUS-10160 v1.02 | 
   * ULUS-10160 v1.03 | 0x00154D28
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 0x0014DCBC
   * ULET-00417 v0.07 | 
   **************************************/ 
  if( _lw(addr + 0x84) == 0x3C043F80 && _lw(addr + 0x70) == 0x3C043D4C  ) { 
    #ifdef PATCHLOG
    logPrintf("[0] 0x%08X (0x%08X) --> cWorldStream_Render()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, cWorldStream_Render_Patched, cWorldStream_Render); // 0x00154D28
    return 1;
  }
  
  /// for stopMenu(); [in pause menu]
  /*************************************
   * ULUS-10160 v1.03 | 0x002C22A0 | OK!
   * ULES-00502 v1.02 |  | 
   * ULES-00503 v1.02 |  | 
   * ULJM-05297 v1.01 |  | 
   * ULET-00417 v0.06 | 0x0031C168 | OK!
   **************************************/ 
  if( _lw(addr + 0x8) == 0x00808025  && _lw(addr + 0x1C) == 0x00000000 && _lw(addr + 0x3C) == 0x308400FF ) {
    #ifdef PATCHLOG
    logPrintf("[0] 0x%08X (0x%08X) --> FUN_002c22a0()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, FUN_002c22a0_patched, FUN_002c22a0); //
    return 1;
  }
  
  /// disable Button Input --> same as LCS
  /*************************************
   * ULUS-10160 v1.03 | 0x0018A288 | OK!
   * ULES-00502 v1.02 |  | 
   * ULES-00503 v1.02 |  | 
   * ULJM-05297 v1.01 |  | 
   * ULET-00417 v0.06 | 0x0018A8D8 | OK!
   **************************************/ 
  if( _lw(addr + 0x8) == 0x00808025 && _lw(addr + 0x14) == 0x26050034 ) {
    #ifdef PATCHLOG
    logPrintf("[0] 0x%08X (0x%08X) --> buttonsToAction()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, buttonsToActionPatched, buttonsToAction); // 0x0018A288
    return 1;
  }
  
  /// for FPS calculation
  /*************************************
   * ULUS-10160 v1.03 | 0x002030D4 | OK!
   * ULES-00502 v1.02 |  | 
   * ULES-00503 v1.02 |  | 
   * ULJM-05297 v1.01 |  | 
   * ULET-00417 v0.06 | 0x001FFE00 | OK!
   **************************************/ 
//if( _lw(addr - 0xC) == 0x1000FFF7 && _lw(addr + 0x4) == 0x00000000 && _lw(addr + 0x8) == 0x27A60010 && _lw(addr + 0x10) == 0x00602825 && _lw(addr + 0x1C) == 0x8FA40014 ) { // 0x002030D4
  if( _lw(addr + 0x6C) == 0x34040006 && _lw(addr - 0x20) == 0x2C840002 ) { // 0x002030D4
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_fpsCap", addr-0x20-text_addr, addr-0x20);
    #endif
    addr_fpsCap = addr-0x20; // 0x002030B4
    
    #ifdef PATCHLOG
    logPrintf("[0] 0x%08X (0x%08X) --> sceKernelGetSystemTimeWide()", addr-text_addr, addr);
    #endif
    MAKE_CALL(addr, sceKernelGetSystemTimeWidePatched); // 0x002030D4
    
    return 1;
  }
  
  
  /// /// /// CRITICAL ///  ///  ///  ///  ///  (checked in sceKernelGetSystemTimeWidePatched) ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  /// 
  
  /// pplayer function
  /*************************************
   * ULUS-10160 v1.03 | 0x0015C424 | OK!
   * ULES-00502 v1.02 |  | 
   * ULES-00503 v1.02 |  | 
   * ULJM-05297 v1.01 |  | 
   * ULET-00417 v0.06 | 0x00153DD8 | OK!
   * ULUS-10160 v1.01 | 0x00144a10 | OK!
   **************************************/ 
  if( _lw(addr + 0x8) == 0x00043200 && _lw(addr + 0x18) == 0x00C42021 && _lw(addr + 0x28) == 0x8C820000 ) {  // 0x0015c424
    #ifdef PATCHLOG
    logPrintf("[1] 0x%08X (0x%08X) --> GetPPLAYER()", addr-text_addr, addr);
    #endif
    GetPPLAYER = (void*)(addr); // get pplayer 
    return 1;
  }
  
  /// pcar function
  /*************************************
   * ULUS-10160 v1.03 | 0x0015C2C8 | OK!
   * ULES-00502 v1.02 | 0x0015C600 | OK!
   * ULES-00503 v1.02 | 0x0015C4C8 | OK!
   * ULJM-05297 v1.01 |  | 
   * ULET-00417 v0.06 | 0x00153C7C | OK!
   * ULUS-10160 v1.01 | 0x00144920 | OK!
   **************************************/ 
  if( (_lw(addr + 0x1C) == 0x00042140 && _lw(addr + 0x8) == 0x00842821 && _lw(addr + 0x38) == 0x00000000) ||  // 0x0015C2C8
      (_lw(addr + 0x10) == 0x00042140 && _lw(addr + 0x4) == 0x00842821 && _lw(addr - 0xC) == 0x24820140) ) {  // ULUS v1.01
    #ifdef PATCHLOG
    logPrintf("[1] 0x%08X (0x%08X) --> GetPCAR()", addr-text_addr, addr);
    #endif
    GetPCAR = (void*)(addr); // get pcar 
    return 1;
  }
  
  /// global gametimer
  /*************************************
   * ULUS-10160 v1.03 | 0x00026BB8 | OK!
   * ULES-00502 v1.02 |  | 
   * ULES-00503 v1.02 |  | 
   * ULJM-05297 v1.01 |  | 
   * ULET-00417 v0.06 | 0x0002F9AC | OK!
   **************************************/ 
  if( _lw(addr + 0xC) == 0x00A6202B && _lw(addr - 0x20) == 0x03E00008 && _lw(addr + 0x50) == 0x00000000 ) { 
    /*******************************************************************
     *  0x00026BB8: 0x8F851DEC '....' - lw         $a1, 7660($gp)
    *******************************************************************/
    global_gametimer = (int16_t) _lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("[1] 0x%08X --> global_gametimer", global_gametimer); // 0x1DEC
    #endif
    
    global_timescale = global_gametimer + 0xC;  
    return 1;
  }
  
  /// global_currentisland & global_systemlanguage [this function sets lots of globals -> weather, clock, cheatsused, gamespeed]
  /*************************************
   * ULUS-10160 v1.03 |  | 
   * ULES-00502 v1.02 |  | 
   * ULES-00503 v1.02 |  | 
   * ULJM-05297 v1.01 |  | 
   * ULET-00417 v0.06 |  |
   **************************************/ 
  if( _lw(addr + 0x4c) == 0xAE040014  &&  _lw(addr + 0x1A8) == 0xA2040074 ) { 
    /*******************************************************************
     *  0x002B2660: 0x8F851DE4 '....' - lw         $a1, 7652($gp)
    *******************************************************************/
    global_currentisland = (int16_t) _lh(addr+0xC); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("[1] 0x%08X --> global_currentisland", global_currentisland); // uGp00001de4
    #endif
    
    /*******************************************************************
     *  0x002B2694: 0x8F841EBC '....' - lw         $a0, 7868($gp)
    *******************************************************************/
    global_systemlanguage = (int16_t) _lh(addr+0x40); // WITHOUT GP!!
    #ifdef PATCHLOG
      logPrintf("[1] 0x%08X --> global_systemlanguage", global_systemlanguage); // uGp00001EBC
    #endif
    
    return 1;
  }
  
  /// globals pointers to objects
  if( _lw(addr + 0x88) == 0x340501DB && _lw(addr + 0x5C) == 0x34050028 && _lw(addr + 0x30) == 0x34050046 ) { 
    /*******************************************************************
     *  0x0018C1C8: 0xAF90C198 '....' - sw         $s0, -15976($gp)
    *******************************************************************/
    ptr_pedestriansobj = (int16_t)_lh(addr+0x38); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X --> ptr_pedestriansobj", ptr_pedestriansobj); // puGpffffc198
    #endif
    
    /*******************************************************************
     *  0x0018C1F4: 0xAF90C19C '....' - sw         $s0, -15972($gp)
    *******************************************************************/
    ptr_vehiclesobj = (int16_t)_lh(addr+0x64); //WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X --> ptr_vehiclesobj", ptr_vehiclesobj); // puGpffffc19c
    #endif
    
    /*******************************************************************
     *  0x0018C220: 0xAF90C1A8 '....' - sw         $s0, -15960($gp)
    *******************************************************************/
    ptr_worldobj =  (int16_t)_lh(addr+0x90); // WITHOUT GP!!
    #ifdef PATCHLOG
      logPrintf("0x%08X --> ptr_worldobj", ptr_worldobj); // puGpffffc1a8
    #endif
    
    /*******************************************************************
     *  0x0018C24C: 0xAF90C1AC '....' - sw         $s0, -15956($gp)
    *******************************************************************/
    ptr_businessobj =  (int16_t)_lh(addr+0xBC); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X --> ptr_businessobj", ptr_businessobj); // puGpffffc1ac
    #endif
    
    /*******************************************************************
     *  0x0018C278: 0xAF91C1B4 '....' - sw         $s1, -15948($gp)
    *******************************************************************/
    ptr_audioscriptobj = (int16_t)_lh(addr+0xE8); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X --> ptr_audioscriptobj", ptr_audioscriptobj); // puGpffffc1b4
    #endif
    
    var_bsnobjsize = 0x160; // from where? TODO
    
    return 1;
  }
  /// add-on objects' sizes)
  if( _lw(addr + 0xD4) == 0x2495FFFF && _lw(addr + 0xFC) == 0x02A4202A && _lw(addr + 0x190) == 0x02408825 ) { 
    /*******************************************************************
     * 0x0008F5C8: 0x24040D10 '...$' - li         $a0, 3344
    *******************************************************************/
    var_pedobjsize = (int16_t)_lh(addr+0x3C); // 0xD10
    #ifdef PATCHLOG
    logPrintf("var_pedobjsize = 0x%X", var_pedobjsize); //
    #endif
    
    /*******************************************************************
     * 0x0008F708: 0x2673F7E0 '..s&' - addiu      $s3, $s3, -2080
    *******************************************************************/
    var_vehobjsize = 0x10000 - _lh(addr+0x17C); // 0x820
    #ifdef PATCHLOG
    logPrintf("var_vehobjsize = 0x%X", var_vehobjsize); //
    #endif
    
    /*******************************************************************
     * 0x0008F7A0: 0x2694FDE0 '...&' - addiu      $s4, $s4, -544
    *******************************************************************/
    var_wldobjsize = 0x10000 - _lh(addr+0x214); // 0x220
    #ifdef PATCHLOG
    logPrintf("var_wldobjsize = 0x%X", var_wldobjsize); //
    #endif
    
    return 1;
  }
  
  
  
  /// /// /// FOR CHEATS ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  ///  /// 
  
  /// skip intro movies --> (JP version black screen after patch)
  if( _lw(addr - 0x30) == 0x34050003 && _lw(addr + 0x4) == 0x34040001 && _lw(addr + 0x8) == 0xAFBF0000 ) { // JP version black screen after patch (0x08804000 + 0x001C7DD4)
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_skipIntroMovie", addr-text_addr, addr);
    #endif
    addr_skipIntroMovie = addr; // 0x001309A8
    return 1;
  }
  
  /// never fall off bike.. when rolling backwards
  if( _lw(addr-0xC) == 0x3405002F && _lw(addr + 0x4) == 0x00004025 && _lw(addr - 0x28) == 0x3C04BF00 && _lw(addr - 0x68) == 0x00000000 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_neverFallOffBike_rollback", addr-text_addr, addr);
    #endif
    addr_neverFallOffBike_rollback = addr; // 0x000EBE8C - nop out function call
    return 1;
  }
  
  /// never fall off bike.. when hitting object
  if( _lw(addr - 0x54) == 0x00409025 && (_lw(addr + 0x1C) == 0x2404FFF6 || _lw(addr + 0x14) == 0x2404FFF6 ) ) { // 0x002614D4
    /*******************************************************************
     *  0x002614D4: 0x0C0C0345 'E...' - jal        sub_00300D14
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_neverFallOffBike_hitobj", addr-text_addr, addr);
    #endif
    addr_neverFallOffBike_hitobj = addr; // 0x002614D4 - nop out function call
    return 1;
  }
  
  
  /// world gravity
  if( _lw(addr - 0x10) == 0x30A50002 && _lw(addr) == 0x3C053C03 && _lw(addr + 0x4) == 0x34A5126F ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_worldgravity", addr-text_addr, addr);
    #endif
    addr_worldgravity = addr; // 0x00263690
    return 1;
  }
  
  /// set weather function
  if( _lw(addr - 0x10) == 0x03E00008 && _lw(addr - 0x8) == 0x03E00008 && _lw(addr+0x8) == 0x03E00008 && _lw(addr+0x10) == 0x2404FFFF && _lw(addr+0x14) == 0x03E00008 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetNextWeather(), SetWeatherNow(), ReleaseWeather()", addr-text_addr, addr);
    #endif
    SetNextWeather = (void*)(addr-0x8);  // 0x2F74E0
    SetWeatherNow  = (void*)(addr);      // 0x2F74E8
    ReleaseWeather = (void*)(addr+0x10); // 0x2F74F8
    /*******************************************************************
     *  0x002F74EC: 0xA7842098 '. ..' - sh         $a0, 8344($gp)
    *******************************************************************/
    global_weather = (int16_t)_lh(addr + 0x4); // WITHOUT GP!! (only to READ current weather)
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_weather", global_weather); //0x2098
    #endif
    return 1;
  }
  
  /// global settings toggle (special! there should be an address at gp+global_displaysettings -> addr there + 0x13 is toggle radar
  if( _lw(addr - 0x4c) == 0x34050001 &&  _lw(addr + 0x8) == 0x14800004 && _lw(addr) == _lw(addr+0x18) ) {  // 0x001B679C
    /*******************************************************************
     *  0x001B6798: 0x00000000 '....' - nop          
     *  0x001B679C: 0x8F8416F4 '....' - lw         $a0, 5876($gp)
     *  0x001B67A4: 0x14800004 '....' - bnez       $a0, loc_001B67B8
     *  0x001B67B0: 0x00000000 '....' - nop        
    *******************************************************************/
    global_displaysettings = (int16_t)_lh(addr); //WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_displaysettings", global_displaysettings); //16F4
    #endif
    return 1;
  }
  
  
  /// traffic & ped density multiplier value(s)
  if( _lw(addr - 0x4) == 0x2412FFFF && _lw(addr + 0x4) == 0x00A08825 && _lw(addr + 0x2C) == 0x00000000  ) { 
    /*******************************************************************
     *  0x00060E30: 0xE7941538 '8...' - swc1       $fpr20, 5432($gp)
    *******************************************************************/
    global_trafficdensity = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_trafficdensity", global_trafficdensity); // 0x1538
    #endif
    
    /*******************************************************************
     *  0x00060E24: 0xE794D1C4 '....' - swc1       $fpr20, -11836($gp)
    *******************************************************************/
    global_peddensity = (int16_t)_lh(addr-0xC); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_peddensity", global_peddensity); // 0xFFFFD1C4
    #endif
    
    return 1;
  }
  
  
  
  
  /// global max health & armor multiplier
  if( _lw(addr - 0x2C) == 0x10000027 && _lw(addr - 0x8) == 0x00000000 && _lw(addr + 0x10) == 0x10000002 &&  _lw(addr + 0x3C) == 0x02002025 ) { 
    /*******************************************************************
     *  0x003003F8: 0x3C04003E '>..<' - lui        $a0, 0x3E
     *  0x003003FC: 0x2484A4B0 '...$' - addiu      $a0, $a0, -23376
     *  0x00300400: 0x90840151 'Q...' - lbu        $a0, 337($a0)
    *******************************************************************/
    global_maxarmormult = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4) + _lh(addr+0x8); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_maxarmormult", global_maxarmormult-text_addr, global_maxarmormult); // 0x3DA601
    #endif

    /*******************************************************************
     *  0x003003A0: 0x3C04003E '>..<' - lui        $a0, 0x3E
     *  0x003003A4: 0x2484A4B0 '...$' - addiu      $a0, $a0, -23376
     *  0x003003A8: 0x90840150 'P...' - lbu        $a0, 336($a0)
    *******************************************************************/
    global_maxhealthmult = (_lh(addr-0x58) * 0x10000) + (int16_t)_lh(addr-0x54) + _lh(addr-0x50); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_maxhealthmult", global_maxhealthmult-text_addr, global_maxhealthmult); // 0x3DA600
    #endif
    
    return 1;
  }
  
  
  /// global never get tired from sprinting & Multiplayer bool
  /*************************************
   * ULUS-10160 v1.03 | 0x001447A0 | OK!
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 0x0013D734 | OK!
   **************************************/ 
  if( _lw(addr + 0x24) == 0x3C05C2C8 && _lw(addr + 0x80) == 0x3C053F00 && _lw(addr + 0xA4) == 0x3C0543FA ) { // 0x001447A0 (in FUN_001447a0)
    /*******************************************************************
     *  0x001447A0: 0x9385E458 'X...' - lbu        $a1, -7080($gp)
    *******************************************************************/
    global_ismultiplayer = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("[1] 0x%08X -> global_ismultiplayer", global_ismultiplayer); // cGpffffe458
    #endif
    
    /*******************************************************************
     *  0x001447F0: 0x3C06003E '>..<' - lui        $a2, 0x3E
     *  0x001447F4: 0x00E52821 '!(..' - addu       $a1, $a3, $a1
     *  0x001447F8: 0x24C6A4B0 '...$' - addiu      $a2, $a2, -23376
     *  0x001447FC: 0x00A62821 '!(..' - addu       $a1, $a1, $a2
     *  0x00144800: 0x90A5014D 'M...' - lbu        $a1, 333($a1)
    *******************************************************************/
    global_unlimtedsprint = (_lh(addr+0x50) * 0x10000) + (int16_t)_lh(addr+0x58) + _lh(addr+0x60); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_unlimtedsprint", global_unlimtedsprint-text_addr, global_unlimtedsprint); // DAT_003da5fd_playerNeverGetsTired
    #endif
    
    return 1;
  }

  /// SetWantedLevel function
  if( _lw(addr + 0x84) == 0x3405000C && _lw(addr - 0x6C) == 0x34020001 ) { // 0x00143470
    SetWantedLevel = (void*)(addr);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetWantedLevel()", addr-text_addr, addr);
    #endif
    
    return 1;
  } 
  /// SetMaxWantedLevel function + global
  if( _lw(addr + 0xC) == 0x2C850007 && _lw(addr + 0x88) == 0x34040005 ) { // 0x002afe3c
    
    SetMaxWantedLevel = (void*)(addr);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetMaxWantedLevel()", addr-text_addr, addr);
    #endif
    
    /*******************************************************************
     *  0x002AFE78: 0xAF84F6E8 '....' - sw         $a0, -2328($gp)
    *******************************************************************/
    global_maxwantedlevel = (int16_t)_lh(addr+0x3C); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_maxwantedlevel", global_maxwantedlevel); // uGpfffff6e8
    #endif
    
    return 1;
  } 
  
  
  /*** global_clockmultiplier *****************
  * ULUS US  - 0x0013D91C
  * ULUS EU  - 0x001352DC
  * ULUS GER - 
  * ULJM JP  - 0x00049AB8
  * ULET 006 - 
  * 
  * Note:  */
  /*************************************
   * ULUS-10160 v1.03 | 0x0013D91C
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
  //if( _lw(addr + 0x8) == 0x3404000C && _lw(addr + 0x18) == 0x3C043F80 && _lw(addr + 0x20) == 0x44846000 && _lw(addr + 0x50) == 0x00000000  ) { // 0x0013D91C
  if( (_lw(addr - 0x3C) == 0x3C0242C8 && _lw(addr - 0x30) == 0x34050006 && _lw(addr + 0x8) == 0x3404000C) ||  // 0x0013D91C
      (_lw(addr - 0x80) == 0x3C0242C8 && _lw(addr - 0x74) == 0x34050006 && _lw(addr + 0x8) == 0x3404000C) ) { // ULUS v1.01 - 0x00130590
    /*******************************************************************
     *  0x0013D91C: 0xAF841DDC '....' - sw         $a0, 7644($gp)
    *******************************************************************/
    global_clockmultiplier = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_clockmultiplier", global_clockmultiplier); // uGp00001ddc
    #endif
    return 1;
  } 
  
  /// globals cheat used boolean & counter 
  if( _lw(addr - 0x4) == 0x34040168 && _lw(addr - 0x1C) == 0x3404015D && _lw(addr - 0x5C) == 0x00402025 ) { 
    /*******************************************************************
     *  
    *******************************************************************/
    global_cheatusedcounter = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_cheatusedcounter", global_cheatusedcounter); // iGp00002448
    #endif
    
    /*******************************************************************
     *  
    *******************************************************************/
    global_cheatusedboolean = (int16_t)_lh(addr+0x4); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_cheatusedboolean", global_cheatusedboolean); // uGpffffc0fe
    #endif
    
    return 1;
  }
  
  
  /// global_freezegame and global_hudincutscene
  if( _lw(addr + 0xB8) == 0x340500CF && _lw(addr - 0x1C) == 0x00808025 ) { 
    /*******************************************************************
     *  0x00086CA8: 0x938420A8 '. ..' - lbu        $a0, 8360($gp)
    *******************************************************************/
    global_freezegame = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_freezegame", global_freezegame); // 0x20A8
    #endif
    
    /*******************************************************************
     *  0x00086CB8: 0x3C04003C '<..<' - lui        $a0, 0x3C
     *  0x00086CBC: 0x24843E30 '0>.$' - addiu      $a0, $a0, 15920
     *  0x00086CC0: 0x90840804 '....' - lbu        $a0, 2052($a0)
    *******************************************************************/
    global_hudincutscene = (_lh(addr+0x10) * 0x10000) + (int16_t)_lh(addr+0x14) + (int16_t)_lh(addr+0x18); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_hudincutscene", global_hudincutscene-text_addr, global_hudincutscene); // DAT_003c4634
    #endif
    
    return 1;
  }
  
  
  
  /// global for helpbox (special! there should be an address at gp+global_helpbox -> addr there + x
  if( _lw(addr + 0x80) == 0x3404014B && _lw(addr + 0x258) == 0x34040168 ) { // 0x00184E28
    /**** from cheat give weapons 1 ***********************************
     * 0x00184E28: 0x8F8416D8 '....' - lw         $a0, 5848($gp)
    *******************************************************************/
    global_helpbox = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_helpbox", global_helpbox); // iGp000016d8
    #endif
    
    return 1;
  }
  
  /// global camera stuff (inside "FUN_002bfe5c_Load_GTA_VCDat")
  if( _lw(addr + 0xC8) == 0x34150000 && _lw(addr + 0x16C) == 0x00003025 ) { 
    /*******************************************************************
     *  0x002BFEE4: 0x3C10003C '<..<' - lui        $s0, 0x3C
     *  0x002BFEE8: 0x26103E30 '0>.&' - addiu      $s0, $s0, 15920
    *******************************************************************/
    global_camera = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_camera", global_camera-text_addr, global_camera); // DAT_003C3E30
    #endif
    
    return 1;
  }
  
  
  /** UNCENSOR GERMAN VERSION **************** https://www.eg.bucknell.edu/~csci320/mips_web/ ********************************
  *
  * If the flag is set to 0x0 like in the German version it will have the following effects on the gameplay:
  * - player can't kick peds on ground
  * - no head exploading with sniper when headshot
  * - dead peds don't bleed
  * - dead peds don't drop money
  * - no bloody tire tracks
  * - no bonecracking sound when running over peds
  * - people wasted stat missing in stats menu
  *
  ****************************************************************************************************************************/
  if( _lw(addr+0x54) == 0x34120002 && _lw(addr+0xA0) == 0x34040008 && _lw(addr+0x180) == 0x28850004 ) { // FUN_00132e84
    #ifdef PATCHLOG
    logPrintf("UNCENSOR GERMAN VERSION! (0x%08X)", addr-text_addr);
    #endif
  
    /// set "isUncut" flag to 0x1    [$s0 holds 0x1]
    //if( _lh(addr+0x7A) == 0xA380 ) _sh(0xA390, addr+0x2A); // one of these makes 
    //if( _lh(addr+0xAE) == 0xA380 ) _sh(0xA390, addr+0xAE); // PauseMenu crash???
    if( _lh(addr+0x2B6) == 0xA380 ) _sh(0xA390, addr+0x2B6); // but works with this alone, so..
    
    return 1;
  } 
    
  /// global_ptr_water 
  if( (_lw(addr - 0x28) == 0x03A03025 && _lw(addr + 0x34) == 0x00094F82) ||  // 0x002F40FC
      (_lw(addr - 0x3C) == 0x02803025 && _lw(addr + 0x58) == 0x00074883) ) { // ULUS v1.01
    /*******************************************************************
     *  0x002F40FC: 0xAF85052C ',...' - sw         $a1, 1324($gp)
    *******************************************************************/
    global_ptr_water =  (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_ptr_water", global_ptr_water); // uGp0000052c
    #endif
    
    return 1;
  }
  
  /// global_buttoninput (used for to trigger stock cheats)
  /*************************************
   * ULUS-10160 v1.03 | 
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
  if( _lw(addr + 0x10) == 0x3053FFFF && _lw(addr + 0x44) == 0x0093282A ) { // 0x000C2934
    /*******************************************************************
     *  0x000C2934: 0x3C04003E '>..<' - lui        $a0, 0x3E
     *  0x000C2938: 0x2495A610 '...$' - addiu      $s5, $a0, -23024
    *******************************************************************/
    global_buttoninput = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_buttoninput", global_buttoninput-text_addr, global_buttoninput); // 0x3da610
    #endif
    
    return 1;
  } 
  
  /// addr_buttoncheat (used for to trigger stock cheats)
  if( _lw(addr + 0x10) == 0x34050032 && _lw(addr - 0x24) == 0x34050031 && _lw(addr - 0x74) == 0x3405004C ) { // 0x00187A10
    /*******************************************************************
     *  0x00187A10: 0x10800004 '....' - beqz       $a0, loc_00187A24
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_buttoncheat", addr-text_addr, addr);
    #endif
    
    addr_buttoncheat = addr; // 0x00187A10
    return 1;
  } 
  
  /// global_garagedata
  if( _lw(addr - 0x10) == 0x2404FFFF && _lw(addr + 0x3C) == 0x24C60030 ) {  // 0x46F9B0
    /*******************************************************************
     *  0x00168E80: 0x3C050047 'G..<' - lui        $a1, 0x47
     *  0x00168E90: 0x24A5F9B0 '...$' - addiu      $a1, $a1, -1616
    *******************************************************************/
    global_garagedata = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x10); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_garagedata", global_garagedata-text_addr, global_garagedata); //
    #endif
    
    var_garageslots = *(char*)(addr+0x24); // 0xC = 12 slots (4 in each garage)
    var_garageslotsize = *(char*)(addr+0x3C); // 0x30
    #ifdef PATCHLOG
    logPrintf("var_garageslots = 0x%08X", var_garageslots); //
    logPrintf("var_garageslotsize = 0x%08X", var_garageslotsize); //
    #endif
    
    return 1;
  } 
  
  /// VCS Pickups
  if( _lw(addr - 0x70) == 0x30870003 && _lw(addr + 0x44) == 0x00001025 &&  _lw(addr + 0x4) == 0x34050000  ) {  // 0x000EF880
    /*******************************************************************
     *  0x000EF880: 0x3C020046 'F..<' - lui        $v0, 0x46
     *  0x000EF888: 0x24423930 '09B$' - addiu      $v0, $v0, 14640
    *******************************************************************/
    global_pickups = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_pickups", global_pickups-text_addr, global_pickups); // 0x00463930
    #endif
    
    var_pickupslots = (int16_t)_lh(addr+0x34);
    var_pickupslotsize = (int16_t)_lh(addr+0x3C);
    #ifdef PATCHLOG
    logPrintf("var_pickupslots = 0x%08X", var_pickupslots); // 0x14F ? 0x150
    logPrintf("var_pickupslotsize = 0x%08X", var_pickupslotsize); // 0x40
    #endif
    
    return 1;
  }
  
  /// Custom Music
  /*************************************
   * ULUS-10160 v1.03 | 0x000B2860
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
  if( _lw(addr + 0x2C) == 0x8C84000C && _lw(addr + 0x38) == 0x01094821 && _lw(addr + 0x64) == 0x00000000 ) {
    HIJACK_FUNCTION(addr, _checkCustomTracksReady_patched, _checkCustomTracksReady); // sub_000B2860
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> _checkCustomTracksReady()", addr-text_addr, addr);
    #endif
    
    return 1;
  } 
  if ( _lw(addr + 0x24) == 0x2A04003A && _lw(addr - 0x10) == 0x00002825 ) {  // 0x001B34A4
    /*******************************************************************
     *  0x001B34A4: 0x3C04003D '=..<' - lui        $a0, 0x3D
     *  0x001B34AC: 0x2484BC10 '...$' - addiu      $a0, $a0, -17392
    *******************************************************************/
    global_radioarea = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); // actual address!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_radioarea", global_radioarea-text_addr, global_radioarea); // 0x003CBC10
    #endif
    
    return 1;
  } 
  /*************************************
   * ULUS-10160 v1.03 | 
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
  if( (_lw(addr - 0x20) == 0x3404006D && _lw(addr + 0x44) == 0x00002025) ||  // 0x000842E8
      (_lw(addr + 0x2c) == 0x3412006D && _lw(addr - 0x14) == 0x00000000) ) { // ULUS v1.01
    /*******************************************************************
     *  0x000842E8: 0x8F841708 '....' - lw         $a0, 5896($gp)
    *******************************************************************/
    global_custrackarea = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_custrackarea", global_custrackarea); // iGp00001708
    #endif
    
    return 1;
  } 
  
  /// VCS Radar Blips
  if( _lw(addr + 0x1c) == 0x340A0000 && _lw(addr + 0x48) == 0x254A0001 &&  _lw(addr + 0x84) == 0x2406FFFE  ) { // FUN_0000A03C
    /*******************************************************************
     *  0x0000A06C: 0x81220290 '..".' - lb         $v0, 656($t1)  <----- 0x290     ?????
     * 
     *  0x000a0a0: 70 02 25 ad     sw         param_2,0x270(t1)    <----- 0x270    
    *******************************************************************/
    var_radarblipspadding = (int16_t)_lh(addr+0x64);
    #ifdef PATCHLOG
    logPrintf("var_radarblipspadding = 0x%08X", var_radarblipspadding); // 0x270
    #endif
    
    
    /*******************************************************************
     *  0x0000A090: 0x2D48004B 'K.H-' - sltiu      $t0, $t2, 75
    *******************************************************************/
    var_radarblipslots = (int16_t)_lh(addr+0x54);
    #ifdef PATCHLOG
    logPrintf("var_radarblipslots = 0x%08X", var_radarblipslots); // 0x4B
    #endif
    
    
    /*******************************************************************
     *  0x0000A088: 0x25290030 '0.)%' - addiu      $t1, $t1, 48
     *******************************************************************/
    var_radarblipslotsize = (int16_t)_lh(addr+0x4C);
    #ifdef PATCHLOG
    logPrintf("var_radarblipslotsize = 0x%08X", var_radarblipslotsize); // 0x30
    #endif
    
    return 1;
  }
  
  /*************************************
   * ULUS-10160 v1.03 | 
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
   if( _lw(addr + 0x60) == 0x26240080 && _lw(addr + 0x2c) == 0x2405FFFF && _lw(addr + 0xB4) == 0x00002825 ) { // FUN_0001fa2c
    /*******************************************************************
     *  0x0001FA64: 0x8F8416DC '....' - lw         $a0, 5852($gp)
    *******************************************************************/
    global_radarblips = _lh(addr+0x38); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_radarblips", global_radarblips); // iGp000016dc
    #endif
    
    return 1;
  }
  
  /// world vehicle spawns
  if( _lw(addr + 0x54) == 0x01201825 && _lw(addr + 0xA8) == 0x00605825 ) { // FUN_002e9448_checkSetVehicleWorldSpawn
    /*******************************************************************
     *  0x002E94C0: 0x3C050068 'h..<' - lui        $a1, 0x68
     *  0x002E94C4: 0x24A5CA20 ' ..$' - addiu      $a1, $a1, -13792  
    *******************************************************************/
    addr_vehiclesworldspawn = (_lh(addr+0x78) * 0x10000) + (int16_t)_lh(addr+0x7C); // 0x0067CA20
    
    /*******************************************************************
     *  0x002E947C: 0x28E300C3 '...(' - slti       $v1, $a3, 195
    *******************************************************************/
    var_vehiclesworldspawnslots = (int16_t)_lh(addr+0x34);
    
    /*******************************************************************
     *  
     *  
    *******************************************************************/
    var_vehiclesworldspawnslotsize = 0x30; // get from code is complicated here TODO
    
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_vehiclesworldspawn", addr_vehiclesworldspawn-text_addr, addr_vehiclesworldspawn); // 0x0067CA20
    logPrintf("var_vehiclesworldspawnslots = 0x%08X", var_vehiclesworldspawnslots);
    logPrintf("var_vehiclesworldspawnslotsize = 0x%08X", var_vehiclesworldspawnslotsize); 
    #endif
    
    return 1;
  }
  
  /// for loadscreen TXDs replacement (VCS's Multiplayer loadscreen are lower quality and look quite bad. Furthermore VCS has random loadscreens already. And 12 singleplayer ones btw!!)
  /*************************************
   * ULUS-10160 v1.03 | 0x0013109C
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
  /*if( _lw(addr + 0x9C) == 0x30A50080 && _lw(addr + 0xD8) == 0x00402825 ) { // FUN_0013109c_loadSplashScreen
    #ifdef PATCHLOG
    logPrintf("[0] 0x%08X (0x%08X) --> loadSplashScreen()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, loadSplashScreen_patched, loadSplashScreen);
    return 1;
  }*/
  
  
  /// GAME.DTZ - FUN_0024f6f0_loadGAMEDTZ (contains pointers to all files packed in the dtz) (GAME.DTZ is loaded to 0x   )
  /*************************************
   * ULUS-10160 v1.01 | 0x0022303C (FUN_00223044) 
   * ULUS-10160 v1.02 | 
   * ULUS-10160 v1.03 | 0x0024f6f0
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   * ULET-00417 v0.07 | 
   **************************************/
   if( _lw(addr + 0xA4) == 0x3C044754 /*&& _lw(addr + 0xA8) == 0x24844147*/ ) { // FUN_0024f6f0
	
    /*******************************************************************
     *  0x0024F720: 0x3C04003C '<..<' - lui        $a0, 0x3C
     *  0x0024F72C: 0x24842500 '.%.$' - addiu      $a0, $a0, 9472
    *******************************************************************/
    ptr_memory_main = (_lh(addr+0x30) * 0x10000) + (int16_t)_lh(addr+0x3C);
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_memory_main", ptr_memory_main-text_addr, ptr_memory_main); // DAT_003c2500
    #endif
  
    /*******************************************************************
     *  0x0024F940: 0xAF842700 '.'..' - sw         $a0, 9984($gp)
     * Handling isn't linked in VCS????      I need these for config though
    *******************************************************************/
    //ptr_handlingCFG = _lh(addr+0x250); //WITHOUT GP!!
    //#ifdef PATCHLOG
      //logPrintf("0x%08X -> ptr_handlingCFG", ptr_handlingCFG); //uGp00002700
    //#endif
    //ptr_handlingCFG     = 0x000018;
    //var_handlingcfgslots     = 53; //todo where to find 
    //var_handlingcfgslotsize   = 0xE0; //todo where to find
    
    /// get more here TODO
    
    /*******************************************************************
     *  0024f7e4 a0 c1 84 af     sw         a0,-0x3e60(gp)
    *******************************************************************/
    ptr_buildingsIPL = (int16_t) _lh(addr+0xF4); // WITHOUT GP!!
    var_buildingsIPLslotsize = 0x60; // todo?!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_buildingsIPL", ptr_buildingsIPL); // uGpffffc1a0
    #endif
    
    /*******************************************************************
     * 0024f7ec a4 c1 84 af     sw         a0,-0x3e5c(gp)
    *******************************************************************/
    ptr_treadablesIPL = (int16_t) _lh(addr+0xFC); // WITHOUT GP!!
    var_treadablesIPLslotsize = 0x60; // todo?
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_treadablesIPL", ptr_treadablesIPL); // uGpffffc1a4
    #endif
  
    /*******************************************************************
     * 0024f7f4 b0 c1 84 af     sw         a0,-0x3e50(gp)
    *******************************************************************/
    ptr_dummysIPL = (int16_t) _lh(addr+0x104); // WITHOUT GP!!
    var_dummysIPLslotsize = 0x60; // todo?
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_dummysIPL", ptr_dummysIPL); // uGpffffc1b0
    #endif

    /*******************************************************************
     * 0x0024F95C: 0xAF842270 'p"..' - sw         $a0, 8816($gp)
    *******************************************************************/
    ptr_particleCFG = (int16_t) _lh(addr+0x26C); // WITHOUT GP!!
    var_particleCFGslots   = 90; // where to find?
    var_particleCFGslotsize = 0x84; // where to find?
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_particleCFG", ptr_particleCFG); // uGp00002270
    #endif

    /*******************************************************************
     * 0x0024f8dc d0 16 84 8f     lw         a0,LAB_000016d0(gp)
    *******************************************************************/
    ptr_carcolsDAT = (int16_t) _lh(addr+0x1EC); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_carcolsDAT", ptr_carcolsDAT); // ptr at iGp000016d0 + 8 
    #endif
    var_carcolsdatslots   = 128; // where to find?
    var_carcolsdatslotsize   = 0x3; // where to find?
    
    /*******************************************************************
     * 0x0024F93C: 0x8F85A470 'p...' - lw         $a1, -23440($gp)
    *******************************************************************/
    ptr_timecycDAT = (int16_t) _lh(addr+0x24C); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_timecycDAT", ptr_timecycDAT); // ptr at piGpffffa470
    #endif
    
    return 1;
  }
  
  
  /// IDEs (in DTZ but need to get globals from other location)
  /*************************************
   * ULUS-10160 v1.01 | 0x002C1ED0
   * ULUS-10160 v1.02 | 
   * ULUS-10160 v1.03 | 0x002E8884
   * ULES-00502 v1.02 | 0x002E8C64
   * ULES-00503 v1.02 | 0x002E8ADC
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 0x002E3E90
   * ULET-00417 v0.07 | 0x002E8C64
   **************************************/ 
  if( _lw(addr + 0x28) == 0x34100006 && _lw(addr + 0x44) == 0x90850010 ) {  // FUN_002e8884_IDE
    /*******************************************************************
     *  0x002E8898: 0xAF841DE8 '....' - sw         $a0, 7656($gp)
    *******************************************************************/
    ptr_IDEs = (int16_t) _lh(addr+0x14); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_IDEs = 0x%08X", addr+0x14-text_addr, addr+0x14, ptr_IDEs); //uGp00001de8
    #endif
    
    /*******************************************************************
     *  0x002E88A8: 0xAF850018 '....' - sw         $a1, 24($gp)
    *******************************************************************/
    ptr_IDETable = (int16_t) _lh(addr+0x24); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_IDETable = 0x%08X", addr+0x24-text_addr, addr+0x24, ptr_IDETable); // iGp00000018
    #endif
    
    return 1;
  }
  
  /// pedstat.dat (in DTZ but need to get globals from other location)
  if( _lw(addr - 0x30) == 0x24A5008C && _lw(addr + 0x8) == 0x8C840000 ) { // 0x001D0528 (inside FUN_001d0480)
    /*******************************************************************
     *  0x001D0528: 0x8F851DA4 '....' - lw         $a1, 7588($gp)
    *******************************************************************/
    ptr_pedstatTable = (int16_t) _lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_pedstatTable", ptr_pedstatTable); // iGp00001da4
    #endif
    
    var_pedstatDATslots   = 42; // where to find?
    var_pedstatDATslotsize = 0x34; // where to find?
    
    return 1;
  }
  
  /// loadStringFromGXT
  if( _lw(addr + 0x3C) == 0x92240022 && _lw(addr + 0x64) == 0x03E00008 ) { // FUN_001f2390_loadStringFromGXT
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> LoadStringFromGXT()", addr-text_addr, addr);
    #endif
    LoadStringFromGXT = (void*)(addr); //FUN_0010fad4_loadStringFromGXT
    HIJACK_FUNCTION(addr, LoadStringFromGXT_patched, LoadStringFromGXT);
    return 1;
  }
  if( _lw(addr - 0x8) == 0x02209025 && _lw(addr + 0x24) == 0x240AFFFF ) { // 0x00070F80
    /*******************************************************************
     *  00070f80 e8 d9 84 8f     lw         param_1,-0x2618(gp)
    *******************************************************************/
    ptr_gxtloadadr = (int16_t) _lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_gxtloadadr", ptr_gxtloadadr); // ppiGpffffd9e8
    #endif
    
    return 1;
  }
  
  /// radarIcon (for names)
  if(  _lw(addr + 0x20) == 0x00402025 && _lh(addr + 0x26) == 0x2604 && _lh(addr + 0x1A) == 0x2784 ) { // FUN_0000c1b8_loadRadarIcons
    /*******************************************************************
     *  0x0000C1DC: 0x260419E0 '...&' - addiu      $a0, $s0, 6624
    *******************************************************************/
    ptr_radarIconList = (int16_t) _lh(addr+0x24); // its just an offset in this case to calc with
    #ifdef PATCHLOG
    logPrintf("0x%08X -> ptr_radarIconList", ptr_radarIconList); // 0x19E0
    #endif
    
    return 1;
  }
  
  /// radio stations count
  if(  _lh(addr + 0x10) == 0xFF && _lw(addr + 0x24) == 0x00000000 && (_lw(addr) == _lw(addr + 0x14)) && _lw(addr - 0xC) == 0x00000000 ) { // 0x000B9164
    /*******************************************************************
     *  0x000B9164: 0x2A44000B '..D*' - slti       $a0, $s2, 11
    *******************************************************************/
    var_radios = (int16_t)_lh(addr);
    var_radios -= 1;
    #ifdef PATCHLOG
    logPrintf("var_radios = 0x%08X", var_radios);
    #endif
    return 1;
  }
  
  
  /// render global (used for cWorldStreamRender)
  if(  _lw(addr + 0x8) == 0x34050002 && _lh(addr + 0x12) == 0x1480 && _lw(addr - 0x18) == 0x00000000 && _lw(addr + 0x2C) == 0x00000000  ) { // 0x0013218C
    /*******************************************************************
     *  0x0013218C: 0x8F8416E8 '....' - lw         $a0, 5864($gp)
    *******************************************************************/
    render = (int16_t) _lh(addr); 
    #ifdef PATCHLOG
    logPrintf("render -> 0x%08X", render); // iGp000016e8
    #endif
    return 1;
  }
  
  
  /// savedata stuff (found twice! but thats fine I guess) (scraped since not working on emu and save space)
  /*************************************
   * ULUS-10160 v1.03 | OK!
   * ULES-00502 v1.02 | OK!
   * ULES-00503 v1.02 | OK!
   * ULJM-05297 v1.01 | should be
   * ULET-00417 v0.06 | should be too
   **************************************/ 
  /* if( _lw(addr + 0x20) == 0x34060010 && _lw(addr - 0x14) == 0x00403025 && _lw(addr - 0x80) == 0x34040001 ) { // 0x2b3a14
    *******************************************************************
     *  0x002B3A14: 0x2785F748 'H..'' - addiu      $a1, $gp, -2232
    *******************************************************************
    savedatakey = (int16_t) _lh(addr); 
    #ifdef PATCHLOG
    logPrintf("savedatakey -> 0x%08X", savedatakey); // gp0xfffff748
    #endif
    
    
    *******************************************************************
     *  0x002B39A4: 0x2792F7DC '...'' - addiu      $s2, $gp, -2084
    *******************************************************************
    if( _lh(addr + 0x58) == 0x9 ) // all versions
      titleid = (int16_t) _lh(addr-0x70); 
    else // German version  
      titleid = (int16_t) _lh(addr-0x74); 
    
    #ifdef PATCHLOG
    logPrintf("titleid -> 0x%08X", titleid); // gp0xfffff7dc
    #endif
    
    saveprefix = titleid + 0xC;
    
    return 1;
  } */ 
  
  /// global_freezetimers
  if( _lw(addr - 0x34) == 0x2405FFFF && _lw(addr - 0x28) == 0x34150001 ) { // 0x00060FE8
    /*******************************************************************
     *  0x00060FE8: 0x3C04003F '?..<' - lui        $a0, 0x3F
     *  0x00060FEC: 0x24845AE0 '.Z.$' - addiu      $a0, $a0, 23264
     *  0x00060FF0: 0xA080014D 'M...' - sb         $zr, 333($a0)
    *******************************************************************/
    global_freezetimers = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x4) + (int16_t)_lh(addr+0x8); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_freezetimers", global_freezetimers-text_addr, global_freezetimers); // DAT_003f5c2d_freezeOnScreenTimer
    #endif

    return 1;
  }
  
  /// ped task function
  if( _lw(addr + 0x38) == 0x3405003A  && _lw(addr + 0x70) == 0x30840200 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TaskCharWith()", addr-text_addr, addr);
    #endif
    TaskCharWith = (void*)(addr);  // 0x002c9698
    return 1;
  }
  
  /// warp ped to vehicle
  if( _lw(addr + 0x10) == 0x34120012 && _lw(addr + 0x14) == 0x00808825 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> WarpPedIntoVehicle()", addr-text_addr, addr);
    #endif
    WarpPedIntoVehicle = (void*)(addr);  // 0x00127a54  
    return 1;
  }
  /// warp ped to vehicle as passenger
  if( _lw(addr + 0x14) == 0x34080012 && _lw(addr + 0x1C) == 0x00809025 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> WarpPedIntoVehicleAsPassenger()", addr-text_addr, addr);
    #endif
    WarpPedIntoVehicleAsPassenger = (void*)(addr);  // 0x00127dc4
    return 1;
  }
  /// request model
  if(  _lw(addr + 0x4) == 0x00C03825 && _lw(addr + 0x40) == 0x00003025 && _lw(addr + 0xC) == _lw(addr + 0x30) ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> RequestModel()", addr-text_addr, addr);
    #endif
    RequestModel = (void*)(addr); // 0x2cf258
    return 1;
  }
  /// GiveWeaponAndAmmo
  if( _lw(addr + 0x48) == 0x0204A021  && _lw(addr + 0xC) == 0x00A08825 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> GiveWeaponAndAmmo()", addr-text_addr, addr);
    #endif
    GiveWeaponAndAmmo = (void*)(addr); // 0x001177DC
    return 1;
  }
  
  /// SetActorSkinTo (needs name to be lower case!)
  if( _lw(addr + 0x2C) == 0x340600A0  && _lw(addr + 0x48) == 0x24840048 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> SetActorSkinTo()", addr-text_addr, addr);
    #endif
    SetActorSkinTo = (void*)(addr); // 0x0010ae04
    return 1;
  }
  /// LoadAllModelsNow
  if( _lw(addr + 0x4) == 0x308600FF  && _lw(addr + 0x14) == 0x34050001  && _lw(addr - 0x10) == 0x00003025 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> LoadAllModelsNow()", addr-text_addr, addr);
    #endif
    LoadAllModelsNow = (void*)(addr); //0x002cf610
    return 1;
  }
  /// RefreshActorSkin
  if( _lw(addr + 0x1C) == 0x24C40030  && _lw(addr + 0x3C) == 0x34050001 ) {
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> RefreshActorSkin()", addr-text_addr, addr);
    #endif
    RefreshActorSkin = (void*)(addr); // 0x0010af00
    return 1;
  }
  
  /// TaskDuckVCS
  if( _lw(addr + 0x34) == 0x34060099  && _lw(addr + 0x90) == 0x34842000 ) { // FUN_001a71f0_taskDuck
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TaskDuckVCS()", addr-text_addr, addr);
    #endif
    TaskDuckVCS = (void*)(addr); // 0x001a71f0
    return 1;
  }
  /// TaskUnDuck
  if( _lw(addr + 0x20) == 0x34060099  && _lw(addr + 0x6C) == 0x3C06C080 ) { // FUN_001a74fc_unDuck
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TaskUnDuck()", addr-text_addr, addr);
    #endif
    TaskUnDuck = (void*)(addr); // 0x001a74fc
    return 1;
  }
  
  /// for loadscreen debug text
  /*************************************
   * ULUS-10160 v1.03 | OK!
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
   if( _lw(addr + 0x2C) == 0x3C043E80 && _lw(addr + 0x10) == 0x3C044040 ) { // FUN_00131490_DrawLoadingBar
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> DrawLoadingBar()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, DrawLoadingBar_patched, DrawLoadingBar);
    return 1;
  } 
  if( _lw(addr + 0x18) == 0x30F100FF && _lw(addr + 0x14) == 0x00C09025 ) { // FUN_0013170c_DrawLoadscreen
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) --> DrawLoadscreen()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, Loadscreen_patched, Loadscreen);
    return 1;
  }

  /// StartNewScript
  if( _lw(addr - 0xC) == 0x2402FFFF  && _lw(addr + 0x28) == 0x24A60001 && _lw(addr + 0x4C) == 0x34040001 ) { // FUN_0005F470
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> StartNewScript()", addr-text_addr, addr);
    #endif
    StartNewScript = (void*)(addr); // 0x0005F470
    return 1;
  }
  /// globals: ScriptSpace & MainScriptSize
  if(  _lw(addr + 0x2C) == 0xA204020A && _lw(addr + 0x28) == 0x34040001 ) { // 0x002B8118
    /*******************************************************************
     *  0x002B8118: 0x8F858E24 '$...' - lw         $a1, -29148($gp)
    *******************************************************************/
    global_ScriptSpace = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_ScriptSpace", global_ScriptSpace); // iGpffff8e24 aka ScriptSpace
    #endif

    /*******************************************************************
     *  0x002B811C: 0x8F861F44 'D...' - lw         $a2, 8004($gp)
    *******************************************************************/
    global_MainScriptSize = (int16_t)_lh(addr+0x4); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_MainScriptSize", global_MainScriptSize); // iGp00001f44 aka MainScriptSize
    #endif
    
    /*******************************************************************
     *  0x02b8100: 50 1f 90 8f     lw         s0,LAB_00001f50(gp)
    *******************************************************************/
    global_LargestMissionScriptSize = (int16_t)_lh(addr-0x18); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_LargestMissionScriptSize", global_LargestMissionScriptSize); // iGp00001f50 aka LargestMissionScriptSize
    #endif
    
    return 1;
  }
  
  /// TankControl
  if( _lw(addr + 0x50) == 0x3C04BFB2  && _lw(addr + 0x78) == 0x3C04403C ) { // FUN_00043c68_TankControl
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> TankControl()", addr-text_addr, addr);
    #endif
    TankControl = (void*)(addr); //
    return 1;
  }
  /// BlowupVehiclesInPath
  if( _lw(addr + 0x38) == 0x3C053DCC  && _lw(addr + 0x58) == 0x30840080 ) { // FUN_0002eb18_blowupVehiclesInPath
    #ifdef PATCHLOG
      logPrintf("0x%08X (0x%08X) -> BlowupVehiclesInPath()", addr-text_addr, addr);
    #endif
    BlowupVehiclesInPath = (void*)(addr); //
    return 1;
  }
  
  /// areas: multiplayer
  if(  _lw(addr + 0x10) == 0x2C85000A && _lw(addr + 0x1C) == 0x00042080 && _lw(addr - 0x10) == 0x00000000 ) { // 0x0006CE78
    
    /*******************************************************************
     *  0x0006CE78: 0x3C04003C '<..<' - lui        $a0, 0x3C
     *  0x0006CE80: 0x24844FC0 '.O.$' - addiu      $a0, $a0, 20416
    *******************************************************************/
    global_mp_parameters = (_lh(addr) * 0x10000) + (int16_t)_lh(addr+0x8); 
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> global_mp_parameters", global_mp_parameters-text_addr, global_mp_parameters); // DAT_003c4fc0
    #endif

    
    /*******************************************************************
     *  0x0006CEAC: 0x8F841704 '....' - lw         $a0, 5892($gp)
    *******************************************************************/
    global_mp_1 = (int16_t)_lh(addr+0x34); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_mp_1", global_mp_1); // iGp00001704
    #endif
    
    return 1;
  }
  
  /// IsPlayerOnAMission
  if( _lw(addr + 0x1C) == 0x14860003 && _lw(addr + 0x28) == 0x34020001  ) { // FUN_0005f4f8 _CTheScripts_IsPlayerOnAMission
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> IsPlayerOnAMission()", addr-text_addr, addr);
    #endif
    IsPlayerOnAMission = (void*)(addr); //
  
    /*******************************************************************
     *  0x0005F4F8: 0x8F841F20 ' ...' - lw         $a0, 7968($gp)
    *******************************************************************/
    global_OnAMissionFlag = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_OnAMissionFlag", global_OnAMissionFlag); // iGp00001f20
    #endif
    
    return 1;
  }
  
  /// CalculateNewVelocity
  if( _lw(addr + 0x4) == 0x8C8501C8  && _lw(addr + 0x10) == 0x30A40001 ) { // FUN_00115cd8_UpdatePosition
  //if( _lw(addr + 0x118) == 0x3C044334  && _lw(addr + 0x28) == 0x00808025 ) { // FUN_00115480_CalculateNewVelocity
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> UpdatePosition()", addr-text_addr, addr);
    #endif
    HIJACK_FUNCTION(addr, UpdatePosition_patched, UpdatePosition); //
    return 1;
  }
  
  
  /// weapon.dat (in DTZ but need to get globals from other location)
  /*************************************
   * ULUS-10160 v1.03 | OK!
   * ULES-00502 v1.02 | 
   * ULES-00503 v1.02 | 
   * ULJM-05297 v1.01 | 
   * ULET-00417 v0.06 | 
   **************************************/ 
   if( _lw(addr + 0x10) == 0x10C00004 && _lw(addr) == 0x000439C0 ) {  // FUN_0031bd70
    /*******************************************************************
     *  0x0031BD7C: 0x8F852950 'P)..' - lw         $a1, 10576($gp)  
    *******************************************************************/
    ptr_weaponTable = (int16_t)_lh(addr+0xC); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> ptr_weaponTable", ptr_weaponTable-text_addr, ptr_weaponTable); // piGp00002950
    #endif
    
    var_weaponDATslots   = 40; // where to find? TODO
    var_weaponDATslotsize = 0x70;
    
    return 1;
  }
  
  
  /// bmx jump multiplier (bmx can't jump in ULUS v1.01 and older!)
  if(  _lw(addr - 0x1C) == 0x3C053F80 && _lw(addr - 0x28) == 0xC48C0630 ) { // in FUN_001658c8_bmx_jumping
    /*******************************************************************
     *  0x00165950: 0xC78EBC34 '4...' - lwc1       $fpr14, -17356($gp)
    *******************************************************************/
    global_bmxjumpmult = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_bmxjumpmult", global_bmxjumpmult); // fGpffffbc34
    #endif

    return 1;
  }
  
  
  /// police chase helicopter model
  #ifdef PREVIEW
  if( _lw(addr) == 0x34050105 && _lw(addr + 0x1C) == 0x34050105 ) { // 0x02d4100 in FUN_002d3e94_StreamVehiclesAndPeds
    /*******************************************************************
     *  0x002D4100: 0x34050105 '...4' - li         $a1, 0x105
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_policechaseheli_1", addr-text_addr, addr);
    #endif
    addr_policechaseheli_1 = addr;
    return 1;
  }
  if( _lw(addr) == 0x34040105 && _lw(addr - 0xC) == 0x24A50004 ) { // 0x01ed254 in FUN_001ed12c
    /*******************************************************************
     *  0x001ED254: 0x34040105 '...4' - li         $a0, 0x105
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_policechaseheli_2", addr-text_addr, addr);
    #endif
    addr_policechaseheli_2 = addr;
    return 1;
  }
  if( _lw(addr) == 0x34050105 && _lw(addr + 0x8) == 0x34060001 ) { // 0x01ed68c in FUN_001ed600
    /*******************************************************************
     *  0x001ED68C: 0x34050105 '...4' - li         $a1, 0x105
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_policechaseheli_3", addr-text_addr, addr);
    #endif
    addr_policechaseheli_3 = addr;
    return 1;
  } 
  #endif
  
  /// DoHoverSuspensionRatios
  if( _lw(addr + 0x60) == 0x84A40056  && _lw(addr + 0x90) == 0x8C950000 ) { // FUN_0002f358
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> DoHoverSuspensionRatios()", addr-text_addr, addr);
    #endif
    DoHoverSuspensionRatios = (void*)(addr); //
    return 1;
  }
  
  /// pre-load garage vehicles when player spawns (this "feature" is what causes the famous garage bug) (not present in ULUS v1.01 and older!)
  if( _lw(addr + 0x1C) == 0x2A440005 && _lw(addr - 0x24) == 0x34040000 ) { // 0x002D0EF4
    /*******************************************************************
     *  0x002D0EF4: 0x0C05A8CE '....' - jal        sub_0016A338
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> 'garage bug fix'", addr-text_addr, addr);
    #endif
    _sw(0x00000000, addr); // nop function
    return 1;
  }
  
  /// addr_heliheight (also planes)
  if( _lh(addr) == 0x42A0 && _lh(addr + 0x18) == 0xBF80 && _lw(addr + 0xC) == 0x00000000 ) {  // 0x002FDDA0
    /*******************************************************************
     *  0x002FDDA0: 0x3C0442A0 '.B.<' - lui        $a0, 0x42A0
    *******************************************************************/
    #ifdef PATCHLOG
    logPrintf("0x%08X (0x%08X) -> addr_heliheight", addr-text_addr, addr);
    #endif
    addr_heliheight = addr; // 0x002FDDA0
    return 1;
  }
  
  #ifdef PREVIEW
  /// wind
  if( _lw(addr + 0x14) == 0x3C053F33 && _lw(addr + 0x2C) == 0x28850015  ) { // 0x002F8E04
    /*******************************************************************
     *  0x002F8E04: 0xE78F209C '. ..' - swc1       $fpr15, 8348($gp)
    *******************************************************************/
    global_Wind = (int16_t)_lh(addr); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_Wind", global_Wind); // fGp0000209c
    #endif
  
    /*******************************************************************
     *  0x002F8E24: 0xE7901E50 'P...' - swc1       $fpr16, 7760($gp)
    *******************************************************************/
    global_WindClipped = (int16_t)_lh(addr+0x20); // WITHOUT GP!!
    #ifdef PATCHLOG
    logPrintf("0x%08X -> global_WindClipped", global_WindClipped); // fGp00001e50 
    #endif
    
    /// nop out to stop game from setting it on its own (we take care of it in the cheat function)
    _sw(0x00000000, addr + 0x20); // 0x002f8e24
  
    return 1;
  }
  #endif
  
  return 0;  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GAMELOG
#define loglines 14
char logstr[loglines][128] = { "a","b","c","","","","","","","","","x","y","z" }; // a = newest log

void debugprint_patched(const char *text, ...) { 
  va_list list;
  char string[128];

  va_start(list, text);
  vsprintf(string, text, list);
  va_end(list);

  /// add to log array
  int i = loglines; 
  while( i > 0 ) { 
    sprintf(logstr[i], "%s", logstr[i-1]); 
  i--;
  } sprintf(logstr[0], "%s", string);
  
  /// print to log.txt as well
  logPrintf(string); 
  
  //sceKernelPrintf(string);  
}

void drawGameLog() { // print log array
  int i = loglines; 
  u32 step = 0x08000000;
  u32 color = WHITE - (loglines * step); 
  float textsize = 0.5f;
  float x = 470.0f, y = 75.0f, pad = 10.0f;
  while( i >= 0 )
    drawString(logstr[--i], ALIGN_RIGHT, FONT_DIALOG, textsize, SHADOW_OFF, x, y+=pad, color+=step);
}
#endif

#ifdef MEMORY
int (*FUN_0029febc)(int param_1); // calc "free"
int (*FUN_0029fed4)(int param_1); // calc "largest free"

extern char buffer[256];

void drawMemoryUsage() {
  char sizestr[16];
  getSizeString(sizestr, sceKernelTotalFreeMemSize());
  snprintf(buffer, sizeof(buffer), "sceKernelTotalFreeMemSize = %s", sizestr);
  drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 350.0f, 20.0f, RED);

  getSizeString(sizestr, sceKernelMaxFreeMemSize());
  snprintf(buffer, sizeof(buffer), "sceKernelMaxFreeMemSize = %s", sizestr);
  drawString(buffer, ALIGN_RIGHT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 350.0f, 35.0f, RED);


  FUN_0029febc = (void*)(mod_text_addr + 0x29febc); // calc "free"
  FUN_0029fed4 = (void*)(mod_text_addr + 0x29fed4); // calc "largest free"

  int _main = mod_text_addr + 0x37d640; // DAT_0037d640 "ptr_memory_main"
  drawString("main", ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 30.0f, 40.0f, WHITE);

  getSizeString(sizestr, getInt(_main));
  snprintf(buffer, sizeof(buffer), "total: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 50.0f, RED);

  getSizeString(sizestr, FUN_0029febc(_main));
  snprintf(buffer, sizeof(buffer), "free: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 60.0f, RED);

  getSizeString(sizestr, FUN_0029fed4(_main));
  snprintf(buffer, sizeof(buffer), "largest: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 70.0f, RED);


  int _streaming = mod_text_addr + 0x385870; // DAT_00385870
  drawString("streaming", ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 30.0f, 90.0f, WHITE);

  getSizeString(sizestr, getInt(_streaming));
  snprintf(buffer, sizeof(buffer), "total: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 100.0f, RED);

  getSizeString(sizestr, FUN_0029febc(_streaming));
  snprintf(buffer, sizeof(buffer), "free: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 110.0f, RED);

  getSizeString(sizestr, FUN_0029fed4(_streaming));
  snprintf(buffer, sizeof(buffer), "largest: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 120.0f, RED);


  int _volatile = mod_text_addr + 0x3859a0; // DAT_003859a0
  drawString("volatile", ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 30.0f, 140.0f, WHITE);

  getSizeString(sizestr, getInt(_volatile));
  snprintf(buffer, sizeof(buffer), "total: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 150.0f, RED);

  getSizeString(sizestr, FUN_0029febc(_volatile));
  snprintf(buffer, sizeof(buffer), "free: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 160.0f, RED);

  getSizeString(sizestr, FUN_0029fed4(_volatile));
  snprintf(buffer, sizeof(buffer), "largest: %s", sizestr);
  drawString(buffer, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 40.0f, 170.0f, RED);
}
#endif
  
/// Game's Function Calls & Hijacks //////////////////////////////////////////////////////////////////////////////////////////////////

SceInt64 cur_micros = 0, delta_micros = 0, last_micros = 0;
u32 frames = 0;
float fps = 0.0f;
int memory_main_free = 0;

SceInt64 sceKernelGetSystemTimeWidePatched(void) { // LCS & VCS
  SceInt64 cur_micros = sceKernelGetSystemTimeWide();
  if( cur_micros >= (last_micros + 1000000) ) { // every second
    delta_micros = cur_micros - last_micros;
    last_micros = cur_micros;
    fps = (frames / (double)delta_micros) * 1000000.0f;
    frames = 0;
  } frames++;
  
  memory_main_free = getInt(ptr_memory_main + 0x10) - getInt(ptr_memory_main + 0xC) - getInt(ptr_memory_main + 0x14); // basically what FUN_0029febc does

  #ifdef LOG
  static int debug_skgstwp = 1;
  static int debug_skgstwp2 = 1;
  if( debug_skgstwp2 && debug_skgstwp == 0 ) {
    logPrintf("[INFO] %i: sceKernelGetSystemTimeWidePatched() ran the SECOND time", getGametime());
    debug_skgstwp2 = 0;
  }  
  if( debug_skgstwp ) {
    logPrintf("[INFO] %i: sceKernelGetSystemTimeWidePatched() ran the FIRST time", getGametime());
    debug_skgstwp = 0;
  }
  #endif
  

  /// Value Getter / Checker //////////////////
  
  gp_ = LCS ? 0 : gp; // helper: zero for LCS, $gp for VCS
  
  /// pplayer
  pplayer = GetPPLAYER();

  /// pcar & more
  pcar = GetPCAR();
  pcar_id = getVehicleID(pcar);
  if( pcar )
    pcar_type = getVehicleTypeByID(pcar_id);

  /// pobj 
  pobj = (pcar == 0) ? pplayer : pcar;

  /// gametimer
  gametimer = getGametime();

  /// island
  island = getIsland();

  /// language
  language = getLanguage();

  /// is multiplayer active
  multiplayer = getMultiplayer();
    
    
    /// Objects 
    
    /* LCS
    0x00348534 -A-> PTR_PtrNode
    0x00348538 -A-> PTR_EntryInfoNode
    0x0034853C -B-> ptr_pedestriansobj
    0x00348540 -B-> ptr_vehiclesobj
    0x00348544 -A-> ptr_buildingsIPL
    0x00348548 -A-> ptr_treadablesIPL
    0x0034854C -B-> ptr_worldobj
    0x00348550 -A-> ptr_dummysIPL
    0x00348554 -B-> ptr_audioscriptobj
    */

    /* VCS
    0xFFFFC190 -A-> PTR_PtrNode
    0xFFFFC194 -A-> PTR_EntryInfoNode
    0xFFFFC198 -B-> ptr_pedestriansobj
    0xFFFFC19C -B-> ptr_vehiclesobj
    0xFFFFC1A0 -A-> ptr_buildingsIPL
    0xFFFFC1A4 -A-> ptr_treadablesIPL
    0xFFFFC1A8 -B-> ptr_worldobj
    0xFFFFC1AC -B-> ptr_businessobj
    0xFFFFC1B0 -A-> ptr_dummysIPL
    0xFFFFC1B4 -B-> ptr_audioscriptobj
    */
    
  int temp;
    
  /// Pedestrians
  temp = getInt(ptr_pedestriansobj + gp_);
  if( isInMemBounds(temp) ) {
    peds_base = getInt(temp); // memory address of first PED Object    
    peds_max = getInt(temp + 0x8); // maximum ped objs possible -> SP: 60 LCS | 70 VCS
    peds_cur = getPedActiveObjects(peds_base, peds_max, var_pedobjsize);
  }
    
  /// Vehicles
  temp = getInt(ptr_vehiclesobj + gp_);
  if( isInMemBounds(temp) ) {
    vehicles_base = getInt(temp); // memory address of first Vehicle Object      
    vehicles_max = getInt(temp + 0x8); // maximum vehcile objs possible -> SP: 
    vehicles_cur = getVehicleActiveObjects(vehicles_base, vehicles_max, var_vehobjsize);
  }
  
  
  /// World Objects
  temp = getInt(ptr_worldobj + gp_);
  if( isInMemBounds(temp) ) {
    worldobjs_base = getInt(temp);
    worldobjs_max = getInt(temp + 0x8); 
    worldobjs_cur = getWorldActiveObjects(worldobjs_base, worldobjs_max, var_wldobjsize);
  }  
  
  
  /// Business Objects
  if( VCS ) {
    temp = getInt(ptr_businessobj + gp);
    if( isInMemBounds(temp) ) {
      businessobjs_base = getInt(temp);
      businessobjs_max = getInt(temp + 0x8); 
      businessobjs_cur = getWorldActiveObjects(businessobjs_base, businessobjs_max, var_bsnobjsize); // THIS IS THE OBJ FUNCTION!
    }  
  }
    
  /// Buildings.ipl
  temp = getInt(ptr_buildingsIPL + gp_);
  if( isInMemBounds(temp) ) {
    buildingsIPL_base = getInt(temp);
    buildingsIPL_max = getInt(temp + 0x8); // LCS: 6315 slots
  }
    
  /// Treadables.ipl
  if( LCS ) { // missing ptr for VCS ? :/
    temp = getInt(ptr_treadablesIPL + gp_);
    if( isInMemBounds(temp) ) {
      treadablesIPL_base = getInt(temp);
      treadablesIPL_max = getInt(temp + 0x8); // LCS: 1219 slots
    }
    }
   
  /// Dummys.ipl
  temp = getInt(ptr_dummysIPL + gp_);
  if( isInMemBounds(temp) ) {
    dummysIPL_base = getInt(temp);
    dummysIPL_max = getInt(temp + 0x8); // LCS: 2575 slots
  }
   
   
  /// pedcols.dat (VCS only) & carcols.dat
  if( VCS )  {
    temp = getInt(ptr_carcolsDAT + gp);
    if( isInMemBounds(temp) ) {
      pedcolsDAT_base = getInt(temp + 0x8);
      carcolsDAT_base = pedcolsDAT_base + (var_carcolsdatslotsize * var_carcolsdatslots); // vcs carcols are right behind pedcols
    }
  } else { // LCS carcols.dat
    carcolsDAT_base = getInt(ptr_carcolsDAT) + 0x4; // 0x4 padding
  }


  /// Currently used Objects
  pickups_cur   = getPickupsActiveObjects(global_pickups, var_pickupslots, var_pickupslotsize);
  mapicons_cur  = getMapiconsActiveObjects(LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding), var_radarblipslots, var_radarblipslotsize);
  vehspawns_cur = getVehicleWorldSpawnSlotActiveObjects(addr_vehiclesworldspawn);
  garage_cur    = getGarageVehicleActiveObjects(global_garagedata);
  

  /// trigger Stock Button Cheats
  if( key_to_pad ) { // by ugly gamecode patching to make this work (easier than calling func)
    #ifdef LOG
    logPrintf("[INFO] %i: key_to_pad = 0x%X", getGametime(), key_to_pad );
    #endif
    setByte(addr_buttoncheat + 0x3, 0x14); //bne
    
    if( key_to_pad != 0x32 ) { // no need to set if R Trigger
      setByte(addr_buttoncheat + 0x10, key_to_pad); // custom key
    }
    key_to_pad = 0x00;
  
  } else { // code restore taken care of next in loop..
    if( getByte(addr_buttoncheat + 0x3) == 0x14 ) { // reset
      setByte(addr_buttoncheat + 0x3, 0x10); // beq (reset)
      
      if( getByte(addr_buttoncheat + 0x10) != 0x32 ) {
        setByte(addr_buttoncheat + 0x10, 0x32); // reset "R-Trigger"
      }
    }
  } clearICacheFor(addr_buttoncheat); // has to be here?! (otherwise R Trigger crashes VCS)


  /// after teleport with vehicle handler
  teleportFixForVehicle();


  /// check cheats FUNC_CHECK
  checkCheats();


  /// patch some things once after spawn (custom vehicle colors, make rc vehicles driveable etc..)
  static int once = 1;
  if( gametimer < menuopendelay*2 && gametimer >= menuopendelay ) { // window between gametime 2000 to 4000
    if( once ) { 
      applyOnce();
      /* if( flag_use_advaconfig ) { // scrapped because config gets too big on PSP to handle :(
      load_config_block(LCS ? "LVEH" : "VVEH", addr_vehiclesworldspawn); // custom spawns from config
      load_config_block(LCS ? "LVCO" : "VVCO", carcolsDAT_base); // custom vehicle colors from config
      if(LCS) load_config_block(LCS ? "LHND" : "VHND", getInt(ptr_handlingCFG + (LCS ? 0 : gp)) + 0x20); // todo VCS
      load_config_block(LCS ? "LPTC" : "VPTC", getInt(ptr_particleCFG + (LCS ? 0 : gp))); // particles
      load_config_block(LCS ? "LPST" : "VPST", getInt(getInt(ptr_pedstatTable+(LCS?0x0:gp)))); // Pedstats
      load_config_block(LCS ? "LWPN" : "VWPN", getInt(getInt(ptr_weaponTable+(LCS?0x0:gp)) + 4)); // Weapon.dat
      } */ once = 0; // has been applied
    }
  } else once = 1; // re-set to 1 for next time (out of window at this point)

  /// apply cheats FUNC_APPLY
  if( gametimer >= menuopendelay ) 
    applyCheats();
  
  /// check for and trigger achievements
  #ifdef ACHIEVEMENTS
  if( gametimer >= menuopendelay ) 
    if( !multiplayer ) 
      achievements();
  #endif
  
  /////////////////////////////////////////////

  //sceKernelDcacheWritebackAll();
  //sceKernelDcacheWritebackInvalidateAll();

  return cur_micros;
}


#ifdef ACHIEVEMENTS

achievement_pack achievement[] = {
  {"Well, thats fast!",       0x100,  0 }, // reach 300 kmh in vehicle
  {"Tsunami",                 0x101,  0 }, // raise water level above value 60
  {"To Hell and back",        0x102,  0 }, // fall through the hell
  {"Pest Control",            0x103,  0 }, // throw a teargas
  
  /// LCS only
  {"Sorry, out of Order",     0x200,  0 }, // get inside the gta3 savehouse elevator
  {"Destination Portland",    0x201,  0 }, // fly the dodo all the way to portland
  {"Back to the future",      0x202,  0 }, // configure a Delorean and reach 88 MPH
  {"Stargazing",              0x203,  0 }, // Freecam near star observatory at night (23:00 - 3:59)
  
  /// VCS only
  {"In The Air Tonight",      0x300,  0 }, // Phil Collins model and on stage
  
  
  /** Achieveable without Cheat Device (these should be in the standalone Trophy Plugin) *******************************************************
  {"There are no Easter Eggs up here. Go away",  x,  0,  0 }, // jump against Easteregg Window from Helipad (VCS)
  {"You picked the wrong house fool",        x,  0,  0 }, // try walk into the wrong savehouse (Eddies Garage LCS) or (OceanView Hotel VCS)
  {"Ah shit, here we go again.",          x,  0,  0 }, // drive BMX with trailer trash outfit (VCS)
  **********************************************************************************************************************************************/

}; int achievement_size = (sizeof(achievement)/sizeof(achievement_pack));


/////////////////////////////////////

void achievements_reset() {
  int i;
  for( i = 0; i < achievement_size; i++ ) { // loop achievements list
    achievement[i].unlocked = 0; // lock
    //achievement[i].value = 0; // zero value
  }
  closeMenu();
  setTimedTextbox("Achievements have been reset!", 7.00f);
}

int achievements_getDone() {
  int i, counter = 0;
  for( i = 0; i < achievement_size; i++ ) // loop achievements list
    if( achievement[i].unlocked ) 
      counter++;
  return counter;  
}

void achievements() {
  int i;
  
  /// CHECK ////////////////////////////////////////////////////////////////////////
  for( i = 0; i < achievement_size; i++ ) { // loop achievements list
    if( !achievement[i].unlocked ) {
      switch(achievement[i].id) {
        
        case 0x100: /// "Speedfreak"
          if( pcar && getVehicleSpeed(pcar) > 6.00f ) { // 6.00 = ?? kmh
            achievement[i].unlocked = 2; // unlocked
          } break;
        
        case 0x101: /// "Tsunami"
          if( ((int)world_waterlevel(FUNC_GET_VALUE, -1, -1, -1) >= 1600) ) { // 1020 = 2.0f, 1153 = 15.3f, 1600 = 60.0f
            achievement[i].unlocked = 2; // unlocked
          } break;
        
        case 0x102: /// "Welcome back from hell"
          if( !pcar && (getFloat(pplayer+0x38) <= (LCS ? -80.0f : -237.0f)) && (getFloat(pplayer+(LCS?0x78:0x148)) <= -0.05f ) ) { // below limit and falling check
            achievement[i].unlocked = 2; // unlocked
          } break;
          
        case 0x103: /// "Pest Control"
          if( getPedsCurrentWeapon(pplayer) == (LCS ? 14 : 16) && (current_buttons & PSP_CTRL_CIRCLE) ) { // 
            achievement[i].unlocked = 2; // unlocked
          } break;
        
        
        /// LCS only
        case 0x200: /// "Sorry, out of order"
          if( LCS && island == 2 && checkCoordinateInsideArea(getFloat(pplayer+0x30), getFloat(pplayer+0x34), getFloat(pplayer+0x38), 103.00f, -484.50f, 16.0f, 2.0f) ) { // make it 3.0f so running near door is enough
            achievement[i].unlocked = 2; // unlocked
          } break;
          
        case 0x201: /// "Dodomaster" 
          if( LCS && pcar && island == 1 && getVehicleID(pcar) == 0xA4 && isVehicleInAir(pcar) && getFloat(pcar+0x30) >= 800.0f && getFloat(pcar+0x38) >= 60.0f ) { // 
            achievement[i].unlocked = 2; // unlocked
          } break;
        
        case 0x202: /// "Back to the future"
          if( LCS && pcar_id == 187 && lcs_getVehicleColorBase(pcar) == 13 && lcs_getVehicleColorStripe(pcar) == 0 && ((getVehicleSpeed(pcar) * 100) * 0.621371) >= 88 ) { // V8 Ghost or Deluxo (same ID! :D)
            achievement[i].unlocked = 2; // unlocked
          } break;
              
        case 0x203: /// "Stargazing"
          if( 
          #ifdef FREECAM 
          flag_freecam && 
          #endif
          island == 3 && (getClockHours() < 4 || getClockHours() >= 23) && checkCoordinateInsideArea(getFloat(global_camera +0x340), getFloat(global_camera +0x344), getFloat(global_camera +0x348), -540.0f, 542.90f, 230.00f, 60.0f) ) { // 
            achievement[i].unlocked = 2; // unlocked
            //drawString("NOW", ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 250.0f, 222.0f, RED); // for testing
          } break;
        
        
        /// VCS only
        case 0x300: /// "In The Air Tonight"
          if( VCS && island == 3 && (getPedID(pplayer) == 139 || getPedID(pplayer) == 146)  && checkCoordinateInsideArea(getFloat(pplayer+0x30), getFloat(pplayer+0x34), getFloat(pplayer+0x38), -1574.0f, 1335.0f, -228.0f, 2.0f) ) { // 
            achievement[i].unlocked = 2; // unlocked
            //drawString("NOW", ALIGN_FREE, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 250.0f, 222.0f, RED); // for testing
          } break;
        
        
        
        case 0: /// 
          if( 0 ) { // 
            achievement[i].unlocked = 2; // unlocked
          } break;
        
        
        
        /*
        case 7: /// "There are no Easter Eggs up here. Go away"
          if( VCS && !pcar && island == 2 && checkCoordinateInsideArea(getFloat(pplayer+0x30), getFloat(pplayer+0x34), getFloat(pplayer+0x38), -483.0f, 1133.0f, 65.5f, 2.0f) ) { // 
            achievement[i].unlocked = 2; // unlocked
          } break;
          
        case 8: /// "You picked the wrong house fool" 
          if( !pcar && island == 1 && checkCoordinateInsideArea(getFloat(pplayer+0x30), getFloat(pplayer+0x34), getFloat(pplayer+0x38), (LCS ? 889.70f : 228.70f), (LCS ? -308.40f : -1277.90f), (LCS ? 8.70f : 12.00f), 2.0f) ) { // Eddies Garage LCS or OceanView Hotel VCS
            achievement[i].unlocked = 2; // unlocked
          } break;
          
        case 11: /// "Ah shit, here we go again."
          if( VCS && ((pcar_id == 0xB2) ) && getPedID(pplayer) == 0x9D ) { // 
            achievement[i].unlocked = 2; // unlocked
          } break;
        */ 
        
      
      }
    }
  }
  
  
  /// DISPLAY //////////////////////////////////////////////////////////////////////
  
  if( flag_menu_running || isTextboxShowing() ) // don't show achievements when in menu or a textbox is there already!
    return;
  
  char buffer[128];
  char au[] = "Achievement unlocked!";
  float duration = 7.00f; // duration the textbox should be displayed
  
  for(i = 0; i < achievement_size; i++) { // loop achievements list
    if( achievement[i].unlocked == 2 ) { // check for "just unlocked"
      snprintf(buffer, sizeof(buffer), "~w~%s (%d/%d) ~n~%s", au, achievements_getDone(), achievement_size, achievement[i].title);
      achievement[i].unlocked = 1; // set "was displayed"
      goto triggerachievement; // trigger it then!
    }
  }
  
  return;
  
  static u8 script_achievement_lcs[] = {
    /// 0399: play_mission_passed_tune 1 
    0x99, 0x03, 
    0x07, // byte
    0x01, // 1

    /// 010E: add_score $PLAYER_CHAR value 50000 // money 
    0x0E, 0x01, 
    0xCE, 0x18, // $PLAYER_CHAR
    0x06, // int
    0x50, 0xC3, 0x00, 0x00, // $50.000
    
    /// 004E: terminate_this_script 
    0x4E, 0x00
  };
  
  static u8 script_achievement_vcs[] = {
    /// 022B: play_mission_passed_tune 1 
    0x2B, 0x02, 
    0x07, // byte
    0x01, // 1

    /// 0094: add_score $PLAYER_CHAR value 50000 // money 
    0x94, 0x00, 
    0xD0, 0x0E, // $PLAYER_CHAR
    0x06, // int
    0x50, 0xC3, 0x00, 0x00, // $50.000 
    
    /// 0023: terminate_this_script 
    0x23, 0x00
  };
  
  triggerachievement:
    CustomScriptExecute(LCS ? (int)&script_achievement_lcs : (int)&script_achievement_vcs); // make game execute it
    setTimedTextbox(buffer, duration);
  
}
#endif

int buttonsToActionPatched(void *a1) { // LCS & VCS
  int res = buttonsToAction(a1);

  //((short *)a1)[5] = 0xFF;   // Simulate L trigger
  //((short *)a1)[7] = 0xFF;   // Simulate R trigger
  //((short *)a1)[32] = 0xFF; // Simulate R trigger (second pad thingy)
    
  #ifdef LOG
  static int debug_btap = 1;
  if( debug_btap ) {
    logPrintf("[INFO] %i: buttonsToActionPatched() ran the first time", getGametime() );
    debug_btap = 0;
  }
  #endif
  
  /// process button inputs
  buttonInput();  
  
  /// block button inputs for game
  if( flag_keys_disable == 1 ) {
    //((short *)a1)[1] = 0; // ANALOG LEFT & RIGHT
    //((short *)a1)[2] = 0; // ANALOG UP & DOWN
    ((short *)a1)[5] = 0; // L-TRIGGER
    ((short *)a1)[7] = 0; // R-TRIGGER
    ((short *)a1)[9] = 0; // UP
    ((short *)a1)[10] = 0; // DOWN
    ((short *)a1)[11] = 0; // LEFT
    ((short *)a1)[12] = 0; // RIGHT
    //((short *)a1)[17] = 0; // START
    ((short *)a1)[18] = 0; // SELECT
    ((short *)a1)[19] = 0; // SQUARE
    ((short *)a1)[20] = 0; // TRIANGLE
    ((short *)a1)[21] = 0; // CROSS
    ((short *)a1)[22] = 0; // CIRCLE  
  }

  return res;
}


/** cWorldStream_Render **************************************
mode 0 - streets, some buildings
mode 1 - more detailed buildings and interior
mode 2 - everything with transparency like trees, windows etc
**************************************************************/
void cWorldStream_Render_Patched(void *this, int mode) { // World is rendered -> allow cheat menu


  #ifdef LOG
  static int debug_cwsrp = 1;
  if( debug_cwsrp ) {
    logPrintf("[INFO] %i: cWorldStream_Render_Patched() ran the first time", getGametime() );
    debug_cwsrp = 0;
  }
  #endif
  
  #ifdef GAMELOG
  if( mode == 0 )
    drawGameLog(); // todo - move to different hooked func
  #endif
  
  #ifdef MEMORY
  if( mode == 0 )
    drawMemoryUsage(); // todo - move to different hooked func
  #endif
  
  if( mode == 0 )
    if( gametimer >= menuopendelay) // delay after new game
      draw();
    #ifdef LANG
    if ( gametimer >= 1000) {
      static int lang_ran = 1;

      if (lang_ran) {
        setup_lang(LanguageConfigStart);
        lang_ran = 0;
      }
    }
    #endif
  
  cWorldStream_Render(this, mode); // continue
}

void (* FUN_002c22a0)(int param_1); // something with initiate text render attributes in PauseMenu -> game is paused -> do not allow menu
void FUN_002c22a0_patched(int param_1) { // VCS
  stopMenu();
  FUN_002c22a0(param_1); // continue
}

void (* FUN_002db0c0)(int param_1); // something when in PauseMenu -> game is paused -> do not allow menu
void FUN_002db0c0_patched(int param_1) { // LCS
  stopMenu();
  FUN_002db0c0(param_1); // continue
}

int (*_checkCustomTracksReady)(int param_1); // return 1 if there is at least one custom track file in the savegame
int _checkCustomTracksReady_patched(int param_1) {
  if( flag_customusic > 0 ) 
    return 1; // found at least 1 track
  
  return _checkCustomTracksReady(param_1);
}

char *msg1 = NULL;
char *msg2 = NULL;

void (*Loadscreen)(char * string1, char * string2, char *txdname, uint param_4);
void Loadscreen_patched(char * string1, char * string2, char *txdname, uint param_4) {
  //if( txdname != NULL )
    //logPrintf("[INFO] %i: Loadscreen_patched(%s)", getGametime(), txdname);
  
  /// backup debug messages for use in "DrawLoadingBar_patched"
  msg1 = string1;
  msg2 = string2;
  
  if( random_loadscreens(FUNC_GET_STATUS, -1, -1) ) {
    static int prev = -1;
    int new = -1;
  
    short whitelist_size = 12;
    const char *whitelist[] = { "loadsc0", // replace if matches one of these
                  "loadsc1", 
                  "loadsc2", 
                  "loadsc3",
                  "loadsc4", // VCS
                  "loadsc5", // VCS
                  "loadsc6", // VCS
                  "loadsc7", // VCS
                  "loadsc8", // VCS
                  "loadsc9", // VCS
                  "loadsc10", // VCS
                  "loadsc11" }; // VCS
                  
    short lcs_replacers_size = 12;
    const char *lcs_replacers[] = { "loadsc0", // the replacements to use for LCS
                    "loadsc1", 
                    "loadsc2", 
                    "loadsc3", 
                    "mpload0", // 5 
                    "mpload1",  
                    "mpload2",
                    "mpload3", 
                    "mpload4", 
                    "mpload5", // 10
                    "mpload6", 
                    "mpload7" };
                    
    short vcs_replacers_size = 22;
    const char *vcs_replacers[] = { "loadsc0", // the replacements to use for VCS
                    "loadsc1", 
                    "loadsc2", 
                    "loadsc3", 
                    "loadsc4", 
                    "loadsc5", 
                    "loadsc6", 
                    "loadsc7", 
                    "loadsc8", 
                    "loadsc9", 
                    "loadsc10", 
                    "loadsc11", 
                    "mpload0", // mp loadscreens are lower quality and look quite bad :/
                    "mpload1", 
                    "mpload2", 
                    "mpload3", 
                    "mpload4", 
                    "mpload5", 
                    "mpload6", 
                    "mpload7", 
                    "mpload8", 
                    "mpload9" };
                    
                    
    if( txdname != NULL ) { // load new splash (if 0 then the game uses the previously loaded)
      #ifdef LOG
      logPrintf("[INFO] %i: Loadscreen_patched() original: '%s'", getGametime(), txdname);
      #endif  
      int i;
      for( i = 0; i < whitelist_size; i++ ) {
        if( strcmp(whitelist[i], txdname) == 0 ) {
          do {
            new = rand() % (LCS ? lcs_replacers_size : vcs_replacers_size); // decide which to use
            #ifdef LOG
            logPrintf("[INFO] %i: Loadscreen_patched() ..random new: %i aka '%s'", getGametime(), new, (LCS ? lcs_replacers[new] : vcs_replacers[new]));
            #endif  
          } while( new == prev ); // don't use the same twice (generate new if it happoens to be the same)
          prev = new;
          #ifdef LOG
          logPrintf("[INFO] %i: Loadscreen_patched() ..replacing '%s' with '%s'!", getGametime(), txdname, (LCS ? lcs_replacers[new] : vcs_replacers[new]));  
          #endif
          txdname = (char *)(LCS ? lcs_replacers[new] : vcs_replacers[new]); // replace!!
        }
      }
    }
  }
  Loadscreen(string1,string2,txdname,param_4);
}
  
void (*DrawLoadingBar)(float param_1);
void DrawLoadingBar_patched(float param_1) {
  DrawLoadingBar(param_1);
  if( debug_loadscreens(FUNC_GET_STATUS,-1,-1) ) { // only if CHEAT status is ON
    if( msg1 != NULL ) {
      if(LCS) drawStringLCS(msg1, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 32.0f, 200.0f, WHITE);
      if(VCS) drawStringVCS(msg1, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 25.0f,  25.0f, WHITE); // alt. 210.0f (right above bar)
    }
    if( msg2 != NULL ) {
      if(LCS) drawStringLCS(msg2, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 32.0f, 215.0f, WHITE);
      if(VCS) drawStringVCS(msg2, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 25.0f,  40.0f, WHITE); // alt. 225.0f (right above bar)
    }
  
  }
}

int LoadStringFromGXT_patched(int gxt_adr,char *string, int param_3, int param_4, int param_5, int param_6,int param_7,int param_8) {
  //ushort *text = testfunc(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8);
  int ret = LoadStringFromGXT(gxt_adr, string, param_3, param_4, param_5, param_6, param_7, param_8);
  
  //logPrintf("CTextGet 0x%08X: 0x%08X %s", ret, gxt_adr, string);
  //logPrintf("0x%08X", &string);
  
  #ifdef USERSCRIPTS
  #define CSTGXTS 16 // defined in main
  #define CSTGXTLGT 256 // ...
  extern ushort custom_gxts[CSTGXTS][CSTGXTLGT];
  if( strncmp(string, "CUST_", 5) == 0 ) { // CUST_00, CUST_01, ...
    #if defined(LOG) || defined(USERSCRIPTLOG)
    logPrintf("%s", string);
    #endif
    
    //logPrintf("%c%c", string[5], string[6]);
    int x = (((int)string[5] - 48) * 10) + ((int)string[6] - 48); // sigh..
    //logPrintf("%i", x);
    
    #if defined(LOG) || defined(USERSCRIPTLOG)
    logPrintf("0x%08X", &custom_gxts[x]);
    #endif
    return (int)&custom_gxts[x];
  }
  #endif
  return ret; 
}

#ifdef PREVIEW
/// fake swimming
int FUN_000e7d70_CCam_IsTargetInWater_patched(int param_1) {
  return (fake_swimming(FUNC_GET_STATUS, -1, -1, -1) ? 0 : FUN_000e7d70_CCam_IsTargetInWater(param_1));
}
 
void *FUN_001a8d9c_CPed_ProcessBuoyancy_patched(int param_1) {     
  FUN_001a8d9c_CPed_ProcessBuoyancy(param_1); // do buoyancy stuff first so that checkPedIsInWater() works

  //logPrintf("> %.2f %.2f 0%.2f Car: 0x%08X -> 0x%08X 0x%08X 0x%08X 0x%08X 0x%02X", param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9);
  
  if( fake_swimming(FUNC_GET_STATUS, -1, -1, -1) && checkPedIsInWater(pplayer) ) {
    float test[4];
    FUN_00109dac_CWaterLevel_GetWaterLevel(getFloat(pplayer+0x30),getFloat(pplayer+0x34), test);
  
    float boost = 0.05f;
    int cross = (flag_menu_running == 0 && (current_buttons & PSP_CTRL_CROSS)) ? 1 : 0; // is cross pressed bool
    float crawl = ((ystick < 0.0f) && cross) ? -ystick*0.3f : 0.0f; // only if stick forward & cross pressed

    char buffer[128]; // commenting this block out will NOT make it work for real hardware ?!? (when it was at other position in file)
    snprintf(buffer, sizeof(buffer), "z = %.2f, xstick = %.2f, ystick = %.2f", test[0], xstick, ystick);
    drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 20.0f, RED);
 
   /** TODO ********
    * - make boost static so that player accelerates and keeps some momentum
    * - animation?! FUN_00286f88_CAnimManager_BlendAnimation
    * - camera front is where player should swim to (like player on land)
    * - 
    *
   ***/
 
    /// turn with camera (todo)
    //setFloat(pplayer+0x10, getFloat(global_camera + 0xC4));
    //setFloat(pplayer+0x14, getFloat(global_camera + 0xC8));
 
 
    /// turn with stick
    if( xstick < -0.20 || xstick > 0.20 ) { 
      setFloat(pplayer + 0xA8, -0.005 * xstick); //turn player
    } else { // stop turning (since game thinks we are falling)
      setFloat(pplayer + 0x88, 0.0f);
    }
  
    /// add more speed when cross pressed
    if( cross ) {
     boost = 0.1f;
    }

    /// small jump
    if( (pressed_buttons & PSP_CTRL_SQUARE) && !cross ) {
     setFloat(pplayer+0x98, 0.1f );
    }
 
    /// forward-thrust
    if( ystick < -0.25 ) { // stick forward
      extern float walkspd_mult; // to allow "Walking Speed" cheat have effect on swim speed as well
      setFloat(pplayer+(LCS?0x70:0x140), -getFloat(pplayer+4)*fabs(ystick*boost*walkspd_mult)); // thrust
      setFloat(pplayer+(LCS?0x74:0x144), getFloat(pplayer)*fabs(ystick*boost*walkspd_mult));  // thrust
    } 
 
    /// adjust player height in water
    if( getFloat(pplayer+0x38) < test[0]-0.4f+crawl) { // -0.4f so that player is right height (lower under water)
      setFloat(pplayer+0x38, test[0]-0.4f+crawl);   // +crawl adjust (add height depending on leaning forward)
    }
 
    /// lean forward
    if( ystick < 0.05f ) { // only forward
     setFloat(pplayer + 0x18,  ystick*0.4f); // just a little
     if( cross ) // even more when cross pressed
       setFloat(pplayer + 0x18,  ystick); // -0.9f
    }
 
    /// disable roll after falling (when getting on land again)
    setFloat(pplayer+0x4E0, getFloat(pplayer+0x4E4)); // continuously setting current dir
     
    /// set current weapon to be fist slot
    setByte(pplayer + 0xB84,  0x00);
 
    /// set camera to not go below sea level
    // todo although only nice to have
 
    /// blocks falling animation
    //setFloat(pplayer + 0x1A8, 0.0f); 
 
  }
  
  return 0;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void user_cheats(int calltype, int keypress, int defaultstatus) {
  switch( calltype ) {
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        #ifdef PREVIEW
        usercheats_create();
        #endif
      }
      break;
  }  
}

#ifdef USERSCRIPTS
void user_scripts(int calltype, int keypress, int defaultstatus) {
  switch( calltype ) {
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        userscripts_create();
      }
      #ifdef HEXEDITOR      
      else if( keypress == PSP_CTRL_SQUARE ) {
        //hexeditor_create(getInt(global_ScriptSpace + (LCS ? 0 : gp)), 0, memory_low, memory_high, "> ScriptSpace");
        int scriptspace = getInt(global_ScriptSpace + (LCS ? 0 : gp));
        hexeditor_create(scriptspace, 1, scriptspace, scriptspace + getInt(global_MainScriptSize + (LCS ? 0 : gp)) + getInt(global_LargestMissionScriptSize + (LCS ? 0 : gp)), "> ScriptSpace");
      
      //} else if( keypress == PSP_CTRL_TRIANGLE ) {
      //  hexeditor_create(getInt(global_ScriptSpace + (LCS ? 0 : gp)) + getInt(global_MainScriptSize + (LCS ? 0 : gp)), 0, memory_low, memory_high, "> ScriptSpace + MainScriptSize");
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        hexeditor_create(getInt(global_ScriptSpace + (LCS ? 0 : gp)) + getInt(global_MainScriptSize + (LCS ? 0 : gp)) + getInt(global_LargestMissionScriptSize + (LCS ? 0 : gp)) - 4096, 0, memory_low, memory_high, "> ScriptSpace + MainScriptSize + LargestMission - 4096");
      }
      #endif
      break;
  }    
}
#endif

#ifdef HEXEDITOR
void hexeditor() {
  address_create();
}

void hexeditpplayer() {
  hexeditor_create(pplayer, 1, pplayer, pplayer + var_pedobjsize, "> pplayer"); // (LCS ? 0xCB0 : 0xD10)
}

void hexeditpcar() {
  if( pcar ) 
    hexeditor_create(pcar, 1, pcar, pcar + var_vehobjsize, "> pcar"); // (LCS ? 0x6E0 : 0x820)
}
#endif

#ifdef FREECAM
void freecam(int calltype, int keypress, int defaultstatus) {
  
  switch( calltype ) {
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        freecam_create();
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        
        hex_marker_addx(global_camera + (LCS ? 0x340 : 0x90), sizeof(float)); // camera_x
        hex_marker_addx(global_camera + (LCS ? 0x344 : 0x94), sizeof(float)); // camera_y
        hex_marker_addx(global_camera + (LCS ? 0x348 : 0x98), sizeof(float)); // camera_z
        
        hex_marker_addx(global_camera + (LCS ? 0x254 : 0x198), sizeof(float)); // fov
        
        hex_marker_addx(global_camera + (LCS ? 0x330 : 0x80), sizeof(float)); // cam_coord_x1
        hex_marker_addx(global_camera + (LCS ? 0x334 : 0x84), sizeof(float)); // cam_coord_y1
        hex_marker_addx(global_camera + (LCS ? 0x338 : 0x88), sizeof(float)); // cam_coord_z1
        
        hex_marker_addx(global_camera + (LCS ? 0x360 : 0xD0), sizeof(float)); // cam_coord_x2
        hex_marker_addx(global_camera + (LCS ? 0x364 : 0xD4), sizeof(float)); // cam_coord_y2
        hex_marker_addx(global_camera + (LCS ? 0x368 : 0xD8), sizeof(float)); // cam_coord_z2
        
        hex_marker_addx(global_camera + (LCS ? 0xCC : 0x798 ), sizeof(float)); // in vehicle camera mode (used to unbind cam)
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(global_camera, 1, global_camera, memory_high, "> camera location");
        #endif
      }
      break;
  }  
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *fps_toggle(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          flag_draw_FPS = status = 0;
        } else {
          flag_draw_FPS = status = 1;
        }
      } break;
    
    case FUNC_TOGGLE_CHEAT:
      status = flag_draw_FPS = keypress; // keypress reused as on/off boolean
      break;
    
    case FUNC_SET: 
      status = flag_draw_FPS = defaultstatus;
      break;
  }
  
  return NULL;
}

void *mem_toggle(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          flag_draw_MEM = status = 0;
        } else {
          flag_draw_MEM = status = 1;
        }
      } break;
    
    case FUNC_TOGGLE_CHEAT:
      status = flag_draw_MEM = keypress; // keypress reused as on/off boolean
      break;
    
    case FUNC_SET: 
      status = flag_draw_MEM = defaultstatus;
      break;
  }
  
  return NULL;
}


void *coords_toggle(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if(status) {
          flag_draw_COORDS = status = 0;
        } else {
          flag_draw_COORDS = status = 1;
        }
      } break;
      
    case FUNC_SET: 
      status = flag_draw_COORDS = defaultstatus;
      break;
  }
  
  return NULL;
}

char speed[16];
char gear[16];
void *speedometer_toggle(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static int i = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_GET_VALUE: 
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return ( i ? "MP/H" : "KM/H");
      
    case FUNC_APPLY:
      if( pcar ) { 
        /// speed (Example: "ESPRIT" 1.065 (fMaxVelocity) * 180 * 1.2 = 230 km/h as set in beta handling.cfg)  See: https://github.com/guard3/g3DTZ/blob/e22a1a2295fe136e702153aabb391453be9f6305/source/core/HandlingMgr.cpp#L13
        sprintf(speed, (i ? "%.0f MP/H" : "%.0f KM/H"), (getVehicleSpeed(pcar) * 180.0f * 1.2f) * (i ? 0.621371 : 1)); // og CD used raw speed calculated from moving vector
        
        /// gears
        sprintf(gear, "Gear: %X", getVehicleCurrentGear( pcar ) );
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          flag_draw_SPEEDO = status = 0;
        } else {
          flag_draw_SPEEDO = status = 1;
        }
      } else if( keypress == PSP_CTRL_LEFT ) { // LEFT
        i = 0;
      } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
        i = 1;
      } break;
      
    case FUNC_SET: 
      status = flag_draw_SPEEDO = defaultstatus;
      if( defaultval >= 0 ) i = defaultval;
      else i = 0;
      if( i > 1) i = 0; // fix for bad config value
      break;
  }
  
  return NULL;
}


#ifdef DEBUG
extern short flag_draw_DBGVALS;
void *debug_vars(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS) { // CROSS
        if( status ) {
          flag_draw_DBGVALS = status = 0;
        } else {
          flag_draw_DBGVALS = status = 1;
        }
      }   
      break;
      
    case FUNC_SET: 
      status = flag_draw_DBGVALS = defaultstatus;
      break;
  }
  
  return NULL;
}  
void *debug_monitor(int calltype, int keypress, int defaultstatus, int defaultval) {  

  char buffer[128]; 
  static int status, i = 0; // default position
  const char *list_names[] = { "General", // 0
                               "Globals", // 1
                               "Player",  // 2
                               "Vehicle", // 3
                               "tests",   // 4
                               "5" };     // 5            
  static int list_size = (sizeof(list_names)/sizeof(*list_names))-1;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*)i;
      
    case FUNC_GET_STRING: 
      return (void *)list_names[i];
      
    case FUNC_APPLY:
      if( flag_menu_running == 0 ) {
        if( i == 0 ) { // General
          drawString("General", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 5.0f, AZURE );
          
            /// mod_text_addr
            snprintf(buffer, sizeof(buffer), "mod_text_addr = 0x%08X", mod_text_addr);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 20.0f, WHITE);
            
            /// mod_text_size
            snprintf(buffer, sizeof(buffer), "mod_text_size = 0x%08X", mod_text_size);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 40.0f, WHITE);
            
            /// mod_data_size
            snprintf(buffer, sizeof(buffer), "mod_data_size = 0x%08X", mod_data_size);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 60.0f, WHITE);
            
            /// Global Pointer Register
            snprintf(buffer, sizeof(buffer), "$gp = 0x%08X (0x%08X)", gp, gp-mod_text_addr);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 80.0f, WHITE);
                

        } else if( i == 1 ) { // Globals
          drawString("Globals", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 5.0f, AZURE );
        
            /// gametimer
            snprintf(buffer, sizeof(buffer), "gametimer = %i", gametimer);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 20.0f, YELLOW);
            
            /// weather globals
            if( LCS ) snprintf(buffer, sizeof(buffer), "weather: %i, %i, %i", getShort(global_weather),      getShort(global_weather + 0x2),      getShort(global_weather + 0x84) );
            if( VCS ) snprintf(buffer, sizeof(buffer), "weather: %i, %i, %i", getShort(global_weather + gp), getShort(global_weather + 0x8 + gp), getShort(global_weather + 0x794 + gp) );
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 40.0f, YELLOW);
          
            /// global_currentisland
            snprintf(buffer, sizeof(buffer), "island = %i", island);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 60.0f, YELLOW);
        
            /// global_systemlanguage
            snprintf(buffer, sizeof(buffer), "language = %i", language);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 80.0f, YELLOW);
        
            /// global_ismultiplayer
            snprintf(buffer, sizeof(buffer), "multiplayer = %i", multiplayer);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 100.0f, YELLOW);
        
        
            //snprintf(buffer, sizeof(buffer), "DAT_0035a288: 0x%X", getInt(mod_text_addr + 0x035a288) );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 40.0f, 70.0f, CYAN);
            //snprintf(buffer, sizeof(buffer), "DAT_0035a28c: 0x%X", getInt(mod_text_addr + 0x035a28C) );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 40.0f, 90.0f, CYAN);
            //snprintf(buffer, sizeof(buffer), "DAT_00354b9c: 0x%X (max)", getInt(mod_text_addr + 0x0354b9c) );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 40.0f, 110.0f, CYAN);
            //snprintf(buffer, sizeof(buffer), "DAT_00354ba0: 0x%X (media max)", getInt(mod_text_addr + 0x0354ba0) );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 40.0f, 130.0f, CYAN);
        
        } else if( i == 2 ) { // pplayer
          drawString("pplayer", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 5.0f, AZURE );
          
          if( pplayer > 0 ) {
            snprintf(buffer, sizeof(buffer), "pplayer = 0x%08X", pplayer); // player ped handle
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 20.0f, WHITE);
          
            snprintf(buffer, sizeof(buffer), "Mass = %.2f, %.2f", getFloat(pplayer+0xD0), getFloat(pplayer+0xD4));
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 40.0f, CYAN);
            
            if( LCS ) snprintf(buffer, sizeof(buffer), "animation: 0x%0X", getByte(pplayer + 0x250) );
            if( VCS ) snprintf(buffer, sizeof(buffer), "animation: 0x%0X", getByte(pplayer + 0x894) );
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 60.0f, CYAN);
            
        
            snprintf(buffer, sizeof(buffer), "health = %.2f", getPedHealth(pplayer) );
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 80.0f, CYAN);
            
            snprintf(buffer, sizeof(buffer), "armor = %.2f", getPedArmor(pplayer) );
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 100.0f, CYAN);
            
            snprintf(buffer, sizeof(buffer), "facing = %.2f", getPedFacingDirectionInDegree(pplayer) );
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 120.0f, CYAN);
            
            snprintf(buffer, sizeof(buffer), "drowning = %d", getPedDrowning(pplayer) );
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 140.0f, CYAN);
          }
      
          
        } else if( i == 3 ) { // pcar
          drawString("pcar", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 5.0f, AZURE );
          
          if( pcar ) {        
            snprintf(buffer, sizeof(buffer), "pcar: 0x%08X", pcar); // player car handle
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 20.0f, WHITE);
            
            snprintf(buffer, sizeof(buffer), "pcar_id = 0x%02X", pcar_id);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 40.0f, AZURE);
              
            snprintf(buffer, sizeof(buffer), "pcar_type = 0x%02X", pcar_type);
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 60.0f, AZURE);
              
            snprintf(buffer, sizeof(buffer), "isVehicleInWater = %X", isVehicleInWater(pcar));
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 80.0f, AZURE);
            
            snprintf(buffer, sizeof(buffer), "isVehicleInAir = %X", isVehicleInAir(pcar));
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 100.0f, AZURE);
            
            snprintf(buffer, sizeof(buffer), "isVehicleUpsideDown = %X", isVehicleUpsideDown(pcar) );
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 120.0f, AZURE);
            
            snprintf(buffer, sizeof(buffer), "health = %.2f", getVehicleHealth(pcar));
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 140.0f, AZURE);
            
            snprintf(buffer, sizeof(buffer), "speed = %.2f", getVehicleSpeed(pcar));
            drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 10.0f, 160.0f, AZURE);
          } 
      
          
        } else if( i == 4 ) { // tests
          drawString("tests", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 5.0f, AZURE );
          
            //snprintf(buffer, sizeof(buffer), "TESTVAL1 = 0x%08X -> 0x%08X", TESTVAL1, *(int*)(TESTVAL1+gp) );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 120.0f, WHITE);
            
            //snprintf(buffer, sizeof(buffer), "TESTVAL1 = 0x%08X", TESTVAL1 );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 120.0f, WHITE);
            //snprintf(buffer, sizeof(buffer), "TESTVAL2 = 0x%08X", TESTVAL2 );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 140.0f, WHITE);
            //snprintf(buffer, sizeof(buffer), "TESTVAL3 = 0x%08X", TESTVAL3 );
            //drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 160.0f, WHITE);

        } else {
          //error
        }
        
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS) { // CROSS
        if( status ) {
          flag_draw_DEBUG = status = 0;
        } else {
          flag_draw_DEBUG = status = 1;
        }
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT && i < list_size) { // RIGHT
        i++;
      
      }   
      break;
      
    case FUNC_SET: 
      status = flag_draw_DEBUG = defaultstatus;
      i = 0;
      if( defaultval >= 0 && defaultval < list_size ) 
        i = defaultval;
      break;
  }
  
  return NULL;
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void *cdr_freezegameinmenu(int calltype, int keypress, int defaultstatus) { 
  static int status;
  static int state = 0; // merry-go-round to acomplish one-time-apply only
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_APPLY: 
      if( state == 1 ) {
        freezegame_enable:
        setFreezeGame(ON);
        state = 2;
      } 
      if(state == 3) {
        freezegame_disable:
        setFreezeGame(OFF);
        state = 0;
      }
      break;  
      
    case FUNC_CHECK: 
      if( state == 0 && flag_menu_running == 1 ) state = 1; // to be frozen
      if( state == 2 && flag_menu_running == 0 ) state = 3; // to be unfrozen
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          setFreezeGame(OFF);
          state = 0;
          status = 0;
          //ini_puts("SETTINGS", "CDR_FREEZEGAMEINMENU", "FALSE", config);
        } else {
          status = 1;
          //ini_puts("SETTINGS", "CDR_FREEZEGAMEINMENU", "TRUE", config);
        }
      } break;
    case FUNC_SET: 
      status = defaultstatus;
      if(status && flag_menu_running ) goto freezegame_enable;
      else if(status && flag_menu_running ) goto freezegame_disable;
      break;
  }
  
  return NULL;
}


void *cdr_allowuiblocking(int calltype, int keypress, int defaultstatus) { 
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          //ini_puts("SETTINGS", "CDR_ALLOWUIBLOCK", "FALSE", config);
          blockui_disable:
          flag_ui_blocking = 0;
          status = 0;
        } else {
          //ini_puts("SETTINGS", "CDR_ALLOWUIBLOCK", "TRUE", config);
          blockui_enable:
          flag_ui_blocking = 1;
          status = 1;
        }
      } break;
    case FUNC_SET: 
      status = defaultstatus;
      if( status ) goto blockui_enable;
      else goto blockui_disable;
      break;
  }
  
  return NULL;
}


void *cdr_uselegend(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          //ini_puts("SETTINGS", "CDR_HIDELEGEND", "FALSE", config);
          uselegend_disable:
          flag_use_legend = 1;
          showoptions = 13;
          status = 0;
        } else {
          //ini_puts("SETTINGS", "CDR_HIDELEGEND", "TRUE", config);
          uselegend_enable:
          flag_use_legend = 0;
          showoptions = 15;
          status = 1;
        }
      } break;
    case FUNC_SET: 
      status = defaultstatus;
      if( status ) 
        goto uselegend_enable;
      else goto uselegend_disable;
      break;
  }
  
  return NULL;
}


void *cdr_alternativefont(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          //ini_puts("SETTINGS", "CDR_ALTERCATFONT", "FALSE", config);
          cataltfont_disable:
          flag_use_cataltfont = 0;
          status = 0;  
              
        } else {
          //ini_puts("SETTINGS", "CDR_ALTERCATFONT", "TRUE", config);
          cataltfont_enable:
          flag_use_cataltfont = 1;
          status = 1;
          
        }
      } break;
    case FUNC_SET: 
      status = defaultstatus;
      if( status ) 
        goto cataltfont_enable;
      else goto cataltfont_disable;
      break;
  }
  
  return NULL;
}

#ifdef CONFIG
void *cdr_liveconfig(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          liveconfig_disable:
          flag_use_liveconfig = 0;
          status = 0;  
              
        } else {
          liveconfig_enable:
          flag_use_liveconfig = 1;
          status = 1;
          
        }
      } break;
    case FUNC_SET: 
      status = defaultstatus;
      if( status ) goto liveconfig_enable;
      else goto liveconfig_disable;
      break;
  }
  
  return NULL;
}
#endif

void *cdr_showpopular(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          category_index[CAT_ALIAS] = 0; // hide all CAT_ALIAS menu entries
          //ini_puts("SETTINGS", "CDR_SHOWCHEATSELECTION", "FALSE", config);
          status = 0;
        } else {
          category_index[CAT_ALIAS] = 1; // unhide all CAT_ALIAS menu entries
          //ini_puts("SETTINGS", "CDR_SHOWCHEATSELECTION", "TRUE", config);
          status = 1;
        }
      } break;
    case FUNC_SET: 
      status = category_index[CAT_ALIAS] = defaultstatus;
      break;
  }
  
  return NULL;
}

void *cdr_collapsecats(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int i;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          status = flag_coll_cats = 0;
          //ini_puts("SETTINGS", "CDR_COLLAPSEABLECATS", "FALSE", config);
          
          collapsecats_disable:
          
          // re-init category_index array
          for(i = 0; i < CAT_COUNTER; i++) 
            if( i != CAT_ALIAS ) // exclude "ALIAS cheats"
              category_index[i] = 1;
          
          //if( !DEBUG ) //hide debug again if not in debug mode
          //#ifndef DEBUG
          //  category_index[CAT_DEBUG] = 0;
          //#endif
          
        } else {
          status = flag_coll_cats = 1;
          //ini_puts("SETTINGS", "CDR_COLLAPSEABLECATS", "TRUE", config);
        }
      } break;
    case FUNC_SET: 
      status = flag_coll_cats = defaultstatus;
      if( !status ) goto collapsecats_disable;
      break;
  }
  
  return NULL;
}


void *cdr_autostartmenu(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: return (int*)status;
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          status = flag_menu_start = 0;
          //ini_puts("SETTINGS", "CDR_AUTOSTARTMENU", "FALSE", config);
        } else {
          status = flag_menu_start = 1;
          //ini_puts("SETTINGS", "CDR_AUTOSTARTMENU", "TRUE", config);
        }
      } break;
    case FUNC_SET: 
      status = flag_menu_start = defaultstatus;
      break;
  }
  
  return NULL;
}

#ifdef LANG

void *cdr_changelang(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static int lang = 0;

  switch( calltype ) {

    case FUNC_GET_STATUS: 
      return (int*)status;

    case FUNC_GET_VALUE: 
      return (int*)lang; 

    case FUNC_GET_STRING:
      if (main_file_table->size <= 0)
        return (void*)("None");
      else 
        return (void*)(main_file_table->lang_files[lang]->Language);

    case FUNC_CHANGE_VALUE:
      if ( keypress == PSP_CTRL_CROSS ) {
        update_lang(lang);
      } 

      else if( keypress == PSP_CTRL_LEFT && lang > 0) {
        lang--;
      }

      else if ( keypress == PSP_CTRL_RIGHT && lang < main_file_table->size-1 ) {
        lang++;
      } 
      
      break;
    
    case FUNC_SET:
      status = defaultstatus;


      if ( defaultval >= 0 && defaultval <= LANG_FILES_LIMIT-1 ) {
        lang = defaultval;
      } else {
        lang = 0;
      }
      LanguageConfigStart = lang;
      break;
  }

  return NULL;
}

#endif

void load_defaults(const Menu_pack *menu_list, int menu_max) { // set all cheats to default value (values from main_menu_sp)
  #ifdef LOG
  logPrintf("[INFO] %i: load_defaults()", getGametime());
  #endif  
  
  static int i;
  void (* func)();
  for( i=0; i < menu_max; i++ ) {
    func = (void *)(menu_list[i].value);
    if( menu_list[i].conf_id != 0 && menu_list[i].def_stat != -1 ) {
      #ifdef LOG
      logPrintf("[DEFAULT] %i: for: '0x%04X'", getGametime(), menu_list[i].conf_id);
      #endif  
      func( FUNC_SET, menu_list[i].cat, menu_list[i].def_stat, 0xDEADBEEF); // set def_stat from Menu_pack  -> the cheats have to reset themself to default value
      ///arg2: is keypress but not used by FUNC_SET -> we use it for categories)
      ///arg4: is default_value of cheat but is set inside FUNC_SET if necessary
    }
  }

}


void exit_game() {
  #ifdef LOG
  logPrintf("[INFO] %i: exit_game()", getGametime());
  #endif
  closeMenu(); // in case flag_use_liveconfig is true
  sceKernelExitGame();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void bttncht_givehealth() {
  if( LCS ) activateCheatCode(L, R, CROSS, L, R, SQUARE, L, R);
  if( VCS ) activateCheatCode(UP, DOWN, LEFT, RIGHT, CIRCLE, CIRCLE, L, R);
}
void bttncht_givearmor() {
  if( LCS ) activateCheatCode(L, R, CIRCLE, L, R, CROSS, L, R);
  if( VCS ) activateCheatCode(UP, DOWN, LEFT, RIGHT, SQUARE, SQUARE, L, R);
}
void bttncht_givemoney() { // int calltype, int keypress, int defaultstatus
  
  /* switch( calltype ) {
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {*/
        if( LCS ) activateCheatCode(L, R, TRIANGLE, L, R, CIRCLE, L, R);
        if( VCS ) activateCheatCode(UP, DOWN, LEFT, RIGHT, CROSS, CROSS, L, R);
  
      /*} else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXEDITOR  
        hex_marker_clear();  
        
        hex_marker_addx(global_buttoninput + (LCS ? 0x95 : 0xA9), sizeof(int));
        hex_marker_addx(global_buttoninput + (LCS ? 0x95 : 0xA9) + 0x4 , sizeof(int));

        hexeditor_create(global_buttoninput + (LCS ? 0x95 : 0xA9), 1, memory_low, memory_high, "> global_buttoninput");
        #endif
      }
      break;
  } */
        
}

void bttncht_weaponset1() {
  if( LCS ) activateCheatCode(UP, SQUARE, SQUARE, DOWN, LEFT, SQUARE, SQUARE, RIGHT);
  if( VCS ) activateCheatCode(LEFT, RIGHT, CROSS, UP, DOWN, SQUARE, LEFT, RIGHT);
}

void bttncht_weaponset2() {
  if( LCS ) activateCheatCode(UP, CIRCLE, CIRCLE, DOWN, LEFT, CIRCLE, CIRCLE, RIGHT);
  if( VCS ) activateCheatCode(LEFT, RIGHT, SQUARE, UP, DOWN, TRIANGLE, LEFT, RIGHT);
}

void bttncht_weaponset3() {
  if( LCS ) activateCheatCode(UP, CROSS, CROSS, DOWN, LEFT, CROSS, CROSS, RIGHT);
  if( VCS ) activateCheatCode(LEFT, RIGHT, TRIANGLE, UP, DOWN, CIRCLE, LEFT, RIGHT);
}

void bttncht_carsonwater() {
  if( LCS ) activateCheatCode(CIRCLE, CROSS, DOWN, CIRCLE, CROSS, UP, L, L);
}

void bttncht_blowupcars() {
  if( LCS ) activateCheatCode(L, L, LEFT, L, L, RIGHT, CROSS, SQUARE);
  if( VCS ) activateCheatCode(L, R, R, LEFT, RIGHT, SQUARE, DOWN, R);
}

void bttncht_perftraction() {
  if( LCS ) activateCheatCode(L, UP, LEFT, R, TRIANGLE, CIRCLE, DOWN, CROSS);
  if( VCS ) activateCheatCode(DOWN, LEFT, UP, L, R, TRIANGLE, CIRCLE, CROSS);
}

void bttncht_unlockmult100() {
  if( LCS ) activateCheatCode(UP, UP, UP, TRIANGLE, TRIANGLE, SQUARE, L, R );
  if( VCS ) activateCheatCode(UP, UP, UP, TRIANGLE, TRIANGLE, CIRCLE, L, R );
}

void bttncht_spawnrhino() {
  if( LCS ) activateCheatCode(L, L, LEFT, L, L, RIGHT, TRIANGLE, CIRCLE);
  if( VCS ) activateCheatCode(UP, L, DOWN, R, LEFT, L, RIGHT, R);
}

void bttncht_playcredits() {
  if( LCS ) activateCheatCode(L, R, L, R, UP, DOWN, L, R);
}
/* void bttncht_randomplayer() {
  if( LCS ) activateCheatCode(L, L, LEFT, L, L, RIGHT, SQUARE, TRIANGLE);
} */
void *bttncht_randomplayer(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int model = -1;
  
  switch( calltype ) {
    case FUNC_GET_VALUE: // for config
      return (int*)model; 
    
    case FUNC_GET_STRING: 
      if( model == -1 ) {
        sprintf(retbuf, "Random");
      } else {
        sprintf(retbuf, "%d", model);
        #ifdef NAMERESOLV
      sprintf(retbuf, "%d (%s)", model, getModelNameViaID(model, -1));
      #endif
      } return (void *)retbuf;
            
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && model > -1) { // LEFT
        model--;
    if( model == 8 ) model = 7; // skip crashing
    if( model == 78 ) model = 68;
    
      } else if( keypress == PSP_CTRL_RIGHT && model < 104) { // RIGHT
        model++;
    if( model == 8 ) model = 9; // skip crashing
    if( model == 69 ) model = 79;
    
      } else if( keypress == PSP_CTRL_CROSS ) { // CROSS
        activateCheatCode(L, L, LEFT, L, L, RIGHT, SQUARE, TRIANGLE);
      
      } else if( keypress == PSP_CTRL_CIRCLE ) { // CIRCLE
        model = -1; // random
      
      } else if( keypress == PSP_CTRL_SQUARE ) {
        // todo - unstuck player
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(addr_randompedcheat, sizeof(int));
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(addr_randompedcheat, 0, memory_low, memory_high, "");
        #endif
      } 
    setRandomPedCheat(model);
      break;
      
    case FUNC_SET: 
      if( defaultval >= 0 && defaultval <= 104 ) { // todo
        model = defaultval;
    setRandomPedCheat(model);
      } else model = -1;
      break;
  }
  
  return NULL;
}

void bttncht_bubblehead() {
  if( LCS ) activateCheatCode(DOWN,DOWN,DOWN,CIRCLE,CIRCLE,CROSS,L,R);
}


void bttncht_pedsweapons() { 
  if( LCS ) activateCheatCode(R, R, L, R, R, L, RIGHT, CIRCLE);
  if( VCS ) activateCheatCode(UP, L, DOWN, R, LEFT, CIRCLE, RIGHT, TRIANGLE);
}

void bttncht_pedsattack() { 
  if( LCS ) activateCheatCode(L, L, R, L, L, R, UP, TRIANGLE);
  if( VCS ) activateCheatCode(DOWN, TRIANGLE, UP, CROSS, L, R, L, R);
}

void bttncht_pedsriot() {
  if( LCS ) activateCheatCode(L, L, R, L, L, R, LEFT, SQUARE);
  if( VCS ) activateCheatCode(R, L, L, DOWN, LEFT, CIRCLE, DOWN, L);
}

void bttncht_agrodrivers() {
  if( LCS ) activateCheatCode(SQUARE, SQUARE, R, CROSS, CROSS, L, CIRCLE, CIRCLE);
  if( VCS ) activateCheatCode(UP, UP, RIGHT, LEFT, TRIANGLE, CIRCLE, CIRCLE, SQUARE);
}

void bttncht_traffichrome() {
  if( LCS ) activateCheatCode(TRIANGLE, R, L, DOWN, DOWN, R, R, TRIANGLE);
  if( VCS ) activateCheatCode(RIGHT, UP, LEFT, DOWN, TRIANGLE, TRIANGLE, L, R);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/** gamespeed ************************************************************************************
 *
 * Completion:   
 * 
 * Todo:     
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *gamespeed(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static float speed = 1.0f; // game default
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*) (int)(speed*10.0f); 
    
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%.1f", speed);
      return (void *)retbuf;
      
    case FUNC_CHECK:
      if(!status) 
        speed = getGamespeed();
      break;
      
    case FUNC_APPLY: 
      setGamespeed(speed);
      break;
      
    case FUNC_CHANGE_VALUE:
      if ( keypress == PSP_CTRL_LEFT && speed > 0.1f ) { // LEFT
        speed -= 0.1f;
        status = 1;
        
      } else if ( keypress == PSP_CTRL_RIGHT && speed < 3.9f ) { // RIGHT
        speed += 0.1f;
        status = 1;
        
      } else if ( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
      
      } else if ( keypress == PSP_CTRL_CIRCLE ) { // CIRCLE
        status = 0;
        setGamespeed(1.0f);
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS 
        hex_marker_clear();
        hex_marker_addx(global_timescale + gp_, sizeof(float));
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(global_timescale + gp_, 0, memory_low, memory_high, "> timescale");
        #endif
      } 
      
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( defaultval > 0 && defaultval <= 40 ) {
        speed = (float)((float)(defaultval)/10.0f);
      } else speed = 1.0f;
      break;
  }
  
  return NULL;
}


/** Limit the "Frames per second" to a preset value ************************************************************************************
 *
 * Completion:   Working
 * 
 * Todo:     /
 * 
 * Notes:     Example:   60 / 2 = 30 FPS
 **************************************************************************************************************************************/
void *fps_cap(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static int i = 2; // 30 FPS (game default)
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*)i;
      
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%i", 60 / i); // 60:3 = 20 FPS
      return retbuf;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          fpscap_disable:
          if( addr_fpsCap > 0 ) {
            _sh(2, addr_fpsCap); 
            clearICacheFor(addr_fpsCap);
          }
          status = 0;
          break;
        } else { // enable
          fpscap_enable: 
          if( addr_fpsCap > 0 ) {
            _sh(i, addr_fpsCap);
            clearICacheFor(addr_fpsCap);
          } 
          status = 1;
          break;
        }
      } else if( keypress == PSP_CTRL_LEFT && i < 7 ) { // LEFT
        i++;
        if( status ) goto fpscap_enable;
      } else if( keypress == PSP_CTRL_RIGHT && i > 1 ) { // RIGHT
        i--;
        if( status ) goto fpscap_enable;
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS 
        hex_marker_clear();
        hex_marker_addx(addr_fpsCap, sizeof(short)); // hour value
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(addr_fpsCap, 0, memory_low, memory_high, "> fps cap");
        #endif
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( defaultval > 0 && defaultval < 60 ) 
        i = defaultval;
      else // fix for bad config value
        i = 2;
      
      if( addr_fpsCap != -1 ) {
        if( status ) goto fpscap_enable;
        else goto fpscap_disable;
      }
      
      break;
  }
  
  return NULL;
}


/** Never fall off bikes **********************************************************************************************************
 *
 * Completion:   96% :|
 * 
 * Todo:     - maybe check in noped game function for pplayer vs pped -> no need for FUNC_APPLY block
 * 
 * Notes:     - Original cheat had 1 more nop'ed memory location though?
 *         - When you are on bike other bike peds can't fall off either! 
          -> whole FUNC_APPLY block is only to re-enable checks for traffic bikr peds when you are on foot or on none-bike
 *********************************************************************************************************************************/
void *neverfalloffbike(int calltype, int keypress, int defaultstatus) {
  static int status;
  static u32 backup = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_APPLY: // see notes
      if( pcar && pcar_type == VEHICLE_BIKE )  {
        if( backup == 0 ) backup = _lw(addr_neverFallOffBike_rollback); // backup jal reference for restoring
        _sw(0x00000000, addr_neverFallOffBike_rollback); // don't fall off when rolling backwards
        _sw(0x00000000, addr_neverFallOffBike_hitobj); // don't fall off bike when hitting object
      } else {
        if( backup != 0) {
          _sw(backup, addr_neverFallOffBike_rollback); // fall off when rolling backwards
          _sw(backup, addr_neverFallOffBike_hitobj); // fall off bike when hitting object
        }
      } 
      clearICacheFor(addr_neverFallOffBike_rollback);
      clearICacheFor(addr_neverFallOffBike_hitobj);
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          neverfalloffbike_disable:
          if( backup != 0 ) {
            _sw(backup, addr_neverFallOffBike_rollback); // fall off when rolling backwards
            _sw(backup, addr_neverFallOffBike_hitobj); // fall off bike when hitting object
          }
          status = 0;
        } else { // enable
          neverfalloffbike_enable:
          if( backup == 0 ) backup = _lw(addr_neverFallOffBike_rollback); // backup jal reference for restoring
          _sw(0x00000000, addr_neverFallOffBike_rollback); // don't fall off when rolling backwards
          _sw(0x00000000, addr_neverFallOffBike_hitobj); // don't fall off bike when hitting object
          status = 1;
        }
        clearICacheFor(addr_neverFallOffBike_rollback);
        clearICacheFor(addr_neverFallOffBike_hitobj);
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( status ) goto neverfalloffbike_enable;
      else goto neverfalloffbike_disable;
      break;
  }
   
  return NULL;
}


/** Teleport Player or Player's Vehicle to preset locations ********************************************************************************************
 *
 * Completion:   100% :)
 * 
 * Todo:     - 
 *        -
 * 
 * Notes:     Because vehicles don't like teleportation there is an aditionl fix needed to "calm down" momentum values. see "teleportFixForVehicle()"
 ******************************************************************************************************************************************************/
void *teleporter(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int i = 0;
  
  switch( calltype ) {
    
    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return (void *)(translate_string(LCS ? lcs_teleports[i].name : vcs_teleports[i].name));
      
    case FUNC_CHANGE_VALUE:
      if ( keypress == PSP_CTRL_LEFT && i > 1 ) { // LEFT
        i--;
        
      } else if ( keypress == PSP_CTRL_RIGHT ) { // RIGHT
        if( LCS && i < lcs_teleportersize ) i++;
        if( VCS && i < vcs_teleportersize ) i++;
        
      } else if ( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( LCS ) teleport(lcs_teleports[i].xval, lcs_teleports[i].yval, lcs_teleports[i].zval);
        if( VCS ) teleport(vcs_teleports[i].xval, vcs_teleports[i].yval, vcs_teleports[i].zval);
        closeMenu();
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS 
        hex_marker_clear();  
        hex_marker_addx(pobj+0x30, sizeof(float));
        hex_marker_addx(pobj+0x34, sizeof(float));
        hex_marker_addx(pobj+0x38, sizeof(float));
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(pobj+0x30, 0, memory_low, memory_high, "> world xyz coord");
        #endif
      }
      break;
      
    case FUNC_SET:
      if( defaultval >= 0 && i <= (LCS ? lcs_teleportersize : vcs_teleportersize) ) 
        i = defaultval;
      else   
        i = LCS ? 26 : 27; // default teleport location in list

      break;
  }  

  return NULL;
}



/**  ********************************************************************************************
 *
 * Completion:   
 * 
 * Todo:     - 
 *        -
 * 
 * Notes:   

 LCS https://gtamods.com/wiki/LCS/VCS_SCM
 
 start + 0xB has 0x43EC 
    + 0xB has 0x6426
     + 0x18 = total_missions
 
 INITOB @ 0x41EBB
 INITHP @ 0x42769
 TAXIDR @ 0x4AF25
 
 0x643E total Missions 118
 0x6442 mission offsets
  for missions; ++4
    0 -> 0x041EB3 + 8 -> INITOB
    10 -> 0x04AF1D + 8 -> TAXIDR
      if there is opcode name_thread 0x03A9
        read name
      else
        unnamend thread
 
  DEBMENU 'threads

  LEVSKIP
  CVIEW
  BVIEW
  WEAPCH
  PLAYCH
  WETIME
  MOCAPS
  ANIMS
 
 ******************************************************************************************************************************************************/

int toValue(char *str) {
  return (int)strtol(str, NULL, 0);
}

char * readMissionNameFromSCM(int mission_number) {
  static int previous = -1;
  static char ret[8] = "UNSET";
  unsigned char buf[256] = "";
  char valstr[16] = "";
  int i, missions = 0, offset = 0;
  
  if( previous == mission_number ) //
    return ret;
  
  FILE *fp = NULL;
  if( ( fp = fopen(LCS ? "disc0:/PSP_GAME/USRDIR/DATA/MAIN.SCM" : "disc0:/PSP_GAME/USRDIR/RUNDATA/MAIN.SCM", "rb")) == NULL ) {
    sprintf(ret, "ERROR");
  }

  offset = 0; // first chunk address
  fseek(fp, offset, SEEK_SET); // got to chunk
  fread(&buf, 0x10, 1, fp); // read in header
  if( buf[0x8] != 0x02 || buf[0xA] != 0x06 ) { // confirm syntax
    sprintf(ret, "ERROR");
    goto msngetexit;
  } sprintf(valstr, "0x%02X%02X%02X", buf[0xD], buf[0xC], buf[0xB]); // next chunk address
  
  offset = toValue(valstr); 
  fseek(fp, offset, SEEK_SET); // got to chunk
  fread(&buf, 0x10, 1, fp); // read in header
  if( buf[0x8] != 0x02 || buf[0xA] != 0x06 ) { //confirm syntax
    sprintf(ret, "ERROR");
    goto msngetexit;
  } sprintf(valstr, "0x%02X%02X%02X", buf[0xD], buf[0xC], buf[0xB]); // next chunk address 
  
  offset = toValue(valstr); 
  fseek(fp, offset, SEEK_SET); // got to chunk
  fread(&buf, 0x10, 1, fp); // read in header
  if( buf[0x8] != 0x02 || buf[0xA] != 0x06 ) { // confirm syntax
    sprintf(ret, "ERROR");
    goto msngetexit;
  }
  
  /// read in total Missions
  offset = offset + 0x10 + 8; // headersize + two other values
  fseek(fp, offset, SEEK_SET);
  fread(&buf, 4, 1, fp); // read in header
  sprintf(valstr, "0x%02X%02X%02X%02X", buf[0x3], buf[0x2], buf[0x1], buf[0x0]); // next chunk address 
  missions = toValue(valstr); 
  //logPrintf("total missions: %i", missions);
  
  
  if( mission_number >= missions ) {
    sprintf(ret, "ERROR");
    goto msngetexit;
  }
  
  /// loop mission offsets
  offset+=0x4; // first offset to mission
  for( i = 0; i < missions; i++, offset+=0x4 ) {
    if( i == mission_number ) {
      fseek(fp, offset, SEEK_SET);
      fread(&buf, 4, 1, fp); // read in header
      sprintf(valstr, "0x%02X%02X%02X%02X", buf[0x3], buf[0x2], buf[0x1], buf[0x0]); //next chunk address 
      //logPrintf("offset for mission %i: 0x%08X", mission_number, toValue(valstr));
      break;
    }
  }
  
  /// retrieve thread name if possible
  offset = toValue(valstr) + 8;
  fseek(fp, offset, SEEK_SET);
  fread(&buf, 256, 1, fp); // read in some script data (256 should be enough)
  sprintf(ret, "NONAME");
  for( i = 0; i < 256; i++ ) { // search the first instructions for name_thread
    if( buf[i] == (LCS ? 0xA9 : 0x38) && buf[i+1] == (LCS ? 0x03 : 0x02) ) { // name_thread opcode
      sprintf(ret, "%s", getString((int)&buf[i+2], 0)); // ugly but I was lazy
      //sprintf(ret, "%s", "DONE");
      break;
    }
  }
  
  msngetexit:
  fclose(fp);
  return ret;
}

#ifdef PREVIEW
void *mission_select(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int i = 0;
  
  switch( calltype ) {
    
    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%d - %s", i, readMissionNameFromSCM(i));
      //sprintf(retbuf, "%d", i);
      return (void*)retbuf;  
      
    case FUNC_CHANGE_VALUE:
      if ( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        
      } else if ( keypress == PSP_CTRL_RIGHT ) { // RIGHT
        i++;
        //todo get missions number
                
      } else if ( keypress == PSP_CTRL_CROSS ) { // CROSS
        /*
        LCS
        
        NONAME_21_3285() progress story?
        
        NONAME_21_2757() teleport to corona?
        
        */
          //03EB: clear_help 
          //00BE: clear_prints 
          //03F0: clear_small_prints 
          
          
          //0x38, 0x00, 0xCE, 0x30, 0x01,      ONMISSION = 1   `?
          
          
        static u8 script_startmission[] = {  // must be static for CustomScriptExecut()!

          /// load_and_launch_mission_internal
          0x1C, 0x04, 
          0x07, // byte
          0x3F, // mission_number
          
          /// terminate_this_script
          0x4E, 0x00
        };
        
        *(char*)&script_startmission[3] = (char)i;
        if( VCS ) {
          writeShort(&script_startmission[0], 0x0289);
          writeShort(&script_startmission[4], 0x0023);
        }
      
        
        //0289: start_mission 62
        //0023: end_thread 
        /*u8 script_startmission[] = { 
          
          ///load_and_launch_mission_internal
          0x89, 0x02, 
          0x07, //byte
          0x3C, //mission_number
          
          ///terminate_this_script
          0x23, 0x00
        };*/
        
        CustomScriptExecute((int)&script_startmission); // make game execute it
        
        closeMenu();
      
      } 
      break;
      
    case FUNC_SET:
      if( defaultval >= 0 ) 
        i = defaultval;
      else   
        i = 0;

      break;
  }  

  return NULL;
}
#endif


void mission_selector() {   
  closeMenu();

  static u8 script[10];
  int pos = 0, temp = 0, address = 0;
  int main_size = getInt(global_MainScriptSize + (LCS ? 0 : gp));
  int script_space = getInt(global_ScriptSpace + (LCS ? 0 : gp));
  for(address = script_space; address < script_space + main_size; address++) {
    if( *(u8*)address == 'L' && *(u8*)(address+1) == 'E' ) { // first because faster
      if( strcmp("LEVSKIP", getString(address, 0)) == 0 ) {
        #ifdef LOG
        logPrintf("LEVSKIP found @ 0x%08X in mainscript", address - script_space);
        #endif

        temp = address - script_space - 2; // 2 is opcode length
        if( VCS ) temp--; // vcs has additional 0xA as string identifier
        address = (int)&temp; // fugglyyy

        /// Opcode
        script[pos++] = 0x02; 
        script[pos++] = 0x00;
    
        /// integer (4 Bytes)
        script[pos++] = 0x06;
    
        /// int value / address in main script
        script[pos++] = *(unsigned char*)address;
        script[pos++] = *(unsigned char*)(address+1);
        script[pos++] = *(unsigned char*)(address+2);
        script[pos++] = *(unsigned char*)(address+3);
        
        CustomScriptExecute((int)&script); // make game execute it
        return;
      }
    }
  }
  setTimedTextbox("Error: LEVSKIP not found?!", 7.00f); // error
}


//void *load_building(int calltype, int keypress, int defaultstatus, int defaultval) {
//  static int i = 0;
  
  /*
  04C0: set_area_visible 1 

  04FE: set_extra_colours 14 fade 0 
  
  03BB: swap_nearest_building_model -380.597 311.117 75.141 radius 50.0 from #NEW_COLMANSN to #IZZY_DONMANROOM 
  
  0368: set_visibility_of_closest_object_of_type 1126.642 -1124.689 14.299 radius 50.0 model #LODEHOUSE3Z visibility 1 
  
  0635: swap_building 20 1@ 
  
  03D0: load_scene -380.597 311.117 75.141 

  --------------
  
  03BB: swap_nearest_building_model 1126.642 -1124.689 14.299 radius 50.0 from #WAREHOUSE3Z to #IZ_VIN 
  0368: set_visibility_of_closest_object_of_type 1126.642 -1124.689 14.299 radius 50.0 model #LODEHOUSE3Z visibility 0 
  03D0: load_scene 1126.642 -1124.689 14.299 

  03BB: swap_nearest_building_model 1126.642 -1124.689 14.299 radius 50.0 from #IZ_VIN to #WAREHOUSE3Z 
  0368: set_visibility_of_closest_object_of_type 1126.642 -1124.689 14.299 radius 50.0 model #LODEHOUSE3Z visibility 1 
  03D0: load_scene 1126.642 -1124.689 14.299 
  */  
  
  /**
  IDE:
  WAREHOUSE3Z = 974
  IZ_VIN = 3888
  
  
  swap_building:  
  1
  ...
  19 = Marias Apartment
  20 = Vincenzos Warehouse
  21 = Sals Office
  ...
  32 = 
  
  SCM TYPEs:
  04 = 2 Bytes upper part of float
  05 = 
  06 = 
  07 = 1 Byte
  08 = 2 Bytes
  09 = 4 Bytes
  
  **/

/*  typedef struct { ///buildings struct
    short no;
    char *name;
    short value;
    float x;
    float y;
    float z;
    float ide_x;
    float ide_y;
    float ide_z;
    short ide_on;
    short ide_off;
    short ide_lod;
  } buildings_pack;
*/
//  const buildings_pack lcs_buildings[] = { 
  ///    no  name            value  x      y      z      ide_x    ide_y    ide_z    ide_on              ide_off                ide_lod (visibility)
    
//    { 1, "Callahan Bridge",        1,    767.00f,  -920.0f,  40.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */    -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    
    //{ 2, "? Bridge",          1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */    -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    //{ 3, "? SUBWAYGATE something",  1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */    -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    //{ 4, "?",              1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */    -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    //{ 5, "Shoreside Tunnel Barrier",  1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */    -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    
//    {   6, "Portland Savehouse",    1,    1140.00f,  -244.0f,  23.50f,    1165.77f,  -248.814f,  19.875f,  /* TONISAPTINTERIOR1 */  3873,  /* IND_LAND097 */    1089  ,  /* NONE */        -1    }, // 
//    {   7, "Staunton Savehouse",    1,    272.00f,  -417.0f,  61.50f,    284.6f,    -425.463f,  45.11f,    /* STAUNTONSAFEHOUSE */  3889,  /* TENKB_BUILDS11 */  2174   ,  /* LODKB_BUILDS11 */  2018  }, // 
//    {   8, "Shoreside Savehouse",    1,    -838.00f,  292.0f,    42.00f,    -842.0f,  295.0f,    10.0f,    /* SHORESAFEHOUSE */  3884,  /* MANSION1 */      2794   ,  /* LODSION1 */      3113  }, // 
    
//    {   9, "The Dolls House",      1,    1030.00f,  -355.0f,  15.00f,    1023.427f,  -342.928f,  21.87f,    /* INDHIBUILD9B */    3763,  /* INDHIBUILD9 */    3761  ,  /* NONE */        -1    }, // 
    
//    {   10, "Drug Warehouse",      1,    990.00f,  -1105.0f,  13.50f,    985.0f,    -1100.0f,  10.0f,    /* DRUGWARE_MAINB */  3882,  /* DRUGWARE_MAIN */    3856  ,  /* NONE */        -1    }, // 
    
//    {   11, "JD's Club (Revue Bar)",  0,    916.00f,  -417.0f,  15.00f,    900.1f,    -421.0f,  14.35f,    /* LUIGICLUBOUT1 */    3754,  /* LUIGICLUBOUT1 */    3754   ,  /* NONE */    -1    }, // 
//    {   11, "JD's Club (Under constr.)",1,    916.00f,  -417.0f,  15.00f,    900.1f,    -421.0f,  14.35f,    /* LUIGICLUBOUT3 */    3756,  /* LUIGICLUBOUT1 */    3754   ,  /* NONE */    -1    }, // 
//    {   11, "JD's Club (Sex Club 7)",  2,    916.00f,  -417.0f,  15.00f,    900.1f,    -421.0f,  14.35f,    /* LUIGICLUBOUT2 */    3757,  /* LUIGICLUBOUT1 */    3754   ,  /* NONE */    -1    }, // 
    
//    {   12, "Fort Staunton",      1,    302.00f,  -413.0f,  68.50f,    0.00f,    0.0f,    0.00f,    /* LOREM */  -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    
//    {   13, "Church",          1,    -15.00f,  -1117.0f,  28.00f,    -28.992f,  -1102.797f,  53.216f,  /* IZZY_CHURCHINT */  3876,  /* CLNM_CTHDRLFCDE */  1540   ,  /* LODM_CTHDRLFCDE */  1796  }, // 
    
//    {   14, "Casino",          1,    1140.50f,  -423.70f,  20.50f,    1160.787f,  -439.627f,  32.147f,  /* JM_GAMBLEDEN */    3782,  /* JM_GAMBLEDENB */    3783   ,  /* NONE */        -1    }, // 
      
//    {   15, "Ship (none)",        0,    1443.00f,  -1159.0f,  12.00f,    1486.036f,  -1119.523f,  20.88f,    /* NULLSHIP */      3837,  /* NULLSHIP */      3837   ,  /* LODSHIP */      3842  }, // 
//    {   15, "Ship",            1,    1443.00f,  -1159.0f,  12.00f,    1486.036f,  -1119.523f,  20.88f,    /* BIGSHIP */      3836,  /* NULLSHIP */      3837   ,  /* LODSHIP */      3842  }, // 
//    {   15, "Ship (inside)",      2,    1493.00f,  -1117.0f,  6.00f,    1486.036f,  -1119.523f,  20.88f,    /* BIGSHIP_HOLD */    3875,  /* NULLSHIP */      3837  ,  /* LODSHIP */      3842  }, // 
    
//    {   16, "Donald's Building",    1,    286.0f,    -1405.0f,  190.50f,  241.985f,  -1444.096f,  27.665f,  /* DONLOVE_BUILDING */  3851,  /* CSKY_SKYSCRP26B */  2146   ,  /* LODY_SKYSCRP26B */  2106  }, // 
    
//    {   17, "Portland Strike Banners",  1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */  -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    //{ 18, "Hangar Jet",        1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */  -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    
    
//    {   19, "Maria's Apartment",    1,    1110.00f,  -478.00f,  33.00f,    1071.592f,  -439.308,  26.933f,   /* IZZY_MARIAAPTINT */  3886,  /* IND_MAINTEN3 */     909   ,  /* LOD_MAINTEN3 */     681   }, // MARIA_NULL
//    {   20, "Vincenzo's Warehouse",    1,    1127.00f,  -1099.50f,  12.00f,    1126.642f,  -1124.689f,  14.299f,  /* IZ_VIN */      3888,  /* WAREHOUSE3Z */    974   ,  /* LODEHOUSE3Z */    796   }, // 
//    {   21, "Salvatore's Office",    1,    1481.00f,  -180.00f,  56.00f,    1457.589f,  -184.383f,  60.145f,  /* IZZY_SALOFFICE */  3885,  /* FRANKSCLB02 */    926   ,  /* LODNKSCLBASE */    696    }, // 
//    {   22, "Shoreside Flophouse",    1,    -1012.00f,  -161.00f,  61.50f,    -1009.557f,  -161.335f,  63.531f,  /* IZ_SHRSDFLPHS */   3868,  /* DOC_SHEDBIG12 */    2866   ,  /* LOD_SHEDBIG12 */    2926  }, // 
//    {   23, "Donald's Office",      1,    235.50f,  -1540.75f,  130.00f,  232.323f,  -1539.983f,  130.0f,    /* IZZY_DONOFFICE */  3871,  /* NULLY_DONOFFICE */  3872   ,  /* NONE */        -1    }, // 
//    {   24, "Toshiko's Apartment",    1,    393.00f,  -1357.00f,  225.00f,  356.013f,  -1356.85f,  25.128f,  /* IZZY_TOSHAPT */    3890,  /* CSKY_SKYSCRP23 */  2166   ,  /* LODY_SKYSCRP23 */  1879  }, // 
//    {   25, "Shoreside Police Room",  1,    -1241.00f,  -42.00f,  59.00f,    -1273.622f,  -34.021f,  57.955f,  /* IZ_SHRSDPLCRM */    3869,  /* POLICE_STATION_SUB */3183   ,  /* NONE */        -1    }, //
//    {   26, "Donald's Mansion Room",  1,    -383.00f,  312.00f,  65.00f,    -380.597f,  311.117f,  75.141f,  /* IZZY_DONMANROOM */  3891,  /* NEW_COLMANSN */    2774   ,  /* NONE */        -1    }, //
//    {   27, "Sawmill",          1,    1171.00f,  -1032.00f,  15.00f,    1173.36f,  -1024.54f,  22.719f,  /* JM_SAWMILL_INSIDE */  4022,  /* SAWMILL */      1170   ,  /* LODMILL */      784    }, // 
    
    //{ 28, "?",            1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */  -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    //{ 29, "?",            1,    0.00f,    0.0f,    0.00f,    0.00f,    0.0f,    0.00f,    /* LOREM */  -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    
//    {   30, "Subway",          1,    339.00f,  -158.00f,  -2.50f,    0.00f,    0.0f,    0.00f,    /* LOREM */  -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    
//    {   31, "Gravestone Mayor",      1,    -72.50f,  -1149.0f,  26.50f,    -69.477f,  -1143.93f,  25.073f,  /* IZZY_GRVSTN_MAYOR */  4076,  /* NULLY_GRVSTN_MAYOR */ 4026,  /* NONE */    -1    }, // 
//    {   31, "Gravestone Paulie",    2,    -81.50f,  -1159.0f,  26.50f,    -86.854f,  -1156.676f,  24.963f,  /* IZZY_GRVSTN_PAULIE */4078,  /* NULLY_GRVSTN_PAULIE */4025,  /* NONE */    -1    }, // 
    
//    {   32, "Staunton Liftbanner",    1,    -63.00f,  -653.0f,  26.50f,    0.00f,    0.0f,    0.00f,    /* LOREM */  -1,  /* LOREM */    -1   ,  /* NONE */    -1    }, // 
    
/*  }; int lcs_buildingssize = (sizeof(lcs_buildings)/sizeof(buildings_pack))-1;

  u8 script_swap[] = { 
    ///0635: swap_building
    0x35, 0x06, 
    0x07, 0x00, //ID
    0x07, 0x1,  //1 ON / 0 OFF
  };
  
  u8 script_loadscene[] = { 
    ///03D0: load_scene
    0xD0 ,0x03,
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //x
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //y
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //z
  };
  
  u8 script_swapnearest[] = { 
    ///03BB: swap_nearest_building_model      
    0xBB ,0x03,
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //x
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //y
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //z
    0x04 ,0x16, 0x43 , //radius     (50 = 0x48, 0x42 | 150 = 0x16, 0x43)
    0x08 ,0x00 ,0x00 , //from #WAREHOUSE3Z
    0x08 ,0x00 ,0x00,  //to IDE (eg: IZ_VIN = #3888 = 0x0F30)
  };
  
  u8 script_visibility[] = { 
    ///0368: set_visibility_of_closest_object_of_type
    0x68 ,0x03,
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //x
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //y
    0x09 ,0x00 ,0x00 ,0x00 ,0x00 , //z
    0x04 ,0x16, 0x43 , //radius 
    0x08 ,0x00 ,0x00, //model IDE
    0x07 ,0x00,  //visibility 0 
  };
  
  u8 script_terminate[] = { 
    ///terminate_this_script
    0x4E, 0x00
  };

  switch( calltype ) {
    
    case FUNC_GET_VALUE: //for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%d - %s", lcs_buildings[i].no, lcs_buildings[i].name);
      return (void*)retbuf;  
      
    case FUNC_CHANGE_VALUE:
      if ( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        
      } else if ( keypress == PSP_CTRL_RIGHT && i < lcs_buildingssize ) { // RIGHT
        i++;

      } else if ( keypress == PSP_CTRL_CROSS ) { // CROSS
                  
        ///    ///    ///    ///    ///    ///
        
        CustomScriptClearBuffer(); //clear previous stuff
        
        *(char*)&script_swap[3] = (char)lcs_buildings[i].no;
        *(char*)&script_swap[5] = (char)lcs_buildings[i].value; 
        CustomScriptAdd(script_swap, sizeof(script_swap)/sizeof(script_swap[0]));
        
        if( lcs_buildings[i].ide_x != 0.0f && lcs_buildings[i].ide_on != -1 ) {
          writeFloat(&script_swapnearest[3], lcs_buildings[i].ide_x);
          writeFloat(&script_swapnearest[8], lcs_buildings[i].ide_y);
          writeFloat(&script_swapnearest[13], lcs_buildings[i].ide_z);
          writeShort(&script_swapnearest[21], lcs_buildings[i].ide_off);
          writeShort(&script_swapnearest[24], lcs_buildings[i].ide_on);
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));
        }
        
        if( lcs_buildings[i].no == 1 ) { //Callahan Bridge
          //todo
          setTimedTextbox("Todo: Collision swap", 5.00f);
        }
        
        if( lcs_buildings[i].no == 12 ) { //Fort Staunton
          //todo
          setTimedTextbox("Todo: Collision swap", 5.00f);
        }
        
        if( lcs_buildings[i].no == 30 ) { //SUBWAY
          
          writeFloat(&script_swapnearest[3], 210.0f);
          writeFloat(&script_swapnearest[8], -65.0f);
          writeFloat(&script_swapnearest[13], -4.35f);
          writeShort(&script_swapnearest[21], 4045); //#SUBWAYSECTION3_NUL
          writeShort(&script_swapnearest[24], 2102); //#SUBWAYSECTION3
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 271.744f);
          writeFloat(&script_swapnearest[8], -81.072f);
          writeFloat(&script_swapnearest[13], -4.35f);
          writeShort(&script_swapnearest[21], 4050); //#SUBWAYSECTION4_NUL
          writeShort(&script_swapnearest[24], 2157); //#SUBWAYSECTION4
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 267.143f);
          writeFloat(&script_swapnearest[8], -69.786f);
          writeFloat(&script_swapnearest[13], -4.333f);
          writeShort(&script_swapnearest[21], 4044); //#INTERSECTARCH_NUL
          writeShort(&script_swapnearest[24], 2095); //#INTERSECTARCHSS4
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 330.0f);
          writeFloat(&script_swapnearest[8], -74.737f);
          writeFloat(&script_swapnearest[13], -6.365f);
          writeShort(&script_swapnearest[21], 4046); //#JM_SUBSECTION5_NUL
          writeShort(&script_swapnearest[24], 2163); //#JM_SUBWAYSECTION5
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 370.0f);
          writeFloat(&script_swapnearest[8], -65.0f);
          writeFloat(&script_swapnearest[13], -4.35f);
          writeShort(&script_swapnearest[21], 4047); //#SUBWAYSECTION6_NUL
          writeShort(&script_swapnearest[24], 2162); //#SUBWAYSECTION6
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 410.0f);
          writeFloat(&script_swapnearest[8], -65.0f);
          writeFloat(&script_swapnearest[13], -4.35f);
          writeShort(&script_swapnearest[21], 4048); //#SUBWAYSECTION7_NUL
          writeShort(&script_swapnearest[24], 2096); //#SUBWAYSECTION7
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));
 
          writeFloat(&script_swapnearest[3], 321.491f);
          writeFloat(&script_swapnearest[8], -144.571f);
          writeFloat(&script_swapnearest[13], -0.757f);
          writeShort(&script_swapnearest[21], 4043); //#SUBWAYSTATION3_NUL
          writeShort(&script_swapnearest[24], 2099); //#SUBWAYSTATION3
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 339.57f);
          writeFloat(&script_swapnearest[8], -163.573f);
          writeFloat(&script_swapnearest[13], -1.418f);
          writeShort(&script_swapnearest[21], 4042); //#RUBBLE1_NUL
          writeShort(&script_swapnearest[24], 2100); //#RUBBLE1
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 365.205f);
          writeFloat(&script_swapnearest[8], -199.129f);
          writeFloat(&script_swapnearest[13], -4.45f);
          writeShort(&script_swapnearest[21], 4041); //#SUBWAYSECTION2_NUL
          writeShort(&script_swapnearest[24], 2093); //#SUBWAYSECTION2
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 348.779f);
          writeFloat(&script_swapnearest[8], -250.0f);
          writeFloat(&script_swapnearest[13], -1.596f);
          writeShort(&script_swapnearest[21], 4040); //#SUB_MIDDLE_NUL
          writeShort(&script_swapnearest[24], 2165); //#SUB_MIDDLE
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 351.078f);
          writeFloat(&script_swapnearest[8], -306.744f);
          writeFloat(&script_swapnearest[13], -4.45f);
          writeShort(&script_swapnearest[21], 4049); //#SUBWAYSECTION1_NUL
          writeShort(&script_swapnearest[24], 2094); //#SUBWAYSECTION1
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 278.364f);
          writeFloat(&script_swapnearest[8], -346.425f);
          writeFloat(&script_swapnearest[13], -0.757f);
          writeShort(&script_swapnearest[21], 4039); //#SUBWAYSTATION1_NUL
          writeShort(&script_swapnearest[24], 2098); //#SUBWAYSTATION1
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));

          writeFloat(&script_swapnearest[3], 260.46f);
          writeFloat(&script_swapnearest[8], -355.346f);
          writeFloat(&script_swapnearest[13], -0.861f);
          writeShort(&script_swapnearest[21], 4038); //#RUBBLE2_NUL
          writeShort(&script_swapnearest[24], 2097); //#RUBBLE2
          CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));
          
        }
        
        if( lcs_buildings[i].ide_lod != -1 ) {
          writeFloat(&script_visibility[3], lcs_buildings[i].ide_x);
          writeFloat(&script_visibility[8], lcs_buildings[i].ide_y);
          writeFloat(&script_visibility[13], lcs_buildings[i].ide_z);
          writeShort(&script_visibility[21], lcs_buildings[i].ide_lod);
          *(char*)&script_visibility[24] = 0x00; //visibility = 0 
          CustomScriptAdd(script_visibility, sizeof(script_visibility)/sizeof(script_visibility[0]));
        }  
        
        writeFloat(&script_loadscene[3], lcs_buildings[i].ide_x);
        writeFloat(&script_loadscene[8], lcs_buildings[i].ide_y);
        writeFloat(&script_loadscene[13], lcs_buildings[i].ide_z);
        CustomScriptAdd(script_loadscene, sizeof(script_loadscene)/sizeof(script_loadscene[0]));
        
        CustomScriptAdd(script_terminate, sizeof(script_terminate)/sizeof(script_terminate[0]));
        
        CustomScriptExecute((int)&script_terminate); //make game execute it


        //closeMenu();
      
      } else if ( keypress == PSP_CTRL_CIRCLE ) { ///CIRCLE

        ///    ///    ///    ///    ///    ///
        
        CustomScriptClearBuffer(); //clear previous stuff
        
        *(char*)&script_swap[3] = (char)lcs_buildings[i].no;
        *(char*)&script_swap[5] = 0x0; //default
        CustomScriptAdd(script_swap, sizeof(script_swap)/sizeof(script_swap[0]));
        
        
        writeFloat(&script_swapnearest[3], lcs_buildings[i].ide_x);
        writeFloat(&script_swapnearest[8], lcs_buildings[i].ide_y);
        writeFloat(&script_swapnearest[13], lcs_buildings[i].ide_z);
        writeShort(&script_swapnearest[21], lcs_buildings[i].ide_on);
        writeShort(&script_swapnearest[24], lcs_buildings[i].ide_off);
        CustomScriptAdd(script_swapnearest, sizeof(script_swapnearest)/sizeof(script_swapnearest[0]));
        
        if( lcs_buildings[i].ide_lod != -1 ) {
          writeFloat(&script_visibility[3], lcs_buildings[i].ide_x);
          writeFloat(&script_visibility[8], lcs_buildings[i].ide_y);
          writeFloat(&script_visibility[13], lcs_buildings[i].ide_z);
          writeShort(&script_visibility[21], lcs_buildings[i].ide_lod);
          *(char*)&script_visibility[24] = 0x01; //visibility = 1 
          CustomScriptAdd(script_visibility, sizeof(script_visibility)/sizeof(script_visibility[0]));
        }  
        
        writeFloat(&script_loadscene[3], lcs_buildings[i].ide_x);
        writeFloat(&script_loadscene[8], lcs_buildings[i].ide_y);
        writeFloat(&script_loadscene[13], lcs_buildings[i].ide_z);
        CustomScriptAdd(script_loadscene, sizeof(script_loadscene)/sizeof(script_loadscene[0]));
        
        CustomScriptAdd(script_terminate, sizeof(script_terminate)/sizeof(script_terminate[0]));
        
        CustomScriptExecute(0); //make game execute it
        
        ///    ///    ///    ///    ///    ///
        
        //closeMenu();
      
      } else if ( keypress == PSP_CTRL_SQUARE ) { ///SQUARE

        ///    ///    ///    ///    ///    ///
        
        if( lcs_buildings[i].x != 0.00f ) {
          teleport(lcs_buildings[i].x, lcs_buildings[i].y, lcs_buildings[i].z);
        } else {
          if( lcs_buildings[i].ide_x != 0.00f )
            teleport(lcs_buildings[i].ide_x, lcs_buildings[i].ide_y, lcs_buildings[i].ide_z);
        }
        
        ///    ///    ///    ///    ///    ///
        
        closeMenu();
      
      } 
      break;
      
    case FUNC_SET:
      if( defaultval >= 0 ) 
        i = defaultval;
      else   
        i = 0;

      break;
  }  

  return NULL;
}*/


/** Skip both intro movies when starting the game *************************************************************************************
 *
 * Completion:   100% :D
 * 
 * Todo:     - 
 * 
 * Notes:     Toggleing cheat will automatically write to config.ini!
 **************************************************************************************************************************************/
void *skip_intros(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          //ini_puts("CHEATS", "CHEAT_SkipIntroMovies", "FALSE", config);
          status = 0;
        } else { // enable
          //ini_puts("CHEATS", "CHEAT_SkipIntroMovies", "TRUE", config);
          status = 1;
        }
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( status ) 
        MAKE_DUMMY_FUNCTION(addr_skipIntroMovie, 0);
        clearICacheFor(addr_skipIntroMovie);
        clearICacheFor(addr_skipIntroMovie+0x4);
      break;
  }
   
  return NULL;
}


/** debug messages on loadscreen ******************************************************************************************************
 *
 * Completion:   
 * 
 * Todo:     - only do the patching stuff when cheat enabled?
 * 
 * Notes:     Toggleing cheat will automatically write to config.ini!
 **************************************************************************************************************************************/
void *debug_loadscreens(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          //ini_puts("CHEATS", "CHEAT_DebugLoadscreens", "FALSE", config);
          status = 0;
        } else { // enable
          //ini_puts("CHEATS", "CHEAT_DebugLoadscreens", "TRUE", config);
          status = 1;
        }
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** Real random loadscreens for LCS and using multiplayer ones as well ****************************************************************
 *
 * Completion:   100%
 * 
 * Todo:     - 
 * 
 * Notes:     VCS's Multiplayer loadscreen are lower quality and look quite bad. 
        Furthermore VCS has random loadscreens already. And 12 singleplayer ones btw!! (LCS has 4 hardcoded only!)
 **************************************************************************************************************************************/
void *random_loadscreens(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          //ini_puts("CHEATS", "CHEAT_RandomLoadscreens", "FALSE", config);
          status = 0;
        } else { // enable
          //ini_puts("CHEATS", "CHEAT_RandomLoadscreens", "TRUE", config);
          status = 1;
        }
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** world textures **************************************************************************************************************
 *
 * Completion:  Its continuously removing texture-applying from loaded textures. It works but is ugly solution
 * 
 * Todo:  - How to get texture from ID or model ID ?????????
 *        - Understand everything better
 *        - VCS has additional AREA files
 * 
 * Notes:  https://translate.google.de/translate?hl=de&sl=ru&tl=en&u=http%3A%2F%2Fgtamodding.ru%2Fwiki%2FLVZ&prev=search
 *         https://translate.google.de/translate?hl=de&sl=ru&tl=en&u=http%3A%2F%2Fgtamodding.ru%2Fwiki%2FWRLD&prev=search&sandbox=1
 
All LCS:

   DAT_00357144  area:
        0 - 3        4 - 7        8 - B        C - F
  0x00    ?????        ?????        val        INT current Island / Subway = 4
  0x10  cur LVZ ptr to table  cur LVZ file    
  0x20
  0x30
  0x40

  0x66BDD0 .lvz (WRLD) load address (depending on island and area)
  + 0x20 = resource ID table (each entry is 2 pointers?! size 0x9588 bytes -> makes 4785 objects?! STAUNTON)

    + 0xB30  --#358--> yellow line street
   

  GTA3PSPHR.IMG stuff:
  if  DAT_00345294_CStreamingInfo + 0xC + (id * 0x14) == 0x1   then model is loaded!

  TextureList is loaded around here. First 2 ptrs forward & backwards Third ptr to tm2
  0x873B310
  .........
  0x875CB80

 * 
 **************************************************************************************************************************************/
void *disable_textures(int calltype, int keypress, int defaultstatus) {
  static int status, offs = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      
      /// WRLD textures (from eg: commer.lvz/.img)
      if( LCS ) offs = getInt(getInt(getInt(render) + 0x10)) + 0x4;
      if( VCS ) offs = getInt(getInt(getInt(render+gp) + 0x18)) + 0x4;
      while( 1 ) { // missing first ptr but whatever
        offs += (LCS ? 0x8 : 0xC); // VCS has additional counter Integer
        
        if( getInt(offs) == 0 ) // not visible so not loaded.. nothing here
          continue;
        
        if( isInMemBounds(getInt(offs)) ) { // if anything else found then list must have ended (fugly)
          if( getByte(getInt(offs)) == 0x1) 
            setByte(getInt(offs), 0x0);
        } else break;
      }

      
      /// Textures from GTA3PPSHR.IMG (ui, effects, peds, vehicles etc..) 
      /*offs = 0x873B310; //LCS ONLY
      while( getInt(offs) != 0 ) { //backwards following pointers
        if( getByte(offs + 0xC) == 0x1) 
          setByte(offs + 0xC, 0x0);
        offs = getInt(offs);
      }
      do { //forwards following pointers
        if( getByte(offs + 0xC) == 0x1) 
          setByte(offs + 0xC, 0x0);
      
        offs = getInt(offs + 0x4);
      
      } while( getInt(offs + 0x4) != 0 ); */
      
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        hex_marker_addx(render + gp_, sizeof(int)); // health value
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(render + gp_, 0, memory_low, memory_high, "> Texturetable of WRLD");
        #endif
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** Godmode **************************************************************************************************************
 *
 * Completion:  99% :)
 * 
 * Todo:  - 
 *        - bar is not completely full with such a huge value.. fixable?
 * 
 * Notes:     /
 **************************************************************************************************************************************/
void *godmode(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setPedHealthAndArmor(pplayer, 9999.00f, 9999.00f);
      setPedNeverOnFire(pplayer); //0 = true
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          setPedHealthAndArmor(pplayer, (float)((u8)getMaxHealthMultiplier()), (float)((u8)getMaxArmorMultiplier()));
          status = 0;
        } else status = 1;
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        hex_marker_addx(pplayer+(LCS ? 0x4B8 : 0x4E4), sizeof(float)); // health value
        hex_marker_addx(pplayer+(LCS ? 0x4BC : 0x4E8), sizeof(float)); // armor value
        hex_marker_addx(pplayer+(LCS ? 0x6DC : 0x6B0), sizeof(int));   // on-fire boolean
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(pplayer + (LCS ? 0x4B8 : 0x4E4), 0, memory_low, memory_high, "> pplayer health & armor");
        #endif
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** powerjump **************************************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:     /
 **************************************************************************************************************************************/
void *powerjump(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( !pcar && flag_menu_running == 0 ) {
        if( getByte(pplayer+(LCS?0x10A:136)) == 0 ) { // player not touching ground (better check todo?)
         
          /// calc current speed
          float speed = sqrt((getFloat(pplayer+(LCS?0x70:0x140)) * getFloat(pplayer+(LCS?0x70:0x140))) + (getFloat(pplayer+(LCS?0x74:0x144)) * getFloat(pplayer+(LCS?0x74:0x144)))); // SQRT( x^2 + y^2 )
          
          /// forward-thrust
          setFloat(pplayer+(LCS?0x70:0x140), -getFloat(pplayer+4) * speed );
          setFloat(pplayer+(LCS?0x74:0x144), getFloat(pplayer) * speed);
        
          /// up-thrust
          if( current_buttons & PSP_CTRL_SQUARE ) // SQUARE pressed
            setFloat(pplayer+(LCS?0x98:0x148), getFloat(pplayer+(LCS?0x98:0x148)) + 0.02f );
        
          /// turn 
          setFloat(pplayer+(LCS?0x88:0x78), (LCS?-0.02:-0.02) * xstick);
      
          /// set rolling after landing animation in the current direction
          setFloat(pplayer+(LCS?0x4E0:0x8D0), getFloat(pplayer+(LCS?0x4E4:0x8D4))); // continuously setting current dir
        }
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if ( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** Invisible **************************************************************************************************************
 *
 * Completion:  100%
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:     VCS is handled differently. Using the same bitflag (which is at 0x1CF btw) unloads the ped?!
 **************************************************************************************************************************************/
void *invisible(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setPedInvisible(pplayer, ON);
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          setPedInvisible(pplayer, OFF);
          status = 0;
        } else status = 1;
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS 
        hex_marker_clear();  
        if( LCS ) hex_marker_addx(pplayer+0x19A, sizeof(char));
        if( VCS ) hex_marker_addx(pplayer+0x4C, sizeof(short));
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(pplayer + (LCS ? 0x19A : 0x4C), 0, memory_low, memory_high, "> pplayer invisible");
        #endif
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}



/** ignored by everyone **************************************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - 
 *        - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *ignored(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setPedIgnoredByEveryone(pplayer, ON);
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          setPedIgnoredByEveryone(pplayer, OFF);
          status = 0;
        } else status = 1;
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #if defined(HEXMARKERS) || defined(HEXEDITOR)
        int temp = pplayer + (LCS ? 0x82E : 0x90E); 
        #endif
        #ifdef HEXMARKERS  
        hex_marker_clear();  
        hex_marker_addx(temp, sizeof(char));
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(temp, 0, memory_low, memory_high, "");
        #endif
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** warp_out_water **************************************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - 
 *        - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *warp_out_water(int calltype, int keypress, int defaultstatus) {
  static int timer = 0;
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( LCS && !pcar && checkPedIsInWater(pplayer) && (gametimer > timer) ) {
        setFloat(pplayer+0x38, -200.0f); //let game take care of spawn point
        timer = gametimer + 2000; // 2 sec
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          status = 0;
        } else status = 1;
      } /*else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXEDITOR  
        int temp = pplayer + 0x142; 
        hex_marker_clear();  
        hex_marker_addx(temp, sizeof(char));
        hexeditor_create(temp, 0, memory_low, memory_high, "");
        #endif
      }*/ break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}

/** warp_out_water_veh **************************************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - VCS.. inWater() doesn't return bool currently and is different for different vehicles
 *        - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *warp_out_water_veh(int calltype, int keypress, int defaultstatus) {
  static int timer = 0;
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar && (pcar_type != VEHICLE_BOAT) && (isVehicleInWater( pcar ) >= 1) && (gametimer > timer)) {
        setFloat(pcar+0x38, -200.0f); // let game take care of spawn point
        timer = gametimer + 2000; // 2 sec
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          status = 0;
        } else status = 1;
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** Unlimited Ammo & No reloading ********************************************************************************************************
 *
 * Completion:   80%
 * 
 * Todo:     - when reset -> set to correct ammo value
 *        - find function patch method to never loose ammo instead?
 *        - set default
 
        - use flag somehow?!: DAT_00385c75_playerFastReload (LCS)
 * 
 * Notes:     /
 **************************************************************************************************************************************/
void *unlimited_ammo(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setPedAmmoForWeapon(pplayer, WEAPON_GRENADE,     0, 99999); // Grenades
      setPedAmmoForWeapon(pplayer, WEAPON_HANDGUN, 10000, 99999); // Handgun
      setPedAmmoForWeapon(pplayer, WEAPON_SHOTGUN, 10000, 99999); // Shotgun
      setPedAmmoForWeapon(pplayer, WEAPON_SMG,     10000, 99999); // SMG
      setPedAmmoForWeapon(pplayer, WEAPON_ASSAULT, 10000, 99999); // Assault
      setPedAmmoForWeapon(pplayer, WEAPON_ROCKET,  10000, 99999); // Rocket & Minigun
      setPedAmmoForWeapon(pplayer, WEAPON_SNIPER,  10000, 99999); // Sniper
      setPedAmmoForWeapon(pplayer, WEAPON_CAMERA,  10000, 99999); // Photos
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 
        if(!status) { // reset default values once (these are not the default ones though TODO)
          setPedAmmoForWeapon(pplayer, WEAPON_GRENADE,   0, 200); // Grenades
          setPedAmmoForWeapon(pplayer, WEAPON_HANDGUN, 100, 200); // Handgun
          setPedAmmoForWeapon(pplayer, WEAPON_SHOTGUN, 100, 200); // Shotgun
          setPedAmmoForWeapon(pplayer, WEAPON_SMG,     100, 200); // SMG
          setPedAmmoForWeapon(pplayer, WEAPON_ASSAULT, 100, 200); // Assault
          setPedAmmoForWeapon(pplayer, WEAPON_ROCKET,  100, 200); // Rocket & Minigun
          setPedAmmoForWeapon(pplayer, WEAPON_SNIPER,  100, 200); // Sniper
          setPedAmmoForWeapon(pplayer, WEAPON_CAMERA,  100, 200); // Photos
        }
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** heavy player - so you can push cars etc ********************************************************************************************
 *
 * Completion:   100%
 * 
 * Todo:     - jumping fix? maybe disable jumping completely?
 *        - 
 * 
 * Notes:     setPedMass(pplayer, 70, 100); is default
 **************************************************************************************************************************************/
void *heavy_player(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int pplyrbckp = 0;
  static int vehicleflag = 0;
  static float backup1 = 0;
  static float backup2 = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_APPLY:
      if( pplayer && backup1 == 0 ) { // set once
        pplyrbckp = pplayer;
        backup1 = getFloat(pplayer+0xD0);
        backup2 = getFloat(pplayer+0xD4);
        if( backup1 < 10000 ) setPedMass(pplayer, backup1*10000, backup2*10000);
      } 
      /// fix for weird vehicle momentum gaining behaviour with high player mass
      if( getPedsCurrentAnimation(pplayer) == 0x12 && vehicleflag == 0 && backup1 != 0 ) { // disable while entering vehicle
        setPedMass(pplayer, backup1, backup2);
        vehicleflag = 1;
      } 
      if( vehicleflag == 1 && getPedsCurrentAnimation(pplayer) != 0x12 && backup1 != 0 ) {
        setPedMass(pplayer, backup1*10000, backup2*10000); // re-enable after vehicle entered
        vehicleflag = 0;
      }
      break;
    
    case FUNC_CHECK:
      if( pplyrbckp != pplayer && pplayer > 0 ) { // new pplayer object loaded -> let apply take care of re-setting 
        backup1 = 0; backup2 = 0; pplyrbckp = 0; vehicleflag = 0;
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // disable
          heavyplayer_disable:
          if( pplayer == pplyrbckp && backup1 > 0) { // if backups were made -> set them back
            setPedMass(pplayer, backup1, backup2);  
            backup1 = 0; backup2 = 0; pplyrbckp = 0; vehicleflag = 0;
          }  
          status = 0;
        } else status = 1;
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS  
        hex_marker_clear();  
        hex_marker_addx(pplayer+0xD0, sizeof(float));
        hex_marker_addx(pplayer+0xD4, sizeof(float));
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(pplayer+0xD0, 0, memory_low, memory_high, "> pplayer mass");
        #endif
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( !status ) 
        goto heavyplayer_disable; 
      break;
  }
   
  return NULL;
}


/** heavy vehicle - so you can push other cars etc ********************************************************************************************
 *
 * Completion:  90 :)
 * 
 * Todo:  - fix vehicle fall through water (isVehicleInWater() doesn't work)
 *        - disable when vehicle on roof (isVehicleUpsideDown( pcar ) && !isVehicleInAir( pcar ) )
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *heavy_vehicle(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int lastcar = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( !pcar && lastcar ) { // not in car anymore -> reset old car's stats
        setFloat(lastcar + 0xD0, getFloat(lastcar+0xD0)/1000);
        setFloat(lastcar + 0xD4, getFloat(lastcar+0xD4)/1000);
        lastcar = 0;
      }
      if (pcar && !lastcar) { // in car -> this will be applied only once!!
        lastcar = pcar;
        if( pcar_type != VEHICLE_BOAT ) { // don't apply for boats because they otherwise do weird stuff
          setFloat(pcar+0xD0, getFloat(pcar+0xD0)*1000);
          setFloat(pcar+0xD4, getFloat(pcar+0xD4)*1000);
        }
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          if( pcar && pcar_type != VEHICLE_BOAT ) { // in vehicle (not boat) -> reset to normal stats
            setFloat(pcar + 0xD0, getFloat(pcar+0xD0)/1000); //
            setFloat(pcar + 0xD4, getFloat(pcar+0xD4)/1000); // read actual values from handling.dat ??
            lastcar = 0;
          }
          status = 0;
        } else status = 1;
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** power brake - stops vehicle immediately ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - maybe add delay before reversing
 *        - 
 * 
 * Notes: NOT Handbrake, only normal brake    
 **************************************************************************************************************************************/
void *powerbrake(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar && (current_buttons & PSP_CTRL_SQUARE) && (getVehicleCurrentGear( pcar ) >= 0x2) ) { // activate power brake only if gear is 2 or higher
        setFloat(pcar + (LCS ? 0x70 : 0x140), 0.0f);
        setFloat(pcar + (LCS ? 0x74 : 0x144), 0.0f);
        
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}

/** NOS ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *nitro(int calltype, int keypress, int defaultstatus) {
  
  static int status;
  static int nos_time = 0;
  static int exhaust_backup = 0;
  static float thrust = 0.015f;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar && (pressed_buttons & PSP_CTRL_CIRCLE) && (getVehicleCurrentGear( pcar ) >= 0x2) && flag_menu_running == 0 ) { // activate nitro only if gear is 2 or higher
        nos_time = gametimer + 2000; // 2 sec
      }
      
      if( gametimer < nos_time ) {
        //drawString("NITRO", ALIGN_CENTER, FONT_DIALOG, SIZE_BIG, SHADOW_OFF, 240.0f, 10.0f, RED );
        
        /// backup and set Exhaust Color
        int exhaust_offs = getParticleOffsetFor("EXHAUST_FUMES");
        if( exhaust_offs != -1 ) {
          if( exhaust_backup == 0 ) {
            exhaust_backup = getInt(exhaust_offs + (LCS ? 0x70 : 0x60) );
          } 
          setInt( exhaust_offs + (LCS ? 0x70 : 0x60), 0x00CC1111 ); // blue-ish
        }
        
        if( pcar && (current_buttons & PSP_CTRL_CROSS ) && flag_menu_running == 0 && isVehicleInAir( pcar ) == 0 && isVehicleUpsideDown( pcar ) == 0 ) {
          
          /// FOV
          setFieldOfView( getFieldOfView() + 4.0f);
        
          /// SPEED
          setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+(LCS?0x70:0x140)) + getFloat(pcar+0x10) * thrust );
          setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+(LCS?0x74:0x144)) + getFloat(pcar+0x14) * thrust );
          setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+(LCS?0x78:0x148)) + getFloat(pcar+0x18) * thrust );
        }
        
        
      } 
      if( nos_time != 0 && gametimer > nos_time ) {
        /// Reset Exhaust Color
        setInt( getParticleOffsetFor("EXHAUST_FUMES") + (LCS ? 0x70 : 0x60), exhaust_backup ); // blue-ish
        exhaust_backup = 0;
        
        /// Reset Nos Timer
        nos_time = 0;
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // reset
          /// Reset Exhaust Color
          if( exhaust_backup ) {
            setInt( getParticleOffsetFor("EXHAUST_FUMES") + (LCS ? 0x70 : 0x60), exhaust_backup ); // blue-ish
            exhaust_backup = 0;
          }
          /// Reset Nos Timer
          nos_time = 0;
          
          status = 0;
        } else status = 1;
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** Rocket Boost **********************************************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - 
 *        - 
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *rocketboost(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static int boost = 0;
  static float thrust = 0.002f;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: 
      return (int*)boost;  
  
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%i", boost);
      return (void *)retbuf;
        
    case FUNC_APPLY:    
      if( pcar && boost > 0 && (current_buttons & PSP_CTRL_CROSS ) && flag_menu_running == 0 && isVehicleUpsideDown( pcar ) == 0 ) {
        
        /// boats are inWater = true and inAir = true
        if( (isVehicleInAir( pcar ) == 0) /*|| driveonwater(FUNC_GET_STATUS, -1, -1)*/ /*|| ((pcar_type == VEHICLE_BOAT) && (isVehicleInWater( pcar ) >= 1))*/ ) { // boats behave uncontrollable with boost and for VCS the "isVehicleInWater" is no bool? Sometimes 2 or 3 even?!
          
          /// SPEED
          setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+(LCS?0x70:0x140)) + getFloat(pcar+0x10) * ((boost+1) * thrust) );
          setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+(LCS?0x74:0x144)) + getFloat(pcar+0x14) * ((boost+1) * thrust) );
          setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+(LCS?0x78:0x148)) + getFloat(pcar+0x18) * ((boost+1) * thrust) );
        }
      }        
      break;
      
    case FUNC_CHANGE_VALUE:
          
      /*if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 
        
      } else*/ if ( keypress == PSP_CTRL_LEFT && boost > 0 ) { // LEFT
        boost--;
        status = 1;
        
      } else if ( keypress == PSP_CTRL_RIGHT && boost < 50 ) { // RIGHT
        boost++;
        status = 1;

      } else if ( keypress == PSP_CTRL_CIRCLE ) { // CIRCLE
        boost = 0;
        status = 0;
      }
      
      if( boost == 0 )
        status = 0;
      
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( defaultval >= 0 && defaultval <= 50 ) 
        boost = defaultval;
      else // fix for bad config value
        boost = 0;
      break;
  }
   
  return NULL;
}

/** untouchable **********************************************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes: https://twitter.com/TheAdmiester/status/1571894351579545602
 **************************************************************************************************************************************/
void *untouchable(int calltype, int keypress, int defaultstatus) {
  static int status, j;
  int base = -1;
  //static forcemult = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
        
    case FUNC_APPLY:    
      
   /// Tests 
   //sprintf(retbuf, "forcemult: %f", forcemult ); 
     //drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 5.0f, 45.0f, WHITE);
    
      /// check if conditions meet
      //if( pcar && /*(pressed_buttons & PSP_CTRL_DOWN) &&*/ flag_menu_running == 0 ) { 
        
      
        /// loop all vehicles
        base = vehicles_base;
        for( j = 0; j < vehicles_max; j++, base += var_vehobjsize ) {
          if( getVehicleObjectIsActive(base) && base != pcar ) { // ignore player's current vehicle
            float radius = 15.0f; // in which force should be applied
            float x1 = getFloat(pobj+0x30);
            float y1 = getFloat(pobj+0x34);
            float z1 = getFloat(pobj+0x38);
        float x2 = getFloat(base+0x30);
            float y2 = getFloat(base+0x34);
            float z2 = getFloat(base+0x38);
           
            /// "wake vehicle up" aka turn AI controlled "floating" to "physical" vehicle
            setVehicleMakePhysical(base);
      
            if( checkCoordinateInsideArea(x1, y1, z1, x2, y2, z2, radius) ) { // vehicle is in radius
             
              /// set vectors
              float distance = distanceBetweenCoordinates3d(x1, y1, z1, x2, y2, z2); 
              float forcemult = distance * 100.0f / radius; // the lower the faster
              float x_obj = (x2 - x1) / forcemult;
              float y_obj = (y2 - y1) / forcemult;
              //float z_obj = (z2 - z1) / forcemult;
              setFloat(base+(LCS?0x70:0x140), getFloat(base+(LCS?0x70:0x140)) + x_obj);
              setFloat(base+(LCS?0x74:0x144), getFloat(base+(LCS?0x74:0x144)) + y_obj);
              //setFloat(base+(LCS?0x78:0x148), z_obj);
            }
          }
        }
    
    /// loop peds
    /*base = peds_base;
      for( j = 0; j < peds_max; j++, base += var_pedobjsize ) {
        if( getPedObjectIsActive(base) && base != pplayer ) { // ignore player
            float radius = 15.0f; // in which force should be applied
            float x1 = getFloat(pobj+0x30);
            float y1 = getFloat(pobj+0x34);
            float z1 = getFloat(pobj+0x38);
        float x2 = getFloat(base+0x30);
      float y2 = getFloat(base+0x34);
      float z2 = getFloat(base+0x38);
      if( checkCoordinateInsideArea(x1, y1, z1, x2, y2, z2, radius) ) { // is in radius
        // todo
      }
      }
    }*/
      //}    
      break;
      
    case FUNC_CHANGE_VALUE:  
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** tank ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - 
 *        - 
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *tank(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int toggle = 1;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar ) {
        if( /* !PPSSPP && */ pcar_type == VEHICLE_BIKE ) // using on bike crashes (but not on PPSSPP prior to 1.16.x)
          break;
      
        TankControl( pcar );
        if( toggle )
          BlowupVehiclesInPath( pcar );
      
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 
      }
      if( keypress == PSP_CTRL_SQUARE ) { // SQUARE
        toggle = 1 - toggle; 
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}

/** tank ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *driveonwater(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar && pcar_type == VEHICLE_CAR) { // only works for cars
        DoHoverSuspensionRatios( pcar );
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** automaticflipover ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *automaticflipover(int calltype, int keypress, int defaultstatus) {
  
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar ) {
        //if( ) { //same for both Stories Games
          if( getFloat(pcar+0x28) < -0.85 ) { //-0.00 would be on side of car | -1 completely on roof
            
            /// invert the following vectors
            setFloat(pcar + 0x00, 0 - getFloat(pcar));
            setFloat(pcar + 0x04, 0 - getFloat(pcar+0x04));
            setFloat(pcar + 0x08, 0 - getFloat(pcar+0x08));
            
            setFloat(pcar + 0x20, 0 - getFloat(pcar+0x20));
            setFloat(pcar + 0x24, 0 - getFloat(pcar+0x24));
            setFloat(pcar + 0x28, 0 - getFloat(pcar+0x28));
          }
        //}
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}





/** indestr_vehicle ********************************************************************************************
 *
 * Completion:  100% :D
 * 
 * Todo:  - maybe save health and restore (for special vehicles only? -> otherwise dumb idea probably)
        -> or read directly from handling 
 *        - tanks!
 *        - Maybe Press Circle to visibly repair vehicle like cheat does
 *        - Opcode: 03F2   2   car handle, int   set upsidedown car not damaged 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *indestr_vehicle(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int lastcar = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar ) { // in vehicle
        if( lastcar == 0 ) lastcar = pcar;
        setVehicleHealth(pcar, 1000000.0f);
        setVehicleNoPhysicalDamage( pcar );
        setVehicleRepairTyres(pcar, pcar_type);
        
      } else if( !pcar && lastcar ) { // not in car anymore -> reset old car's stats
        setVehicleHealth(lastcar, 1000.0f); // reset car health
        lastcar = 0;
      }  
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // reset
          setVehicleHealth(pcar, 1000.0f); // reset car health
          lastcar = 0;
          status = 0;
        } else status = 1;
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        if( pcar ) {
          #if defined(HEXMARKERS) || defined(HEXEDITOR)
          int temp = pcar + (VCS ? 0x27c : 0x268);
          if( VCS && mod_text_size == 0x00377D30 ) temp = pcar + 0x29C; // ULUS v1.01
          #endif
          #ifdef HEXMARKERS
          hex_marker_clear();  
          hex_marker_addx(temp, sizeof(float));
          #endif
          #ifdef HEXEDITOR  
          hexeditor_create(temp, 0, memory_low, memory_high, "> vehicle health value");
          #endif
        }
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** lockdoors ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *lockdoors(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int lastcar = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar ) { // in vehicle
        if( lastcar == 0 ) lastcar = pcar;
        setVehicleDoorsLocked(pcar, TRUE);
                
      } else if( !pcar && lastcar ) { // not in car anymore -> reset
        setVehicleDoorsLocked(lastcar, FALSE); //reset
        lastcar = 0;
      }  
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) { // reset
          if( pcar ) 
            setVehicleDoorsLocked(pcar, FALSE); // reset
          lastcar = 0;
          status = 0;
        } else status = 1;
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        if( pcar ) {
          #if defined(HEXMARKERS) || defined(HEXEDITOR)
          int temp = pcar + (LCS ? 0x294 : 0x2A8);
          #endif
          #ifdef HEXMARKERS
          hex_marker_clear();  
          hex_marker_addx(temp, sizeof(char));
          #endif
          #ifdef HEXEDITOR  
          hexeditor_create(temp, 0, memory_low, memory_high, "> vehicle lock doors");
          #endif
        }
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}

/** hover vehicle ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:     - restore default wheel camber value from handling file in memory
 * 
 * Notes:

     The "handling" is still not great. It should take car mass and handling values into account.
     Also not instant apply force but calc new direction from momentum and front of car vectors. (LCS: 0x90 = 0x70s & 0x10s & mass)
 
 LCS: vehicle calc turn & move when on ground
 --------------------------------------------
 FUN_0000ce30_CAutomobile_ProcessControl
  FUN_0001e9b4_CAutomobile_ProcessCarWheelPair
    FUN_000a1b78_CVehicle_ProcessWheel
      FUN_0020a1e4_CPhysical_ApplyTurnForce
      FUN_0020a0e8_ApplyMoveForce
    
 **************************************************************************************************************************************/
void *hover_vehicle(int calltype, int keypress, int defaultstatus) { 
  static int status = 0;
  static int lastcar = 0; // for backup
  static float speed, xyspeed;
  static float wheels; // for backup
  static int hovermation = 2;
  static int hovermation_time = 0;
  const static wchar_t delo[] = L"Delorean";
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHECK:
      
      /// The Delorean  
      if( LCS && pcar && pcar_id == 187 ) { // its a V8 Ghost
        if( lcs_getVehicleColorBase( pcar ) == 13 && lcs_getVehicleColorStripe( pcar ) == 0 ) { // color 
          setInt(global_m_pVehicleNameToPrint, (int)&delo[0]); // redirect m_pVehicleNameToPrint

          if( hover_vehicle(FUNC_GET_STATUS, -1, -1) == 0 ) // only when real cheat is disabled (otherwise double apply)
            hover_vehicle(FUNC_APPLY, -1, -1); // apply hover cheat in loop
          
        } else {
          if( getInt(global_m_pVehicleNameToPrint) == (int)&delo[0] ) { // fix - was delorean before -> do restore stuff
            setInt(global_m_pVehicleNameToPrint, getInt(global_m_pVehicleName)); // restore name to display
            setVehicleGravityApplies(pcar, TRUE); // restore gravity for vehicle
            setVehicleWheelCamber(pcar, wheels); // restore wheels camber to default value
            wheels = 0;
            lastcar = 0;
          }
          
        }
      }
      if( LCS && !pcar && lastcar ) { // not in car anymore -> reset old car's values
        setVehicleGravityApplies(lastcar, TRUE); // restore gravity for vehicle
        setVehicleWheelCamber(lastcar, wheels); // restore wheels camber to default value
        wheels = 0;
        lastcar = 0;
      }
      break;
        
    case FUNC_APPLY:
      if( !pcar && lastcar ) { // not in car anymore -> reset old car's values
        setVehicleGravityApplies(lastcar, TRUE); // restore gravity for vehicle
        setVehicleWheelCamber(lastcar, wheels);  // restore wheels camber to default value
        wheels = 0;
        lastcar = 0;
      }
      
      if( pcar && !lastcar ) { // in new car -> backup car's values (this will be done only once)
        wheels = getVehicleWheelCamber(pcar);
        lastcar = pcar;
      }
      
      if( pcar && (pcar_type == VEHICLE_CAR || pcar_type == VEHICLE_BIKE) ) { // in vehicle!
        /// get speed
        speed = getVehicleSpeed(pcar);
        xyspeed = sqrt((getFloat(pcar+(LCS?0x70:0x140)) * getFloat(pcar+(LCS?0x70:0x140))) + (getFloat(pcar+(LCS?0x74:0x144)) * getFloat(pcar+(LCS?0x74:0x144)))); // SQRT( x^2 + y^2 )
        
        /// disable world gravity for vehicle
        setVehicleGravityApplies(pcar, FALSE);
        
        /// lower wheels slowly
        if( pcar_type == VEHICLE_CAR ) { // car
          if( getVehicleWheelCamber(pcar) < 3.0000 ) { // 0x40400000 same as edison
            setVehicleWheelCamber(pcar, getVehicleWheelCamber( pcar ) + 0.05); 
          }
        }

        /// always keep car level #OK
          // flip vehicle forward/backward + up/down with analog
          setFloat(pcar+(LCS?0x80:0x70), getFloat(pcar) * (-(getFloat(pcar+0x18)-(ystick/2)) * 0.1) );
          setFloat(pcar+(LCS?0x84:0x74), getFloat(pcar+0x4) * (-(getFloat(pcar+0x18)-(ystick/2)) * 0.1) );
          setFloat(pcar+(LCS?0x88:0x78), getFloat(pcar+0x8) * (-(getFloat(pcar+0x18)-(ystick/2)) * 0.1) );

          // flip adjustments + ///roll vehicle left/right  + left/right leaning with analog
          setFloat(pcar+(LCS?0x80:0x70), getFloat(pcar+(LCS?0x80:0x70)) + getFloat(pcar+0x10) * ( (getFloat(pcar+0x8)+(xstick/2)) * 0.1) );
          setFloat(pcar+(LCS?0x84:0x74), getFloat(pcar+(LCS?0x84:0x74)) + getFloat(pcar+0x14) * ( (getFloat(pcar+0x8)+(xstick/2)) * 0.1) );
          setFloat(pcar+(LCS?0x88:0x78), getFloat(pcar+(LCS?0x88:0x78)) + getFloat(pcar+0x18) * ( (getFloat(pcar+0x8)+(xstick/2)) * 0.1) );
          
          
        /// turning left/right #OK
        setFloat(pcar + (LCS?0x88:0x78), -0.04 * xstick); // spin vehicle around Z world axis (slow turning -0.02 | -0.05 faster turning)
        
        ///manual hovering up/down
        if( (current_buttons & PSP_CTRL_UP) && ((xyspeed < 0.1f) || (current_buttons & PSP_CTRL_RTRIGGER)) && flag_menu_running == 0 ) 
          setFloat(pcar+(LCS?0x98:0x148), getFloat(pcar+(LCS?0x98:0x148)) + 0.02); // 0.015 for LCS?
        if( (current_buttons & PSP_CTRL_DOWN) && ((xyspeed < 0.1f) || (current_buttons & PSP_CTRL_RTRIGGER)) && flag_menu_running == 0 ) {
          setFloat(pcar+(LCS?0x98:0x148), getFloat(pcar+(LCS?0x98:0x148)) - 0.02);
        }
         
        /// hovering animation #could be smoother
        if( speed < 0.05 ) { 
          if( hovermation == 1 ) { // animation up
            if( gametimer >= hovermation_time + 1000 ) { // 1 sec
               hovermation_time = gametimer;
               hovermation = 2;
            } else setFloat(pcar+(LCS?0x98:0x148), getFloat(pcar+(LCS?0x98:0x148))+0.001);
          } else if( hovermation == 2 ) { // animation down
            if( gametimer >= hovermation_time + 1000 ) { // 1 sec
               hovermation_time = gametimer;
               hovermation = 1;
            } else setFloat(pcar+(LCS?0x98:0x148), getFloat(pcar+(LCS?0x98:0x148))-0.001);
          }
        }
      
        ///////////////////////////////////////////////////////////

        if( !(current_buttons & PSP_CTRL_RTRIGGER) && flag_menu_running == 0 ) { /// "handbrake" keeps momentum (old style)
          if( xyspeed > 0.1f ) { // to allow hover animation without force forward
            if( getVehicleCurrentGear(pcar) > 0 ) { // not in reverse
              /// keep the momentum in the direction the vehicle
              setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+0x10) * speed);
              setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+0x14) * speed);
              setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+0x18) * speed);
            } else {
              /// keep the momentum in the reverse direction the vehicle
              setFloat(pcar+(LCS?0x70:0x140), -getFloat(pcar+0x10) * speed);
              setFloat(pcar+(LCS?0x74:0x144), -getFloat(pcar+0x14) * speed);
              setFloat(pcar+(LCS?0x78:0x148), -getFloat(pcar+0x18) * speed);
            }
          }
        }
             
        /// thrust, reverse & automatically slowing down (could also be done by setting lower/higher speed but I'd like to keep the "handbrake feature")
        if( current_buttons & PSP_CTRL_CROSS && flag_menu_running == 0 ) {  /// forward
          setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+(LCS?0x70:0x140)) + getFloat(pcar+0x10) * 0.010);
          setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+(LCS?0x74:0x144)) + getFloat(pcar+0x14) * 0.010);
          setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+(LCS?0x78:0x148)) + getFloat(pcar+0x18) * 0.022);
          
        } else if( current_buttons & PSP_CTRL_SQUARE && flag_menu_running == 0 ) { /// break & reverse
          if( getVehicleCurrentGear(pcar) > 0 || speed < 0.5f ) { /// limit reverse speed
            setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+(LCS?0x70:0x140)) - getFloat(pcar+0x10) * 0.02);
            setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+(LCS?0x74:0x144)) - getFloat(pcar+0x14) * 0.02);
            setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+(LCS?0x78:0x148)) - getFloat(pcar+0x18) * 0.05);
          }
        } else { /// stopping in air (because there is no ground that would do that normally)
          if( speed > 0.01f ) { /// stopping depending on speed (the slower the faster)
            setFloat(pcar+(LCS?0x70:0x140), getFloat(pcar+(LCS?0x70:0x140)) - (getFloat(pcar+(LCS?0x70:0x140)) * (0.01/speed))); // 0.0003 the higher the faster
            setFloat(pcar+(LCS?0x74:0x144), getFloat(pcar+(LCS?0x74:0x144)) - (getFloat(pcar+(LCS?0x74:0x144)) * (0.01/speed)));
            setFloat(pcar+(LCS?0x78:0x148), getFloat(pcar+(LCS?0x78:0x148)) - (getFloat(pcar+(LCS?0x78:0x148)) * (0.01/speed)));
          }
        }
      
        ///////////////////////////////////////////////////////////////////////////////////////////////
   /*
        // LCS 0x70 moving currently
        // LCS 0x80 turning / leaning currently
        // LCS 0x90 moving to be applied
        // LCS 0xA0 turning / leaning to be applied
     
        extern char buffer[256];
     
        snprintf(buffer, sizeof(buffer), "magicval: %.3f", magicval);
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 40.0f, 20.0f, RED);
     
        snprintf(buffer, sizeof(buffer), "00:  %.2f, %.2f, %.2f", getFloat(pcar+0x0), getFloat(pcar+0x4), getFloat(pcar+0x8));
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 40.0f, WHITE);
     
        snprintf(buffer, sizeof(buffer), "10:  %.2f, %.2f, %.2f", getFloat(pcar+0x10), getFloat(pcar+0x14), getFloat(pcar+0x18));
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 60.0f, WHITE);
   
        snprintf(buffer, sizeof(buffer), "20:  %.2f, %.2f, %.2f", getFloat(pcar+0x20), getFloat(pcar+0x24), getFloat(pcar+0x28));
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 80.0f, WHITE);
   
   
        snprintf(buffer, sizeof(buffer), "70:  %.2f, %.2f, %.2f", getFloat(pcar+0x70), getFloat(pcar+0x74), getFloat(pcar+0x78));
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 110.0f, WHITE);
   
        snprintf(buffer, sizeof(buffer), "80:  %.2f, %.2f, %.2f", getFloat(pcar+0x80), getFloat(pcar+0x84), getFloat(pcar+0x88));
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 130.0f, WHITE);
   
        snprintf(buffer, sizeof(buffer), "90:  %.2f, %.2f, %.2f", getFloat(pcar+0x90), getFloat(pcar+0x94), getFloat(pcar+0x98));
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 150.0f, WHITE);
     
        snprintf(buffer, sizeof(buffer), "A0:  %.2f, %.2f, %.2f", getFloat(pcar+0xA0), getFloat(pcar+0xA4), getFloat(pcar+0xA8));
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 20.0f, 170.0f, WHITE);
       
       
        snprintf(buffer, sizeof(buffer), "speed (0x124): %.3f", speed);
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 80.0f, 190.0f, RED);
 
        snprintf(buffer, sizeof(buffer), "xyspeed (calced): %.3f", xyspeed );
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 80.0f, 210.0f, RED);
     
        snprintf(buffer, sizeof(buffer), "xyzspeed (calced): %.3f", xyzspeed );
        drawString(buffer, ALIGN_FREE, FONT_DIALOG, SIZE_NORMAL, SHADOW_OFF, 80.0f, 230.0f, RED);
   */
        ///////////////////////////////////////////////////////////////////////////////////////////////  
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        if( status ) {
          if( pcar && (pcar_type == VEHICLE_CAR || pcar_type == VEHICLE_BIKE) ) {
            setVehicleGravityApplies(pcar, TRUE); // restore gravity for vehicle
            setVehicleWheelCamber(pcar, wheels); // restore wheels camber to default value
            wheels = 0;
            lastcar = 0;
          } status = 0;
        } else status = 1;
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;

      if( pplayer > 0 && lastcar && (pcar_type == VEHICLE_CAR || pcar_type == VEHICLE_BIKE) ) {
        setVehicleGravityApplies(lastcar, TRUE); // restore gravity for vehicle
        setVehicleWheelCamber(lastcar, wheels);  // restore wheels camber to default value
        wheels = 0;
        lastcar = 0;        
      } 
      break;
  }
   
  return NULL;
}



/** aim of death ********************************************************************************************
 *
 * Completion:   99%
 * 
 * Todo:  - make PEDs on Bikes fall off dramatically (not casually exit and die)
 *        - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *aim_of_death(int calltype, int keypress, int defaultstatus) { 
  static int status;
  int ped = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      ped = getInt(pplayer+(LCS ? 0x6BC : 0x81C));
        
      if( ped > 0 ) { // addr of ped aimed at
        setPedHealthAndArmor(ped, 0, 0);
        setPedExitVehicleNow(ped); // peds on motorbikes don't die -> make them exit and die
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) 
        status = 1 - status; // CROSS
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** step through wall / teleport one step in players facing direction *******************************************************************
 *
 * Completion:   100% :D
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:     z coordinate is ignored here
 **************************************************************************************************************************************/
void stepthroughwall() {
  if( pplayer ) {
    setFloat(pobj+0x30, getFloat(pplayer+0x30) + ( cos( getFloat(pplayer+(LCS ? 0x4E0 : 0x8D0)) + (M_PI/2) ) * 1.2 ) );
    setFloat(pobj+0x34, getFloat(pplayer+0x34) + ( sin( getFloat(pplayer+(LCS ? 0x4E0 : 0x8D0)) + (M_PI/2) ) * 1.2 ) );
  }
}



/** gravity ************************************************************************************
 *
 * Completion:   95% :|
 * 
 * Todo:     - understand value 0x3C10 ?!
 * 
 * Notes:   LCS US v3.00   ->     0x0020A898: 0x3C053C03 '.<.<' - lui        $a1, 0x3C03
 *          VCS US v1.05   ->     0x00263690: 0x3C053C03 '.<.<' - lui        $a1, 0x3C03
 * 
 * 0x3C03126F = 0.008f   = 1.0g
 
       ************************************************************
        0020a894 36 00 15 3c     lui        s5,0x36
        0020a898 03 3c 05 3c     lui        a1,0x3c03
        0020a89c 6f 12 a5 34     ori        a1,a1,0x126f
      ************************************************************
 * 
 **************************************************************************************************************************************/
static int gravity_reverse = 0;
short alt_gravity_button_pressed = 0;

void *world_gravity(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static float gravity = 1.0f; // 2.0g * 0.008f = 0.016f
  static float val;
  int val2;

  switch( calltype ) {
    case FUNC_GET_STATUS: 
      if( keypress == 42 ) 
        return (int*)gravity_reverse; // for config
      else return (int*)status; // for cheat status

    case FUNC_GET_VALUE: // for config
      return (int*)(int)((gravity  * 10.0f) + 1000.0f); 
      //-11.0g will be  890 for config
      // -4.2g will be  958 for config
      // -0.0g will be 1000 for config
      //  1.0g will be 1010 for config
      //  1.2g will be 1012 for config
    
    case FUNC_APPLY:
      /// whats tis fuckery (basically: 0.008f = 0x3C03126F >> 16 = 0x3C03)
      val = (gravity * 0.008f); 
      val2 = getInt((int)&val) >> 16;
      
      if( (gravity_reverse && ( current_buttons & PSP_CTRL_UP ) && flag_menu_running == 0) || alt_gravity_button_pressed) { // flag_menu_stop will prevent going up in menu trigger it
        val2 -= 0x8000; // make negative
      }
      setShort(addr_worldgravity, val2);
      
      clearICacheFor(addr_worldgravity);
      alt_gravity_button_pressed = 0;
      break;  
      
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%.1fg", gravity);
      if( gravity_reverse ) 
        sprintf(retbuf, "%s   up = reverse", retbuf);
      return retbuf;
        
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1;
        gravity_reverse = 1 - gravity_reverse;
        
      } else if( keypress == PSP_CTRL_CIRCLE ) {
        gravity = 1.0f;
      
      } else if( keypress == PSP_CTRL_LEFT && gravity > -11.0f ) { // LEFT
        if( gravity >= -4.9f && gravity <= 5.9f )
          gravity -= 0.1f;
        else  
          gravity -= 1.0f;
        
        status = 1;
        
      } else if( keypress == PSP_CTRL_RIGHT && gravity < 29.0f ) { // RIGHT
        if( gravity <= 4.9f && gravity >= -5.9f )
          gravity += 0.1f;
        else
          gravity += 1.0f;
        status = 1;
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS  
        hex_marker_clear();
        hex_marker_addx(addr_worldgravity, sizeof(short));
        #endif
        #ifdef HEXEDITOR
        hexeditor_create(addr_worldgravity, 0, memory_low, memory_high, "> world gravity");
        #endif
      }
      
      if( gravity == 1.0f && alt_gravity_button_pressed == 0 ) {
        setShort(addr_worldgravity, 0x3C03); // reset to default
        if( gravity_reverse == 0 ) 
          status = 0; // disable on default value
      }
      
      if( keypress == 42 ) // for buttonApply() alternate gravity reverse solution
        status = 1;
      
      break;
      
    case FUNC_SET: // special: status is depending on value here
      /// set ReverseButton
      status = gravity_reverse = defaultstatus;
      
      /// set Gravity value
      if( defaultval > 200 ) { // 200 can be set back to 0 at some point (fix for old configs)
        val = ((float)defaultval - 1000.0f) / 10.0f; // gravity value from config eg: 1.2f for 1.2g
        #ifdef LOG
        //logPrintf("defaultval: '0x%X'", defaultval);
        //logPrintf("val: '%f'", val);
        #endif
      } else val = 1.0f;
      
      if( val == 1.0f && !gravity_reverse ) {
        status = 0;
        
      } else if( val >= -12.0f && val <= 30.0f ) {
        gravity = val;
        status = 1;
        
      } else { // complete reset
        gravity = 1.0f;
        setShort(addr_worldgravity, 0x3C03); // reset to default
        gravity_reverse = 0;
        status = 0;
      }
      break;
  }
  
  return NULL;
}

/** driveonwalls ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *driveonwalls(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar ) {
        setVehicleGravityApplies(pcar, TRUE); // set gravity true
      
        if( getFloat(pcar+0x28) < 0.50 ) { // -0.00 would be on side of car | -1 completely on roof | 1 normal
          setVehicleGravityApplies(pcar, FALSE); // set gravity false
          
          if( gravity_reverse && (current_buttons & PSP_CTRL_UP) && flag_menu_running == 0 ) { // reverse button from worldgravity cheat is pressed
            setFloat(pcar + (LCS?0x70:0x140),  getFloat(pcar + (LCS?0x70:0x140)) + getFloat(pcar + 0x20) * 0.01 );
            setFloat(pcar + (LCS?0x74:0x144),  getFloat(pcar + (LCS?0x74:0x144)) + getFloat(pcar + 0x24) * 0.01 );
            setFloat(pcar + (LCS?0x78:0x148),  getFloat(pcar + (LCS?0x78:0x148)) + getFloat(pcar + 0x28) * 0.01 );
          } else {
            setFloat(pcar + (LCS?0x70:0x140),  getFloat(pcar + (LCS?0x70:0x140)) - getFloat(pcar + 0x20) * 0.01 );
            setFloat(pcar + (LCS?0x74:0x144),  getFloat(pcar + (LCS?0x74:0x144)) - getFloat(pcar + 0x24) * 0.01 );
            setFloat(pcar + (LCS?0x78:0x148),  getFloat(pcar + (LCS?0x78:0x148)) - getFloat(pcar + 0x28) * 0.01 );
          }
        }
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          if( pcar ) setVehicleGravityApplies(pcar, TRUE); // restore gravity for vehicle
          status = 0;
        } else status = 1;
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** Current Vehicle Color ********************************************************************************************
 *
 * Completion:   99% :/
 * 
 * Todo:     - 
 *        - make LCS cheat same as VCS. Copy real color value to pos: 127 and adjust there like in VCS
 * 
 * Notes:     for VCS the full color value is in vehcile_object @ 0x224 & 0x228 while LCS uses presets
 ******************************************************************************************************************************************************/
void *vehicle_base_color(int calltype, int keypress, int defaultstatus) {
  static char lcs_color = 0; // LCS : 0 to 127          BASE         STRIPE
  static int vcs_color = 0;  // VCS : 0xFFRRGGBB  -> in ram  -> BB GG RR FF   BB GG RR FF
  static char pos = 2;
  static unsigned int r, g, b;
  if( pcar ) {
    switch( calltype ) {
      case FUNC_GET_STRING: 
        if( LCS ) sprintf(retbuf, "%i", lcs_color);
        if( VCS ) {
          sprintf(retbuf, pos == 0 ? translate_string("Red: %i Green: %i Blue:>%i<") : 
                          pos == 1 ? translate_string("Red: %i Green:>%i<Blue: %i") : 
                          pos == 2 ? translate_string("Red:>%i<Green: %i Blue: %i") : translate_string("error"), (vcs_color & 0x000000ff), (vcs_color & 0x0000ff00) >> 8, (vcs_color & 0x00ff0000) >> 16);
        }
        return (void *)retbuf;
      
      case FUNC_CHECK:
        if( LCS ) lcs_color = lcs_getVehicleColorBase( pcar );
        if( VCS ) vcs_color = vcs_getVehicleColorBase( pcar );
        break;
        
      case FUNC_CHANGE_VALUE:
        if( keypress == PSP_CTRL_TRIANGLE ) {
          #if defined(HEXMARKERS) || defined(HEXEDITOR)
          int temp = pcar + (LCS ? 0x1F0 : 0x224);
          #endif
          #ifdef HEXMARKERS
          hex_marker_clear();
          hex_marker_addx(temp, (LCS ? sizeof(char) : sizeof(int)));
          #endif
          #ifdef HEXEDITOR  
          hexeditor_create(temp, 0, memory_low, memory_high, "> Primary Color");
          #endif
          break;
        }
        
        if( VCS ) {
          if( keypress == PSP_CTRL_CROSS ) { // CROSS
            pos--;
            if( pos < 0 ) pos = 2;
            break;
          }
          r = (vcs_color & 0x000000ff);
          g = (vcs_color & 0x0000ff00) >> 8;
          b = (vcs_color & 0x00ff0000) >> 16;
          if( keypress == PSP_CTRL_LEFT ) {
            if( pos == 2 ) if( r > 0x00 ) r--;
            if( pos == 1 ) if( g > 0x00 ) g--;
            if( pos == 0 ) if( b > 0x00 ) b--;
            vcs_color = 0xFF000000 + (b << 16) + (g << 8) + r;
          }
          if( keypress == PSP_CTRL_RIGHT) {
            if( pos == 2 ) if( r < 0xFF ) r++;
            if( pos == 1 ) if( g < 0xFF ) g++;
            if( pos == 0 ) if( b < 0xFF ) b++;
            vcs_color = 0xFF000000 + (b << 16) + (g << 8) + r;
          }
          
          vcs_setVehicleColorBase(pcar, vcs_color);
        }
        if( LCS ) {
          if( keypress == PSP_CTRL_LEFT && lcs_color > 0 ) {
            lcs_color--;
          } else if( keypress == PSP_CTRL_RIGHT && lcs_color < 127 ) {
            lcs_color++;
          }
          
          lcs_setVehicleColorBase(pcar, lcs_color);
        }
        break;
    }  

  } else {
    if( calltype == FUNC_GET_STRING ) 
      return "/"; // not in a car
  }  
  return NULL;
}

void *vehicle_stripe_color(int calltype, int keypress, int defaultstatus) {
  static char lcs_color = 0; // LCS : 0 to 127        BASE         STRIPE
  static int vcs_color = 0;  // VCS : 0xFFRRGGBB -> in ram -> BB GG RR FF   BB GG RR FF
  static char pos = 2;
  static unsigned int r, g, b;
  if( pcar ) {
    switch( calltype ) {
      case FUNC_GET_STRING: 
        if( LCS ) sprintf(retbuf, "%i", lcs_color);
        if( VCS ) {
          sprintf(retbuf, pos == 0 ? translate_string("Red: %i Green: %i Blue:>%i<") : 
                          pos == 1 ? translate_string("Red: %i Green:>%i<Blue: %i") : 
                          pos == 2 ? translate_string("Red:>%i<Green: %i Blue: %i") : translate_string("error"), (vcs_color & 0x000000ff), (vcs_color & 0x0000ff00) >> 8, (vcs_color & 0x00ff0000) >> 16);
        }
        return (void *)retbuf;
      
      case FUNC_CHECK:
        if( LCS ) lcs_color = lcs_getVehicleColorStripe( pcar );
        if( VCS ) vcs_color = vcs_getVehicleColorStripe( pcar );
        break;
        
      case FUNC_CHANGE_VALUE:
        if( keypress == PSP_CTRL_TRIANGLE ) {
          #if defined(HEXMARKERS) || defined(HEXEDITOR)
          int temp = pcar + (LCS ? 0x1F1 : 0x228);
          #endif
          #ifdef HEXMARKERS
          hex_marker_clear();
          hex_marker_addx(temp, (LCS?sizeof(char):sizeof(int)));
          #endif
          #ifdef HEXEDITOR  
          hexeditor_create(temp, 0, memory_low, memory_high, "> Secondary Color");
          #endif
          break;
        }
          
        if( VCS ) {
          if( keypress == PSP_CTRL_CROSS ) { // CROSS
            pos--;
            if( pos < 0) pos = 2;
            break;
          }
          r = (vcs_color & 0x000000ff);
          g = (vcs_color & 0x0000ff00) >> 8;
          b = (vcs_color & 0x00ff0000) >> 16;
          if( keypress == PSP_CTRL_LEFT ) {
            if( pos == 2 ) if( r > 0x00 ) r--;
            if( pos == 1 ) if( g > 0x00 ) g--;
            if( pos == 0 ) if( b > 0x00 ) b--;
            vcs_color = 0xFF000000 + (b << 16) + (g << 8) + r;
          }
          if( keypress == PSP_CTRL_RIGHT ) {
            if( pos == 2 ) if( r < 0xFF ) r++;
            if( pos == 1 ) if( g < 0xFF ) g++;
            if( pos == 0 ) if( b < 0xFF ) b++;
            vcs_color = 0xFF000000 + (b << 16) + (g << 8) + r;
          }
          
          vcs_setVehicleColorStripe(pcar, vcs_color);
        }
        if( LCS ) {
          if( keypress == PSP_CTRL_LEFT && lcs_color > 0 ) {
            lcs_color--;
          } else if( keypress == PSP_CTRL_RIGHT && lcs_color < 127 ) {
            lcs_color++;
          }
          
          lcs_setVehicleColorStripe(pcar, lcs_color);
        }
        break;
    }  

  } else {
    if( calltype == FUNC_GET_STRING ) 
      return "/"; // not in a car
  }  
  return NULL;
}



/** Weather ************************************************************************************
 *
 * Completion:  99%
 * 
 * Todo:     - Checking for weather only checks one (first) global -> it can be "sunny" but with rain effect for example
 * 
 * Notes: There are 3 weather values: 
          First: Sky Color, Clouds -> global_weather
          Second: Effekt (like rain, fog, snow)
          Third: next & force weather when set (otherwise 0xFFFF for random cycle, "ReleaseWeather" sets 0xFFFF)
 **************************************************************************************************************************************/
void *world_weather(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static int i = 0;

  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_APPLY:
      SetWeatherNow(i);
      break;
    
    case FUNC_CHECK:
      if( !status ) { // cheat is not enabled so we can check for the current weather
        i = getWeather(); //getShort(global_weather + gp_);
      }
      break;
      
    case FUNC_GET_STRING: 
      return (void*)(LCS ? weather_lcs[i] : weather_vcs[i]);
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; // switch status
        if( status == 0 ) 
          ReleaseWeather();
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        status = 1;
      } else if( keypress == PSP_CTRL_RIGHT && i < 7 ) { // RIGHT
        i++;
        status = 1;
      } break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( defaultval >= 0 ) 
        i = defaultval;
      else i = 0;

      if( status == 0 && pplayer > 0 ) // check pplayer so that inital FUNC_SET on boot doesn't call function
        ReleaseWeather(); // otherwise crashes VCS US on boot! but US only????
      break;
  }
  
  return NULL;
}



/** traffic density multiplier ********************************************************************************************
 *
 * Completion:  100% :D
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:     global_trafficdensity
 **************************************************************************************************************************************/
void *traffic_density(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setInt(global_trafficdensity + gp_, 0);
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
        trafficdensity_disable:
        setInt(global_trafficdensity + gp_, 0x3f800000); // float = 1.0
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        hex_marker_addx(global_trafficdensity + gp_, sizeof(float));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_trafficdensity+ gp_, 0, memory_low, memory_high, "> traffic density multiplier");
        #endif
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if(!status && pplayer > 0) // check if game in playable state
        goto trafficdensity_disable;
      break;
  }
   
  return NULL;
  
}
/** traffic freeze ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *traffic_freeze(int calltype, int keypress, int defaultstatus) {
  static int status;
  int j, base;
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      base = vehicles_base;
      for( j = 0; j < vehicles_max; j++, base += var_vehobjsize ) {
        if( base != pcar ) { // ignore player's current vehicle
          setVehicleMakePhysical(base);
          setPedOrVehicleFreezePosition(base, TRUE); // fully freeze (needs reset)
      
          // @ 0x44 -> make 44 82 19 to 44 8A 19 will disable / freeze in place until touched

          //setFloat(base+(LCS?0x70:0x140), 0.00f); // zero moving vectors -> results in slow-mo 
          //setFloat(base+(LCS?0x74:0x144), 0.00f);
          //setFloat(base+(LCS?0x78:0x148), 0.00f);
      
        } else { // reset when player entered already frozen vehicle
          setPedOrVehicleFreezePosition(base, FALSE); // reset
        }
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
        if( status == 0 ) {
          base = vehicles_base;
          for( j = 0; j < vehicles_max; j++, base += var_vehobjsize ) {
            setPedOrVehicleFreezePosition(base, FALSE); // reset
          }
        }
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}

/** ped density multiplier ********************************************************************************************
 *
 * Completion:  98% :D
 * 
 * Todo:  - LCS: avenging angels still spawning
 *        - 
 * 
 * Notes:     global_peddensity
 **************************************************************************************************************************************/
void *peds_density(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setInt(global_peddensity+gp_, 0x0);
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
        peddensity_disable:
        setInt(global_peddensity + gp_, 0x3f800000); // float = 1.0
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        hex_marker_addx(global_peddensity + gp_, sizeof(float));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_peddensity + gp_, 0, memory_low, memory_high, "> ped density multiplier");
        #endif
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( !status && pplayer > 0 ) 
        goto peddensity_disable;
      break;
  }
   
  return NULL;
  
}

/** onmission ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:     
  LCS see: int FUN_001541f0_CTheScripts_IsPlayerOnAMission
  VCS see: int FUN_0005f4f8_CTheScripts_IsPlayerOnAMission
  
  LCS: DAT_00334d7c_ScriptSpace + DAT_0035a48c_OnAMissionFlag
  
 **************************************************************************************************************************************/
void *onmission(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, option = 0;
  static int offsetinspace = 0; // getInt(global_OnAMissionFlag + (LCS ? 0 : gp));
  static int scriptspace = 0; // getInt(global_ScriptSpace + (LCS ? 0 : gp));
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      //return IsPlayerOnAMission();
      return (int*)status;
    
    case FUNC_GET_STRING: 
      return ( option ? "true" : "false");
      
    case FUNC_CHECK:
      offsetinspace = getInt(global_OnAMissionFlag + (LCS ? 0 : gp));
      scriptspace = getInt(global_ScriptSpace + (LCS ? 0 : gp));
      if( status == 0 && offsetinspace > 0 && isInMemBounds(scriptspace) ) 
        option = getByte(scriptspace + offsetinspace);
      break;  
    
    case FUNC_APPLY:
      if( offsetinspace > 0 && scriptspace > 0 ) {
        setByte(scriptspace + offsetinspace, option);
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
        
      } else if( keypress == PSP_CTRL_LEFT ) { // LEFT
        if( status )
          option = 1 - option;
      } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
        if( status )
          option = 1 - option;
  
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear(); 
        #endif
        if( offsetinspace == 0 ) {
          #ifdef HEXMARKERS
          hex_marker_addx(offsetinspace, sizeof(char));
          #endif
          #ifdef HEXEDITOR  
          hexeditor_create(offsetinspace, 0, memory_low, memory_high, "> OnAMissionFlag");
          #endif
        } else if( offsetinspace > 0 && scriptspace > 0 ) {
          #ifdef HEXMARKERS
          hex_marker_addx(scriptspace + offsetinspace, sizeof(char));
          #endif
          #ifdef HEXEDITOR
          hexeditor_create(scriptspace + offsetinspace, 0, memory_low, memory_high, "> OnAMissionFlag");
          #endif
        }
      }
      
      //if( status == 0 && offsetinspace > 0 ) {
      //  setByte(getInt(global_ScriptSpace + (LCS ? 0 : gp)) + offsetinspace, 0); //we could be on a mission though
      //}
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      
      if( defaultval == 0 ) 
        option = 0;
      else if( defaultval == 1 ) 
        option = 1;
      else option = 0;
      
      break;
  }
   
  return NULL;
  
}


/** freeze_timers ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *freeze_timers(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setByte(global_freezetimers, 1); // stop timers
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
        freezetimers_disable:
        setByte(global_freezetimers, 0); // normal
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        hex_marker_addx(global_freezetimers, sizeof(char));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_freezetimers, 0, memory_low, memory_high, "> freeze timers");
        #endif
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( !status && global_freezetimers != -1 ) 
        goto freezetimers_disable;
      break;
  }
   
  return NULL;
  
}


/** kill peds aiming at player  ********************************************************************************************
 *
 * Completion:  90
 * 
 * Todo:  - FIst fighters don't count?!
 *        - 
 * 
 * Notes:     
 * 
 * 0x10C  ped to ?
 * 0x258  ped to focus?
 * 0x25C  veh to focus?
 * 0x310  ped to ?
 * 
 * 0x274
 * 0x530
 * 
 * 0x6BC  ped aiming with gun
 * 0x6E0  last touched ped
 * 0x724  array of peds in range  
 * 0x74C   (byte) number of peds 0xA max
 * 
 **************************************************************************************************************************************/
void *peds_killaiming(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int j;
  
  int base = -1;
  //static char buffer[64];
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      base = peds_base;
      //snprintf(buffer, sizeof(buffer), "Debug: %i, 0x%08X = 0x%08X", peds_max, base, pobj);
      //setTimedTextbox(buffer, 7.00f);
      for( j = 0; j < peds_max; j++, base += var_pedobjsize ) {
        if( getInt(base+(LCS?0x6BC:0x81C)) == pobj ) { // can be pplayer or pcar -> use pobj
          setPedHealthAndArmor(base, 0, 0);
        }
      }
      //status = 0;
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
        
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** peds_freeze  ********************************************************************************************
 *
 * Completion: 
 * 
 * Todo:  - disable all animation as well?
 *        - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *peds_freeze(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int j;
  
  int base = -1;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      /// freeze
      base = peds_base;
      for( j = 0; j < peds_max; j++, base += var_pedobjsize ) {
        if( base != pplayer ) {
          setPedOrVehicleFreezePosition(base, TRUE);
        }
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        if( status ) {
          /// unfreeze
          base = peds_base;
          for(j = 0; j < peds_max; j++, base += var_pedobjsize) {
            if( base != pplayer ) {
              setPedOrVehicleFreezePosition(base, FALSE);
            }
          }
          status = 0;
        } else status = 1;
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
  
}


/** show PEDs Health and Armor when aimed at *******************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - make the text stick to the aiming indicator somehow and not fixed on screen center
 *        - 
 * 
 * Notes: 
 **************************************************************************************************************************************/
/*void *peds_showstats(int calltype, int keypress, int defaultstatus) {
  static int status;
  static int j;
  
  int base = -1;
  int curped = -1;
  
  static char buffer1[16];
  static char buffer2[16];
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      base = peds_base;
      curped = getInt(pplayer+(LCS?0x6BC:0x81C));
      
      if( curped > 0 ) {
        for( j = 0; j < peds_max; j++, base += var_pedobjsize ) {
          if( curped == base ) {

            /// Health
            snprintf(buffer1, sizeof(buffer1), "%.2f", getPedHealth(base));
            drawString(buffer1, ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 100.0f, (LCS?LCS_HEALTH:VCS_HEALTH) );
            
            /// Armor
            if( getPedArmor(base) > 0.00f ) {
              snprintf(buffer2, sizeof(buffer2), "%.2f", getPedArmor(base));
              drawString(buffer2, ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 85.0f, (LCS?LCS_ARMOR:VCS_ARMOR) );
            }
          }
        }
      }
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
  
}*/



/** maximum health ********************************************************************************************************************
 *
 * Completion:  100%
 * 
 * Todo:  - keep current health when changing multiplier
 *        - 
 * 
 * Notes:     global_maxhealthmult
 **************************************************************************************************************************************/
void *max_health(int calltype, int keypress) {
  static unsigned char healthval;
  static int status;
  
  switch( calltype ) {
    
    case FUNC_GET_STATUS: 
      if( healthval > 0x64 )
        status = 1;
      else status = 0;
      return (int*)status;
    
    case FUNC_GET_STRING:
      healthval = getMaxHealthMultiplier();
      sprintf(retbuf, "%i%%", healthval); // "0x%02X %%"
      return (void*)retbuf;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && healthval > 0x64 ) {
        healthval -= 1;
        setMaxHealthMultiplier(healthval);
        setPedHealthAndArmor(pplayer, (float)(healthval), -1);
      }
      if( keypress == PSP_CTRL_RIGHT && healthval < 0xFF ) {
        healthval += 1;
        setMaxHealthMultiplier(healthval);
        setPedHealthAndArmor(pplayer, (float)(healthval), -1);
      }
      if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_maxhealthmult, sizeof(char));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_maxhealthmult, 0, memory_low, memory_high, "> max health multiplier");
        #endif
      }
      break;
  }
  return NULL;
}
/** maximum armour ********************************************************************************************************************
 *
 * Completion:  100%
 * 
 * Todo:  -  keep current armor when changing multiplier
 *        - 
 * 
 * Notes:     global_maxarmormult
 **************************************************************************************************************************************/
void *max_armor(int calltype, int keypress) {
  static unsigned char armorval;
  static int status;
  
  switch( calltype ) {
    
    case FUNC_GET_STATUS: 
      if( armorval > 0x64 )
        status = 1;
      else status = 0;
      return (int*)status;
      
    case FUNC_GET_STRING:
      armorval = getMaxArmorMultiplier();
      sprintf(retbuf, "%i%%", armorval); // "0x%02X %%"
      return (void*)retbuf;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && armorval > 0x64 ) {
        armorval -= 1;
        setMaxArmorMultiplier(armorval);
        setPedHealthAndArmor(pplayer, -1, (float)armorval);
      }
      if( keypress == PSP_CTRL_RIGHT && armorval < 0xFF ) {
        armorval += 1;
        setMaxArmorMultiplier(armorval);
        setPedHealthAndArmor(pplayer, -1, (float)armorval);
      }
      if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_maxhealthmult, sizeof(char));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_maxarmormult, 0, memory_low, memory_high, "> max armor multiplier");
        #endif
      } 
      break;
  }
  return NULL;
}

/** unlimited_sprinting ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - Other way to do it: patch FUN_0013f37c_sprinting
 *        - 
 * 
 * Notes: This is a flag set by the game as reward at some point.
 **************************************************************************************************************************************/
void *unlimited_sprinting(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setUnlimitedSprintStatus(0x1);
      //setByte(pplayer+0xbb3, 0x1); //this alone works as well for LCS
      break;
      
    case FUNC_CHECK:
      if( !status ) 
        status = getUnlimitedSprintStatus();
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          unlimsprint_disable:
          setUnlimitedSprintStatus(0x0);
          status = 0;
        } else status = 1;
      }
      if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_unlimtedsprint, sizeof(char));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_unlimtedsprint, 0, memory_low, memory_high, "> unlimted sprinting bool");
        #endif
      } 
      break;
      
    case FUNC_SET: // todo - not used currently
      status = defaultstatus;
      if( pplayer > 0 ) { // check if game in playable state
        if( !status ) 
          goto unlimsprint_disable;
      }
      break;
  }
   
  return NULL;
}



/** Wanted level ************************************************************************************
 *
 * Completion:   99%
 * 
 * Todo:  - 
 * 
 * Notes:     
 **************************************************************************************************************************************/
void *wanted_level(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0;
  static int i = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING:
      sprintf(retbuf, translate_string("%i Stars (%i max)"), i, getByte(global_maxwantedlevel+(LCS ? 0 : gp)));
      return (void*)retbuf;
      
    case FUNC_APPLY:
      SetWantedLevel(pplayer, i);
      break;  
      
    case FUNC_CHECK:
      if( !status && pplayer > 0 ) // check current level when cheat deactivated
        i = getWantedLevel(pplayer); 
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 

      } else if( keypress == PSP_CTRL_SQUARE ) { // SQUARE
        SetMaxWantedLevel(i);

      } else if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        status = 1;
      } else if( keypress == PSP_CTRL_RIGHT && i < 6 ) { // RIGHT
        i++;
        if( i > getByte(global_maxwantedlevel+gp_) ) 
          SetMaxWantedLevel(i);
        status = 1;
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        hex_marker_addx(global_maxwantedlevel+gp_, sizeof(char));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_maxwantedlevel+gp_, 0, memory_low, memory_high, "> maximum wanted level possible");
        #endif
      } break;
      
    case FUNC_SET: 
      /// set Cheat status
      status = defaultstatus;
      
      /// set Cheat's default value
      if( defaultval >= 0 && defaultval <= 6 ) 
        i = defaultval;
      else i = 0;
      
      /// reset maximum level possible to 6 (note: set on gameboot via "menu_setDefaults" but reset by game when game loaded, so fine)
      if( !status && pplayer > 0 ) 
        SetMaxWantedLevel(6);

      break;
  }
  
  return NULL;
}



/** Wanted level ************************************************************************************
 *
 * Completion:   99%
 * 
 * Todo:  - 
 * 
 * Notes:     LCS
        0x4E0 current direction in rad
        0x2F4 speed multiplier sort of
        
Walking Speed in:
CPed_ProcessControl
  CalculateNewVelocity  <-- calculated (writes value to ped handle)
  UpdatePosition      <-- applied (reads and removes from ped handle)
  
  
 I firstly hooked "CalculateNewVelocity" and adjusted the values there. 
 On real hardware only for VCS and when aiming at a PED + fight button would instantly crash the game?! And I don't know why..
 
 **************************************************************************************************************************************/
float walkspd_mult = 1.00f;
float pedwalkspd_mult = 1.00f;

void (*UpdatePosition)(int handle);
void UpdatePosition_patched(int handle) {

  if( walking_speed(FUNC_GET_STATUS,-1,-1,-1) != 0 ) { // only if CHEAT status is ON
    //logPrintf("walking_speed is enabled");
    if( handle == pplayer && walkspd_mult != 1.00f ) { // only for Player
      setFloat(handle+(LCS?0x4d8:0x794), getFloat(handle+(LCS?0x4d8:0x794)) * walkspd_mult);
      setFloat(handle+(LCS?0x4dc:0x798), getFloat(handle+(LCS?0x4dc:0x798)) * walkspd_mult);
    }    
  }
  
  if( pedwalking_speed(FUNC_GET_STATUS,-1,-1,-1) ) { // only if CHEAT status is ON
    //logPrintf("pedwalking_speed is enabled");
    if( handle != pplayer && pedwalkspd_mult != 1.00f ) { // only for non Player
      setFloat(handle+(LCS?0x4d8:0x794), getFloat(handle+(LCS?0x4d8:0x794)) * pedwalkspd_mult);
      setFloat(handle+(LCS?0x4dc:0x798), getFloat(handle+(LCS?0x4dc:0x798)) * pedwalkspd_mult);
    }    
  }

  UpdatePosition(handle); // then reads the floats and removes from obj again
}

void *walking_speed(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0;
  //static float boost = 1.00f; // now global
  static int tval;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      //logPrintf("walkspd_mult: %.2f, Ergebnis = %.4f, int = %i", walkspd_mult, walkspd_mult * 100.0f, (int)(walkspd_mult * 100.0f)); //rounding tests
      tval = (int)(walkspd_mult*100.0f);
      if( (tval % 5) != 0 ) tval++; // fix floats rounding error
      return (int*) tval; 
    
    case FUNC_GET_STRING:
      sprintf(retbuf, "%.02fx", walkspd_mult);
      return (void*)retbuf;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 

      } else if( keypress == PSP_CTRL_CIRCLE ) { // SQUARE
        walkspd_mult = 1.00f;
        status = 0;

      } else if( keypress == PSP_CTRL_LEFT && walkspd_mult > 0.10f ) { // LEFT
        walkspd_mult-=0.05f;
        status = 1;
    
      } else if( keypress == PSP_CTRL_RIGHT && walkspd_mult < 10.00f ) { // RIGHT
        walkspd_mult+=0.05f;
        status = 1;
      } break;
      
    case FUNC_SET: 
      /// set Cheat status
      status = defaultstatus;
      
      /// set Cheat's default value
      if( defaultval > 0 && defaultval <= 2000 ) { // 20.0f is max here
        walkspd_mult = (float)((float)(defaultval)/100.0f);
      } else walkspd_mult = 1.0f;
      break;
  }
  
  return NULL;
}
void *pedwalking_speed(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0;
  //static float boost = 1.00f; // now global
  static int tval;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      tval = (int)(pedwalkspd_mult*100.0f);
      if( (tval % 5) != 0 ) tval++; // fix floats rounding error
      return (int*) tval; 
    
    case FUNC_GET_STRING:
      sprintf(retbuf, "%.02fx", pedwalkspd_mult);
      return (void*)retbuf;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; 

      } else if( keypress == PSP_CTRL_CIRCLE ) { // SQUARE
        pedwalkspd_mult = 1.00f;
        status = 0;

      } else if( keypress == PSP_CTRL_LEFT && pedwalkspd_mult > 0.10f ) { // LEFT
        pedwalkspd_mult-=0.05f;
        status = 1;
    
      } else if( keypress == PSP_CTRL_RIGHT && pedwalkspd_mult < 10.00f ) { // RIGHT
        pedwalkspd_mult+=0.05f;
        status = 1;
      } break;
      
    case FUNC_SET: 
      /// set Cheat status
      status = defaultstatus;
      
      /// set Cheat's default value
      if( defaultval > 0 && defaultval <= 2000) { // 20.0f is max here
        pedwalkspd_mult = (float)((float)(defaultval)/100.0f);
      } else pedwalkspd_mult = 1.0f;
      break;
  }
  
  return NULL;
}


/** World Time ************************************************************************************
 *
 * Completion:   /
 * 
 * Todo:  - it works but there must be a native freeze time like in E3 mode
 *        - still fucks up weather
 * 
 * Notes:     /
 **************************************************************************************************************************************/
void *world_time(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0;
  static char current_hour = 0; 
  static char current_min = 0;
  static char current_sec = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_GET_STRING:
      sprintf(retbuf, "%02i:%02i:%02i", current_hour, current_min, current_sec);
      return (void*)retbuf;
    
    case FUNC_APPLY: // because load new game will wipe the high 0xFF -> so continously set AND..
      setClockFreeze(ON);
      break;  
      
    case FUNC_CHECK:
      //if( !status ) { ..check here for new time if game loaded or started with cheat activated or changed by game in a mission or something
        current_hour = getClockHours();
        current_min = getClockMinutes();
        current_sec = getClockSeconds();
      //} 
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          worldtime_disable:
          setClockFreeze(OFF);
          status = 0;
        } else {
          worldtime_enable:
          //setClockFreeze(ON); // FUNC_APPLY will take care now
          status = 1;
        }  

      } else if( keypress == PSP_CTRL_LEFT ) { // LEFT
        if( current_hour <= 0 ) 
          current_hour = 23;
        else current_hour--;
        setClockTime(current_hour, current_min, current_sec);
        
      } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
        if( current_hour >= 23 ) 
          current_hour = 0;
        else current_hour++;
        setClockTime(current_hour, current_min, current_sec);
      
      } else if( keypress == PSP_CTRL_SQUARE ) {
        setClockTime(current_hour, 0x00, 0x00);
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        //int gp_tmp = (LCS ? 0 : gp);
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_clockmultiplier + gp_ + 0x4, sizeof(char)); // hour value
        hex_marker_addx(global_clockmultiplier + gp_ + 0x5, sizeof(char)); // minute value
        hex_marker_addx(global_clockmultiplier + gp_ + 0x6, sizeof(char)); // second value
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_clockmultiplier+ gp_ + 0x4, 0, memory_low, memory_high, "> game clock");
        #endif
      } break;
      
    case FUNC_SET:
      status = defaultstatus;
      if( pplayer > 0 ) { // check if game in playable state
        if( status ) 
          goto worldtime_enable;
        else goto worldtime_disable;
      } break;
  }
  
  return NULL;
}


/** world_liftcontrol ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        
 * Notes:     
 *   global_bridgeState:
 *   0 = is up permanent
 *   1 = is up (bell)
 *   2 = going down (bell)
 *   3 = is down
 *   4 = about to go up (bell)
 *   5 = going up
 *   6 = is down permanent
 **************************************************************************************************************************************/
void *world_liftcontrol(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, state = 1;
  //const char *list_statenames[] = { "up permanently", "up", "descending", "down", "about to ascend", "ascending", "down permanently" };
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_GET_STRING: 
      sprintf(retbuf, " %s", translate_string(state ? "down" : "up"));
      return retbuf;
      
    case FUNC_APPLY:
      //setInt(global_bridgeState, state);
      SetBridgeState(state ? 6 : 0);
      break;  
      
    case FUNC_CHECK:
      if( !status ) {
        int temp = getInt(global_bridgeState);
        if( temp == 6 || temp == 4 || temp == 3 ) {
          state = 1; // down
        } else state = 0; // up
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          setInt(global_bridgeState, 3); // to get back into cycle 
          SetBridgeState(2);
          status = 0;
        } else status = 1;

      } else if( keypress == PSP_CTRL_LEFT ) {   
        if( status ) 
          state = 1 - state;
      
      } else if( keypress == PSP_CTRL_RIGHT ) {   
        if( status ) 
          state = 1 - state;
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_bridgeState, sizeof(int));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_bridgeState, 0, memory_low, memory_high, "> bridge state");
        #endif
      } 
      
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
  
  return NULL;
}


/** world_realtimeclock ********************************************************************************************
 *
 * Completion:  99%
 * 
 * Todo:  - 
 *        
 * Notes:   might break missions ?!
 **************************************************************************************************************************************/
void *world_realtimeclock(int calltype, int keypress, int defaultstatus) {
  static int status;
  static pspTime timetest;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_APPLY:
      setClockMultiplier(60000); //real time
      break;

    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          realtime_disable:
          setClockMultiplier(1000); // default
          setClockFreeze(OFF); // fix (copies back gametimer)
          status = 0;
      
        } else {
          realtime_enable:
          setClockMultiplier(60000); // real time
          status = 1;
        }        

      } else if( keypress == PSP_CTRL_SQUARE ) { // SQUARE
        /// Set PSP SystemTime as Gametime
        sceRtcGetCurrentClockLocalTime(&timetest); // https://github.com/pspdev/pspsdk/blob/master/src/rtc/psprtc.h
        setClockTime((char)timetest.hour, (char)timetest.minutes, (char)timetest.seconds);  

      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        //int gp_tmp = (LCS ? 0 : gp);
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_clockmultiplier + gp_, sizeof(int));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_clockmultiplier+ gp_, 0, memory_low, memory_high, "> game time multiplier");
        #endif
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      if( pplayer > 0 ) { // check if game in playable state
        if( status ) 
          goto realtime_enable;
        else goto realtime_disable;
      } break;
  }
  
  return NULL;

}

/** no_cheating_warning ********************************************************************************************
 *
 * Completion:  100 % :)
 * 
 * Todo:  - 
 *        - 
 *        
 * Notes:   When CHeats are used two globals (bool & counter) are set and will be saved / loaded from savegame!
 **************************************************************************************************************************************/
void *no_cheating_warning(int calltype, int keypress, int defaultstatus) {
  static int status;
  //int gp_tmp = (LCS ? 0 : gp); //to minimize code
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_GET_STRING: 
      if( status ) {
        sprintf(retbuf, " (%i used)", getInt(global_cheatusedcounter + gp_) / 1000);
        return retbuf;
      } else return "";
      
    case FUNC_CHECK:
      if( getByte(global_cheatusedboolean + gp_) == 1 || getInt(global_cheatusedcounter + gp_) > 0 ) {
        status = 1;
      } else status = 0;
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        setByte(global_cheatusedboolean + gp_, 0);
        setInt(global_cheatusedcounter + gp_, 0);
        status = 0;
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_cheatusedboolean + gp_, sizeof(char));
        hex_marker_addx(global_cheatusedcounter + gp_, sizeof(int));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_cheatusedboolean+ gp_, 0, memory_low, memory_high, "> cheatcode used boolean (IN THIS PLAY SESSION)");
        #endif
      } break;
      
  }
   
  return NULL;
  
}


/** Default Radio Station *****************************************************************************************************************************
 *
 * Completion:   100%
 * 
 * Todo:  - 
 *        -
 * 
 * Notes:     
 ******************************************************************************************************************************************************/

void *traffic_radiostation(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0;
  static int i = 0; // default radio station
  static int j;
  
  int base = -1;
  
  switch( calltype ) {
    
    case FUNC_GET_STATUS:
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING:
      sprintf(retbuf, "%s", getRadioStationName(i));
      return (void *)retbuf;

    case FUNC_APPLY:
      base = vehicles_base;
      for( j = 0; j < vehicles_max; j++, base += var_vehobjsize ) {
        if( base != pcar ) { //ignore player's current vehicle
          setVehicleRadioStation(base, i);
        }
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
        if( i < var_radios ) i++;
        
      } else if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; // switch status
      }
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      if( defaultval >= 0 ) 
        i = defaultval;
      else 
        i = var_radios; // default
      if( i > var_radios ) 
        i = var_radios; // fix for bad config value
      break;
  }  

  return NULL;
}


/** Water level *****************************************************************************************************************************
 *
 * Completion:   60%
 * 
 * Todo:  - VCS: far water level
 *        - VCS: reset water areas (0x00 back to 0x80)
 *        - LCS: map border level
 *
 *        - onetimeapply solution: could be set a little earlier once water.dat is loaded and thus before spawing
 * 
 * Notes:     
 ******************************************************************************************************************************************************/
void *world_waterlevel(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0; 
  static float level = 0.00f; // default
  static int onetimeapply = 0; // see FUNC_APPLY, FUNC_SET
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*) (int)((level*10)+1000.00f); // so that its positive and both .XX are saved in int (ugly) but whatever -> 1000 +- value
    
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%.2f", level);
      return (void *)retbuf;

    case FUNC_APPLY: // this is a special case as waterlevel can't be set via config restore because "menu_setDefaults" is executed before water.data is loaded in memory -> ugly fix: on-time-apply in FUNC_APPLY
      if( onetimeapply ) {
        setWaterLevel(level);
        onetimeapply = 0;
      }
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && level > -100.00 ) { // LEFT
        level -= 0.1;
        setWaterLevel(level);
        status = 1;
        
      } else if( keypress == PSP_CTRL_RIGHT && level < 150.00 ) { // RIGHT
        level += 0.1;
        setWaterLevel(level);
        status = 1;
        
      } else if( keypress == PSP_CTRL_CROSS ) { // CROSS
        if( status ) {
          resetWaterLevel();
          status = 0;
        } else {
          setWaterLevel(level);
          status = 1;
        }
      
      } else if( keypress == PSP_CTRL_CIRCLE ) {
        status = 0;
        level = LCS ? 0.00f : 6.00f;
        resetWaterLevel();
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXEDITOR  
        hexeditor_create(LCS ? global_ptr_water : global_ptr_water+gp, 0, memory_low, memory_high, "> water level");
        #endif
      } 
      
      break;
      
    case FUNC_SET:
      
      status = defaultstatus;
          
      if( defaultval >= 500 && defaultval <= 2000 ) {  // very rough values for bounds check
        level = (float)((float)(defaultval-1000)/10.0f);
        //if(status && pplayer > 0) // only set when game active and cheat on
        //  setWaterLevel(level);
        //else
          if( status )onetimeapply = 1;
        
      } else { 
        level = LCS ? 0.00f : 6.00f; // set default height
        if( !status && pplayer > 0 ) // only reset when game active and cheat off
          resetWaterLevel();
      }
      break;
  }  

  return NULL;
}


#ifdef PREVIEW
/** Wave height *****************************************************************************************************************************
 *
 * Completion:   
 * 
 * Todo:  - VCS small waves?! and no higher than 1.00f
 * 
 * Notes: -0.50f == no waves LCS
 * 
 * Sunny 0.25  00 00 80 3E
 * Cloud 0.70  33 33 33 3F
 * Rainy 1.00  00 00 80 3F
 ******************************************************************************************************************************************************/
void *world_waveheight(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0; 
  static float level = 0.00f; // default
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*) (int)((level*100)+1000.00f); // so that its positive and both .XX are saved in int (ugly) but whatever -> 1000 +- value
    
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%.2f", level + (LCS ? 0.50f : 0)); // so that no waves is at 0
      return (void *)retbuf;

    case FUNC_APPLY:
      setWindClipped(level); // necessary at all?
      break;
    
    case FUNC_CHECK: 
      if( status == 0 ) { 
        ///// patched out before so taken care of here ///////// see 0x00130820 @ LCS US v3
    setWindClipped(1.0f);
      if( getWind() <= 1.0)
          setWindClipped(getWind());
    ////////////////////////////////////////////////////////
    level = getWind();
      } 
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && level > (LCS ? -0.50f : 0.00f) ) { // LEFT
        level -= 0.05f;
        status = 1;
        if( LCS && level <= -0.50f ) level = -0.50f; // small fix
        if( VCS && level <= 0.00f ) level = 0.00f; // small fix
        
      } else if( keypress == PSP_CTRL_RIGHT && level < 50.00f ) { // RIGHT
        level += 0.05f;
        status = 1;
        
      } else if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status; // switch status
      
      } else if( keypress == PSP_CTRL_CIRCLE ) {
        status = 0;
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXEDITOR  
        hexeditor_create(LCS ? global_WindClipped : global_WindClipped+gp, 0, memory_low, memory_high, "> wave height");
        #endif
      } 
      
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      if( status ) {
        level = (float)((float)(defaultval-1000)/100.0f);
      }
      break;
  }  

  return NULL;
}
#endif

/******************************************************************************************************************************************************
 *
 * Completion:   
 * 
 * Todo:  - 
 *        -
 * 
 * Notes:     
 ******************************************************************************************************************************************************/
void *pickup_spawner(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int i = 0;
  static float x = 0, y = 0, z = 0;
  
  switch( calltype ) {
    
    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return (void *)(LCS ? lcs_pickups[i].name : vcs_pickups[i].name);
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
        if( LCS && i < lcs_pickupsize ) i++;
        if( VCS && i < vcs_pickupsize ) i++;
        
      } else if( keypress == PSP_CTRL_CROSS || keypress == PSP_CTRL_SQUARE ) { 
        
        /// calculate coordinate in front of player
        x = getFloat(pplayer+0x30) + ( cos( getFloat(pplayer+(LCS?0x4E0:0x8D0)) + (M_PI/2) ) * 2 );
        y = getFloat(pplayer+0x34) + ( sin( getFloat(pplayer+(LCS?0x4E0:0x8D0)) + (M_PI/2) ) * 2 );
        z = getFloat(pplayer+0x38);
        
        char type = 0x3; // (LCS ? lcs_pickups[i].type : vcs_pickups[i].type);
        if( keypress == PSP_CTRL_SQUARE ) {
          type = LCS ? 0xF : 0xB; /// to NOT make it re-appear / a permanent pickup
        }
        
        /// money needs to be type 0x8 to work though
        if( (LCS && lcs_pickups[i].id == 0x20A) || (VCS && vcs_pickups[i].id == 0x1B5) )
          type = 0x8;
        
        spawnPickup(LCS ? lcs_pickups[i].id : vcs_pickups[i].id, type, LCS ? lcs_pickups[i].amount : vcs_pickups[i].amount, x, y, z); //spawn it!            
        
      } 
      break;
      
    case FUNC_SET:
      if( defaultval >= 0 ) 
        i = defaultval;
      else i = 0; // default
      if( i > (LCS ? lcs_pickupsize : vcs_pickupsize) )
        i = 0; // fix for bad config value
      break;
  }  

  return NULL;
}


/*****************************************************************************************************************************************************
 *
 * Completion:   
 * 
 * Todo:  - 
 *        -
 * 
 * Notes:     
 ******************************************************************************************************************************************************/
char *lcs_playerModels[] = {  // 8 chars max
  "plr",     //     "Leone Suit"
  "plr2",    // 128 "Lawyer"
  "plr3",    // 129 "Casual"
  "plr4",    // 130 "Chauffeur"
  "plr5",    // 131 "Overall"
  "plr6",    // 132 "Tuxedo"
  "plr7",    // 133 "Avengin Angels"
  "plr8",    // 134 "The King of Rockn Roll"
  "plr9",    // 135 "Cocks Mascot"
  "plr10",   // 136 "Boxer Shorts"
  "plr11",   // 137 "Super Hero"
  "plr12",   // 138 "Dragon"
  "plr13",   //
  "plr14",   //
  "plr15",   //
  "plr16",   //
  
  "sal_01",   // 65
  "ray_01",   // 66
  
  "vinc_01",  // 72
  "grdang1",  // 73
  "ned_01",   // 74
  "mastor",   // 75
  "tool_01",  // 76
  
  "grdang2",  // 78
  "m_hole",   // 79
//"franfor",  // 80 crash
  "kazuki",   // 81
  "paulie",   // 82
  "hopper",   // 83
  "avery",    // 84
  "casa",     // 85
  "baby",     // 86
  "mcaffry",  // 87
  
  "eight",    // 92
  "mar_01",   // 93
  "love_01",  // 94
  "mickeyh",  // 95
  "wayne",    // 96
  "biker1",   // 97
  "biker2",   // 98
  "donovan",  // 99
  "kas_01",   // 100
  "wkas_01",  // 101
  
  "jdmade",   // 104
  
  "thug_01",  // 106
  "thug_02",  // 107
  
//"camp_man", // 109 error
//"camp_wom", // 110 error
  "miguel",   // 111
  "deliass",  // 112
  "hobo_01",  // 113
  
//"cop",      // 114 error
//"swat",     // 115
//"fbi",      // 116
//...
//"hitman",   // 122
//"gang01",   // 123
//"gang02",   // 124
  
  "holejog",  // 125
  "chauff",   // 126
  
  "philc",    // 139
  "grease",   // 140
  "gun_ass",  // 141
  "jd_shot",  // --
  "bodybag",  // --
  "sal_con",  // 142
  
  "PLAYER",   // early "PLR" (beta naming)
  "TONI_C1",  // early "PLR3" (beta naming) "the dummy"
}; // VCS via IDE (can even load all peds this way)

/// only special chars for LCS and very experimental
void *player_model(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int i = 0;
  static int list_size = (sizeof(lcs_playerModels)/sizeof(*lcs_playerModels))-1;
  static char buf[16] = "";
  
  switch( calltype ) {
    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      sprintf(buf, "%s", LCS ? lcs_playerModels[i] : vcs_getNameForPedViaID(i));
      return (void *)(buf);

        
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { 
        if( !pcar ) {
          /// either via functions
          SetActorSkinTo(pplayer, buf); // SetActorSkinTo
          LoadAllModelsNow(0); // LoadAllModelsNow
          RefreshActorSkin(pplayer); // RefreshActorSkin
      
          /// or via script (for ULUX todo?)
          /* static u8 script_loadmodel[] = {  // must be static for CustomScriptExecut()!
            /// load_and_launch_mission_internal
            0x1C, 0x04, 
            0x07, // byte
            0x3F, // mission_number
            
            /// terminate_this_script
            0x4E, 0x00
          }; CustomScriptExecute((int)&script_loadmodel); // make game execute it */
        }
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0) { // LEFT
        i--;
        if( VCS ) 
          while( getAddressOfIdeSlotForID(i) <= 0 ) // no address means empty IDE slot
            i--;
      
      } else if( keypress == PSP_CTRL_RIGHT && i < (LCS ? list_size : 0xA7) ) { // RIGHT (using getLastIdeOfType(MODELINFO_PED) will make SetActorSkinTo crash the game ?!?!?!?!?)
        i++;
        if( VCS ) 
          while( getAddressOfIdeSlotForID(i) <= 0 ) // no address means empty IDE slot
            i++;
      
      } break;
      
    case FUNC_SET:
      if( defaultval >= 0 ) 
        i = defaultval;
      else i = 0; // first ped is first IDE for both games
      if( i > (LCS ? list_size : 0xA7) ) 
        i = 0; // fix for bad config value
      break;
  }
  
  return NULL;
}


/** Vehicle Spawner ***********************************************************************************************************************************
 *
 * Completion:   95 %
 * 
 * Todo:  - 
 *        - check for blocking vehicle and unload (like debug version)
 * 
 * Notes:     
 ******************************************************************************************************************************************************/
void *vehicle_spawner(int calltype, int keypress, int defaultstatus, int defaultval) {
  static short id = 172; // default
  static float x = 0, y = 0, z = 0, deg = 0;
  #ifndef DEBUG
  static int i = 0;
  static short blacklist_lcs[] = { 0xC0, 0xC5, 0xC6, 0xC8, 0xC9 }; // FERRY, TRAIN, HELI, AEROPL, DODO
  static short blacklist_vcs[] = { 0x118 }; // AEROPL
  #endif
  static int status = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*)(int)id;
    
    case FUNC_GET_STRING:
      sprintf(retbuf, "%s", getRealVehicleNameViaID(id));
      if( retbuf[0] == '\0' ) // some vehicles don't have translations..
        sprintf(retbuf, "%s", getGxtIdentifierForVehicleViaID(id)); // ..use the GXT identifier-name then
      return (void *)retbuf;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && id > getFirstIdeOfType(MODELINFO_VEHICLE) ) { // LEFT
        id--;
      #ifndef DEBUG
      for( i = 0; i < ((LCS ? sizeof(blacklist_lcs) : sizeof(blacklist_lcs))/sizeof(blacklist_lcs[0])); i++ ) { // skip blacklisted
         if( id == (LCS ? blacklist_lcs[i] : blacklist_vcs[i]) ) {
              id--;
            i = 0;
           }
        } if( id < getFirstIdeOfType(MODELINFO_VEHICLE) ) id+=2; // first is blacklisted, go to second
      #endif
      
      } else if( keypress == PSP_CTRL_RIGHT && id < getLastIdeOfType(MODELINFO_VEHICLE) ) {
        id++;
        #ifndef DEBUG
      for( i = 0; i < ((LCS ? sizeof(blacklist_lcs) : sizeof(blacklist_lcs))/sizeof(blacklist_lcs[0])); i++ ) { // skip blacklisted
         if( id == (LCS ? blacklist_lcs[i] : blacklist_vcs[i]) ) {
              id++;
            i = 0;
           }
        } if( id > getLastIdeOfType(MODELINFO_VEHICLE) ) id-=2; // last is blacklisted, go to pre-last
      #endif
      
      } else if( keypress == PSP_CTRL_SQUARE ) { // "become vehicle"
        
        if( !pcar ) {
          /*** LCS ****************************************************
          024C: request_model 172 
          0390: load_all_models_now 
          0054: get_player_coordinates $PLAYER_CHAR store_to 0@ 1@ 2@ 
          0175: get_player_heading $PLAYER_CHAR store_to 3@ 
          02D3: get_ground_z_for_3d_coord 0@ 1@ 2@ store_to 4@ 
          00A5: create_car 172 at 0@ 1@ 4@ store_to 5@ 
          017A: set_car_heading 5@ to 3@ 
          036E: warp_char_into_car $PLAYER_CHAR to 5@ 
          004E: terminate_this_script
          ************************************************************/
          /*
          4C 02 08 AC 00 
          90 03 
          54 00 CE 18 0C 0D 0E 
          75 01 CE 18 0F 
          D3 02 0C 0D 0E 10
          A5 00 08 AC 00 0C 0D 10 11 
          7A 01 11 0F 
          6E 03 CE 18 11 
          4E 00          
          */
          
          /*** VCS ****************************************************
          0160: request_model 162 
          0228: load_all_models_now 
          0043: get_char_coordinates $PLAYER_CHAR store_to 0@ 1@ 2@ 
          00CE: get_char_heading $PLAYER_CHAR store_to 3@ 
          01BB: get_ground_z_for_3d_coord 0@ 1@ 2@ store_to 4@ 
          0048: create_car 162 at 0@ 1@ 4@ store_to 5@ 
          00D1: set_car_heading 5@ to 3@ 
          021C: warp_char_into_car $PLAYER_CHAR to 5@ 
          0023: end_thread
          ************************************************************/
          
          static u8 script_becomevehicle[] = { // must be static for CustomScriptExecut()! 
            /// request_model
            0x4C, 0x02,
            0x08, // type = short
            0xAC, 0x00, // vehicle_id
            
            /// load_all_models_now
            0x90, 0x03,  // Opcode: 0390
            
            /// get_player_coordinates
            0x54, 0x00,
            0xCE, 0x18,  // $PLAYER_CHAR 536
            0x0C, //0@
            0x0D, //1@
            0x0E, //2@
                
            /// get_player_heading
            0x75, 0x01,
            0xCE, 0x18, // $PLAYER_CHAR
            0x0F,  // 3@ (store_to)
            
            /// get_ground_z_for_3d_coord
            0xD3, 0x02,
            0x0C, // 0@
            0x0D, // 1@
            0x0E, // 2@
            0x10, // 4@ (store_to)
            
            /// create_car
            0xA5, 0x00,
            0x08, // type = 
            0xAC, 0x00, //vehicle_id
            0x0C, //0@
            0x0D, //1@
            0x10, //4@
            0x11, //5@ (store_to)
            
            /// set_car_heading
            0x7A, 0x01,
            0x11, // 5@
            0x0F, // 3@
            
            /// WARP_PLAYER_INTO_CAR
            0x6E, 0x03,
            0xCE, 0x18, // $PLAYER_CHAR 536
            0x11,      // 5@
            
            /// terminate_this_script
            0x4E, 0x00 // Opcode: 004E
          };
          
          if( VCS ) { // overwrite opcodes
            writeShort(&script_becomevehicle[0], 0x0160);
            writeShort(&script_becomevehicle[5], 0x0228);
            writeShort(&script_becomevehicle[7], 0x0043);
            writeShort(&script_becomevehicle[14], 0x00CE);
            writeShort(&script_becomevehicle[19], 0x01BB);
            writeShort(&script_becomevehicle[25], 0x0048);
            writeShort(&script_becomevehicle[34], 0x00D1);
            writeShort(&script_becomevehicle[38], 0x021C);
            writeShort(&script_becomevehicle[43], 0x0023);
            
            /// update $PLAYER_CHAR
            writeShort(&script_becomevehicle[9], 0x0ED0);
            writeShort(&script_becomevehicle[16], 0x0ED0);
            writeShort(&script_becomevehicle[40], 0x0ED0);
          }
          
          if( VCS && mod_text_size == 0x00377D30 ) { // fix for ULUS 1.01
            /// update $PLAYER_CHAR (is $759)
            writeShort(&script_becomevehicle[9], 0xF7CF);
            writeShort(&script_becomevehicle[16], 0xF7CF);
            writeShort(&script_becomevehicle[40], 0xF7CF);
          }

          if( LCS && mod_text_size == 0x00320A34 ) { // fix for ULUS 1.02
            /// update $PLAYER_CHAR (is $534)
            writeShort(&script_becomevehicle[9], 0x16CE);
            writeShort(&script_becomevehicle[16], 0x16CE);
            writeShort(&script_becomevehicle[40], 0x16CE);
          }
          
          writeShort(&script_becomevehicle[3], id); // insert vehicle_id
          writeShort(&script_becomevehicle[28], id); // insert vehicle_id
          
          CustomScriptExecute((int)&script_becomevehicle); // make game execute it
          
          closeMenu();
        }
        
      } else if( keypress == PSP_CTRL_CROSS ) {

        /// calculate coordinate in front of player
        x = getFloat(pplayer+0x30) + ( cos( getFloat(pplayer+(LCS?0x4E0:0x8D0)) + (M_PI/2) ) * (pcar ? 6 : 4) ); // 4 adjusts distance
        y = getFloat(pplayer+0x34) + ( sin( getFloat(pplayer+(LCS?0x4E0:0x8D0)) + (M_PI/2) ) * (pcar ? 6 : 4) ); // more disctance when in vehicle
        z = getFloat(pplayer+0x38); // z will be calculated by game though (via find ground)
                    
        /// calc degree (with driver door facing player!)
        deg = (180.0f / M_PI) * getFloat(pplayer + (LCS ? 0x4E0 : 0x8D0)) + ((getFloat(pplayer + (LCS ? 0x4E0 : 0x8D0)) < 0.00f) ?  360.0f : 0.0f);
        deg += 90.0f;
        if( deg > 360.0f ) 
          deg -= 360.0f;
          
        /*** LCS ****************************************************
        024C: request_model 162 
        0390: load_all_models_now 
        02D3: get_ground_z_for_3d_coord X Y Z store_to 0@ 
        00A5: create_car 162 at X Y 0@ store_to 1@ 
        017A: set_car_heading 1@ to DEG
        020F: lock_car_doors 1@ mode 1
        004E: terminate_this_script
        ************************************************************/
        
        /*** VCS ****************************************************
        0160: request_model 162 
        0228: load_all_models_now 
        01BB: get_ground_z_for_3d_coord X Y Z store_to 0@ 
        0048: create_car 162 at X Y 0@ store_to 1@ 
        00D1: set_car_heading 1@ to DEG
        013A: lock_car_doors 1@ mode 1
        0023: terminate_this_script
        ************************************************************/

        static u8 script_spawnvehicle[] = { // must be static for CustomScriptExecut()!
          /// request_model
          0x4C, 0x02,
          0x08,     // type = short
          0x00, 0x00, // vehicle_id
          
          /// load_all_models_now
          0x90, 0x03,  // Opcode: 0390
              
          
          /// get_ground_z_for_3d_coord
          0xD3, 0x02,
          0x09, 
          0x00, 0x00, 0x00, 0x00, // for x
          0x09, 
          0x00, 0x00, 0x00, 0x00, // for y
          0x09, 
          0x00, 0x00, 0x00, 0x00, // for z
          0x0C, // 0@ (store_to)
          
          /// create_car
          0xA5, 0x00,
          0x08, // type = 
          0x00, 0x00, // vehicle_id
          0x09, 
          0x00, 0x00, 0x00, 0x00, // for x
          0x09, 
          0x00, 0x00, 0x00, 0x00, // for y
          0x0C, // 0@
          0x0D, // 1@ (store_to)
          
          /// set_car_heading
          0x7A, 0x01,
          0x0D, // 1@
          0x09, 
          0x00, 0x00, 0x00, 0x00, // deg
          
          /// lock_car_doors (mode 1)
          0x0F, 0x02, 
          0x0D, // 1@
          0x07, 0x01, // mode 1
          
          /// terminate_this_script
          0x4E, 0x00 // Opcode: 004E
        };
        
        if( VCS ) { // overwrite opcodes
          writeShort(&script_spawnvehicle[0], 0x0160);
          writeShort(&script_spawnvehicle[5], 0x0228);
          writeShort(&script_spawnvehicle[7], 0x01BB);
          writeShort(&script_spawnvehicle[25], 0x0048);
          writeShort(&script_spawnvehicle[42], 0x00D1);
          writeShort(&script_spawnvehicle[50], 0x013A);
          writeShort(&script_spawnvehicle[55], 0x0023);
        }
        
        writeShort(&script_spawnvehicle[3], id); // insert vehicle_id
        writeShort(&script_spawnvehicle[28], id); // insert vehicle_id
          
        writeFloat(&script_spawnvehicle[10], x);
        writeFloat(&script_spawnvehicle[15], y);
        writeFloat(&script_spawnvehicle[20], z);
        
        writeFloat(&script_spawnvehicle[31], x);
        writeFloat(&script_spawnvehicle[36], y);
        
        writeFloat(&script_spawnvehicle[46], deg);
        
        CustomScriptExecute((int)&script_spawnvehicle); // make game execute it
          
      }
      break;
      
    case FUNC_SET:
      if( defaultval > 0 ) 
        id = defaultval;
    else id = 172;
      break;
  }  

  return NULL;
}


/*
Priority List:  teleport
--------------
LCS      VCS
yellow     pink  = Destination
blue     blue  = vehicle / ped
green    green  = Object / pickup
red      red    = Target to kill / destroy

if( no destination icons && pay'n'spray icon is global map visible bool )  ?

*/

/// in VCS icon might say ped although vehicle is mean where ped is passenger for example!
/// vcs races: blue arrows over other cars before destination sphere

void teleport_next_sub() {
  int i, type, slot, objbase, base = LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding);
  
  for( i = 0; i < var_radarblipslots; base+=var_radarblipslotsize, i++ ) {
    if( getMapiconIsVisible(base) && getMapiconID(base) == 0x00 ) { // && getMapiconColor(base) != 0x00 aka red (exclude targets) good idea?
      type = getMapiconType(base);
      slot = getMapiconLinkedObjectSlotNumber(base); // -1 if no slot when type is not an obj
      
      switch( type ) {
        case 0: // VCS only (Business)
          // TODO 
          break;
        
        case 1: // Vehicle
          if( slot >= 0 && slot <= vehicles_max ) {
            objbase = vehicles_base + (var_vehobjsize * slot);
            if( getVehicleObjectIsActive(objbase) ) {
              if( getVehicleDriverHandle(objbase) == 0 ) { // no driver! teleport to driver seat
                TaskCharWith(pplayer, 0x12, objbase); // this one only works from apply-loop hooked function AND within another function!!
                WarpPedIntoVehicle(pplayer, objbase); 
              } else { // teleport player on top of vehicle
                teleport(getFloat(objbase+0x30), getFloat(objbase+0x34), getFloat(objbase+0x38)+2.0f);
              }
              return;
            } else continue;
          }
          break;
        
        case 2: // Pedestrian
          if( slot >= 0 && slot <= peds_max ) {
            objbase = peds_base + (var_pedobjsize * slot);
            if( getPedObjectIsActive(objbase) ) {
              teleport(getFloat(objbase+0x30), getFloat(objbase+0x34), getFloat(objbase+0x38));
              return;
            } else continue;
          }
          break;
        
        case 3: // World Object  (can be pickup as well!)
          if( slot >= 0 && slot <= worldobjs_max ) {
            objbase = worldobjs_base + (var_wldobjsize * slot);
            if( getWorldObjectIsActive(objbase) ) {
              teleport(getFloat(objbase+0x30), getFloat(objbase+0x34), getFloat(objbase+0x38)+0.5f);
              return;
            } else continue;
          }
          break;
          
        
        default: // use coordinates in blip slot
          if( getFloat(base+(LCS ? 0x14 : 0x18)) != 0.0f ) { // z coordinate must be != 0
            teleport(getFloat(base+(LCS ? 0xC : 0x10)), getFloat(base+(LCS ? 0x10 : 0x14)), getFloat(base+(LCS ? 0x14 : 0x18)) + 1.00f);
            return;
          }
        
      }        
      
    }        
  }
}

void *teleport_next(int calltype, int keypress) {
  static int status = 0;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
    
    case FUNC_GET_STRING: 
      return "";
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS  
        status = 1;
      } break;
      
    case FUNC_APPLY: 
      status = 0; // apply once
      teleport_next_sub();
      break;
    
  }
  
  return NULL;
}


void teleport_mission() { // gets next mission host location from mapicon
  int i, base = LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding);
  
  for( i = 0; i < var_radarblipslots; base+=var_radarblipslotsize, i++ ) {
    if( getMapiconIsActive(base) && getMapiconType(base) == 0x05 ) {
      
      char icn = getMapiconID(base);
      
      if( (LCS && icn >= 31 && icn <= 40) || (VCS && icn >= 0x23 && icn <= 0x2E) ) {
        if( getFloat(base+(LCS ? 0x14 : 0x18)) != 0.0f ) { // z coordinate must be != 0
          teleport(getFloat(base+(LCS ? 0xC : 0x10)), getFloat(base+(LCS ? 0x10 : 0x14)), getFloat(base+(LCS ? 0x14 : 0x18)) + 1.00f);
          return;
        }
      }
    }
  }
}


/// TODO - get_ground_z_for_3d_coord (opcode: 02D3)
void teleport_marker() {
  int i, base = LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding);
  
  for( i = 0; i < var_radarblipslots; base+=var_radarblipslotsize, i++) {
    if( getMapiconIsActive(base) && getMapiconID(base) == (LCS ? 49 : 0x10) ) {
      teleport(getFloat(base+(LCS ? 0xC : 0x10)), getFloat(base+(LCS ? 0x10 : 0x14)), (LCS ? -150.0f : -250.0f)); // z is always 0 for marker sadly -> telep under map will reset player by game on street  TODO find z coordinate like debug build
      return;
    }        
  }  
}

/*
Priority List:  kill / destroy
--------------
LCS      VCS
red      red    = Target to kill / destroy
green    green  = Object / !pickup

*/
void kill_all_targets() {
  int i, type, slot, objbase, base = LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding);
  
  for( i = 0; i < var_radarblipslots; base+=var_radarblipslotsize, i++ ) {
    if( getMapiconIsActive(base) && getMapiconID(base) == 0x00 && getMapiconColor(base) == 0x00 ) { // red == target
      type = getMapiconType(base);
      slot = getMapiconLinkedObjectSlotNumber(base); // -1 if no slot when type is not an obj
      
      switch( type ) {
        case 1: /// Vehicle
          if( slot >= 0 && slot <= vehicles_max ) {
            objbase = vehicles_base + (var_vehobjsize * slot);
            if( getVehicleObjectIsActive(objbase) ) {
              setVehicleHealth(objbase, 0.0f);  // to be sure
              makeVehicleExplode(objbase);
              //alternatively get PED base from seat position and kill (TODO i guess?)
              //return;
            } //else continue;
          }
          break;
        
        case 2: /// Pedestrian
          if( slot >= 0 && slot <= peds_max ) {
            objbase = peds_base + (var_pedobjsize * slot);
            if( getPedObjectIsActive(objbase) ) {
              setPedHealthAndArmor(objbase, 0.0f, 0.0f);
              //return;
            } //else continue;
          }
          break;
        
        /* case 3: /// World Object
          if( slot >= 0 && slot <= worldobjs_max ) {
            objbase = worldobjs_base + (var_wldobjsize * slot);
            if( getWorldObjectIsActive(objbase) ) {
              
              return;
            } else continue;
          }
          break; */ 
      
      }        
    }        
  }
}


void freeze_all_targets() {
  int i, type, slot, objbase, base = LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding);
  
  for( i = 0; i < var_radarblipslots; base+=var_radarblipslotsize, i++ ) {
    if( getMapiconIsActive(base) && getMapiconID(base) == 0x00 && getMapiconColor(base) == 0x00 ) { // red == target
      type = getMapiconType(base);
      slot = getMapiconLinkedObjectSlotNumber(base); // -1 if no slot when type is not an obj
      
      switch( type ) {
        case 1: /// Vehicle
          if( slot >= 0 && slot <= vehicles_max ) {
            objbase = vehicles_base + (var_vehobjsize * slot);
            if( getVehicleObjectIsActive(objbase) ) {
              setPedOrVehicleFreezePosition(objbase, TRUE);
              //return;
            } //else continue;
          }
          break;
        
        case 2: /// Pedestrian
          if( slot >= 0 && slot <= peds_max ) {
            objbase = peds_base + (var_pedobjsize * slot);
            if( getPedObjectIsActive(objbase) ) {
              setPedOrVehicleFreezePosition(objbase, TRUE);
              //return;
            } //else continue;
          }
          break;
          
      }
    }
  }
}



const char *buttonCheatNames[] = { 
  "Spawn selected Vehicle", 
  "Toggle Hover Bike & Car", 
  "Teleport Destination", 
  "Kill / destroy all targets", 
  "Switch Radio Off", 
  "Switch Radio / Custom Tracks", 
  "Toggle FPS", 
  "Step through wall",
  "Toggle top down camera",
  "Crouch",
  "Reverse Gravity",
  "Toggle Gather Spell", 
  "Toggle Slowmo", 
  "Impulse", 
  "Jump with Vehicle", 
  "Lock Vehicle Doors", 
  "Unlock Vehicle Doors", 
  //Zoom out minimap
  //
  //
 };  

extern short alt_gravity_button_pressed; // ugly solution for Reverse Gravity
 
void buttonApply(int i) { // apply when button hold
  if( i == 10 ) { // Reverse Gravity    
    alt_gravity_button_pressed = 1;
    world_gravity(FUNC_CHANGE_VALUE, 42, 0, 0); // enable cheat
  } 
}

void buttonApplyOnce(int i) { // apply once on button press
  if( i == 0 ) { // spawn selected vehicle
    vehicle_spawner(FUNC_CHANGE_VALUE, PSP_CTRL_CROSS, 0, 0);
    //testfunc(9);
    
  } else if( i == 1 ) { // toggle hover bike & car
    hover_vehicle(FUNC_CHANGE_VALUE, PSP_CTRL_CROSS, 0);

  } else if( i == 2 ) { // teleport dest
    //teleport_next(FUNC_APPLY, 0);
    teleport_next_sub();
    
  } else if( i == 3 ) { // kill targets
    kill_all_targets();
    
  } else if( i == 4 ) { // switch radio off
    if( pcar )
      setRadioStationNow(var_radios); // last is OFF
    
  } else if( i == 5 ) { // toggle fps
    if( pcar ){
      if( getByte(getInt((LCS ? global_custrackarea : global_custrackarea+gp)) + 0x8) == (LCS ? 0x1 : 0xB) ) { // custom tracks toggle is ON
        setByte(getInt((LCS ? global_custrackarea : global_custrackarea+gp)) + 0x8, LCS ? 0x0 : 0xA); // disable custom tracks toggle
        if( VCS ) setRadioStationNow(0); // re-enable radio playback (todo instead of 0 get last radio station played?)
      } else {
        setByte(getInt((LCS ? global_custrackarea : global_custrackarea+gp)) + 0x8, LCS ? 0x1 : 0xB); // enable custom tracks toogle
        setByte(global_radioarea + (LCS ? 0x350 : 0x578), 0x0); // trigger playback
      }
    }
    
  } else if( i == 6 ) { // FPS toggle
    fps_toggle(FUNC_CHANGE_VALUE, PSP_CTRL_CROSS, 0);
    
  } else if( i == 7 ) { // step through wall
    stepthroughwall();
    
  } else if( i == 8 ) { // camera topdown
    camera_topdown(FUNC_CHANGE_VALUE, PSP_CTRL_CROSS, 0);
    
  } else if( i == 9 ) { // crouch
    if( !pcar && ((current_buttons & PSP_CTRL_RTRIGGER) == 0) ) { // R-Trigger is fix for manual aiming 
      if( isPedCrouching(pplayer) )
        TaskUnDuck(pplayer, 0x1);
      else TaskDuck(pplayer); 
    }
    
  } else if( i == 10 ) { 
    ///handled in buttonApply() !!!
        
  } else if( i == 11 ) { // Toggle GatherSpell
    gather_spell(FUNC_CHANGE_VALUE, PSP_CTRL_CROSS, 0);
    
  } else if( i == 12 ) { 
    if( gamespeed(FUNC_GET_STATUS, 0, 0, 0) == 0 ) {
      gamespeed(FUNC_SET, PSP_CTRL_CROSS, 1, 3);
    } else {
      gamespeed(FUNC_CHANGE_VALUE, PSP_CTRL_CIRCLE, 0, 0);
    }
    
  } else if( i == 13 ) { // Impulse
    int j, base;
    float radius = 50.0f; // in which force should be applied
    float x1 = getFloat(pobj+0x30);
    float y1 = getFloat(pobj+0x34);
    float z1 = getFloat(pobj+0x38);
  
    /// Vehicles
    base = vehicles_base;
    for( j = 0; j < vehicles_max; j++, base += var_vehobjsize ) {
      if( getVehicleObjectIsActive(base) && base != pcar ) { // ignore player's current vehicle
      float x2 = getFloat(base+0x30);
      float y2 = getFloat(base+0x34);
      float z2 = getFloat(base+0x38);
      if( checkCoordinateInsideArea(x1, y1, z1, x2, y2, z2, radius) ) { // is in radius
        /// "wake vehicle up" aka turn AI controlled "floating" to "physical" vehicle
        setVehicleMakePhysical(base);
        /// set vectors
        float distance = distanceBetweenCoordinates3d(x1, y1, z1, x2, y2, z2); 
        float forcemult = distance * 30.0f / radius; // the lower the faster
        float x_obj = (x2 - x1) / forcemult;
        float y_obj = (y2 - y1) / forcemult;
        //float z_obj = (z2 - z1) / forcemult;
        setFloat(base+(LCS?0x70:0x140), getFloat(base+(LCS?0x70:0x140)) + x_obj);
        setFloat(base+(LCS?0x74:0x144), getFloat(base+(LCS?0x74:0x144)) + y_obj);
        //setFloat(base+(LCS?0x78:0x148), getFloat(base+(LCS?0x78:0x148)) + z_obj);
        setFloat(base+(LCS?0x78:0x148), 0.2f);
      }
    }
  }
  
  /// Pedestrians (needs 2 loops currently, or double press) :/
    /*base = peds_base;
    for( j = 0; j < peds_max; j++, base += var_pedobjsize ) {
      if( getPedObjectIsActive(base) && base != pplayer ) { // ignore player
        float x2 = getFloat(base+0x30);
    float y2 = getFloat(base+0x34);
    float z2 = getFloat(base+0x38);
    if( checkCoordinateInsideArea(x1, y1, z1, x2, y2, z2, radius) ) { // is in radius
      float distance = distanceBetweenCoordinates3d(x1, y1, z1, x2, y2, z2); 
      float forcemult = distance * 30.0f / radius; // the lower the faster
      float x_obj = (x2 - x1) / forcemult;
      float y_obj = (y2 - y1) / forcemult;
      //float z_obj = (z2 - z1) / forcemult;
      setFloat(base+(LCS?0x70:0x140), getFloat(base+(LCS?0x70:0x140)) + x_obj);
      setFloat(base+(LCS?0x74:0x144), getFloat(base+(LCS?0x74:0x144)) + y_obj);
      //setFloat(base+(LCS?0x78:0x148), getFloat(base+(LCS?0x74:0x144)) + 0.2f);
      setFloat(base+(LCS?0x78:0x148), 0.2f);
    }
    }
  }*/
  
  /// World Objects
    /*base = worldobjs_base;
    for( j = 0; j < worldobjs_max; j++, base += var_wldobjsize ) {
      if( getWorldObjectIsActive(base) ) { // ignore player
        float x2 = getFloat(base+0x30);
    float y2 = getFloat(base+0x34);
    float z2 = getFloat(base+0x38);
    if( checkCoordinateInsideArea(x1, y1, z1, x2, y2, z2, radius) ) { // is in radius
      // make physical! todo
      
      float distance = distanceBetweenCoordinates3d(x1, y1, z1, x2, y2, z2); 
      float forcemult = distance * 30.0f / radius; // the lower the faster
      float x_obj = (x2 - x1) / forcemult;
      float y_obj = (y2 - y1) / forcemult;
      //float z_obj = (z2 - z1) / forcemult;
      setFloat(base+(LCS?0x70:0x140), getFloat(base+(LCS?0x70:0x140)) + x_obj);
      setFloat(base+(LCS?0x74:0x144), getFloat(base+(LCS?0x74:0x144)) + y_obj);
      //setFloat(base+(LCS?0x78:0x148), getFloat(base+(LCS?0x74:0x144)) + 0.2f);
      setFloat(base+(LCS?0x78:0x148), 0.2f);
    }
    }
  }*/
      
  } else if( i == 14 ) { 
    if( pcar && !isVehicleInAir( pcar ) && !isVehicleUpsideDown( pcar ) ) 
      setFloat(pcar+(LCS?0x78:0x148), 0.3f);
  
  } else if( i == 15 ) { // lock vehicle doors
    if( pcar )
      setVehicleDoorsLocked(pcar, TRUE);
  
  } else if( i == 16 ) { // unlock vehicle doors
    if( pcar )
      setVehicleDoorsLocked(pcar, FALSE);
  
  } else if( i == 17 ) { 
    setTimedTextbox("TODO", 3.0f);
  } 
}

void *up_button(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, i = 10; // default position  
  static int list_size = (sizeof(buttonCheatNames)/sizeof(*buttonCheatNames))-1;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;

    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return (void *)translate_string(buttonCheatNames[i]);
      
    case FUNC_APPLY:
      if( (pressed_buttons & PSP_CTRL_UP) && ((current_buttons & PSP_CTRL_LTRIGGER) == 0) && flag_menu_running == 0 && flag_menu_show == 1 ) { // flag_menu_stop will prevent going up in menu trigger it
        buttonApplyOnce(i); // pressed button (apply ONCE!)
      }
      if( (current_buttons & PSP_CTRL_UP) && ((current_buttons & PSP_CTRL_LTRIGGER) == 0) && flag_menu_running == 0 && flag_menu_show == 1 ) { // flag_menu_stop will prevent going up in menu trigger it
        buttonApply(i); // pressed button
      } 
      break;  
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT && i < list_size ) { // RIGHT
        i++;
      
      }   
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      i = 10;
      if( defaultval >= 0 && defaultval <= list_size ) 
        i = defaultval;
      break;
  }
  
  return NULL;
}
  
void *down_button(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, i = 9; // default position    
  static int list_size = (sizeof(buttonCheatNames)/sizeof(*buttonCheatNames))-1;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;

    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return (void *)buttonCheatNames[i];
      
    case FUNC_APPLY:
      if( (pressed_buttons & PSP_CTRL_DOWN) && ((current_buttons & PSP_CTRL_LTRIGGER) == 0) && flag_menu_running == 0 && flag_menu_show == 1 ) { // flag_menu_stop will prevent going up in menu trigger it
        buttonApplyOnce(i);
      }
      if( (current_buttons & PSP_CTRL_DOWN) && ((current_buttons & PSP_CTRL_LTRIGGER) == 0) && flag_menu_running == 0 && flag_menu_show == 1 ) { // flag_menu_stop will prevent going up in menu trigger it
        buttonApply(i);
      } 
      break;  
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT && i < list_size) { // RIGHT
        i++;
      
      }   
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      i = 9;
      if( defaultval >= 0 && defaultval <= list_size ) 
        i = defaultval;
      break;
  }
  
  return NULL;
}




void *touch_pedestrian(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, i = 0; // default position
  const char *list_names[] = { "kill instantly",    // 0
                               "replenish Health",  // 1
                               "freeze position",   // 2
                               "unfreeze position", // 3
                               "unload now",        // 4
                               #ifdef HEXEDITOR
                               "open in HexEditor", // 5
                               #endif
                             };
                
  static int list_size = (sizeof(list_names)/sizeof(*list_names))-1;
  static int addr;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;

    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return (void *)list_names[i];
      
    case FUNC_APPLY:
      addr = getObjectsTouchedObjectAddress(pobj);
      if( getTypeFromAddress(addr) == PEDESTRIAN ) {
        if( i == 0 ) { // 0
          setPedHealthAndArmor(addr, 0.0f, 0.0f);
          
        } else if( i == 1 ) { // 1
          setPedHealthAndArmor(addr, 100.0f, 100.0f);
          
        } else if( i == 2 ) { // 2
          setPedOrVehicleFreezePosition(addr, TRUE);
          
        } else if( i == 3 ) { // 3
          setPedOrVehicleFreezePosition(addr, FALSE);
          
        } else if( i == 4 ) { // 4
          if( LCS ) setPedFlagToUnload(addr);
          if( VCS ) {
            setFloat(addr+0x30, 2000.0f); // move far away
            setFloat(addr+0x34, 2000.0f); // 
            setFloat(addr+0x38, -250.0f); // move map bottom limit to make game take care of it automatically
          }
        /* } else if( i == 4 ) { // 4
          if( pcar ) { //player has car
            //TaskCharWith(addr, 0x11, pcar); // crashes
        } */
          
        } else if( i == 5 ) { // 5
          #ifdef HEXEDITOR
          if( flag_menu_running == 0 ) {
            hexeditor_create(addr, 1, addr, addr + var_pedobjsize, "> touched Pedestrian");
            flag_menu_start = 1;
          }
          #endif
        }
      }
      break;  
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT && i < list_size ) { // RIGHT
        i++;
      
      }   
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      i = 0;
      if( defaultval >= 0 && defaultval <= list_size ) 
        i = defaultval;
      break;
  }
  
  return NULL;
}

void *touch_vehicle(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, i = 0; // default position
  const char *list_names[] = { "make it float away",     // 0
                               "set it on fire",         // 1
                               "make it explode",        // 2
                               "unload it",              // 3
                               "lock the doors",         // 4
                               "unlock the doors",       // 5
                               "freeze position",        // 6
                               "unfreeze position",      // 7
                               "enter as passenger",     // 8
                               "make peds exit",         // 9
                               "warp into as spectator", // 10
                               "warp into as driver",    // 11
                               #ifdef HEXEDITOR
                               "open in HexEditor",      // 12
                               #endif
                             };
  static int list_size = (sizeof(list_names)/sizeof(*list_names))-1;
  static int addr;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;

    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return (void *)translate_string(list_names[i]);
      
    case FUNC_APPLY:
      addr = getObjectsTouchedObjectAddress(pobj);
      if( getTypeFromAddress(addr) == VEHICLE ) {
        if( i == 0 ) { // make it float away
          setVehicleGravityApplies(addr, FALSE); // gravity off  
          
        } else if( i == 1 ) { // set it on fire
          setVehicleHealth(addr, 0.0f); // zero health
          
        } else if( i == 2 ) { // make it explode
          makeVehicleExplode(addr);
          
        } else if( i == 3 ) { // unload it
          if( LCS ) setVehicleFlagToUnload(addr);
          if( VCS ) {
            setFloat(addr+0x30, 2000.0f); // move far away
            setFloat(addr+0x34, 2000.0f); // 
            setFloat(addr+0x38, -250.0f); // move map bottom limit to make game take care of it automatically
          }
        } else if( i == 4 ) { // lock the doors
          setVehicleDoorsLocked(addr, TRUE);
          
        } else if( i == 5 ) { // unlock the doors
          setVehicleDoorsLocked(addr, FALSE);
          
        } else if( i == 6 ) { // freeze position
          setPedOrVehicleFreezePosition(addr, TRUE);
        
        } else if( i == 7 ) { // unfreeze position
          setPedOrVehicleFreezePosition(addr, FALSE);
          
        } else if( i == 8 ) { // enter as passenger
          if( !pcar ) 
            TaskCharWith(pplayer, 0x11, addr);
          
        } else if( i == 9 ) { // make peds exit
          if( getVehicleDriverHandle(addr) ) 
            TaskCharWith(getVehicleDriverHandle(addr), 0x10, addr); // task exit vehicle
          
          if( getVehiclePassengerHandle(addr) ) 
            TaskCharWith(getVehiclePassengerHandle(addr), 0x10, addr); // task exit vehicle
          
          if( getVehicleBackseatLeftHandle(addr) ) 
            TaskCharWith(getVehicleBackseatLeftHandle(addr), 0x10, addr); // task exit vehicle
          
          if( getVehicleBackseatRightHandle(addr) ) 
            TaskCharWith(getVehicleBackseatRightHandle(addr), 0x10, addr); // task exit vehicle
          
          
        } else if( i == 10 ) { // warp into as spectator
          if( !pcar ) // OTHERWISE REMOTE CONTROLLED MULTIPLE VEHICLES
            WarpPedIntoVehicle(pplayer, addr); //hacky
          
        } else if( i == 11 ) { // warp into as driver
          if( !pcar ) { // OTHERWISE REMOTE CONTROLLED MULTIPLE VEHICLES
            TaskCharWith(pplayer, 0x12, addr);
            WarpPedIntoVehicle(pplayer, addr);
          }
        } else if( i == 12 ) { // open in HexEditor
          #ifdef HEXEDITOR
          if( flag_menu_running == 0 ) {
            hexeditor_create(addr, 1, addr, addr + var_vehobjsize, "> touched Vehicle");
            flag_menu_start = 1;
          }
          #endif
        }
      }
      break;  
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT && i < list_size ) { // RIGHT
        i++;
      
      }   
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      i = 0;
      if( defaultval >= 0 && defaultval <= list_size ) 
        i = defaultval;
      break;
  }
  
  return NULL;
}

void *touch_object(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, i = 0; // default position
  const char *list_names[] = { "unload it", 
                               #ifdef HEXEDITOR
                               "open in HexEditor" 
                               #endif
                             };
                
  static int list_size = (sizeof(list_names)/sizeof(*list_names))-1;
  static int addr;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;

    case FUNC_GET_VALUE: // for config
      return (int*)i;
    
    case FUNC_GET_STRING: 
      return (void *)list_names[i];
      
    case FUNC_APPLY:
      addr = getObjectsTouchedObjectAddress(pobj);
      if( getTypeFromAddress(addr) == WORLDOBJ ) { 
        if( i == 0 ) { // 0
          setFloat(addr+0x38, -150.0f); // just move it under map
          
        } else if( i == 1 ) { // 1
          #ifdef HEXEDITOR
          if( flag_menu_running == 0 ) {
            hexeditor_create(addr, 1, addr, addr + var_wldobjsize, "> touched World Object");
            flag_menu_start = 1;
          }
          #endif
        }
      }
      break;  
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
        
      } else if( keypress == PSP_CTRL_LEFT && i > 0) { // LEFT
        i--;
        
      } else if( keypress == PSP_CTRL_RIGHT && i < list_size) { // RIGHT
        i++;
      
      }   
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      i = 0;
      if( defaultval >= 0 && defaultval <= list_size ) 
        i = defaultval;
      break;
  }
  
  return NULL;
}




/********************
mark OBJECTS on radar
 - pickups
 - weapons like in multiplayer`? (with radius?)
 - stunt jumps -> VCS use icon! or green / red for completed or not?!
 - baloons (VCS) -> use icon!
 - players vehicle(s)
 - 
****/
void *markonmap(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status, j = 0, i = 0; // default position
  static int res = 0; 
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;

    case FUNC_GET_VALUE: // for config
      return (int*)j;
    
    case FUNC_GET_STRING: 
      if( j == 0 ) { // pickups
        snprintf(retbuf, sizeof(retbuf), translate_string("Pickups - %s (%i)"), translate_string((LCS ? lcs_pickups[i].name : vcs_pickups[i].name)), getPickupsActiveObjectsWithID((LCS ? lcs_pickups[i].id : vcs_pickups[i].id)) );
          
      } else if( j == 1 ) { // weapons in range
        if( status ) 
          snprintf(retbuf, sizeof(retbuf), translate_string("Weapons in range (%i)"), res);
        else snprintf(retbuf, sizeof(retbuf), translate_string("Weapons in range"));
        //res = 0; // if apply loop not updating
          
      } else if( j == 2 ) { // stunt jumps
        //sprintf(retbuf, "Unique Stunt Jumps");
          
      } else if( j == 3 ) { // baloons
        //sprintf(retbuf, "Baloons");
          
      } else if( j == 4 ) { // player's vehicles
        //sprintf(retbuf, "Player's Vehicle(s)");
      } 
      return (void *)retbuf;
      
    case FUNC_APPLY:
      removeAllCustomMapIcons(); // remove all custom created ones
      if( j == 0 ) { // pickups
        createCustomMapIconsForPickupsWithID((LCS ? lcs_pickups[i].id : vcs_pickups[i].id), 0, 4); //3 is white, 4 is yellow
        
      } else if( j == 1 ) { // weapons in range
        //drawString("todo - weapons in range", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 10.0f, AZURE );
        res = createCustomMapIconsForWeaponsInRange(150.0f);
        
      } else if( j == 2 ) { // stunt jumps
        //drawString("todo - stunt jumps", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 10.0f, AZURE );
        //createCustomMapIconsForUSJs();
        
      } else if( j == 3 ) { // baloons
        //drawString("todo - baloons", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 10.0f, AZURE );
        //createCustomMapIconsForBaloons();
        
      } else if( j == 4 ) { // player's vehicles
        //drawString("todo - player's vehicles", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 10.0f, AZURE );
        //createCustomMapIconsForPlayersVehicles();
        
      } 
      break;  
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
        removeAllCustomMapIcons();
        
      } else if( keypress == PSP_CTRL_SQUARE ) { // SQUARE
        if( j < 1 ) 
          j++; // adjust max options here
        else j = 0;
        if( LCS && j == 3 ) // skip VCS only stuff
          j++;
        removeAllCustomMapIcons();
        
      } else if( keypress == PSP_CTRL_LEFT ) { // LEFT
        if( j == 0 && i > 0) // pickups
          i--;
        //removeAllCustomMapIcons();
      } else if( keypress == PSP_CTRL_RIGHT /* && i < todo*/ ) { // RIGHT
        if( j == 0 && i < (LCS ? lcs_pickupsize : vcs_pickupsize) ) // pickups
          i++;
        //removeAllCustomMapIcons();
      }   
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      j = 0;
      if( defaultval >= 0  ) // todo < value check
        j = defaultval;
      break;
  }
  
  return NULL;
}



/** camera always centered  ********************************************************************************************
 *
 * Completion: 
 * 
 * Todo:  - 
 * 
 * Notes:   global_camera  
 **************************************************************************************************************************************/
void *camera_centered(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      if( pcar )
        setCameraCenterBehindPlayer();
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status;
    
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_camera + (LCS ? 0x1AA : 0x113), sizeof(float)); // not ulux
        #endif
        #ifdef HEXEDITOR
        hexeditor_create( global_camera + (LCS ? 0x1AA : 0x113), 0, memory_low, memory_high, "> centered");
        #endif
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


/** Field Of View *************************************************************************************************************************************
 *
 * Completion:   
 * 
 * Todo:  -  
 * 
 * Notes:     
 ******************************************************************************************************************************************************/

void *fieldofview(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0; 
  static float fov = 70.00f; // default
  
  switch( calltype ) {
    
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*) (int)fov; 
    
    case FUNC_GET_STRING: 
      sprintf(retbuf, "%.2f", fov);
      return (void *)retbuf;

    case FUNC_CHECK:
      if( !status ) 
        fov = getFieldOfView();
      break;
      
    case FUNC_APPLY: 
      setFieldOfView(fov);
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_LEFT && fov > 10.00 ) { // LEFT
        fov -= 1.0;
        status = 1;
        
      } else if( keypress == PSP_CTRL_RIGHT && fov < 140.00 ) { // RIGHT
        fov += 1.0;
        status = 1;
        
      } else if( keypress == PSP_CTRL_CROSS ) { // CROSS
        status = 1 - status;
      
      } else if( keypress == PSP_CTRL_CIRCLE ) { // CIRCLE
        status = 0;
        setFieldOfView(70.0f);
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_camera + (LCS ? 0x254 : 0x198), sizeof(float));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create( global_camera + (LCS ? 0x254 : 0x198), 0, memory_low, memory_high, "> FOV");
        #endif
      } 
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      if( defaultval >= 10 && defaultval <= 140 ) {
        fov = (float)defaultval;
      } else fov = 70.0f;
      break;
  }  

  return NULL;
}


void *camera_topdown(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      setFloat(global_camera + (LCS ? 0x0D4 : 0x854), 30.00f); // camera distance in Vehicle
      setFloat(global_camera + (LCS ? 0x13C : 0x890), 30.00f); // camera distance on foot
      if( LCS && mod_text_size == 0x0031F854 ) { // ULUX v0.02
        setFloat(global_camera + 0x23C, -2.00f); // keep camera highest point possible
        setFloat(global_camera + 0x240, -0.03f); // simulate camera move(necessary for vehicle)
      } else {
        setFloat(global_camera + (LCS ? 0x24C : 0x0E8), -2.00f); // keep camera highest point possible
        setFloat(global_camera + (LCS ? 0x250 : 0x194), -0.03f); // simulate camera move(necessary for vehicle)
      } break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status;
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_camera + (LCS ? 0x0D4 : 0x854), sizeof(float));
        hex_marker_addx(global_camera + (LCS ? 0x13C : 0x890), sizeof(float));
        hex_marker_addx(global_camera + (LCS ? 0x24C : 0x0E8), sizeof(float)); // not ulux
        hex_marker_addx(global_camera + (LCS ? 0x250 : 0x194), sizeof(float)); // not ulux
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create( global_camera + (LCS ? 0x24C : 0x194), 0, memory_low, memory_high, "> TopDown camera");
        #endif
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
  
}



/**  ********************************************************************************************
 *
 * Completion:  
 * 
 * Todo:  - 
 *        - 
 * 
 * Notes:   developer flag -> spawns at debug area on "new game" & Multiplayer can be started without enough players
 **************************************************************************************************************************************/
void *dev_flag(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        if( status ) {
          devflag_disable:
          setByte(global_developerflag + (LCS ? 0 : gp), 0);
          status = 0;
        } else {
          devflag_enable:
          setByte(global_developerflag + (LCS ? 0 : gp), 1);
          status = 1;
        }
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();  
        hex_marker_addx(global_developerflag+(VCS?gp:0), sizeof(char));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_developerflag+(VCS?gp:0), 0, memory_low, memory_high, "> developer flag");
        #endif
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      //if( global_developerflag > 0 ) {
        if( status )  //&& pplayer > 0
          goto devflag_enable;
        else {
          status = 0;
          if( pplayer > 0 ) // dev flag is off on game boot and doesn't need to be set! only when triggered by player while playing, right?!
            goto devflag_disable;
        }
      //}
      break;
  }
   
  return NULL;
  
}



/** Gather Spell *****************************************************************************************************************************
 *
 * Completion:   
 * 
 * Todo:  -  
 * 
 * Notes:     
 ******************************************************************************************************************************************************/
void gather_helper(int base, float x, float y, float z) {
  int speed = 30; // the lower the faster
  
  /// calculate vectors
  float x_obj = (getFloat(base+0x30) - x) / speed;
  float y_obj = (getFloat(base+0x34) - y) / speed;
  float z_obj = (getFloat(base+0x38) - z) / speed;
  
  /// set vectors
  setFloat(base+(LCS?0x70:0x140), -x_obj);
  setFloat(base+(LCS?0x74:0x144), -y_obj);
  setFloat(base+(LCS?0x78:0x148), -z_obj);
}
void *gather_spell(int calltype, int keypress, int defaultstatus) {
  static int status = 0; 
  
  float x, y, z;
  int j = 0;
  
  switch( calltype ) {
    
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY: 
      /// calculate position to gather too
      x = getFloat(pobj+0x30) + ( cos( getFloat(pplayer + (LCS ? 0x4E0 : 0x8D0)) + (M_PI/2) ) * 20);
      y = getFloat(pobj+0x34) + ( sin( getFloat(pplayer + (LCS ? 0x4E0 : 0x8D0)) + (M_PI/2) ) * 20);
      z = getFloat(pobj+0x38) + 6.00f;
      
      /// Vehicles
      int base = vehicles_base;
      for( j = 0; j < vehicles_max; j++, base += var_vehobjsize ) { 
        if( getVehicleObjectIsActive(base) && base != pcar ) { // ignore player's current vehicle
          setVehicleMakePhysical(base);
          gather_helper(base, x, y, z);
        }
      }
      /// Pedestrians
      base = peds_base;
      for( j = 0; j < peds_max; j++, base += var_pedobjsize ) 
        if( getPedObjectIsActive(base) && base != pplayer ) // ignore player
          gather_helper(base, x, y, z);
      
      /// World Objects
      base = worldobjs_base;
      for( j = 0; j < worldobjs_max; j++, base += var_wldobjsize ) 
        if( getWorldObjectIsActive(base) ) 
          gather_helper(base, x, y, z);
        
      break;
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status;
      } 
      break;
      
    case FUNC_SET:
      status = defaultstatus;
      break;
  }  

  return NULL;
}


/** bmxjumpheight ************************************************************************************
 *
 * Completion:   100%
 * 
 * Todo:     
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *bmxjumpheight(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static float mult = 1.0f;
  static float defval = 0.06f; // game default (EU v1.03 at least)
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*) (int)(mult*10.0f); 
    
    case FUNC_GET_STRING: 
      //sprintf(retbuf, "%0.1fx (%.02f)", mult, getBmxJumpMultiplier()); 
      sprintf(retbuf, "%0.1fx", mult);
      return (void *)retbuf;
      
    //case FUNC_CHECK:
      //break;
      
    case FUNC_APPLY: 
      setBmxJumpMultiplier(defval * mult);
      break;
      
    case FUNC_CHANGE_VALUE:
      if ( keypress == PSP_CTRL_LEFT && mult > 1.0f ) { // LEFT
        mult -= 0.5f;
        status = 1;
        
      } else if ( keypress == PSP_CTRL_RIGHT && mult < 20.0f ) { // RIGHT
        mult += 0.5f;
        status = 1;
        
      //} else if ( keypress == PSP_CTRL_CROSS ) { // CROSS
        //status = 1 - status;
        //if( !status ) mult = 1.0f;
      
      } else if ( keypress == PSP_CTRL_CIRCLE ) { // CIRCLE
        status = 0;
        mult = 1.0f;
      
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(global_bmxjumpmult + gp_, sizeof(float));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(global_bmxjumpmult + gp_, 0, memory_low, memory_high, "> global_bmxjumpmult");
        #endif
      } 
      
      if( mult <= 1.0f ) {
        status = 0;
        mult = 1.0f; // default
      } 
      
      setBmxJumpMultiplier(defval * mult);
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      mult = 1.0f; // default
      if( defaultval >= 10 && defaultval <= 200 ) {
        mult = (float)((float)(defaultval)/10.0f);
      }
      if( mult == 1.0f )
        status = 0;        
      break;
  }
  
  return NULL;
}



/// PREVIEW /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /

#ifdef PREVIEW
/** policechaseheli ************************************************************************************
 *
 * Completion:   100%
 * 
 * Todo:     
 * 
 * Notes: 
 **************************************************************************************************************************************/
void *policechaseheli(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status;
  static int pos = 1;
                //  0 Dis  1 Def  2+ Customs....
  const short list_value_lcs[] = { 0x00,  0xD7, 0xD3, 0xD5, 0xD6, 0xD7, 0xD8 };
  const short list_value_vcs[] = { 0x00, 0x105, 0xAD, 0xBC, 0xBD, 0xD5, 0x104, 0x105, 0x106, 0x107, 0x113 };
  static int list_size_lcs = (sizeof(list_value_lcs)/sizeof(*list_value_lcs))-1;
  static int list_size_vcs = (sizeof(list_value_vcs)/sizeof(*list_value_vcs))-1;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_GET_VALUE: // for config
      return (int*)pos;
    
    case FUNC_APPLY:
      break;
    
    case FUNC_CHECK:
      break;
    
    case FUNC_GET_STRING: 
      if( pos == 0 ) sprintf(retbuf, "%s", "Disabled");
      else sprintf(retbuf, "%s", LCS ? getGxtIdentifierForVehicleViaID(list_value_lcs[pos]) : getRealVehicleNameViaID(list_value_vcs[pos])); // LCS doesn't have translations for all vehicles so we use the GXT name here
      if( pos == 1 ) sprintf(retbuf, "%s%s", retbuf, " (default)");
      return (void*)retbuf;  
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CIRCLE) { // CIRCLE
        status = 0; // switch status
        pos = 1; // default id
        
      } else if( keypress == PSP_CTRL_LEFT && pos > 0 ) { // LEFT
        pos--;
        status = 1;
        
      } else if( keypress == PSP_CTRL_RIGHT && pos < (LCS ? list_size_lcs : list_size_vcs) ) { // RIGHT
        pos++;
        status = 1;
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) { // RIGHT
        #ifdef HEXMARKERS
        hex_marker_clear();
        hex_marker_addx(addr_policechaseheli_2, sizeof(short));
        #endif
        #ifdef HEXEDITOR  
        hexeditor_create(addr_policechaseheli_2, 0, memory_low, memory_high, "> addr_policechaseheli_2");
        #endif
      } 
      
      if( pos == 1 ) 
        status = 0;
      
      setPoliceChaseHeliModel(LCS ? list_value_lcs[pos] : list_value_vcs[pos]);      
      RequestModel(LCS ? list_value_lcs[pos] : list_value_vcs[pos], 1);
      LoadAllModelsNow(1);
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      //pos = defaultval;
      if( pos == 1 ) 
        status = 0;
      break;
      
  }
  
  return NULL;
}
#endif

#ifdef PREVIEW
/** fake_swimming ************************************************************************************
 *
 * Completion: 
 * 
 * Todo:  There must be some null pointer somewhere in the hooked function. There is very weird behaviour on real hardware. 
 *        This needs investigating..   
 * 
 * Notes: all logic is inside the hooked functions
 **************************************************************************************************************************************/
void *fake_swimming(int calltype, int keypress, int defaultstatus, int defaultval) {
  static int status = 0;

  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
          
    case FUNC_APPLY:
     if( pcar && (pcar_type != VEHICLE_BOAT) && (isVehicleInWater(pcar) >= 1) && isVehicleInAir(pcar) ) { // when vehicle in water and no ground contact
        if( current_buttons & PSP_CTRL_TRIANGLE ) { // player wants to exit vehicle
          setPedExitVehicleNow(pplayer);
      }
     }
      break;
    
    case FUNC_CHECK:
      break;
    
    case FUNC_GET_STRING: 
      break; 
    
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status;
      } 
      break; 
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
      
  }
  
  return NULL;
}
#endif


/// TESTS /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /

/*char *teststring = "Hello WOrld!";
wchar_t wide[256];

void AsciiToUni(const char* in, wchar_t* out) {
  while ( *in != '\0' ) {
    *out++ = *in++;
  } *out = '\0';
}*/

//char buffer[256];

void test_func() { //for testing only

  //runfunc = 1;
  
  /*** REMEMBER ************************************************************************
  Everything executed in here is run via hooked render function!
  Which means: Hooked functions might crash here but work fine from inside apply loop!!
  **************************************************************************************/

  closeMenu();
  
  ///  LCS: Trigger Function test (see patchonce)
  //setTimedTextbox("Trigger Cheat: Show Credits", 3);
  //FUN_0028a9b8_CHEAT_ShowCredits(0);
  
  ///  LCS: Trigger Function test (see patchonce)
  //setTimedTextbox("LCS test Open Trunk!", 3);
  //if(LCS && pcar) 
    //FUN_00008bd0( pcar ); //open trunk
  
  ///  VCS: Backup water level areas
  /*logPrintf("Water: ");
  int i, j, startofwater = getInt(LCS ? global_ptr_water : global_ptr_water+gp) + 0xC;
  for( i = startofwater, j = 0; i < startofwater+0x5000; i += 0x01, j++ ) //set water everywhere  
    if( *(unsigned char*)i == 0x80 ) 
      logPrintf("0x%X, ", j);
  setTimedTextbox("done", 5);*/

  /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///   

  /*** LCS ************
  $PLAYER_CHAR = 0x18CE ($536)

  0@ = 0x0C
  1@ = 0x0D
  2@ = 0x0E
  ...
  
  ********************/

  /*** VCS ************
  $PLAYER_CHAR = 0x0ED0 ($782)

  0@ = 0x
  1@ = 0x
  2@ = 0x
  ...
  
  ********************/


  static u8 script[] = {  // must be static for CustomScriptExecut()! 
    
    /// PLAY_END_OF_GAME_TUNE
    //0x44, 0x04,
    
    
    /// 00DC: launch_mission @LEVSKIP
    //0xDC, 0x00, 0x06, 0x73, 0x9B, 0x03, 0x00,
    
    /// 00DC: launch_mission @CVIEW
    //0xDC, 0x00, 0x06, 0xDB, 0xC0, 0x03, 0x00,
    
    /// 00DC: launch_mission @BVIEW
    //0xDC, 0x00, 0x06, 0xC4, 0xE4, 0x03, 0x00,
    
    /// start_new_script @DEBMENU
    //0x4F, 0x00, 0x06, 0x7D, 0x26, 0x02, 0x00, 0x00, 
    
    
    
    //0x01, 0x00, 0x06, 0x00, 0x00 , 0x00 , 0x00 , 
    
    //0xE6 , 0x00 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00 , 0x06 , 0x0E , 0x00 , 0x00 , 0x00 , 
    
    //0x4D , 0x00 , 0x06 , 0xFF , 0xFF , 0xFF , 0xFF , 
    
    //0xEA, 0x03, 0x48, 0x45, 0x4C, 0x50, 0x42, 0x00, 0x00, 0x00,
    
    
    /// print_help 'TRAIN'
    0xEA, 0x03, 0x54, 0x52, 0x41, 0x49, 0x4E, 0x00, 0x00, 0x00,
    
    /// terminate_this_script
    0x4E, 0x00
  };
  
  /* writeFloat(&script[3], getFloat(pplayer + 0x30));
  writeFloat(&script[8], getFloat(pplayer + 0x34));
  writeFloat(&script[13], getFloat(pplayer + 0x38)); */
        
        
  //short id = 130; //vehicle_id
  //*(short*)&script[3] = id; //insert vehicle_id
  //*(short*)&script[28] = id; //insert vehicle_id
  
  CustomScriptExecute((int)&script); // make game execute it


  /// ALL VEHICLE 
  /*int j, base;
  base = vehicles_base;
  for( j = 0; j < vehicles_max; j++, base += var_vehobjsize ) {
    if( base != pcar ) { // ignore player's current vehicle
      // getInt(editor_base_adr + 0x44) & 0x1f0) >> 4
    setVehicleMakePhysical(base);
    }
  }*/
  
}

/*struct node { 
  //char *string;
  int time;
  struct node *next;
};

struct node *head = NULL;
  
void print_list() {
    struct node *current = head;
  char string[256];

    while(current != NULL) {
        //printf("%d\n", current->val);
    sprintf(string, "Gametime: %d", current->time); 
    drawString(string, ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 40.0f, WHITE); 
    
        current = current->next;
    }
}

//insertion at the beginning
void insertatbegin(int time){

   //create a link
   struct node *lk = (struct node*) malloc(sizeof(struct node));
   lk->time = time;

   // point it to old first node
   lk->next = head;

   //point first to new first node
   head = lk;
}
void insertatend(int time){

   //create a link
   struct node *lk = (struct node*) malloc(sizeof(struct node));
   lk->time = time;
   struct node *linkedlist = head;

   // point it to old first node
   while(linkedlist->next != NULL)
      linkedlist = linkedlist->next;

   //point first to new first node
   linkedlist->next = lk;
}
void insertafternode(struct node *list, int time){
   struct node *lk = (struct node*) malloc(sizeof(struct node));
   lk->time = time;
   lk->next = list->next;
   list->next = lk;
}
void deleteatbegin(){
   head = head->next;
}
void deleteatend(){
   struct node *linkedlist = head;
   while (linkedlist->next->next != NULL)
      linkedlist = linkedlist->next;
   linkedlist->next = NULL;
}
void deletenode(int key){
   struct node *temp = head, *prev = NULL;
   if (temp != NULL && temp->time == key) {
      head = temp->next;
      return;
   }

   // Find the key to be deleted
   while (temp != NULL && temp->time != key) {
      prev = temp;
      temp = temp->next;
   }

   // If the key is not present
   if (temp == NULL) return;

   // Remove the node
   prev->next = temp->next;
}
int searchlist(int key){
   struct node *temp = head;
   while(temp != NULL) {
      if(temp->time == key) {
         return 1;
      }
      temp=temp->next;
   }
   return 0;
}
int getsize(){
   int counter = 0;
   struct node *temp = head;
   while(temp != NULL) {
      counter++;
      temp=temp->next;
   }
   return counter;
}
*/


void *test_switch(int calltype, int keypress, int defaultstatus) {
  static int status;
  
  /*static int ret;
  static float x;
  static float y;
  static float z;
  static char auStack48[32]; 
  static int iStack16;*/
  
  static int abc = 1;
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      drawString("test_switch() is ON", ALIGN_CENTER, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 240.0f, 10.0f, AZURE );
      
      //sprintf(retbuf, "Test: 0x%08X & %d = 0x%08X", global_mp_parameters, 2, global_mp_parameters & 2); // DAT_0037d768 & 2
      //drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, 5.0f, 45.0f, WHITE);
      
      //memset(&stack[0], 0, sizeof(stack));
      
      /* x = getFloat(pplayer+0x30);
      y = getFloat(pplayer+0x34);
      z = getFloat(pplayer+0x38);      
      ret = FUN_000bc6d4_checkForGround(&x,auStack48,&iStack16,'\x01','\0','\0',0,0,0,0,'\0');
      
      sprintf(retbuf, "ret: %i, 0x%08X -> 0x%08X 0x%08X", ret, auStack48, &iStack16);
      
      drawString(retbuf, 0, 1.0f, 1.0f, 0, 18.0f, 15.0f, 0xCCCCCCCC); */
      
      
      /*setInt(addr_vehiclesworldspawn, 0xAC);  // vehicle id
      setFloat(addr_vehiclesworldspawn + 0x4, 1409.0f);
      setFloat(addr_vehiclesworldspawn + 0x8, -195.0f);
      setFloat(addr_vehiclesworldspawn + 0xC, 51.0f);
      
      setByte(addr_vehiclesworldspawn + 0x18, 0x01); // force spawn
      
      setInt(addr_vehiclesworldspawn + 0x20, 0x00000000);  // gametime
      setInt(addr_vehiclesworldspawn + 0x24, 0xFFFFFFFF);  // >> 8 = vehicle slot no
      setShort(addr_vehiclesworldspawn + 0x28, 0xFFFF);
      setByte(addr_vehiclesworldspawn + 0x2A, 0);  // not blocked
      */
    
  
      //setByte(global_buttoninput+0x0E, 0xFF);
      //setByte(global_buttoninput+0x40, 0xFF);
      
      //setByte(testoffs, 0x00);
      
      //if( pcar ) {
      //  testfunc2( pcar );
      //  testfunc3( pcar );
      //}
      
      /*if(key_to_pad) {
        testfunc(global_buttoninput, key_to_pad); // testfunc2(0)
        key_to_pad = 0;
      }*/
      
      
    //  setFloat(mod_text_addr + 0x37F4DC, -0.75f);
    
    //drawGameLog();
  
    //FUN_123(pobj); 
            
      if( abc ) { // quick apply once
        abc = 0;
      }  
    
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
        
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXEDITOR  
        //if( isInMemBounds(savedatakey + (LCS ? 0 : gp)) ) {
          //hex_marker_clear();
          //hex_marker_addx(savedatakey + (LCS ? 0 : gp), 0x10);
          hexeditor_create(getInt(global_ScriptSpace + (LCS ? 0 : gp)), 0, memory_low, memory_high, "> global_ScriptSpace");
        //}
        #endif
      } else if( keypress == PSP_CTRL_SQUARE ) {
        #ifdef HEXEDITOR  
        //if( isInMemBounds(savedatakey + (LCS ? 0 : gp)) ) {
          //hex_marker_clear();
          //hex_marker_addx(savedatakey + (LCS ? 0 : gp), 0x10);
          hexeditor_create(getInt(global_ScriptSpace + (LCS ? 0 : gp)) + getInt(global_MainScriptSize + (LCS ? 0 : gp)), 0, memory_low, memory_high, "> global_ScriptSpace + global_MainScriptSize");
        //}
        #endif
      }
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      break;
  }
   
  return NULL;
}


void *mp_test(int calltype, int keypress, int defaultstatus) {
  static int status;
  float x, y;
  
  switch( calltype ) {
    case FUNC_GET_STATUS: 
      return (int*)status;
      
    case FUNC_APPLY:
      
      x = 5.0f, y = 40.0f;
      
      sprintf(retbuf, "Game Type: %d", getInt(global_mp_parameters+0x14));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
      /* 0 = Liberty City Survivor
       * 1 = Street Rage
       * 2 = Protection Racket
       * 3 = Get Stretch
       * ....
       */
      
      
      sprintf(retbuf, "Location: %d", getInt(global_mp_parameters+0x18));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
      /* 0 = Portland
       * 1 = Staunton
       * 2 = Shoreside
       */
      
      sprintf(retbuf, "ScoreLimit: %d", getInt(global_mp_parameters+0x1C));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);

      sprintf(retbuf, "TimeLimit: %d", getInt(global_mp_parameters+0x20));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
      
      
      /*sprintf(retbuf, "Team Game: %d", getByte(global_mp_parameters + 2));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
      
      sprintf(retbuf, "Powerup On: %d", getByte(global_mp_parameters + 1));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
       
       
      sprintf(retbuf, "RacePowerup: %d", getByte(global_mp_parameters + 4));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
      
      sprintf(retbuf, "Race Revr: %d", getByte(global_mp_parameters + 0x10));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
      */
      
      sprintf(retbuf, "Skip Cut Scene: %d", getByte(global_mp_parameters + 0x38));
      drawString(retbuf, ALIGN_LEFT, FONT_DIALOG, SIZE_SMALL, SHADOW_OFF, x, y+=15.0f, WHITE);
            
      break;  
      
    case FUNC_CHANGE_VALUE:
      if( keypress == PSP_CTRL_CROSS ) {
        status = 1 - status; // CROSS
      } else if( keypress == PSP_CTRL_TRIANGLE ) {
        #ifdef HEXEDITOR  
        //if( isInMemBounds(savedatakey + (LCS ? 0 : gp)) ) {
          //hex_marker_clear();
          //hex_marker_addx(savedatakey + (LCS ? 0 : gp), 0x10);
          hexeditor_create(getInt(global_mp_parameters), 0, memory_low, memory_high, "> global_mp_parameters");
        //}
        #endif
      } 
      break;
      
    case FUNC_SET: 
      status = defaultstatus;
      //status = 1;
      break;
  }
   
  return NULL;
}

