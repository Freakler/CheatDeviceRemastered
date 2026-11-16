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
#include <pspuser.h>
#include <systemctrl.h>
#include <systemctrl_se.h>

#include "platform.h"
#include "putils.h"
#include "logs.h"

/**
 * @returns Is there a display active on the device?
 */
static bool PPSSPP_IsDisplayActive()
{
  bool32 displayActive = false;

  int result = sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__GET_HAS_DISPLAY, NULL, 0, &displayActive, sizeof(displayActive));
  if ( result < 0 ) return false;

  return (bool)displayActive;
}

/**
 * Makes a system request to send a string to the platform's debug output.
 * @param string String to send
 * @note Henrik: "Commonly ignored, used when automated tests generate output."
 */
static inline void PPSSPP_SendOutput(const cstr string)
{
  sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__SEND_OUTPUT, string, (int)(strlen(string) + 1), NULL, 0);
}

/**
 * @returns Are we on an emulator?
 */
static inline bool PPSSPP_IsEmulator()
{
  return sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__IS_EMULATOR, NULL, 0, NULL, 0) == 0;
}

/**
 * @note Henrik: "For testing / automated tests.  Runs a save state verification pass (async.)"
 */
static inline void PPSSPP_VerifyState()
{
  sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__VERIFY_STATE, NULL, 0, NULL, 0);
}

/**
 * Makes a system request to save a screenshot of the PSP's screen.
 */
static inline void PPSSPP_TakeScreenshot()
{
  sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__EMIT_SCREENSHOT, NULL, 0, NULL, 0);
}

/**
 * Toggles Fast Forward ON / OFF.
 */
static inline void PPSSPP_ToggleFastForward(bool toggle)
{
  // argAddr is being used as a boolean here
  // https://github.com/hrydgard/ppsspp/blob/56c694d88bbf82270e8b472fe63abd60f3f8e0a9/Core/HLE/sceIo.cpp#L2066
  sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__TOGGLE_FASTFORWARD, (void *)toggle, 0, NULL, 0);
}

/**
 * Get current device's screen aspect ratio.
 * @returns Current device's screen aspect ratio.
 */
static float PPSSPP_GetAspectRatio()
{
  float aspectRatio = 0.0f;

  int result = sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__GET_ASPECT_RATIO, NULL, 0, &aspectRatio, sizeof(aspectRatio));
  if ( result < 0 ) return (480.0f / 272.0f); // Return default PSP Aspect Ratio

  return aspectRatio;
}

/**
 * Get current device's screen / PPSSPP window scale.
 * @returns Current device's screen / PPSSPP window scale.
 */
static float PPSSPP_GetDisplayScale()
{
  float displayScale = 0.0f;

  int result = sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__GET_SCALE, NULL, 0, &displayScale, sizeof(displayScale));
  if ( result < 0 ) return 1.0f;

  return displayScale;
}

/**
 * Get current axis value.
 * @param axis Axis to check.
 * @returns Current axis value of the specified Joystick Input Axis.
 */
static float PPSSPP_GetAxisValue(PPSSPPInputAxis axis)
{
  float joystickAxis = 0.0f;

  // argAddr is being used as an integer here
  int result = sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__GET_AXIS, (void *)axis, 0, &joystickAxis, sizeof(joystickAxis));
  if ( result < 0 ) return 0.0f;

  return joystickAxis;
}

/**
 * Get key pressed state.
 * @param key Key to check.
 * @returns Is the specified key pressed?
 */
static bool PPSSPP_IsKeyPressed(PPSSPPInputKeyCode key)
{
  uint8 vkeyStatus = false;

  // argAddr is being used as an integer here
  int result = sceIoDevctl(PPSSPP_DEVCTL_COMMAND, EMULATOR_DEVCTL__GET_VKEY, (void *)key, 0, &vkeyStatus, sizeof(vkeyStatus));
  if ( result < 0 ) return false;

  return vkeyStatus;
}

// All CFW Names
static const u8str CFWNames[CFW_TYPES_NUMBER] =
{
  "Unknown",
  "None",
  "SE",
  "OE",
  "IE",
  "M33",
  "HX",
  "GEN",
  "TDP",
  "PRO",
  "TN",
  "ME",
  "ARK-4",
  "ARK-5",
  "Adrenaline",
  "Epinephrine",
};

/**
 * @returns Current CFW running.
 */
static CFWType GetCFWType()
{
  if ( PlatformManager.Platform.type == PLTF_PPSSPP ) return CFW_NONE;

  int32 SESysConfigMagic[2]; int configExRet;

  // Only get needed magic values (sizeof(SEConfig) errors out on Epinephrine)
  if ( configExRet = sctrlSEGetConfigEx((SEConfig*)&SESysConfigMagic, sizeof(SESysConfigMagic)), configExRet != 0 )
  {
    ERROR_LOG("sctrlSEGetConfigEx == 0x%08X", configExRet); // No cfw or really really old one?
    return CFW_UNKNOWN;
  }

  if ( SESysConfigMagic[0] == SECONFIG_MAGIC_PRO ) return CFW_PRO; // PRO to PRO-C2
  if ( SESysConfigMagic[0] == SECONFIG_MAGIC_M33 ) return CFW_M33; // M33
  if ( SESysConfigMagic[0] == SECONFIG_MAGIC_EPI1 && SESysConfigMagic[1] == SECONFIG_MAGIC_EPI2 ) return CFW_EPINEPHRINE; // Epinephrine v8+
  if ( SESysConfigMagic[0] == (int32)ARK_CONFIG_MAGIC )
  {
    if ( sctrlHENFindFunction("SystemControl", "ArkCtrl", ARK_CONFIG_MAGIC)             != 0 ) return CFW_ARK5; // ARK-5
    if ( sctrlHENFindFunction("SystemControl", "SystemCtrlForUser", ARK_CONFIG_MAGIC)   != 0 ) return CFW_ARK4; // ARK-4

    return CFW_UNKNOWN; // Impossible to reach here (I think)
  }

  // https://wololo.net/talk/viewtopic.php?t=7973
  if ( sctrlHENFindFunction("SystemControl", "VshCtrlLib", 0xFD26DA72)          != 0 ) return CFW_TN; // TN
  if ( sctrlHENFindFunction("SystemControl", "SystemCtrlForKernel", 0x221400A6) != 0 ) return CFW_ME; // ME

  const int SEVer = sctrlSEGetVersion();

  if ( SEVer == 0x00000500 ) return CFW_OE; // 3.03 OE-A
  if ( SEVer > 0x00030000 && PlatformManager.Platform.type == PLTF_PSVITA ) return CFW_ADRENALINE; // v3 - v7

  // @todo Detect SE, IE, HX, GEN and TDP for a complete and global implementation
  return CFW_UNKNOWN;
}

// All possible platform names
static const u8str PlatformNames[PLTF_TYPES_NUMBER] =
{
  "Unknown",
  "PSP",
  "PS Vita",
  "PPSSPP"
};

static PlatformType GetPlatformType()
{
  if ( PPSSPP_IsEmulator() ) return PLTF_PPSSPP;

  // Find Adrenaline config file
  if ( doesFileExist("flash1:/config.adrenaline") ) return PLTF_PSVITA;

  return PLTF_PSP;
}

/**
 * Initialise Platform Manager
 */
static void PlatformManager_Initialise()
{
  if ( PlatformManager.Initialised ) return;

  PlatformManager.Platform.type = GetPlatformType();
  PlatformManager.Platform.name = PlatformNames[PlatformManager.Platform.type];

  PlatformManager.CFW.type = GetCFWType();
  PlatformManager.CFW.name = CFWNames[PlatformManager.CFW.type];

  if ( PlatformManager.Platform.type == PLTF_PPSSPP )
    strcpy(PlatformManager.fullName, "PPSSPP");
  else
    snprintf(PlatformManager.fullName, sizeof(PlatformManager.fullName), "%s (%s)", PlatformManager.Platform.name, PlatformManager.CFW.name);

  PlatformManager.Initialised = true;
}

struct PlatformMgr PlatformManager =
{
  .Initialised          = false,
  .Initialise           = PlatformManager_Initialise,

  .PPSSPP =
  {
    .IsDisplayActive    = PPSSPP_IsDisplayActive,
    .SendOutput         = PPSSPP_SendOutput,
    .IsEmulator         = PPSSPP_IsEmulator,
    .VerifyState        = PPSSPP_VerifyState,
    .TakeScreenshot     = PPSSPP_TakeScreenshot,
    .ToggleFastForward  = PPSSPP_ToggleFastForward,
    .GetAspectRatio     = PPSSPP_GetAspectRatio,
    .GetDisplayScale    = PPSSPP_GetDisplayScale,
    .GetAxisValue       = PPSSPP_GetAxisValue,
    .IsKeyPressed       = PPSSPP_IsKeyPressed,
  },
};