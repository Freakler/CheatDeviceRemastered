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

#include <systemctrl.h>
#include <pspctrl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef NAMERESOLV
#include "minIni.h"
#endif

#include "main.h" // MEMCHECK, NAMERESOLV etc
#include "putils.h"
#include "cheats.h" // for using hooked functions
#include "functions.h"
#include "blitn.h"
#include "logs.h"
#include "storiesrwsdk.h"

register uaddr gp __asm__("gp");

extern usize mod_text_size; // for ulux fix

/// externs from main.c
extern bool LCS, VCS;

/// externs from cheats.c
extern uaddr pplayer;
extern uaddr pcar;
extern uaddr pobj;
extern usize peds_max;
extern uaddr peds_base;
extern uaddr var_pedobjsize;
extern usize vehicles_max;
extern uaddr vehicles_base;
extern usize var_vehobjsize;
extern usize worldobjs_max;
extern uaddr worldobjs_base;
extern usize var_wldobjsize;
extern uaddr global_weather;
extern uaddr global_gametimer;
extern uaddr global_maxhealthmult;
extern uaddr global_maxarmormult;
extern uaddr global_unlimtedsprint;
extern uaddr global_unlimtedswim;
extern uaddr global_ismultiplayer;
extern uaddr global_clockmultiplier;
extern uaddr global_displaysettings;
extern uaddr global_radarbool;
extern uaddr global_hudbool;
extern uaddr global_camera;
extern uaddr global_timescale;
extern uaddr global_maplegendbool;
extern uaddr global_freezegame;
extern uaddr global_helpbox;
extern uaddr global_helpbox_string;
extern uaddr global_helpbox_timedisplayed;
extern uaddr global_helpbox_displaybool;
extern uaddr global_helpbox_permboxbool;
extern uaddr global_helpbox_duration;
extern uaddr global_dialog_string;
extern uaddr global_hudincutscene;
extern uaddr global_currentisland;
extern uaddr global_systemlanguage;
extern uaddr global_ptr_water;
extern uaddr global_radioarea;
extern uaddr global_buttoninput;
extern uaddr global_pickups;
extern uaddr var_pickupslots;
extern usize var_pickupslotsize;
extern uaddr var_radarblipslots;
extern usize var_radarblipslotsize;
extern usize var_radarblipspadding;
extern uaddr global_radarblips;
extern uaddr ptr_handlingCFG;
extern usize var_handlingcfgslotsize;
extern uaddr buildingsIPL_base;
extern usize buildingsIPL_max;
extern usize var_buildingsIPLslotsize;
extern uaddr treadablesIPL_base;
extern usize treadablesIPL_max;
extern usize var_treadablesIPLslotsize;
extern uaddr ptr_particleCFG;
extern usize var_particleCFGslots;
extern usize var_particleCFGslotsize;
extern uaddr ptr_pedstatTable;
extern usize var_pedstatDATslots;
extern usize var_pedstatDATslotsize;
extern uaddr addr_vehiclesworldspawn;
extern usize var_vehiclesworldspawnslots;
extern usize var_vehiclesworldspawnslotsize;
extern usize var_garageslots;
extern usize var_garageslotsize;
extern uaddr ptr_IDEs;
extern uaddr ptr_IDETable;
extern uaddr ptr_gxtloadadr;
extern uaddr ptr_radarIconList;
extern uaddr var_radios;
extern uaddr addr_heliheight;
extern uaddr global_bmxjumpmult;
#ifdef PREVIEW
extern uaddr addr_policechaseheli_1;
extern uaddr addr_policechaseheli_2;
extern uaddr addr_policechaseheli_3;
#endif
extern uaddr global_WindClipped;
extern uaddr global_Wind;
extern uaddr addr_randompedcheat;


/// Buttons for cheats
const byte L        = 0x31;
const byte R        = 0x32;
const byte UP       = 0x55;
const byte DOWN     = 0x44;
const byte LEFT     = 0x4C;
const byte RIGHT    = 0x52;
const byte CROSS    = 0x58;
const byte CIRCLE   = 0x43;
const byte SQUARE   = 0x53;
const byte TRIANGLE = 0x54;
const byte NONE     = 0x20;
uint32 key_to_pad; // for custom button trigger (only cheats currently)


const u8string lcs_garagenames[] = { "Portland", "Staunton Island", "Shoreside Vale" };
const u8string vcs_garagenames[] = { "101 Bayshore Avenue", "The Compound", "Clymenus Suite" };

const u8string weather_vcs[] = { "Sunny", "Cloudy", "Rainy", "Foggy", "Extra Sunny", "Hurricane", "Extra Colours", "Ultra Sunny", "8", "9", "10", "11", "12", "13", "14", "15"};
const u8string weather_lcs[] = { "Sunny", "Cloudy", "Rainy", "Foggy", "Extra Sunny", "Hurricane", "Extra Colours", "Snow", "8", "9", "10", "11", "12", "13", "14", "15"};


const GTAWorldLocation lcs_teleports[] = { // z value + 0.20
  {"",                                  {     0.f,      0.f,     0.f } }, // fix for config (otherwise first is 0 and wont be saved/loaded)
  {"Portland Ferry Dock",               {   728.f,    175.f,     4.f } },
  {"Portland Car Crusher",              {  1133.f,     32.f,     1.f } },
  {"Portland Head Radio",               {   982.f,    -47.f,     8.f } },
  {"Portland Sal's House",              {  1420.f,   -187.f,    50.f } },
  {"Portland Safehouse",                {  1155.f,   -257.f,    18.f } },
  {"Portland Claude Speed's Hideout",   {   859.f,   -302.f,     8.f } },
  {"Portland Ma's House",               {  1210.f,   -330.f,    26.f } },
  {"Portland Pay 'n' Spray",            {   926.f,   -341.f,    10.f } },
  {"Portland Lighthouse",               {  1645.f,   -364.f,    33.f } },
  {"Portland Ammunation",               {  1063.f,   -395.f,    16.f } },
  {"Portland JD's Revue Club",          {   910.f,   -417.f,    15.f } },
  {"Portland Marco's Bistro",           {  1347.f,   -456.f,    50.f } },
  {"Portland Maria's Apartment",        {  1118.f,   -485.f,    20.f } },
  {"Portland Hospital",                 {  1145.f,   -590.f,    15.f } },
  {"Portland Police Station",           {  1139.f,   -668.f,    15.f } },
  {"Portland Import/Export Dock",       {  1571.f,   -697.f,    12.f } },
  {"Portland Callahan Bridge",          {  1050.f,   -941.f,    15.f } },
  {"Portland Debug Spawn Point",        {  1250.f,  -1161.f,    12.f } }, // +
  {"Portland Vincenzo's Office",        {  1115.f,  -1107.f,    14.f } },
  {"Staunton Ferry Dock",               {   193.f,    227.f,    12.f } },
  {"Staunton Hospital",                 {   195.f,    -21.f,    17.f } },
  {"Staunton Tunnel Under Construction",{   340.f,    -90.f,   -12.f } },
  {"Staunton Liberty Campus",           {   174.f,   -132.f,    17.f } },
  {"Staunton Leon McAffrey",            {  -168.f,   -313.f,     4.f } },
  {"Staunton Fire Station",             {   -86.f,   -435.f,    17.f } },
  {"Staunton Safehouse",                {   292.f,   -440.f,    27.f } }, // default
  {"Staunton Pay 'n' Spray",            {   368.f,   -493.f,    27.f } },
  {"Staunton Ammunation",               {   351.f,   -707.f,    27.f } },
  {"Staunton Belleville Park",          {    68.f,   -772.f,    29.f } },
  {"Staunton Really High",              {    68.f,   -772.f,   900.f } },
  {"Staunton Police Station",           {   343.f,  -1153.f,  23.20f } },
  {"Staunton Police Station Backlot",   {   342.f,  -1064.f,  26.20f } },
  {"Staunton Helicopter landing pad",   {   335.f,  -1098.f,  76.50f } }, // +
  {"Staunton Church",                   {    22.f,  -1130.f,  26.34f } },
  {"Staunton Phil's Gun Shop",          {   179.f,  -1259.f,  26.37f } },
  {"Staunton V8 Ghost Spawn",           {    63.f,  -1316.f,    27.f } }, // +
  {"Staunton Sal's Payphone",           {   196.f,  -1417.f,  26.37f } },
  {"Staunton Love Media Building",      {   213.f,  -1497.f,  27.60f } },
  {"Staunton Top of Tall Building",     {    95.f,  -1509.f,   217.f } }, // +
  {"SSV Cochrane Dam",                  { -1145.f,    485.f,  68.42f } },
  {"SSV Cochrane Dam Lower",            { -1052.f,    465.f,  23.81f } },
  {"SSV Donald Love's House",           {  -365.f,    241.f,  61.10f } },
  {"SSV Safehouse",                     {  -825.f,    304.f,  40.22f } },
  {"SSV Sindacco Hideout Near Dam",     { -1092.f,    248.f,     4.f } },
  {"SSV Pay 'n' Spray",                 { -1133.f,     34.f,    59.f } },
  {"SSV Claude Speed's Hideout",        {  -651.f,    -36.f,    19.f } },
  {"SSV Wichita Gardens",               {  -417.f,    -34.f,     4.f } },
  {"SSV Police Station",                { -1264.f,    -40.f,    59.f } },
  {"SSV Hospital",                      { -1260.f,    -79.f,    59.f } },
  {"SSV Flophouse",                     { -1040.f,   -182.f,    34.f } },
  {"SSV Airport Terminal",              {  -742.f,   -468.f,     9.f } },
  {"SSV See The Sight Booth",           {  -646.f,   -746.f,    19.f } },
  {"SSV Wing Surfing Location",         { -1343.f,   -852.f,  18.70f } },
  {"SSV Airport Runway",                { -1422.f,  -1012.f,  11.00f } }, // original: -1585,   -936,   11.30

}; const array_size lcs_teleportersize = ARRAY_SIZE(lcs_teleports) - 1;


const GTAWorldLocation vcs_teleports[] = { // z value + 0.20
  {"",                                  {     0.f,      0.f,     0.f } }, // fix for config (otherwise first is 0 and wont be saved/loaded)
  {"Downtown Ammu-Nation",              {  -679.f,   1195.f,    12.f } },
  {"Downtown Hospital",                 {  -822.f,   1162.f,    12.f } },
  {"Hyman Memorial Stadium",            { -1044.f,   1313.f,     9.f } },
  {"Hyman Memorial Stadium Stage",      { -1569.f,   1335.f,  -228.f } }, // +
  {"8 Ball's Bomb Shop",                {  -909.f,   1116.f,    12.f } },
  {"Lance's Hotel",                     {  -842.f,   1038.f,    16.f } },
  {"Inside Skyscraper",                 {  -526.f,    949.f,    69.f } }, // +
  {"Highest Solid Ground",              {  -338.f,   1262.f,   304.f } }, // +
  {"Downtown King Knuts",               {  -878.f,    796.f,    12.f } },
  {"Downtown Police Helipad",           {  -614.f,    778.f,    24.f } },
  {"Moist Palms Hotel",                 {  -675.f,    560.f,    12.f } },
  {"Skimmer",                           {  -550.f,    568.f,     9.f } },
  {"Compound Safehouse",                { -1087.f,    356.f,    12.f } },
  {"White Stallionz Bar",               { -1087.f,    113.f,    12.f } },
  {"Garbage Dump",                      { -1202.f,      8.f,    12.f } },
  {"Little Haiti Pay 'n' Spray",        {  -864.f,   -122.f,    12.f } },
  {"Mary-Jo's Apartment",               {  -866.f,   -267.f,    11.f } },
  {"Print Works",                       { -1043.f,   -268.f,    12.f } },
  {"Umberto's House",                   { -1106.f,   -424.f,    11.f } },
  {"Little Havana Hospital",            {  -870.f,   -470.f,    12.f } },
  {"Starfish Island",                   {  -446.f,   -485.f,    12.f } },
  {"Diaz's Mansion",                    {  -359.f,   -525.f,    13.f } },
  {"Stonewalls J's",                    { -1156.f,   -694.f,    12.f } },
  {"Sunshine Autos",                    { -1085.f,   -768.f,    12.f } },
  {"Marty's Trailer",                   {  -947.f,   -864.f,    13.f } },
  {"Debug Spawn Point",                 {  -974.f,  -1104.f,    15.f } }, // +
  {"101 Bayshore Ave Safehouse",        {  -806.f,  -1185.f,    12.f } }, // default
  {"Viceport Pay 'n' Spray",            {  -913.f,  -1273.f,    12.f } },
  {"Phil's Depot",                      {  -710.f,  -1553.f,    13.f } },
  {"Fort Baxter",                       { -1717.f,   -324.f,    15.f } },
  {"Airport Terminal",                  { -1401.f,   -793.f,    15.f } },
  {"Airport Terminal inside",           { -1422.f,   -852.f,    15.f } },
  {"Airport Runway 18R",                { -1735.f,   -731.f,    15.f } },
  {"Airport Runway 22",                 { -1266.f,  -1077.f,    15.f } },
  {"Airport Runway 36L",                { -1735.f,  -1736.f,    15.f } },
  {"Prawn Island",                      {    35.f,   1028.f,    11.f } },
  {"Inside Mendez's Mansion",           {    17.f,   1153.f,  -193.f } }, // +
  {"Interglobal Films",                 {   -31.f,    953.f,    11.f } },
  {"Vice Point Mall",                   {   492.f,   1121.f,    17.f } },
  {"Gonzalez's Apartment",              {   276.f,    756.f,    12.f } },
  {"Vice Point Hospital",               {   430.f,    706.f,    12.f } },
  {"Vice Point Pay 'n' Spray",          {   334.f,    428.f,    12.f } },
  {"Leaf Links Golf Course",            {   109.f,    247.f,    22.f } },
  {"Electric Boulevard",                {   322.f,    201.f,    12.f } },
  {"Malibu Club",                       {   506.f,    -96.f,    11.f } },
  {"The Clymenus Suite Safehouse",      {   268.f,   -174.f,    12.f } },
  {"Washington Beach Police Dept",      {   416.f,   -428.f,    11.f } },
  {"Ocean View Hospital",               {  -101.f,   -975.f,    11.f } },
  {"Washington Beach",                  {   488.f,  -1002.f,    12.f } },
  {"Ocean Beach Pay 'n' Spray",         {   -18.f,  -1257.f,    11.f } },
  {"Ocean View Hotel",                  {   259.f,  -1285.f,    12.f } },
  {"Ocean Beach Ammunation",            {   -53.f,  -1480.f,    11.f } },
  {"Lance's House",                     {   -84.f,  -1571.f,    11.f } },

}; const array_size vcs_teleportersize = ARRAY_SIZE(vcs_teleports) - 1;

/* const radio_pack lcs_radiostations[] = {
  {"Head Radio",      0x00}, // FEA_FM0
  {"Double Cleff FM", 0x01}, // FEA_FM1
  {"K-Jah",           0x02}, // ..
  {"Rise FM",         0x03},
  {"Lips 106",        0x04},
  {"Radio del Mundo", 0x05},
  {"MSX98",           0x06},
  {"Flashback FM",    0x07},
  {"The Liberty Jam", 0x08},
  {"LCFR",            0x09},
  {"Radio Off",       0x0A}, // FEA_NON

}; static const array_size lcs_radiosize = ARRAY_SIZE(lcs_radiostations)-1;

const radio_pack vcs_radiostations[] = {
  {"Flash FM",       0x00}, // FEA_FM0
  {"V Rock",         0x01}, // FEA_FM1
  {"Paradise",       0x02}, // ..
  {"VCPR",           0x03},
  {"VCFL Radio",     0x04},
  {"The Wave 103",   0x05},
  {"Fresh 105 FM",   0x06},
  {"Espantoso",      0x07},
  {"Emotion 98.3",   0x08},
  {"Radio Off",      0x09}, // FEA_NON

}; static const array_size vcs_radiosize = ARRAY_SIZE(vcs_radiostations)-1; */

/* const GTAVehicle lcs_vehicles[] = { //https://www.gtamodding.com/wiki/List_of_vehicles_(LCS)
  ///id   //type        //GXTname    //name      //hndlng_no   //spawnsave
  {0x82,  VEHICLE_CAR,   "SPIDER",   "Deimos SP",        57,   true  }, //130
  {0x83,  VEHICLE_CAR,   "LANDSTK",  "Landstalker",       0,   true  },
  {0x84,  VEHICLE_CAR,   "IDAHO",    "Idaho",             1,   true  },
  {0x85,  VEHICLE_CAR,   "STINGER",  "Stinger",           2,   true  },
  {0x86,  VEHICLE_CAR,   "LINERUN",  "Linerunner",        3,   true  },
  {0x87,  VEHICLE_CAR,   "PEREN",    "Perennial",         4,   true  },
  {0x88,  VEHICLE_CAR,   "SENTINL",  "Sentinel",          5,   true  },
  {0x89,  VEHICLE_CAR,   "PATRIOT",  "Patriot",           6,   true  },
  {0x8A,  VEHICLE_CAR,   "FIRETRK",  "Firetruck",         7,   true  },
  {0x8B,  VEHICLE_CAR,   "TRASHM",   "Trashmaster",       8,   true  },
  {0x8C,  VEHICLE_CAR,   "STRECH",   "Stretch",           9,   true  },
  {0x8D,  VEHICLE_CAR,   "MANANA",   "Manana",           10,   true  },
  {0x8E,  VEHICLE_CAR,   "INFERNS",  "Infernus",         11,   true  },
  {0x8F,  VEHICLE_CAR,   "BLISTA",   "Blista",           45,   true  },
  {0x90,  VEHICLE_CAR,   "PONY",     "Pony",             12,   true  },
  {0x91,  VEHICLE_CAR,   "MULE",     "Mule",             13,   true  },
  {0x92,  VEHICLE_CAR,   "CHEETAH",  "Cheetah",          14,   true  },
  {0x93,  VEHICLE_CAR,   "AMBULAN",  "Ambulance",        15,   true  },
  {0x94,  VEHICLE_CAR,   "FBICAR",   "FBI Cruiser",      16,   true  },
  {0x95,  VEHICLE_CAR,   "MOONBM",   "Moonbeam",         17,   true  },
  {0x96,  VEHICLE_CAR,   "ESPERAN",  "Esperanto",        18,   true  },
  {0x97,  VEHICLE_CAR,   "TAXI",     "Taxi",             19,   true  },
  {0x98,  VEHICLE_CAR,   "KURUMA",   "Kuruma",           20,   true  }, // 152
  {0x99,  VEHICLE_CAR,   "BOBCAT",   "Bobcat",           21,   true  },
  {0x9A,  VEHICLE_CAR,   "WHOOPEE",  "Mr Whoopee",       22,   true  },
  {0x9B,  VEHICLE_CAR,   "BFINJC",   "BF Injection ",    23,   true  },
  {0x9C,  VEHICLE_CAR,   "HEARSE",   "Hearse",            4,   true  }, // hndlng_no same as PEREN
  {0x9D,  VEHICLE_CAR,   "POLICAR",  "Police",           24,   true  },
  {0x9E,  VEHICLE_CAR,   "ENFORCR",  "Enforcer",         25,   true  },
  {0x9F,  VEHICLE_CAR,   "SECURI",   "Securicar",        26,   true  },
  {0xA0,  VEHICLE_CAR,   "BANSHEE",  "Banshee",          27,   true  },
  {0xA1,  VEHICLE_CAR,   "BUS",      "Bus",              28,   true  },
  {0xA2,  VEHICLE_CAR,   "RHINO",    "Rhino",            29,   true  },
  {0xA3,  VEHICLE_CAR,   "BARRCKS",  "Barracks OL",      30,   true  },
  {0xA4,  VEHICLE_PLANE, "DODO",     "Dodo",             34,   true  },
  {0xA5,  VEHICLE_CAR,   "COACH",    "Coach",            35,   true  },
  {0xA6,  VEHICLE_CAR,   "CABBIE",   "Cabbie",           36,   true  },
  {0xA7,  VEHICLE_CAR,   "STALION",  "Stallion",         37,   true  },
  {0xA8,  VEHICLE_CAR,   "RUMPO",    "Rumpo",            38,   true  },
  {0xA9,  VEHICLE_CAR,   "RCBANDT",  "RC Bandit",        39,   true  },
  {0xAA,  VEHICLE_CAR,   "BELLYUP",  "Triad Fish Van",   46,   true  },
  {0xAB,  VEHICLE_CAR,   "MRWONGS",  "Mr Wongs",         47,   true  },
  {0xAC,  VEHICLE_CAR,   "MAFIACR",  "Leone Sentinel",   40,   true  }, // 172
  {0xAD,  VEHICLE_CAR,   "YARDICR",  "Yardie Lobo",      48,   true  },
  {0xAE,  VEHICLE_CAR,   "YAKUZCR",  "Yakuza Stinger",   49,   true  },
  {0xAF,  VEHICLE_CAR,   "DIABLCR",  "Diablo Stallion",  50,   true  },
  {0xB0,  VEHICLE_CAR,   "COLOMCR",  "Cartel Cruiser",   51,   true  },
  {0xB1,  VEHICLE_CAR,   "HOODSCR",  "Hoods Rumpo XL",   52,   true  },
  {0xB2,  VEHICLE_CAR,   "PANLANT",  "Panlantic",        53,   true  },
  {0xB3,  VEHICLE_CAR,   "FLATBED",  "Flatbed",          43,   true  },
  {0xB4,  VEHICLE_CAR,   "YANKEE",   "Yankee",           44,   true  },
  {0xB5,  VEHICLE_CAR,   "BORGNIN",  "Bickle'76",        54,   true  },
  {0xB6,  VEHICLE_CAR,   "TOYZ",     "TOYZ",             12,   true  }, // hndlng_no same as PONY
  {0xB7,  VEHICLE_CAR,   "CAMPVAN",  "Campaign Rumpo",   55,   true  },
  {0xB8,  VEHICLE_CAR,   "BALLOT",   "Ballot Van ",      56,   true  },
  {0xB9,  VEHICLE_CAR,   "SHELBY",   "Hellenbach GT",    58,   true  },
  {0xBA,  VEHICLE_CAR,   "PONTIAC",  "Phobos Vt",        59,   true  },
  {0xBB,  VEHICLE_CAR,   "ESPRIT",   "V8 Ghost",         60,   true  },
  {0xBC,  VEHICLE_CAR,   "BARRACK",  "Barracks OL",      61,   true  },
  {0xBD,  VEHICLE_CAR,   "HOTROD",   "Thunder-Rodd",     62,   true  },
  {0xBE,  VEHICLE_CAR,   "SINDACO",  "Sindacco Argento", 63,   true  },
  {0xBF,  VEHICLE_CAR,   "FORELLI",  "Forelli ExSess",   64,   true  },
  {0xC0,  VEHICLE_FERRY, "FERRY",    "Ferry",            32,   false },
  {0xC1,  VEHICLE_BOAT,  "GHOST",    "Ghost",            -1,   false }, // boat without texture
  {0xC2,  VEHICLE_BOAT,  "SPEEDER",  "Speeder",          73,   true  },
  {0xC3,  VEHICLE_BOAT,  "REEFER",   "Reefer",           74,   true  },
  {0xC4,  VEHICLE_BOAT,  "PREDATR",  "Predator",         72,   true  },
  {0xC5,  VEHICLE_TRAIN, "TRAIN",    "Train",            31,   false },
  {0xC6,  VEHICLE_HELI,  "HELI",     "Helicopter",       -1,   false }, // "escape"
  {0xC7,  VEHICLE_HELI,  "HELI",     "Helicopter",       33,   true  }, // "chopper"    (aka: tiny invisible helicopter)
  {0xC8,  VEHICLE_PLANE, "AEROPL",   "Aeroplane",        41,   false },
  {0xC9,  VEHICLE_PLANE, "DODO",     "Dodo",             42,   false }, // 201
  {0xCA,  VEHICLE_BIKE,  "ANGEL",    "Angel",            68,   true  },
  {0xCB,  VEHICLE_BIKE,  "PIZZABO",  "Pizzaboy",         66,   true  }, // hndlng_no same as FAGGIO
  {0xCC,  VEHICLE_BIKE,  "NOODLBO",  "Noodleboy",        66,   true  }, // hndlng_no same as FAGGIO
  {0xCD,  VEHICLE_BIKE,  "PCJ600",   "PCJ-600",          65,   true  }, // 205
  {0xCE,  VEHICLE_BIKE,  "FAGGIO",   "Faggio",           66,   true  },
  {0xCF,  VEHICLE_BIKE,  "FREEWAY",  "Freeway",          71,   true  },
  {0xD0,  VEHICLE_BIKE,  "ANGEL2",   "Avenger",          70,   true  },
  {0xD1,  VEHICLE_BIKE,  "SANCH2",   "Manchez",          69,   true  },
  {0xD2,  VEHICLE_BIKE,  "SANCHEZ",  "Sanchez",          67,   true  },
  {0xD3,  VEHICLE_HELI,  "RCGOBLI",  "RC Goblin",        79,   true  },
  {0xD4,  VEHICLE_HELI,  "RCRAIDE",  "RC Raider",        80,   true  },
  {0xD5,  VEHICLE_HELI,  "HUNTER",   "Hunter",           78,   true  },
  {0xD6,  VEHICLE_HELI,  "MAVERIC",  "Maverick",         75,   true  },
  {0xD7,  VEHICLE_HELI,  "POLMAV",   "Police Maverick",  77,   true  },
  {0xD8,  VEHICLE_HELI,  "VCNMAV",   "VCN Maverick",     76,   true  },

}; static const array_size lcs_vehiclessize = ARRAY_SIZE(lcs_vehicles)-1;


const GTAVehicle vcs_vehicles[] = { //https://www.youtube.com/watch?v=7l3-B_kvLHg
  ///id  //type           //GXTname    //name            //hndlng_no   //spawnsave
  {0xAA,  VEHICLE_CAR,     "UNKNOWN",    "Split-6 ATV",      -1,   true  },
  {0xAB,  VEHICLE_CAR,     "UNKNOWN",    "Admiral",          -1,   true  },
  {0xAC,  VEHICLE_CAR,     "UNKNOWN",    "Cheetah",          -1,   true  },
  {0xAD,  VEHICLE_HELI,    "UNKNOWN",    "Little Willie",    -1,   true  },
  {0xAE,  VEHICLE_CAR,     "UNKNOWN",    "Baggage Handler",  -1,   true  },
  {0xAF,  VEHICLE_CAR,     "UNKNOWN",    "Banshee",          -1,   true  },
  {0xB0,  VEHICLE_CAR,     "UNKNOWN",    "Perennial",        -1,   true  },
  {0xB1,  VEHICLE_CAR,     "UNKNOWN",    "Blista Compact",   -1,   true  },
  {0xB2,  VEHICLE_BICYCLE, "UNKNOWN",    "BMX",              -1,   true  },
  {0xB3,  VEHICLE_BICYCLE, "UNKNOWN",    "BMX",              -1,   true  },
  {0xB4,  VEHICLE_CAR,     "UNKNOWN",    "Bobcat",           -1,   true  },
  {0xB5,  VEHICLE_CAR,     "UNKNOWN",    "Bulldoger",        -1,   true  },
  {0xB6,  VEHICLE_CAR,     "UNKNOWN",    "Burrito",          -1,   true  },
  {0xB7,  VEHICLE_CAR,     "UNKNOWN",    "Cabbie",           -1,   true  },
  {0xB8,  VEHICLE_CAR,     "UNKNOWN",    "Caddy",            -1,   true  },
  {0xB9,  VEHICLE_BOAT,    "UNKNOWN",    "???",              -1,   true  },
  {0xBA,  VEHICLE_CAR,     "UNKNOWN",    "Polaris V8",       -1,   true  },
  {0xBB,  VEHICLE_CAR,     "UNKNOWN",    "Deluxo",           -1,   true  },
  {0xBC,  VEHICLE_HELI,    "UNKNOWN",    "Armadillo",        -1,   true  },
  {0xBD,  VEHICLE_HELI,    "UNKNOWN",    "Air Ambulance",    -1,   true  },
  {0xBE,  VEHICLE_BIKE,    "UNKNOWN",    "WinterGreen",      -1,   true  },
  {0xBF,  VEHICLE_BIKE,    "UNKNOWN",    "VCPD WinterGreen", -1,   true  },
  {0xC0,  VEHICLE_CAR,     "UNKNOWN",    "Esperanto",        -1,   true  },
  {0xC1,  VEHICLE_CAR,     "UNKNOWN",    "FBI Washington",   -1,   true  },
  {0xC2,  VEHICLE_CAR,     "UNKNOWN",    "Fire Truck",       -1,   true  },
  {0xC3,  VEHICLE_CAR,     "UNKNOWN",    "Glendale",         -1,   true  },
  {0xC4,  VEHICLE_CAR,     "UNKNOWN",    "Greenwood",        -1,   true  },
  {0xC5,  VEHICLE_CAR,     "UNKNOWN",    "Hermes",           -1,   true  },
  {0xC6,  VEHICLE_CAR,     "UNKNOWN",    "Borres '64",       -1,   true  }, // amphycar
  {0xC7,  VEHICLE_CAR,     "UNKNOWN",    "Idaho",            -1,   true  },
  {0xC8,  VEHICLE_CAR,     "UNKNOWN",    "Landstalker",      -1,   true  },
  {0xC9,  VEHICLE_CAR,     "UNKNOWN",    "Manana",           -1,   true  },
  {0xCA,  VEHICLE_BIKE,    "UNKNOWN",    "Ventoso",          -1,   true  },
  {0xCB,  VEHICLE_CAR,     "UNKNOWN",    "Oceanic",          -1,   true  },
  {0xCC,  VEHICLE_CAR,     "UNKNOWN",    "VCPD Cheetah",     -1,   true  },
  {0xCD,  VEHICLE_BIKE,    "UNKNOWN",    "Sanchez",          -1,   true  },
  {0xCE,  VEHICLE_CAR,     "UNKNOWN",    "Stallion",         -1,   true  },
  {0xCF,  VEHICLE_CAR,     "UNKNOWN",    "VCPD Cruiser",     -1,   true  },
  {0xD0,  VEHICLE_BIKE,    "UNKNOWN",    "Domestobot",       -1,   true  }, // robot from mission
  {0xD1,  VEHICLE_CAR,     "UNKNOWN",    "Patriot",          -1,   true  },
  {0xD2,  VEHICLE_CAR,     "UNKNOWN",    "Pony",             -1,   true  },
  {0xD3,  VEHICLE_CAR,     "UNKNOWN",    "Sentinel",         -1,   true  },
  {0xD4,  VEHICLE_BIKE,    "UNKNOWN",    "PCJ-600",          -1,   true  },
  {0xD5,  VEHICLE_HELI,    "UNKNOWN",    "Maverick",         -1,   true  },
  {0xD6,  VEHICLE_BOAT,    "UNKNOWN",    "Reefer",           -1,   true  },
  {0xD7,  VEHICLE_BOAT,    "UNKNOWN",    "Speeder",          -1,   true  },
  {0xD8,  VEHICLE_CAR,     "UNKNOWN",    "Linerunner",       -1,   true  },
  {0xD9,  VEHICLE_CAR,     "UNKNOWN",    "Walton",           -1,   true  },
  {0xDA,  VEHICLE_CAR,     "UNKNOWN",    "Barracks OL",      -1,   true  },
  {0xDB,  VEHICLE_BOAT,    "UNKNOWN",    "Predator",         -1,   true  },
  {0xDC,  VEHICLE_CAR,     "UNKNOWN",    "Flatbed",          -1,   true  },
  {0xDD,  VEHICLE_CAR,     "UNKNOWN",    "Barracks OL",      -1,   true  },
  {0xDE,  VEHICLE_PLANE,   "UNKNOWN",    "Biplane",          -1,   true  },
  {0xDF,  VEHICLE_CAR,     "UNKNOWN",    "Moonbeam",         -1,   true  },
  {0xE0,  VEHICLE_CAR,     "UNKNOWN",    "Rumpo",            -1,   true  },
  {0xE1,  VEHICLE_BOAT,    "UNKNOWN",    "Yola",             -1,   true  },
  {0xE2,  VEHICLE_CAR,     "UNKNOWN",    "Taxi",             -1,   true  },
  {0xE3,  VEHICLE_CAR,     "UNKNOWN",    "Ambulance",        -1,   true  },
  {0xE4,  VEHICLE_CAR,     "UNKNOWN",    "Strech",           -1,   true  },
  {0xE5,  VEHICLE_BIKE,    "UNKNOWN",    "Bike",             -1,   true  },
  {0xE6,  VEHICLE_CAR,     "UNKNOWN",    "Quad",             -1,   true  }, // quadbike
  {0xE7,  VEHICLE_CAR,     "UNKNOWN",    "Biker Angel",      -1,   true  },
  {0xE8,  VEHICLE_CAR,     "UNKNOWN",    "Freeway",          -1,   true  },
  {0xE9,  VEHICLE_BOAT,    "UNKNOWN",    "JetSki",           -1,   true  }, // jetski
  {0xEA,  VEHICLE_CAR,     "UNKNOWN",    "VCPD Enforcer",    -1,   true  },
  {0xEB,  VEHICLE_CAR,     "UNKNOWN",    "Boxville",         -1,   true  },
  {0xEC,  VEHICLE_CAR,     "UNKNOWN",    "Benson",           -1,   true  },
  {0xED,  VEHICLE_CAR,     "UNKNOWN",    "Coach",            -1,   true  },
  {0xEE,  VEHICLE_CAR,     "UNKNOWN",    "Mule",             -1,   true  },
  {0xEF,  VEHICLE_CAR,     "UNKNOWN",    "Voodoo",           -1,   true  },
  {0xF0,  VEHICLE_CAR,     "UNKNOWN",    "Securicar",        -1,   true  },
  {0xF1,  VEHICLE_CAR,     "UNKNOWN",    "Trashmaster",      -1,   true  },
  {0xF2,  VEHICLE_NONE,    "",           "",                 -1,   false }, // 242 ?????
  {0xF3,  VEHICLE_CAR,     "UNKNOWN",    "Yankee",           -1,   true  },
  {0xF4,  VEHICLE_CAR,     "UNKNOWN",    "Mr. Whoopee",      -1,   true  },
  {0xF5,  VEHICLE_CAR,     "UNKNOWN",    "Sandking",         -1,   true  },
  {0xF6,  VEHICLE_CAR,     "UNKNOWN",    "Rhino",            -1,   true  },
  {0xF7,  VEHICLE_BOAT,    "UNKNOWN",    "Dingley",          -1,   true  },
  {0xF8,  VEHICLE_BOAT,    "UNKNOWN",    "Marquis",          -1,   true  },
  {0xF9,  VEHICLE_BOAT,    "UNKNOWN",    "Rio",              -1,   true  },
  {0xFA,  VEHICLE_BOAT,    "UNKNOWN",    "Tropic",           -1,   true  },
  {0xFB,  VEHICLE_CAR,     "UNKNOWN",    "Forklift",         -1,   true  },
  {0xFC,  VEHICLE_BIKE,    "UNKNOWN",    "Streetfighter",    -1,   true  },
  {0xFD,  VEHICLE_CAR,     "UNKNOWN",    "Virgo",            -1,   true  },
  {0xFE,  VEHICLE_CAR,     "UNKNOWN",    "Stinger",          -1,   true  },
  {0xFF,  VEHICLE_CAR,     "UNKNOWN",    "BF Injection",     -1,   true  },
  {0x100, VEHICLE_CAR,     "UNKNOWN",    "Phoenix",          -1,   true  },
  {0x101, VEHICLE_BOAT,    "UNKNOWN",    "Squallo",          -1,   true  },
  {0x102, VEHICLE_BOAT,    "UNKNOWN",    "Jetmax",           -1,   true  },
  {0x103, VEHICLE_CAR,     "UNKNOWN",    "Mesa Grande",      -1,   true  },
  {0x104, VEHICLE_HELI,    "UNKNOWN",    "VCN Maverick",     -1,   true  },
  {0x105, VEHICLE_HELI,    "UNKNOWN",    "VCPD Maverick",    -1,   true  },
  {0x106, VEHICLE_HELI,    "UNKNOWN",    "Sparrow",          -1,   true  },
  {0x107, VEHICLE_HELI,    "UNKNOWN",    "Sea Sparrow",      -1,   true  },
  {0x108, VEHICLE_BOAT,    "UNKNOWN",    "Violator",         -1,   true  },
  {0x109, VEHICLE_CAR,     "UNKNOWN",    "Cholo Sabre",      -1,   true  },
  {0x10A, VEHICLE_CAR,     "UNKNOWN",    "Comet",            -1,   true  },
  {0x10B, VEHICLE_CAR,     "UNKNOWN",    "Cuban Hermes",     -1,   true  },
  {0x10C, VEHICLE_CAR,     "UNKNOWN",    "FBI Rancher",      -1,   true  },
  {0x10D, VEHICLE_CAR,     "UNKNOWN",    "Gang Rancher",     -1,   true  },
  {0x10E, VEHICLE_CAR,     "UNKNOWN",    "Infernus",         -1,   true  },
  {0x10F, VEHICLE_CAR,     "UNKNOWN",    "Regina",           -1,   true  },
  {0x110, VEHICLE_CAR,     "UNKNOWN",    "Sabre",            -1,   true  },
  {0x111, VEHICLE_CAR,     "UNKNOWN",    "Sabre Turbo",      -1,   true  },
  {0x112, VEHICLE_CAR,     "UNKNOWN",    "Sentinel XS",      -1,   true  },
  {0x113, VEHICLE_HELI,    "UNKNOWN",    "Hunter",           -1,   true  },
  {0x114, VEHICLE_CAR,     "UNKNOWN",    "Washington",       -1,   true  },
  {0x115, VEHICLE_BOAT,    "UNKNOWN",    "Coast Guard",      -1,   true  },
  {0x116, VEHICLE_PLANE,   "UNKNOWN",    "Skimmer",          -1,   true  },
  {,  VEHICLE_HELI,        "UNKNOWN",    "VCPD Maverick",    -1,   true  },
  {,  VEHICLE_NONE,        "",           "",                 -1,   false },
  {,  VEHICLE_NONE,        "",           "",                 -1,   false },
  {,  VEHICLE_PLANE,       "UNKNOWN",    "Aeroplane",        -1,   false },

}; static const array_size vcs_vehiclessize = ARRAY_SIZE(vcs_vehicles)-1; */


/* const GTACharacter lcs_characters[] = {
  ///id  //model        //name            ///row  ///spawnsave
  {0x00,  "player",      "Toni Cipriani",   -1,    true }, // MPC_000
  {0x01,  "cop",         "Officer '69'",    -1,    true }, // MPC_031
  {0x02,  "swat",        "The Specialist",  -1,    true }, // MPC_032
  {0x03,  "fbi",         "Agent Ochre",     -1,    true }, // MPC_033
  {0x04,  "army",        "G.I. Jones",      -1,    true }, // MPC_034
  {0x05,  "medic",       "",                -1,    true },
  {0x06,  "fireman",     "Sam",             -1,    true }, // MPC_035
  {0x07,  "male01",      "Arden Long",      -1,    true }, // MPC_056
  {0x08,  "?",           "",                -1,    false },
  {0x09,  "taxi_d",      "Ekant Sing",      -1,    true }, // MPC_053
  {0x0A,  "pimp",        "Papi Love",        2,    true }, // MPC_036
  {0x0B,  "criminal01",  "",                13,    true },
  {0x0C,  "criminal02",  "Rick O’Shea",     14,    true }, // MPC_055
  {0x0D,  "male02",      "",                15,    true },
  {0x0E,  "male03",      "Randy Boner",     16,    true }, // MPC_051
  {0x0F,  "fatmale01",   "",                -1,    true },
  {0x10,  "fatmale02",   "",                17,    true },
  {0x11,  "female01",    "Anne Job",        18,    true }, // MPC_050
  {0x12,  "female02",    "",                19,    true },
  {0x13,  "female03",    "",                20,    true },
  {0x14,  "fatfemale01", "Ophelia Lardaz",  21,    true }, // MPC_043
  {0x15,  "fatfemale02", "",                22,    true },
  {0x16,  "prostitute",  "Lucy Lastic",     23,    true }, // MPC_037
  {0x17,  "prostitute2", "Mona Lott",       24,    true }, // MPC_059
  {0x18,  "p_man1",      "",                25,    true },
  {0x19,  "p_man2",      "",                26,    true },
  {0x1A,  "p_wom1",      "Ann Thrax",       27,    true }, // MPC_057
  {0x1B,  "p_wom2",      "",                28,    true },
  {0x1C,  "ct_man1",     "",                29,    true },
  {0x1D,  "ct_man2",     "Hugh Jazz",       30,    true }, // MPC_047
  {0x1E,  "ct_wom1",     "Mrs.Menalot",     31,    true }, // MPC_046
  {0x1F,  "ct_wom2",     "",                32,    true },
  {0x20,  "li_man1",     "",                33,    true },
  {0x21,  "li_man2",     "",                34,    true },
  {0x22,  "li_wom1",     "",                35,    true },
  {0x23,  "li_wom2",     "",                36,    true },
  {0x24,  "docker1",     "",                37,    true },
  {0x25,  "docker2",     "",                38,    true },
  {0x26,  "scum_man",    "Owen Monie",      39,    true }, // MPC_052
  {0x27,  "scum_wom",    "",                40,    true },
  {0x28,  "worker1",     "",                41,    true },
  {0x29,  "worker2",     "",                42,    true },
  {0x2A,  "b_man1",      "",                43,    true },
  {0x2B,  "b_man2",      "",                44,    true },
  {0x2C,  "b_man3",      "",                -1,    true },
  {0x2D,  "b_wom1",      "Fay Slift",       45,    true }, // MPC_058
  {0x2E,  "b_wom2",      "",                46,    true },
  {0x2F,  "b_wom3",      "",                -1,    true },
  {0x30,  "mod_man",     "",                47,    true },
  {0x31,  "mod_wom",     "",                48,    true },
  {0x32,  "st_man",      "",                49,    true },
  {0x33,  "st_wom",      "",                50,    true },
  {0x34,  "fan_man1",    "",                51,    true },
  {0x35,  "fan_man2",    "",                52,    true },
  {0x36,  "fan_wom",     "",                -1,    true },
  {0x37,  "hos_man",     "",                -1,    true },
  {0x38,  "hos_wom",     "",                -1,    true },
  {0x39,  "const1",      "Bill Durzbum",    -1,    true }, // MPC_038
  {0x3A,  "const2",      "",                -1,    true },
  {0x3B,  "shopper1",    "",                -1,    true },
  {0x3C,  "shopper2",    "",                -1,    true },
  {0x3D,  "shopper3",    "",                60,    true },
  {0x3E,  "stud_man",    "",                61,    true },
  {0x3F,  "stud_wom",    "",                62,    true },
  {0x40,  "cas_man",     "",                63,    true },
  {0x41,  "cas_wom",     "",                64,    true },
  {0x42,  "camp_man",    "",                -1,    true },
  {0x43,  "camp_wom",    "",                -1,    true },
  {0x44,  "HITMAN",      "Hitman",         122,    true }, // MPC_021
  {0x45,  "?",           "",                -1,    false },
  {0x46,  "?",           "",                -1,    false },
  {0x47,  "?",           "",                -1,    false },
  {0x48,  "?",           "",                -1,    false },
  {0x49,  "?",           "",                -1,    false },
  {0x4A,  "?",           "",                -1,    false },
  {0x4B,  "?",           "",                -1,    false },
  {0x4C,  "?",           "",                -1,    false },
  {0x4D,  "?",           "",                -1,    false },
  {0x4E,  "?",           "",                -1,    false },
  {0x4F,  "gang01",      "",               123,    true }, // LEONE_A
  {0x50,  "gang02",      "Lou Scannon",    124,    true }, // LEONE_B - MPC_022
  {0x51,  "gang03",      "Hung Lo",         69,    true }, // TRIAD_A - MPC_023
  {0x52,  "gang04",      "",                 3,    true }, // TRIAD_B
  {0x53,  "gang05",      "",                 4,    true }, // DIABLO_A
  {0x54,  "gang06",      "Cruz Vormen",      5,    true }, // DIABLO_B - MPC_024
  {0x55,  "gang07",      "Yamazaki",        68,    true }, // YAKUZA_A - MPC_025
  {0x56,  "gang08",      "",                 6,    true }, // YAKUZA_B
  {0x57,  "gang09",      "",                 7,    true }, // YARDIE_A
  {0x58,  "gang10",      "Busta Moves",      8,    true }, // YARDIE_B - MPC_026
  {0x59,  "gang11",      "Juan Kerr",        9,    true }, // COLOMBIAN_A - MPC_027
  {0x5A,  "gang12",      "",                10,    true }, // COLOMBIAN_B
  {0x5B,  "gang13",      "Nick Yakar",      11,    true }, // HOOD_A - MPC_028
  {0x5C,  "gang14",      "Robin Emblind",   12,    true }, // HOOD_B - MPC_045
  {0x5D,  "gang15",      "",                89,    true }, // Forelli thug1
  {0x5E,  "gang16",      "Lou Bricant",     90,    true }, // Forelli thug2 - MPC_029
  {0x5F,  "gang17",      "Lance Urwell",    88,    true }, // Sindacco thug1 - MPC_030
  {0x60,  "gang18",      "",                89,    true }, // Sindacco thug2
  {0x61,  "vice1",       "",                -1,    true },
  {0x62,  "vice2",       "",                -1,    true },
  {0x63,  "vice3",       "",                -1,    true },
  {0x64,  "vice4",       "",                -1,    true },
  {0x65,  "vice5",       "",                -1,    true },
  {0x66,  "vice6",       "",                -1,    true },
  {0x67,  "vice7",       "",                -1,    true },
  {0x68,  "vice8",       "",                -1,    true },
  {0x69,  "",            "",                -1,    false },
  {0x6A,  "",            "",                -1,    false },
  {0x6B,  "",            "",                -1,    false },
  {0x6C,  "",            "",                -1,    false },
  {0x6D,  "",            "",                -1,    false }, // 6D is used for all special models?
  {0x6E,  "",            "",                -1,    false },
  {0x6F,  "",            "",                -1,    false },
  {0x70,  "",            "",                -1,    false },
  {0x71,  "",            "",                -1,    false },
  {0x72,  "",            "",                -1,    false }, // avenging angel #1
  {0x73,  "",            "",                -1,    false }, // avenging angel #2 (black guy)

*/

  ///from character viewer
  //row: 142   Model:   SAL_CON    Sal Convict
  //row: --  Model:  BODYBAG    bodybag
  //row: --  Model:  JD_shot    JD shot dead
  //row: 141  Model:  GUN_ASS    Gun assistant
  //row: 140  Model:  GREASE    Grease Zuko
  //row: 139  Model:  PHILC    Phil Cassidy
  //row: 138  Model:  plr12    (Dragon)
  //row: 137  Model:  plr11    (Bad Super Hero)
  //row: 136  Model:  plr10    (Boxer Shorts)
  //row: 135  Model:  plr9    (Cock's Mascot)
  //....
  //row: 128  Model:  plr2    (Lawyer)
  //row: 126  Model:  Chauff    Chauffeur
  //row: 125  Model:  HOLEJOG    Mayor RC Hole (Jogging outfit)
  //row: 124  Model:  gang02    LEONE_B
  //row: 123  Model:  gang01    LEONE_A
  //row: 122  Model:  HITMAN    Hitman
  //row: 121  Model:  fatmale01
  //row: 120  Model:  taxi_d
  //row: 119  Model:  fireman
  //row: 118  Model:  medic
  //row: 117  Model:  army
  //row: 116  Model:  fbi
  //row: 115  Model:  swat
  //row: 114  Model:  cop
  //row: 113  Model:  HOBO_01    Donald Love 2


  ///more from main.scm
  // ROW: 65 MODEL: SAL_01 Salvatore (Mob Don)  109   MPC_040
  // ROW: 66 MODEL: RAY_01 Ray (Bad Cop)        110
  // ROW: 72 MODEL: VINC_01 Vincenzo Cilli      111   MPC_039
  // ROW: 73 MODEL: GRDANG1 Avenging Angel
  // ROW: 74 MODEL: NED_01 Ned Burner (Reporter)
  // ROW: 75 MODEL: MASTOR Massimo Torrini (Assassin)
  // ROW: 76 MODEL: TOOL_01 JD O'Toole                MPC_012
  // ROW: 77 MODEL: Not used Fthr Kenny
  // ROW: 78 MODEL: GRDANG2 Avenging Angel 2
  // ROW: 79 MODEL: M_HOLE Mayor RC Hole
  // ROW: 80 MODEL: FRANFOR Franco Forelli     119
  // ROW: 81 MODEL: KAZUKI Kazuki Kasen        120    MPC_064
  // ROW: 82 MODEL: PAULIE Paulie Sindacco            MPC_049
  // ROW: 83 MODEL: HOPPER Jane Hopper
  // ROW: 84 MODEL: AVERY Avery Carrington
  // ROW: 85 MODEL: CASA Giovanni Casa
  // ROW: 86 MODEL: BABY Giovanni Casa (baby)
  // ROW: 87 MODEL: MCAFFRY Det. Leon McAffrey
  // ROW: 92 MODEL: EIGHT 8Ball
  // ROW: 93 MODEL: MAR_01 Maria
  // ROW: 94 MODEL: LOVE_01 Donald Love
  // ROW: 95 MODEL: MICKEYH Mickey H
  // ROW: 96 MODEL: WAYNE Wayne (Biker Dude)         MPC_048
  // ROW: 97 MODEL: BIKER1 Biker Dude 1              MPC_044
  // ROW: 98 MODEL: BIKER2 Biker Dude 2
  // ROW: 99 MODEL: DONOVAN O'Donovan
  // ROW: 100 MODEL: KAS_01 Toshiko Kasen (kimono)   kas_01

  /* more from Ghidra
  mar_01
  HOBO_01
  LOVE_01
  JDMADE
  TOOL_01
  MCAFFRY
  SAL_CON
  SAL_01
  WKAS_01   MPC_041
  KAS_01
  MICKEYH
  BIKER1
  BIKER2
  GRDANG1
  GRDANG2
  */


  /*** Multiplayer Characters from .text
  MPC_000 - plr          Toni
  MPC_001 - plr2         'Lionel'
  MPC_002 - plr3         Cipriani
  MPC_003 - plr4         Chauffeur
  MPC_004 - plr5         13th
  MPC_005 - plr6         Toni the Tux
  MPC_006 - plr7         Fury
  MPC_007 - plr8         The King
  MPC_008 - plr9         Cocks
  MPC_009 - plr10        Strak
  MPC_010 - plr11        'T'
  MPC_011 - plr12        Dragon
  MPC_012 - tool_01      JD
  MPC_013 - gimp         The Gimp
  MPC_014 - avery        Avery
  MPC_015 - baby         Choc-Choc
  MPC_016 - eight        8-Ball
  MPC_017 - mar_01       Maria
  MPC_018 - mickeyh      'Hamfists'
  MPC_019 - biker2       PhilDeGirth
  MPC_020 - thug_01      Rico Garlik
  MPC_021 - hitman       No.1
  MPC_022 - gang02       Lou Scannon
  MPC_023 - gang03       Hung Lo
  MPC_024 - gang06       Cruz Vormen
  MPC_025 - gang07       Yamazaki
  MPC_026 - gang10       Busta Moves
  MPC_027 - gang11       Juan Kerr
  MPC_028 - gang13       Nick Yakar
  MPC_029 - gang16       Lou Bricant
  MPC_030 - gang17       Lance Urwell
  MPC_031 - cop          Office '69'
  MPC_032 - swat         The Specialist
  MPC_033 - fbi          Agent Ochre
  MPC_034 - army         G.I. Jones
  MPC_035 - fireman      Sam
  MPC_036 - pimp         Papi Love
  MPC_037 - prostitute   Lucy Lastic
  MPC_038 - const1       Bill Durzbum
  MPC_039 - vinc_01      Vincenzo
  MPC_040 - sal_01       Salvatore
  MPC_041 - wkas_01      Toshi
  MPC_042 - kas_01       Toshiko
  MPC_043 - Fatfemale01  Ophelia Lardez
  MPC_044 - biker1       Ed Banger
  MPC_045 - gang14       Robin Emblind
  MPC_046 - ct_wom1      Mrs. Menalot
  MPC_047 - ct_man2      Hugh Jazz
  MPC_048 - wayne        Wayne
  MPC_049 - paulie       Paulie
  MPC_050 - female01     Anne Job
  MPC_051 - male03       Randy Boner
  MPC_052 - scum_man     Owen Monie
  MPC_053 - taxi_d       Ekant Sing
  MPC_054 - deliass      M.T.Head
  MPC_055 - criminal02   Rick O'Shea
  MPC_056 - male01       Arden Long
  MPC_057 - p_wom1       Ann Thrax
  MPC_058 - B_wom1       Fay Slift
  MPC_059 - prostitute2  Mona Lott
  MPC_060 - plr13        Antonio
  MPC_061 - plr14        Sweats
  MPC_062 - plr16        Wiseguy
  MPC_063 - plr15        Goodfella
  MPC_064 - kazuki       Kazuki
  **/

  /* https://tcrf.net/Grand_Theft_Auto:_Liberty_City_Stories/Debug_Menu
  ROW: -- MODEL: player
  ROW: 114 MODEL: cop
  ROW: 115 MODEL: swat
  ROW: 116 MODEL: fbi
  ROW: 117 MODEL: army
  ROW: 118 MODEL: medic
  ROW: 119 MODEL: fireman
  ROW: 71 MODEL: male01
  ROW: 120 MODEL: taxi_d
  ROW: 2 MODEL: pimp
  ROW: 13 MODEL: criminal01
  ROW: 14 MODEL: criminal02
  ROW: 15 MODEL: male02
  ROW: 16 MODEL: male03
  ROW: 121 MODEL: fatmale01
  ROW: 17 MODEL: fatmale02
  ROW: 18 MODEL: female01
  ROW: 19 MODEL: female02
  ROW: 20 MODEL: female03
  ROW: 21 MODEL: fatfemale01
  ROW: 22 MODEL: fatfemale02
  ROW: 23 MODEL: prostitute
  ROW: 24 MODEL: prostitute2
  ROW: 25 MODEL: p_man1
  ROW: 26 MODEL: p_man2
  ROW: 27 MODEL: p_wom1
  ROW: 28 MODEL: p_wom2
  ROW: 29 MODEL: ct_man1
  ROW: 30 MODEL: ct_man2
  ROW: 31 MODEL: ct_wom1
  ROW: 32 MODEL: ct_wom2
  ROW: 33 MODEL: li_man1
  ROW: 34 MODEL: li_man2
  ROW: 35 MODEL: li_wom1
  ROW: 36 MODEL: li_wom2
  ROW: 37 MODEL: docker1
  ROW: 38 MODEL: docker2
  ROW: 39 MODEL: scum_man
  ROW: 40 MODEL: scum_wom
  ROW: 41 MODEL: worker1
  ROW: 42 MODEL: worker2
  ROW: 43 MODEL: b_man1
  ROW: 44 MODEL: b_man2
  ROW: 67 MODEL: b_man3
  ROW: 45 MODEL: b_wom1
  ROW: 46 MODEL: b_wom2
  ROW: 70 MODEL: b_wom3
  ROW: 47 MODEL: mod_man
  ROW: 48 MODEL: mod_wom
  ROW: 49 MODEL: st_man
  ROW: 50 MODEL: st_wom
  ROW: 51 MODEL: fan_man1
  ROW: 52 MODEL: fan_man2
  ROW: 53 MODEL: fan_wom
  ROW: 54 MODEL: hos_man
  ROW: 55 MODEL: hos_wom
  ROW: 56 MODEL: const1
  ROW: 57 MODEL: const2
  ROW: 58 MODEL: shopper1
  ROW: 59 MODEL: shopper2
  ROW: 60 MODEL: shopper3
  ROW: 61 MODEL: stud_man
  ROW: 62 MODEL: stud_wom
  ROW: 63 MODEL: cas_man
  ROW: 64 MODEL: cas_wom
  ROW: 123 MODEL: gang01 LEONE_A
  ROW: 124 MODEL: gang02 LEONE_B
  ROW: 69 MODEL: gang03 TRIAD_A
  ROW: 3 MODEL: gang04 TRIAD_B
  ROW: 4 MODEL: gang05 DIABLO_A
  ROW: 5 MODEL: gang06 DIABLO_B
  ROW: 68 MODEL: gang07 YAKUZA_A
  ROW: 6 MODEL: gang08 YAKUZA_B
  ROW: 7 MODEL: gang09 YARDIE_A
  ROW: 8 MODEL: gang10 YARDIE_B
  ROW: 9 MODEL: gang11 COLOMBIAN_A
  ROW: 10 MODEL: gang12 COLOMBIAN_B
  ROW: 11 MODEL: gang13 HOOD_A
  ROW: 12 MODEL: gang14 HOOD_B
  ROW: 89 MODEL: gang15 Forelli thug1
  ROW: 90 MODEL: gang16 Forelli thug2
  ROW: 88 MODEL: gang17 Sindacco thug1
  ROW: 89 MODEL: gang18 Sindacco thug2
  ROW: 65 MODEL: SAL_01 Salvatore (Mob Don)
  ROW: 66 MODEL: RAY_01 Ray (Bad Cop)
  ROW: 72 MODEL: VINC_01 Vincenzo Cilli
  ROW: 73 MODEL: GRDANG1 Avenging Angel
  ROW: 74 MODEL: NED_01 Ned Burner (Reporter)
  ROW: 75 MODEL: MASTOR Massimo Torrini (Assassin)
  ROW: 76 MODEL: TOOL_01 JD O'Toole
  ROW: 77 MODEL: Not used Fthr Kenny
  ROW: 78 MODEL: GRDANG2 Avenging Angel 2
  ROW: 79 MODEL: M_HOLE Mayor RC Hole
  ROW: 80 MODEL: FRANFOR Franco Forelli
  ROW: 81 MODEL: KAZUKI Kazuki Kasen
  ROW: 82 MODEL: PAULIE Paulie Sindacco
  ROW: 83 MODEL: HOPPER Jane Hopper
  ROW: 84 MODEL: AVERY Avery Carrington
  ROW: 85 MODEL: CASA Giovanni Casa
  ROW: 86 MODEL: BABY Giovanni Casa (baby)
  ROW: 87 MODEL: MCAFFRY Det. Leon McAffrey
  ROW: 92 MODEL: EIGHT 8Ball
  ROW: 93 MODEL: MAR_01 Maria
  ROW: 94 MODEL: LOVE_01 Donald Love
  ROW: 95 MODEL: MICKEYH Mickey H
  ROW: 96 MODEL: WAYNE Wayne (Biker Dude)
  ROW: 97 MODEL: BIKER1 Biker Dude 1
  ROW: 98 MODEL: BIKER2 Biker Dude 2
  ROW: 99 MODEL: DONOVAN O'Donovan
  ROW: 100 MODEL: KAS_01 Toshiko Kasen (kimono)
  ROW: 101 MODEL: WKAS_01 Toshiko Kasen (western)
  ROW: 104 MODEL: JDMADE JD O'Toole - Made Man
  ROW: 106 MODEL: THUG_01 Sicilian Thug 1
  ROW: 107 MODEL: THUG_02 Sicilian Thug 2
  ROW: 109 MODEL: CAMP_MAN Campaign Worker
  ROW: 110 MODEL: CAMP_WOM Campaign Worker 2
  ROW: 111 MODEL: MIGUEL Miguel
  ROW: 112 MODEL: DELIASS Deli Assistant
  ROW: 113 MODEL: HOBO_01 Donald Love 2
  ROW: 122 MODEL: HITMAN Hitman
  ROW: 125 MODEL: HOLEJOG Mayor RC Hole (Jogging outfit)
  ROW: 126 MODEL: Chauff Chauffeur
  ROW: 128 MODEL: plr2 (Lawyer)
  ROW: 129 MODEL: plr3 (Travel clothes)
  ROW: 130 MODEL: plr4 (Chauffeur)
  ROW: 131 MODEL: plr5 (Hockey mask)
  ROW: 132 MODEL: plr6 (Tuxedo)
  ROW: 133 MODEL: plr7 (Avenging Angel)
  ROW: 134 MODEL: plr8 (The King of Rock'n Roll)
  ROW: 135 MODEL: plr9 (Cock's Mascot)
  ROW: 136 MODEL: plr10 (Boxer Shorts)
  ROW: 137 MODEL: plr11 (Bad Super Hero)
  ROW: 138 MODEL: plr12 (Dragon)
  ROW: 139 MODEL: PHILC Phil Cassidy
  ROW: 140 MODEL: GREASE Grease Zuko
  ROW: 141 MODEL: GUN_ASS Gun assistant
  ROW: -- MODEL: JD_shot JD shot dead
  ROW: -- MODEL: BODYBAG bodybag
  ROW: 142 MODEL: SAL_CON Sal Convict
*/

//}; static const array_size lcs_charactersize = ARRAY_SIZE(lcs_characters)-1;


/*const GTACharacter vcs_characters[] = {
  ///id  //model      //name          ///row  ///spawnsave
  {0x00,  "plr",       "Corporal Vance",     -1,    true }, // MPC_000 - Corporal Vance
  {0x08,  "cop",       "T.J.Sucker",         -1,    true }, // MPC_014
  {0x09,  "swat",      "The Specialist",     -1,    true }, // MPC_015
  {0x0A,  "fbi",       "Agent Provocateur",  -1,    true }, // MPC_016
  {0x0B,  "army",      "Cam O'Flarge",       -1,    true }, // MPC_017
  {0x0C,  "medic",     "",                   -1,    true }, // medic guy
  {0x0D,  "fireman",   "Jose",               -1,    true }, // MPC_019
  {0x0E,  "male01",    "",                   -1,    true }, // male
  //0xF ?  MaryJ
  {0x10,  "HFYST",     "",                   -1,    true }, // female
  {0x11,  "HFOST",     "",                   -1,    true }, // female
  {0x12,  "HMYST",     "",                   -1,    true }, // male
  {0x13,  "HMOST",     "",                   -1,    true }, // male
  {0x14,  "HFYRI",     "",                   -1,    true }, // female
  {0x15,  "HFORI",     "",                   -1,    true }, // female
  {0x16,  "HMYRI",     "",                   -1,    true }, // male
  {0x17,  "HMORI",     "",                   -1,    true }, // male
  {0x18,  "HFYBE",     "Tess Tickle",        -1,    true }, // MPC_030
  {0x19,  "HFOBE",     "",                   -1,    true }, // fat female beach
  {0x1A,  "HMYBE",     "Harry Balzac",       -1,    true }, // MPC_032
  {0x1B,  "HMOBE",     "",                   -1,    true }, // fat male beach
  {0x1C,  "HFYBU",     "",                   -1,    true }, // female
  {0x1D,  "HFYMD",     "Sue E. Sydle",       -1,    true }, // MPC_035
  {0x1E,  "HFYCG",     "",                   -1,    true }, // female with box
  {0x1F,  "HFYPR",     "Jenny Tulls",        -1,    true }, // MPC_037
  {0x20,  "HFOTR",     "Anita Bath",         -1,    true }, // MPC_038
  {0x21,  "HMOTR",     "Daryl Lect",         -1,    true }, // MPC_039
  {0x22,  "HMYAP",     "",                   -1,    true }, // male delivery
  {0x23,  "HMOCA",     "",                   -1,    true }, // old male
  {0x24,  "BMODK",     "",                   -1,    true }, // male black worker
  {0x25,  "BMYCR",     "Duane DeVane",       -1,    true }, // MPC_043
  {0x26,  "BFYST",     "",                   -1,    true }, // female black
  {0x27,  "BFOST",     "",                   -1,    true }, // fat female black
  {0x28,  "BMYST",     "Abe Rudder",         -1,    true }, // MPC_046
  {0x29,  "BMOST",     "",                   -1,    true }, // male black
  {0x2A,  "BFYRI",     "",                   -1,    true }, // female black
  {0x2B,  "BFORI",     "",                   -1,    true }, // female black
  {0x2C,  "BMYRI",     "",                   -1,    true }, // male black
  {0x2D,  "BFYBE",     "",                   -1,    true }, // female black bikini
  {0x2E,  "BMYBE",     "",                   -1,    true }, // male black swimsuit
  {0x2F,  "BFOBE",     "",                   -1,    true }, // fat female black swimsuit
  {0x30,  "BMOBE",     "",                   -1,    true }, // fat male black swimsuit
  {0x31,  "BMYBU",     "",                   -1,    true }, // male black suit
  {0x32,  "BFYPR",     "",                   -1,    true }, // female black
  {0x33,  "BFOTR",     "",                   -1,    true }, // fat male black
  {0x34,  "BMOTR",     "Abe Ozo",            -1,    true }, // MPC_058
  {0x35,  "BMYPI",     "Poop Dawg",          -1,    true }, // MPC_059
  {0x36,  "BMYBB",     "Moe Lester",         -1,    true }, // MPC_060
  {0x37,  "WMYCR",     "Dylan Weed",         -1,    true }, // MPC_061
  {0x38,  "WFYST",     "Dee Generate",       -1,    true }, // MPC_062
  {0x39,  "WFOST",     "",                   -1,    true }, // female old lady
  {0x3A,  "WMYST",     "",                   -1,    true }, // male sunglasses
  {0x3B,  "WMOST",     "",                   -1,    true }, // male old hat
  {0x3C,  "WFYRI",     "Pearl Neklass",      -1,    true }, // MPC_066
  {0x3D,  "WFORI",     "",                   -1,    true }, // female
  {0x3E,  "WMYRI",     "Hayden Seek",        -1,    true }, // MPC_068
  {0x3F,  "WMORI",     "",                   -1,    true }, // male
  {0x40,  "WFYBE",     "Stacey Rhect",       -1,    true }, // MPC_070
  {0x41,  "WMYBE",     "Chris Packet",       -1,    true }, // MPC_071
  {0x42,  "WFOBE",     "",                   -1,    true }, // fat female bikini
  {0x43,  "WMOBE",     "Pops",               -1,    true }, // MPC_073
  {0x44,  "WMYCW",     "",                   -1,    true }, // male worker
  {0x45,  "WFOGO",     "",                   -1,    true }, // female old lady sport
  {0x46,  "WMOGO",     "",                   -1,    true }, // male old golfer
  {0x47,  "WFYLG",     "",                   -1,    true }, // female lifeguard
  {0x48,  "WMYLG",     "",                   -1,    true }, // male lifeguard
  {0x49,  "WFYBU",     "",                   -1,    true }, // female business
  //0x4A  MPC_080 - WMYBU    Boyd Schidt
  {0x4B,  "WMOBU",     "",                   -1,    true }, // male business
  {0x4C,  "WFYPR",     "Candy Barr",         -1,    true }, // MPC_082
  {0x4D,  "WFOTR",     "",                   -1,    true }, // fat female
  {0x4E,  "WMOTR",     "",                   -1,    true }, // male
  {0x4F,  "WMYPI",     "Des Buratto",        -1,    true }, // MPC_085
  {0x50,  "WMOCA",     "",                   -1,    true }, // male
  {0x51,  "WFYJG",     "Martha Fokker",      -1,    true }, // MPC_087
  {0x52,  "WMYJG",     "Leo Tarred",         -1,    true }, // MPC_088
  {0x53,  "WFYSK",     "",                   -1,    true }, // female bikini sunglasses
  {0x54,  "WMYSK",     "",                   -1,    true }, // male sunglasses
  {0x55,  "WFYSH",     "",                   -1,    true }, // female shopping
  {0x56,  "WFOSH",     "",                   -1,    true }, // female old shopping
  {0x57,  "JFOTO",     "",                   -1,    true }, // female photgrapher
  {0x58,  "JMOTO",     "",                   -1,    true }, // male photgrapher
  {0x59,  "PGang7A",   "Lee Vitout",         -1,    true }, // MPC_095
  {0x5A,  "PGang7B",   "",                   -1,    true }, // male
  {0x5B,  "Gang1A",    "Joaquin Noff",       -1,    true }, // MPC_097
  {0x5C,  "Gang1B",    "",                   -1,    true }, // male
  {0x5D,  "Gang2A",    "Decjuan DeMarco",    -1,    true }, // MPC_099
  {0x5E,  "Gang2B",    "",                   -1,    true }, // male
  {0x5F,  "Gang3A",    "Kanye Diggit",       -1,    true }, // MPC_101
  {0x60,  "Gang3B",    "",                   -1,    true }, // male
  {0x61,  "Gang4A",    "PVT. Parts",         -1,    true }, // MPC_103
  {0x62,  "Gang4B",    "PVT. Places",        -1,    true }, // MPC_104
  {0x63,  "Gang5A",    "Tate Urchips",       -1,    true }, // MPC_105
  {0x64,  "Gang5B",    "Buster Nutt",        -1,    true }, // MPC_106
  {0x65,  "Gang6A",    "Harris Mint",        -1,    true }, // MPC_107
  {0x66,  "Gang6B",    "",                   -1,    true }, // male biker
  {0x67,  "Gang7A",    "Troy",               -1,    true }, // MPC_109
  {0x68,  "Gang7B",    "",                   -1,    true }, // male fat
  {0x69,  "Gang8A",    "",                   -1,    true }, // male golfer
  {0x6A,  "Gang8B",    "",                   -1,    true }, // male golfer
  {0x6B,  "Gang9A",    "Ron A. Muck",        -1,    true }, // MPC_113
  {0x6C,  "Gang9B",    "",                   -1,    true }, // male black bald
  {0x6D,  "DiaGgA",    "Beau Nurr",          -1,    true }, // MPC_115
  {0x6E,  "DiaGgB",    "",                   -1,    true }, // male hawaii shirt
  {0x6F,  "GonGgA",    "Fidel Sistaxez",     -1,    true }, // MPC_117
  {0x70,  "GonGgB",    "Hugo Zehr",          -1,    true }, // MPC_118
  {0x71,  "MenGgA",    "Juan Ovzem",         -1,    true }, // MPC_119
  {0x72,  "MenGgB",    "",                   -1,    true }, // male
  {0x73,  "DEA1",      "Agent Alman",        -1,    true }, // MPC_121
  {0x74,  "DEA2",      "",                   -1,    true }, // dea agent with shirt
  {0x75,  "vice1",     "Officer Cracker",    -1,    true }, // MPC_123
  {0x76,  "vice2",     "Officer Butts",      -1,    true }, // MPC_124
  {0x77,  "BCop",      "Colin Allcars",      -1,    true }, // MPC_125
  {0x78,  "Alberto",   "Alberto",            -1,    true }, // MPC_126
  {0x79,  "Arman",     "Armando Mendez",     -1,    true }, // MPC_127
  {0x7A,  "Barry",     "Barry Mickelthwaite",-1,    true }, // MPC_128
  {0x7B,  "Burger",    "",                   -1,    true }, // male employee
  {0x7C,  "Bry",       "Forbes",             -1,    true }, // MPC_130
  {0x7D,  "BryB",      "",                   -1,    true }, // Forbes with paperbag
  {0x7E,  "BryE",      "",                   -1,    true }, // Forbes injured
  {0x7F,  "Cokhed",    "Spitz",              -1,    true }, // MPC_133
  {0x80,  "Diaz",      "Ricardo Diaz",       -1,    true }, // MPC_134
  {0x81,  "Diego",     "Diego Mendez",       -1,    true }, // MPC_135
  {0x82,  "FlmCrw",    "",                   -1,    true }, // male
  {0x83,  "Gtr1",      "",                   -1,    true }, // male guitarist
  {0x84,  "Gonz",      "Gonzalez",           -1,    true }, // MPC_138
  {0x85,  "Hitman",    "No. 1",              -1,    true }, // MPC_139
  {0x86,  "Gtr2",      "",                   -1,    true }, // male guitarist
  {0x87,  "Jerry",     "Jerry Martinez",     -1,    true }, // MPC_141
  {0x88,  "LanSm",     "Lance",              -1,    true }, // MPC_142
  {0x89,  "LanSt",     "Lance T.",           -1,    true }, // MPC_143
  {0x8A,  "Lou",       "Louise Cassidy-Williams", -1,  true }, //
  //0x8B   PhCol2      crash                                //Phil Collins
  {0x8C,  "LouB",      "",                   -1,    true }, // female bloody
  {0x8D,  "LouM",      "",                   -1,    true }, // female
  {0x8E,  "Marty",     "Marty Jay Williams", -1,    true }, // MPC_148
  {0x8F,  "ShpKpr3",   "",                   -1,    true }, // male worker
  {0x90,  "GayBiA",    "Gabe Hiker",         -1,    true }, // MPC_150  Gabe Hiker     ???
  {0x91,  "GayBiB",    "",                   -1,    true }, //
  {0x92,  "PhCol",     "Phil Collins",       -1,    true }, // MPC_152
  {0x93,  "Phil",      "Phil Cassidy",       -1,    true }, // MPC_153
  {0x94,  "Reni",      "Reni Wassulmaier",   -1,    true }, // MPC_154
  {0x95,  "RenOp",     "Ms. Reni",           -1,    true }, // MPC_155
  {0x96,  "Robber",    "Robin Banks",        -1,    true }, // MPC_156
  {0x97,  "Sarge",     "Sgt. Peppah",        -1,    true }, // MPC_157
  {0x98,  "ShopKpr",   "",                   -1,    true }, // nerd guy
  {0x99,  "Umber",     "Umberto Robina",     -1,    true }, // MPC_159
  {0x9A,  "ZomA",      "Bub",                -1,    true }, // MPC_160
  {0x9B,  "plr2",      "Vic",                -1,    true }, // MPC_001 - Vic
  {0x9C,  "plr3",      "Pimp Dawg",          -1,    true }, // MPC_002 - Pimp Dawg
  {0x9D,  "plr4",      "'Vince'",            -1,    true }, // MPC_003 - 'Vince'
  {0x9E,  "plr5",      "Vide Loca",          -1,    true }, // MPC_004 - Vide Loca
  {0x9F,  "plr6",      "Victor",             -1,    true }, // MPC_005 - Victor
  {0xA0,  "plr7",      "Traks",              -1,    true }, // MPC_006 - Traks
  {0xA1,  "plr8",      "Hood",               -1,    true }, // MPC_007 - Hood
  {0xA2,  "plr9",      "Muscle",             -1,    true }, // MPC_008 - Muscle
  {0xA3,  "plr10",     "Repo",               -1,    true }, // MPC_009 - Repo
  {0xA4,  "plr11",     "Smuggler",           -1,    true }, // MPC_010 - Smuggler
  {0xA5,  "plr12",     "L'Homme",            -1,    true }, // MPC_011 - L'Homme
  {0xA6,  "plr13",     "Winner",             -1,    true }, // vic r*lax shirt
  {0xA7,  "plr14",     "Wetz",               -1,    true }, // MPC_013 - Wetz

*/
  /*** Multiplayer Characters from .text
  MPC_000 - plr       Corporal Vance
  MPC_001 - plr2      Vic
  MPC_002 - plr3      Pimp Dawg
  MPC_003 - plr4      'Vince'
  MPC_008 - plr9      Muscle
  MPC_009 - plr10     Repo
  MPC_004 - plr5      Vide Loca
  MPC_005 - plr6      Victor
  MPC_006 - plr7      Traks
  MPC_007 - plr8      Hood
  MPC_010 - plr11     Smuggler
  MPC_011 - plr12     L'Homme
  MPC_013 - plr14     Wetz
  MPC_141 - Jerry     Jerry Martinez
  MPC_153 - Phil      Phil Cassidy
  MPC_157 - Sarge     Sgt. Peppah
  MPC_148 - Marty     Marty Jay Williams
  MPC_126 - Alerto    Alberto
  MPC_130 - Bry       Forbes
  MPC_143 - LanSt     Lance T.
  MPC_150 - GayBiA    Gabe Hiker
  MPC_156 - Robber    Robin Banks
  MPC_159 - Umber     Umberto Robina
  MPC_127 - Arman     Armando Mendez
  MPC_128 - Barry     Barry Mickelthwaite
  MPC_133 - CokHed    Spitz
  MPC_134 - Diaz      Ricardo Diaz
  MPC_135 - Diego     Diego Mendez
  MPC_138 - Gonz      Gonzalez
  MPC_139 - Hitman    No. 1
  MPC_142 - LanSm     Lance
  MPC_152 - PhCol     Phil Collins
  MPC_154 - Reni      Reni Wassulmaier
  MPC_155 - RenOp     Ms. Reni
  MPC_160 - ZomA      Bub
  MPC_097 - Gang1A    Joaquin Noff
  MPC_099 - Gang2A    Decjuan DeMarco
  MPC_103 - Gang4A    PVT. Parts
  MPC_104 - Gang4B    PVT. Places
  MPC_109 - Gang7A    Troy
  MPC_105 - Gang5A    Tate Urchips
  MPC_106 - Gang5B    Buster Nutt
  MPC_095 - PGang7A   Lee Vitout
  MPC_101 - Gang3A    Kanye Diggit
  MPC_107 - Gang6A    Harris Mint
  MPC_113 - Gang9A    Ron A. Muck
  MPC_115 - DiaGgA    Beau Nurr
  MPC_117 - GonGgA    Fidel Sistaxez
  MPC_118 - GonGgB    Hugo Zehr
  MPC_119 - MenGgA    Juan Ovzem
  MPC_014 - cop       T.J.Sucker
  MPC_019 - fireman   Jose
  MPC_059 - BMYPI     Poop Dawg
  MPC_015 - swat      The Specialist
  MPC_016 - fbi       Agent Provocateur
  MPC_082 - WFYPR     Candy Barr
  MPC_123 - vice1     Officer Cracker
  MPC_125 - BCop      Colin Allcars
  MPC_038 - HFOTR     Anita Bath
  MPC_017 - army      Cam O'Flarge
  MPC_030 - HFYBE     Tess Tickle
  MPC_032 - HMYBE     Harry Balzac
  MPC_035 - HFYMD     Sue E. Sydle
  MPC_037 - HFYPR     Jenny Tulls
  MPC_039 - HMOTR     Daryl Lect
  MPC_043 - BMYCR     Duane DeVane
  MPC_046 - BMYST     Abe Rudder
  MPC_058 - BMOTR     Abe Ozo
  MPC_060 - BMYBB     Moe Lester
  MPC_061 - WMYCR     Dylan Weed
  MPC_062 - WFYST     Dee Generate
  MPC_066 - WFYRI     Pearl Neklass
  MPC_068 - WMYRI     Hayden Seek
  MPC_070 - WFYBE     Stacey Rhect
  MPC_071 - WMYBE     Chris Packet
  MPC_073 - WMOBE     Pops
  MPC_080 - WMYBU     Boyd Schidt
  MPC_085 - WMYPI     Des Buratto
  MPC_087 - WFYJG     Martha Fokker
  MPC_088 - WMYJG     Leo Tarred
  MPC_121 - DEA1      Agent Alman
  MPC_124 - vice2     Officer Butts
  **************************************/

//}; static const array_size vcs_charactersize = ARRAY_SIZE(vcs_characters)-1;



const GTAPickup lcs_pickups[] = {
  ///name                //id  //type  //amount
  {"Mobilephone",         0x102, 0x0F,  0x00 },
  {"Brass Knuckles",      0x103, 0x0F,  0x00 },
  {"Chisel",              0x104, 0x0F,  0x00 },
  {"Hockey Stick",        0x105, 0x0F,  0x00 },
  {"Nightstick",          0x106, 0x0F,  0x00 },
  {"Butcher Knife",       0x107, 0x02,  0x00 },
  {"Baseball Bat",        0x108, 0x0F,  0x00 },
  {"Fire Axe",            0x109, 0x0F,  0x00 },
  {"Meat Cleaver",        0x10A, 0x0F,  0x00 },
  {"Machete",             0x10B, 0x0F,  0x00 },
  {"Katana",              0x10C, 0x0F,  0x00 },
  {"Chainsaw",            0x10D, 0x0F,  0x00 },
  {"Grenade",             0x10E, 0x0F,  0x0A },
  {"Teargas",             0x10F, 0x0F,  0x0A },
  {"Molotov",             0x110, 0x02,  0x0A },
  // 0x111 flying rocket
  {"Pistol",              0x112, 0x0F,  0x64 },
  {"Colt",                0x113, 0x0F,  0x12 },
  {"AK-47",               0x114, 0x0F,  0x64 },
  {"Shotgun",             0x115, 0x0F,  0x1C },
  {"Spaz 12",             0x116, 0x0F,  0x1C },
  {"Combat Shotgun",      0x117, 0x0F,  0x1C },
  {"M4",                  0x118, 0x0F,  0x64 },
  {"TEC-9",               0x119, 0x0F,  0x64 },
  {"Micro SMG",           0x11A, 0x0F,  0x64 },
  {"MAC-10",              0x11B, 0x0F,  0x64 },
  {"MP5",                 0x11C, 0x0F,  0x64 },
  {"Sniper",              0x11D, 0x0F,  0x0C },
  {"PSG Sniper",          0x11E, 0x0F,  0x32 },
  {"Rocket Launcher",     0x11F, 0x0F,  0x64 },
  {"Flamethrower",        0x120, 0x0F,  0x64 },
  {"M60",                 0x121, 0x0F,  0x64 },
  {"Minigun",             0x122, 0x0F,  0x64 },
  {"Remote Explosives",   0x123, 0x0F,  0x0A },
  {"Camera",              0x124, 0x0F,  0x64 },

  {"Money",               0x20A, 0x08,  0x64 },

  /// special
  {"Information",         0x22D, 0x03,  0x00 },
  {"Health",              0x22E, 0x0F,  0x00 },
  {"Adrenaline",          0x22F, 0x0F,  0x00 },
  {"Police Bribe",        0x23F, 0x0F,  0x00 },
  {"Movie",               0x247, 0x03,  0x00 },
  {"Rampage",             0x248, 0x03,  0x00 },
  {"Armour",              0x24B, 0x0F,  0x00 },
  {"Dollar",              0xEA9, 0x10,  0x00 },
  {"Clothes",             0xED5, 0x03,  0x00 },
  {"Save CD",             0xF2B, 0x03,  0x00 },

  {"Property (red)",      0xF05, 0x03,  0x00 },
  {"Property (green)",    0xF06, 0x03,  0x00 },

  /// multiplayer
  {"Mega Damage (MP)",    0xF07, 0x03,  0x00 },
  {"Plus (MP)",           0xF09, 0x03,  0x00 },
  {"Invisibility (MP)",   0xF0A, 0x03,  0x00 },
  {"Sticky Tires (MP)",   0xF9E, 0x03,  0x00 },
  {"Instant Repair (MP)", 0xF9F, 0x03,  0x00 },

  /// objects
  {"Briefcase (closed)",  0x206, 0x03,  0x00 },
  {"Briefcase (open)",    0xFAE, 0x03,  0x00 }, // with drugs
  {"Drug Package",        0x208, 0x03,  0x00 }, // GTA3 hidden package & in beta
  {"Drug Package Big",    0x231, 0x03,  0x00 }, // hidden package in Sindacco Chronicles
  {"TopSecret Folder",    0xF03, 0x03,  0x00 }, // used in LC Confidential
  {"Rockstar Bag (black)",0xFA1, 0x03,  0x00 }, // used as hidden package in beta
  {"Rockstar Bag (brown)",0xFA3, 0x03,  0x00 }, // used as hidden package in beta
  {"Hidden Package",      0xFE9, 0x03,  0x00 },


  // {"Cooking Pod",      0xFA2, 0x03,   0x00 },
}; const array_size lcs_pickupsize = ARRAY_SIZE(lcs_pickups) - 1;

const GTAPickup vcs_pickups[] = {
  ///name                //id     //type  //amount
  {"Phone",              0x14A,   0x0B,   0x01 }, // check type & amount
  {"Brass Knuckles",     0x14B,   0x0B,   0x01 },
  {"Butterfly Knife",    0x14C,   0x0B,   0x01 },
  {"Golf Club",          0x14D,   0x0B,   0x01 },
  {"Nightstick",         0x14E,   0x0B,   0x01 },
  {"Knife",              0x14F,   0x0B,   0x01 },
  {"Baseball Bat",       0x150,   0x0B,   0x01 },
  {"Axe",                0x151,   0x0B,   0x01 },
  {"Gaff Hook",          0x152,   0x0B,   0x01 },
  {"Machete",            0x153,   0x0B,   0x01 },
  {"Katana",             0x154,   0x0B,   0x01 },
  {"Chainsaw",           0x155,   0x0B,   0x01 },
  {"Grenade",            0x156,   0x0B,   0x0A },
  {"Teargas",            0x157,   0x0B,   0x0A }, // cut weapon  //check type & amount
  {"Molotov",            0x158,   0x0B,   0x0A },
   // 0x159 flying rocket
  {"Pistol",             0x15A,   0x0B,   0x1C },
  {"Python",             0x15B,   0x0B,   0x12 },
  {"AK-47",              0x15C,   0x0B,   0x64 },
  {"Shotgun",            0x15D,   0x0B,   0x1C },
  {"Combat Shotgun",     0x15E,   0x0B,   0x1C },
  {"Stubby Shotgun",     0x15F,   0x0B,   0x1C },
  {"Assault Rifle",      0x160,   0x0B,   0x64 },
  {"Scorpion",           0x161,   0x0B,   0x64 },
  {"Micro SMG",          0x162,   0x0B,   0x64 },
  {"MAC-10",             0x163,   0x0B,   0x64 },
  {"SMG",                0x164,   0x0B,   0x64 },
  {"Sniper",             0x165,   0x0B,   0x0E },
  {"Combat Sniper",      0x166,   0x0B,   0x32 },
  {"Rocket Launcher",    0x167,   0x0B,   0x0A },
  {"Flamethrower",       0x168,   0x0B,   0x258},
  {"M249",               0x169,   0x0B,   0x64 },
  {"Minigun",            0x16A,   0x0B,   0x1F4 },
  {"Remote Explosives",  0x16B,   0x0B,   0x0A },
  {"Camera",             0x16C,   0x0B,   0x0A }, // check type & amount
  {"Information",        0x16D,   0x0B,   0x00 }, // check type & amount
  // 0x16E MG Barrel
  {"Binoculars",         0x16F,   0x0B,   0x01 },
  {"Landmine",           0x170,   0x0B,   0x0A }, // cut weapon  //no corona though?!

  {"Money",              0x1B5,   0x08,   0x64 },

  {"Information",        0x1D1,   0x0B,   0x00 },  // there are two??
  {"Health",             0x1D2,   0x0B,   0x00 },
  {"Adrenaline",         0x1D3,   0x0B,   0x00 },
  {"Armour",             0x1D4,   0x0B,   0x00 },
  {"Police Bribe",       0x1DA,   0x0B,   0x00 },


  {"Movie",              0x1E1,   0x03,   0x00 },
  {"Rampage",            0x1E2,   0x03,   0x00 },

  {"Property (green)",   0x1F8,   0x03,   0x00 },
  {"Clothes",            0x1F9,   0x03,   0x00 },
  {"Save",               0x1FB,   0x03,   0x00 },

  {"Rampage",           0x1CE9,   0x03,   0x00 },
  {"Buy Vehicle",       0x1D42,   0x03,   0x00 },

}; const array_size vcs_pickupsize = ARRAY_SIZE(vcs_pickups) - 1;


/* const GTAMapIcon lcs_mapicons[] = {
  ///name            //id    //color(yes/no)
  {"0",                 0,   1   }, // objective
  {"radar_center",      1,   0   },
  {"arrow",             2,   0   },
  {"radar_north",       3,   0   },
  {"4",                 4,   0   },
  {"5",                 5,   0   },
  {"radar_cortez",      6,   0   },
  {"radar_diaz",        7,   0   },
  {"radar_kent",        8,   0   },
  {"radar_lawyer",      9,   0   },
  {"radar_phil",       10,   0   },
  {"11",               11,   0   },
  {"12",               12,   0   },
  {"13",               13,   0   },
  {"14",               14,   0   },
  {"15",               15,   0   },
  {"gun",              16,   0   },
  {"17",               17,   0   },
  {"18",               18,   0   },
  {"radar_save",       19,   0   },
  {"20",               20,   0   },
  {"21",               21,   0   },
  {"22",               22,   0   },
  {"23",               23,   0   },
  {"24",               24,   0   },
  {"25",               25,   0   }, // empty??
  {"26",               26,   0   },
  {"spray",            27,   0   },
  {"28",               28,   0   },
  {"29",               29,   0   }, // property?
  {"phone",            30,   0   },
  {"radar_8Ball",      31,   0   },
  {"radar_Burner",     32,   0   },
  {"radar_Cipriani",   33,   0   },
  {"radar_Donald",     34,   0   },
  {"radar_Joseph",     35,   0   },
  {"radar_Leone",      36,   0   },
  {"radar_Maria",      37,   0   },
  {"radar_McRay",      38,   0   },
  {"radar_Toshiko",    39,   0   },
  {"radar_Vincenzo",   40,   0   },
  {"radar_bomb",       41,   0   },
  {"radar_cShop",      42,   0   },
  {"radar_propertyR",  43,   0   },
  {"radar_flag",       44,   0   },
  {"radar_powerup",    45,   0   },
  {"mp_base",          46,   0   },
  {"mp_checkpoint",    47,   0   },
  {"mp_player",        48,   0   },
  {"mp_objective",     49,   1   },
  {"mp_car",           50,   0   },
  {"mp_tank",          51,   0   },
  {"mp_carlockup",     52,   0   },
  {"mp_targetplayer",  53,   0   },
  {"arrow_00",         54,   0   },
  {"arrow_01",         55,   0   },
  {"arrow_02",         56,   0   },
  {"arrow_03",         57,   0   },
  {"arrow_04",         58,   0   },
  {"arrow_05",         59,   0   },
  {"arrow_06",         60,   0   },
  {"arrow_07",         61,   0   },
}; static const array_size lcs_mapiconsize = ARRAY_SIZE(lcs_mapicons)-1;


const GTAMapIcon vcs_mapicons[] = {
  ///name             //id      //color(yes/no)
  {"0",                0x00,     1   },
  {"radar_center",     0x01,     0   },
  {"arrow",            0x02,     0   },
  {"radar_north",      0x03,     0   },
  {"gun",              0x04,     0   },
  {"5",                0x05,     0   },
  {"radar_save",       0x06,     0   },
  {"7",                0x07,     0   }, // invisible
  {"spray",            0x08,     0   },
  {"phone",            0x09,     0   },
  {"radar_bomb",       0x0A,     0   },
  {"radar_cshop",      0x0B,     0   },
  {"radar_powerup",    0x0C,     0   },
  {"mp_base",          0x0D,     0   },
  {"mp_checkpoint",    0x0E,     0   },
  {"mp_player",        0x0F,     0   },
  {"mp_objective",     0x10,     1   },
  {"mp_car",           0x11,     0   },
  {"mp_tank",          0x12,     0   },
  {"mp_carlockup",     0x13,     0   },
  {"mp_targetplayer",  0x14,     0   },
  {"arrow_00",         0x15,     0   },
  {"arrow_01",         0x16,     0   },
  {"arrow_02",         0x17,     0   },
  {"arrow_03",         0x18,     0   },
  {"arrow_04",         0x19,     0   },
  {"arrow_05",         0x1A,     0   },
  {"arrow_06",         0x1B,     0   },
  {"arrow_07",         0x1C,     0   },
  {"radar_empire",     0x1D,     0   },
  {"radar_empire_dam", 0x1E,     0   },
  {"radar_empire_con", 0x1F,     0   },
  {"32",               0x20,     0   }, // ? small
  {"33",               0x21,     0   }, // ? medium
  {"34",               0x22,     0   }, // ? big
  {"radar_bryan",      0x23,     0   },
  {"radar_gonz",       0x24,     0   },
  {"radar_lance",      0x25,     0   },
  {"radar_louise",     0x26,     0   },
  {"radar_marty",      0x27,     0   },
  {"radar_phil",       0x28,     0   },
  {"radar_reni",       0x29,     0   },
  {"radar_diaz",       0x2A,     0   },
  {"radar_jerry",      0x2B,     0   },
  {"radar_mendez",     0x2C,     0   },
  {"radar_umberto",    0x2D,     0   },
  {"radar_plane",      0x2E,     0   },
  {"radar_gunshop",    0x2F,     0   },
  {"radar_usj",        0x30,     0   }, // stunt jump
  {"radar_rampage",    0x31,     0   }, // rampage
  {"radar_balloon",    0x32,     0   }, // balloon
  {"radar_bcase",      0x33,     0   },
  {"radar_mpbomb",     0x34,     0   },
  {"radar_boat",       0x35,     0   },
  {"radar_heli",       0x36,     0   },
  //everything higher CRASHes
}; static const array_size vcs_mapiconsize = ARRAY_SIZE(vcs_mapicons)-1;
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

const uaddr memory_low  = 0x08400000; // memory bounds
uaddr memory_high = 0x0A000000; // default ~32 MB (with high memory layout -> 0x0C000000) dynamically overwritten in module_start

bool isInMemBounds(uaddr valtocheck) {
  return (valtocheck >= memory_low && valtocheck < memory_high);
}

void setBit(uaddr adr, uint8 bit, bool boolean) { // 1bit
  #if MEMCHECK
  if( isInMemBounds(adr) && bit <= 7 ) {
  #endif
    if( boolean ) { // switch on
      *(uint8*)(adr) |= (1 << bit);
    } else { // switch off
      *(uint8*)(adr) &= ~(1 << bit);
    }
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setBit()", 5.00f);
    MEM_LOG("[ERROR] %lu: setBit(0x%X, 0x%X, 0x%X)", getGametime(), adr, bit, boolean);
  }
  #endif
}

uint8 getBit(uaddr adr, uint8 bit) {
  #if MEMCHECK
  if( isInMemBounds(adr) ) {
  #endif
    return *(uint8*)(adr) & (1 << bit);
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getBit()", 5.00f);
    MEM_LOG("[ERROR] %lu: getBit(0x%X, %i)", getGametime(), adr, bit);
  }
  return 0;
  #endif
}

void setByte(uaddr adr, byte value) { // 8bit (unsigned 0-255)
  #if MEMCHECK
  if( isInMemBounds(adr) ) {
  #endif
    *(byte*)adr = value;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setByte()", 5.00f);
    MEM_LOG("[ERROR] %lu: setByte(0x%X, 0x%X)", getGametime(), adr, value);
  }
  #endif
}

byte getByte(uaddr adr) { // 8bit (unsigned 0-255)
  #if MEMCHECK
  if( isInMemBounds(adr) ) {
  #endif
    return *(byte*)adr;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getByte()", 5.00f);
    MEM_LOG("[ERROR] %lu: getByte(0x%X)", getGametime(), adr);
  }
  return 0;
  #endif
}

void setNibbleLow(uaddr adr, uint8 value) { // set 4bit
  #if MEMCHECK
  if( isInMemBounds(adr) ) {
  #endif
    *(byte*)adr = (*(uint8*)(adr) & 0xF0) | (value & 0xF); // write low quartet;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setNibbleLow()", 5.00f);
    MEM_LOG("[ERROR] %lu: setNibbleLow(0x%X, 0x%X)", getGametime(), adr, value);
  }
  #endif
}

uint8 getNibbleLow(uaddr adr) { //get lower 4bit converted to char
  #if MEMCHECK
  if( isInMemBounds(adr) ) {
  #endif
    return *(uint8*)adr & 0xF; //eg: 0xE6 & 0xF    -> 0x6
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getNibbleLow()", 5.00f);
    MEM_LOG("[ERROR] %lu: getNibbleLow(0x%X)", getGametime(), adr);
  }
  return 0;
  #endif
}

void setNibbleHigh(uaddr adr, uint8 value) { //set 4bit
  #if MEMCHECK
  if( isInMemBounds(adr) ) {
  #endif
    *(uint8*)adr = (*(uint8*)adr & 0x0F) | ((value & 0xF) << 4); // write high quartet
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setNibbleHigh()", 5.00f);
    MEM_LOG("[ERROR] %lu: setNibbleHigh(0x%X, 0x%X)", getGametime(), adr, value);
  }
  #endif
}

uint8 getNibbleHigh(uaddr adr) {
  #if MEMCHECK
  if( isInMemBounds(adr) ) {
  #endif
    return (*(uint8*)adr >> 4) & 0xF; // eg: 0xE6 >> 4 -> 0x?E  & 0xF -> 0x0E
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getNibbleHigh()", 5.00f);
    MEM_LOG("[ERROR] %lu: getNibbleHigh(0x%X)", getGametime(), adr);
  }
  return 0;
  #endif
}

void setShort(uaddr adr, short value) { // 16bit
  #if MEMCHECK
  if( isInMemBounds(adr) && (adr % 2 == 0) ) {
  #endif
    *(short*)adr = value;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setShort()", 5.00f);
    MEM_LOG("[ERROR] %lu: setShort(0x%X, 0x%X)", getGametime(), adr, value);
  }
  #endif
}

short getShort(uaddr adr) { // 16bit
  #if MEMCHECK
  if( isInMemBounds(adr) && (adr % 2 == 0) ) {
  #endif
    return *(short*)adr;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getShort()", 5.00f);
    MEM_LOG("[ERROR] %lu: getShort(0x%X)", getGametime(), adr);
  }
  return 0;
  #endif
}

void setInt(uaddr adr, int value) { // 32bit
  #if MEMCHECK
  if( isInMemBounds(adr) && (adr % 4 == 0) ) {
  #endif
    *(int*)adr = value;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setInt()", 5.00f);
    MEM_LOG("[ERROR] %lu: setInt(0x%X, 0x%X)", getGametime(), adr, value);
  }
  #endif
}

int getInt(uaddr adr) { // 32bit
  #if MEMCHECK
  if( isInMemBounds(adr) && (adr % 4 == 0)) {
  #endif
    return *(int*)adr;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getInt()", 5.00f);
    MEM_LOG("[ERROR] %lu: getInt(0x%X)", getGametime(), adr);
  }
  return 0;
  #endif
}

void setFloat(uaddr adr, float value) { // 32bit
  #if MEMCHECK
  if( isInMemBounds(adr) && (adr % 4 == 0) ) {
  #endif
    *(float*)adr = value;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setFloat()", 5.00f);
    MEM_LOG("[ERROR] %lu: setFloat(0x%X, %f)", getGametime(), adr, value);
  }
  #endif
}

float getFloat(uaddr adr) { // 32bit
  #if MEMCHECK
  if( isInMemBounds(adr) && (adr % 4 == 0) ) {
  #endif
    return *(float*)adr;
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getFloat()", 5.00f);
    MEM_LOG("[ERROR] %lu: getFloat(0x%X)", getGametime(), adr);
  }
  return 0.0f;
  #endif
}

#define MAX_STRING_SIZE 256

void setString(uaddr adr, u8string string, bool unicode) { // unicode -> (where every second is a char)
  #if MEMCHECK
  if( adr >= memory_low && adr+MAX_STRING_SIZE < memory_high ) {
  #endif
    uint32 pos; array_uidx mem;
    for( pos = 0, mem = 0; pos < MAX_STRING_SIZE && string[mem] != 0x00; (unicode ? pos+=2 : pos++), mem++ ) {
      *(u8char*)(adr+pos) = string[mem];
    } *(u8char*)(adr+pos) = '\0'; // finalize

  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: setString()", 5.00f);
    MEM_LOG("[ERROR] %lu: setString(0x%X)", getGametime(), adr);
  }
  #endif
}

u8string getString(uaddr adr, bool unicode) { // mode = 1 (where every second is a char)
  static char str[MAX_STRING_SIZE];
  CLEAR_STR(str);

  #if MEMCHECK
  if( adr >= memory_low && adr+MAX_STRING_SIZE < memory_high ) {
  #endif
    array_uidx i = 0;
    for( uint32 pos = 0; pos < MAX_STRING_SIZE && !STR_EMPTY((u8char*)(adr+pos)); (unicode ? pos+=2 : pos++), i++ ) {
      str[i] = *(u8char*)(adr+pos);
    } str[i] = '\0';
  #if MEMCHECK
  } else {
    setTimedTextbox("~r~Error: getString()", 5.00f);
    MEM_LOG("[ERROR] %lu: getString(0x%X)", getGametime(), adr);
  }
  #endif
  return str;
}

#ifdef SAVEDITOR
u8char *getSavedataKey() {
  static u8char key[0x11];
  memset(&key, 0, sizeof(key));

  extern u32 savedatakey;

  if( isInMemBounds(savedatakey + (LCS ? 0 : gp)) ) {
    for( int i = 0; i < 0x10; i++ )
      key[i] = *(u8char*)(savedatakey + (LCS ? 0 : gp) + i);
    key[0x10] = 0x00;
  }

  DEBUG_LOG("gamekey: '%s'", key);

  return key;
}
#endif

/////////////////////////////////////////////////

static SceInt64 after_teleport_fix_time;
static SceInt64 after_teleport_fix = 0;

/** Will teleport the player (or vehicle player is in) to coordinates
 *
 * @param x - x world coordinate
 * @param y - y world coordinate
 * @param z - z world coordinate (height)
 */
void SetPlayerPosition(float x, float y, float z) {
  if( pcar ) {
    setFloat(pcar+0x30, x);
    setFloat(pcar+0x34, y);
    setFloat(pcar+0x38, z + 1.0); // a little higher so that car doesn't get stuck in ground or/and bounce away
    setFloat(pcar + (LCS ? 0x78 : 0x148), 0.0f); // up/down drifting to zero

    after_teleport_fix = 1;
    after_teleport_fix_time = getGametime();

  } else {
    setFloat(pplayer+0x30, x);
    setFloat(pplayer+0x34, y);
    setFloat(pplayer+0x38, z);
  }
}
void SetPlayerPositionFixForVehicle() { // teleporting with vehicle behaviour fix
  if( after_teleport_fix && pcar ) {
    if ( getGametime() >= after_teleport_fix_time + 500 ) { //0.5 sec
      after_teleport_fix = 0;
      after_teleport_fix_time = 0;

    } else {
      /// zeroing spinning/moving vectors
      setFloat(pcar +  0x8, 0.0f);
      setFloat(pcar + 0x18, 0.0f);
      setFloat(pcar + 0x70, 0.0f);
      setFloat(pcar + 0x74, 0.0f);

    if( LCS ) {
        //setFloat(pcar + 0x78, 0.0f); // not allowed so that vehicle falls to ground while other vectors are still zeroed
        setFloat(pcar + 0x80, 0.0f);
        setFloat(pcar + 0x84, 0.0f);
        setFloat(pcar + 0x88, 0.0f);

      } else if( VCS ) {
        setFloat(pcar + 0x78, 0.0f);
        setFloat(pcar + 0x140, 0.0f);
        setFloat(pcar + 0x144, 0.0f);
        //setFloat(pcar + 0x148, 0.0f); // not allowed so that vehicle falls to ground while other vectors are still zeroed
      }
    }
  }
}

/// Pedestrian ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float getPedFacingDirectionInDegree(uaddr ped_base_adr) {
  return ((180.0f / M_PI) * getFloat(ped_base_adr + (LCS ? 0x4E0 : 0x8D0)) + ((getFloat(ped_base_adr + (LCS ? 0x4E0 : 0x8D0)) < 0.00f) ?  360.0f : 0.0f) ); //rad -pi through 0 to +pi !!!!!   -> degree
}

uint8 getMaxHealthMultiplier() {
  return getByte(global_maxhealthmult);
}

void setMaxHealthMultiplier(uint8 value) {
  setByte(global_maxhealthmult, value);
}

uint8 getMaxArmorMultiplier() {
  return getByte(global_maxarmormult);
}

void setMaxArmorMultiplier(uint8 value) {
  setByte(global_maxarmormult, value);
}

bool getUnlimitedSprintStatus() {
  return getByte(global_unlimtedsprint);
}

void setUnlimitedSprintStatus(bool value) {
  setByte(global_unlimtedsprint, value);
}

bool getUnlimitedSwimStatus() {
  return getByte(global_unlimtedswim);
}

void setUnlimitedSwimStatus(bool value) {
  setByte(global_unlimtedswim, value);
}

int getWantedLevel(uaddr ped_base_adr) {
  return getInt(ped_base_adr + (LCS ? 0x830 : 0x910));
}

int getMediaAttentionValue(uaddr ped_base_adr) {
  return getInt(ped_base_adr + (LCS ? 0x810 : 0x8F0));
}


/**********************************************************************************************************************************
 * Set the Health & Armor Value for a Pedestrian (incl. player)
 *
 * uaddr ped_base_adr   - pedestrian object memory location
 * float health     - 0.0f - 100.0f + x
 * float armor       - 0.0f - 100.0f + x
 *
 * Notes:   Setting health value to 0.0f will kill PED!
**********************************************************************************************************************************/
void setPedHealthAndArmor(uaddr ped_base_adr, float health, float armor) {
  if( health >= 0 ) setFloat(ped_base_adr+(LCS ? 0x4b8 : 0x4E4), health); // health
  if( armor >= 0 ) setFloat(ped_base_adr+(LCS ? 0x4bc : 0x4E8), armor); // armor
}

float getPedHealth(uaddr ped_base_adr) {
  return getFloat(ped_base_adr + (LCS ? 0x4B8 : 0x4E4));
}

float getPedArmor(uaddr ped_base_adr) {
  return getFloat(ped_base_adr + (LCS ? 0x4BC : 0x4E8));
}

void setPedNeverOnFire(uaddr ped_base_adr) {
  setInt(ped_base_adr + (LCS ? 0x6DC : 0x6B0), 0); // zero out addr to fire
}

void setPedInvisible(uaddr ped_base_adr, bool boolean) {
  if( LCS ) setBit(ped_base_adr+0x19A, 7, boolean ? ON : OFF); // old: setByte(ped_base_adr+0x19A, (boolean) ? 0x82 : 0x02 );
  if( VCS ) setShort(ped_base_adr+0x4C, boolean ? 0x2010 : 0x0); // bitflags pendant to LCS would be at 0x1CF but behaves differently in VCS!
}

void setPedIgnoredByEveryone(uaddr ped_base_adr, bool boolean) {
  setByte(ped_base_adr + (LCS ? 0x82E : 0x90E), boolean ? 0x2 : 0x0);
}

void setPedCanBeTargeted(uaddr ped_base_adr, bool boolean) {
  setByte(ped_base_adr + (LCS ? 0x1a0 : 0x0), boolean ? 0x0 : 0x2); // TODO: vcs
}

void setPedFlagToUnload(uaddr ped_base_adr) {
  if( LCS ) setByte(ped_base_adr+0x48, 1);
  // TODO: VCS
}

void setPedOrVehicleFreezePosition(uaddr base_adr, bool boolean) {
  if( LCS ) setByte(base_adr + 0x143, boolean ? 0x1 : 0x0);
  if( VCS ) setByte(base_adr + 0xEF, boolean ? 0x80 : 0x0);
}



/**********************************************************************************************************************************
 * Set the Mass Value for a Pedestrian (incl. player)
 *
 * uaddr ped_base_adr   - pedestrian object memory location
 * float mass1       - player default is 70.0f
 * float mass2       - player default is 100.0f
 *
 * Notes:   Will affect jumping height!
**********************************************************************************************************************************/
void setPedMass(uaddr ped_base_adr, float mass1, float mass2) {
  setFloat(ped_base_adr+0xD0, mass1);
  setFloat(ped_base_adr+0xD4, mass2);
}

void setPedAmmoForWeapon(uaddr ped_base_adr, int type, int loaded, int total) {
    switch(type) {

    case WEAPON_GRENADE:
        setInt(ped_base_adr + (LCS ? 0x5D8: 0x5BC), total); // (int) grenades total
        break;

    case WEAPON_HANDGUN:
        setInt(ped_base_adr + (LCS ? 0x5f0 : 0x5D4), loaded); // (int) handgun ammo loaded
        setInt(ped_base_adr + (LCS ? 0x5f4 : 0x5D8), total);  // (int) handgun ammo total
        break;

      case WEAPON_SHOTGUN:
        setInt(ped_base_adr + (LCS ? 0x60c : 0x5f0), loaded); // (int) shotgun ammo loaded
        setInt(ped_base_adr + (LCS ? 0x610 : 0x5f4), total);  // (int) shotgun ammo total
        break;

      case WEAPON_SMG:
        setInt(ped_base_adr + (LCS ? 0x628 : 0x60c), loaded); // (int) smg ammo loaded
        setInt(ped_base_adr + (LCS ? 0x62c : 0x610), total);  // (int) smg ammo total
        break;

      case WEAPON_ASSAULT:
        setInt(ped_base_adr + (LCS ? 0x644 : 0x628), loaded); // (int) asault ammo loaded
        setInt(ped_base_adr + (LCS ? 0x648 : 0x62c), total);  // (int) asault ammo total
        break;

      case WEAPON_ROCKET:
        setInt(ped_base_adr + (LCS ? 0x660 : 0x644), loaded); // (int) rocket ammo loaded
        setInt(ped_base_adr + (LCS ? 0x664 : 0x648), total);  // (int) rocket ammo total
        break;

      case WEAPON_SNIPER:
        setInt(ped_base_adr + (LCS ? 0x67c : 0x660), loaded); // (int) sniper ammo loaded
        setInt(ped_base_adr + (LCS ? 0x680 : 0x664), total);  // (int) sniper ammo total
        break;

      case WEAPON_CAMERA:
        setInt(ped_base_adr + (LCS ? 0x698 : 0x67C), loaded); // (int) camera ammo loaded
        setInt(ped_base_adr + (LCS ? 0x69C : 0x680), total);  // (int) camera ammo total
        break;

    }

}

uint8 getPedsCurrentAnimation() { // eg: 0x12 is entering vehicle
  return getByte(pplayer + (LCS ? 0x250 : 0x894));
}

int getPedsCurrentWeapon(uaddr ped_base_adr) {
  int currentWeaponSlot = getByte(ped_base_adr + (LCS ? 0x6B8 : 0x789));
  return getInt(ped_base_adr + (LCS ? 0x594 : 0x578) + (currentWeaponSlot * 0x1C));
}

ushort getPedID(uaddr ped_base_adr) {
  if( ped_base_adr ) return (ushort)getShort(ped_base_adr+0x58);
  else return 0xFFFF; // unknown
}

uaddr getPedsVehicleObjectAddress(uaddr base_adr) { // there will be address if ped is inside a vehicle
  return (uaddr)getInt(base_adr + (LCS ? 0x534 : 0x480));
}

uaddr getObjectsTouchedObjectAddress(uaddr base_adr) { // there will be address if ped/vehicle touches other object (0 if nothing touched)
  return (uaddr)getInt(base_adr + (LCS ? 0x12C : 0x150));
}

uint8 getTypeFromAddress(uaddr address) { // eg: if the address belongs to a vehicle object it returns VEHICLE
  if( address >= peds_base && address <= (peds_base + (var_pedobjsize * peds_max)) ) {
    return PEDESTRIAN;
  }
  if( address >= vehicles_base && address <= (vehicles_base + (var_vehobjsize * vehicles_max)) ) {
    return VEHICLE;
  }
  if( address >= worldobjs_base && address <= (worldobjs_base + (var_wldobjsize * worldobjs_max)) ) {
    return WORLDOBJ;
  }
  return FUNC_ERROR; // unknown
}

/// replaced by IDE names
/* char *getPedModelByID(char id) { // 0x00 player, 0x01 Cop...
  for( int i = 0; i <= (LCS ? lcs_charactersize : vcs_charactersize); i++ ) {
    if( id == (char)(LCS ? lcs_characters[i].id : vcs_characters[i].id) )
      return (void *)(LCS ? lcs_characters[i].model : vcs_characters[i].model);
  }
  return "unknown";

}
char *getPedNameByID(char id) { // 0x00 "Toni Cipriani", 0x17 "Mona Lott"...
  for( int i = 0; i <= (LCS ? lcs_charactersize : vcs_charactersize); i++ ) {
    if( id == (char)(LCS ? lcs_characters[i].id : vcs_characters[i].id) )
      return (void *)(LCS ? lcs_characters[i].name : vcs_characters[i].name);
  }
  return "unknown";
} */


bool getPedObjectIsActive(uaddr ped_base_adr) {
  // Better way of doing it? Don't need "getInt" func for it
  return isInMemBounds(*(uaddr*)(ped_base_adr + 0x40));
}

usize getPedActiveObjects(uaddr peds_base, usize peds_max, usize peds_size) {
  usize ret = 0;

  for( array_uidx i = 0; i < peds_max; peds_base+=peds_size, i++ )
    if( getPedObjectIsActive(peds_base) ) ret++;

  return ret;
}

bool checkPedIsInWater(uaddr ped_base_adr) { // TODO: VCS
  return ((getByte(ped_base_adr + 0x142) & 0x10) != 0); // true
}

bool getPedDrowning(uaddr ped_base_adr) { // not in car!
  if( LCS )
    return (getInt(ped_base_adr + 0x19c) & 0x10000) != 0;
  if( VCS )
    return getInt(ped_base_adr + 0x8b4) == 0x44; // 0x3A if drowned ?!
  
  return false;
}

/// Vehicles ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16 getVehicleID(uaddr vehicle_base_adr) {
  if( vehicle_base_adr ) return getShort(vehicle_base_adr+0x58);
  else return 0; // not in car
}

uint16 getVehicleTypeByID(uint16 id) { // pcar_type
  uint8 flag = getNibbleLow( getAddressOfHandlingSlotForID(id) + (LCS ? 0xD2 : 0xD3) );
  switch( flag ) { // 1: IS_BIKE    2: IS_HELI        4: IS_PLANE      8: IS_BOAT
    case 1: return VEHICLE_BIKE;
    case 2: return VEHICLE_HELI;
    case 4: return VEHICLE_PLANE;
    case 8: return VEHICLE_BOAT;
    default: return VEHICLE_CAR;
  }
}

uint8 lcs_getVehicleColorBase(uaddr vehicle_base_adr) { // returns color if valid else -1
  if( vehicle_base_adr ) return getByte(vehicle_base_adr+0x1F0);
  return 0;
}

uint8 lcs_getVehicleColorStripe(uaddr vehicle_base_adr) { // returns color if valid else -1
  if( vehicle_base_adr ) return getByte(vehicle_base_adr+0x1F1);
  return 0;
}

void lcs_setVehicleColorBase(uaddr vehicle_base_adr, uint8 x) {
  setByte(vehicle_base_adr+0x1F0, x);
}

void lcs_setVehicleColorStripe(uaddr vehicle_base_adr, uint8 x) {
  setByte(vehicle_base_adr+0x1F1, x);
}

CRGBA vcs_getVehicleColorBase(uaddr vehicle_base_adr) {
  if( vehicle_base_adr ) return COLOR32(getInt(vehicle_base_adr+0x224));
  return COLOR32(0);
}

void vcs_setVehicleColorBase(uaddr vehicle_base_adr, CRGBA color) {
  setInt(vehicle_base_adr+0x224, color.color32);
}

CRGBA vcs_getVehicleColorStripe(uaddr vehicle_base_adr) {
  if( vehicle_base_adr ) return COLOR32(getInt(vehicle_base_adr+0x228));
  return COLOR32(0);
}

void vcs_setVehicleColorStripe(uaddr vehicle_base_adr, CRGBA color) {
  setInt(vehicle_base_adr+0x228, color.color32);
}

void setVehicleHealth(uaddr vehicle_base_adr, float value) {
  if( VCS && mod_text_size == 0x00377D30 ) { // ULUS v1.01
    setFloat(vehicle_base_adr + 0x29C, value);
	return;
  }
  setFloat(vehicle_base_adr + (LCS ? 0x268 : 0x27C), value);
}

float getVehicleHealth(uaddr vehicle_base_adr) {
  if( VCS && mod_text_size == 0x00377D30 ) { // ULUS v1.01
    return getFloat(vehicle_base_adr + 0x29C);
  }
  return getFloat(vehicle_base_adr + (LCS ? 0x268 : 0x27C));
}

void setVehicleNoPhysicalDamage(uaddr vehicle_base_adr) {
  setByte(vehicle_base_adr + (LCS ? 0x140 : 0x132), 0x0);
}

void makeVehicleExplode(uaddr vehicle_base_adr) {
  void (*makeVehExplode)(int vehicle_obj, int player_or_zero, int vcs_only); // if pplayer: it counts for stats
  makeVehExplode = (void*)( getInt(getInt(vehicle_base_adr + 0x5c) + (LCS ? 0x11c : 0x14C)));
  makeVehExplode(vehicle_base_adr, 0 /*or pplayer*/, 0);

  // ghidra code (game uses different functions for different vehicle types)
  // LCS: (**(code **)(*(int *)(addr + 0x5c) + 0x11c)) (addr + *(short *)(*(int *)(addr + 0x5c) + 0x118),0);
  // VCS: (**(code **)(*(int *)(addr + 0x5c) + 0x14c)) (addr + *(short *)(*(int *)(addr + 0x5c) + 0x148),0,0);
}

void setVehicleRadioStation(uaddr vehicle_base_adr, uint8 id) {
  if( LCS && mod_text_size == 0x0031F854 ) { // ULUX v0.02
    setByte(vehicle_base_adr + 0x29C, id);
    return;
  }
  setByte(vehicle_base_adr + (LCS ? 0x2A0 : 0x2B7), id);
}

void setRadioStationNow(uint8 id) {
  setByte(global_radioarea + (LCS ? 0x345 : 0x56D), id); // station id
  setByte(global_radioarea + (LCS ? 0x344 : 0x56C), 1); // do it now
}

void setVehicleGravityApplies(uaddr vehicle_base_adr, bool boolean) { // 0xX2 = gravity = true, 0xX1 = no gravity = false
  setNibbleLow(vehicle_base_adr + (LCS ? 0x142 : 0xEC), boolean ? 0x2 : 0x1);
}

void setVehicleWheelCamber(uaddr vehicle_base_adr, float val) {
  setFloat(vehicle_base_adr + (LCS ? 0x618 : 0x6A8), val);
}

float getVehicleWheelCamber(uaddr vehicle_base_adr) {
  return getFloat(vehicle_base_adr + (LCS ? 0x618 : 0x6A8));
}

float getVehicleSpeed() {
  //return getFloat(vehicle_base_adr + (LCS ? 0x124 : 0x108)); // calculated by game (up/down counts too) (has some weird multiplier?! (probably gamespeed) Which is also different for LCS vs VCS?!?! its ALSO not consistent and is calculated wrong with higher speeds sometimes...)
  return sqrtf((getFloat(pcar+(LCS?0x70:0x140)) * getFloat(pcar+(LCS?0x70:0x140))) + (getFloat(pcar+(LCS?0x74:0x144)) * getFloat(pcar+(LCS?0x74:0x144))) + (getFloat(pcar+(LCS?0x78:0x148)) * getFloat(pcar+(LCS?0x78:0x148)))); // doing it myself then!! sqrtf( x^2 + y^2 + z^2 )
}

uint8 getVehicleCurrentGear(uaddr vehicle_base_adr) {
  if( LCS && mod_text_size == 0x0031F854 ) // ULUX v0.02
    return getByte(vehicle_base_adr + 0x26C);
  
  return getByte(vehicle_base_adr + (LCS ? 0x270 : 0x284));
}

void setVehicleFlagToUnload(uaddr vehicle_base_adr) { // aka: mark no longer needed
  if( LCS ) setByte(vehicle_base_adr+0x48, 1);
}

void setVehicleDoorsLocked(uaddr vehicle_base_adr, bool boolean) { // TODO: vcs proto
  setByte(vehicle_base_adr + (LCS ? 0x294 : 0x2A8), (boolean) ? 0x02 : 0x01); // 0x2 is locked, 0x1 is normal
}

int getVehicleAiStatus(uaddr vehicle_base_adr) { // 3 = "Physiscs", 2 = "floating" / AI controlled
  return (getInt(vehicle_base_adr + (LCS ? 0x44 : 0x48)) & 0x1f0) >> 4;
}

void setVehicleMakePhysical(uaddr vehicle_base_adr) { // like game does it "FLOATING CAR TURNED INTO PHYSICS CAR"
  setInt(vehicle_base_adr + (LCS ? 0x44 : 0x48), (getInt(vehicle_base_adr + (LCS ? 0x44 : 0x48)) & 0xfffffe0f) | 0x30 ); // set 3x xx xx xx
}

void setVehicleRepairTyres(uint16 vehicle_type) {
  // short vehicle_type = getVehicleTypeByID(getVehicleID(vehicle_base_adr)); //use gloabl instead? -> yes
  if( vehicle_type == VEHICLE_CAR ) {
    // indeflatable  car tyres
    setByte(pcar + (LCS ? 0x356 : 0x3B6), 0x00);
    setByte(pcar + (LCS ? 0x357 : 0x3B7), 0x00);
    setByte(pcar + (LCS ? 0x358 : 0x3B8), 0x00);
    setByte(pcar + (LCS ? 0x359 : 0x3B9), 0x00);

  } else if( vehicle_type == VEHICLE_BIKE ) {
    // indeflatable  bike tyres
    setByte(pcar + (LCS ? 0x3F8 : 0x434), 0x00);
    setByte(pcar + (LCS ? 0x3F9 : 0x435), 0x00);
  }
}

void setPedExitVehicleNow(uaddr ped_base_adr) {
  if( LCS )
    if( getByte(ped_base_adr+0x538) == 0x1 ) // is in vehicle bool
      setByte(ped_base_adr+0x250, 0x10); // trigger exiting

  if( VCS )
    setByte(ped_base_adr+0x894, 0x10); // trigger exiting
}

bool isVehicleInWater(uaddr vehicle_base_adr) { // TODO: VCS
  if( LCS ) if( getByte(vehicle_base_adr+0x142) & 0x10 ) return true;
  //if( VCS ) if( getByte(vehicle_base_adr+0xED) ) return 1; //different for different vehicles???
  if( VCS ) return getByte(vehicle_base_adr+0xED);

  return false;
}

bool isVehicleInAir(uaddr vehicle_base_adr) {
  return getByte(vehicle_base_adr+ (LCS ? 0x10A : 0x136)) == 0;
}

bool isVehicleUpsideDown(uaddr vehicle_base_adr UNUSED) {
  return getFloat(pcar+0x28) < -0.85; // -0.00 would be on side of car | -1 completely on roof
}

bool getVehicleObjectIsActive(uaddr vehicle_base_adr) {
  return (getShort(vehicle_base_adr+0x62) != -1 && getInt(vehicle_base_adr+0x1C) == 0); // TODO: ugly? okay?
}

usize getVehicleActiveObjects(uaddr vehicle_base, usize vehicle_max, usize vehicle_size) {
  usize ret = 0;
  for( array_uidx i = 0; i < vehicle_max; vehicle_base+=vehicle_size, i++ ) {
    if( getVehicleObjectIsActive(vehicle_base) )
      ret++;
  }
  return ret;
}

float getBmxJumpMultiplier() { // VCS only
  return getFloat(global_bmxjumpmult+gp);
}

void setBmxJumpMultiplier(float value) { // VCS only
  setFloat(global_bmxjumpmult+gp, value);
}

/// world objects ///////////
bool getWorldObjectIsActive(uaddr worldobjs_base_adr) {
  return getInt(worldobjs_base_adr+0x64) == 0x42C80000;
}

usize getWorldActiveObjects(uaddr worldobjs_base, usize worldobjs_max, usize worldobjs_size) {
  usize ret = 0;
  for( array_uidx i = 0; i < worldobjs_max; worldobjs_base+=worldobjs_size, i++ ) {
    if( getWorldObjectIsActive(worldobjs_base) )
      ret++;
  }
  return ret;
}

int getVehicleDriverHandle(uaddr vehicle_base_adr) {
  return getInt(vehicle_base_adr + (LCS ? 0x1F8 : 0x384) );
}

int getVehiclePassengerHandle(uaddr vehicle_base_adr) {
  return getInt(vehicle_base_adr + (LCS ? 0x1FC : 0x388) );
}

int getVehicleBackseatLeftHandle(uaddr vehicle_base_adr) {
  return getInt(vehicle_base_adr + (LCS ? 0x200 : 0x38C) );
}

int getVehicleBackseatRightHandle(uaddr vehicle_base_adr) {
  return getInt(vehicle_base_adr + (LCS ? 0x204 : 0x390) );
}

void setHeliHeightLimit(float height) {
  int local = 0;
  memcpy(&local, &height, sizeof(int));
  local >>= 16;

  //DEBUG_LOG("local = 0x%08X, upper = 0x%04X", local, upper);
  setShort(addr_heliheight, (short)local); // 80.0f is default

  /// In ARK-4 when high memory is enabled the height value was out of the calculated bounds (and thus getInt fails) -> this should work now but I reworked it anyways
  //int local = getInt((int)&height) >> 16; // only upper part of float needed eg: 80.0f = 0x42A0
  //setShort(addr_heliheight, local); // 80.0f is default

  /// set the full instruction
  //int local = getShort(addr_heliheight + 2) * 0x10000 + (getInt((int)&height) >> 16);
  //setInt(addr_heliheight, local); // set the full instruction (and not individual bytes) is an easy fix for ppsspp jit problem apparently (Update: apparently not)

  clearICacheFor(addr_heliheight); // needed for PPSSPP
}

#ifdef PREVIEW
void setPoliceChaseHeliModel(short model) {
  setShort(addr_policechaseheli_1, model);
  clearICacheFor(addr_policechaseheli_1); // needed for PPSSPP

  setShort(addr_policechaseheli_2, model);
  clearICacheFor(addr_policechaseheli_2); // needed for PPSSPP

  setShort(addr_policechaseheli_3, model);
  clearICacheFor(addr_policechaseheli_3); // needed for PPSSPP
}
#endif

/// Pickups ///////////
bool getPickupIsActive(uaddr pickup_base_adr) { // slot is in use (some pickups are one-time pickups others like weapons respawn after some time! -> slot is still used although nothing in world)
  return getByte(pickup_base_adr+(LCS?0x32:0x38)) != 0; // just like ghidra
}

bool getPickupIsCollectable(uaddr pickup_base_adr) { // slot is used by pickup currently collectable in world
  return (bool)getInt(pickup_base_adr+(LCS?0x14:0x20)) != 0 || getInt(pickup_base_adr+(LCS?0x18:0x24)) != 0; // just like ghidra
}

uint16 getPickupID(uaddr pickup_base_adr) {
  return getShort(pickup_base_adr+(LCS?0x26:0x34));
}

float getPickupCoordX(uaddr pickup_base_adr) {
  return getFloat(pickup_base_adr);
}

float getPickupCoordY(uaddr pickup_base_adr) {
  return getFloat(pickup_base_adr+0x04);
}

float getPickupCoordZ(uaddr pickup_base_adr) {
  return getFloat(pickup_base_adr+0x08);
}

u8string getPickupNameByID(uint16 id) {
  for( array_uidx i = 0; i <= (LCS ? lcs_pickupsize : vcs_pickupsize); i++ ) {
    if( id == (LCS ? lcs_pickups[i].id : vcs_pickups[i].id) )
      return LCS ? lcs_pickups[i].name : vcs_pickups[i].name;
  }
  return "unknown";
}

usize getPickupsActiveObjects(uaddr pickups_base, usize pickups_max, usize pickups_size) {
  usize ret = 0;

  for( array_uidx i = 0; i < pickups_max; pickups_base+=pickups_size, i++)
    if( getPickupIsActive(pickups_base) ) ret++;

  return ret;
}

usize getPickupsActiveObjectsWithID(uint16 id) {
  usize ret = 0;
  uaddr addr = global_pickups;

  for( uaddr i = 0; i < var_pickupslots; addr+=var_pickupslotsize, i++)
    if( getPickupIsActive(addr) && getPickupID(addr) == id ) ret++;

  return ret;
}

iaddr spawnPickup(uint16 id, uint8 type, int16 amount, float x, float y, float z) {
  uint32 cur_slot = 0;
  uaddr cur_offset = 0;

  for( cur_offset = global_pickups; cur_slot < var_pickupslots; cur_offset += var_pickupslotsize, cur_slot++ ) {

    // if() // slots that don't accept pickups for whatever reason
      // continue;

    if( !getPickupIsActive(cur_offset) ) { // no active pickup in this slot
      ///setTimedTextbox("found!", 5.00f);
      setFloat(cur_offset+0x0, x); // world x coordinate
      setFloat(cur_offset+0x4, y); // world y coordinate
      setFloat(cur_offset+0x8, z); // world z coordinate

      setShort(cur_offset+(LCS?0x1C:0x2C), amount); // amount

      setShort(cur_offset+(LCS?0x26:0x34), id); // ID

      setByte(cur_offset+(LCS?0x32:0x38), type); // type 0x3 means one time pickup only

      setInt(cur_offset+0xC, 0); // zero out old pointer to be sure
      setByte(cur_offset+(LCS?0x33:0x39), 0x00); // not yet collected
      setByte(cur_offset+(LCS?0x50:0x3C), 0xFF); // flag?

      if( type == 0x8 ) { // money pickup additional stuff
        setFloat(cur_offset+0xC, 1.0f);
        setByte(cur_offset+(LCS?0x20:0x30), 0x88); // TODO: why? whats this?
        setByte(cur_offset+(LCS?0x21:0x31), 0x88);
        setByte(cur_offset+(LCS?0x22:0x32), 0x88);
      }

      return (iaddr)cur_offset;
    }
  }

  setTimedTextbox("~r~Error: spawnPickup() found no free slot", 5.00f);
  return FUNC_ERROR; // error
}


/// Pickups ///////////

/**
LCS: FUN_00163e40      DAT_00646A20    (_radarblip_color)
  slots = 0x4B
  slotsize = 0x50

VCS: FUN_0000a03c      iGp000016dc ptr + 0x270
  slots = 0x4B
  slotsize = 0x30
*/

bool getMapiconIsActive(uaddr mapicon_base_adr) { // aka. slot is in use (there can be inactive mission icons though!!)
  return (LCS && getByte(mapicon_base_adr+0x33) != 0) || (VCS && getByte(mapicon_base_adr+0x28) != 0); // TODO: like ghidra
}

// aka. something is displayed!
bool getMapiconIsVisible(uaddr mapicon_base_adr) { // TODO: like ghidra
  return ( getMapiconIsActive(mapicon_base_adr) &&
      ((LCS && getByte(mapicon_base_adr+0x3E) > 0)  // display bool: 1 = arrow, 2 = on map, 3 = both
      || (VCS && (getBit(mapicon_base_adr+0x20, 5) ||  // show arrow
            getBit(mapicon_base_adr+0x20, 6)       // display on map
            ) ) ) );
}

usize getMapiconsActiveObjects(uaddr mapicon_base, usize mapicons_max, usize mapicons_size) {
  usize ret = 0;

  for( array_uidx i = 0; i < mapicons_max; mapicon_base+=mapicons_size, i++ )
    if( getMapiconIsActive(mapicon_base) ) ret++;
  
  return ret;
}

uint8 getMapiconID(uaddr mapicon_base_adr) {
  return getByte(mapicon_base_adr+(LCS?0x40:0x29));
}

u8string getMapiconNameByID(uint16 id) {
  static char ret[32];
  uaddr offset = 0;

  id--; // first ptr in table has id 1
  if( LCS ) offset = (uaddr)getInt(ptr_radarIconList + (id*0x4));
  if( VCS ) offset = (uaddr)getInt(getInt(global_radarblips+gp) + ptr_radarIconList + (id*0x4));

  if( isInMemBounds(offset) ) {
    snprintf(ret, sizeof(ret), "%s", getString(offset + 0x10, 0));
  } else snprintf(ret, sizeof(ret), "%i", ++id); // print id instead - no pointer in table here

  return ret;
}

int getMapiconType(uaddr mapicon_base_adr) {
  return getInt(mapicon_base_adr+0x04);
}

u8string getMapiconTypeName(uaddr mapicon_base_adr) { // TODO: VCS
  switch( getMapiconType(mapicon_base_adr) ) {
    case 0: return "Business"; // VCS only
    case 1: return "Vehicle";
    case 2: return "Pedestrian";
    case 3: return "World Object";
    case 4: return "Coordinate";
    case 5: return "Sphere Coordinate";
  }
  return "unknown";
}

uint8 getMapiconColor(uaddr mapicon_base_adr) {
  return getInt(mapicon_base_adr);
}

u8string getMapiconColorName(uaddr mapicon_base_adr) {
  // char buf[16];
  switch( getMapiconColor(mapicon_base_adr) ) {
    case 0: return "Red";
    case 1: return "Green";
    case 2: return "Blue";
    case 3: return "White";
    case 4: return "Yellow";
    case 5: return "Magenta";
    case 6: return "Cyan";
    case 7: return "Brown";
    case 8: return "Black"; // TODO: from here rgba ?``?
  }
  // snprintf(buf, sizeof(buf), "0x%08X", getMapiconColor(mapicon_base_adr)); // R G B A string?
  return "unknown";
}

array_uidx getMapiconLinkedObjectSlotNumber(uaddr mapicon_base_adr) { //return slot of vehicle (starts with 0)
  if( getInt(mapicon_base_adr+0x08) != 0 )
    return (array_uidx)(getInt(mapicon_base_adr+0x08) >> 8);
  return FUNC_ERROR; // error
}

void removeMapIcon(uaddr mapicon_base_adr) {
  uaddr i = mapicon_base_adr;
  for( ; i < mapicon_base_adr+var_radarblipslotsize; i++ )
    setByte(i,0x00);

  setByte(i+(LCS ? 0x30 : 0x2A), 0x01); // set default empty
  setByte(i+(LCS ? 0x35 : 0x2C), 0xFF); // set default empty

  // DEBUG_LOG("removeMapIcon() 0x%08X", mapicon_base_adr);
}

void removeAllMapIcons() {
  for( uaddr i = 0, j = (LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding)); i < var_radarblipslots; j+=var_radarblipslotsize, i++ ) {
    removeMapIcon(j);
  }
}

void removeAllCustomMapIcons() { // zeroes only the custom created ones!!!
  for( uaddr i = 0, j = (LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding)); i < var_radarblipslots; j+=var_radarblipslotsize, i++ ) {
    // DEBUG_LOG("removeAllCustomMapIcons() loop %i 0x%08X", i, j);
    if( getByte(j+(LCS ? 0x4F : 0x2F)) == 0x01 ) { // my watermark to be a custom
      // DEBUG_LOG("its a custom %X", getByte(j+(LCS ? 0x4F : 0x2F)));
      removeMapIcon(j);
    }
  }
}

iaddr createCustomMapIcon(uint8 icon, CRGBA color, bool onlywhenclose, uint8 size, float x, float y, float z) {
  for( uaddr i = 0, j = (LCS ? global_radarblips : (getInt(global_radarblips+gp)+var_radarblipspadding)); i < var_radarblipslots; j+=var_radarblipslotsize, i++ ) {
    if( getMapiconIsActive(j) )
      continue;

    if( LCS ) {
      setInt(j+0x00, color.color32); // color
      setInt(j+0x4, 4); // MapiconType -> 4 = coordinate

      setFloat(j+0x0C, x); // display x coordinate
      setFloat(j+0x10, y); // display y coordinate
      setFloat(j+0x14, z); // display z coordinate
      setFloat(j+0x20, x); // world x coordinate
      setFloat(j+0x24, y); // world y coordinate
      setFloat(j+0x28, z); // world z coordinate

      setByte(j+0x32, 0x01); // brightness (0x00 is more greyed out)
      setByte(j+0x34, onlywhenclose); // minimap (always = 0, in reach = 1)
      setByte(j+0x3E, 0x02); // also necessary to be displayed
      setByte(j+0x33, 0x01); // display bool
      setShort(j+0x3C, (short)size); //
      setShort(j+0x40, (short)icon); // the icon

      setByte(j+0x4F, 0x01); // my indicator that this is a custom icon set by me (for use with lcs_removeAllCustomMapIcons() for example)
    }

    if( VCS ) {
      setInt(j+0x00, color.color32); // color
      setInt(j+0x4, 4); // MapiconType -> 4 = coordinate
      setFloat(j+0x10, x); // display x coordinate
      setFloat(j+0x14, y); // display y coordinate
      setFloat(j+0x18, z); // display z coordinate

      setByte(j+0x20, 0x56); // TODO: bit stuff @ 0x20

      setByte(j+0x28, size); //
      setByte(j+0x29, icon); // the icon

      setByte(j+0x2F, 0x01); // my indicator that this is a custom icon set by me (for use with lcs_removeAllCustomMapIcons() for example)
    }

    return (iaddr)j;
  }
  setTimedTextbox("Error: createCustomMapIcon() Max/no free item slots?!", 5.00f);
  return FUNC_ERROR;
}

usize createCustomMapIconsForPickupsWithID(uint16 id, uint8 icon, CRGBA color) {
  usize counter = 0;
  for( uaddr i = 0, j = global_pickups; i < var_pickupslots; i++, j+= var_pickupslotsize ) {
    if( getPickupID(j) == id && getPickupIsCollectable(j) ) { // && getPickupIsActive(j)
      createCustomMapIcon(icon, color, 0x00, 0x01, getPickupCoordX(j), getPickupCoordY(j), getPickupCoordZ(j));
      counter++;
    }
  }
  return counter;
}

usize createCustomMapIconsForWeaponsInRange(float range) {
  usize counter = 0;
  CRGBA color;
  
  float px = getFloat(pobj+0x30);
  float py = getFloat(pobj+0x34);
  
  uaddr addr = global_pickups;
  for( uaddr i = 0; i < var_pickupslots; i++, addr+= var_pickupslotsize ) {
    if( getPickupID(addr) <= (LCS ? 0x124 : 0x16F ) && getPickupIsCollectable(addr) ) { // check if active

      float x = getPickupCoordX(addr);
      float y = getPickupCoordY(addr);

      if( (px > x-range && px < x+range) && (py > y-range && py < y+range) ) { // check in range to display

        // if( LCS ) { // TODO: VCS

        switch( getPickupID(addr) ) { // decide color

          /** LCS ********* VCS *****/

          /// Slot #1 - meele
          case 0x104:    case 0x14C:
          case 0x105:    case 0x14D:
          case 0x106:    case 0x14E:
          case 0x107:    case 0x14F:
          case 0x108:    case 0x150:
          case 0x109:    case 0x151:
          case 0x10A:    case 0x152:
          case 0x10B:    case 0x153:
          case 0x10C:    case 0x154:
          case 0x10D:    case 0x155:
            color.color32 = 0xCFCFFFFF; // from LCS Multiplayer!
            break;

          /// Slot #2 - Throwable
          case 0x10E:    case 0x156:
          case 0x10F:    case 0x158:
          case 0x110:
          case 0x123:
            color.color32 = 0x9F9FFFFF; // from LCS Multiplayer! (remote explosive has 0x0000001 in MP tough)
            break;

          /// Slot #3 - Handguns
          case 0x112:    case 0x15A:
          case 0x113:    case 0x15B:
            color.color32 = 0xCFFF00FF; // from LCS Multiplayer!
            break;

          /// Slot #4 - Shotgun
          case 0x115:    case 0x15D:
          case 0x116:    case 0x15E:
          case 0x117:    case 0x15F:
            color.color32 = 0x7FFF7FFF; // from LCS Multiplayer!
            break;

          /// Slot #5 - SMG
          case 0x119:    case 0x161:
          case 0x11A:    case 0x162:
          case 0x11B:    case 0x163:
          case 0x11C:    case 0x164:
            color.color32 = 0xFFFF7FFF; // from LCS Multiplayer!
            break;

          /// Slot #6 - Assault
          case 0x114:    case 0x15C:
          case 0x118:    case 0x160:
            color.color32 = 0xFFCF7FFF; // from LCS Multiplayer!
            break;

          /// Slot #7 - Heavy
          case 0x11F:    case 0x167:
          case 0x120:    case 0x168:
          case 0x121:    case 0x169:
          case 0x122:
            color.color32 = 0xFF9FFFFF; // from LCS Multiplayer!
            break;

          /// Slot #8 - Sniper
          case 0x11D:    case 0x165:
          case 0x11E:    case 0x166:
            color.color32 = 0xFF7FCFFF; // from LCS Multiplayer!
            break;

            //health = 0xFFDFDFFF // from LCS Multiplayer!
            //armor = 0xBFFFBFFF // from LCS Multiplayer!

          default:
            color.color32 = 0xFFCFCFCF; // white (rgb made by me)
        }
        createCustomMapIcon(0, color, 0x00, 0x01, x, y, getPickupCoordZ(addr));
        counter++;
      }
    }
  }

  return counter;
}

/// Misc ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8 getClockHours() {
  return getByte(global_clockmultiplier + (LCS ? 0 : gp) + 0x4);
}

uint8 getClockMinutes() {
  return getByte(global_clockmultiplier + (LCS ? 0 : gp) + 0x5);
}

uint8 getClockSeconds() {
  return getByte(global_clockmultiplier + (LCS ? 0 : gp) + 0x6);
}

int16 getClockMultiplier() { // actually int (but we use upper bits for "time freezing")
  return getShort(global_clockmultiplier + (LCS ? 0 : gp));
}

void setClockMultiplier(int16 val) { // actually int (but we use upper bits for "time freezing")
  setShort(global_clockmultiplier + (LCS ? 0 : gp), val);
}

void setClockFreeze(bool boolean) {
  if( !boolean )
    setInt(global_clockmultiplier + (LCS ? 0x74 : gp+0x4E8), getGametime()); // copy back current gametime to some clock global (fix for time skipping after freeze)
  setByte(global_clockmultiplier + (LCS ? 0x3 : gp+0x3), (boolean) ? 0xFF : 0x00); // set huge clock multiplier value so it basically is frozen
}

void setClockTime(uint8 hours, uint8 minutes, uint8 seconds) {
  setByte(global_clockmultiplier + 0x4 + (LCS ? 0 : gp), hours);
  setByte(global_clockmultiplier + 0x5 + (LCS ? 0 : gp), minutes);
  setByte(global_clockmultiplier + 0x6 + (LCS ? 0 : gp), seconds);
}

void setClockHours(uint8 hours) {
  setByte(global_clockmultiplier + 0x4 + (LCS ? 0 : gp), hours);
}

uint32 getGametime() {
  return getInt(global_gametimer + (LCS ? 0 : gp));
}

uint16 getWeather() {
  return getShort(global_weather + (LCS ? 0 : gp)); // first of three only!!!
}


/*****************************************************
Display Settings toggles...

LCS has globals for this
VCS uses global to address!! (address + 0x13 is radar) (address + 0x18 is HUD)
******************************************************/
void setDisplaySettingsToggleRadar(bool boolean) {
  if( LCS ) setByte(global_radarbool, (boolean ? 0x00 : 0x02) ); // set 2 to hide minimap
  if( VCS && getInt(global_displaysettings+gp) > 0)
    setByte(getInt(global_displaysettings+gp)+0x13, (boolean ? 0x00 : 0x02) ); // set 0 to show minimap
}

bool getDisplaySettingsToggleRadar() {
  if( LCS ) return !getByte(global_radarbool);

  if( VCS && getInt(global_displaysettings+gp) > 0)
    return !getByte(getInt(global_displaysettings+gp)+0x13);
  
  return false;
}

void setDisplaySettingsToggleHUD(bool boolean) {
  if( LCS ) setByte(global_hudbool, boolean); // set 0 to hide hud

  if( VCS && getInt(global_displaysettings+gp) > 0)
    setByte(getInt(global_displaysettings+gp)+0x18, boolean); // set 1 to show hud
}

bool getDisplaySettingsToggleHUD() {
  if( LCS ) return getByte(global_hudbool);

  if( VCS && getInt(global_displaysettings+gp) > 0)
    return getByte(getInt(global_displaysettings+gp)+0x18);

  return false;
}

void setFreezeGame(bool boolean) {
  setByte(global_freezegame + (LCS ? 0 : gp), boolean); // set 1 to freeze game
}

void setMapLegendToggle(bool boolean) {
  if( LCS ) setByte(global_maplegendbool, boolean); // set 0 to hide

  if( VCS && getInt(global_displaysettings+gp) > 0)
    setByte(getInt(global_displaysettings+gp)+0x10, boolean); // set 1 to show hud
}

bool getMapLegendToggle() {
  if( LCS ) return getByte(global_maplegendbool);

  if( VCS && getInt(global_displaysettings+gp) > 0)
    return getByte(getInt(global_displaysettings+gp)+0x10);

  return false;
}


/** Create a timed textbox popup (will disappear after time or other textbox)
 *
 * @param sentence - text to be displayed
 * @param duration - time to display box

 *  LCS see "FUN_00182e94_ClearSmallPrints"
 *  VCS gets main global as arg1

 * Calling the game function crashes so i made it this way..
 */
void setTimedTextbox(u8string sentence, float duration) {
  uaddr base = 0;

  if( LCS ) base = global_helpbox_string; // DAT_00649dc0_string (0x8E4DDC0)
  if( VCS ) base = getInt(global_helpbox+gp) + 0x243C; // iGp000016d8 + 0x243C

  Utf8ToGameEncoding(sentence, (wstring)(base));

  if( LCS ) {
    setInt(global_helpbox_timedisplayed, 0);     // reset displaying time                  DAT_0035a808 (0x08B5E808)
    setByte(global_helpbox_displaybool, 0x1);    // display bool                           DAT_0035a810 (0x08B5E810)
    setByte(global_helpbox_permboxbool, 0x0);    // clear permanent box if there is any    DAT_0035a815 (0x08B5E815)
    setFloat(global_helpbox_duration, duration); // set duration                           DAT_0035a818 (0x08B5E818)
    //setByte(global_helpbox_duration, 0xAA);    // set my indicator (to allow custom textbox while in menu and blocked flag) see blockTextBox()
  }
  if( VCS ) {
    setInt(getInt(global_helpbox+gp) + 0x2A3C, 0);          // reset displaying time
    setByte(getInt(global_helpbox+gp) + 0x2a44, 0x1);       // display bool + 0x2a44
    setByte(getInt(global_helpbox+gp) + 0x2a49, 0x0);       // clear permanent box if there is any + 0x2a49
    setFloat(getInt(global_helpbox+gp) + 0x2a4C, duration); // set duration
  }
}

bool isTextboxShowing() {
  if( LCS ) return getByte(global_helpbox_displaybool);
  if( VCS ) return getByte(getInt(global_helpbox+gp) + 0x2a44);
  
  return false;
}

void blockTextBox() {
  if( LCS ) {
    // check if its my custom textbox (allow those)
    //if( getByte(global_helpbox_duration) != 0xAA )  // TODO: WHY DOES THIS NOT WORK?!
      setByte(global_helpbox_displaybool, 0x0);
  }
  if( VCS ) setByte(getInt(global_helpbox+gp) + 0x2a44, 0x0);
}

bool isDialogShowing() {
  if( LCS ) if( getByte(global_dialog_string) != 0 ) return true; // yes
  if( VCS ) if( getByte(getInt(global_helpbox+gp) + 0x18) != 0 ) return true; // yes

  return false; //no
}

void blockDialogs() {
  if( LCS ) return setByte(global_dialog_string, 0x0);
  if( VCS ) return setByte(getInt(global_helpbox+gp) + 0x18, 0x0);
}

bool isHudDisabledWhileCutsceneCamera() { // returns 1 if HUD is disabled while cutscene is playing
  if( LCS ) return !getByte(global_hudincutscene); // is 0x0 when in cutscene to disable HUD (in normal mode 0x1)
  if( VCS ) return getByte(global_hudincutscene); // is 0x1 when in cutscene to disable HUD (in normal mode 0x0)

  return false;
}

int32 getIsland() {
  // 1 = Portland, 2 = Staunton, 3 = ShoresideVale
  // 1 = Vice Beach (right), 2 = Mainland (left), 3 = Stadium
  return getInt(global_currentisland + (LCS ? 0 : gp));
}

int32 getLanguage() { // 0 = English, 1 = French, 2 = German, ....
  return getInt(global_systemlanguage + (LCS ? 0 : gp));
}

bool getMultiplayer() {
  return (bool)getInt(global_ismultiplayer + (LCS ? 0 : gp));
}

void setWindClipped(float wind) {
  setFloat(global_WindClipped + (LCS ? 0 : gp), wind);
}

float getWind() {
  return getFloat(global_Wind + (LCS ? 0 : gp));
}

void setWaterLevel(float level) {
  uaddr levels = (uaddr)getInt(getInt(LCS ? global_ptr_water : global_ptr_water+gp)); // number of floats in waterlevel area
  uaddr waterlevels = (uaddr)getInt(getInt( (LCS ? global_ptr_water : global_ptr_water+gp))+0x4 ); // water level height floats
  uaddr startofwater = (uaddr)getInt(LCS ? global_ptr_water : global_ptr_water+gp) + 0xC;

  for( uaddr i = waterlevels, j = 0; j < levels; i += 0x04, j++ )  // set waterlevel(s)
    setFloat(i, level);

  for( uaddr i = startofwater; i < startofwater+0x5000; i += 0x01 ) // set water everywhere
    if( *(uint8*)i == 0x80 )
      *(uint8*)i = LCS ? 0x1F : 0x00; // TODO: reset for VCS
}

void resetWaterLevel() {
  uaddr levels = (uaddr)getInt(getInt(LCS ? global_ptr_water : global_ptr_water+gp)); // number of floats in waterlevel area
  uaddr waterlevels = (uaddr)getInt(getInt( (LCS ? global_ptr_water : global_ptr_water+gp))+0x4 ); // water level height floats
  uaddr startofwater = (uaddr)getInt(LCS ? global_ptr_water : global_ptr_water+gp) + 0xC;

  for( uaddr i = waterlevels, j = 0; j < levels; i += 0x04, j++ ) { // set waterlevel(s)
    if( LCS && j >= 12 && j <= 17 )
      setFloat(i, 63.20f); // water behind dam 0x427CCCCD
    else
      setFloat(i, LCS ? 0.00f : 6.00f); // VCS = 0x40C00000
  }

  for( uaddr i = startofwater; i < startofwater+0x5000; i += 0x01 ) // set water everywhere
    if ( *(uint8*)i == 0x1F )
      *(uint8*)i = 0x80;
}


void setLastButtonPressedInHistory(uint8 button) {
  for( int i = 11; i >= 0; i-- )
    setByte(global_buttoninput + (LCS ? 0x95 : 0xA9) + i, getByte(global_buttoninput + (LCS ? 0x95 : 0xA9) + i - 1)); // move previous
  
  setByte(global_buttoninput + (LCS ? 0x95 : 0xA9), button); // write newest
}

void activateCheatCode(uint8 a, uint8 b, uint8 c, uint8 d, uint8 e, uint8 f, uint8 g, uint8 h) {
  DEBUG_LOG("activateCheatCode(%X %X %X %X %X %X %X %X )", a, b, c, d, e, f, g, h);
  setLastButtonPressedInHistory(a);
  setLastButtonPressedInHistory(b);
  setLastButtonPressedInHistory(c);
  setLastButtonPressedInHistory(d);
  setLastButtonPressedInHistory(e);
  setLastButtonPressedInHistory(f);
  setLastButtonPressedInHistory(g);
  key_to_pad = h;  // last input must be triggered (via code hex magic)
  //AddToCheatString(global_buttoninput, h); //or (calling function) void (*AddToCheatString)(int pad, char key);
}


/// Vehicle SPawns ///////////

bool getVehicleWorldSpawnSlotIsActive(uaddr base) {
  return (bool)getInt(base);
}

usize getVehicleWorldSpawnSlotActiveObjects(uaddr base) {
  usize ret = 0;

  for( uaddr i = 0; i < var_vehiclesworldspawnslots; base+=var_vehiclesworldspawnslotsize, i++) {
    if( getVehicleWorldSpawnSlotIsActive(base) )
      ret++;
  }

  return ret;
}

uaddr getVehicleWorldSpawnSlotAddress(uint32 slot) { // slots start with 0!!!
  if(slot > var_vehiclesworldspawnslots) return 0;
  return addr_vehiclesworldspawn + (var_vehiclesworldspawnslotsize * slot);
}

bool isCustomParkedVehicleSpawnViaSlot(uint32 slot) {
  uaddr addr = getVehicleWorldSpawnSlotAddress(slot);
  return (getByte(addr + (LCS ? 0x2B : 0x2F)) == 0x42);
}

uaddr createParkedVehicleSpawnViaSlot(uint32 slot, uint16 id, float x, float y, float z, float deg, int16 color1, int16 color2, bool alarm, int16 door_lock) {
  if( slot > var_vehiclesworldspawnslots ) return 0;

  uaddr addr = getVehicleWorldSpawnSlotAddress(slot);

  setInt(addr, id);
  setFloat(addr+0x4, x);
  setFloat(addr+0x8, y);
  setFloat(addr+0xC, z);
  setFloat(addr+0x10, deg);

  if( LCS ) {
    setShort(addr+0x14, color1); // -1 for random
    setShort(addr+0x16, color2); // -1 for random

  }
  if( VCS ) {
    setInt(addr+0x14, (int)color1); // -1 for random
    setInt(addr+0x18, (int)color2); // -1 for random

  }

  setByte(addr + (LCS ? 0x19 : 0x1D), alarm); // LCS: 0, 10, 20, 30, .. 100
  setShort(addr + (LCS ? 0x1A : 0x1E), door_lock); // LCS: 0, 1, 2, 5, 10, 20, 30, 40, 50, 70


  /// LCS 0x24 >> 8 = slot number when spawned
  ////////////////////////////////////////
  setByte(addr + (LCS ? 0x18 : 0x1C), 0x01); // force_spawn
  ////////////////////////////////////////

  // these are set by game but /////////////////
  setInt(addr + (LCS ? 0x20 : 0x24), 0x00000000); // later gametime difference until respawn
  setInt(addr + (LCS ? 0x24 : 0x28), 0xFFFFFFFF); // once vehicle is spawned this will be the slot number the vehicle is loaded to ( >> 8 )

  setShort(addr + (LCS ? 0x28 : 0x2C), 0xFFFF); // allowed to spawn (inside time window which is different for slots)
  setByte(addr + (LCS ? 0x2A : 0x2E), 0x00); // allowed to spawn (enough time since last spawn passed and nothing blocking in world)
  //////////////////////////////////////////////

  if( id > 0 )
    setByte(addr + (LCS ? 0x2B : 0x2F), 0x42); // my custom flag indicator
  else
    setByte(addr + (LCS ? 0x2B : 0x2F), 0x00); // clear

  return addr;
}

uaddr createParkedVehicleSpawnViaReplaceID(uint16 replace_id UNUSED, uint16 id UNUSED, float x UNUSED, float y UNUSED, float z UNUSED, float deg UNUSED, int16 color1 UNUSED, int16 color2 UNUSED, bool alarm UNUSED, int16 door_lock UNUSED) {
  /// TODO:
    // int slot = ....
    // return createParkedVehicleSpawnViaSlot(slot, id, x, y, z, deg, color1, color2, alarm, door_lock);
  return FUNC_ERROR;
}

/// Garage ////////////////////////////////
bool getGarageVehicleSlotIsActive(uaddr base) {
  return (bool)getShort(base);
}

usize getGarageVehicleActiveObjects(uaddr base) {
  usize ret = 0;

  for( uaddr i = 0; i < var_garageslots; base+=var_garageslotsize, i++ ) {
    if( getGarageVehicleSlotIsActive(base) ) ret++;
  }

  return ret;
}

void createGarageVehicle(uaddr garage_base_adr, uint16 id, float x, float y, float z, float vec1, float vec2, float vec3, uint8 flags1, uint8 flags2, uint8 radio, uint8 var1, uint8 var2) { // TODO: replace vector with degree?
  setShort(garage_base_adr, id);
  setFloat(garage_base_adr+0x04, x);
  setFloat(garage_base_adr+0x08, y);
  setFloat(garage_base_adr+0x0C, z);
  setFloat(garage_base_adr+0x10, vec1);
  setFloat(garage_base_adr+0x14, vec2);
  setFloat(garage_base_adr+0x18, vec3);
  setFloat(garage_base_adr+0x1C, 1.00f); // traction mult
  setByte(garage_base_adr+0x20, flags1);
  setByte(garage_base_adr+0x21, flags2);
  if( LCS ) { // colors
    setByte(garage_base_adr+0x24, 0); // black
    setByte(garage_base_adr+0x25, 0); // black
  } else {
    setInt(garage_base_adr+0x24, 0xFFFFFFFF); // white
    setInt(garage_base_adr+0x28, 0xFFFFFFFF); // white
  }
  setByte(garage_base_adr+(LCS ? 0x26 : 0x2C), radio);
  setByte(garage_base_adr+(LCS ? 0x27 : 0x2D), var1);
  setByte(garage_base_adr+(LCS ? 0x28 : 0x2E), var2);
}

int8 getIdeType(array_uidx id) {
  uaddr adr = getAddressOfIdeSlotForID(id);
  if( isInMemBounds(adr) ) return getByte(adr + 0x10);

  return FUNC_ERROR;
}

u8string getIdeTypeName(uint8 no) {
  switch( no ) {
    case 1: return "obj";
    case 3: return "tobj"; // timed object
    case 4: return "weap"; // weapon
    case 5: return "hier";
    case 6: return "cars";
    case 7: return "ped";
  }
  return "unknown";
}

usize getIdeSlotSizeByType(uint8 no) {
  switch( no ) {
    case 1: return LCS ? 0x3C  :  0x40;
    case 3: return LCS ? 0x48  :  0x4C;
    case 4: return LCS ? 0x40  :  0x44;
    case 5: return LCS ? 0x2C  :  0x30;
    case 6: return LCS ? 0x190 : 0x2A0;
    case 7: return LCS ? 0x44  :  0xFC;
  }
  return LCS ? 0x28 : 0x30; // LCS: default size is 0x28 + x (depending on type)
}

array_idx getFirstIdeOfType(int8 type) { // LCS: first for type 'cars' 6 would be 130 (aka Deimos SP)
  array_size slots = getInt(ptr_IDEs+(LCS?0x0:gp));
  for(array_uidx i = 0; i < slots; i++) {
    if( getIdeType(i) == type )
      return (array_idx)i;
  }
  return FUNC_ERROR; // error
}
int getLastIdeOfType(int8 type) { // LCS: last for type 'cars' 6 would be 0xD8 (aka VCN Maverick)
  int slots = getInt(ptr_IDEs+(LCS?0x0:gp));
  for(int i = slots; i > 0; i--) {
    if( getIdeType(i) == type )
      return i;
  }
  return FUNC_ERROR; // error
}

uaddr getAddressOfIdeSlotForID(array_uidx id) {
  uaddr idetable = getInt(ptr_IDETable+(LCS?0x0:gp));
  array_size slots = getInt(ptr_IDEs+(LCS?0x0:gp));

  if( id > slots )
    return FUNC_ERROR; // error

  //if( getInt(idetable + (0x4 * id)) != 0 ) { // there is a ptr here for this id!
    return (uaddr)getInt(idetable + (0x4 * id));
  //}
  //return FUNC_ERROR; // error
}


iaddr getAddressOfHandlingSlotForID(array_uidx id) { // via IDE-table
  /*
  for ( int i = 0; i < (LCS ? lcs_vehiclessize : vcs_vehiclessize); i++ ) {
    if ( id == (LCS ? lcs_vehicles[i].id : vcs_vehicles[i].id) ) {
      return getInt(ptr_handlingCFG) + 0x20 + (var_handlingcfgslotsize * (LCS ? lcs_vehicles[i].hndlng_no : vcs_vehicles[i].hndlng_no));
    }
  } */

  uaddr idetable = getInt(ptr_IDETable+(LCS?0x0:gp));

  if( VCS ) return (iaddr)getInt(getInt(idetable+(id*0x4))+0x38); // VCS has direct pointer to address in ide
  if( LCS ) return (iaddr)getInt(ptr_handlingCFG) + 0x20 + (getByte(getInt(idetable+(id*0x4))+0x42) * 0xF0); // 0x20 is some padding

  return FUNC_ERROR; // error
}

u8string getModelNameViaHash(uint32 hash, uint32 time) {
  /*
  for( int i = 0; i < (LCS ? lcs_namesize : vcs_namesize); i++) {
    if( hash == (LCS ? lcs_names[i].hash : vcs_names[i].hash) )
      return LCS ? lcs_names[i].name : vcs_names[i].name;
  }
  return "unknown"; */

  /// now in thread ///////////////////////////////
  #ifdef NAMERESOLV
  extern uint32 hash_to_check;
  extern uint32 last_hash;
  extern u8char hashbuffer[];
  extern int name_resolver_status;
  if( name_resolver_status > 0 ) { // "name_resolver" thread is alive!
    if( hash == last_hash ) // previously checked
      return hashbuffer;

    if( getGametime() > time ) { // wait before checking ini (makes scrolling in editor faster)
      hash_to_check = hash; // let the thread work
      return "Loading..."; // until then "unknown"
    }
  }
  #endif

  static char buf[16];
  snprintf(buf, sizeof(buf), "0x%08lX", hash);
  return buf;

  //or

  //return "unknown";
}

u8string getModelNameViaID(uint32 id, uint32 time) {
  //int slots = getInt(ptr_IDEs+(LCS?0x0:gp));
  uaddr idetable = getInt(ptr_IDETable+(LCS?0x0:gp));
  return getModelNameViaHash( getInt( getInt(idetable+(id*0x4))+0x8), time); // time = 0 means resolve immediately
}

//extern int (* LoadStringFromGXT)(int gxt_adr,char *string,int param_3,int param_4,int param_5, int param_6,int param_7,int param_8); // hooked

u8string getGxtTranslation(u8string identifier) { // entering "MAFIACR" returns "Leone Sentinel"
  // TODO: understand ARGS??
  return getString(LoadStringFromGXT(getInt(ptr_gxtloadadr + (LCS ? 0 : gp)), identifier, 0x00000002, 0x000000FF, 0x000000FF, 0x00000000, 0x00FF0000, 0x00FFFFFF), 1); // mode 1 -> wide char
}

u8string getRealVehicleNameViaID(uint16 id) { // 0x82 "Deimos", 0x83 "Landstalker" ...
  return getGxtTranslation(getGxtIdentifierForVehicleViaID(id));
}

u8string getGxtIdentifierForVehicleViaID(uint16 id) { // eg: returns "MAFIACR" for entered id 172 (Leone Sentinel)
  uaddr idetable = getInt(ptr_IDETable+(LCS?0x0:gp));
  return getString( getInt(idetable+(id*0x4)) + (LCS ? 0x2E : 0x158), 0 ); //gxt name is inside IDE
}

u8string vcs_getNameForPedViaID(uint16 id) { // eg: returns "PHCOL" for entered id 0x92 (Phil Collins)
  uaddr idetable = getInt(ptr_IDETable+gp);
  return getString( getInt(idetable+(id*0x4)) + 0xE4, 0 ); //name is inside IDE
}

u8string removeColor(u8string string) { // fix - sindacco chronicles gave radio stations a custom color ("~Y~XYZ")
  return STR_STARTSWITH_CHAR(string, '~') ? &string[3] : string;
}

u8string getRadioStationName(uint32 no) { // for LCS: 0 = Head Radio, 1 = Double Clef, .... 10 = Radio Off
  static u8char buf[32];

  if( no == var_radios ) // highest possible is "radio off"
    return removeColor(getGxtTranslation("FEA_NON"));
  else if ( no < var_radios ){
    snprintf(buf, sizeof(buf), "FEA_FM%lu", no);
    return removeColor(getGxtTranslation(buf));
  } else { // error
    snprintf(buf, sizeof(buf), "#%lu", no);
    return buf;
  }
}

/* void setPedSkin(int ped_handle, u8string name) {
  SetActorSkinTo(ped_handle, name); //SetActorSkinTo
  LoadAllModelsNow(0); //LoadAllModelsNow
  RefreshActorSkin(ped_handle); //RefreshActorSkin
} */

void setCameraCenterBehindPlayer() {
  if( LCS && (mod_text_size == 0x0031F854 || mod_text_size == 0x00320A34) ) { // ULUX v0.02 & ULUS v1.02
    setByte(global_camera + 0x19A, 0x1);
    return;
  }

  setByte(global_camera + (LCS ? 0x1AA : 0x113), 0x1);
}

void setFieldOfView(float fov) {
  if( LCS && (mod_text_size == 0x0031F854 || mod_text_size == 0x00320A34) ) { // ULUX v0.02 & ULUS v1.02
    setFloat(global_camera + 0x244, fov);
    return;
  }

  setFloat(global_camera + (LCS ? 0x254 : 0x198), fov);
}

float getFieldOfView() {
  if( LCS && (mod_text_size == 0x0031F854 || mod_text_size == 0x00320A34) ) // ULUX v0.02 & ULUS v1.02
    return getFloat(global_camera + 0x244);

  return getFloat(global_camera + (LCS ? 0x254 : 0x198));
}

void setGamespeed(float val) {
  setFloat(global_timescale + (LCS ? 0 : gp), val);
}

float getGamespeed() {
  return getFloat(global_timescale + (LCS ? 0 : gp));
}

bool isPedCrouching() {
  return getInt( pplayer + (LCS ? 0x198 : 0x1CC)) & (LCS ? 0x10 : 0x2000);
}

void TaskDuck() {
  if( LCS ) {
    // *(uint32 *)(pplayer + 0x198) = *(uint32 *)(pplayer + 0x198) | 8;
    setInt(pplayer + 0x198, getInt(pplayer + 0x198) | 8);
    TaskDuckLCS(pplayer); //duck
  } else {
    //if( VCS ) {
    // *(uint32 *)(pplayer + 0x1cc) = *(uint32 *)(pplayer + 0x1cc) | 0x1000;
    setInt(pplayer + 0x1cc, getInt(pplayer + 0x1cc) | 0x1000);
    TaskDuckVCS(pplayer, -1, 1); // duck (duration -1 for forever)
  }
}

usize removeIPLObjectsCollision(uint16 id) {
  usize counter = 0;
  uaddr adr = buildingsIPL_base;
  for( uaddr i = 0; i < buildingsIPL_max; adr += var_buildingsIPLslotsize, i++ ) { // Buildings.ipl
    if(getShort(adr+0x58) == id)
    {
      setFloat(adr+0x38, -200.0f); // move below map
      counter++;
    }
  }
  for( uaddr i = 0, adr = treadablesIPL_base; i < treadablesIPL_max; adr += var_treadablesIPLslotsize, i++ ) { // Treadables.ipl
    if(getShort(adr+0x58) == id)
    {
      setFloat(adr+0x38, -200.0f); // move below map
      counter++;
    }
  }
  return counter;
}


uaddr getParticleOffsetFor(const u8string name) {
  uaddr offset = getInt(ptr_particleCFG+(LCS?0x0:gp)); // PARTICLE.CFG
  for( uaddr i = 0; i < var_particleCFGslots; i++, offset += var_particleCFGslotsize ) {
    if( strcmp(name, getString(offset+0x4, 0)) == 0 ){
      return offset;
    }
  }
  return 0; // error not found
}

u8string getPedstatName(uint32 no) {
  uaddr adr = getInt(getInt(ptr_pedstatTable+(LCS?0x0:gp))) + (no * var_pedstatDATslotsize);
  if( no > var_pedstatDATslots - 1 ) {
    return "ERROR";
  }
  return getString(adr + (LCS ? 0x4 : 0x1A), 0);
}


/// *** /// *** /// *** /// *** /// *** /// SCM Stuff /// *** /// *** /// *** /// *** /// *** /// *** ///
extern usize global_MainScriptSize;
extern uaddr global_ScriptSpace;
extern usize global_LargestMissionScriptSize;

void CustomScriptClear(uaddr address, usize size) { // TODO: write END SCRIPT ? for when overwriting still running script?
  memset((void*)(address), 0x00, size);
}

void CustomScriptPlace(const byte *script, uaddr address, usize size) {
  memcpy((void*)(address), script, size);
}

void CustomScriptExecute(uaddr address) {
  /*** ScriptSpace ************************
  MAIN Section              @ 0
  -> Size = global_MainScriptSize

  MissionScripts              @ after main size
  -> Size = global_LargestMissionScriptSize

  main section jumps:
  positive

  mission section jumps:
  negative!

  LCS ->
    ScriptSpace  (ppsppp)        = 0xBF0F400
    global_MainScriptSize         = 0x41EB3
    global_LargestMissionScriptSize   = 0x5B94

    end of script space = 0xBF56E47
    - 4096 (custom script size)
    = 0xBF56647 is where custom script will be placed!
  ****************************************/

  uaddr loadadr = address - (uaddr)getInt(global_ScriptSpace + (LCS ? 0 : gp)); // address of script relative to script space (negative not intended but works! as long as no jumps!!!)
  //DEBUG_LOG("StartNewScript @ 0x%08X (0x%08X)", loadadr, address);
  StartNewScript(loadadr);
}

int getScriptGlobalValue(uint32 var) {
	return getInt(getInt(global_ScriptSpace + (LCS ? 0 : gp)) + (var * 4));
}

void setRandomPedCheat(int8 id) {
  if( id == -1 ) {
    setInt(addr_randompedcheat, 0xDFFF1112); // default random
  } else {
    setByte(addr_randompedcheat + 0x0, id);
    setByte(addr_randompedcheat + 0x1, 0x00);
    setByte(addr_randompedcheat + 0x2, 0x10);
    setByte(addr_randompedcheat + 0x3, 0x34);
  }
  clearICacheFor(addr_randompedcheat);
}