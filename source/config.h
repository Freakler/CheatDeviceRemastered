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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "main.h"
#include "cheats.h"

int save_config(const Menu_pack *menu_list, int menu_max);
int load_config(const Menu_pack *menu_list, int menu_max);
int create_config(const Menu_pack *menu_list, int menu_max);

int load_config_block(char *magic, int address);

int writeBool(SceUID file, short id, char boolean);
int writeValue(SceUID file, short id, int value);
int writeCategoryHeader(SceUID file, char *magic);

int workBlock(SceUID file, const Menu_pack *menu_list, int menu_max, int identifier, int mode);
int fillBlock(SceUID file, int size, char placeholder);
void setHeader(SceUID file, int items, int size, int type);

int workAchievement(SceUID file, achievement_pack *achlist, int menu_max);

int getValueFromConfigFor(SceUID file, char * magic, short id);
int getAchievementFromConfigFor(SceUID file, char * magic, char id, int mode);
  
int setValueInConfigFor(short id, int value);

int writeMemory(SceUID file, int start, int slots, int size);

#endif