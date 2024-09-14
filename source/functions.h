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

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__


enum{
  FALSE, // 0
  TRUE   // 1
};

typedef struct { // teleporter struct
  char *name;
  float xval;
  float yval;
  float zval;
} teleports_pack;

typedef struct { // vehicles struct
  short id;
  short type;
  char *GXTname;   // like internal name
  char *name;      // In-Game name
  short hndlng_no; // number of vehicle in handling order (@ 0x52 in vehicle style block)
  short spawnsave; // bool crashes game when spawned
} vehicles_pack;

typedef struct { // character struct
  unsigned char id;
  char *model;   // model name
  char *name;    // Real name
  short row;
  int spawnsave; // bool crashes game when spawned
} character_pack;

typedef struct {
  char *name;
  short id;
  char type;    // LCS: 00 clothes?? | 03 save_cd, rampage | 08 money | 0F weapons, health, adrenaline    -> 0xF are re-appearing ones? rest one-time pickups
  short amount; // standard value for ammo / money object holds
} pickups_pack;

typedef struct {
  char *name;
  char id;   // short in LCS, char in VCS
  char color;
} mapicons_pack;


enum ModelInfoType {
  MODELINFO_SIMPLE       = 1,
  MODELINFO_MLO          = 2, // unused
  MODELINFO_TIME         = 3,
  MODELINFO_WEAPON       = 4,
  MODELINFO_ELEMENTGROUP = 5,
  MODELINFO_VEHICLE      = 6,
  MODELINFO_PED          = 7,
  MODELINFO_XTRACOMP     = 8, // unused
};

/* enum VehicleType {
  VEHICLETYPE_CAR,
  VEHICLETYPE_BOAT,
  VEHICLETYPE_JETSKI, //VCS
  VEHICLETYPE_TRAIN,
  VEHICLETYPE_HELI,
  VEHICLETYPE_PLANE,
  VEHICLETYPE_BIKE,
  VEHICLETYPE_FERRY,
}; */

enum {
  VEHICLE,
  PEDESTRIAN,
  WORLDOBJ,
  MAP
};

enum {
  VEHICLE_CAR,
  VEHICLE_BIKE,
  VEHICLE_HELI,
  VEHICLE_PLANE,
  VEHICLE_BOAT,
  //VEHICLE_TRAIN,   // LCS only
  //VEHICLE_FERRY,   // LCS only
  //VEHICLE_BICYCLE, // VCS only
  //VEHICLE_JETSKI,  // VCS only
  
};

enum {
  WEAPON_FIST,    // 0
  WEAPON_MEELE,   // 1
  WEAPON_GRENADE, // ..
  WEAPON_HANDGUN,
  WEAPON_SHOTGUN,
  WEAPON_SMG,
  WEAPON_ASSAULT,
  WEAPON_ROCKET,
  WEAPON_SNIPER,
  WEAPON_CAMERA
};


////////////////////////////////////

int debugPrintf(const char *text, ...); // for testing only

////////////////////////////////////

void (* SetActorSkinTo)(int ped_obj, const char *name); // name must be lower case!!
void (* LoadAllModelsNow)(char x);
void (* RefreshActorSkin)(int ped_obj);

////////////////////////////////////

int isInMemBounds(int valtocheck);

void setBit(int adr, char bit, char boolean);

void setInt(int adr, int value);
int getInt(int adr);

void setShort(int adr, short value);
short getShort(int adr);

void setChar(int adr, char value);
char getChar(int adr);

void setByte(int adr, unsigned char value);
unsigned char getByte(int adr);

void setNibbleLow(int adr, unsigned char value);
unsigned char getNibbleLow(int adr);

void setNibbleHigh(int adr, unsigned char value);
unsigned char getNibbleHigh(int adr);

void setFloat(int adr, float value);
float getFloat(int adr);

void setString(int adr, char* string, int mode);
char *getString(int adr, int mode);

unsigned char *getSavedataKey();

////////////////////////////////////////////////////////////////////////
 
void teleport(float x, float y, float z);
void teleportFixForVehicle();


/// Player flags
char getMaxHealthMultiplier();
void setMaxHealthMultiplier(char value);
char getMaxArmorMultiplier();
void setMaxArmorMultiplier(char value);
char getUnlimitedSprintStatus();
void setUnlimitedSprintStatus(char value);
char getUnlimitedSwimStatus();
void setUnlimitedSwimStatus(char value);

float getPedFacingDirectionInDegree(int ped_base_adr);

int getObjectsTouchedObjectAddress(int base_adr);
int getTypeFromAddress(int address);


/// PEDs
float getPedHealth(int ped_base_adr);
float getPedArmor(int ped_base_adr);

int getWantedLevel(int ped_base_adr);
int getMediaAttentionValue(int ped_base_adr);

void setPedHealthAndArmor(int ped_base_adr, float health, float armor);
void setPedNeverOnFire(int ped_base_adr);
void setPedAmmoForWeapon(int ped_base_adr, int type, int loaded, int total);
void setPedMass(int ped_base_adr, float mass1, float mass2);
void setPedExitVehicleNow(int ped_base_adr);
void setPedInvisible(int ped_base_adr, int boolean);
void setPedIgnoredByEveryone(int ped_base_adr, int boolean);
void setPedCanBeTargeted(int ped_base_adr, int boolean);
void setPedFlagToUnload(int ped_base_adr);

void setPedOrVehicleFreezePosition(int ped_base_adr, int boolean);

char getPedsCurrentAnimation(int ped_base_adr);
int getPedsCurrentWeapon(int ped_base_adr);

unsigned char getPedID(int ped_base_adr);
int getPedsVehicleObjectAddress(int base_adr);

int getPedObjectIsActive(int ped_base_adr);
int getPedActiveObjects(int peds_base, int peds_max, int peds_size);
int checkPedIsInWater(int ped_base_adr);
int getPedDrowning(int ped_base_adr);


/// Vehicle 
char lcs_getVehicleColorBase(int vehicle_base_adr);
char lcs_getVehicleColorStripe(int vehicle_base_adr);
void lcs_setVehicleColorBase(int vehicle_base_adr, char x);
void lcs_setVehicleColorStripe(int vehicle_base_adr, char x);
int vcs_getVehicleColorBase(int vehicle_base_adr);
int vcs_getVehicleColorStripe(int vehicle_base_adr);
void vcs_setVehicleColorBase(int vehicle_base_adr, int color);
void vcs_setVehicleColorStripe(int vehicle_base_adr, int color);

void setVehicleRadioStation(int vehicle_base_adr, char id);
void setRadioStationNow(char id);
void setVehicleGravityApplies(int vehicle_base_adr, int boolean);
void setVehicleWheelCamber(int vehicle_base_adr, float val);
float getVehicleWheelCamber(int vehicle_base_adr);
float getVehicleSpeed(int vehicle_base_adr);
char getVehicleCurrentGear(int vehicle_base_adr);
void setVehicleFlagToUnload(int vehicle_base_adr);
void setVehicleDoorsLocked(int vehicle_base_adr, int boolean);
void setVehicleMakePhysical(int vehicle_base_adr);
int getVehicleAiStatus(int vehicle_base_adr);

short getVehicleID(int vehicle_base_adr);
short getVehicleTypeByID(short id);

int isVehicleInWater(int vehicle_base_adr);
int isVehicleInAir(int vehicle_base_adr);
int isVehicleUpsideDown(int vehicle_base_adr);

void setVehicleHealth(int vehicle_base_adr, float value);
float getVehicleHealth(int vehicle_base_adr);
void setVehicleNoPhysicalDamage(int vehicle_base_adr);
void setVehicleRepairTyres(int vehicle_base_adr, short vehicle_type);

void makeVehicleExplode(int vehicle_base_adr);

int getVehicleObjectIsActive(int vehicle_base_adr);
int getVehicleActiveObjects(int vehicle_base, int vehicle_max, int vehicle_size);

int getVehicleDriverHandle(int vehicle_base_adr);
int getVehiclePassengerHandle(int vehicle_base_adr);
int getVehicleBackseatLeftHandle(int vehicle_base_adr);
int getVehicleBackseatRightHandle(int vehicle_base_adr);

void setHeliHeightLimit(float height);
void setPoliceChaseHeliModel(short model);

float getBmxJumpMultiplier();
void setBmxJumpMultiplier(float value);


/// World
void setWaterLevel(float level);
void resetWaterLevel();
void setWindClipped(float wind);
float getWind();
int getWorldObjectIsActive(int worldobjs_base_adr);
int getWorldActiveObjects(int worldobjs_base, int worldobjs_max, int worldobjs_size);


/// pickups
int getPickupIsActive(int pickup_base_adr);
short getPickupID(int pickup_base_adr);
char *getPickupNameByID(short id);
int getPickupsActiveObjects(int pickups_base, int pickups_max, int pickups_size);
int getPickupsActiveObjectsWithID(short id);
int spawnPickup(short id, char type, short amount, float x, float y, float z);


///mapicons
int getMapiconIsActive(int mapicon_base_adr);
int getMapiconIsVisible(int mapicon_base_adr);
int getMapiconsActiveObjects(int mapicon_base, int mapicons_max, int mapicons_size);
char *getMapiconNameByID(short id);
char getMapiconID(int mapicon_base_adr);
int getMapiconType(int mapicon_base_adr);
char *getMapiconTypeName(int mapicon_base_adr);
int getMapiconColor(int mapicon_base_adr);
char *getMapiconColorName(int mapicon_base_adr);
int getMapiconLinkedObjectSlotNumber(int mapicon_base_adr);

void removeMapIcon(int mapicon_base_adr);
void removeAllMapIcons();
void removeAllCustomMapIcons();
int createCustomMapIcon(char icon, int color, char onlywhenclose, char size, float x, float y, float z);

int createCustomMapIconsForPickupsWithID(short id, char icon, int color);
int createCustomMapIconsForWeaponsInRange(float range);

void setLastButtonPressedInHistory(char button);
void activateCheatCode(char a, char b, char c, char d, char e, char f, char g, char h);


int getAddressOfHandlingSlotForID(short id);


/// Misc
char getClockHours();
char getClockMinutes();
char getClockSeconds();
short getClockMultiplier();
void setClockMultiplier(short val);
void setClockTime(char hours, char minutes, char seconds);
void setClockHours(char hours);
void setClockFreeze(int boolean);
int getWeather();

int getGametime();
int getMultiplayer();

void setGamespeed(float val);
float getGamespeed();

void setDisplaySettingsToggleRadar(int boolean);
int getDisplaySettingsToggleRadar();
void setDisplaySettingsToggleHUD(int boolean);
int getDisplaySettingsToggleHUD();
void setFreezeGame(int boolean);
void setMapLegendToggle(int boolean);
int getMapLegendToggle();


void setTimedTextbox(const char *sentence, float duration);
int isTextboxShowing();
void blockTextBox();
int isDialogShowing();
void blockDialogs();

int isHudDisabledWhileCutsceneCamera();

int getIsland();
int getLanguage();

int isPedCrouching(int ped);
void TaskDuck(int ped);

void setRandomPedCheat(char id);


/// Parked Vehicle Spawns
int getVehicleWorldSpawnSlotIsActive(int base);
int getVehicleWorldSpawnSlotActiveObjects(int base);
int getVehicleWorldSpawnSlotAddress(int slot);
int isCustomParkedVehicleSpawnViaSlot(int slot);
int createParkedVehicleSpawnViaSlot(int slot, short id, float x, float y, float z, float deg, short color1, short color2, char alarm, short door_lock);
int createParkedVehicleSpawnViaReplaceID(short replace_id, short id, float x, float y, float z, float deg, short color1, short color2, char alarm, short door_lock);


/// Garage
int getGarageVehicleSlotIsActive(int base);
int getGarageVehicleActiveObjects(int base);
void createGarageVehicle(int garage_base_adr, short id, float x, float y, float z, float vec1, float vec2, float vec3, char flags1, char flags2, char radio, char var1, char var2);

int getIdeType(short id);
int getFirstIdeOfType(int type);
int getLastIdeOfType(int type);
char *getIdeTypeName(char no);
int getAddressOfIdeSlotForID(short id);
int getIdeSlotSizeByType(char no);

char *getModelNameViaHash(int hash, int time);
char *getModelNameViaID(int id, int time);
char *getGxtTranslation(char *identifier);
char *getRealVehicleNameViaID(short id);
char *getGxtIdentifierForVehicleViaID(int id);
char *vcs_getNameForPedViaID(int id);

char *getRadioStationName(int no);

char *removeColor(char *string);


/// Camera
void setCameraCenterBehindPlayer();
void setFieldOfView(float fov);
float getFieldOfView();

void setPedSkin(int ped_handle, const char *name);
int removeIPLObjectsCollision(short id);

int getParticleOffsetFor(char *name);
char *getPedstatName(int no);


/// SCM
void CustomScriptAdd();
void CustomScriptClearBuffer();
void CustomScriptClear(int address, int codesize);
void CustomScriptPlace(u8 *script, int address, int codesize);
void CustomScriptExecute(int var);

#endif
