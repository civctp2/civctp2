[![Build Status](https://travis-ci.com/civctp2/civctp2.svg?branch=master)](https://travis-ci.com/civctp2/civctp2)

#### civctp2
- git svn import of http://ctp2.darkdust.net/anonsvn/
- original hosting: https://ctp2.darkdust.net/
- forums: https://apolyton.net/forum/other-games/call-to-power-2/ctp2-source-code-project

# Project maintainer wanted!
All contributions are on-hold until a project maintainer for this github repo has been found.
For details please follow the discussion here: https://github.com/ptitSeb/ctp2/issues/3#issuecomment-427801575

### Original readme files
- [Activision CTP2 Source Code_Readme.txt](https://github.com/civctp2/civctp2/blob/master/Activision%20CTP2%20Source%20Code_Readme.txt)
- [Apolyton CTP2 Source Code_Readme.txt](https://github.com/civctp2/civctp2/blob/master/Apolyton%20CTP2%20Source%20Code_Readme.txt)

## About

This is the Apolyton version of the *Call to Power II* source code based on the released source code from Activision, that did not include the patch. The code has been stripped of comments, but is otherwise complete as far as Activision's code goes. It was originally built with Microsoft Visual Studio 6.0. In the mean time the Apolyton code was modified so that it works with the .NET compiler as well.
This version builds on Linux (i386 and x86_64), Pandora and Pyra.

Note that the game files are not included in this repo, you can get them from the original CD or from the [GoG version](https://www.gog.com/game/call_to_power_2).

![screenshot](screenshot.png "screenshot of CTP2 running on Linux")

This version should support CD Audio music, and also supports Ripped CD Audio compressed as ogg (like found in the GoG version). The Audio tracks should be named `Track02.ogg` to `Track11.ogg` and be in the `ctp2_program/ctp/music` folder.

## Building on Linux

You will probably need GCC 5.x or later to build. The code doesn't seem to build on GCC 4.8.

You will need SDL 1.2, SDL_Mixer, SDL_Image and the GTK 2.0 libraries.
You will also need `byacc`
On debian and friends, use `sudo apt install libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev byacc gtk+-2.0-dev` to install them all.

The build itself is pretty classing and straight forward:

```
./autogen.sh
CFLAGS="$CFLAGS -w -fuse-ld=gold" CXXFLAGS="$CXXFLAGS -w -fuse-ld=gold" ./configure --enable-silent-rules
make
```

You can look at `./configure` for options, but there aren't many.

Also, note that `make -j4` may fail the first time. Some files are auto-generated and the make dependencies don't catch that.

Finally, you may have to use gold instead of ld for link (add `-fuse-ld=gold` to your CFLAGS & CXXFLAGS if needed).

## Installation on Linux
Since CTP2 was originally a Windows game and CTP2 is not free even if its source code has been released, the installation process is a bit complicated.

### Prepare the data
First, you need the orignal game files of CTP2.
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

## Running
When everything is setup, simply go to the program folder, for example with `cd ~/ctp2/ctp2_program/ctp` and launch the game with `./ctp2`. There are a few command line option like `-fullscreen` that can be useful.
