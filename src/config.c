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

#include <stdio.h>
#include <string.h>
#include <pspkernel.h>
#include <sys/unistd.h>

#include "main.h"
#include "cheats.h"
#include "putils.h"
#include "functions.h"
#include "config.h"
#include "logs.h"

#ifdef CONFIG
extern int gp_; // a thread has its own gp register obviously
extern int gametimer;

extern char config[128];
extern const CDRMenuItem main_menu[];

extern const array_size menu_size;
extern bool LCS, VCS;
extern bool flag_coll_cats;
extern bool category_index[];

//extern bool flag_use_advaconfig;
extern uaddr hex_adr;

/*extern uaddr addr_vehiclesworldspawn;
extern usize var_vehiclesworldspawnslots;
extern usize var_vehiclesworldspawnslotsize;

extern uaddr carcolsDAT_base;
extern uaddr pedcolsDAT_base;
extern usize var_carcolsdatslotsize;
extern usize var_carcolsdatslots;

extern uaddr ptr_handlingCFG;
extern usize var_handlingcfgslots;
extern usize var_handlingcfgslotsize;

extern uaddr ptr_particleCFG;
extern usize var_particleCFGslots;
extern usize var_particleCFGslotsize;

extern uaddr ptr_pedstatTable;
extern usize var_pedstatDATslots;
extern usize var_pedstatDATslotsize;

extern uaddr ptr_weaponTable;
extern usize var_weaponDATslots;
extern usize var_weaponDATslotsize;*/


/******************************************************************

Cheat     0x1xxx    xCHT
Category  0x2xxx    xCAT
Setting   0x3xxx    SETT

Custom VehSpawns  ?
Custom Pickups    ?
Handling data    ?

HexLocations?

-----------------------------------

The Header
4 Byte -> Category Magic (LCAT == LibertyCity Category)
4 Byte -> saved value size (1 = char boolean, 4 = integer values, 0 = memory)
4 Byte -> entries (if memory then bytes)
4 Byte -> block-length

4C 43 41 54 AA AA AA AA BB BB BB BB CC CC CC CC    LCATªªªª»»»»����

SETT (global)
LCAT
VCAT
LCHT
VCHT
...

******************************************************************/


bool saveing = false; // my mutex flag thing

static int save_thread(SceSize args UNUSED, void *argp UNUSED) {
  DEBUG_LOG("%lu: save_thread()", getGametime());

  /// this is not as its supposed to have worked
  const CDRMenuItem *menu_list = main_menu;
  usize menu_max = menu_size;

  /// the easy way
  if( !doesFileExist(config) ) { // no config file yet
    int ret = create_config(menu_list, menu_max); // fully generate it (SETT + Lxxx OR Vxxx parts only this way!)
    DEBUG_LOG("save_thread done easy way");
    saveing = false;
    sceKernelExitDeleteThread(0);
    return ret;
  }

  char tempfig[256];

  /// there is a config file! -> rename it to copy from later
  snprintf(tempfig, sizeof(tempfig), "%s_", config);
  sceIoRename(config, tempfig);
  sceIoRemove(config); // to be sure?!

  /// generate a new one
  create_config(menu_list, menu_max); // for current game (LCS or VCS)

  /// copy the other category parts from the backup and append to the new file (IF there are any!) (if Liberty City: append VCAT, VCHT etc..)
  SceUID f1 = sceIoOpen(config, PSP_O_WRONLY | PSP_O_APPEND, 0777);
  SceUID f2 = sceIoOpen(tempfig, PSP_O_RDONLY, 0777);
  if( f1 < 0 ) {
    ERROR_LOG("sceIoOpen f1 error (0x%08X)", f1);
    goto save_error_exit;
  }
  if( f2 < 0 ) {
    ERROR_LOG("sceIoOpen f2 error (0x%08X)", f2);
    goto save_error_exit;
  }

  /////////////////////////////////////////////////////////////////////////////

  #define BLOCKBUFF 256 // adjust if needed!
  char tempbuffer[BLOCKBUFF+0x10];

  char magicbuf[8];
  int read, entries = 0, type = 0, blocksize = 0;

  while( true ) {
    memset(magicbuf, 0, sizeof(magicbuf));
    read = sceIoRead(f2, &magicbuf, sizeof(int));
    //DEBUG_LOG("read %d", read);
    if( read <= 0 )
      break; // eof

    sceIoRead(f2, &type, sizeof(int));
    sceIoRead(f2, &entries, sizeof(int));
    sceIoRead(f2, &blocksize, sizeof(int));

    //DEBUG_LOG("magicbuf %s type %d entries %d blocksize 0x%08X", magicbuf, type, entries, blocksize);

    /// error checks
    if( blocksize < 0 || (type != 0 && blocksize > BLOCKBUFF) ) { // BLOCKSIZE is copy buffer size
      closeMenu();
      setTimedTextbox("Error: bad blocksize!?", 7.00f);
      break; // error - datablock too big
    }
    if( type < 0 || type > 4 ) { // shouldn't happen -- maybe broken file
      closeMenu();
      setTimedTextbox("Error: bad type!?", 7.00f);
      break; // error - type unknown
    }

    if( STR_STARTSWITH_CHAR(magicbuf, (LCS ? 'V' : 'L')) ) { // select everything specific to THE OTHER stories game
      //DEBUG_LOG("do stuff with %s", magicbuf);

      if( type == 0 ) { // pure memory block
        //DEBUG_LOG("its a memory block! 0x%08X bytes (without fillers)", entries);
        //copy the block
        sceIoLseek(f2, -0x10, SEEK_CUR); // include header
        sceIoLseek(f1, 0, SEEK_END);

        int bytesread = 0;
        while( bytesread < blocksize+0x10) { // + header
          bytesread += sceIoRead(f2, tempbuffer, sizeof(BLOCKBUFF));
          sceIoWrite(f1, tempbuffer, sizeof(BLOCKBUFF));
        }

      } else {
        /// copy the block
        sceIoLseek(f2, -0x10, SEEK_CUR); // include header
        sceIoRead(f2, tempbuffer, blocksize+0x10);
        sceIoLseek(f1, 0, SEEK_END);
        sceIoWrite(f1, tempbuffer, blocksize+0x10);
      }

    } else
      sceIoLseek(f2, blocksize, SEEK_CUR); // seek to next block

    //sceKernelDelayThread(100*1000); // 100ms
  }

  ///////////////////////////////////

  save_error_exit:

  sceIoClose(f1);
  sceIoClose(f2);
  sceIoRemove(tempfig);

  //DEBUG_LOG("save_thread done");

  saveing = false; // done
  sceKernelExitDeleteThread(0);
  return 0;
}

func_stat save_config() {
  DEBUG_LOG("%lu: save_config(%s)", getGametime(), config);

  ///////////////////////
  if( !saveing ) { // only start thread if not running already
    SceUID thid = sceKernelCreateThread("save_thread", save_thread, 0x18, 0x1000, PSP_THREAD_ATTR_USER, NULL);
    if( thid < 0 ) {
      ERROR_LOG("Could not create thread 0x%08X", thid);
	  // 0x80020190 = nomemory (for stackSize 0x10000)
      //sceKernelSleepThread(); // TODO: ?
      return thid;
    }
    saveing = true;
    sceKernelStartThread(thid, 0, NULL);
  }
  ///////////////////////

  return FUNC_OK; //done
}

static void setHeader(SceUID file, int entries, int size, int type) {
  sceIoLseek(file, -(size + 12), SEEK_CUR);
  sceIoWrite(file, &type, sizeof(int));
  sceIoWrite(file, &entries, sizeof(int));
  sceIoWrite(file, &size, sizeof(int));
  sceIoLseek(file, size, SEEK_CUR);
}

void writeValue(SceUID file, int16 id, int value) {
  sceIoWrite(file, &id, sizeof(id));
  sceIoWrite(file, &value, sizeof(value));
}

void writeBool(SceUID file, int16 id, bool boolean) {
  sceIoWrite(file, &id, sizeof(id));
  sceIoWrite(file, &boolean, sizeof(boolean));
}

func_stat writeCategoryHeader(SceUID file, u8string magic) {
  //DEBUG_LOG("category %s", magic);
  u8char buffer[16]; memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s", magic);
  return sceIoWrite(file, buffer, sizeof(buffer));
}

int workBlock(SceUID file, const CDRMenuItem *menu_list, usize menu_max, int identifier, int mode) {
  int ret, counter = 0;
  void *(* func)(int calltype, int keypress, bool defaultstatus, int defaultval);

  for( array_uidx i = 0; i < menu_max; i++ ) {
    func = menu_list[i].value;
    if( (menu_list[i].def_stat != -1) || (menu_list[i].type == MENU_CDR_EDITOR || menu_list[i].type == MENU_CDR_FILES ) ) { // only for cheats and editors
      if( (menu_list[i].conf_id >> 12) == identifier ) { // eg.: 0x3XXX = Settings
        //DEBUG_LOG("id 0x%04X", menu_list[i].conf_id);

        if( mode == 1 ) { /// "FUNC_GET_VALUE"
          ret = (int)func(FUNC_GET_VALUE, 0, 0, 0);
          //DEBUG_LOG("ret = 0x%08X", ret);
          if( ret > 0 )
            writeValue(file, menu_list[i].conf_id, ret);  // only positive values will be saved (if 0 its default and doesn't need to be saved in ini) ...
          else continue;

        } else { /// "FUNC_GET_STATUS"
          if( identifier == 2 ) { // Categories
            writeBool(file, menu_list[i].conf_id, category_index[menu_list[i].cat]); // get category bools directly from array

          } else // everything else
            writeBool(file, menu_list[i].conf_id, (int)func(FUNC_GET_STATUS, 42, 0, 0)); // some cheats like "world_gravity" return a different value than the normal status to store (thus 42 as identifier for that)
        }

        counter++;
      }
    }
  } //DEBUG_LOG("counter = %i", counter);
  return counter;
}

static int fillBlock(SceUID file, int size, char placeholder) { // fill up the block to look nice
  while( (size % 16) != 0 ) {
    //DEBUG_LOG("size = %i, mod = %i", size, (size % 16));
    sceIoWrite(file, &placeholder, sizeof(char));
    size++;
  }
  return size;
}

/*
static int writeMemory(SceUID file, int start, int slots, int size) {

  if( !isInMemBounds(start) ) {
    //DEBUG_LOG("writeMemory error address: 0x%08X", start);
    return 0;
  }

  int ret, addr;

  for( int i = 0, addr = start; i < slots; i++, addr += size ) {
    ret = sceIoWrite(file, (void *)addr, size);
    //if( ret <= 0) {
    //  DEBUG_LOG("sceIoWrite error 0x%08X", ret);
    //}
  }

  return i * size; // bytes written
}
  */

func_stat create_config(const CDRMenuItem *menu_list, usize menu_max) {
  DEBUG_LOG("%lu: create_config(%s)", getGametime(), config);

  int ret, counter;

  SceUID file = sceIoOpen(config, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
  if( file < 0 ) {
    ERROR_LOG("sceIoOpen error (0x%08X)", file);
    return FUNC_ERROR; // error
  }

  /// Menu Settings
  writeCategoryHeader(file, "SETT"); // LCS ? "LSET" : "VSET"
  counter = workBlock(file, menu_list, menu_max, 0x3, 0); // 0x3xxx is for CDR Settings
  ret = fillBlock(file, counter * 3, 0x00); // 3 because "id" (short) 2 +  "bool" (char) 1
  setHeader(file, counter, ret, 1); // 1 = char (saves booleans)

  /// Hex Editor Address
  writeCategoryHeader(file, LCS ? "LHEX" : "VHEX");
  sceIoWrite(file, &hex_adr, sizeof(hex_adr));
  ret = fillBlock(file, sizeof(hex_adr), 0x00); // 4 because int
  setHeader(file, sizeof(hex_adr), ret, 0); // ret is whole blocksize, 0 for memory

  /// Categories
  writeCategoryHeader(file, LCS ? "LCAT" : "VCAT");
  counter = workBlock(file, menu_list, menu_max, 0x2, 0); // 0x2xxx is for Categories
  ret = fillBlock(file, counter * 3, 0x00); // 3 because "id" (short) 2 +  "bool" (char) 1
  setHeader(file, counter, ret, 1); // 1 = char

  /// Cheats
  writeCategoryHeader(file, LCS ? "LCHT" : "VCHT");
  counter = workBlock(file, menu_list, menu_max, 0x1, 0); // 0x1xxx is for Cheats & 0 triggers "FUNC_GET_STATUS"
  ret = fillBlock(file, counter * 3, 0x00); // 3 because "id" (short) 2 +  "bool" (char) 1
  setHeader(file, counter, ret, 1); // 1 = char

  /// Cheats (Values)
  writeCategoryHeader(file, LCS ? "LVAL" : "VVAL");
  counter = workBlock(file, menu_list, menu_max, 0x1, 1); // 0x1xxx is for Cheats & 1 triggers "FUNC_GET_VALUE"
  ret = fillBlock(file, counter * 6, 0x00); // 6 because "id" (short) 2 +  "value" (int) 4
  setHeader(file, counter, ret, 4); // 4 = int (saves values)

  /// Editor Positions
  writeCategoryHeader(file, LCS ? "LEDT" : "VEDT");
  counter = workBlock(file, menu_list, menu_max, 0x4, 1); // 0x4xxx is for Cheats & 1 triggers "FUNC_GET_VALUE"
  ret = fillBlock(file, counter * 6, 0x00); // 6 because "id" (short) 2 +  "value" (int) 4
  setHeader(file, counter, ret, 4); // 4 = int (position value)


  /*if( flag_use_advaconfig ) { // scrapped because file gets too big on PSP to handle :(

    //DEBUG_LOG("create_config now advanced stuff");
    //DEBUG_LOG("gp_ = 0x%08X", gp_);

    /// Vehicle Spawns
    writeCategoryHeader(file, LCS ? "LVEH" : "VVEH");
    counter = writeMemory(file, addr_vehiclesworldspawn, var_vehiclesworldspawnslots, var_vehiclesworldspawnslotsize);
    ret = fillBlock(file, counter, 0x00); // counter holds bytes
    setHeader(file, counter, ret, 0); // counter is bytes, ret is whole blocksize, 0 for memory

    /// Car Colors
    writeCategoryHeader(file, LCS ? "LVCO" : "VVCO");
    counter = writeMemory(file, carcolsDAT_base, var_carcolsdatslots, var_carcolsdatslotsize);
    ret = fillBlock(file, counter, 0x00); //counter holds bytes
    setHeader(file, counter, ret, 0); //counter is bytes, ret is whole blocksize, 0 for memory

    /// Ped Colors (VCS only)
    if( VCS ) {
      writeCategoryHeader(file, "VPCO");
      counter = writeMemory(file, pedcolsDAT_base, var_carcolsdatslots, var_carcolsdatslotsize);
      ret = fillBlock(file, counter, 0x00); // counter holds bytes
      setHeader(file, counter, ret, 0); // counter is bytes, ret is whole blocksize, 0 for memory
    }

    /// Handling
    if( LCS ) { // TODO: VCS
      writeCategoryHeader(file, LCS ? "LHND" : "VHND");
      counter = writeMemory(file, getInt(ptr_handlingCFG + (LCS ? 0 : gp_)) + 0x20, var_handlingcfgslots, var_handlingcfgslotsize);
      ret = fillBlock(file, counter, 0x00); // counter holds bytes
      setHeader(file, counter, ret, 0); // counter is bytes, ret is whole blocksize, 0 for memory
    }

    /// Particles
    writeCategoryHeader(file, LCS ? "LPTC" : "VPTC");
    counter = writeMemory(file, getInt(ptr_particleCFG + (LCS ? 0 : gp_)), var_particleCFGslots, var_particleCFGslotsize);
    ret = fillBlock(file, counter, 0x00); // counter holds bytes
    setHeader(file, counter, ret, 0); // counter is bytes, ret is whole blocksize, 0 for memory

    /// pedstats.dat
    writeCategoryHeader(file, LCS ? "LPST" : "VPST");
    counter = writeMemory(file, getInt(getInt(ptr_pedstatTable+(LCS?0x0:gp_))), var_pedstatDATslots, var_pedstatDATslotsize);
    ret = fillBlock(file, counter, 0x00); // counter holds bytes
    setHeader(file, counter, ret, 0); // counter is bytes, ret is whole blocksize, 0 for memory

    /// weapon.dat (singleplayer only)
    writeCategoryHeader(file, LCS ? "LWPN" : "VWPN");
    counter = writeMemory(file, getInt(getInt(ptr_weaponTable+(LCS?0x0:gp_)) + 4), var_weaponDATslots, var_weaponDATslotsize);
    ret = fillBlock(file, counter, 0x00); // counter holds bytes
    setHeader(file, counter, ret, 0); // counter is bytes, ret is whole blocksize, 0 for memory

  }*/

  //DEBUG_LOG("create_config done");
  sceIoClose(file);
  return FUNC_OK; // success
}

static int getValueFromConfigFor(SceUID file, const char *magic, short id) {

  //DEBUG_LOG("looking for id: 0x%04X", id);
  sceIoLseek(file, 0, SEEK_SET); // seek to start of file

  char magicbuf[8];
  int read, entries = 0, type = 0, blocksize = 0;

  do {
    sceIoLseek(file, blocksize, SEEK_CUR); // 0 in first run

    memset(magicbuf, 0, sizeof(magicbuf));
    read = sceIoRead(file, &magicbuf, sizeof(int));
    //DEBUG_LOG("read %d", read);
    if( read <= 0 )
      return FUNC_ERROR; // category not found or error

    //DEBUG_LOG("magicbuf %s vs magic %s", magicbuf, magic);

    //sceIoLseek(file, 4, SEEK_CUR);
    sceIoRead(file, &type, sizeof(int));
    sceIoRead(file, &entries, sizeof(int));
    sceIoRead(file, &blocksize, sizeof(int));

    //DEBUG_LOG("blocksize 0x%08X", blocksize);

  } while( strcmp(magicbuf, magic) != 0 );

  //DEBUG_LOG("found magic category block!");

  short _id = 0;
  char _bool = 0;
  short _val = 0; // unused atm
  int _value = 0;

  for( int i = 0; i < entries; i++ ) {
    //DEBUG_LOG("%i/%i", i+1, entries);
    sceIoRead(file, &_id, sizeof(_id));

    if( type == 1) {
      sceIoRead(file, &_bool, sizeof(_bool));
      if( _id == id ) return (int)_bool;

    } else if( type == 2) {
      sceIoRead(file, &_val, sizeof(_val));
      if( _id == id ) return (int)_val;

    } else if( type == 4) {
      sceIoRead(file, &_value, sizeof(_value));
      if( _id == id ) return (int)_value;

    }
  }

  //DEBUG_LOG("id 0x%04X not found!", id);
  return FUNC_ERROR; // not found
}


func_stat load_config(const CDRMenuItem *menu_list, usize menu_max) { // loads menu defaults if config value not found (do we want that?)
  CONFIG_LOG("load_config()");

  char tempfig[256];
  snprintf(tempfig, sizeof(tempfig), "%s_", config);
  if( doesFileExist(tempfig) ) { // if this (sill) exists, user probably exited game before saving was done
    ERROR_LOG("found bad config");
    sceIoRemove(tempfig);
    sceIoRemove(config);
    return FUNC_ERROR;
  }

  int ret;
  static int keypress = -2;
  void (* func)(int calltype, int keypress, bool defaultstatus, int defaultval);

  SceUID file = sceIoOpen(config, PSP_O_RDONLY, 0777);
  if( file < 0 ) {
    ERROR_LOG("sceIoOpen(%s) error 0x%08X", config, file);
    return FUNC_ERROR; // error
  }

  for( array_uidx i = 0; i < menu_max; i++ ) {
    if( (LCS && menu_list[i].LC) || (VCS && menu_list[i].VC) ) { // only if this is meant for the game version
      func = (void *)(menu_list[i].value);

      CONFIG_LOG("for: 0x%04X", menu_list[i].conf_id);

      if( menu_list[i].def_stat != -1 ) {

        /// CHEATS //////////////////////////////
        if( (menu_list[i].conf_id >> 12) == 1 ) {
          int status = getValueFromConfigFor(file, LCS ? "LCHT" : "VCHT", menu_list[i].conf_id);
          int valuee = getValueFromConfigFor(file, LCS ? "LVAL" : "VVAL", menu_list[i].conf_id);
          //CONFIG_LOG("id 0x%04X -> status %d, valuee %d", menu_list[i].conf_id, status, valuee);
          func(FUNC_SET, keypress, status < 0 ? menu_list[i].def_stat : status, valuee < 0 ? 0x0EADBEEF : valuee);
        }

        /// CATEGORIES //////////////////////////
        if( (menu_list[i].conf_id >> 12) == 2 ) {
          if( flag_coll_cats ) { // only set categories if they are set to be able to collapse/expandable
            ret = getValueFromConfigFor(file, LCS ? "LCAT" : "VCAT", menu_list[i].conf_id);
            func(FUNC_SET, menu_list[i].cat, ret < 0 ? menu_list[i].def_stat : ret, 0); // set def_stat from CDRMenuItem if not found in config
          }
        }

        /// CHEAT DEVICE SETTINGS ///////////////
        if( (menu_list[i].conf_id >> 12) == 3 ) {
          ret = getValueFromConfigFor(file, "SETT", menu_list[i].conf_id);
          func(FUNC_SET, keypress, ret < 0 ? menu_list[i].def_stat : ret, 0);
        }


      }

      if( menu_list[i].type == MENU_CDR_EDITOR || menu_list[i].type == MENU_CDR_FILES ) {

        /// EDITOR POSITIONS ////////////////////
        if( (menu_list[i].conf_id >> 12) == 4 ) {
          ret = getValueFromConfigFor(file, LCS ? "LEDT" : "VEDT", menu_list[i].conf_id);
          if( ret == -1 ) // value not found..
            ret = 0; // ..pass zero to set as default for those
          func(FUNC_SET, ret, 0, 0);
        }
      }
    }
  }
  CONFIG_LOG("load_config -> menu loop done");

  sceIoClose(file); // "load_config_block" opens config again

  /// Hex Editor Address
  load_config_block(LCS ? "LHEX" : "VHEX", (int)&hex_adr);
  CONFIG_LOG("hex_adr done (0x%08X)", hex_adr);

  /// Editor stuff --> files get too big to handle on PSP.. scrap all this :(
  /*if( flag_use_advaconfig && gametimer > 0) { // gametime excludes load_config called right after gameboot

    CONFIG_LOG("load_config -> advanced block");

    ***********************************************************************************************
     * Some reset on new game (World Vehicles) while others are loaded once on gameboot (Car Colors)
     * In here we only load when location known already (might not at gameboot for some) and
     * for some with Gametime check as they would be overwritten anyways.
     *
     * "load_config_block(xyz)" is called like applyOnce() for those resetting to handle that
     * otherwise most of those below only trigger on manual "load Settings" via menu
    ***********************************************************************************************

    /// WorldSpawns (reset on newgame)
    if(  addr_vehiclesworldspawn > 0 )
      load_config_block(LCS ? "LVEH" : "VVEH", addr_vehiclesworldspawn);

    /// CarColors
    if( carcolsDAT_base > 0 )
      load_config_block(LCS ? "LVCO" : "VVCO", carcolsDAT_base);

    /// Pedcols.dat (VCS only)
    if( VCS && pedcolsDAT_base > 0 )
      load_config_block("VPCO", pedcolsDAT_base);

    /// handling.dat
    if( LCS && getInt(ptr_handlingCFG + (LCS ? 0 : gp_)) > 0 )
      load_config_block(LCS ? "LHND" : "VHND", getInt(ptr_handlingCFG + (LCS ? 0 : gp_)) + 0x20);

    /// particles.cfg
    if( getInt(ptr_particleCFG + (LCS ? 0 : gp_)) > 0 )
      load_config_block(LCS ? "LPTC" : "VPTC", getInt(ptr_particleCFG + (LCS ? 0 : gp_)));

    /// pedstats.dat
    if( getInt(getInt(ptr_pedstatTable+(LCS?0x0:gp_))) > 0 )
      load_config_block(LCS ? "LPST" : "VPST", getInt(getInt(ptr_pedstatTable+(LCS?0x0:gp_))));

    /// weapon.dat
    if( getInt(getInt(ptr_weaponTable+(LCS?0x0:gp_)) + 4) > 0 )
      load_config_block(LCS ? "LWPN" : "VWPN", getInt(getInt(ptr_weaponTable+(LCS?0x0:gp_)) + 4));


  }*/

  CONFIG_LOG("all done");

  return FUNC_OK; // success
}

func_stat load_config_block(const u8string magic, uaddr address) {
  CONFIG_LOG("load_config_block(%s, 0x%08X)", magic, address);

  //if( !flag_use_advaconfig )
  //  return FUNC_ERROR;

  if( !isInMemBounds(address) )
    return FUNC_ERROR;

  SceUID file = sceIoOpen(config, PSP_O_RDONLY, 0777);
  if( file < 0 ) {
    ERROR_LOG("sceIoOpen error (0x%08X)", file);
    return FUNC_ERROR; // error
  }

  ///////////////////////////////////////////////////////////////////////////

  sceIoLseek(file, 0, SEEK_SET); // seek to start of file
  char magicbuf[8];
  int read, entries = 0, type = 0, blocksize = 0;

  CONFIG_LOG("do loop");

  do { // loop it
    sceIoLseek(file, blocksize, SEEK_CUR); // 0 in first run
    memset(magicbuf, 0, sizeof(magicbuf));
    read = sceIoRead(file, &magicbuf, sizeof(int));

    if( read <= 0 ) {
      ERROR_LOG("sceIoRead (0x%08X)", read);
      sceIoClose(file);
      return FUNC_ERROR; // category not found or error
    }

    sceIoRead(file, &type, sizeof(int));
    sceIoRead(file, &entries, sizeof(int));
    sceIoRead(file, &blocksize, sizeof(int));

  } while( strcmp(magicbuf, magic) != 0 );

  CONFIG_LOG("type = 0x%08X, entries = 0x%08X, blocksize = 0x%08X, ", type, entries, blocksize);

  if( type == 0 && entries > 0) {
    CONFIG_LOG("all looks good.. read in!");
    read = sceIoRead(file, (void *)address, entries); // entries holds bytes to write
    CONFIG_LOG("sceIoRead returned 0x%08X", read);
    sceIoClose(file);
    CONFIG_LOG("load_config_block success");
    return FUNC_OK; // success
  }

  ///////////////////////////////////////////////////////////////////////////
  sceIoClose(file);
  ERROR_LOG("load_config_block end");
  return FUNC_ERROR; // error
}

/////////////////////////////////////////////////////////////////

int setValueInConfigFor(short id UNUSED, int value UNUSED) {

  // if id found
    // update value

  // if id not found
    // insert and rebuild config file

  return FUNC_ERROR; // not found
}
#endif
