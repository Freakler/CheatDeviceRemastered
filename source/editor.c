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
#include <limits.h>

#include "cheats.h"
#include "main.h"
#include "editor.h"
#include "functions.h"
#include "utils.h"

#ifdef EDITORS
register int gp asm("gp");

extern int LCS;
extern int VCS;
extern int multiplayer;

/// from cheats.c
extern int peds_max;
extern int peds_cur;
extern int peds_base;
extern int vehicles_max;
extern int vehicles_cur;
extern int vehicles_base;
extern int worldobjs_max;
extern int worldobjs_cur;
extern int worldobjs_base;
extern int businessobjs_max;
extern int businessobjs_cur;
extern int businessobjs_base;
extern int pickups_cur;
extern int mapicons_cur;
extern int vehspawns_cur;
extern int handlingcfg_cur;
extern int garage_cur;

extern u32 var_pedobjsize;
extern u32 var_vehobjsize;
extern u32 var_wldobjsize;
extern u32 var_bsnobjsize;
extern u32 global_pickups;
extern u32 var_pickupslots;
extern u32 var_pickupslotsize;
extern u32 var_radarblipslots;
extern u32 var_radarblipslotsize;
extern u32 var_radarblipspadding;
extern u32 global_radarblips;
extern u32 ptr_handlingCFG;
extern u32 var_handlingcfgslotsize;
extern u32 buildingsIPL_base;
extern u32 buildingsIPL_max;
extern u32 var_buildingsIPLslotsize;
extern u32 treadablesIPL_base;
extern u32 treadablesIPL_max;
extern u32 var_treadablesIPLslotsize;
extern u32 dummysIPL_base;
extern u32 dummysIPL_max;
extern u32 var_dummysIPLslotsize;
extern u32 carcolsDAT_base;
extern u32 pedcolsDAT_base;
extern u32 var_carcolsdatslots;
extern u32 var_carcolsdatslotsize;
extern u32 ptr_particleCFG;
extern u32 var_particleCFGslots;
extern u32 var_particleCFGslotsize;
extern u32 ptr_pedstatTable;
extern u32 var_pedstatDATslots;
extern u32 var_pedstatDATslotsize;
extern u32 ptr_weaponTable;
extern u32 var_weaponDATslots;
extern u32 var_weaponDATslotsize;
extern u32 ptr_timecycDAT;
extern u32 ptr_IDEs;
extern u32 ptr_IDETable;
extern u32 global_garagedata;
extern u32 var_garageslots;
extern u32 var_garageslotsize;
extern u32 addr_vehiclesworldspawn;
extern u32 var_vehiclesworldspawnslots;
extern u32 var_vehiclesworldspawnslotsize;
extern u32 var_radios;

/// from functions.c
extern const teleports_pack lcs_teleports[];
extern int lcs_teleportersize;

extern const teleports_pack vcs_teleports[];
extern int vcs_teleportersize;

extern const pickups_pack lcs_pickups[]; 
extern int lcs_pickupsize;

extern const pickups_pack vcs_pickups[]; 
extern int vcs_pickupsize;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Editor_pack stats_menu[] = {
  //name                           //postfix    //address   //edit_bool  //type       //precision  //*value    //steps   //min    //max
  {"Tires popped with gunfire"      , ""        , 0x08B5E058  , TRUE    , TYPE_INTEGER  , DEC    , 0          , 1      , 0      , 999999999 },
  {"Property destroyed"             , " $"      , 0x08B5E074  , TRUE    , TYPE_INTEGER  , DEC    , 0          , 1      , 0      , 999999999 },
  {"Days passed in game"            , ""        , 0x08B5E10C  , TRUE    , TYPE_INTEGER  , DEC    , 0          , 1      , 0      , 999999999 },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};


const Editor_pack lcs_garage_menu[] = {  
  //name                        //postfix      //address    //edit_bool  //type      //precision  //*value         //steps    //min    //max  
  {"Vehicle Identifier"             , ""        , 0x00      , TRUE    , TYPE_INTEGER  , DEC    , garage_vehicle    , 1      , 0      , INT_MAX  },

  {"X position"                     , ""        , 0x04      , TRUE    , TYPE_FLOAT    , 2      , 0                   , 0.1    , -4000    , 4000     },
  {"Y position"                     , ""        , 0x08      , TRUE    , TYPE_FLOAT    , 2      , 0                   , 0.1    , -4000    , 4000     },
  {"Z position"                     , ""        , 0x0C      , TRUE    , TYPE_FLOAT    , 2      , 0                   , 0.1    , -4000    , 4000    },
  
//  {"Orientation"                  , " Degree" , 0x10      , TRUE    , TYPE_FLOAT    , 2      , garage_orientation  , 1.00    , 0.00    , 360.00   },
//  {"Rotation vector"              , ""        , 0x10      , TRUE    , TYPE_FLOAT    , 2      , 0                   , 0.01    , -1.00    , 1.00     },  // TODO
//  {"Rotation vector"              , ""        , 0x14      , TRUE    , TYPE_FLOAT    , 2      , 0                   , 0.01    , -1.00    , 1.00     },
//  {"Rotation vector"              , ""        , 0x18      , TRUE    , TYPE_FLOAT    , 2      , 0                   , 0.01    , -1.00    , 1.00     },
//  {"Pitch"                        , " Degree" , 0x18      , TRUE    , TYPE_FLOAT    , 2      , garage_pitch      , 1.00    , -90.00  , 90.00   },
  
  {"Traction multiplier"            , ""        , 0x1C      , TRUE    , TYPE_FLOAT    , 2      , 0                   , 0.01    , -200.00  , 200.00   },
  
  {"Flag: Bullet-proof (BP)"        , NULL      , 0x20      , TRUE    , TYPE_BIT      , 0      , 0                   , 0      , 0       , 0     },
  {"Flag: Fire-proof (FP)"          , NULL      , 0x20      , TRUE    , TYPE_BIT      , 1      , 0                   , 0      , 0       , 0     },
  {"Flag: Explosion-proof (EP)"     , NULL      , 0x20      , TRUE    , TYPE_BIT      , 2      , 0                   , 0      , 0       , 0     },
  {"Flag: Damage-proof (DP)"        , NULL      , 0x20      , TRUE    , TYPE_BIT      , 3      , 0                   , 0      , 0       , 0     },
  {"Flag: Meele-proof"              , NULL      , 0x20      , TRUE    , TYPE_BIT      , 4      , 0                   , 0      , 0       , 0     },
  {"Flag: Tires pop-proof (PP)"     , NULL      , 0x20      , TRUE    , TYPE_BIT      , 5      , 0                   , 0      , 0       , 0     },
  {"Flag: Strong (QD)"              , NULL      , 0x20      , TRUE    , TYPE_BIT      , 6      , 0                   , 0      , 0       , 0     },
  {"Flag: Heavy (H) (WP)"           , NULL      , 0x20      , TRUE    , TYPE_BIT      , 7      , 0                   , 0      , 0       , 0     },
  
  {"Flag: Permanent Color (PC)"     , NULL      , 0x21      , TRUE    , TYPE_BIT      , 0      , 0                   , 0      , 0       , 0     },
  {"Flag: Car bomb installed"       , NULL      , 0x21      , TRUE    , TYPE_BIT      , 1      , 0                   , 0      , 0       , 0     },
  {"Flag: Tip-proof (TP)"           , NULL      , 0x21      , TRUE    , TYPE_BIT      , 2      , 0                   , 0      , 0       , 0     },
  {"Flag: Marked (0677)"            , NULL      , 0x21      , TRUE    , TYPE_BIT      , 3      , 0                   , 0      , 0       , 0     },
//  {"? 0x21"                       , NULL      , 0x21      , TRUE    , TYPE_BIT      , 4      , 0                   , 0      , 0       , 0     }, // unused ?
//  {"? 0x21"                       , NULL      , 0x21      , TRUE    , TYPE_BIT      , 5      , 0                   , 0      , 0       , 0     }, // unused ?
//  {"? 0x21"                       , NULL      , 0x21      , TRUE    , TYPE_BIT      , 6      , 0                   , 0      , 0       , 0     }, // unused ?
//  {"? 0x21"                       , NULL      , 0x21      , TRUE    , TYPE_BIT      , 7      , 0                   , 0      , 0       , 0     }, // unused ?
  
//  {"? 0x22"                       , ""        , 0x22      , TRUE    , TYPE_BYTE     , 0      , 0                   , 1      , 0x00    , 0xFF    }, // unused ?
//  {"? 0x23"                       , ""        , 0x23      , TRUE    , TYPE_BYTE     , 0      , 0                   , 1      , 0x00    , 0xFF    }, // unused ?
  
  {"Primary Color"                  , ""        , 0x24      , TRUE    , TYPE_BYTE     , 1 /*dec*/ , 0                , 1      , 0x00    , 0xFF    },
  {"Secondary Color"                , ""        , 0x25      , TRUE    , TYPE_BYTE     , 1 /*dec*/ , 0                , 1      , 0x00    , 0xFF    },
  
  {"Radio Station"                  , ""        , 0x26      , TRUE    , TYPE_BYTE     , 0 /*hex*/ , radiostation     , 0      , 0      , 0       },

  {"Variation 1"                    , ""        , 0x27      , TRUE    , TYPE_BYTE     , 0 /*hex*/ , 0                , 1      , 0x00    , 0xFF     },
  {"Variation 2"                    , ""        , 0x28      , TRUE    , TYPE_BYTE     , 0 /*hex*/ , 0                , 1      , 0x00    , 0xFF     },
           

  /// Variations:   set one of both (doesn't matter which, the other 0xFF), or even both for 2 addons!!
  
  // Landstalker    0x00 = roofrack
  //          0x01 = boxes in back 
  //          0xFF = nothing
  
  // Partriot      0x00 = low tarp 
  //          0x01 = high tarp
  //          0x02 = pipes
  //          0xFF = nothing
  
  /// Variation Vehivcles: Mule / Yankee / Hearse / BARRACKS OL / Stallion / Phobos VT / Deimos / Stinger / Manana / ..
  
  // https://gta.fandom.com/wiki/Special_Vehicles_in_GTA_Liberty_City_Stories
  // https://gtaforums.com/topic/838112-gta-vcs-special-vehicle-guide/
  // http://gtaforums.com/topic/849155-gta-lcs-special-vehicle-guide/page-47#entry1069557353
  
  {NULL,NULL,0,0,0,0,0,0,0}
};


const Editor_pack vcs_garage_menu[] = {  
  //name                  //postfix       //address    //edit_bool  //type        //precision  //*value         //steps    //min    //max  
  {"Vehicle Identifier"         , ""        , 0x00      , TRUE    , TYPE_INTEGER    , DEC    , garage_vehicle    , 1      , 0      , INT_MAX  },

  {"X position"                 , ""        , 0x04      , TRUE    , TYPE_FLOAT    , 2        , 0            , 0.1    , -4000    , 4000     },
  {"Y position"                 , ""        , 0x08      , TRUE    , TYPE_FLOAT    , 2        , 0            , 0.1    , -4000    , 4000     },
  {"Z position"                 , ""        , 0x0C      , TRUE    , TYPE_FLOAT    , 2        , 0            , 0.1    , -4000    , 4000     },

//  {"Orientation"              , " Degree" , 0x10      , TRUE    , TYPE_FLOAT    , 2        , garage_orientation  , 1.00    , 0.00    , 360.00   },
//  {"Rotation vector"          , ""        , 0x10      , TRUE    , TYPE_FLOAT    , 2        , 0            , 0.01    , -1.00    , 1.00   },  // TODO
//  {"Rotation vector"          , ""        , 0x14      , TRUE    , TYPE_FLOAT    , 2        , 0            , 0.01    , -1.00    , 1.00   },
//  {"Rotation vector"          , ""        , 0x18      , TRUE    , TYPE_FLOAT    , 2        , 0            , 0.01    , -1.00    , 1.00   },
//  {"Pitch"                    , " Degree" , 0x18      , TRUE    , TYPE_FLOAT    , 2        , garage_pitch , 1.00    , -90.00  , 90.00   },
 
  {"Traction multiplier"        , ""        , 0x1C      , TRUE    , TYPE_FLOAT    , 2        , 0            , 0.01    , -200.00  , 200.00 },
  
  {"Flag: Bullet-proof (BP)"    , NULL      , 0x20      , TRUE    , TYPE_BIT      , 0        , 0            , 0      , 0      , 0      },
  {"Flag: Fire-proof (FP)"      , NULL      , 0x20      , TRUE    , TYPE_BIT      , 1        , 0            , 0      , 0      , 0      },
  {"Flag: Explosion-proof (EP)" , NULL      , 0x20      , TRUE    , TYPE_BIT      , 2        , 0            , 0      , 0      , 0      },
  {"Flag: Damage-proof (DP)"    , NULL      , 0x20      , TRUE    , TYPE_BIT      , 3        , 0            , 0      , 0      , 0      },
  {"Flag: Meele-proof"          , NULL      , 0x20      , TRUE    , TYPE_BIT      , 4        , 0            , 0      , 0      , 0      },
  {"Flag: Tires pop-proof (PP)" , NULL      , 0x20      , TRUE    , TYPE_BIT      , 5        , 0            , 0      , 0      , 0      },
  {"Flag: Strong (QD)"          , NULL      , 0x20      , TRUE    , TYPE_BIT      , 6        , 0            , 0      , 0      , 0      },
  {"Flag: Heavy (H) (WP)"       , NULL      , 0x20      , TRUE    , TYPE_BIT      , 7        , 0            , 0      , 0      , 0      },
  
  {"Flag: Permanent Color (PC)" , NULL      , 0x21      , TRUE    , TYPE_BIT      , 0        , 0            , 0      , 0      , 0      },
  {"Flag: Car bomb installed"   , NULL      , 0x21      , TRUE    , TYPE_BIT      , 1        , 0            , 0      , 0      , 0      },
  {"Flag: Tip-proof (TP)"       , NULL      , 0x21      , TRUE    , TYPE_BIT      , 2        , 0            , 0      , 0      , 0      },
  {"Flag: Marked (0677)"        , NULL      , 0x21      , TRUE    , TYPE_BIT      , 3        , 0            , 0      , 0      , 0      },
//{"? 0x21"                     , NULL      , 0x21      , TRUE    , TYPE_BIT      , 4        , 0            , 0      , 0      , 0      }, // unused ?
//{"? 0x21"                     , NULL      , 0x21      , TRUE    , TYPE_BIT      , 5        , 0            , 0      , 0      , 0     }, // unused ?
//{"? 0x21"                     , NULL      , 0x21      , TRUE    , TYPE_BIT      , 6        , 0            , 0      , 0      , 0     }, // unused ?
//{"? 0x21"                     , NULL      , 0x21      , TRUE    , TYPE_BIT      , 7        , 0            , 0      , 0      , 0      }, // unused ?
  
//{"? 0x22"                     , ""        , 0x22      , TRUE    , TYPE_BYTE      , 0      , 0            , 1      , 0x00    , 0xFF     }, // unused ?
//{"? 0x23"                     , ""        , 0x23      , TRUE    , TYPE_BYTE      , 0      , 0            , 1      , 0x00    , 0xFF     }, // unused ?
  
  {"Primary Color R"            , "/255"    , 0x24      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },
  {"Primary Color G"            , "/255"    , 0x25      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },
  {"Primary Color B"            , "/255"    , 0x26      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },
  {"Primary Color Alpha"        , "/255"    , 0x27      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },
  {"Secondary Color R"          , "/255"    , 0x28      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },
  {"Secondary Color G"          , "/255"    , 0x29      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },
  {"Secondary Color B"          , "/255"    , 0x2A      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },
  {"Secondary Color Alpha"      , "/255"    , 0x2B      , TRUE    , TYPE_BYTE      , 1      , 0            , 1      , 0x00    , 0xFF     },

  {"Radio Station"              , ""        , 0x2C      , TRUE    , TYPE_BYTE      , 0 /*hex*/ , radiostation , 0      , 0      , 0      },

  {"Variation 1"                , ""        , 0x2D      , TRUE    , TYPE_BYTE      , 0 /*hex*/ , 0            , 1      , 0x00    , 0xFF  },
  {"Variation 2"                , ""        , 0x2E      , TRUE    , TYPE_BYTE      , 0 /*hex*/ , 0            , 1      , 0x00    , 0xFF  },
    
  {NULL,NULL,0,0,0,0,0,0,0}
};


void *garage_vehicle(int calltype, int keypress, int base_address, int address) { 
  static char buffer[32];
  
  switch( calltype ) {
    case FUNC_GET_STRING:
      sprintf(buffer, "%s", getRealVehicleNameViaID(getShort(address))); // on error print hex
      if( buffer[0] == '\0' ) // some vehicles don't have translations..
        sprintf(buffer, "%s", getGxtIdentifierForVehicleViaID(getShort(address))); // on error print hex  
      // sprintf(buffer, "%s - %i", getShort(address), buffer); //on error print hex
      return (void *)buffer;
      
    case FUNC_CHANGE_VALUE:
      if( keypress ) {
        if ( keypress == PSP_CTRL_LEFT && getShort(address) > getFirstIdeOfType(MODELINFO_VEHICLE) ) { // LEFT
          setShort(address, getShort(address) - 1);
        } else if ( keypress == PSP_CTRL_RIGHT && getShort(address) < getLastIdeOfType(MODELINFO_VEHICLE) ) { // RIGHT
          setShort(address, getShort(address) + 1);
        }
      }
      break;
  }
  return NULL;
}

void *radiostation(int calltype, int keypress, int base_address, int address) { 
  static int i = 0;  
  static char buffer[16];
  
  switch( calltype ) {
    case FUNC_GET_STRING:
      i = getByte(address);
      sprintf(buffer, "%s", getRadioStationName(i));
      return (void *)buffer;
      
    case FUNC_CHANGE_VALUE:
      if( keypress ) {
        i = getByte(address);
        if( keypress == PSP_CTRL_LEFT && i > 0 ) { ///LEFT
          i--;
        } else if( keypress == PSP_CTRL_RIGHT ) { ///RIGHT
          if( i < var_radios ) 
            i++;
        }
        setByte(address, i);
      }
      break;
  }
  return NULL;
}

void *pedstatname(int calltype, int keypress, int base_address, int address) { 
  static int i = 0;  
  static char buffer[16];
  
  switch( calltype ) {
    case FUNC_GET_STRING:
      i = getByte(address);
      sprintf(buffer, "%s", getPedstatName(i) );
      return (void *)buffer;
      
    case FUNC_CHANGE_VALUE:
      if( keypress ) {
        i = getByte(address);
        if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
          i--;
        } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
          if( i < var_pedstatDATslots - 1 )
            i++;
        }
        setByte(address, i);
      }
      break;
  }
  return NULL;
}


const Editor_pack lcs_pedobj_menu[] = {
  //name                          //postfix    //address     //edit_bool  //type        //precision  //*value         //steps      //min    //max    
  {"World Coordinate X"              , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"              , ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"              , ""      , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },

  {"Identifier"                      , ""      , 0x58      , FALSE    , TYPE_BYTE    , DEC    , 0            , 0x1    , 0x0000   , 0xFFFF   },
  
  {"Weight 1"                        , ""      , 0xD0      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1      , 0        , 999999   },
  {"Weight 2"                        , ""      , 0xD4      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1      , 0        , 999999   },
  
//  {"PED to ?"                      , ""      , 0x10C     , FALSE    , TYPE_INTEGER    , HEX    , 0            , 1      , INT_MIN  , INT_MAX   },
  
//  {"0x197 Flags"                   , ""      , 0x197     , TRUE    , TYPE_BIT      , 0      , 0            , 0      , 0      , 0     },
//  {"0x197 Flags"                   , ""      , 0x197     , TRUE    , TYPE_BIT      , 1      , 0            , 0      , 0      , 0     },
  {"Bleeding bool"                   , ""      , 0x197     , TRUE    , TYPE_BIT      , 2      , 0            , 0      , 0      , 0     },
//  {"0x197 Flags"                   , ""      , 0x197     , TRUE    , TYPE_BIT      , 3      , 0            , 0      , 0      , 0     },
  {"Cannot land bool"                , ""      , 0x197     , TRUE    , TYPE_BIT      , 4      , 0            , 0      , 0      , 0     },
//  {"0x197 Flags"                   , ""      , 0x197     , TRUE    , TYPE_BIT      , 5      , 0            , 0      , 0      , 0     },
//  {"0x197 Flags"                   , ""      , 0x197     , TRUE    , TYPE_BIT      , 6      , 0            , 0      , 0      , 0     },
//  {"0x197 Flags"                   , ""      , 0x197     , TRUE    , TYPE_BIT      , 7      , 0            , 0      , 0      , 0     },
  
/*  {"0x19A Flags"                   , ""      , 0x19A     , TRUE    , TYPE_BIT      , 0      , 0            , 0      , 0      , 0     },
  {"0x19A Flags"                     , ""      , 0x19A     , TRUE    , TYPE_BIT      , 1      , 0            , 0      , 0      , 0     },
  {"0x19A Flags"                     , ""      , 0x19A     , TRUE    , TYPE_BIT      , 2      , 0            , 0      , 0      , 0     },
  {"0x19A Flags"                     , ""      , 0x19A     , TRUE    , TYPE_BIT      , 3      , 0            , 0      , 0      , 0     },
  {"0x19A Flags"                     , ""      , 0x19A     , TRUE    , TYPE_BIT      , 4      , 0            , 0      , 0      , 0     },
  {"0x19A Flags"                     , ""      , 0x19A     , TRUE    , TYPE_BIT      , 5      , 0            , 0      , 0      , 0     },
  {"0x19A Flags"                     , ""      , 0x19A     , TRUE    , TYPE_BIT      , 6      , 0            , 0      , 0      , 0     }, */
  {"Invisible bool"                  , ""      , 0x19A     , TRUE    , TYPE_BIT      , 7      , 0            , 0      , 0      , 0     },
  
  {"Bloody footprints"               , ""      , 0x198     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0x40     },
  
  {"PED to focus?"                   , ""      , 0x258     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"VEH to focus?"                   , ""      , 0x25C     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"PED to follow?"                  , ""      , 0x274     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  
  {"PED to ?"                        , ""      , 0x310     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  
  {"Movement Status"                 , ""      , 0x354     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x0000  , 0xFFFF   }, // 0x01 = standing, 0x02 = walking, 0x04 = running, 0x05 = sprinting
  
  {"Health"                          , ""      , 0x4B8     , TRUE    , TYPE_FLOAT      , 2      , 0            , 1      , 0      , 999999    },
  {"Armor"                           , ""      , 0x4BC     , TRUE    , TYPE_FLOAT      , 2      , 0            , 1      , 0      , 999999    },
  
  {"Facing Direction (currently)"    , ""      , 0x4E0     , FALSE    , TYPE_FLOAT     , 2      , 0            , 0.10    , -3.14    , 3.14   },
  {"Facing Direction (to be aplied)" , ""      , 0x4E4     , TRUE    , TYPE_FLOAT      , 2      , 0            , 0.10    , -3.14    , 3.14   }, // 0.00  = North, 1.50 = East, 3.00 South, -1.50 West
  
  {"In-Vehicle bool"                 , ""      , 0x538     , FALSE    , TYPE_BOOL      , 0      , 0            , 0      , 0x00    , 0x01     },
  
  
  {"WeaponID - Meele Slot"           , ""      , 0x594     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"WeaponID - Projectile Slot"      , ""      , 0x5CC     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  
  {"WeaponID - Handgun Slot"         , ""      , 0x5E8     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"Handgun Ammo (loaded)"           , ""      , 0x5F0     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  {"Handgun Ammo (total)"            , ""      , 0x5F4     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  
  {"WeaponID - Shotgun Slot"         , ""      , 0x604     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"Shotgun Ammo (loaded)"           , ""      , 0x60C     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  {"Shotgun Ammo (total)"            , ""      , 0x610     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  
  {"WeaponID - SMG Slot"             , ""      , 0x620     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"SMG Ammo (loaded)"               , ""      , 0x628     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  {"SMG Ammo (total)"                , ""      , 0x62C     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  
  {"WeaponID - Assault Rifle Slot"   , ""      , 0x63C     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"Assault Rifle Ammo (loaded)"     , ""      , 0x644     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  {"Shotgun Ammo (total)"            , ""      , 0x648     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  
  {"WeaponID - Launcher Slot"        , ""      , 0x658     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"Launcher Ammo (loaded)"          , ""      , 0x660     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  {"Launcher Ammo (total)"           , ""      , 0x664     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  
  {"WeaponID - Sniper Slot"          , ""      , 0x674     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"Sniper Ammo (loaded)"            , ""      , 0x67C     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  {"Sniper Ammo (total)"             , ""      , 0x680     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
   
  {"WeaponID - Other Slot"           , ""      , 0x690     , FALSE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"Photos (loaded)"                 , ""      , 0x698     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  {"Photos (total)"                  , ""      , 0x69C     , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0      , 999999    },
  
  {"Selected Weapon Slot"            , ""      , 0x6B8     , FALSE    , TYPE_BYTE      , DEC    , 0            , 1      , 0x0    , 0x9       },
  
  {"PED aiming at"                   , ""      , 0x6BC     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"Last touched PED"                , ""      , 0x6E0     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"Ped in range #1 (nearest)"       , ""      , 0x724     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"Ped in range #2"                 , ""      , 0x728     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  //..
  {"Ped in range #10 (farest)"       , ""      , 0x748     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"Ped in range counter"            , " of 10", 0x74C     , FALSE    , TYPE_BYTE      , DEC    , 0            , 1      , 0x00    , 0xFF     }, 
  
  {"Ped ??"                          , ""      , 0x780     , FALSE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  
  {"Selected Weapon Slot"            , ""      , 0xB84     , TRUE     , TYPE_BYTE      , DEC    , 0            , 1      , 0      , 9         },
  
  {"Is targeted bool"                , ""      , 0xBB4     , FALSE    , TYPE_BOOL      , 0      , 0            , 0      , 0x00    , 0x01     },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_pedobj_menu[] = {
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value         //steps      //min    //max    
  {"World Coordinate X"             , ""       , 0x30      , TRUE    , TYPE_FLOAT      , 2      , 0            , 0.1    , -4000    , 4000    },
  {"World Coordinate Y"             , ""       , 0x34      , TRUE    , TYPE_FLOAT      , 2      , 0            , 0.1    , -4000    , 4000    },
  {"World Coordinate Z"             , ""       , 0x38      , TRUE    , TYPE_FLOAT      , 2      , 0            , 0.1    , -4000    , 4000    },

  {"Identifier"                     , ""       , 0x58      , FALSE   , TYPE_BYTE       , DEC    , 0            , 0x1    , 0x0000  , 0xFFFF   },

  {"Weight 1"                       , ""      , 0xD0       , TRUE    , TYPE_FLOAT      , 2      , 0            , 1      , 0       , 999999   },
  {"Weight 2"                       , ""      , 0xD4       , TRUE    , TYPE_FLOAT      , 2      , 0            , 1      , 0       , 999999   },

  {"Health"                         , ""      , 0x4E4      , TRUE    , TYPE_FLOAT      , 2      , 0            , 1      , 0       , 999999   },
  {"Armor"                          , ""      , 0x4E8      , TRUE    , TYPE_FLOAT      , 2      , 0            , 1      , 0       , 999999   },
  
  {"WeaponID - Meele Slot"          , ""      , 0x578      , FALSE   , TYPE_BYTE       , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 

  {"WeaponID - Handgun Slot"        , ""      , 0x5CC      , FALSE   , TYPE_BYTE       , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, 
  {"Handgun Ammo (loaded)"          , ""      , 0x5D4      , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0       , 999999   },
  {"Handgun Ammo (total)"           , ""      , 0x5D8      , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , 0       , 999999   },
  
  {"Selected Weapon Slot"           , ""      , 0x789      , FALSE   , TYPE_BYTE       , DEC    , 0            , 1      , 0x0     , 0x9      },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_vehicleobj_menu[] = {
  
  //name                      //postfix    //address     //edit_bool  //type        //precision  //*value         //steps      //min    //max  
  {"World Coordinate X"            , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"            , ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"            , ""      , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
//{"0x44 Flag 0"                   , ""      , 0x44      , TRUE    , TYPE_BIT      , 0      , 0            , 0      , 0      , 0     },
//{"0x44 Flag 1"                   , ""      , 0x44      , TRUE    , TYPE_BIT      , 1      , 0            , 0      , 0      , 0     },
//{"0x44 Flag 2"                   , ""      , 0x44      , TRUE    , TYPE_BIT      , 2      , 0            , 0      , 0      , 0     },
//{"0x44 Flag 3"                   , ""      , 0x44      , TRUE    , TYPE_BIT      , 3      , 0            , 0      , 0      , 0     },
//{"0x44 Flag 4)"                  , ""      , 0x44      , TRUE    , TYPE_BIT      , 4      , 0            , 0      , 0      , 0     },
//{"0x44 Flag 5"                   , ""      , 0x44      , TRUE    , TYPE_BIT      , 5      , 0            , 0      , 0      , 0     },
//{"0x44 Flag 6"                   , ""      , 0x44      , TRUE    , TYPE_BIT      , 6      , 0            , 0      , 0      , 0     },
//{"0x44 Flag 7"                   , ""      , 0x44      , TRUE    , TYPE_BIT      , 7      , 0            , 0      , 0      , 0     },

//{"0x45 Flag 0"                   , ""      , 0x45      , TRUE    , TYPE_BIT      , 0      , 0            , 0      , 0      , 0     },
//{"0x45 Flag 1"                   , ""      , 0x45      , TRUE    , TYPE_BIT      , 1      , 0            , 0      , 0      , 0     },
//{"0x45 Flag 2"                   , ""      , 0x45      , TRUE    , TYPE_BIT      , 2      , 0            , 0      , 0      , 0     },
//{"0x45 Flag 3"                   , ""      , 0x45      , TRUE    , TYPE_BIT      , 3      , 0            , 0      , 0      , 0     },
//{"0x45 Flag 4)"                  , ""      , 0x45      , TRUE    , TYPE_BIT      , 4      , 0            , 0      , 0      , 0     },
//{"0x45 Flag 5"                   , ""      , 0x45      , TRUE    , TYPE_BIT      , 5      , 0            , 0      , 0      , 0     },
//{"0x45 Flag 6"                   , ""      , 0x45      , TRUE    , TYPE_BIT      , 6      , 0            , 0      , 0      , 0     },
//{"0x45 Flag 7"                   , ""      , 0x45      , TRUE    , TYPE_BIT      , 7      , 0            , 0      , 0      , 0     },
 
//{"0x46 Flag 0"                   , ""      , 0x46      , TRUE    , TYPE_BIT      , 0      , 0            , 0      , 0      , 0     }, // model
//{"0x46 Flag 1 (fix)"             , ""      , 0x46      , TRUE    , TYPE_BIT      , 1      , 0            , 0      , 0      , 0     },
//{"0x46 Flag 2"                   , ""      , 0x46      , TRUE    , TYPE_BIT      , 2      , 0            , 0      , 0      , 0     },
  {"Is Rendered bool"              , ""      , 0x46      , TRUE    , TYPE_BIT      , 3      , 0            , 0      , 0      , 0     },
//{"0x46 Flag 4 (fix)"             , ""      , 0x46      , TRUE    , TYPE_BIT      , 4      , 0            , 0      , 0      , 0     },
  {"Disable Lighting bool"         , ""      , 0x46      , TRUE    , TYPE_BIT      , 5      , 0            , 0      , 0      , 0     },
//{"0x46 Flag 6"                   , ""      , 0x46      , TRUE    , TYPE_BIT      , 6      , 0            , 0      , 0      , 0     },
//{"0x46 Flag 7"                   , ""      , 0x46      , TRUE    , TYPE_BIT      , 7      , 0            , 0      , 0      , 0     },

  
  {"Unload Vehicle"                , ""      , 0x48      , TRUE    , TYPE_BIT      , 0      , 0            , 0      , 0      , 0     }, //
//{"0x48 Flag 1"                   , ""      , 0x48      , TRUE    , TYPE_BIT      , 1      , 0            , 0      , 0      , 0     },
//{"0x48 Flag 2"                   , ""      , 0x48      , TRUE    , TYPE_BIT      , 2      , 0            , 0      , 0      , 0     },
//{"Disable Refelection (alt?)"    , ""      , 0x48      , TRUE    , TYPE_BIT      , 3      , 0            , 0      , 0      , 0     },
//{"0x48 Flag 4"                   , ""      , 0x48      , TRUE    , TYPE_BIT      , 4      , 0            , 0      , 0      , 0     },
  {"Disable Refelection"           , ""      , 0x48      , TRUE    , TYPE_BIT      , 5      , 0            , 0      , 0      , 0     },
//{"0x48 Flag 6"                   , ""      , 0x48      , TRUE    , TYPE_BIT      , 6      , 0            , 0      , 0      , 0     },
//{"0x48 Flag 7"                   , ""      , 0x48      , TRUE    , TYPE_BIT      , 7      , 0            , 0      , 0      , 0     },
   
  {"Identifier"                    , ""      , 0x58      , FALSE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x0000  , 0xFFFF   }, // garage_vehicle
  
  {"Mass 1"                        , ""      , 0xD0      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"Mass 2"                        , ""      , 0xD4      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Reactiontime multiplier"       , ""      , 0xD8      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
//{"0x142 Flag 0"                  , ""      , 0x142      , TRUE    , TYPE_BIT      , 0     , 0            , 0      , 0      , 0     },
  {"Gravity applies bool"          , ""      , 0x142      , TRUE    , TYPE_BIT      , 1     , 0            , 0      , 0      , 0     },
  {"Freeze position bool"          , ""      , 0x142      , TRUE    , TYPE_BIT      , 2     , 0            , 0      , 0      , 0     },
//{"0x142 Flag 3"                  , ""      , 0x142      , TRUE    , TYPE_BIT      , 3     , 0            , 0      , 0      , 0     },
//{"0x142 Flag 4"                  , ""      , 0x142      , TRUE    , TYPE_BIT      , 4     , 0            , 0      , 0      , 0     },
//{"0x142 Flag 5"                  , ""      , 0x142      , TRUE    , TYPE_BIT      , 5     , 0            , 0      , 0      , 0     },
//{"0x142 Flag 6"                  , ""      , 0x142      , TRUE    , TYPE_BIT      , 6     , 0            , 0      , 0      , 0     },
//{"0x142 Flag 7"                  , ""      , 0x142      , TRUE    , TYPE_BIT      , 7     , 0            , 0      , 0      , 0     },
  
  {"Ground type"                   , ""      , 0x145      , FALSE    , TYPE_BYTE    , HEX    , 0            , 0x1    , 0x00    , 0xFF   },

  {"Primary Color"                 , ""      , 0x1F0      , TRUE    , TYPE_BYTE     , DEC    , 0            , 0x1    , 0x00    , 0xFF   },
  {"Secondary Color"               , ""      , 0x1F1      , TRUE    , TYPE_BYTE     , DEC    , 0            , 0x1    , 0x00    , 0xFF   },
  
  {"Health"                        , ""      , 0x268      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1      , 0      , 999999  },
  {"Currenty Gear"                 , ""      , 0x270      , TRUE    , TYPE_BYTE     , DEC    , 0            , 0x1    , 0x00    , 0xFF   },
  {"Radio Station"                 , ""      , 0x2A0      , TRUE    , TYPE_BYTE     , 0      , radiostation , 0      , 0      , 0       },
  
  {"Honking Bool"                  , ""      , 0x2A8      , TRUE    , TYPE_BOOL     , 0      , 0            , 0      , 0x00    , 0x01     },

  {"0x5E5 Flag 0 carbomb installed"     , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 0     , 0            , 0      , 0      , 0     }, // Car Bomb
  {"0x5E5 Flag 1 use remote detonator"  , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 1     , 0            , 0      , 0      , 0     }, // Car Bomb
  {"0x5E5 Flag 2 detonate entering car" , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 2     , 0            , 0      , 0      , 0     }, // Car Bomb
  {"0x5E5 Flag 3"                       , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 3     , 0            , 0      , 0      , 0     }, // Car Bomb
  {"0x5E5 Flag 4 someone is in car"     , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 4     , 0            , 0      , 0      , 0     }, // Car Bomb
//{"0x5E5 Flag 5"                       , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 5     , 0            , 0      , 0      , 0     }, // Car Bomb
//{"0x5E5 Flag 6"                       , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 6     , 0            , 0      , 0      , 0     }, // Car Bomb
//{"0x5E5 Flag 7"                       , ""      , 0x5E5      , TRUE    , TYPE_BIT      , 7     , 0            , 0      , 0      , 0     }, // Car Bomb
  
  {"Wheel Camber"              , ""      , 0x618      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1      , 0      , 999999   },

  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_vehicleobj_menu[] = {
  //name                  //postfix     //address     //edit_bool  //type        //precision  //*value     //steps    //min    //max  
  {"World Coordinate X"        , ""        , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"        , ""        , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"        , ""        , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Identifier"                , ""        , 0x58      , FALSE   , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 0xFFFF   }, // garage_vehicle
  
  {"Mass"                      , " kg"     , 0xD0      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"TurnMass"                  , " kg m3"  , 0xD4      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Health"                    , ""        , 0x27C     , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Primary Color"             , ""        , 0x224     , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"Secondary Color"           , ""        , 0x228     , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_worldobj_menu[] = {
  //name                  //postfix     //address     //edit_bool  //type      //precision  //*value      //steps    //min    //max  
  {"World Coordinate X"        , ""        , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"        , ""        , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"        , ""        , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Identifier"                , ""        , 0x58      , FALSE   , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 0xFFFF   },
  
  {"Mass"                      , " kg"     , 0xD0      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"TurnMass"                  , " kg m3"  , 0xD4      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Air Resistance"            , ""        , 0xDC      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01    , 0      , 1     },
  {"Elasticity"                , ""        , 0xE0      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01    , 0      , 1     },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_worldobj_menu[] = {
  //name                  //postfix    //address     //edit_bool  //type      //precision  //*value     //steps      //min    //max  
  {"World Coordinate X"        , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"        , ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"        , ""      , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Identifier"                , ""      , 0x58      , FALSE   , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 0xFFFF   },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_pickups_menu[] = {
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value         //steps    //min    //max  
  {"World Coordinate X"          , ""      , 0x00      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"          , ""      , 0x04      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"          , ""      , 0x08      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
//{"pointer to world object"     , ""      , 0x14      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"Ammo/Amount"                 , ""      , 0x1C      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0.1    , -4000    , 4000     },
  
//{"0x22 ?"                      , ""      , 0x22      , TRUE    , TYPE_BYTE     , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
  
  {"Identifier"                  , ""      , 0x26      , FALSE   , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 0xFFFF   },
  {"Type"                        , ""      , 0x32      , TRUE    , TYPE_BYTE     , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
  
  {"is collected?"               , ""      , 0x33      , TRUE    , TYPE_BOOL     , 0      , 0            , 0      , 0x00    , 0x01     },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_pickups_menu[] = {
  //name                  //postfix    //address     //edit_bool  //type     //precision  //*value         //steps    //min    //max  
  {"World Coordinate X"          , ""      , 0x00      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"          , ""      , 0x04      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"          , ""      , 0x08      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Ammo/Amount"                 , ""      , 0x2C      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0.1    , -4000    , 4000     },
  
  {"Identifier"                  , ""      , 0x34      , FALSE   , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 0xFFFF   },
  {"Type"                        , ""      , 0x38      , TRUE    , TYPE_BYTE     , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
  
//{"0x36 always 0x2 for active?" , ""      , 0x36      , TRUE    , TYPE_BYTE     , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
  {"is collected?"               , ""      , 0x39      , TRUE    , TYPE_BOOL     , 0      , 0            , 0      , 0x00    , 0x01     },
//{"0x3A able to take now?!"     , ""      , 0x39      , TRUE    , TYPE_BYTE     , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

/****** LCS tests **********************************************************************
icon  Color  "0x4"  "0x8"    
-----------------------------------
0    2    1    0x1101     "friend" blue vehicle with vincenzo on passaenger seat
               0x1201
               0x1000     -> vehicle on slot 16
                    
0    4    4    0          "destination" yellow location with sphere
40   2    5    0          Vincenzo Icon
0    0    2    0x1614     "Threat" red PED enemy to kill  (world coordinates are NOT updated)
0    0    2    0x2D04     "Threat" red PED enemy to kill  (world coordinates are NOT updated)
0    0    2    0x3601     "Threat" red PED enemy to kill  (world coordinates are NOT updated)

018B add blip for car               ->  FUN_001640b0(1,    DAT_val,  0,      3);
018C add blip for char              ->  FUN_001640b0(2,    DAT_val,  0,      3);
018D add blip for object            ->  FUN_001640b0(3,    DAT_val,  1,      3);
018F add blip for coordinate        ->  FUN_00163e40(4,    &local_b0,  4)
018E blip with sphere to coordinate ->  FUN_00163e40(5,    &local_b0,  5);
****/
const Editor_pack lcs_mapicons_menu[] = {
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value         //steps      //min    //max  
  {"Color"                             , ""      , 0x00      , TRUE    , TYPE_INTEGER    , 0      , mapicon_color      , 0      , 0      , 0     }, // RGBA!!!
  {"Linked Object Type"                , ""      , 0x04      , FALSE    , TYPE_INTEGER    , 0      , mapicon_type      , 0      , 0      , 0      }, // 1 = car, 2 = Ped, 3 = world obj, 4 = coordinate, 5 = sphere to coordinate
  {"Linked Object Slot"                , ""      , 0x08      , FALSE    , TYPE_INTEGER    , 0      , mapicon_slot      , 0      , 0      , 0      }, // shift by >> 8 to get Slot number of Vehicle / Pedestrian / World Obj
  
  {"World Coordinate X"                , ""      , 0x0C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // only for coordinate type(s)
  {"World Coordinate Y"                , ""      , 0x10      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"                , ""      , 0x14      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"? Coordinate X"                    , ""      , 0x20      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"? Coordinate Y"                    , ""      , 0x24      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"? Coordinate Z"                    , ""      , 0x28      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"mp counter ?"                      , ""      , 0x30      , TRUE    , TYPE_SHORT    , HEX    , 0            , 0x1    , 0x000    , 0xFFFF   },
  
  {"Brightness"                        , ""      , 0x32      , TRUE    , TYPE_BOOL      , 0      , 0            , 0x1    , 0x00    , 0x01     },
//  {"Display Bool ?"                  , ""      , 0x33      , FALSE   , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     }, // 0xFF
  {"Display on radar only when close"  , ""      , 0x34      , TRUE    , TYPE_BOOL      , 0      , 0            , 0x1    , 0x01    , 0x00     },
//{"?"                                 , ""      , 0x35      , FALSE   , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
//0x36 / 0x37 unused?
  {"?"                                 , ""      , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f    , 0.0f    , 4000.0f  },
  {"Size"                              , ""      , 0x3C      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 0xFFFF   },
  {"Display"                           , ""      , 0x3E      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 0xFFFF   },
  {"Icon"                              , ""      , 0x40      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x000    , 70       }, // short?!
  {"?"                                 , ""      , 0x42      , TRUE    , TYPE_SHORT    , HEX    , 0            , 0x1    , 0x000    , 0xFFFF   },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_mapicons_menu[] = {
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value         //steps    //min    //max  
  {"Color"                             , ""      , 0x00      , TRUE    , TYPE_INTEGER   , 0     , mapicon_color      , 0      , 0      , 0     }, // OK
  {"Linked Object Type"                , ""      , 0x04      , FALSE   , TYPE_INTEGER   , 0     , mapicon_type      , 0      , 0      , 0      }, // 1 = car, 2 = Ped, 3 = world obj,
  {"Linked Object Slot"                , ""      , 0x08      , FALSE   , TYPE_INTEGER   , 0     , mapicon_slot      , 0      , 0      , 0      }, // shift by >> 8 to get Slot number of Vehicle / Pedestrian / World Obj
  
  {"?"                                 , ""      , 0x0C      , TRUE    , TYPE_INTEGER   , HEX   , 0            , 1      , INT_MIN  , INT_MAX  },
  
  {"World Coordinate X"                , ""      , 0x10      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"                , ""      , 0x14      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"                , ""      , 0x18      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"display bool ?"                    , ""      , 0x20      , TRUE    , TYPE_BIT      , 0     , 0            , 0      , 0      , 0     },
  {"Brightness"                        , ""      , 0x20      , TRUE    , TYPE_BIT      , 1     , 0            , 0      , 0      , 0     },
  {"?remove replace save?"             , ""      , 0x20      , TRUE    , TYPE_BIT      , 2     , 0            , 0      , 0      , 0     },
  {"Display on radar only when close"  , ""      , 0x20      , TRUE    , TYPE_BIT      , 3     , 0            , 0      , 0      , 0     },
  {"unknown"                           , ""      , 0x20      , TRUE    , TYPE_BIT      , 4     , 0            , 0      , 0      , 0     },
  {"show arrow"                        , ""      , 0x20      , TRUE    , TYPE_BIT      , 5     , 0            , 0      , 0      , 0     },
  {"display on map"                    , ""      , 0x20      , TRUE    , TYPE_BIT      , 6     , 0            , 0      , 0      , 0     },
  {"display bool ?"                    , ""      , 0x20      , TRUE    , TYPE_BIT      , 7     , 0            , 0      , 0      , 0     },
  
  {"? float"                           , ""      , 0x24      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f    , 0.0f    , 4000.0f  },

  {"Size"                              , ""      , 0x28      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF     }, // 0xFF
  {"Icon"                              , ""      , 0x29      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 70     },
  {"?"                                 , ""      , 0x2A      , TRUE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
  {"?"                                 , ""      , 0x2B      , TRUE    , TYPE_BYTE      , HEX    , 0            , 0x1    , 0x00    , 0xFF     },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};


void *mapicon_color(int calltype, int keypress, int base_address, int address) { 
  static int color = 0;  
  static char buffer[16];
  
  switch( calltype ) {
    case FUNC_GET_STRING:
      if( getMapiconColor(base_address) >= 0 && getMapiconColor(base_address) <= 8 ) 
        sprintf(buffer, "%s", getMapiconColorName(base_address));
      else  
        sprintf(buffer, "RGBA: %08X", getMapiconColor(base_address));
      return (void *)buffer;
      
    case FUNC_CHANGE_VALUE:
      if( keypress && getMapiconColor(base_address) >= 0 && getMapiconColor(base_address) <= 8 ) {
        if( keypress == PSP_CTRL_LEFT && color > 0 ) { ///LEFT
          color--;
        } else if ( keypress == PSP_CTRL_RIGHT && color < 8 ) { ///RIGHT
          color++;
        }
        setInt(address, color);
      }
      break;
  }
  return NULL;
}

void *mapicon_type(int calltype, int keypress, int base_address, int address) { 
  static char buffer[16];
  
  switch( calltype ) {
    
    case FUNC_GET_STRING:
      sprintf(buffer, "%s", getMapiconTypeName(base_address));
      return (void *)buffer;
      
  }
  return NULL;
}

void *mapicon_slot(int calltype, int keypress, int base_address, int address) { 
  static char buffer[16];
  
  switch( calltype ) {
    case FUNC_GET_STRING:
      if( getMapiconLinkedObjectSlotNumber(base_address) != -1 ) 
        sprintf(buffer, "#%i", getMapiconLinkedObjectSlotNumber(base_address)+1);
      else 
        sprintf(buffer, "/");
      return (void *)buffer;
  }
  return NULL;
}

/*****************************************************************************************************************************
typedef struct lcs_handling_item {
    float       fMass;              //B
    float       fTurnMass;          //C
    float       fTractionMultiplier;//J
    float       fCollisionDamageMultiplier; (fCollisionDamageMultiplier = (fMass/2000.0)*fCollisionDamageMultiplier) //ab
    DWORD       CarId;
    float       1/fMass;
    float       fDragMult;          //D
    float       _f1C;               //null
    float       CentreOfMassX;      //F
    float       CentreOfMassY;      //G
    float       CentreOfMassZ;      //H
    float       _f2C;               //null
    int         nPercentSubmerged   //I
    float       (fMass*0.8)/nPercentSubmerged;
    float       _f38; //дублирует значение _f94 //выставляет движок
    float       _f3C; //0xBC23D70A //выставляет движок
    float       _f40; //дублирует значение _f94 //выставляет движок
    float       _f44; //Unknown, transmission gear 1 related
    float       _f48; //Unknown, transmission gear 1 related
    float       _f4C; //0xBC23D70A //выставляет движок
    float       _f50; //Unknown, transmission gear 2 related
    float       _f54; //Unknown, transmission gear 2 related
    float       _f58; //Unknown, transmission gear 2 related
    float       _f5C; //Unknown, transmission gear 3 related
    float       _f60; //Unknown, transmission gear 3 related
    float       _f64; //Unknown, transmission gear 3 related
    float       _f68; //Unknown, transmission gear 4 related
    float       _f6C; //Unknown, transmission gear 4 related
    float       _f70; //Unknown, transmission gear 4 related
    float       _f74; //Unknown, transmission gear 5 related
    float       _f78; //Unknown, transmission gear 5 related
    float       _f7C; //Unknown, transmission gear 5 related
    char        cDriveType_TransmissionData;    //Q
    char        cEngineType_TransmissionData;   //R
    byte        nNumberOfGears_TransmissionData;//M
    byte        padding;                           //null
    float       fEngineAcceleration; fEngineAcceleration=fEngineAcceleration*2500*5(*2 for 4wd)  //O                           
    float       fEngineInertia;                 //P
    float       fMaxVelocity; fMaxVelocity=fMaxVelocity*180.0 
    float       fMaxVelocity/1.2;
    float       _f94 //=-0.2 для всего транспорта, -0.05 для байков; //выставляет движок
    float       _f98;
    float       fBrakeDeceleration; fBrakeDeceleration=fBrakeDeceleration*2500; //S
    float       fBrakeBias;                     //T
    float       bABS;                           //U
    float       fSteeringLock;                  //V
    float       fTractionLoss;                  //K
    float       fTractionBias;                  //L
    float       _fB4;                           //null
    float       fSuspensionForceLevel;          //a
    float       fSuspensionDampingLevel;        //b
    float       fSuspensionUpperLimit;          //d
    float       fSuspensionLowerLimit;          //e
    float       fSuspensionBias;                //f
    float       fSuspensionAntiDive;            //g
    DWORD       dwflags <format=hex>;           //ag
    float       fSeatOffsetDistance;            //aa
    DWORD       dwMonetaryValue;                //ac
    byte        bFrontLights;                   //ah
    byte        bRearLights;                    //ai
    byte        padding[18];
  };
***********************************************************************************************************************************/
const Editor_pack lcs_handlingcfg_menu[] = {
  //name                               //postfix  //address   //edit_bool    //type    //precision  //*value              //steps      //min    //max  
  {"fMass"                              , " Kg"   , 0x00      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 1.00f    , 1.0f     , 50000.00f }, // B - OK
  {"fTurnMass"                          , " Kg"   , 0x04      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 1.00f    , 1.0f     , 50000.00f }, // C - OK
  {"fTractionMultiplier"                , " x1.0" , 0x08      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.05f    , 0.5f     , 20.0f     }, // J - OK
  {"fCollisionDamageMultiplier"         , ""      , 0x0C      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , 0.5f     , 20.0f     }, // ab
  
  {"Identifier"                         , ""      , 0x10      , TRUE    , TYPE_INTEGER    , DEC    , 0                    , 1        , 0        , 999       }, // A
  {"1/fMass"                            , ""      , 0x14      , TRUE    , TYPE_FLOAT      , 4      , 0                    , 0.0001f  , 0.00002f , 1.0f      }, // ok
  {"fDragMult"                          , ""      , 0x18      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.1f     , 0.1f     , 50.0f     }, // D
//{"_f1C"                               , ""      , 0x1C      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , 0.05f    , 999999.0f }, // unused?
  
  {"CentreOfMass.x"                     , ""      , 0x20      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // F - OK
  {"CentreOfMass.y"                     , ""      , 0x24      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // G - OK
  {"CentreOfMass.z"                     , ""      , 0x28      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // H - OK
//{"_f2C"                               , ""      , 0x2C      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // unused?
   
  {"nPercentSubmerged"                  , " %"    , 0x30      , TRUE    , TYPE_INTEGER    , DEC    , 0                    , 1        , 10       , 120       }, // I - OK
  {"(fMass * 0.8) / nPercentSubmerged"  , ""      , 0x34      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , 0.08f    , 333.33f   }, // OK
//{"?"                                  , ""      , 0x38      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , 0.08f    , 333.33f   }, // OK
//{"?"                                  , ""      , 0x3C      , TRUE    , TYPE_FLOAT      , 2      , 0                    , 0.01f    , 0.08f    , 333.33f   }, // OK
  
  ///**  transmission gears related - TODO **///
  
  {"TransmissionData.nDriveType"       , ""      , 0x80      , TRUE    , TYPE_BYTE        , 0      , vehicle_transtype    , 0x1      , 0x00     , 0xFF     }, // Q - OK
  {"TransmissionData.nEngineType"      , ""      , 0x81      , TRUE    , TYPE_BYTE        , 0      , vehicle_enginetype   , 0x1      , 0x00     , 0xFF     }, // R - OK
  {"TransmissionData.nNumberOfGears"   , ""      , 0x82      , TRUE    , TYPE_BYTE        , DEC    , 0                    , 1        , 1        , 99       }, // M - OK
//{"_f83"                              , ""      , 0x83      , TRUE    , TYPE_BYTE        , HEX    , 0                    , 0x1      , 0x00     , 0xFF     }, // unused?
  
  {"fEngineAcceleration"               , ""      , 0x84      , TRUE    , TYPE_FLOAT       , 3      , 0                    , 0.001f   , 0.00f    , 1.0f     }, // O - OK
  {"fEngineInertia"                    , ""      , 0x88      , TRUE    , TYPE_FLOAT       , 1      , 0                    , 0.5f     , 0.00f    , 50.0f    }, // P - OK
  {"fMaxVelocity"                      , ""      , 0x8C      , TRUE    , TYPE_FLOAT       , 3      , 0                    , 0.001f   , 0.001f   , 4.0f     }, // fMaxVelocity=fMaxVelocity*180.0 
  
  {"fMaxVelocity/1.2"                  , ""      , 0x90      , TRUE    , TYPE_FLOAT       , 3      , 0                    , 0.001f   , 0.001f   , 1.0f     }, 
  {"_f94"                              , ""      , 0x94      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.05f    , -0.05f   , -0.2f    }, // -0.05f for bikes else -0.2f
//  {"_f98 speed?"                     , ""      , 0x98      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.05f    , -0.05f   , -0.2f    }, // updated by game?? Is this the vehicles speed? 
  {"fBrakeDeceleration"                , ""      , 0x9C      , TRUE    , TYPE_FLOAT       , 3      , 0                    , 0.001f   , 0.00004f , 1.0f     }, // S - fBrakeDeceleration=fBrakeDeceleration*2500; [0.1 to 10.0]   =???? 
  
  {"fBrakeBias"                        , ""      , 0xA0      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.00f    , 1.0f     }, // T - OK
  {"bABS"                              , ""      , 0xA4      , TRUE    , TYPE_BOOL        , 0      , 0                    , 0        , 0x00     , 0x01     }, // U
  {"fSteeringLock"                     , ""      , 0xA8      , TRUE    , TYPE_FLOAT       , 1      , 0                    , 1.00f    , 10.0f    , 40.0f    }, // V - OK
  {"fTractionLoss"                     , ""      , 0xAC      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.0f     , 1.0f     }, // K - OK
  {"fTractionBias"                     , ""      , 0xB0      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.0f     , 1.0f     }, // L - OK
//  {"_fB4"                            , ""      , 0xB4      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.0f     , 1.0f     }, // unused?
  
  {"fSuspensionForceLevel"             , ""      , 0xB8      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.01f    , 999.0f  }, // a
  {"fSuspensionDampingLevel"           , ""      , 0xBC      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.01f    , 999.0f  }, // b
  
  {"fSuspensionUpperLimit"             , ""      , 0xC0      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.01f    , 999.0f  }, // d
  {"fSuspensionLowerLimit"             , ""      , 0xC4      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , -0.01f   , 0.0f    }, // e
  {"fSuspensionBias"                   , ""      , 0xC8      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , 0.0f     , 1.0f    }, // f (suspension bias between front and rear)
  {"fSuspensionAntiDive"               , ""      , 0xCC      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.05f    , 0.0f     , 1.0f    }, // g
                                                                                                                                                             //     D0 D1 D2 D3
                                                                                                                  /// ag   dwflags <format=hex>;    Digits: 0x 21 43 65 87
  {"ModelFlag 1"                       , ""      , 0xD0      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , vehicle_flag1        , 0x1      , 0x0      , 0xF     }, // 1: 1G_BOOST        2: 2G_BOOST       4: REV_BONNET    8: HANGING_BOOT
  {"ModelFlag 2"                       , ""      , 0xD0      , TRUE    , TYPE_NIBBLE_HIGH , DEC    , vehicle_flag2        , 0x1      , 0x0      , 0xF     }, // 1: NO_DOORS      2: IS_VAN        4: IS_BUS        8: IS_LOW
  {"ModelFlag 3"                       , ""      , 0xD1      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , vehicle_flag3        , 0x1      , 0x0      , 0xF     }, // 1: DBL_EXHAUST      2: TAILGATE_BOOT  4: NOSWING_BOOT  8: NONPLAYER_STABILISER  
  {"ModelFlag 4"                       , ""      , 0xD1      , TRUE    , TYPE_NIBBLE_HIGH , DEC    , vehicle_flag4        , 0x1      , 0x0      , 0xF     }, // 1: NEUTRALHANDLING 2: HAS_NO_ROOF    4: IS_BIG        8: HALOGEN_LIGHTS
  {"ModelFlag 5"                       , ""      , 0xD2      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , vehicle_flag5        , 0x1      , 0x0      , 0xF     }, // 1: IS_BIKE      2: IS_HELI        4: IS_PLANE      8: IS_BOAT
  {"ModelFlag 6"                       , ""      , 0xD2      , TRUE    , TYPE_NIBBLE_HIGH , DEC    , vehicle_flag6        , 0x1      , 0x0      , 0xF     }, // 1: NO_EXHAUST    2: REARWHEEL_1ST  4: HANDBRAKE_TYRE8: SIT_IN_BOAT
  {"ModelFlag 7"                       , ""      , 0xD3      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , vehicle_flag7        , 0x1      , 0x0      , 0xF     }, // 1: FAT_REARW    2: NARROW_FRONTW  4: GOOD_INSAND
  {"ModelFlag 8"                       , ""      , 0xD3      , TRUE    , TYPE_NIBBLE_HIGH , DEC    , vehicle_flag8        , 0x1      , 0x0      , 0xF     }, // 1: FORCE_GRND_CLR
 
  
  {"fSeatOffsetDistance"               , ""      , 0xD4      , TRUE    , TYPE_FLOAT       , 2      , 0                    , 0.01f    , -999.0f  , 999.0f  }, // aa
  {"nMonetaryValue"                    , " $"    , 0xD8      , TRUE    , TYPE_INTEGER     , DEC    , 0                    , 1        , 1        , 100000  }, // ac
  
  {"bFrontLights"                      , ""      , 0xDC      , TRUE    , TYPE_BYTE        , 0      , vehicle_lights_front , 1        , 0        , 3       }, // ah (0 = long, 1 = small, 2 = big, 3 = tall)
  {"bRearLights"                       , ""      , 0xDD      , TRUE    , TYPE_BYTE        , 0      , vehicle_lights_rear  , 1        , 0        , 3       }, // ai (0 = long, 1 = small, 2 = big, 3 = tall)

  ///** unused padding ***/// 
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_handlingcfg_menu[] = {
  //name                              //postfix   //address   //edit_bool    //type  //precision  //*value              //steps    //min    //max  
  {"fMass"                             , " Kg"    , 0xBC      , TRUE    , TYPE_FLOAT   , 2      , 0                    , 1.00f    , 1.0f     , 50000.00f }, // B
  {"fTurnMass"                         , " Kg"    , 0xC0      , TRUE    , TYPE_FLOAT   , 2      , 0                    , 1.00f    , 1.0f     , 50000.00f }, // C
  {"fTractionMultiplier"               , " x1.0"  , 0xC4      , TRUE    , TYPE_FLOAT   , 2      , 0                    , 0.05f    , 0.5f     , 20.0f     }, // J - OK
  {"fCollisionDamageMultiplier"        , ""       , 0xC8      , TRUE    , TYPE_FLOAT   , 2      , 0                    , 0.01f    , 0.5f     , 20.0f     }, // ab
  
  {"fDragMult"                         , ""      , 0x98      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.1f     , 0.1f     , 50.0f     }, // D
  
  {"CentreOfMass.x"                    , ""      , 0xA0      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // F - OK
  {"CentreOfMass.y"                    , ""      , 0xA4      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // G - OK
  {"CentreOfMass.z"                    , ""      , 0xA8      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // H - OK
//{"_f2C"                              , ""      , 0xAC      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , -10.0f   , 10.0f     }, // unused?
  
  {"nPercentSubmerged"                 , " %"    , 0xB0      , TRUE    , TYPE_INTEGER  , DEC    , 0                    , 1        , 10       , 120       }, // I - OK
  {"(fMass * 0.8) / nPercentSubmerged" , ""      , 0xB4      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.08f    , 333.33f   }, // OK
//{"?"                                 , ""      , 0xB8      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.08f    , 333.33f   }, // OK

  {"TransmissionData.nDriveType"       , ""      , 0x5C      , TRUE    , TYPE_BYTE     , 0      , vehicle_transtype    , 0x1      , 0x00     , 0xFF      }, // Q - OK
  {"TransmissionData.nEngineType"      , ""      , 0x5D      , TRUE    , TYPE_BYTE     , 0      , vehicle_enginetype   , 0x1      , 0x00     , 0xFF      }, // R - OK
  {"TransmissionData.nNumberOfGears"   , ""      , 0x5E      , TRUE    , TYPE_BYTE     , DEC    , 0                    , 1        , 1        , 99        }, // M - OK
//{"_f83"                              , ""      , 0x5F      , TRUE    , TYPE_BYTE     , HEX    , 0                    , 0x1      , 0x00     , 0xFF      }, // unused?
  
  {"fEngineAcceleration"               , ""      , 0x48      , TRUE    , TYPE_FLOAT    , 3      , 0                    , 0.001f   , 0.00f    , 1.0f      }, // O - OK
  {"fEngineInertia"                    , ""      , 0x4C      , TRUE    , TYPE_FLOAT    , 1      , 0                    , 0.5f     , 0.00f    , 50.0f     }, // P - OK
  {"fMaxVelocity"                      , ""      , 0x50      , TRUE    , TYPE_FLOAT    , 3      , 0                    , 0.001f   , 0.001f   , 4.0f      }, // fMaxVelocity=fMaxVelocity*180.0 
  
  {"fMaxVelocity/1.2"                  , ""      , 0x54      , TRUE    , TYPE_FLOAT    , 3      , 0                    , 0.001f   , 0.001f   , 1.0f      }, 
  {"_f94"                              , ""      , 0x58      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.05f    , -0.05f   , -0.2f     }, // -0.05f for bikes else -0.2f
//{"_f98 speed?"                       , ""      , 0x5C      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.05f    , -0.05f   , -0.2f     }, // updated by game?? Is this the vehicles speed? 
  {"fBrakeDeceleration"                , ""      , 0x60      , TRUE    , TYPE_FLOAT    , 3      , 0                    , 0.001f   , 0.00004f , 1.0f      }, // S - fBrakeDeceleration=fBrakeDeceleration*2500; [0.1 to 10.0]   =???? 
  
  {"fBrakeBias"                        , ""      , 0x64      , TRUE    , TYPE_FLOAT    , 2     , 0                     , 0.01f    , 0.00f    , 1.0f      }, // T - OK
  
  {"fSteeringLock"                     , ""      , 0x68      , TRUE    , TYPE_FLOAT    , 1      , 0                    , 1.00f    , 10.0f    , 40.0f     }, // V - OK
  {"fTractionLoss"                     , ""      , 0x6C      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.0f     , 1.0f      }, // K - OK
  {"fTractionBias"                     , ""      , 0x70      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.0f     , 1.0f      }, // L - OK
  
  {"fSuspensionForceLevel"             , ""      , 0x74      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.01f    , 999.0f    }, // a
  {"fSuspensionDampingLevel"           , ""      , 0x78      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.01f    , 999.0f    }, // b
  
  {"fSuspensionUpperLimit"             , ""      , 0x7C      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.01f    , 999.0f    }, // d
  {"fSuspensionLowerLimit"             , ""      , 0x80      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , -0.01f   , 0.0f      }, // e
  {"fSuspensionBias"                   , ""      , 0x84      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , 0.0f     , 1.0f      }, // f (suspension bias between front and rear)
  {"fSuspensionAntiDive"               , ""      , 0x88      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.05f    , 0.0f     , 1.0f      }, // g
                                                                                                                                                //     D0 D1 D2 D3
  {"fSeatOffsetDistance"               , ""      , 0x8C      , TRUE    , TYPE_FLOAT    , 2      , 0                    , 0.01f    , -999.0f  , 999.0f    }, // aa
  {"nMonetaryValue"                    , " $"    , 0x90      , TRUE    , TYPE_INTEGER  , DEC    , 0                    , 1        , 1        , 100000    }, // ac
  
  {"bFrontLights"                      , ""      , 0x94      , TRUE    , TYPE_BYTE     , 0      , vehicle_lights_front , 1        , 0        , 3         }, // ah (0 = long, 1 = small, 2 = big, 3 = tall)
  {"bRearLights"                       , ""      , 0x95      , TRUE    , TYPE_BYTE     , 0      , vehicle_lights_rear  , 1        , 0        , 3         }, // ai (0 = long, 1 = small, 2 = big, 3 = tall)

/*  
  {"ModelFlag 1"              , ""      , 0xD0      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , 0            , 0x1    , 0x0    , 0xF     }, //
  {"ModelFlag 2"              , ""      , 0xD0      , TRUE    , TYPE_NIBBLE_HIGH  , DEC    , 0            , 0x1    , 0x0    , 0xF     }, //
  {"ModelFlag 3"              , ""      , 0xD1      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , 0            , 0x1    , 0x0    , 0xF     }, //  
  {"ModelFlag 4"              , ""      , 0xD1      , TRUE    , TYPE_NIBBLE_HIGH  , DEC    , 0            , 0x1    , 0x0    , 0xF     }, //
  {"ModelFlag 5"              , ""      , 0xD2      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , 0            , 0x1    , 0x0    , 0xF     }, //
  {"ModelFlag 6"              , ""      , 0xD2      , TRUE    , TYPE_NIBBLE_HIGH  , DEC    , 0            , 0x1    , 0x0    , 0xF     }, //
  {"ModelFlag 7"              , ""      , 0xD3      , TRUE    , TYPE_NIBBLE_LOW  , DEC    , vehicle_flag5      , 0x1    , 0x0    , 0xF     }, // 1: IS_BIKE      2: IS_HELI        4: IS_PLANE      8: IS_BOAT
  {"ModelFlag 8"              , ""      , 0xD3      , TRUE    , TYPE_NIBBLE_HIGH  , DEC    , 0            , 0x1    , 0x0    , 0xF     }, //
 */
  {NULL,NULL,0,0,0,0,0,0,0}
};

void *vehicle_flag1(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleLow(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch( nibble ) {
      case 0x1: return "1G_BOOST";
      case 0x2: return "2G_BOOST";
      case 0x4: return "REV_BONNET";
      case 0x8: return "HANGING_BOOT";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if ( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if ( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleLow(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_flag2(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleHigh(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch( nibble ) {
      case 0x1: return "NO_DOORS";
      case 0x2: return "IS_VAN";
      case 0x4: return "IS_BUS";
      case 0x8: return "IS_LOW";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleHigh(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_flag3(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleLow(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch( nibble ) { 
      case 0x1: return "DBL_EXHAUST";
      case 0x2: return "TAILGATE_BOOT";
      case 0x4: return "NOSWING_BOOT";
      case 0x8: return "NONPLAYER_STABILISER";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if ( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if ( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleLow(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_flag4(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleHigh(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch( nibble ) {
      case 0x1: return "NEUTRALHANDLING";
      case 0x2: return "HAS_NO_ROOF";
      case 0x4: return "IS_BIG";
      case 0x8: return "HALOGEN_LIGHTS";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleHigh(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_flag5(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleLow(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch( nibble ) {
      case 0x1: return "IS_BIKE";
      case 0x2: return "IS_HELI";
      case 0x4: return "IS_PLANE";
      case 0x8: return "IS_BOAT";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleLow(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_flag6(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleHigh(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch( nibble ) {
      case 0x1: return "NO_EXHAUST";
      case 0x2: return "REARWHEEL_1ST";
      case 0x4: return "HANDBRAKE_TYRE8";
      case 0x8: return "SIT_IN_BOAT";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleHigh(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_flag7(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleLow(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch(nibble) {
      case 0x1: return "FAT_REARW";
      case 0x2: return "NARROW_FRONTW";
      case 0x4: return "GOOD_INSAND";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleLow(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_flag8(int calltype, int keypress, int base_address, int address) { 
  static char buffer[8];
  char nibble = getNibbleHigh(address);
  
  if( calltype == FUNC_GET_STRING ) {
    switch( nibble ) {
      case 0x1: return "FORCE_GRND_CLR";
      
      default: 
        sprintf(buffer, "%i", nibble);
        return (void *)buffer;  
    }
  }
  
  if( calltype == FUNC_CHANGE_VALUE ) {
    if( keypress ) {
      if( keypress == PSP_CTRL_LEFT && nibble > 0x0 ) // LEFT
        nibble--;
        
      if( keypress == PSP_CTRL_RIGHT && nibble < 0xF ) // RIGHT
        nibble++;
      
      setNibbleHigh(address, nibble);
    }
  }
  
  return NULL;
}

void *vehicle_enginetype(int calltype, int keypress, int base_address, int address) { 
  const char *list_name[] = { "Diesel", "Electro", "Petrol"};
  const unsigned char list_val[] = { 0x44, 0x45, 0x50 };
  static short list_size = (sizeof(list_val)/sizeof(*list_val));
  static short i = 0;  
  static char buffer[16];
  unsigned char current;
  
  switch( calltype ) {
    
    case FUNC_GET_STRING:
      current = getByte(address);
      for( i = 0; i < list_size; i++ ) {
        if( current == list_val[i] ) break;
      }
      if( i == list_size && current != list_val[i-1]) {
        i = -1; // error - not found in list
        // else unknown -> output id
        sprintf(buffer, "0x%X", current);
        return (void *)buffer;
      }
      return (void *)list_name[i];
  
    case FUNC_CHANGE_VALUE:
      if( keypress ) {
        if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
          i--;
        } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
          if( LCS && i < list_size-1 ) 
            i++;
        }
        if(i >= 0) 
          setByte(address, list_val[i]);
      } break;
  }
  return NULL;
}

void *vehicle_lights_front(int calltype, int keypress, int base_address, int address ) {  
  const char *list_name[] = { "Long", "Small", "Big", "Tall"};
  const unsigned char list_val[] = { 0x00, 0x01, 0x02, 0x03 }; // 0 = long, 1 = small, 2 = big, 3 = tall
  static short list_size = (sizeof(list_val)/sizeof(*list_val));
  static short i = 0;  
  static char buffer[16];
  unsigned char current;
  
  switch( calltype ) {
    
    case FUNC_GET_STRING:
      current = getByte(address);
      for( i = 0; i < list_size; i++ ) {
        if( current == list_val[i] ) break;
      }
      if( i == list_size && current != list_val[i-1]) {
        i = -1; // error - not found in list
        // else unknown -> output id
        sprintf(buffer, "0x%X", current);
        return (void *)buffer;
      }
      return (void *)list_name[i];
  
    case FUNC_CHANGE_VALUE:
      if( keypress ) {
        if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
          i--;
        } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
          if( LCS && i < list_size-1 ) 
            i++;
        }
        if(i >= 0) 
          setByte(address, list_val[i]);
      } break;
  }
  return NULL;
}

void *vehicle_lights_rear(int calltype, int keypress, int base_address, int address ) {  
  const char *list_name[] = { "Long", "Small", "Big", "Tall"};
  const unsigned char list_val[] = { 0x00, 0x01, 0x02, 0x03 }; // 0 = long, 1 = small, 2 = big, 3 = tall
  static short list_size = (sizeof(list_val)/sizeof(*list_val));
  static short i = 0;  
  static char buffer[16];
  unsigned char current;
  
  switch( calltype ) {
    
    case FUNC_GET_STRING:
      current = getByte(address);
      for( i = 0; i < list_size; i++ ) {
        if( current == list_val[i] ) break;
      }
      if( i == list_size && current != list_val[i-1]) {
        i = -1; // error - not found in list
        // else unknown -> output id
        sprintf(buffer, "0x%X", current);
        return (void *)buffer;
      }
      return (void *)list_name[i];
  
    case FUNC_CHANGE_VALUE:
      if( keypress ) {
        if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
          i--;
        } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
          if( LCS && i < list_size-1 ) 
            i++;
        }
        if(i >= 0) 
          setByte(address, list_val[i]);
      } break;
  }
  return NULL;
}

void *vehicle_transtype(int calltype, int keypress, int base_address, int address ) { 
  const char *list_name[] = { "Rear", "Front", "4-Wheel"};
  const unsigned char list_val[] = { 0x52, 0x46, 0x34 };
  static short list_size = (sizeof(list_val)/sizeof(*list_val));
  static short i = 0;  
  static char buffer[16];
  unsigned char current;
  
  switch( calltype ) {
    case FUNC_GET_STRING:
      current = getByte(address);
      for( i = 0; i < list_size; i++ ) {
        if( current == list_val[i] ) break;
      }
      if( i == list_size && current != list_val[i-1]) {
        i = -1; // error - not found in list
        // else unknown -> output id
        sprintf(buffer, "0x%X", current);
        return (void *)buffer;
      }
      return (void *)list_name[i];
  
    case FUNC_CHANGE_VALUE:
      if( keypress ) {
        if( keypress == PSP_CTRL_LEFT && i > 0 ) { // LEFT
          i--;
        } else if( keypress == PSP_CTRL_RIGHT ) { // RIGHT
          if( LCS && i < list_size-1 ) 
            i++;
        }
        if(i >= 0) 
          setByte(address, list_val[i]);
      } break;
  }
  return NULL;
}



const Editor_pack lcs_vehiclespawns_menu[] = {
  //name                        //postfix    //address     //edit_bool  //type      //precision  //*value         //steps    //min    //max  
  {"Identifier"                     , ""      , 0x00      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0x82      , 0xD8     },
  
  {"World Coordinate X"             , ""      , 0x04      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f   , -4000.0f  , 4000.0f  },
  {"World Coordinate Y"             , ""      , 0x08      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f   , -4000.0f  , 4000.0f  },
  {"World Coordinate Z"             , ""      , 0x0C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f   , -4000.0f  , 4000.0f  },
  
  {"Rotation"                       , ""      , 0x10      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f      , 360.0f   },
  
  {"Primary Color"                  , ""      , 0x14      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , -1        , 128      }, // -1 for random
  {"Secondary Color"                , ""      , 0x16      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , -1        , 128      }, // -1 for random
  
  {"force_spawn"                    , ""      , 0x18      , TRUE    , TYPE_BOOL     , 0      , 0            , 0      , 0x00      , 0x01     },
  {"alarm"                          , ""      , 0x19      , TRUE    , TYPE_BYTE     , DEC    , 0            , 0x1    , 0x00      , 0xFF     },
  {"door_lock"                      , ""      , 0x1A      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x0000    , 0xFFFF   },
  {"min_delay ?"                    , ""      , 0x1C      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x0000    , 0xFFFF   },
  {"max_delay ? *1000 ?"            , ""      , 0x1E      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x0000    , 0xFFFF   },

  {"gametime + delay (after spawn)" , ""      , 0x20      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0         , INT_MAX  }, // when gametime >= value then new vehicle can spawn
  {"?"                              , ""      , 0x24      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , -1        , 999      },
  {"?"                              , ""      , 0x26      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , -1        , 999      },
  
  {"bool (can spawn when -1)"       , ""      , 0x28      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , -1        , 0        }, // -1 to allow spawn (inside time window which is different for slots)
  {"bool (cannot spawn when 1)"     , ""      , 0x2A      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , -1        , 1        }, // 0 to allow spawn (enough time since last spawn passed and nothing blocking in world)


  // TODO spawn in specific time period only? how handled???

  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_vehiclespawns_menu[] = {
  //name                          //postfix    //address     //edit_bool  //type   //precision  //*value    //steps    //min    //max  
  {"Identifier"                     , ""      , 0x00      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0        , INT_MAX  },
  {"World Coordinate X"             , ""      , 0x04      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Y"             , ""      , 0x08      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  {"World Coordinate Z"             , ""      , 0x0C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     },
  
  {"Rotation"                       , ""      , 0x10      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 360.0f   },
  
  {"color ?"                        , ""      , 0x14      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"color ?"                        , ""      , 0x18      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },  
  
  {"force_spawn"                    , ""      , 0x1C      , TRUE    , TYPE_BOOL     , 0      , 0            , 0      , 0x00     , 0x01     },
  {"alarm"                          , ""      , 0x1D      , TRUE    , TYPE_BYTE     , DEC    , 0            , 0x1    , 0x00     , 0xFF     },
  {"door_lock"                      , ""      , 0x1E      , TRUE    , TYPE_SHORT    , DEC    , 0            , 0x1    , 0x0000   , 0xFFFF   },
  
  {"gametime + delay (after spawn)" , ""      , 0x20      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0        , INT_MAX  }, // when gametime >= value then new vehicle can spawn
  
  {"bool (can spawn when -1)"       , ""      , 0x2C      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , -1       , 0        }, // -1 to allow spawn
  {"bool (cannot spawn when 1)"     , ""      , 0x2E      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , -1       , 1        }, // 0 to allow spawn (enough time since last spawn passed and nothing blocking in world)

  /// TODO 

  {NULL,NULL,0,0,0,0,0,0,0}
};


const Editor_pack lcs_colsdat_menu[] = {
  //name                    //postfix    //address    //edit_bool //type        //precision  //*value       //steps    //min    //max  
  {"Red value"              , " of 255"    , 0x00      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF },
  {"Green value"            , " of 255"    , 0x01      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF },
  {"Blue value"             , " of 255"    , 0x02      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF },
  {"Alpha value"            , " of 255"    , 0x03      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF },
  {NULL,NULL,0,0,0,0,0,0,0}
};
const Editor_pack vcs_colsdat_menu[] = {
  //name                    //postfix    //address    //edit_bool //type        //precision  //*value      //steps    //min    //max  
  {"Red value"              , " of 255"    , 0x00      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF },
  {"Green value"            , " of 255"    , 0x01      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF },
  {"Blue value"             , " of 255"    , 0x02      , TRUE    , TYPE_BYTE      , DEC    , 0            , 0x1    , 0x00    , 0xFF },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_ipl_menu[] = { // todo for rotation : https://gtamods.com/wiki/Game.dtz#Generating_IPL_in_text_format
  //name                  //postfix       //address     //edit_bool  //type     //precision  //*value      //steps    //min    //max  
  {"World Coordinate X"     , ""          , 0x30       , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f    , -4000.0f  , 4000.0f  },
  {"World Coordinate Y"     , ""          , 0x34       , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f    , -4000.0f  , 4000.0f  },
  {"World Coordinate Z"     , ""          , 0x38       , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1f    , -4000.0f  , 4000.0f  },
  
  {"Identifier"             , ""          , 0x58       , TRUE    , TYPE_SHORT    , DEC    , 0            , 1       , 0x82      , 0xD8     },

  // TODO 

  {NULL,NULL,0,0,0,0,0,0,0}
};


const Editor_pack lcs_ide_menu[] = { 
  //name                    //postfix   //address  //edit_bool  //type        //precision  //*value         //steps    //min    //max  
  {"Name (CRC32 hashed)"      , ""      , 0x08      , TRUE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"Type"                     , ""      , 0x10      , TRUE    , TYPE_BYTE      , DEC    , 0            , 1      , 0x00     , 0xFF     },
  {"Count 2dfx"               , ""      , 0x11      , TRUE    , TYPE_BYTE      , DEC    , 0            , 1      , 0x00     , 0xFF     },
  {"Collision in DTZ"         , ""      , 0x12      , TRUE    , TYPE_BOOL      , 0      , 0            , 1      , 0        , 1        },
  {"Offset to Collision"      , ""      , 0x14      , TRUE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"2dfx ID"                  , ""      , 0x18      , TRUE    , TYPE_SHORT     , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"Object.DAT ID"            , ""      , 0x1A      , TRUE    , TYPE_SHORT     , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"?"                        , ""      , 0x1C      , TRUE    , TYPE_SHORT     , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"Texture ID"               , ""      , 0x1E      , TRUE    , TYPE_SHORT     , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"Type (CRC32 hashed)"      , ""      , 0x20      , TRUE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"Offset to model in .DTZ"  , ""      , 0x24      , TRUE    , TYPE_INTEGER   , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {NULL,NULL,0,0,0,0,0,0,0}
};
const Editor_pack lcs_ide_obj_menu[] = {  /// 1 - obj 
  {"Drawdistance 1"                        , ""      , 0x28      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f },
  {"Drawdistance 2"                        , ""      , 0x2C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f },
  {"Drawdistance 3 / (Disap-dist. if LOD)" , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f },
  {"Mesh Count"                            , ""      , 0x34      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"?"                                     , ""      , 0x35      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"flags ?"                               , ""      , 0x36      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF     },
  {"flags ?"                               , ""      , 0x37      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF     },
  {"Offset to orig. IDE object (if LOD)"   , ""      , 0x38      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_ide_tobj_menu[] = {  /// 3 - tobj
  {"Drawdistance 1"                        , ""      , 0x28      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f  },
  {"Drawdistance 2"                        , ""      , 0x2C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f  },
  {"Drawdistance 3 / (Disap-dist. if LOD)" , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f  },
  {"Mesh Count"                            , ""      , 0x34      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1       , 0x00    , 0xFF     },
  {"?"                                     , ""      , 0x35      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1       , 0x00    , 0xFF     },
  {"flags ?"                               , ""      , 0x36      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1       , 0x00    , 0xFF     },
  {"flags ?"                               , ""      , 0x37      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1       , 0x00    , 0xFF     },
  {"Offset to orig. IDE object (if LOD)"   , ""      , 0x38      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1       , INT_MIN  , INT_MAX },
  {"Time On"                               , ""      , 0x3C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1       , INT_MIN  , INT_MAX },
  {"Time Off"                              , ""      , 0x40      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1       , INT_MIN  , INT_MAX },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_ide_weap_menu[] = {  /// 4 - weap
  {"Drawdistance 1"                        , ""      , 0x28      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f  },
  {"Drawdistance 2"                        , ""      , 0x2C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f  },
  {"Drawdistance 3 / (Disap-dist. if LOD)" , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f  },
  {"Mesh Count"                            , ""      , 0x34      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF      },
  {"?"                                     , ""      , 0x35      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF      },
  {"flags ?"                               , ""      , 0x36      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF      },
  {"flags ?"                               , ""      , 0x37      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF      },
  {"Weapon.DAT ID"                         , ""      , 0x38      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"Animation ID"                          , ""      , 0x3C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_ide_hier_menu[] = {  /// 5 - hier
  // unused
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_ide_cars_menu[] = {  /// 6 - cars
  {"Primary Color"                        , ""      , 0x2C      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"Secondary Color"                      , ""      , 0x2D      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  
  {"GXT name"                             , ""      , 0x2E      , TRUE    , TYPE_STRING   , 8      , 0            , 0      , 0      , 0         },
  {"Vehicle Type"                         , ""      , 0x38      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0      , 6         }, // 0 = Car / Heli, 1 = Boat, 2 = train, 3 = toyheli(s), 4 = deaddodo / aeroplane, 5 = Bike, 6 = Ferry
  
  {"Wheel Scale"                          , ""      , 0x3C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01f  , 0.0f    , 9999.0f  },
  {"Wheel ID (IDE)"                       , ""      , 0x40      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 237    , 256       }, // IDE
  
  {"Handling ID"                          , ""      , 0x42      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"Number of Doors"                      , ""      , 0x44      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  
  {"Headlights spacing"                   , ""      , 0x50      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Headlights position y"                , ""      , 0x54      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Headlights position z"                , ""      , 0x58      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Rearlights spacing"                   , ""      , 0x60      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Rearlights position y"                , ""      , 0x64      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Rearlights position z"                , ""      , 0x68      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Frontseats position x"                , ""      , 0x70      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Frontseats position y"                , ""      , 0x74      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Frontseats position z"                , ""      , 0x78      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Backseats position x"                 , ""      , 0x80      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Backseats position y"                 , ""      , 0x84      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  {"Backseats position z"                 , ""      , 0x88      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , -200.0f  , 200.0f  },
  
  {"?"                                    , ""      , 0xA4      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f },
  {"?"                                    , ""      , 0x184     , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f    , 0.0f    , 9999.0f },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack lcs_ide_ped_menu[] = {  /// 7 - ped
  {" ?"                                   , ""      , 0x2C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX },
  {"Threat (ped.dat)"                     , ""      , 0x30      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX },
  {"Behaviour (pedstat.dat)"              , ""      , 0x34      , TRUE    , TYPE_INTEGER  , 0      , pedstatname  , 0      , 0        , 0       },
  {" ?"                                   , ""      , 0x38      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"Offset to ?"                          , ""      , 0x3C      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX },
  {"RadioA"                               , ""      , 0x40      , TRUE    , TYPE_BYTE     , 0      , radiostation , 0      , 0        , 0       },
  {"RadioB"                               , ""      , 0x41      , TRUE    , TYPE_BYTE     , 0      , radiostation , 0      , 0        , 0       },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_ide_menu[] = { 
  //name                               //postfix    //address  //edit_bool  //type        //precision  //*value         //steps    //min    //max  
  {"Name (CRC32 hashed)"                  , ""      , 0x08      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"Type"                                 , ""      , 0x10      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00     , 0xFF     },
  {"Number of 2dfx"                       , ""      , 0x11      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00     , 0xFF     },
  {"Collision in DTZ"                     , ""      , 0x12      , TRUE    , TYPE_BOOL     , 0      , 0            , 1      , 0        , 1        },
  {"Offset to Collision"                  , ""      , 0x14      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"2dfx ID"                              , ""      , 0x18      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"Object.DAT ID"                        , ""      , 0x1A      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"Texture ID"                           , ""      , 0x1E      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"?"                                    , ""      , 0x20      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0x0000   , 0xFFFF   },
  {"Type (CRC32 hashed)"                  , ""      , 0x24      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"Offset to model in .DTZ"              , ""      , 0x28      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_ide_obj_menu[] = { 
  {"Drawdistance 1"                       , ""      , 0x2C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  {"Drawdistance 2"                       , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  {"Drawdistance 3 / (Disap-dist. if LOD)", ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  {"Mesh Count"                           , ""      , 0x38      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"?"                                    , ""      , 0x39      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"flags ?"                              , ""      , 0x3A      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF     },
  {"flags ?"                              , ""      , 0x3B      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF     },
  {"Offset to orig. IDE object (if LOD)"  , ""      , 0x3C      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN , INT_MAX  }, 
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_ide_tobj_menu[] = { 
  {"Drawdistance 1"                       , ""      , 0x2C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  {"Drawdistance 2"                       , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  {"Drawdistance 3 / (Disap-dist. if LOD)", ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  {"Mesh Count"                           , ""      , 0x38      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"?"                                    , ""      , 0x39      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"flags ?"                              , ""      , 0x3A      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF     },
  {"flags ?"                              , ""      , 0x3B      , TRUE    , TYPE_BYTE     , HEX    , 0            , 1      , 0x00    , 0xFF     },
  {"Offset to orig. IDE object (if LOD)"  , ""      , 0x3C      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN , INT_MAX  },
  {"Time On"                              , ""      , 0x40      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN , INT_MAX  },
  {"Time Off"                             , ""      , 0x44      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN , INT_MAX  },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_ide_weap_menu[] = { 
  {"Drawdistance 1"                       , ""      , 0x2C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f     , 9999.0f  },
  {"Drawdistance 2"                       , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f     , 9999.0f  },
  {"Drawdistance 3 / (Disap-dist. if LOD)", ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f     , 9999.0f  },
  {"?"                                    , ""      , 0x38      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"Weapon.DAT ID"                        , ""      , 0x3C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"?"                                    , ""      , 0x40      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  },
    
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_ide_hier_menu[] = { 
//{"Unknown"                 , ""      , 0x34      , TRUE    , TYPE_INTEGER    , DEC    , 0            , 1      , INT_MIN  , INT_MAX   },
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_ide_cars_menu[] = { 
/*{"Colouring R"              , " of 255"    , 0x30      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     }, 
  {"Colouring G"              , " of 255"    , 0x31      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"Colouring B"              , " of 255"    , 0x32      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"Colouring Alpha"          , " of 255"    , 0x33      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     }, 
  
  {"Colouring R"              , " of 255"    , 0x34      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"Colouring G"              , " of 255"    , 0x35      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"Colouring B"              , " of 255"    , 0x36      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },
  {"Colouring Alpha"          , " of 255"    , 0x37      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     },*/
  
  {"?"                        , ""           , 0x50      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  {"Vehicle Type"             , ""           , 0x54      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0       , 6        }, // 0 = Car / Heli, 1 = Boat, 2 = train, 3 = toyheli(s), 4 = deaddodo / aeroplane, 5 = Bike, 6 = Ferry
  
  {"Wheel Scale"              , ""           , 0x58      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01f  , 0.0f    , 9999.0f  },
  {"rear height ?"            , ""           , 0x5C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01f  , 0.0f    , 9999.0f  },
  {"?"                        , ""           , 0x60      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01f  , 0.0f    , 9999.0f  },
  {"?"                        , ""           , 0x64      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01f  , 0.0f    , 9999.0f  },
  {"?"                        , ""           , 0x68      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.01f  , 0.0f    , 9999.0f  },
  
  {"?"                        , ""           , 0x154     , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.0f   , 0.0f    , 9999.0f  },
  
  {"Name"                     , ""           , 0x158     , TRUE    , TYPE_STRING   , 8      , 0            , 0      , 0       , 0        },
  
//{"?"                        , ""           , 0x168     , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN , INT_MAX  },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_ide_ped_menu[] = { 
  {"?"                        , ""           , 0x30      , TRUE    , TYPE_INTEGER   , DEC   , 0            , 1      , INT_MIN  , INT_MAX },
  {"Threat (ped.dat)"         , ""           , 0x34      , TRUE    , TYPE_INTEGER   , DEC   , 0            , 1      , INT_MIN  , INT_MAX },
  {"Behaviour (pedstat.dat)"  , ""           , 0x38      , TRUE    , TYPE_INTEGER   , 0     , pedstatname  , 0      , 0      , 0         },
  {"?"                        , ""           , 0xB8      , TRUE    , TYPE_INTEGER   , DEC   , 0            , 1      , INT_MIN  , INT_MAX },
  {"?"                        , ""           , 0xC8      , TRUE    , TYPE_INTEGER   , DEC   , 0            , 1      , INT_MIN  , INT_MAX },
  {"Name"                     , ""           , 0xE4      , TRUE    , TYPE_STRING    , 8     , 0            , 0      , 0        , 0       },
  {NULL,NULL,0,0,0,0,0,0,0}
};
  
  
const Editor_pack lcs_particlecfg_menu[] = { 
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value         //steps    //min    //max  
  {"Particle Type Name"                   , ""        , 0x04      , TRUE    , TYPE_STRING    , 20    , 0            , 0      , 0      , 0     }, // A
  
  {"Render Colouring R"                   , " of 255" , 0x70      , TRUE    , TYPE_BYTE      , DEC   , 0            , 1      , 0x00    , 0xFF     }, // B
  {"Render Colouring G"                   , " of 255" , 0x71      , TRUE    , TYPE_BYTE      , DEC   , 0            , 1      , 0x00    , 0xFF     }, // C
  {"Render Colouring B"                   , " of 255" , 0x72      , TRUE    , TYPE_BYTE      , DEC   , 0            , 1      , 0x00    , 0xFF     }, // D
  {"Initial Color Variation"              , ""        , 0x74      , TRUE    , TYPE_BYTE      , DEC   , 0            , 1      , 0      , 100     }, // CV (for r,g,b only, in %) (0-100);
  
  {"0x18 ?"                               , ""        , 0x18      , TRUE    , TYPE_FLOAT    , 2      , 0            , 1.00f    , -9999.0f  , 9999.0f  }, //
  
  {"Default Initial Radius"               , ""        , 0x1C      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.001f  , -9999.0f  , 9999.0f  }, // E
  {"Expansion Rate"                       , ""        , 0x20      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.001f  , -9999.0f  , 9999.0f  }, // F

  ///Color "Fade-to-Black" options:
  {"Initial Intensity (0-255)"            , " of 255" , 0x36      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 255      }, // G
  {"Fade Time"                            , ""        , 0x34      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 9999    }, // H
  {"Fade Amount (-255 to 255)"            , ""        , 0x38      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 9999    }, // I

  ///"Fade Alpha" options:
  {"Initial Intensity (alpha)"            , " of 255" , 0x3A      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 9999    }, // GA
  {"Fade Time (alpha)"                    , ""        , 0x3C      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 9999    }, // HA
  {"Fade Amount (alpha)"                  , ""        , 0x3E      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 9999    }, // IA
  
  ///"Z Rotation" options:  
  {"Initial Angle (0-1023)"               , ""        , 0x24      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 1023     }, // GZA
  {"Change Time"                          , ""        , 0x28      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 999      }, // HZA
  {"Angle Change Amount"                  , ""        , 0x26      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 999      }, // IZA
  {"Initial Z Radius"                     , ""        , 0x2C      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.001f  , -4000.0f  , 4000.0f    }, // GZR
  {"Change Time"                          , ""        , 0x2A      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 999      }, // HZR
  {"Z Radius Change Amount"               , ""        , 0x30      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.001f  , -4000.0f  , 4000.0f    }, // IZR


  {"Animation Speed"                      , ""       , 0x42      , TRUE    , TYPE_SHORT    , DEC    , 0            , 1      , 0      , 999      }, // J   
  {"Start Animation Frame"                , ""       , 0x40      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     }, // K
  {"Final Animation Frame"                , ""       , 0x41      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     }, // L
  {"Rotation Speed (0=None,i-deg/frame)"  , ""       , 0x44      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0      , 999      }, // M
  {"Gravitational Acceleration"           , ""       , 0x48      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.001f  , -4000.0f  , 4000.0f   }, // N
  
  {"Drag/Friction Decceleration"          , ""       , 0x4C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0      , 999      }, // O
  {"Default Life-Span of Particle"        , " msec"  , 0x50      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , 0      , 999      }, // P
  {"Position Random Error"                , ""       , 0x54      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f    , -4000.0f  , 4000.0f   }, // Q
  {"Velocity Random Error"                , ""       , 0x58      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f    , -4000.0f  , 4000.0f   }, // R
  {"Expansion Rate Error"                 , ""       , 0x5C      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f    , -4000.0f  , 4000.0f   }, // S
  {"Rotation Rate Error"                  , ""       , 0x60      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // T
  {"Life-Span Error Shape"                , ""       , 0x64      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1      , 0x00    , 0xFF     }, // U
  {"Trail Length Multiplier"              , ""       , 0x68      , TRUE    , TYPE_FLOAT    , 1      , 0            , 0.1f    , 0.0f    , 9999.9f   }, // V
  
  {"Stretch value of texture in X"        , ""       , 0x80      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f    , -4000.0f  , 4000.0f   }, // WX
  {"Stretch value of texture in Y"        , ""       , 0x84      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f    , -4000.0f  , 4000.0f   }, // WY
  {"Wind factor"                          , ""       , 0x88      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f    , -4000.0f  , 4000.0f   }, // WI



  {"ZCHECK_FIRST"                         , ""      , 0x6C      , TRUE    , TYPE_BIT      , 0     , 0            , 0      , 0      , 0     }, // Z
  {"ZCHECK_STEP"                          , ""      , 0x6C      , TRUE    , TYPE_BIT      , 1     , 0            , 0      , 0      , 0     }, 
  {"DRAW_OPAQUE"                          , ""      , 0x6C      , TRUE    , TYPE_BIT      , 2     , 0            , 0      , 0      , 0     }, 
  {"SCREEN_TRAIL"                         , ""      , 0x6C      , TRUE    , TYPE_BIT      , 3     , 0            , 0      , 0      , 0     },
  {"SPEED_TRAIL"                          , ""      , 0x6C      , TRUE    , TYPE_BIT      , 4     , 0            , 0      , 0      , 0     }, 
  {"RAND_VERT_V"                          , ""      , 0x6C      , TRUE    , TYPE_BIT      , 5     , 0            , 0      , 0      , 0     }, 
  {"CYCLE_ANIM"                           , ""      , 0x6C      , TRUE    , TYPE_BIT      , 6     , 0            , 0      , 0      , 0     }, 
  {"DRAW_DARK"                            , ""      , 0x6C      , TRUE    , TYPE_BIT      , 7     , 0            , 0      , 0      , 0     }, 
  {"VERT_TRAIL"                           , ""      , 0x6D      , TRUE    , TYPE_BIT      , 0     , 0            , 0      , 0      , 0     },
  {"DRAWTOP2D"                            , ""      , 0x6D      , TRUE    , TYPE_BIT      , 1     , 0            , 0      , 0      , 0     }, 
  {"CLIPOUT2D"                            , ""      , 0x6D      , TRUE    , TYPE_BIT      , 2     , 0            , 0      , 0      , 0     }, 
  {"ZCHECK_BUMP"                          , ""      , 0x6D      , TRUE    , TYPE_BIT      , 3     , 0            , 0      , 0      , 0     },
  {"ZCHECK_BUMP_FIRST"                    , ""      , 0x6D      , TRUE    , TYPE_BIT      , 4     , 0            , 0      , 0      , 0     }, 
//{"Flag 5"                               , ""      , 0x6D      , TRUE    , TYPE_BIT      , 5     , 0            , 0      , 0      , 0     }, 
//{"Flag 6"                               , ""      , 0x6D      , TRUE    , TYPE_BIT      , 6     , 0            , 0      , 0      , 0     }, 
//{"Flag 7"                               , ""      , 0x6D      , TRUE    , TYPE_BIT      , 7     , 0            , 0      , 0      , 0     }, 
  
  //B2  C2  D2  FT  CR

  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_particlecfg_menu[] = { 
  //name                                 //postfix    //address     //edit_bool  //type      //precision  //*value         //steps    //min    //max  
  {"Particle Type Name"                   , ""         , 0x04      , TRUE    , TYPE_STRING   , 20    , 0            , 0       , 0        , 0     },
  
  {"Render Colouring R"                   , " of 255"  , 0x60      , TRUE    , TYPE_BYTE     , DEC    , 0           , 1       , 0x00     , 0xFF     }, // B
  {"Render Colouring G"                   , " of 255"  , 0x61      , TRUE    , TYPE_BYTE     , DEC    , 0           , 1       , 0x00     , 0xFF     }, // C
  {"Render Colouring B"                   , " of 255"  , 0x62      , TRUE    , TYPE_BYTE     , DEC    , 0           , 1       , 0x00     , 0xFF     }, // D
  {"Initial Color Variation"              , ""         , 0x64      , TRUE    , TYPE_BYTE     , DEC    , 0           , 1       , 0        , 100      }, // CV (for r,g,b only, in %) (0-100);
  
  {"0x18 ?"                               , ""         , 0x18      , TRUE    , TYPE_FLOAT    , 2      , 0           , 1.00f   , -9999.0f , 9999.0f  }, //
  
  {"Default Initial Radius"               , ""         , 0x1C      , TRUE    , TYPE_FLOAT    , 4      , 0           , 0.001f  , -9999.0f , 9999.0f  }, // E LCS
  {"Expansion Rate"                       , ""         , 0x20      , TRUE    , TYPE_FLOAT    , 4      , 0           , 0.001f  , -9999.0f , 9999.0f  }, // F LCS

  ///Color "Fade-to-Black" options:
  {"Initial Intensity (0-255)"            , " of 255" , 0x26      , TRUE    , TYPE_SHORT     , DEC    , 0           , 1       , 0        , 255      }, // G
  {"Fade Time"                            , ""        , 0x24      , TRUE    , TYPE_SHORT     , DEC    , 0           , 1       , 0        , 9999     }, // H
  {"Fade Amount (-255 to 255)"            , ""        , 0x28      , TRUE    , TYPE_SHORT     , DEC    , 0           , 1       , 0        , 9999     }, // I
  
  ///"Fade Alpha" options:
  {"Initial Intensity (alpha)"            , " of 255" , 0x2A      , TRUE    , TYPE_SHORT     , DEC    , 0           , 1       , 0        , 9999     }, // GA
  {"Fade Time (alpha)"                    , ""        , 0x2C      , TRUE    , TYPE_SHORT     , DEC    , 0           , 1       , 0        , 9999     }, // HA
  {"Fade Amount (alpha)"                  , ""        , 0x2E      , TRUE    , TYPE_SHORT     , DEC    , 0           , 1       , 0        , 9999     }, // IA
   
  
  {"Animation Speed"                      , ""        , 0x32      , TRUE    , TYPE_SHORT     , DEC    , 0           , 1       , 0        , 999      }, // J   
  {"Start Animation Frame"                , ""        , 0x30      , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // K
  {"Final Animation Frame"                , ""        , 0x31      , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // L
  {"Rotation Speed (0=None,i-deg/frame)"  , ""        , 0x34      , TRUE    , TYPE_INTEGER   , DEC    , 0           , 1       , 0        , 999      }, // M
  {"Gravitational Acceleration"           , ""        , 0x38      , TRUE    , TYPE_FLOAT     , 4      , 0           , 0.001f  , -4000.0f , 4000.0f  }, // N
  
  {"Drag/Friction Decceleration"          , ""        , 0x3C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1       , 0        , 999      }, // O
  {"Default Life-Span of Particle"        , " msec"   , 0x40      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1       , 0        , 999      }, // P
  {"Position Random Error"                , ""        , 0x44      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f   , -4000.0f , 4000.0f  }, // Q
  {"Velocity Random Error"                , ""        , 0x48      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f   , -4000.0f , 4000.0f  }, // R
  {"Expansion Rate Error"                 , ""        , 0x4C      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f   , -4000.0f , 4000.0f  }, // S
  {"Rotation Rate Error"                  , ""        , 0x50      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1       , INT_MIN  , INT_MAX  }, // T
  {"Life-Span Error Shape"                , ""        , 0x54      , TRUE    , TYPE_BYTE     , DEC    , 0            , 1       , 0x00     , 0xFF     }, // U
  {"Trail Length Multiplier"              , ""        , 0x58      , TRUE    , TYPE_FLOAT    , 1      , 0            , 0.1f    , 0.0f     , 9999.9f  }, // V
  
  {"Stretch value of texture in X"        , ""        , 0x70      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f   , -4000.0f , 4000.0f  }, // WX
  {"Stretch value of texture in Y"        , ""        , 0x74      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f   , -4000.0f , 4000.0f  }, // WY
  {"Wind factor"                          , ""        , 0x78      , TRUE    , TYPE_FLOAT    , 4      , 0            , 0.01f   , -4000.0f , 4000.0f  }, // WI
  

  {"ZCHECK_FIRST"                         , ""        , 0x5C      , TRUE    , TYPE_BIT      , 0     , 0            , 0        , 0        , 0     }, // Z
  {"ZCHECK_STEP"                          , ""        , 0x5C      , TRUE    , TYPE_BIT      , 1     , 0            , 0        , 0        , 0     }, 
  {"DRAW_OPAQUE"                          , ""        , 0x5C      , TRUE    , TYPE_BIT      , 2     , 0            , 0        , 0        , 0     }, 
  {"SCREEN_TRAIL"                         , ""        , 0x5C      , TRUE    , TYPE_BIT      , 3     , 0            , 0        , 0        , 0     },
  {"SPEED_TRAIL"                          , ""        , 0x5C      , TRUE    , TYPE_BIT      , 4     , 0            , 0        , 0        , 0     }, 
  {"RAND_VERT_V"                          , ""        , 0x5C      , TRUE    , TYPE_BIT      , 5     , 0            , 0        , 0        , 0     }, 
  {"CYCLE_ANIM"                           , ""        , 0x5C      , TRUE    , TYPE_BIT      , 6     , 0            , 0        , 0        , 0     }, 
  {"DRAW_DARK"                            , ""        , 0x5C      , TRUE    , TYPE_BIT      , 7     , 0            , 0        , 0        , 0     }, 
  {"VERT_TRAIL"                           , ""        , 0x5D      , TRUE    , TYPE_BIT      , 0     , 0            , 0        , 0        , 0     },
  {"DRAWTOP2D"                            , ""        , 0x5D      , TRUE    , TYPE_BIT      , 1     , 0            , 0        , 0        , 0     }, 
  {"CLIPOUT2D"                            , ""        , 0x5D      , TRUE    , TYPE_BIT      , 2     , 0            , 0        , 0        , 0     }, 
  {"ZCHECK_BUMP"                          , ""        , 0x5D      , TRUE    , TYPE_BIT      , 3     , 0            , 0        , 0        , 0     },
  {"ZCHECK_BUMP_FIRST"                    , ""        , 0x5D      , TRUE    , TYPE_BIT      , 4     , 0            , 0        , 0        , 0     }, 
//{"Flag 5"                               , ""        , 0x5D      , TRUE    , TYPE_BIT      , 5     , 0            , 0        , 0        , 0     }, 
//{"Flag 6"                               , ""        , 0x5D      , TRUE    , TYPE_BIT      , 6     , 0            , 0        , 0        , 0     }, 
//{"Flag 7"                               , ""        , 0x5D      , TRUE    , TYPE_BIT      , 7     , 0            , 0        , 0        , 0     }, 

  {NULL,NULL,0,0,0,0,0,0,0}
};





const Editor_pack lcs_pedstatsdat_menu[] = { 
  //name                  //postfix   //address     //edit_bool  //type        //precision    //*value      //steps    //min    //max  
//{"No"                    , ""      , 0x00      , FALSE   , TYPE_INTEGER  , DEC   , 0            , 1      , INT_MIN  , INT_MAX  },
  {"PedStat Type Name"     , ""      , 0x04      , TRUE    , TYPE_STRING   , 20    , 0            , 0      , 0      , 0      }, // A:  PedStat type name
  
  {"Flee Distance"         , ""      , 0x1C      , TRUE    , TYPE_FLOAT    , 0     , 0            , 1.0f   , 0      , 999.0f }, // B:  Flee distance
  {"Heading change rate "  , " Deg"  , 0x20      , TRUE    , TYPE_FLOAT    , 1     , 0            , 0.1f   , 0      , 999.0f }, // C:  Heading change rate
  
  {"Fear"                  , " %"    , 0x24      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0      , 100    }, // D:  Fear
  {"Temper"                , " %"    , 0x25      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0      , 100    }, // E:  Temper
  {"Lawfullnes"            , " %"    , 0x26      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0      , 100    }, // F:  Lawfullness
  {"Sexiness"              , " %"    , 0x27      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0      , 100    }, // G:  Sexiness
  {"Attack strength"       , ""      , 0x28      , TRUE    , TYPE_FLOAT    , 1     , 0            , 0.1f   , 0      , 900.0f }, // H: Attack strength  
  {"Defend weakness"       , ""      , 0x2C      , TRUE    , TYPE_FLOAT    , 1     , 0            , 0.1f   , 0      , 900.0f }, // I:  Defend weakness
  
  {"PunchOnly"             , ""      , 0x30      , TRUE    , TYPE_BIT      , 0     , 0            , 0      , 0      , 0      }, // J: Flags
  {"CanKneeHead"           , ""      , 0x30      , TRUE    , TYPE_BIT      , 1     , 0            , 0      , 0      , 0      }, 
  {"CanKick"               , ""      , 0x30      , TRUE    , TYPE_BIT      , 2     , 0            , 0      , 0      , 0      }, 
  {"CanRoundHse"           , ""      , 0x30      , TRUE    , TYPE_BIT      , 3     , 0            , 0      , 0      , 0      },
  {"NoDive"                , ""      , 0x30      , TRUE    , TYPE_BIT      , 4     , 0            , 0      , 0      , 0      }, 
  {"1HitKnockdown"         , ""      , 0x30      , TRUE    , TYPE_BIT      , 5     , 0            , 0      , 0      , 0      }, 
  {"ShoppingBags"          , ""      , 0x30      , TRUE    , TYPE_BIT      , 6     , 0            , 0      , 0      , 0      }, 
  {"GunPanic"              , ""      , 0x30      , TRUE    , TYPE_BIT      , 7     , 0            , 0      , 0      , 0      }, 
    
  {NULL,NULL,0,0,0,0,0,0,0}
};


const Editor_pack vcs_pedstatsdat_menu[] = { 
  //name                     //postfix    //address     //edit_bool  //type        //precision  //*value         //steps    //min    //max  
//  {"No"                     , ""      , 0x00      , FALSE   , TYPE_INTEGER  , DEC   , 0            , 1      , INT_MIN , INT_MAX  },
  {"PedStat Type Name"        , ""      , 0x1A      , TRUE    , TYPE_STRING   , 20    , 0            , 0      , 0       , 0     }, // A:  PedStat type name
  
  {"Flee Distance"            , ""      , 0x04      , TRUE    , TYPE_FLOAT    , 0     , 0            , 1.0f   , 0       , 999.0f }, // B:  Flee distance
  {"Heading change rate "     , " Deg"  , 0x08      , TRUE    , TYPE_FLOAT    , 1     , 0            , 0.1f   , 0       , 999.0f }, // C:  Heading change rate
  
  {"Fear"                     , " %"    , 0x16      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0       , 100    }, // D:  Fear
  {"Temper"                   , " %"    , 0x17      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0       , 100    }, // E:  Temper
  {"Lawfullnes"               , " %"    , 0x18      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0       , 100    }, // F:  Lawfullness
  {"Sexiness"                 , " %"    , 0x19      , TRUE    , TYPE_BYTE     , DEC   , 0            , 1      , 0       , 100    }, // G:  Sexiness
  {"Attack strength"          , ""      , 0x0C      , TRUE    , TYPE_FLOAT    , 1     , 0            , 0.1f   , 0       , 900.0f }, // H: Attack strength  
  {"Defend weakness"          , ""      , 0x10      , TRUE    , TYPE_FLOAT    , 1     , 0            , 0.1f   , 0       , 900.0f }, // I:  Defend weakness
  
  {"PunchOnly"                , ""      , 0x14      , TRUE    , TYPE_BIT      , 0     , 0            , 0      , 0       , 0     }, // J: Flags
  {"CanKneeHead"              , ""      , 0x14      , TRUE    , TYPE_BIT      , 1     , 0            , 0      , 0       , 0     }, 
  {"CanKick"                  , ""      , 0x14      , TRUE    , TYPE_BIT      , 2     , 0            , 0      , 0       , 0     }, 
  {"CanRoundHse"              , ""      , 0x14      , TRUE    , TYPE_BIT      , 3     , 0            , 0      , 0       , 0     },
  {"NoDive"                   , ""      , 0x14      , TRUE    , TYPE_BIT      , 4     , 0            , 0      , 0       , 0     }, 
  {"1HitKnockdown"            , ""      , 0x14      , TRUE    , TYPE_BIT      , 5     , 0            , 0      , 0       , 0     }, 
  {"ShoppingBags"             , ""      , 0x14      , TRUE    , TYPE_BIT      , 6     , 0            , 0      , 0       , 0     }, 
  {"GunPanic"                 , ""      , 0x14      , TRUE    , TYPE_BIT      , 7     , 0            , 0      , 0       , 0     }, 
  
  {NULL,NULL,0,0,0,0,0,0,0}
};




const Editor_pack vcs_businessobj_menu[] = { 
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value //steps    //min    //max  
  {"World Coordinate X"      , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000    },
  {"World Coordinate Y"      , ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000    },
  {"World Coordinate Z"      , ""      , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000    },
  
  {"Identifier"              , ""      , 0x58      , FALSE    , TYPE_SHORT   , DEC    , 0            , 0x1    , 0x000    , 0xFFFF  },
  
  {NULL,NULL,0,0,0,0,0,0,0}
};




const Editor_pack lcs_weapondat_menu[] = { 
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value  //steps    //min    //max  
  {"Fire type"              , ""      , 0x00      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 0      , 0      , 0          }, // B: Fire type
  {"Range"                  , ""      , 0x04      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // C: Range  
  {"Firing Rate"            , ""      , 0x08      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // D: Firing Rate
  {"Reload"                 , ""      , 0x0C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // E: Reload
  {"Amount of Ammunition"   , ""      , 0x10      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // F: Amount of Ammunition
  {"Damage"                 , ""      , 0x14      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // G: Damage
  {"Speed"                  , ""      , 0x18      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // H: Speed
  {"Radius"                 , ""      , 0x1C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // I: Radius
  {"Life span"              , ""      , 0x20      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // J: Life span
  {"Spread"                 , ""      , 0x24      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // K: Spread
  
  {"?"                      , ""      , 0x28      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
  {"?"                      , ""      , 0x2C      , TRUE    , TYPE_INTEGER  , HEX    , 0            , 1      , INT_MIN  , INT_MAX  },
 
  {"Fire offset vector"     , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // L: Fire offset vector
  {"Fire offset vector"     , ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // M: Fire offset vector
  {"Fire offset vector"     , ""      , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // N: Fire offset vector

  {"animation to play"      , ""      , 0x40      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // P: animation to play
  
  
  
  {"model id"              , ""      , 0x60      , TRUE    , TYPE_INTEGER   , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // Y: model id
  {"model2"                , ""      , 0x64      , TRUE    , TYPE_INTEGER   , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // Z: model2 id
  
  {"Weapon Slot"           , ""      , 0x68      , TRUE    , TYPE_INTEGER   , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // b: Weapon Slot

  //a: Flags -> Stored in HEX... so (from right to left)
  {"Flag 1"                , ""      , 0x6C      , TRUE    , TYPE_NIBBLE_LOW  , HEX   , 0            , 0x1    , 0x0    , 0xF    }, // 1:USE_GRAVITY    2:SLOWS_DOWN    4:DISSIPATES    8:RAND_SPEED  
  {"Flag 2"                , ""      , 0x6C      , TRUE    , TYPE_NIBBLE_HIGH , HEX   , 0            , 0x1    , 0x0    , 0xF    }, // 1:EXPANDS      2:EXPLODES    4:CANAIM    8:CANAIM_WITHARM
  {"Flag 3"                , ""      , 0x6D      , TRUE    , TYPE_NIBBLE_LOW  , HEX   , 0            , 0x1    , 0x0    , 0xF    }, // 1:1ST_PERSON    2:HEAVY      4:THROW      8:RELOAD_LOOP2START
  {"Flag 4"                , ""      , 0x6D      , TRUE    , TYPE_NIBBLE_HIGH , HEX   , 0            , 0x1    , 0x0    , 0xF    }, // 1:USE_2ND    2:GROUND_2ND  4:FINISH_3RD  8:RELOAD
  {"Flag 5"                , ""      , 0x6E      , TRUE    , TYPE_NIBBLE_LOW  , HEX   , 0            , 0x1    , 0x0    , 0xF    }, // 1:FIGHTMODE    2:CROUCHFIRE  4:COP3_RD    8:GROUND_3RD
  {"Flag 6"                , ""      , 0x6E      , TRUE    , TYPE_NIBBLE_HIGH , HEX   , 0            , 0x1    , 0x0    , 0xF    }, // 1:PARTIALATTACK 2:ANIMDETONATE
  {"Flag 7 (unused)"       , ""      , 0x6F      , TRUE    , TYPE_NIBBLE_LOW  , HEX   , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 8 (unused)"       , ""      , 0x6F      , TRUE    , TYPE_NIBBLE_HIGH , HEX   , 0            , 0x1    , 0x0    , 0xF    }, 
 
  {NULL,NULL,0,0,0,0,0,0,0}
};

const Editor_pack vcs_weapondat_menu[] = { 
  //name                  //postfix    //address     //edit_bool  //type        //precision  //*value         //steps    //min    //max  
  {"Fire type"             , ""      , 0x04      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 0      , 0        , 0        }, // B: Fire type
  {"Range"                 , ""      , 0x08      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // C: Range  
  {"Firing Rate"           , ""      , 0x0C      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // D: Firing Rate  
  {"Reload"                , ""      , 0x10      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // E: Reload
  {"Amount of Ammunition"  , ""      , 0x14      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // F: Amount of Ammunition
  {"Damage"                , ""      , 0x18      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // G: Damage
  {"Speed"                 , ""      , 0x1C      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // H: Speed
  {"Radius"                , ""      , 0x20      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // I: Radius
  {"Life span"             , ""      , 0x24      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // J: Life span
  {"Spread"                , ""      , 0x28      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // K: Spread
  
  {"Fire offset vector"    , ""      , 0x30      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // L: Fire offset vector
  {"Fire offset vector"    , ""      , 0x34      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // M: Fire offset vector
  {"Fire offset vector"    , ""      , 0x38      , TRUE    , TYPE_FLOAT    , 2      , 0            , 0.1    , -4000    , 4000     }, // N: Fire offset vector

  {"animation to play"     , ""      , 0x40      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // P: animation to play
  
  {"model id"              , ""      , 0x60      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // Y: model id
  {"model2"                , ""      , 0x64      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // Z: model2 id
  
  {"Weapon Slot"           , ""      , 0x68      , TRUE    , TYPE_INTEGER  , DEC    , 0            , 1      , INT_MIN  , INT_MAX  }, // b: Weapon Slot

  ///a: Flags -> Stored in HEX... so (from right to left)
  {"Flag 1"                , ""      , 0x00      , TRUE    , TYPE_NIBBLE_LOW  , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 2"                , ""      , 0x00      , TRUE    , TYPE_NIBBLE_HIGH , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 3"                , ""      , 0x01      , TRUE    , TYPE_NIBBLE_LOW  , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 4"                , ""      , 0x01      , TRUE    , TYPE_NIBBLE_HIGH , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 5"                , ""      , 0x02      , TRUE    , TYPE_NIBBLE_LOW  , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 6"                , ""      , 0x02      , TRUE    , TYPE_NIBBLE_HIGH , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 7 (unused)"       , ""      , 0x03      , TRUE    , TYPE_NIBBLE_LOW  , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 
  {"Flag 8 (unused)"       , ""      , 0x03      , TRUE    , TYPE_NIBBLE_HIGH , HEX    , 0            , 0x1    , 0x0    , 0xF    }, 

  {NULL,NULL,0,0,0,0,0,0,0}
};



//     Amb        Amb_Obj    Amb_bl    Amb_Obj_bl  Dir        Sky top    Sky bot    SunCore    SunCorona  SunSz  SprSz  SprBght    Shdw  LightShd  PoleShd    FarClip    FogSt  LightOnGround  LowCloudsRGB  TopCloudRGB    BottomCloudRGB  BlurRGB      WaterRGBA
//    178  183  255    95 95 95    20  20 20    63 63  51  92 92 97    0 0 5    16 16 29  255 255 0  5  0  0  1.0    1.3    1.0      239    99      0      900.0    100.0  1.5        30  20  0    23  32  33    3 3 3      18  26  23    80 92 99 80

// LC   Amb       Amb_Obj    Amb_bl    Amb_Obj_bl  Dir        Sky top    Sky bot    SunCore    SunCorona  SunSz  SprSz  SprBght    Shdw  LightShd  PoleShdw  FarClp    FogSt  LightOnGround  LowCloudsRGB  TopCloudRGB    BottomCloudRGB  BlurRGB      WaterRGBA
// GTA3 AMB        XXXX      XXXX      XXXX        DIR        SKY TOP    SKY BOT    SUNCORE   SUNCORONA  SUNSZ    SPRSZ  SPRBGHT   SHDW  LIGHTSHD  TREESHD    FARCLP    FOGST  LIGHTONGROUND  LOWCLOUDSRGB  TOPCLOUDRGB    BOTTOMCLOUDRGB  XXXX      WATERRGBA
//    55 55 45     95 95 95   55 55 45  55 55 45  255 255 255    0 0 5    05 05 05   255 255 0  5 0 0    1.0    1.0    1.0      200    100      0      2000.0    100.0  1.0        30 20 0      23 32 33    3 3 3      056  038  000  85  85  65  192

const Editor_pack timecycdat_menu[] = { 
  //name                  //postfix //address   //edit_bool  //type        //precision  //*value //steps    //min    //max  
  {"Amb R"                , ""      ,  0x180     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Amb
  {"Amb G"                , ""      ,   0xC0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Amb B"                , ""      ,   0x00     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {"Amb_Obj R"            , ""      ,  0x240     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Amb_Obj
  {"Amb_Obj G"            , ""      ,  0x300     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Amb_Obj B"            , ""      ,  0x3C0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {"Amb_bl R"             , ""      ,  0x480     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Amb_bl
  {"Amb_bl G"             , ""      ,  0x540     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Amb_bl B"             , ""      ,  0x600     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {"Amb_Obj_bl R"         , ""      ,  0x6C0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Amb_Obj_bl
  {"Amb_Obj_bl G"         , ""      ,  0x780     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Amb_Obj_bl B"         , ""      ,  0x840     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
    
  {"Dir R"                , ""      ,  0x900     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Dir
  {"Dir G"                , ""      ,  0x9C0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Dir B"                , ""      ,  0xA80     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
      
  {"Sky top R"            , ""      ,  0xB40     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Sky top 
  {"Sky top G"            , ""      ,  0xC00     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Sky top B"            , ""      ,  0xCC0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
        
  {"Sky bot R"            , ""      ,  0xD80     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Sky bot 
  {"Sky bot G"            , ""      ,  0xE40     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Sky bot B"            , ""      ,  0xF00     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
          
  {"SunCore R"            , ""      ,  0xFC0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // SunCore
  {"SunCore G"            , ""      , 0x1080     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"SunCore B"            , ""      , 0x1140     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {"SunCorona R"          , ""      , 0x1200     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // SunCorona
  {"SunCorona G"          , ""      , 0x12C0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"SunCorona B"          , ""      , 0x1380     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, //
 
  {"SunSz"                , ""      , 0x1440     , TRUE    , TYPE_BYTE_AS_FLT , 1     , 0           , 1       , 0x00     , 0xFF     }, // SunSz
  {"SprSz"                , ""      , 0x1500     , TRUE    , TYPE_BYTE_AS_FLT , 1    , 0           , 1       , 0x00     , 0xFF     }, // SprSz
  {"SprBght"              , ""      , 0x15C0     , TRUE    , TYPE_BYTE_AS_FLT , 1    , 0           , 1       , 0x00     , 0xFF     }, // SprBght
  
  {"Shdw"                 , ""      , 0x1680     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // Shdw
  {"LightShd"             , ""      , 0x1740     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // LightShd
  {"PoleShd"              , ""      , 0x1800     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // PoleShd
/*
  {"FarClip"              , ""      , 0x18C0     , TRUE    , TYPE_SHORT     , DEC    , 0           , 0x1    , 0x000    , 0xFFFF    }, // FarClip
  {""                     , ""      , 0x1980     , TRUE    , TYPE_SHORT     , DEC    , 0           , 0x1    , 0x000    , 0xFFFF    }, // 

  {"FogSt"                , ""      , 0x1A40     , TRUE    , TYPE_SHORT     , DEC    , 0           , 0x1    , 0x000    , 0xFFFF    }, // FogSt
  {""                     , ""      , 0x1B00     , TRUE    , TYPE_SHORT     , DEC    , 0           , 0x1    , 0x000    , 0xFFFF    }, // 
*/
  {"LightOnGround"        , ""      , 0x1BC0     , TRUE    , TYPE_BYTE_AS_FLT , 1    , 0           , 1       , 0x00     , 0xFF     }, // LightOnGround
 
  {"LowClouds R"          , ""      , 0x1C80     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // LowCloudsRGB
  {"LowClouds G"          , ""      , 0x1D40     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"LowClouds B"          , ""      , 0x1E00     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {"TopCloud R"           , ""      , 0x1EC0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // TopCloudRGB
  {"TopCloud G"           , ""      , 0x1F80     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"TopCloud B"           , ""      , 0x2040     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {"BottomCloud R"        , ""      , 0x2100     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // BottomCloudRGB
  {"BottomCloud G"        , ""      , 0x21C0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"BottomCloud B"        , ""      , 0x2280     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
 
  {"Blur R"               , ""      , 0x2340     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // BlurRGB
  {"Blur G"               , ""      , 0x2400     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Blur B"               , ""      , 0x24C0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {"Water R"              , ""      , 0x2580     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // WaterRGBA
  {"Water G"              , ""      , 0x2640     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Water B"              , ""      , 0x2700     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  {"Water A"              , ""      , 0x27C0     , TRUE    , TYPE_BYTE      , DEC    , 0           , 1       , 0x00     , 0xFF     }, // 
  
  {NULL,NULL,0,0,0,0,0,0,0}
};

/* by DenielX
AmbientBlue V
AmbientGreen V
AmbientRed V

Is not Ambient_Obj in principle. This is the color of particle effects that are not registered in particle.cfg.
For example, the color of some splashes from a boat (PSP&PS2), a splash from the sinking of land transport (PSP&PS2),
a trace of tire braking on soils such as grass and sand (PS2).Conditionally named Particle Color
AmbientRed_Obj ? 
AmbientGreen_Obj ?
AmbientBlue_Obj ?

AmbientRed_Bl V
AmbientGreen_Bl V
AmbientBlue_Bl V
AmbientRed_Obj_Bl - in fact it is AmbientRed_Obj V
AmbientGreen_Obj_Bl - in fact it is AmbientGreen_Obj V
AmbientBlue_Obj_Bl - in fact it is AmbientBlue_Obj V
DirectionalRed V
DirectionalGreen V
DirectionalBlue V
SkyTopRed V
SkyTopGreen V
SkyTopBlue V
SkyBottomRed V
SkyBottomGreen V
SkyBottomBlue V
SunCoreRed V
SunCoreGreen V
SunCoreBlue V
SunCoronaRed V
SunCoronaGreen V
SunCoronaBlue V
SunSize V
SpriteSize V
SpriteBrightness V
ShadowStrength V
LightShadowStrength V
PoleShadowStrength ? The engine reads the data and uses it in calculations. But probably because of the disabled shadows of static objects - zero result.
FarClip V
FogStart V
LightsOnGroundBrightness V Light from traffic lights on the ground.
LowCloudsRed V
LowCloudsGreen V
LowCloudsBlue V
CloudsTopRed V
CloudsTopGreen V
CloudsTopBlue V
CloudsBottomRed V
CloudsBottomGreen V
CloudsBottomBlue V
BlurBlue V
BlurGreen V
BlurRed V
WaterRed V  
WaterGreen V
WaterBlue V
WaterAlpha V


short FarClip[192];//interpreted as float
short FogStart[192];//interpreted as float

float SunSize = char SunSize / 10;
float SpriteSize= char SpriteSize /10;
float SpriteBrightness = char SpriteBrightness /10;
float LightsOnGroundBrightness = char LightsOnGroundBrightness / 10;
*/



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int editor_garage_current;
extern int editor_garageslot_current; // not used here
void *editor_garage(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", garage_cur, var_garageslots );
    return (void *)buffer;

  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_garage_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_garage_current = value;
    return 0;
  }
  
  editor_create(EDITOR_GARAGE, 2, "Garage", (LCS ? lcs_garage_menu : vcs_garage_menu), global_garagedata, var_garageslotsize, var_garageslots);
  return NULL;
}

extern int editor_pedobj_current;
void *editor_pedobj(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", peds_cur, peds_max);
    return (void *)buffer;
    
  } else if( calltype == FUNC_GET_VALUE ) { 
    return (int*)editor_pedobj_current;
    
  } else if( calltype == FUNC_SET ) {
    editor_pedobj_current = value;
    return 0;
  }
  editor_create(EDITOR_PEDOBJ, 1, "People Objects", (LCS ? lcs_pedobj_menu : vcs_pedobj_menu), peds_base, var_pedobjsize, peds_max);
  return NULL;
}

extern int editor_vehicleobj_current;
void *editor_vehicleobj(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", vehicles_cur, vehicles_max);
    return (void *)buffer;
  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_vehicleobj_current;
    
  } else if( calltype == FUNC_SET ) {
    editor_vehicleobj_current = value;
    return 0;
  }
  editor_create(EDITOR_VEHICLEOBJ, 1, "Vehicle Objects", (LCS ? lcs_vehicleobj_menu : vcs_vehicleobj_menu), vehicles_base, var_vehobjsize, vehicles_max);
  return NULL;
}

extern int editor_worldobj_current;
void *editor_worldobj(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", worldobjs_cur, worldobjs_max);
    return (void *)buffer;
    
  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_worldobj_current;
    
  } else if( calltype == FUNC_SET ) {
    editor_worldobj_current = value;
    return 0;
  }
  editor_create(EDITOR_WORLDOBJ, 1, "World Objects", (LCS ? lcs_worldobj_menu : vcs_worldobj_menu), worldobjs_base, var_wldobjsize, worldobjs_max);
  return NULL;
}

extern int editor_businessobj_current;
void *editor_businessobj(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", businessobjs_cur, businessobjs_max);
    return (void *)buffer;
    
  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_businessobj_current;
    
  } else if( calltype == FUNC_SET ) {
    editor_businessobj_current = value;
    return 0;
  }
  editor_create(EDITOR_BUSINESSOBJ, 1, "Business Objects", vcs_businessobj_menu, businessobjs_base, var_bsnobjsize, businessobjs_max);
  return NULL;
}

extern int editor_pickup_current;
void *editor_pickups(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", pickups_cur, var_pickupslots);
    return (void *)buffer;
  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_pickup_current;
  
  } else if( calltype == FUNC_SET) {
    editor_pickup_current = value;
    return 0;
  }
  editor_create(EDITOR_PICKUPS, 1, "Pickups", (LCS ? lcs_pickups_menu : vcs_pickups_menu), global_pickups, var_pickupslotsize, var_pickupslots);
  return NULL;
}

extern int editor_mapicon_current;
void *editor_mapicons(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", mapicons_cur, var_radarblipslots);
    return (void *)buffer;
    
  } else if(calltype == FUNC_GET_VALUE ) {
    return (int*)editor_mapicon_current;
  
  } else if(calltype == FUNC_SET) {
    editor_mapicon_current = value;
    return 0;
  }
  editor_create(EDITOR_MAPICONS, 1, "Mapicons", (LCS ? lcs_mapicons_menu : vcs_mapicons_menu), (LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding)), var_radarblipslotsize, var_radarblipslots);
  return NULL;
}

extern int editor_vehiclespawn_current;
void *editor_vehspawns(int calltype, int value) {
  static char buffer[16];
  if( calltype == FUNC_GET_STRING ) { 
    sprintf(buffer, " (%i/%i)", vehspawns_cur, var_vehiclesworldspawnslots);
    return (void *)buffer;

  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_vehiclespawn_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_vehiclespawn_current = value;
    return 0;
  }
  editor_create(EDITOR_VEHWORLDSPAWNS, 1, "Parked Vehicle Spawns", (LCS ? lcs_vehiclespawns_menu : vcs_vehiclespawns_menu), addr_vehiclesworldspawn, var_vehiclesworldspawnslotsize, var_vehiclesworldspawnslots);
  return NULL;
}


///////////      ////////////      ////////////      ////////////      ////////////      ////////////      ////////////      ////////////      ////////////      ////////////

extern int editor_ide_current;
void *editor_ide(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
    
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_ide_current;
    
  } else if( calltype == FUNC_SET ) {
    editor_ide_current = value;
    return 0;
  }
  
  editor_create(EDITOR_IDE, 1, "IDEs", (LCS ? lcs_ide_menu : vcs_ide_menu), getInt(ptr_IDETable+(LCS?0x0:gp)), 0x4, getInt(ptr_IDEs+(LCS?0x0:gp))); // this is special as its a pointer table thus 0x4 for one pointer
  return NULL;
}

extern int editor_buildingsIPL_current;
void *editor_buildingsipl(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
  
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_buildingsIPL_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_buildingsIPL_current = value;
    return 0;
  }
  
  editor_create(EDITOR_BUILDINGSIPL, 1, "Buildings.ipl", lcs_ipl_menu , buildingsIPL_base, var_buildingsIPLslotsize, buildingsIPL_max); // LCS: 6315 slots (0xCC9A00)
  return NULL;
}

extern int editor_treadablesIPL_current;
void *editor_treadablesipl(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
  
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_treadablesIPL_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_treadablesIPL_current = value;
    return 0;
  }
  editor_create(EDITOR_TREADABLESIPL, 1, "Treadables.ipl", lcs_ipl_menu , treadablesIPL_base, var_treadablesIPLslotsize, treadablesIPL_max); // LCS: 1219 slots
  return NULL;
}

extern int editor_dummysIPL_current;
void *editor_dummysipl(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
    
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_dummysIPL_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_dummysIPL_current = value;
    return 0;
  }
  editor_create(EDITOR_DUMMYSIPL, 1, "Dummys.ipl", lcs_ipl_menu , dummysIPL_base, var_dummysIPLslotsize, dummysIPL_max); // LCS: 2575 slots
  return NULL;
}

extern int editor_carcolsDAT_current;
void *editor_carcolsdat(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
  
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_carcolsDAT_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_carcolsDAT_current = value;
    return 0;
  }
  editor_create(EDITOR_CARCOLSDAT, 1, "Carcols.dat", LCS ? lcs_colsdat_menu : vcs_colsdat_menu, carcolsDAT_base, var_carcolsdatslotsize, var_carcolsdatslots);
  return NULL;
}

extern int editor_pedcolsDAT_current;
void *editor_pedcolsdat(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
    
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_pedcolsDAT_current;
  
  } else if( calltype == FUNC_SET) {
    editor_pedcolsDAT_current = value;
    return 0;
  }
  editor_create(EDITOR_PEDCOLSDAT, 1, "Pedcols.dat", LCS ? lcs_colsdat_menu : vcs_colsdat_menu, pedcolsDAT_base, var_carcolsdatslotsize, var_carcolsdatslots);
  return NULL;
}

extern int editor_vehicle_current;
void *editor_handlingcfg(int calltype, int value) { // for VCS there is no link in dtz to handling anymore! ptr to slot is in IDE + 0x38
  if( calltype == FUNC_GET_STRING ) 
    return "";
    
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_vehicle_current;
  
  } else if( calltype == FUNC_SET) {
    editor_vehicle_current = value;
    return 0;
  }
  editor_create(EDITOR_HANDLINGCFG, 1, "Handling.cfg", (LCS ? lcs_handlingcfg_menu : vcs_handlingcfg_menu), getInt(ptr_IDETable + (LCS ? 0 : gp)), 0x4, getInt(ptr_IDEs + (LCS ? 0 : gp)) ); // this is special because we need to loop through IDE table 
  return NULL;
}

extern int editor_particleCFG_current;
void *editor_particlecfg(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
    
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_particleCFG_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_particleCFG_current = value;
    return 0;
  }
  
  editor_create(EDITOR_PARTICLECFG, 1, "Particle.cfg", (LCS ? lcs_particlecfg_menu : vcs_particlecfg_menu), getInt(ptr_particleCFG+(LCS?0x0:gp)), var_particleCFGslotsize, var_particleCFGslots);
  return NULL;
}

extern int editor_pedstatsDAT_current;
void *editor_pedstatsdat(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) 
    return "";
    
  else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_pedstatsDAT_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_pedstatsDAT_current = value;
    return 0;
  }
  editor_create(EDITOR_PEDSTATSDAT, 1, "Pedstats.dat", (LCS ? lcs_pedstatsdat_menu : vcs_pedstatsdat_menu), getInt(getInt(ptr_pedstatTable+(LCS?0x0:gp))), var_pedstatDATslotsize, var_pedstatDATslots);
  return NULL;
}

extern int editor_weaponDAT_current;
void *editor_weapondat(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) {
    return "";
  
  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_weaponDAT_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_weaponDAT_current = value;
    return 0;
  }
  editor_create(EDITOR_WEAPONDAT, 1, "Weapon.dat", (LCS ? lcs_weapondat_menu : vcs_weapondat_menu), getInt(getInt(ptr_weaponTable+(LCS?0x0:gp)) + (multiplayer ? 0 : 4)), var_weaponDATslotsize, var_weaponDATslots);
  return NULL;
}

extern int editor_timecycDAT_current;
void *editor_timecycdat(int calltype, int value) {
  if( calltype == FUNC_GET_STRING ) {
    return "";
  
  } else if( calltype == FUNC_GET_VALUE ) {
    return (int*)editor_timecycDAT_current;
  
  } else if( calltype == FUNC_SET ) {
    editor_timecycDAT_current = value;
    return 0;
  }
  editor_create(EDITOR_TIMECYCDAT, 1, "Timecyc.dat", timecycdat_menu, getInt(ptr_timecycDAT+(LCS?0x0:gp)) + 0x280, -1, 8 * 24);
  return NULL;
}
#endif
