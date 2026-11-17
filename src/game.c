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

#include "mctypes.h"
#include "game.h"
#include "putils.h"

/******************************************************************
* Grand Theft Auto: Liberty City Stories                          *
*******************************************************************
* Game ID          | Text Size  | Data Size  |  Notes             *
*                                                                 *
* -- North America ---------------------------------------------- *
* ULUS-10041 v1.02 | 0x00320A34 | 0x0002DBE4 |                    *
* ULUS-10041 v1.04 | Unknown    | Unknown    |                    *
* ULUS-10041 v1.05 | 0x0032BEDC | 0x0002E0F8 |                    *
* ULUS-10041 v2.00 | 0x0032BEDC | 0x0002E0F8 | Same as v1.05      *
* ULUS-10041 v3.00 | 0x0032BFC4 | 0x0002E110 |                    *
* ULUS-10041 v4.00 | 0x0032BFC4 | 0x0002E110 | Same as v3.00      *
*                                                                 *
* -- Germany ---------------------------------------------------- *
* ULES-00182 v1.00 | 0x0032BE8C | 0x0002E0F8 |                    *
* ULES-00182 v2.00 | 0x0032BF74 | 0x0002E110 |                    *
*                                                                 *
* -- Europe ----------------------------------------------------- *
* ULES-00151 v1.05 | 0x0032BF6C | 0x0002E0F8 |                    *
* ULES-00151 v2.00 | 0x0032BF6C | 0x0002E0F8 | Same as v1.05      *
* ULES-00151 v3.00 | 0x0032C044 | 0x0002E110 |                    *
* ULES-00151 v4.00 | 0x0032C044 | 0x0002E110 | Same as v3.00      *
*                                                                 *
* -- Japan ------------------------------------------------------ *
* ULJM-05255 v1.01 | 0x0033388C | 0x0002FEB0 |                    *
* ULJM-05359 v1.01 | 0x0033388C | 0x0002FEB0 | Same as ULJM-05255 *
* ULJM-05885 v1.00 | 0x0033388C | 0x0002FEB0 | Same as ULJM-05255 *
* NPJH-50825 v?.?? | Unknown    | Unknown    |                    *
*                                                                 *
* -- Prototype -------------------------------------------------- *
* ULUX-80146 v0.02 | 0x0031F854 | 0x0002DEC8 |                    *
* ULUX-80146 v0.03 | 0x0033205C | 0x0002E1E0 |                    *
* ULET-00361 v0.02 | 0x0032C044 | 0x0002E110 | European Build     *
* ULET-00362 v0.02 | 0x0032BF74 | 0x0002E110 | German Build       *
******************************************************************/

/******************************************************************
* Grand Theft Auto: Vice City Stories                             *
*******************************************************************
* Game ID          | Text Size  | Data Size  |  Notes             *
*                                                                 *
* -- North America ---------------------------------------------- *
* ULUS-10160 v1.01 | 0x00377D30 | 0x00020E34 |                    *
* ULUS-10160 v1.02 | Unknown    | Unknown    |                    *
* ULUS-10160 v1.03 | 0x003864DC | 0x0001F85C |                    *
*                                                                 *
* -- Germany ---------------------------------------------------- *
* ULES-00503 v1.02 | 0x0038673C | 0x0001F85C |                    *
*                                                                 *
* -- Europe ----------------------------------------------------- *
* ULES-00502 v1.02 | 0x003868BC | 0x0001F85C |                    *
*                                                                 *
* -- Japan ------------------------------------------------------ *
* ULJM-05297 v1.01 | 0x00386750 | 0x0001F7E0 |                    *
* ULJM-05395 v1.01 | 0x00386750 | 0x0001F7E0 | Same as ULJM-05297 *
* ULJM-05884 v1.00 | 0x00386750 | 0x0001F7E0 | Same as ULJM-05297 *
* NPJH-50827 v?.?? | Unknown    | Unknown    |                    *
*                                                                 *
* -- Prototype -------------------------------------------------- *
* ULUX-80212 v?.?? | Unknown    | Unknown    |                    *
* ULET-00417 v0.01 | Unknown    | Unknown    |                    *
* ULET-00417 v0.06 | 0x0038664C | 0x0001F86C |                    *
* ULET-00417 v0.07 | 0x003868BC | 0x0001F85C |                    *
******************************************************************/

static const struct EbootModInfo GTAEbootModInfos[GTA_EBOOTS_NUMBER] =
{
  // LCS /////////////////////////////////////////////////////////////
  // North America ///////////////////////////////////////////////////
  { .game = GTA_LCS, .TextSize = 0x00320A34, .DataSize = 0x0002DBE4  }, // ULUS-10041 v1.02
  { .game = GTA_LCS, .TextSize = 0,          .DataSize = 0           }, // ULUS-10041 v1.04
  { .game = GTA_LCS, .TextSize = 0x0032BEDC, .DataSize = 0x0002E0F8  }, // ULUS-10041 v1.05 / v2.00
  { .game = GTA_LCS, .TextSize = 0x0032BFC4, .DataSize = 0x0002E110  }, // ULUS-10041 v3.00 / v4.00

  // Germany /////////////////////////////////////////////////////////
  { .game = GTA_LCS, .TextSize = 0x0032BE8C, .DataSize = 0x0002E0F8  }, // ULES-00182 v1.00
  { .game = GTA_LCS, .TextSize = 0x0032BF74, .DataSize = 0x0002E110  }, // ULES-00182 v2.00 / ULET-00362 v0.02

  // Europe ///////////////////////////////////////////////////////////
  { .game = GTA_LCS, .TextSize = 0x0032BF6C, .DataSize = 0x0002E0F8  }, // ULES-00151 v1.05 / v2.00
  { .game = GTA_LCS, .TextSize = 0x0032C044, .DataSize = 0x0002E110  }, // ULES-00151 v3.00 / v4.00 / ULET-00361 v0.02

  // Japan ////////////////////////////////////////////////////////////
  { .game = GTA_LCS, .TextSize = 0x0033388C, .DataSize = 0x0002FEB0  }, // ULJM-05255 v1.01 / ULJM-05359 v1.01 / ULJM-05885 v1.00

  // Prototype ////////////////////////////////////////////////////////
  { .game = GTA_LCS, .TextSize = 0x0031F854, .DataSize = 0x0002DEC8  }, // ULUX-80146 v0.02
  { .game = GTA_LCS, .TextSize = 0x0033205C, .DataSize = 0x0002E1E0  }, // ULUX-80146 v0.03

  
  // VCS //////////////////////////////////////////////////////////////
  // North America ////////////////////////////////////////////////////
  { .game = GTA_VCS, .TextSize = 0x00377D30, .DataSize = 0x00020E34  }, // ULUS-10160 v1.01
  { .game = GTA_VCS, .TextSize = 0,          .DataSize = 0           }, // ULUS-10160 v1.02
  { .game = GTA_VCS, .TextSize = 0x003864DC, .DataSize = 0x0001F85C  }, // ULUS-10160 v1.03

  // Germany //////////////////////////////////////////////////////////
  { .game = GTA_VCS, .TextSize = 0x0038673C, .DataSize = 0x0001F85C  }, // ULES-00503 v1.02

  // Europe ///////////////////////////////////////////////////////////
  { .game = GTA_VCS, .TextSize = 0x003868BC, .DataSize = 0x0001F85C  }, // ULES-00502 v1.02

  // Japan ////////////////////////////////////////////////////////////
  { .game = GTA_VCS, .TextSize = 0x00386750, .DataSize = 0x0001F7E0  }, // ULJM-05297 v1.01 / ULJM-05395 v1.01 / ULJM-05884 v1.00

  // Prototype ////////////////////////////////////////////////////////
  { .game = GTA_VCS, .TextSize = 0,          .DataSize = 0           }, // ULUX-80212 v?.??
  { .game = GTA_VCS, .TextSize = 0,          .DataSize = 0           }, // ULET-00417 v0.01
  { .game = GTA_VCS, .TextSize = 0x0038664C, .DataSize = 0x0001F86C  }, // ULET-00417 v0.06
  { .game = GTA_VCS, .TextSize = 0x003868BC, .DataSize = 0x0001F85C  }, // ULET-00417 v0.07
}; static const array_size GTAEbootModInfosSize = ARRAY_SIZE(GTAEbootModInfos);

static GTAEbootID GetCurrentEbootID()
{
  for ( array_size i = 0; i < GTAEbootModInfosSize; i++ )
  {
    if (
      GameManager.modInfo.TextSize == GTAEbootModInfos[i].TextSize &&
      GameManager.modInfo.DataSize == GTAEbootModInfos[i].DataSize 
    )
      return (GTAEbootID)i;
  }

  return GTA_EBOOT_UNKNOWN;
}

static void Initialise()
{
  if ( GameManager.Initialised ) return;

  GameManager.gameEboot = GetCurrentEbootID();
  if (GameManager.gameEboot == GTA_EBOOT_UNKNOWN) // well shit...
  {
    return;
  }

  GameManager.game = GTAEbootModInfos[GameManager.gameEboot].game;

  GameManager.Initialised = true;
}

struct GameMgr GameManager =
{
  .Initialise = Initialise,
};