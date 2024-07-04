# GameSwitcher Protoype for RG35XX SP/PLUS/H/2024' 'splush24' muOS v11.2

A GameSwitcher app for muOS v11.2 inspired by onionOS

<div>
<img src="https://i.imgur.com/a1uIJaF.png" width="300">
<img src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExeG1oMjY3ZHNubnpnMTJpMmg5MGUwemcwenJ2eTZxY3lpdDZheGhydCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Pq4IfxNQF8FO6e1x0g/source.gif" width="300">
</div>

The GameSwitcher allows you to launch your Recent/Favorite Retroarch games and jump between them.

This app was developed rapidly so it may have bugs. Backup your saves / games before installing.

# How To Install:
Copy to main SD under ARCHIVE (will have Retroarch Cheats.zip already there on fresh image)

Install by going from the MUOS Main Menu > APPLICATIONS > Archive Manger > MustardGameSwitcher

Launch by going to MUOS Main Menu > APPLICATIONS > MustardGameSwitcher

After launching, you should now see your recent Retroarch games show up.


# Additional Configuration:

From the muOS main menu, open `Retroarch`. Then go into `Settings -> Saving` and toggle

- `Save State Thumbnails: ON`- default
- `Sort Save States into Folders by Core Name: ON`- default
- `Auto Save State: ON`- not default
- `Auto Load State: ON`- not default

Once you've completed this, make sure to save your configuration (`Configuration File -> Save Current Configuration`)

# Controls:

If you followed the configuration guide, you can now open games from the GameSwitcher by pressing `A`, and close them by pressing `MENU + Start`.

- `X`: Exit GameSwitcher
- `B`: Swap to list view & toggle between recent and favorites
- `A`: Launch Game
- `Left/Right`: Navigate recent games


# How To Build

Make sure your PC is running X86 Linux (or wsl like I am). Extract the RG35xx+ Batocera toolchain to /opt/rg35xx, then run `./buildArm7.sh`
Download Link: https://github.com/rg35xx-cfw/rg35xx-cfw.github.io/releases/tag/rg35xx_plus_h_sdk_20240207

You can also test the app on your PC by running `./buildX86.sh`, just make sure you have SDL2 and all other packages installed.
You will also need to copy some of the files onto `/mnt/muOSDump/` to test it properly.

Personal ToDo:

- better / cleanup debugging & logging solution (WIP)
- add/utilize MUOS theme & elements (WIP)
- fix external ds (drastic) savestate behavior
- ability to remove history items within app
- change controls to allign with main OS

  
Additions & bugs fixed from main
+ fixed to work with MUOS 11.2
+ implemented background image asset as background image
+ dynamic savestate screenshot location from retroarch config (only SD1 for now)
+ archive .zip package installs under applications like dingux commander etc
+ copy to archive zip automatically on compilation
+ double game enteries in history list
+ screenshots only displaying for 1 title per system / not always sync'd with correct title
+ dynamic savestate location only working when mapped to SD1
+ fixed 'last played' to work with power-on setting
+ fix resume power-on function & shutdown savestate if a gameswitcher launched game was running
+ Support for external (non-retroarch) emulators & screenshots
+ auto-start solution - (replace history function in frontend.sh & startup behavior to history as hacky solution)

