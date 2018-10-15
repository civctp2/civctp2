#### civctp2
- git svn import of http://ctp2.darkdust.net/anonsvn/
- original hosting: https://ctp2.darkdust.net/
- forums: https://apolyton.net/forum/other-games/call-to-power-2/ctp2-source-code-project

# Project maintainer wanted!
All contributions are on-hold until project maintainer for github repo is found.
For details please follow discussion here: https://github.com/ptitSeb/ctp2/issues/3#issuecomment-427801575

# Actual README file

### Original readme files
- [Activision CTP2 Source Code_Readme.txt](https://github.com/civctp2/civctp2/blob/master/Activision%20CTP2%20Source%20Code_Readme.txt)
- [Apolyton CTP2 Source Code_Readme.txt](https://github.com/civctp2/civctp2/blob/master/Apolyton%20CTP2%20Source%20Code_Readme.txt)

### New readme file candidate

Currently there is [no official README file](https://github.com/civctp2/civctp2/issues/3) for this project, but below is proposed one based on @ptitSeb's version, for the code on [his branch](https://github.com/civctp2/civctp2/tree/ptitSeb).
Please **be aware** that it may not be 100% accurate for the current `master` state, since his code has significant updates ahead of current master branch.

@ptitSeb's README file starts here:

## About

This is the Apolyton version of *Call to Power II* source code based on the released source code of Call to Power II, without the patch. It has been stripped of comments, but is otherwise complete as far as Activision code goes. It was originally built using Microsoft Visual Studio 6.0. In the mean time the Apolyton code was modified so that it works with the .NET compiler as well.
This version build on Linux (i386 and x86_64), Pandora and Pyra.


Note that datas are not include in this repo, you'll need to get it from Original CD or from GoG version.

![screenshot](screenshot.png "screenshot of CTP2 running on Linux")

This version should support CD Audio music, and also support Ripped CD Audio compressed as ogg (like found in the GoG version). The Audio tracks should be Named `Track02.ogg` to `Track11.ogg` and be in the `ctp2_program/ctp/music` folder.

## Building

You will probably need GCC 5.x or later to build. It doesn't seems to build on GCC 4.8.

You will need SDL 1.2, SDL_Mixer, SDL_Image and GTK 2.0 libraries.
You will also need `byacc`
On debian and friend it's `sudo apt install libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev byacc gtk+-2.0-dev` to install all this.

The build itself is pretty classing and straight forward:


```
./autogen.sh
CFLAGS="$CFLAGS -w -fuse-ld=gold" CXXFLAGS="$CXXFLAGS -w -fuse-ld=gold" ./configure --enable-silent-rules
make
```

You can look at `./configure` for option, but there isn't much there.

Also, note that `make -j4` may fail the first time. Some file are auto-generated and the make dependencies doesn't catch that.

Finally, you may have to use gold instead of ld for link (add `-fuse-ld=gold` to your CFLAGS & CXXFLAGS if needed).

## Installation
Beeing a CD Windows game initially, the installation process is bit complicated.

### Prepare the datas
First you need the Data of the original game.
If you have the CD version, it's better to install it on a Windows machine / using Wine and copy over the data. You need `ctp2_data`, `ctp2_program` and `Scenarios` folders.

If you have the GoG version, use `innoextract` to extract all the required datas (you can `sudo apt install innoextract` if you need) from the `setup_call_to_power2_2.0.0.13.exe` windows installer.
For example `innoextract -m -I app  setup_call_to_power2_2.0.0.13.exe` will extract the `app`folder taht contains all game datas.
The same `ctp2_data`, `ctp2_program` and `Scenarios` folders as with the CD are needed.

Put those folder in you `home` path or anywhere else, for example `~/ctp2`

### Use Apolyton datas
You then need to copy updated datas from this git over the original data. Simply `cp -r ctp2_data/* ~/ctp2/ctp2_data` should do the trick.

Note that this is a Windows game, where file name is on a Case Insensitive file system. This version of the CTP2 handle this, and file name can have any case, but the `cp` command may not overwrite file that have different case. If you have strange behavour or think the data is not up-to-date, check that you don't have 2 copies of files, with different case (or lowercase everything if needed). Using the GoG version, no renaming is needed.

### Copy Linux executable
When the build is over, you need to copy the main CTP2 executable, plus mapgen library to the program folder (wich is `ctp2_program/ctp`).
for the executable, it will be something like: `cp -v ctp2_code/ctp2 ~/ctp2/ctp2_program/ctp/`
The mapgen libraries will go to `ctp2_program/ctp/dll/map/` .
So do something like `cp -v ctp2_code/mapgen/.libs/*.so ~/ctp2/ctp2_program/ctp/dll/map/`

## Running

When everything is setup, simply go to the program folder, for example with `cd ~/ctp2/ctp2_program/ctp` and launch the game with `./ctp2`. There are a few command line option like `-fullscreen` that can be usefull.
