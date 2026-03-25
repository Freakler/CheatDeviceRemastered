/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-, Freakler
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

/*
 * This is my try to make the smallest loader binary possible
 * for CheatDevice Remastered, detecting when player
 * is actually playing GTA (LCS or VCS).
 * 
 * This is made for CFW where you can't choose plugins per game, like:
 *  - PRO-C2
 *  - LME 2.3
 *  - Adrenaline (PSVita)
 * 
 * ...so you don't end up loading a 400+ KB plugin by mistake
 * and (most likely) crashing your game :D
 * (hence why this plugin is REALLY small...)
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * This loader will automatically run the correct version
 * of CheatDevice based on available memory:
 * 
 * PSP-1000 -> Always Lite version
 * 
 * PRO-C2
 *  High Memory Layout or not -> Lite version (high mem isn't available)
 * 
 * LME 2.3, Adrenaline and ARK
 *  High Memory Layout -> Full version
 *  else -> Lite version
 * 
 * - danssmnt
 */

#include <pspmoduleinfo.h>
#include <pspkernel.h>
#include <systemctrl.h>
#include <pspsysmem_kernel.h>
#include <string.h>

PSP_MODULE_INFO("CDRLoader", PSP_MODULE_KERNEL, 0, 1);
PSP_MAIN_THREAD_ATTR(0);

#define PATH_MAX 256

/* https://github.com/uofw/uofw/blob/master/include/modulemgr_options.h#L12 */
/* for SceKernelLMOption.position */
#define SCE_KERNEL_LM_POS_LOW       (PSP_SMEM_Low)  /* Place module at lowest possible address */

/* https://github.com/uofw/uofw/blob/master/include/modulemgr_options.h#L17 */
/* for SceKernelLMOption.access */
#define SCE_KERNEL_LM_ACCESS_NOSEEK (1)

/* Lowest address in which we could detect that some sort of High Memory Layout is being used */
/* Keep in mind ARK's Extra Memory only goes up to 48 MB on Adrenaline (which is safer)       */
#define LOWEST_ADDR_HIGH_MEM_LAYOUT 0x0AC00000

/* CheatDevice Remastered plugin filenames */
static const char *CDR_version_filenames[] =
{
  "cheatdevice_remastered.prx",      /* Full version */
  "cheatdevice_remastered_lite.prx", /* Lite version */
};

static int _main(SceSize args, void *argp)
{
  (void)args;

  /* Wait until GTA is loaded
   * If in 5 tries, 'GTA3' module wasn't found
   * most likely we aren't playing GTA so we're unloading :D
   */
  int tries = 0;
  while( !sceKernelFindModuleByName("GTA3") )
  {
    if ( tries == 5 ) goto UNLOAD;
    sceKernelDelayThread(1000 * 1000); // 1 sec
    tries++;
  }


  const char *plugin_filename;

  /* Load lite / full version depending on memory available            */
  /* Check getHighMemBound function in source/utils.c for more details */
  const SceUID high_addr_block = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_High, 0x100, NULL);
  if ( high_addr_block < 0 ) goto UNLOAD;

  /* Highest address is not calculated precisely this way but it gives a rough idea */
  const int highest_addr = (int)sceKernelGetBlockHeadAddr(high_addr_block) + 0x100;
  sceKernelFreePartitionMemory(high_addr_block);

  /* Higher highest addr -> Load Full version */
  /* Lower highest addr -> Load Lite version  */
  if ( highest_addr >= LOWEST_ADDR_HIGH_MEM_LAYOUT ) plugin_filename = CDR_version_filenames[0];
  else plugin_filename = CDR_version_filenames[1];


  /* Get current plugin directory from argp (this is where we'll try to find CDR) */
  char *directory = (char *)argp;
  char *slash = directory ? strrchr(directory, '/') : NULL;
  if ( !slash ) goto UNLOAD;
  slash++; *slash = '\0'; /* Cut argp string to get plugin directory */

  char full_path[PATH_MAX];
  strcpy(full_path, directory);       /* Copy plugin directory  */
  strcat(full_path, plugin_filename); /* Append filename        */


  /* Load module options */
  const SceKernelLMOption CDR_module_options =
  {
    .size     = sizeof(SceKernelLMOption),
    .mpidtext = PSP_MEMORY_PARTITION_USER,
    .mpiddata = PSP_MEMORY_PARTITION_USER,
    .position = SCE_KERNEL_LM_POS_LOW, /* [TODO] Should we load at the highest address possible instead? Or maybe even load at a custom address? */
    .access   = SCE_KERNEL_LM_ACCESS_NOSEEK,
  };

  /* Load and start CheatDevice */
  SceUID modid = sceKernelLoadModule(full_path, 0, (SceKernelLMOption *)&CDR_module_options);
  if ( modid >= 0 ) sceKernelStartModule(modid, 0, NULL, NULL, NULL);

  /* Unload this module after all work */
  UNLOAD:
  sceKernelSelfStopUnloadModule(1, 0, NULL);
  
  return 0;
}

int module_start(SceSize args, void *argp)
{
  /* Create and start main thread */
  SceUID thid = sceKernelCreateThread("", _main, 32, 0x1000, 0, 0);
  if ( thid < 0 ) return 1;

  sceKernelStartThread(thid, args, argp);
  return 0;
}