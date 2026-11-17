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

#ifndef GAME_H_
#define GAME_H_

#include "mctypes.h"

typedef enum GTAEbootID
{
  GTA_EBOOT_UNKNOWN = -1,

  // LCS /////////////////////

  // North America
  GTA_LCS_ULUS10041_1_02,
  GTA_LCS_ULUS10041_1_04,
  GTA_LCS_ULUS10041_1_05,
  GTA_LCS_ULUS10041_3_00,

  // Germany
  GTA_LCS_ULES00182_1_00,
  GTA_LCS_ULES00182_2_00,

  // Europe
  GTA_LCS_ULES00151_1_05,
  GTA_LCS_ULES00151_3_00,

  // Japan
  GTA_LCS_ULJM05255_1_01,

  // Prototypes
  GTA_LCS_ULUX80146_0_02,
  GTA_LCS_ULUX80146_0_03,

  // VCS /////////////////////

  // North America
  GTA_VCS_ULUS10160_1_01,
  GTA_VCS_ULUS10160_1_02,
  GTA_VCS_ULUS10160_1_03,

  // Germany
  GTA_VCS_ULES00503_1_02,

  // Europe
  GTA_VCS_ULES00502_1_02,

  // Japan
  GTA_VCS_ULJM05297_1_01,

  // Prototypes
  GTA_VCS_ULUX80212_X_XX,
  GTA_VCS_ULET00417_0_01,
  GTA_VCS_ULET00417_0_06,
  GTA_VCS_ULET00417_0_07,
  
  GTA_EBOOTS_NUMBER
} GTAEbootID;

typedef enum GTAGame
{
  GTA_UNKNOWN = -1,
  GTA_LCS     =  0,
  GTA_VCS     =  1,
  GTA_TOTAL_GAMES
} GTAGame;

struct EbootModInfo
{
  GTAGame game;
  usize   TextSize;
  usize   DataSize;
};

struct PatchGameModuleInfo
{
  uaddr TextAddr;
  usize TextSize;
  usize DataSize;
};

struct GameMgr
{
  bool Initialised;
  void (*Initialise)();

  GTAEbootID gameEboot;
  GTAGame game;
  struct PatchGameModuleInfo modInfo;
};

#define TEXTADDR(addr) (uaddr)(GameManager.modInfo.TextAddr + (addr))
#define FROMTEXTADDR(addr) (uaddr)((addr) - GameManager.modInfo.TextAddr)

// 1st offset -> LCS
// 2st offset -> VCS
#define GTA_ADDR(base, lcs_offset, vcs_offset) (uaddr)((base) + (GameManager.game == GTA_LCS ? (lcs_offset) : (vcs_offset)))

// Returns "LCS" or "VCS" depending on game
#define BOOLGTA_NAME() BOOLSTRCUSTOM(GameManager.game == GTA_LCS, STRINGIFY(LCS), STRINGIFY(VCS))

extern struct GameMgr GameManager;

#endif /* GAME_H_ */