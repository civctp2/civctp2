THIS MATERIAL IS NOT MADE OR SUPPORTED BY ACTIVISION.
This version of the Call To Power 2 is developed at apolyton.net

Contact the authors on the Apolyton forums at
http://apolyton.net/forums/forumdisplay.php?forumid=213
or at ctp2source@apolyton.net

Based on:

Call To Power(c) II Source Code Release
Copyright (C) 2001 by Activision, Inc.
All rights reserved.


Documentation Updated: 07/19/05


Included Files & Directories:
=============================

\bin\                                    <- .dll's and support .exe's
\ctp2_code\                              <- Source Code for CTP2
CTP2 Source Code_Readme.htm              <- Readme file in .html format
CTP2 Source Code_Readme.txt              <- Readme file in .txt format
Apolyton CTP2 Source Code_Readme.txt     <- Apolyton updated readme file in .txt format
CTP2_EULA.doc                            <- End User Licensing Agreement

Information:
============

This is the Apolyton version of Call to Power II source code based on the released source code of Call to Power II, without the patch. It has been stripped of comments, but is otherwise complete as far as Activision code goes. It was originally built using Microsoft Visual Studio 6.0. In the mean time the Apolyton code was modified so that it works with the .NET compiler as well.  

Beware that not everything you need to build the game is here!

In particular, the DirectX SDK is not included. The game was originally built with DirectX 7 and needed the DirectX Media SDK. Since DirectX Media SDK has been integrated into DirectX you do not need to download it separately. Newer versions of the DirectX SDK available from Microsoft at:

http://msdn.microsoft.com/directx/directxdownloads/

Make sure you add the Include and Lib and BaseClasses directories from this SDK to Tools->Options->Directories from Visual Studio, if they are not there already. If you installed them to default locations, that means you need to add e.g. for DirectX 9.0:

C:\DXSDK\Include
C:\DXSDK\Samples\C++\DirectShow\BaseClasses

to your include directories path, and

C:\DXSDK\Lib
C:\DXSDK\Samples\C++\DirectShow\BaseClasses\Debug_Unicode

to your lib directories path.

Note that the project files have been updated to link the code with DirectX 9.0. It has not been tested whether this may break linkage to older versions of DirectX.

If you are using Visual Studio C++ 6.0 you need to add the following line to the list of include directories in front of the Microsoft standart directories, since MSVSC++ 6.0 comes with a defect standart library.

C:\<your CTP2 directory>\ctp2_code\compiler\msvc6\stlfixes (if you use )

In order to build, you need an environment variable named CDKDIR on your computer. In Windows XP/2000, go to Control
Panel->System->Environment Variables, and add it, with it's value set to <your source path>/ctp2/bin, the directory with bison, flex, and other miscellaneous utilities in it.
In Windows 9X/ME add to your autoexec.bat the following line:

SET CDKDIR=<your source path>\CTP2\bin 

Note that you must not add spaces between the environment variable name and the equals sign. The same holds for the path name and the equals sign.

You have also to create a tmp directory on the harddrive you have the code, e.g.: C:\tmp

Lastly, YOU NEED THE DATA DIRECTORY FROM THE ORIGINAL SHIPPING GAME. All the graphics, sounds, and other data files needed to actually run the game are not included here. Place the ctp2_data directory from the game install directory at the same directory level as ctp2_code in this archive to run an executable built from DevStudio. For instance if your ctp2_data directory has the path:

C:\<your CTP2 directory>\ctp2_data\

then your ctp2_code directory should has the path:

C:\<your CTP2 directory>\ctp2_code\


That's the basics. For the most part, you are on your own from here on out.  Good luck!
