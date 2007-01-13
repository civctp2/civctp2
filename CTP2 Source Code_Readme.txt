Call To Power© II Source Code Release
Copyright (C) 2001 by Activision, Inc.
All rights reserved.

Documentation Updated: 10/27/03


Included Files & Directories:
=============================

\bin\				<- .dll’s and support .exe’s
\ctp2_code\			<- Source Code for CTP2
CTP2 Source Code_Readme.htm	<- Readme file in .html format
CTP2 Source Code_Readme.txt	<- Readme file in .txt format
CTP2_EULA.doc			<- End User Licensing Agreement

Information:
============

This is the released source code to Call to Power II, including the patch.  It has been stripped of comments, but is otherwise complete as far as Activision code goes.  It was built using Microsoft Visual Studio 6.0.  It has not been tested with the .NET compiler, but chances are good it will work.  

Beware that not everything you need to build the game is here!

In particular, we are not able to provide you with the Miles sound library.  Before the game will build without it, someone will need to strip references to mss.h and not link with mss.lib, and modify all code that fails to compile or link.  That will leave you with a game without sound but that is otherwise functional.

In addition, the DirectX SDK and DirectX Media SDK are not included.  The game was originally built with DirectX 7, but newer versions of the SDKs available from Microsoft at

http://msdn.microsoft.com/library/default.asp?url=/downloads/list/directx.asp

for DirectX and

http://www.microsoft.com/downloads/details.aspx?displaylang=en&familyid=9909BA22-8CAA-4241-954C-E7D710E18522

for DirectX Media SDK should work.

Make sure you add the Include and Lib and classes/base directories from those SDKs to Tools->Options->Directories from Visual Studio, if they aren't there already.  If you installed them to default locations, that means you need to add:

c:\DXSDK\Include
C:\DXMEDIA\Include
C:\DXMEDIA\Classes\Base

to your include directories path, and

C:\DXSDK\Lib
C:\DXMEDIA\Lib

to your lib directories path. 

In order to build, you need an environment variable named CDKDIR on your computer.  In Windows XP/2000, go to Control
Panel->System->Environment Variables, and add it, with it's value set to <your source path>/ctp2/bin, the directory with bison, flex, and other miscellaneous utilities in it.

Lastly, YOU NEED THE DATA DIRECTORY FROM THE ORIGINAL SHIPPING GAME.  All the graphics, sounds, and other data files needed to actually run the game are not included here.  Place the ctp2_data directory from the game install directory at the same directory level as ctp2_code in this archive to run an executable built from DevStudio.

That's the basics.  For the most part, you are on your own from here on out.  Good luck!
