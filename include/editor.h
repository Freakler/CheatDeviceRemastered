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

#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "mctypes.h"

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
  EDITOR_EMPIRE,
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
  const u8string name;
  const u8string postfix;
  const uaddr addr_offset;
  const bool edit_bool; // can be edited in editor (if false it will be greyed out, only for info)
  const uint8 type;
  const uint8 precision; // float accuracy after dot / bitflag position
  void *value;
  const float step;
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


extern void *editor_garage(int calltype, int value);
extern void *editor_stats(int calltype, int value);
extern void *editor_vehicolors(int calltype, int value);
extern void *editor_pedobj(int calltype, int value);
extern void *editor_vehicleobj(int calltype, int value);
extern void *editor_worldobj(int calltype, int value);
extern void *editor_businessobj(int calltype, int value);
extern void *editor_pickups(int calltype, int value);
extern void *editor_mapicons(int calltype, int value);
extern void *editor_vehspawns(int calltype, int value);
extern void *editor_empire(int calltype, int value);

extern void *editor_ide(int calltype, int value);

extern void *editor_buildingsipl(int calltype, int value);
extern void *editor_treadablesipl(int calltype, int value);
extern void *editor_dummysipl(int calltype, int value);
extern void *editor_carcolsdat(int calltype, int value);
extern void *editor_pedcolsdat(int calltype, int value);
extern void *editor_handlingcfg(int calltype, int value);
extern void *editor_particlecfg(int calltype, int value);
extern void *editor_pedstatsdat(int calltype, int value);
extern void *editor_weapondat(int calltype, int value);
extern void *editor_timecycdat(int calltype, int value);


/// editor helper functions
extern void *garage_vehicle(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *radiostation(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *mapicon_name(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *mapicon_color(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *mapicon_type(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *mapicon_slot(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *vehicle_enginetype(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *vehicle_lights_front(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *vehicle_lights_rear(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *vehicle_transtype(int calltype, int keypress, uaddr base_address, uaddr address);
extern void *handling_flag(int calltype, int keypress, uaddr base_address, uaddr address, int arg);
extern void *model_flag(int calltype, int keypress, uaddr base_address, uaddr address, int arg);

extern void *empire_owner(int calltype, int keypress, uaddr base_address, uaddr address, int steps, int block);
extern void *empire_type(int calltype, int keypress, uaddr base_address, uaddr address, int steps, int block);
extern void *empire_state(int calltype, int keypress, uaddr base_address, uaddr address, int steps, int block);
extern void *empire_scale(int calltype, int keypress, uaddr base_address, uaddr address, int steps, int block);
extern void *empire_condition(int calltype, int keypress, uaddr base_address, uaddr address, int steps, int block);
extern void *empire_damage(int calltype, int keypress, uaddr base_address, uaddr address, int steps, int block);
#endif
