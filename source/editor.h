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

#ifndef __EDITOR_H__
#define __EDITOR_H__


enum {
  HEX, // 0
  DEC  // 1
};

enum {
  EDITOR_SAVEDITOR_SIMP,
  EDITOR_SAVEDITOR_SRPT,
  EDITOR_SAVEDITOR_GRGE,
  EDITOR_SAVEDITOR_PLYR,
  EDITOR_SAVEDITOR_STAT,
  EDITOR_SAVEDITOR_OVER,
  EDITOR_GARAGE,
  EDITOR_PEDOBJ,
  EDITOR_VEHICLEOBJ,
  EDITOR_WORLDOBJ,
  EDITOR_BUSINESSOBJ,
  EDITOR_PICKUPS,
  EDITOR_MAPICONS,
  EDITOR_STATS,
  EDITOR_HANDLINGCFG,
  EDITOR_VEHWORLDSPAWNS,
  EDITOR_BUILDINGSIPL,
  EDITOR_TREADABLESIPL,
  EDITOR_DUMMYSIPL,
  EDITOR_CARCOLSDAT,
  EDITOR_PEDCOLSDAT,
  EDITOR_PEDSTATSDAT,
  EDITOR_PARTICLECFG,
  EDITOR_WEAPONDAT,
  EDITOR_TIMECYCDAT,
  EDITOR_IDE
};

typedef struct {
  char *name;
  char *postfix;
  short address;
  char edit_bool; // can be edited in editor (if false it will be greyed out, only for info)
  char type;
  char precision; // float accuracy after dot / bitflag position
  void *value;
  float steps;
  //int min;
  //int max;
} Editor_pack;


extern const Editor_pack lcs_ide_menu[];
extern const Editor_pack lcs_ide_obj_menu[];
extern const Editor_pack lcs_ide_tobj_menu[];
extern const Editor_pack lcs_ide_weap_menu[];
extern const Editor_pack lcs_ide_hier_menu[];
extern const Editor_pack lcs_ide_cars_menu[];
extern const Editor_pack lcs_ide_ped_menu[];
extern const Editor_pack vcs_ide_menu[];
extern const Editor_pack vcs_ide_obj_menu[];
extern const Editor_pack vcs_ide_tobj_menu[];
extern const Editor_pack vcs_ide_weap_menu[];
extern const Editor_pack vcs_ide_hier_menu[];
extern const Editor_pack vcs_ide_cars_menu[];
extern const Editor_pack vcs_ide_ped_menu[];


void *editor_garage(int calltype, int value);
void *editor_stats(int calltype, int value);
void *editor_vehicolors(int calltype, int value);
void *editor_pedobj(int calltype, int value);
void *editor_vehicleobj(int calltype, int value);
void *editor_worldobj(int calltype, int value);
void *editor_businessobj(int calltype, int value);
void *editor_pickups(int calltype, int value);
void *editor_mapicons(int calltype, int value);
void *editor_vehspawns(int calltype, int value);

void *editor_ide(int calltype, int value);

void *editor_buildingsipl(int calltype, int value);
void *editor_treadablesipl(int calltype, int value);
void *editor_dummysipl(int calltype, int value);
void *editor_carcolsdat(int calltype, int value);
void *editor_pedcolsdat(int calltype, int value);
void *editor_handlingcfg(int calltype, int value);
void *editor_particlecfg(int calltype, int value);
void *editor_pedstatsdat(int calltype, int value);
void *editor_weapondat(int calltype, int value);
void *editor_timecycdat(int calltype, int value);


/// editor helper functions
void *garage_vehicle(int calltype, int keypress, int base_address, int address);
void *radiostation(int calltype, int keypress, int base_address, int address);
void *mapicon_name(int calltype, int keypress, int base_address, int address);
void *mapicon_color(int calltype, int keypress, int base_address, int address);
void *mapicon_type(int calltype, int keypress, int base_address, int address);
void *mapicon_slot(int calltype, int keypress, int base_address, int address);
void *vehicle_enginetype(int calltype, int keypress, int base_address, int address);
void *vehicle_lights_front(int calltype, int keypress, int base_address, int address);
void *vehicle_lights_rear(int calltype, int keypress, int base_address, int address);
void *vehicle_transtype(int calltype, int keypress, int base_address, int address);
void *handling_flag(int calltype, int keypress, int base_address, int address, int arg);
void *model_flag(int calltype, int keypress, int base_address, int address, int arg);

#endif
