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

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "mctypes.h"
#include "storiesrwsdk.h"

typedef struct {
  u8string name;
  Vector3D position;
} GTAWorldLocation;

typedef struct { // vehicles struct
  uint16 id;
  int16 type;
  u8string GXTname;   // like internal name
  u8string name;      // In-Game name
  int16 hndlng_no; // number of vehicle in handling order (@ 0x52 in vehicle style block)
  bool16 spawnsave; // bool crashes game when spawned
} GTAVehicle;

typedef struct { // character struct
  uint8 id;
  u8string model;   // model name
  u8string name;    // Real name
  int16 row;
  bool32 spawnsave; // bool crashes game when spawned
} GTACharacter;

typedef struct {
  u8string name;
  uint16 id;
  uint8 type;    // LCS: 00 clothes?? | 03 save_cd, rampage | 08 money | 0F weapons, health, adrenaline    -> 0xF are re-appearing ones? rest one-time pickups
  uint16 amount; // standard value for ammo / money object holds
} GTAPickup;

typedef struct {
  u8string name;
  uint8 id;   // short in LCS, char in VCS
  uint8 color;
} GTAMapIcon;

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

extern void (* SetActorSkinTo)(int ped_obj, const char *name); // name must be lower case!!
extern void (* LoadAllModelsNow)(char x);
extern void (* RefreshActorSkin)(int ped_obj);

////////////////////////////////////

extern bool isInMemBounds(uaddr valtocheck);

extern void setBit(uaddr adr, uint8 bit, bool boolean);

extern void setInt(uaddr adr, int value);
extern int getInt(uaddr adr);

extern void setShort(uaddr adr, short value);
extern short getShort(uaddr adr);

extern void setByte(uaddr adr, byte value);
extern byte getByte(uaddr adr);

extern void setNibbleLow(uaddr adr, uint8 value);
extern uint8 getNibbleLow(uaddr adr);

extern void setNibbleHigh(uaddr adr, uint8 value);
extern uint8 getNibbleHigh(uaddr adr);

extern void setFloat(uaddr adr, float value);
extern float getFloat(uaddr adr);

extern void setString(uaddr adr, u8string string, bool unicode);
extern u8string getString(uaddr adr, bool unicode);

#ifdef SAVEDITOR
extern u8char *getSavedataKey();
#endif

////////////////////////////////////////////////////////////////////////

extern void SetPlayerPosition(float x, float y, float z);
extern void SetPlayerPositionFixForVehicle();

/// Player flags
extern uint8 getMaxHealthMultiplier();
extern void setMaxHealthMultiplier(uint8 value);
extern uint8 getMaxArmorMultiplier();
extern void setMaxArmorMultiplier(uint8 value);
extern bool getUnlimitedSprintStatus();
extern void setUnlimitedSprintStatus(bool value);
extern bool getUnlimitedSwimStatus();
extern void setUnlimitedSwimStatus(bool value);

extern float getPedFacingDirectionInDegree(uaddr ped_base_adr);

extern uaddr getObjectsTouchedObjectAddress(uaddr base_adr);
extern uint8 getTypeFromAddress(uaddr address);

/// PEDs
extern float getPedHealth(uaddr ped_base_adr);
extern float getPedArmor(uaddr ped_base_adr);

extern int getWantedLevel(uaddr ped_base_adr);
extern int getMediaAttentionValue(uaddr ped_base_adr);

extern void setPedHealthAndArmor(uaddr ped_base_adr, float health, float armor);
extern void setPedNeverOnFire(uaddr ped_base_adr);
extern void setPedAmmoForWeapon(uaddr ped_base_adr, int type, int loaded, int total);
extern void setPedMass(uaddr ped_base_adr, float mass1, float mass2);
extern void setPedExitVehicleNow(uaddr ped_base_adr);
extern void setPedInvisible(uaddr ped_base_adr, bool boolean);
extern void setPedIgnoredByEveryone(uaddr ped_base_adr, bool boolean);
extern void setPedCanBeTargeted(uaddr ped_base_adr, bool boolean);
extern void setPedFlagToUnload(uaddr ped_base_adr);

extern void setPedOrVehicleFreezePosition(uaddr ped_base_adr, bool boolean);

extern uint8 getPedsCurrentAnimation();
extern int getPedsCurrentWeapon(uaddr ped_base_adr);

extern ushort getPedID(uaddr ped_base_adr);
extern uaddr getPedsVehicleObjectAddress(uaddr base_adr);

extern bool getPedObjectIsActive(uaddr ped_base_adr);
extern usize getPedActiveObjects(uaddr peds_base, usize peds_max, usize peds_size);
extern bool checkPedIsInWater(uaddr ped_base_adr);
extern bool getPedDrowning(uaddr ped_base_adr);

/// Vehicle
extern uint8 lcs_getVehicleColorBase(uaddr vehicle_base_adr);
extern uint8 lcs_getVehicleColorStripe(uaddr vehicle_base_adr);
extern void lcs_setVehicleColorBase(uaddr vehicle_base_adr, uint8 x);
extern void lcs_setVehicleColorStripe(uaddr vehicle_base_adr, uint8 x);
extern CRGBA vcs_getVehicleColorBase(uaddr vehicle_base_adr);
extern CRGBA vcs_getVehicleColorStripe(uaddr vehicle_base_adr);
extern void vcs_setVehicleColorBase(uaddr vehicle_base_adr, CRGBA color);
extern void vcs_setVehicleColorStripe(uaddr vehicle_base_adr, CRGBA color);

extern void setVehicleRadioStation(uaddr vehicle_base_adr, uint8 id);
extern void setRadioStationNow(uint8 id);
extern void setVehicleGravityApplies(uaddr vehicle_base_adr, bool boolean);
extern void setVehicleWheelCamber(uaddr vehicle_base_adr, float val);
extern float getVehicleWheelCamber(uaddr vehicle_base_adr);
extern float getVehicleSpeed();
extern uint8 getVehicleCurrentGear(uaddr vehicle_base_adr);
extern void setVehicleFlagToUnload(uaddr vehicle_base_adr);
extern void setVehicleDoorsLocked(uaddr vehicle_base_adr, bool boolean);
extern void setVehicleMakePhysical(uaddr vehicle_base_adr);
extern int getVehicleAiStatus(uaddr vehicle_base_adr);

extern uint16 getVehicleID(uaddr vehicle_base_adr);
extern uint16 getVehicleTypeByID(uint16 id);

extern bool isVehicleInWater(uaddr vehicle_base_adr);
extern bool isVehicleInAir(uaddr vehicle_base_adr);
extern bool isVehicleUpsideDown(uaddr vehicle_base_adr);

extern void setVehicleHealth(uaddr vehicle_base_adr, float value);
extern float getVehicleHealth(uaddr vehicle_base_adr);
extern void setVehicleNoPhysicalDamage(uaddr vehicle_base_adr);
extern void setVehicleRepairTyres(uint16 vehicle_type);

extern void makeVehicleExplode(uaddr vehicle_base_adr);

extern bool getVehicleObjectIsActive(uaddr vehicle_base_adr);
extern usize getVehicleActiveObjects(uaddr vehicle_base, usize vehicle_max, usize vehicle_size);

extern int getVehicleDriverHandle(uaddr vehicle_base_adr);
extern int getVehiclePassengerHandle(uaddr vehicle_base_adr);
extern int getVehicleBackseatLeftHandle(uaddr vehicle_base_adr);
extern int getVehicleBackseatRightHandle(uaddr vehicle_base_adr);

extern void setHeliHeightLimit(float height);
extern void setPoliceChaseHeliModel(short model);

extern float getBmxJumpMultiplier();
extern void setBmxJumpMultiplier(float value);

/// World
extern void setWaterLevel(float level);
extern void resetWaterLevel();
extern void setWindClipped(float wind);
extern float getWind();
extern bool getWorldObjectIsActive(uaddr worldobjs_base_adr);
extern usize getWorldActiveObjects(uaddr worldobjs_base, usize worldobjs_max, usize worldobjs_size);

/// pickups
extern bool getPickupIsActive(uaddr pickup_base_adr);
extern uint16 getPickupID(uaddr pickup_base_adr);
extern u8string getPickupNameByID(uint16 id);
extern usize getPickupsActiveObjects(uaddr pickups_base, usize pickups_max, usize pickups_size);
extern usize getPickupsActiveObjectsWithID(uint16 id);
extern iaddr spawnPickup(uint16 id, uint8 type, int16 amount, float x, float y, float z);

///mapicons
extern bool getMapiconIsActive(uaddr mapicon_base_adr);
extern bool getMapiconIsVisible(uaddr mapicon_base_adr);
extern usize getMapiconsActiveObjects(uaddr mapicon_base, usize mapicons_max, usize mapicons_size);
extern u8string getMapiconNameByID(uint16 id);
extern uint8 getMapiconID(uaddr mapicon_base_adr);
extern int getMapiconType(uaddr mapicon_base_adr);
extern u8string getMapiconTypeName(uaddr mapicon_base_adr);
extern uint8 getMapiconColor(uaddr mapicon_base_adr);
extern u8string getMapiconColorName(uaddr mapicon_base_adr);
extern array_uidx getMapiconLinkedObjectSlotNumber(uaddr mapicon_base_adr);

extern void removeMapIcon(uaddr mapicon_base_adr);
extern void removeAllMapIcons();
extern void removeAllCustomMapIcons();
extern iaddr createCustomMapIcon(uint8 icon, CRGBA color, bool onlywhenclose, uint8 size, float x, float y, float z);
extern usize createCustomMapIconsForPickupsWithID(uint16 id, uint8 icon, CRGBA color);
extern usize createCustomMapIconsForWeaponsInRange(float range);

extern void setLastButtonPressedInHistory(uint8 button);
extern void activateCheatCode(uint8 a, uint8 b, uint8 c, uint8 d, uint8 e, uint8 f, uint8 g, uint8 h);

extern iaddr getAddressOfHandlingSlotForID(uint16 id);

/// Misc
extern uint8 getClockHours();
extern uint8 getClockMinutes();
extern uint8 getClockSeconds();
extern int16 getClockMultiplier();
extern void setClockMultiplier(int16 val);
extern void setClockTime(uint8 hours, uint8 minutes, uint8 seconds);
extern void setClockHours(uint8 hours);
extern void setClockFreeze(bool boolean);
extern uint16 getWeather();

extern uint32 getGametime();
extern bool getMultiplayer();

extern void setGamespeed(float val);
extern float getGamespeed();

extern void setDisplaySettingsToggleRadar(bool boolean);
extern bool getDisplaySettingsToggleRadar();
extern void setDisplaySettingsToggleHUD(bool boolean);
extern bool getDisplaySettingsToggleHUD();
extern void setFreezeGame(bool boolean);
extern void setMapLegendToggle(bool boolean);
extern bool getMapLegendToggle();

extern void setTimedTextbox(u8string sentence, float duration);
extern bool isTextboxShowing();
extern void blockTextBox();
extern bool isDialogShowing();
extern void blockDialogs();

extern bool isHudDisabledWhileCutsceneCamera();

extern int32 getIsland();
extern int32 getLanguage();

extern bool isPedCrouching();
extern void TaskDuck();

extern void setRandomPedCheat(int8 id);

/// Parked Vehicle Spawns
extern bool getVehicleWorldSpawnSlotIsActive(uaddr base);
extern usize getVehicleWorldSpawnSlotActiveObjects(uaddr base);
extern uaddr getVehicleWorldSpawnSlotAddress(uint32 slot);
extern bool isCustomParkedVehicleSpawnViaSlot(uint32 slot);
extern uaddr createParkedVehicleSpawnViaSlot(uint32 slot, uint16 id, float x, float y, float z, float deg, int16 color1, int16 color2, bool alarm, int16 door_lock);
extern uaddr createParkedVehicleSpawnViaReplaceID(uint16 replace_id UNUSED, uint16 id UNUSED, float x UNUSED, float y UNUSED, float z UNUSED, float deg UNUSED, int16 color1 UNUSED, int16 color2 UNUSED, bool alarm UNUSED, int16 door_lock UNUSED);

/// Garage
extern bool getGarageVehicleSlotIsActive(uaddr base);
extern usize getGarageVehicleActiveObjects(uaddr base);
extern void createGarageVehicle(uaddr garage_base_adr, uint16 id, float x, float y, float z, float vec1, float vec2, float vec3, uint8 flags1, uint8 flags2, uint8 radio, uint8 var1, uint8 var2);

extern int8 getIdeType(array_uidx id);
extern array_idx getFirstIdeOfType(int8 type);
extern int getLastIdeOfType(int8 type);
extern u8string getIdeTypeName(uint8 no);
extern uaddr getAddressOfIdeSlotForID(array_uidx id);
extern usize getIdeSlotSizeByType(uint8 no);

extern u8string getModelNameViaHash(uint32 hash, uint32 time);
extern u8string getModelNameViaID(uint32 id, uint32 time);
extern u8string getGxtTranslation(u8string identifier);
extern u8string getRealVehicleNameViaID(uint16 id);
extern u8string getGxtIdentifierForVehicleViaID(uint16 id);
extern u8string vcs_getNameForPedViaID(uint16 id);

extern u8string getRadioStationName(uint32 no);

extern u8string removeColor(u8string string);

/// Camera
extern void setCameraCenterBehindPlayer();
extern void setFieldOfView(float fov);
extern float getFieldOfView();

extern void setPedSkin(int ped_handle, const u8string name);
extern usize removeIPLObjectsCollision(uint16 id);

extern uaddr getParticleOffsetFor(const u8string name);
extern u8string getPedstatName(uint32 no);


/// SCM
extern void CustomScriptClear(uaddr address, usize size);
extern void CustomScriptPlace(const byte *script, uaddr address, usize size);
extern void CustomScriptExecute(uaddr address);

extern int getScriptGlobalValue(uint32 var);

#endif
