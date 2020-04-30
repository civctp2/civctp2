[![Build Status](https://travis-ci.com/civctp2/civctp2.svg?branch=master)](https://travis-ci.com/civctp2/civctp2)

#### civctp2
- git svn import of http://ctp2.darkdust.net/anonsvn/
- original hosting: https://ctp2.darkdust.net/
- forums: https://apolyton.net/forum/other-games/call-to-power-2/ctp2-source-code-project

### Original readme files
- [Activision CTP2 Source Code_Readme.txt](https://github.com/civctp2/civctp2/blob/master/Activision%20CTP2%20Source%20Code_Readme.txt)
- [Apolyton CTP2 Source Code_Readme.txt](https://github.com/civctp2/civctp2/blob/master/Apolyton%20CTP2%20Source%20Code_Readme.txt)

## About

This is the Apolyton version of the *Call to Power II* source code based on the released source code from Activision, that did not include the patch. The code has been stripped of comments, but is otherwise complete as far as Activision's code goes. It was originally built with Microsoft Visual Studio 6.0. In the mean time the Apolyton code was modified so that it works with the .NET compiler as well.
This version builds on Linux (i386 and x86_64), Pandora and Pyra.

Note that the game files are not included in this repo, you can get them from the original CD or from the [GoG version](https://www.gog.com/game/call_to_power_2).

![screenshot](screenshot.png "screenshot of CTP2 running on Linux")

The Windows version supports CD Audio music, and the Linux version supports Ripped CD Audio compressed as ogg (like found in the GoG version). The Audio tracks should be named `Track02.ogg` to `Track11.ogg` and be in the `ctp2_program/ctp/music` folder.

As the Linux version does not support direct play from CD-rom, you need to rip the assets-files from the CD and put them on your disk.

## Building on Linux

You will probably need GCC 5.x or later to build. The code doesn't seem to build on GCC 4.8.

You will need SDL 2.0, SDL_Mixer 2.0, SDL_Image 2.0. To be able to play movies you also need ffmpeg libraries (some).
You will also need `byacc`
On debian and friends, use `sudo apt install libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libtiff-dev libavcodec-dev libavformat-dev libswscale-dev byacc flex` to install them all.

The build itself is pretty classing and straight forward:

```
./autogen.sh
CFLAGS="$CFLAGS -w -O3 -fuse-ld=gold" CXXFLAGS="$CXXFLAGS -fpermissive -w -O3 -fuse-ld=gold" ./configure --enable-silent-rules
make
```

If you want to build a debug version it is:

```
./autogen.sh
CFLAGS="$CFLAGS -w -Og -fno-omit-frame-pointer -fuse-ld=gold" CXXFLAGS="$CXXFLAGS -fpermissive -w -Og -fno-omit-frame-pointer -fuse-ld=gold" ./configure --enable-silent-rules --enable-debug
make
```

The option -Og optimizes bulds for debugging, this speeds up the debug function and should in general not change anything of the behavior of the executable, however it still could do so. In that case remove the -Og option.

You can look at `./configure` for options, but there aren't many.

Also, note that `make -j4` may fail the first time. Some files are auto-generated and the make dependencies don't catch that.

Finally, you may have to use gold instead of ld for link (add `-fuse-ld=gold` to your CFLAGS & CXXFLAGS if needed).

## Installation on Linux
Since CTP2 was originally a Windows game and CTP2 is not free even if its source code has been released, the installation process is a bit complicated.

### Prepare the data
First, you need the original game files of CTP2.
If you have the CD version, it's better to install it on a Windows machine / using Wine and copy over the files. You need the folders `ctp2_data`, `ctp2_program` and `Scenarios`.

If you have the GoG version, use `innoextract` to extract all the required files (you can `sudo apt install innoextract` if you need) from the `setup_call_to_power2_2.0.0.13.exe` windows installer.
For example `innoextract -m -I app  setup_call_to_power2_2.0.0.13.exe` will extract the `app`folder that contains all the game files, which are in the same subfolders as in the CD version: `ctp2_data`, `ctp2_program` and `Scenarios`.

Put those folder in you `home` path or anywhere else, for example `~/ctp2`

### Install the Apolyton edition
Once you have the original game files, copy the updated files from this git over them. Simply `cp -r ctp2_data/* ~/ctp2/ctp2_data` should do that.
Note, that this is a Windows game, where file names are case-insensitive. This version of CTP2 handles this, and file names can have any case, but the `cp` command may not overwrite the files that differ in case. If you find a strange behavior or think the files are not up-to-date, check that you don't have 2 copies of a file in different cases (lowercase everything if needed). With the GoG version, no renaming is needed.

### Copy Linux executable
When the build is over, you need to copy the main CTP2 executable, plus the mapgen libraries to the program folder (wich is `ctp2_program/ctp`).
for the executable, and the command is something like: `cp -v ctp2_code/ctp2 ~/ctp2/ctp2_program/ctp/`
The mapgen libraries will go to `ctp2_program/ctp/dll/map/` .
So do something like `cp -v ctp2_code/mapgen/.libs/*.so ~/ctp2/ctp2_program/ctp/dll/map/`

If you get the message on the terminal: `Failed to load module "atk-bridge"`. Install at-spi with `sudo apt install at-spi`.

## Running
When everything is setup, simply go to the program folder, for example with `cd ~/ctp2/ctp2_program/ctp` and launch the game with `./ctp2`. There are a few command line option like `-fullscreen` that can be useful.

### Converting old Linux save-games to new unified format
The old format of Linux save-games was not compatible with that of windows save-games. The format is now unified such that each OS can open save-games from the other. However, due to this, old Linux save-games need to be converted to the new format if you want to continue with those under the new ctp2 version (after [22dd1804804](https://github.com/civctp2/civctp2/commit/22dd180480445561bbbcd3efc60f08d2fe5c53e5)). A [game-converter-version](https://github.com/civctp2/civctp2/releases/tag/GameConverter) was created for this purpose (see, [#77](https://github.com/civctp2/civctp2/pull/77)), which can load the old format but saves games to the new format.

