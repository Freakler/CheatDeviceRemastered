# CheatDevice Remastered Loader
_Loader add-on made and readme written by **[@danssmnt](https://github.com/danssmnt)**_.

Plugin that loads CheatDevice Remastered ONLY when you start GTA LCS / VCS.

## Why?
This plugin was made for some CFWs, like:
 - ***PRO-C2***
 - ***(L)ME 2.3***
 - ***Adrenaline (PSVita)***

which, unlike ***ARK-4***, don't have *per-game* plugins.

The problem with them is, if you have CheatDevice Remastered enabled and try to play a different game, most likely, the game will crash / have problems. That is because CheatDevice is a big plugin and clogs up a high amount of memory which games need (even if CDR isn't being used!)

So, this plugin solves that problem, it will detect IF you're playing GTA, and if you are, only THEN it'll load CheatDevice!
Hence why this plugin is really small (~2KB), unlike CheatDevice, which is gigantic (+400KB).

Besides this, it'll also load a more appropriate version of CheatDevice (Lite or Full) depending on the memory available for the system.

## How will the plugin load CheatDevice?
That'll depend on PSP model, CFW you're using and some CFW settings.

CheatDevice Remastered Lite will be loaded if:
 - you're on a PSP-1000 (since those only have 32MB of RAM)
 - you're using ***PRO-C2*** CFW (since `High Memory Layout` is kinda broken there)
 - or if `High Memory Layout` is disabled on ***(L)ME 2.3*** or ***Adrenaline (PSVita)***

## Installation
> [!WARNING]
> Do not use this with ***PPSSPP*** or ***ARK-4***. Those have per-game plugins and do not need this plugin at all.

0. Install CheatDevice Remastered (see [here](../README.md#installation) how)

1. Append the following line to `ms0:/seplugins/GAME.TXT`:
```
ms0:PSP/PLUGINS/cheatdevice_remastered/cdr_compat_loader.prx 1
```

2. Done!

---

If you encounter problems with the loader, you can find us at the [CheatDevice Remastered's official Discord server](https://discord.gg/7DERFmkgYq).