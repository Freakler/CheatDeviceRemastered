<a name="readme-top"></a>
![ref0](https://github.com/Freakler/CheatDeviceRemastered/blob/main/pictures/logo.png)<br />
for Grand Theft Auto Liberty &amp; Vice City Stories for PlayStation Portable

![ref0](https://github.com/Freakler/CheatDeviceRemastered/blob/main/pictures/capture_000.png)
<br />

## About The Project
After over 15 years since the original CheatDevice release, with this remastered version (remake actually) I re-created the whole project from ground up whenever I had some freetime. It is one plugin for both Stories Games now and compatible with almost all versions!!

Once in-game, just like with the original, the menu can be opened and closed by pressing ***L + UP***. And when saving your cheat preferences from the menu a config file will be created which automatically re-loads everything the next time you start the game. But there really is so much more so why don't you go take a look yourself..

Happy Cheating! :)
<br /><br />


## Installation
Copy the whole ```cheatdevice_remastered``` folder into the ```/PSP/PLUGINS/``` directory. 

### PSP & Adrenaline *(extra steps)*
1. Move ```cheatdevice_remastered.prx``` to ```/seplugins/``` folder
2. Edit ```ms0:/seplugins/GAME.TXT``` file (create it if not already there) and add the line ```ms0:seplugins/cheatdevice_remastered.prx 1``` <br />
(If you are using the [native resolution patch by TheFloW](https://github.com/TheOfficialFloW/GTANativeRes) make sure to load the CheatDevice first) (Vita only!)
3. In the VSH-menu enable 333Mhz in Game-mode for a smoother experience.
4. Enable extra RAM in Recovery Menu (aka. "Force High Memory Layout")

:warning: **If you are using a PSP 1000 (Fat) model**<br>
Please use ```cheatdevice_remastered_lite.prx``` instead.
<br /><br />


## Compatibility
All versions of the game (except the Japanese releases) are supported! As a bonus the German versions will be patched back uncut again. Tested and working on selected PSP CFWs as well as on [Adrenaline eCFW](https://github.com/TheOfficialFloW/Adrenaline/releases) for Vita and [PPSSPP](https://www.ppsspp.org/) v1.12.3+
 
PPSSPP is like 98% compatible right now. There might be more issues due to the emulation however.
 
This will NOT work with mobile versions!
<br /><br />


## UserScripts
You can create basic UserScripts saved in plain text which currently is the next best thing to the original UserCheats. Everything from simply changing the weather to creating whole new missions is possible! 

A compilation of UserScripts can be found here: https://github.com/Freakler/CheatDeviceRemastered-UserScripts

A guide to get started in UserScripting here: https://github.com/Freakler/CheatDeviceRemastered-UserScripts/wiki/UserScript-Tutorial---Part-1,-Getting-Started
<br /><br />


## Changelog
<details><summary>v1.0h "The Translation Release" (15th Sep. 2024)</summary><ul>
<li>added minigun to pickup spawner and mark on map cheat for VCS
<li>fixed 'if and' / 'if or' conditions for user scripts
<li>fixed vehicle spawner blacklist in VCS (thx NABNOOB)
<li>vehicles created with vehicle spawner now disappear when not close (thx NABNOOB)
<li>added option to switch back to "real" speedometer speed calculation
<li>added PED Clothes Colors to Pedestrian Editor
<li>enabled Handling- & ModelFlags in Handling Editor (thx NABNOOB)
<li>doubled the maximum user script size & custom strings to 32
<li>fixed a problem with heli-height-patch in combination with PPSSPP
<li>added experimental Swimming Cheat for LCS
<li>added Option to swap X with R for special cheats (for gta_remastered's swapped controls)
<li>added unlimited swimming cheat for VCS (by danssmnt)
<li>the whole menu can now be translated!! (by danssmnt)
<li>more optimizations and fixes
</ul></details>

<details><summary>v1.0g2 (3rd Mar. 2024)</summary><ul>
<li>better hover vehicle controls 
<li>added VCS garage fix 
<li>speedometer fix (more realistic but probably still not accurate)
<li>rocket boost fix (1 was the same as off)
<li>cheat description strings adjusted
<li>added Sindacco Chronicles Hidden package object to pickups
<li>now skipping special vehicles in vehicle spawner
<li>removed crashing skin "FRANFOR" (LCS)
<li>fixed userscript opcode 0482 (VCS)
<li>added "the dummy" skin (LCS)
<li>added fix for Sindacco Chronicles' custom radio color showing in menu
<li>added unlimited height limit for helis and planes (VCS)
<li>fixed and enhanced power-jump 
<li>added lock/unlock car to up/down button cheat
<li>fixed PPSSPP blackscreen for LCS
<li>more optimizations and fixes 
</ul></details>

<details><summary>v1.0g "The Open Source Release" (4th May 2023)</summary><ul>
<li>fixed newline bug in UserScripts
<li>better "high memory layout" detection
<li>added Cheat "Untouchable"
<li>added Cheat "Freeze Traffic"
<li>added Cheat "Cars drive on water"
<li>added Cheat "Mission Selector"
<li>added Action-buttons Cheat "Impulse"
<li>added Action-buttons Cheat "Jump with Vehicle"
<li>added "unfreeze" option for ped/vehicle to touch cheats
<li>added option to adjust player model of stock cheat for LCS
<li>added option to display free main memory 
<li>fixed bug in loading last touched ped/vehicle/object position
<li>added Timecycle Editor (thanks to @DenielX)
<li>removed the "bigger legend box" option
<li>removed the "disable advanced UI" option
<li>removed the "show Ped's stats when aimed at" cheat
<li>removed min and max bounds for editors
<li>you can now use R + UP/DOWN to fast scroll through categories
<li>even more bug fixes and code cleaning for open-sourcing
</ul></details>

<details><summary>v1.0f "The late Anniversary Release" (29th Dez. 2022)</summary><ul>
<li>added lite version of plugin without advanced features for casual cheaters
<li>moved "CDR/" folder from root to "PSP/PLUGINS/cheatdevice_remastered/"
<li>config .cfg and names .ini are no longer dynamicly created next to the prx
<li>added teleport to Highest Solid Ground for VCS
<li>added teleport to Stadium Stage for VCS
<li>added teleport to Mendez's Mansion Interior for VCS
<li>fixed a bug loading first teleport location from config
<li>UserScripts can now have 16 custom strings with a max length of 256
<li>fixed UserScript bugs due to custom strings and added more error messages
<li>added fix for crouching and manual aiming at the same time
<li>added Cheat to adjust the BMX Jump Height (thanks to darkdraggy)
<li>added Cheat to warp out of water with car automatically for LCS
<li>fixed bug where warping out of water results in endless loop if water below
<li>added some more Stock cheats
<li>realtime clock cheat now sets system time continuously
<li>more bug fixes and code cleaning for opensourceing
<li>more & updated UserScripts 
</ul></details>

<details><summary>v1.0e3 (25th Jun. 2022)</summary><ul>
<li>fixed a bug in UserScript translation ("if and" / "if or")
<li>vehicle spawner now makes sure vehicle doors are not locked
<li>blocked more crashing cheats in multiplayer 
</ul></details>

<details><summary>v1.0e2 (24th Jun. 2022)</summary><ul>
<li>editor's slot/no position is now saved to config as well
<li>added possibility to use custom text in scripts
<li>added folder support to UserScripts
<li>more & updated UserScripts 
</ul></details>

<details><summary>v1.0e "The Config Update" (29th Apr. 2022)</summary><ul>
<li>added button to auto-select current weapon in Weapon.dat Editor
<li>config rework, removed .ini in favor of faster binary file
<li>added option to enable autosaving to config
<li>added the classic Gather Spell cheat
<li>added the classic Rocket Boost cheat
<li>added "Reverse Gravity" to button up/down cheat shortcuts
<li>added "Toggle GatherSpell" to button up/down cheat shortcuts
<li>added "Toggle Slowmo" to button up/down cheat shortcuts
<li>deactivated user scripts menu in multiplayer
<li>more UserScripts 
</ul></details>

<details><summary>v1.0d "The UserScripts Update" (20th Mar. 2022)</summary><ul>
<li>fixed Infinite Ammo for VCS (Grenade & Camera Slots)
<li>fixed unload vehicle teleport bug in VCS
<li>added UserScripts
<li>added Weapon.dat Editor
<li>added missing info to Particle and Handling Editors for VCS
<li>added facing direction in degree to draw coordinates
<li>internal Script usage wont overwrite mission scripts anymore
<li>removed LCS Building / Interior switching because of UserScripts 
</ul></details>

<details><summary>v1.0c2 (5th Jan. 2022)</summary><ul>
<li>fixed crash when trying to aim-melee a PED in VCS on PSP/Vita
<li>fixed crash for "Behave like Tank" cheat with Bikes on PSP/Vita
<li>added VCN Maverick spawn on VCN landing pad 
</ul></details>

<details><summary>v1.0c "The Content Update" (27th Dec. 2021)</summary><ul>
<li>fixed crash on Loadscreen after resume from sleepmode in LCS
<li>fixed menu text in LCS not returning to fullscreen after sleepmode
<li>fixed rare crashes with random loadscreens
<li>fixed wrong RadioStation-name detection in Editors
<li>added IDE type-dynamic Editors with basic info
<li>adjusted Speed'O'meter position for Mulitplayer
<li>added OnMission bool fake cheat
<li>added Staunton Bridge Lift control cheat
<li>added Delorean detection like in original LCS CheatDevice
<li>added Building / Interior switching (LCS only for now)
<li>added Walking Speed Multiplier cheats for Player & Pedestrians 
</ul></details>

<details><summary>v1.0b3 (15th Nov. 2021)</summary><ul>
<li>added Vehicle behaves like Tank cheat
<li>added experimental Powerjump cheat
<li>reworked vehicle spawner
<li>bugfixes 
</ul></details>

<details><summary>v1.0b2 (8th Nov. 2021)</summary><ul>
<li>fixed crash on PPSSPP when pressing R-Trigger 
</ul></details>

<details><summary>v1.0b "The PPSSPP Update" (7th Nov. 2021)</summary><ul>
<li>added support for use with PPSSPP
<li>added color-box for ped/vehicle-colors editor
<li>adjusted World-Gravity value bounds and config
<li>added N.O.S. boost cheat for vehicles
<li>changed fast scrolling in Editors (now hold R-Trigger + LEFT / RIGHT)
<li>increased height limit for helicopters in LCS (thanks to darkdraggy) 
</ul></details>

<details><summary>v1.0 "The Initial Release" (24th Oct. 2021)</summary><ul>
<li>typo & design stuff
</ul></details>

<details><summary>v0.1c</summary><ul>
<li>fixed Pickup Spawner not working for VCS
</ul></details>

<details><summary>v0.1b "Preview" (2nd Oct. 2021)</summary><ul>
<li>added lock vehicle doors when inside
<li>added ped model swapping
<li>added crouching
<li>added better Scrollbar behaviour
<li>added side mission timer freeze
<li>added debug loadscreen messages 
</ul></details>

<br />

## See also
- Right analog stick control *(for [PPSSPP](https://github.com/Freakler/ppsspp-GTARemastered), for [Adrenaline](https://github.com/TheOfficialFloW/RemasteredControls/releases/tag/GTARemastered))*
- Native Resolution Patch *(for [Adrenaline](https://github.com/TheOfficialFloW/GTANativeRes))*

<br />

## Thanks & Greetings
Edison Carter, vettefan88, Waterbottle, Jeremie Blanc, ADePSP, Joek2100, PSPHacker108, Sousanator, Rasal, Mister Enchilada, Skiller, theY4Kman, Noru, KING_REY-S, thehambone, the NSA, gtaforums.com, gtamods.com, gtamodding.ru, TheFlow, aap, Firehead, Silent, neur0n, Samilop "Cimmerian" Iter, darkdraggy, hrydgard, unknownbrackets, Micsuit / danssmnt, NielsB, metehan989, DenielX, Acid_Snake

People on the [CheatDeviceRemastered Discord Server](https://discord.gg/7DERFmkgYq) and everyone else contributing and supporting!

<p align="right"><a href="#readme-top">back to top</a></p>
