# Microsoft Developer Studio Project File - Name="ctp2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CTP2 - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "civctp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "civctp.mak" CFG="CTP2 - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ctp2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - Win32 Test" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - Win32 NDebug" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - Win32 Debug Browse" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - Win32 Optimized Test" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - SDL Debug" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - SDL Final" (based on "Win32 (x86) Application")
!MESSAGE "ctp2 - Win32 Final with Logging" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ctp", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zd /Og /Oi /Os /Ob2 /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Ox
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\libs\anet\win\lib\anet2.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /pdb:none /map:"civctp_rel.map" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"ctp2r.exe"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "_PLAYTEST" /D "_DEBUG" /D "_AI_BACKDOOR" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /Yu"c3.h" /FD /c
# SUBTRACT CPP /X /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\libs\anet\win\lib\anet2d.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /map:"CivCTP_dbg.map" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"CivCTP_dbg.exe"
# SUBTRACT LINK32 /verbose /profile

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ctp2___W"
# PROP BASE Intermediate_Dir "ctp2___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Test"
# PROP Intermediate_Dir "Test"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\ctp2\\" /I "..\ctp2\ctp2_utils\\" /I "..\ctp2\ctp2_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\ctp2\debugtools" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /X /Fr
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /Zi /Ot /Og /Ob2 /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\newdb" /I "..\gs\database" /I "..\gs\events" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\slic_debug" /I "..\ai" /I "..\ai\CityManagement" /I "..\ai\diplomacy" /I "..\ai\mapanalysis" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\squads" /D "_PLAYTEST" /D "_AI_BACKDOOR" /D "_DEBUG" /D "_TEST" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Ox /Ow /Oi
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib ole32.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib ..\libs\anet\win\lib\anet2d.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib imagehlp.lib vfw32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"CivCTP.exe"
# SUBTRACT BASE LINK32 /verbose /profile /map
# ADD LINK32 MSVCRT.lib MSVCRTD.lib ..\libs\anet\win\lib\anet2d.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib strmbasd.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"CivCTP_test.exe"
# SUBTRACT LINK32 /verbose /profile /map

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ctp2___W"
# PROP BASE Intermediate_Dir "ctp2___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ctp2___W"
# PROP Intermediate_Dir "ctp2___W"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /D "_DEBUG" /D "_AI_BACKDOOR" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /X /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_NDBG" /U "_DEBUG" /Yu"c3.h" /FD /c
# SUBTRACT CPP /X /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\anet\win\lib\anet2d.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib strmbasd.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib imagehlp.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"CivCTP_dbg.exe"
# SUBTRACT BASE LINK32 /verbose /profile /map
# ADD LINK32 ..\libs\anet\win\lib\anet2d.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"CivCTP_ndbg.exe"
# SUBTRACT LINK32 /verbose /profile /map

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ctp2___0"
# PROP BASE Intermediate_Dir "ctp2___0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "final"
# PROP Intermediate_Dir "final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /Yu"c3.h" /FD /c
# ADD CPP /nologo /G5 /MD /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\os\include\\" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_BFR_" /Yu"c3.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\anet\win\lib\anet2.lib strmbasd.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib imagehlp.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib /nologo /subsystem:windows /pdb:none /map:"civctp_rel.map" /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"civctp_rel.exe"
# SUBTRACT BASE LINK32 /profile /debug
# ADD LINK32 ..\libs\anet\win\lib\anet2.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /pdb:none /map:"ctp2.map" /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"ctp2.exe"
# SUBTRACT LINK32 /profile /debug

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ctp2___Win32_Debug_Browse0"
# PROP BASE Intermediate_Dir "ctp2___Win32_Debug_Browse0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "_DEBUG" /D "_AI_BACKDOOR" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /X /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "_PLAYTEST" /D "_DEBUG" /D "_AI_BACKDOOR" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /Fr /Yu"c3.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\anet\win\lib\anet2d.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib strmbasd.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /map:"CivCTP_dbg.map" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"CivCTP_dbg.exe"
# SUBTRACT BASE LINK32 /verbose /profile
# ADD LINK32 ..\libs\anet\win\lib\anet2d.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /map:"CivCTP_dbg.map" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"CivCTP_dbg.exe"
# SUBTRACT LINK32 /verbose /profile
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy for Remote Debugging
PostBuild_Cmds=if                                                                         exist                                                                         R:\ctp2_project\ctp2_code\ctp\CivCTP_dbg.exe                                                                         copy                                                                         C:\src\ctp2_project\ctp2_code\ctp\CivCTP_dbg.exe                                                                         R:\ctp2_project\ctp2_code\ctp\                                                                        	rem                                                 if                                                                         exist                                                                         R:\ctp2_project\ctp2_code\ctp\CivCTP_dbg.map                                                                         copy                                                                         C:\src\ctp2_project\ctp2_code\ctp\CivCTP_dbg.map                                                                         R:\ctp2_project\ctp2_code\ctp\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ctp2___Win32_Optimized_Test1"
# PROP BASE Intermediate_Dir "ctp2___Win32_Optimized_Test1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ctp2___Win32_Optimized_Test1"
# PROP Intermediate_Dir "ctp2___Win32_Optimized_Test1"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Zd /Og /Oi /Os /Ob2 /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /Ox
# ADD CPP /nologo /G5 /MD /W3 /GX /Zd /Og /Oi /Os /Ob2 /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "_PLAYTEST" /Yu"c3.h" /FD /c
# SUBTRACT CPP /Ox /Ot
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\anet\win\lib\anet2.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib strmbasd.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /pdb:none /map:"civctp_rel.map" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"ctp2.exe"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 ..\libs\anet\win\lib\anet2.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /pdb:none /map:"civctp_rel.map" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"ctp2.exe" /fixed:no

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ctp2_-_SDL_Debug"
# PROP BASE Intermediate_Dir "ctp2_-_SDL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ctp2_-_SDL_Debug"
# PROP Intermediate_Dir "ctp2_-_SDL_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_SDL" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "_PLAYTEST" /D "_DEBUG" /D "_AI_BACKDOOR" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "_DEBUGTOOLS" /D "_DEBUG_MEMORY" /D "USE_SDL" /Yu"c3.h" /FD /c
# SUBTRACT CPP /X /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\libs\anet\win\lib\anet2d.lib imagehlp.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib SDL.lib SDL_image.lib SDL_mixer.lib strmbasd.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /map:"CivCTP_dbg.map" /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"CivCTP_SDL_dbg.exe"
# SUBTRACT LINK32 /verbose /profile

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ctp2_-_SDL_Final"
# PROP BASE Intermediate_Dir "ctp2_-_SDL_Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ctp2_-_SDL_Final"
# PROP Intermediate_Dir "ctp2_-_SDL_Final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\\" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "USE_SDL" /Yu"c3.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\os\include" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_BFR_" /D "USE_SDL" /Yu"c3.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\anet\win\lib\anet2.lib strmbasd.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib SDL.lib SDL_image.lib SDL_mixer.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib imagehlp.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib /nologo /subsystem:windows /pdb:none /map:"civctp_rel.map" /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"civctp_rel.exe"
# SUBTRACT BASE LINK32 /profile /debug
# ADD LINK32 ..\libs\anet\win\lib\anet2.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib SDL.lib SDL_image.lib SDL_mixer.lib strmbasd.lib amstrmid.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /pdb:none /map:"ctp2.map" /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"ctp2_SDL.exe"
# SUBTRACT LINK32 /profile /debug

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ctp2___Win32_Final_with_Logging"
# PROP BASE Intermediate_Dir "ctp2___Win32_Final_with_Logging"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "FinalLog"
# PROP Intermediate_Dir "FinalLog"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\os\include\\" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_BFR_" /Yu"c3.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /G5 /MD /GX /Ox /Ot /Og /Oi /Op /Oy- /Ob2 /I "..\os\include\\" /I "..\os\common" /I "..\os\win32" /I "..\ctp\ctp2_utils\\" /I "..\ctp\ctp2_rsrc\\" /I "..\ctp\debugtools" /I "..\libs\freetype\lib" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\ui\netshell" /I "..\robot\utility" /I "..\robot\pathing" /I "..\robot\aibackdoor" /I "..\libs\freetype\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_utils\\" /I "..\ui\aui_sdl\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_ctp2\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\mm\\" /I "..\robotcom\backdoor" /I "..\gs\slic" /I "..\gfx\layers" /I "..\mapgen" /I "..\ui\freetype" /I "..\sound" /I "..\GameWatch\gamewatch" /I "..\GameWatch\gwciv" /I "..\libs\miles" /I "..\ctp\fingerprint" /I "..\libs\zlib" /I "..\ui\slic_debug" /I "..\gs\outcom" /I "..\ctp\\" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\gs\events" /I "..\gs\newdb" /I "..\ai\diplomacy" /I "..\ai" /I "..\ai\mapanalysis" /I "..\ai\strategy\scheduler" /I "..\ai\strategy\agents" /I "..\ai\strategy\goals" /I "..\ai\strategy\squads" /I "..\ai\citymanagement" /D "__GW_USE_IMPORT" /D "__GWCIV_USE_IMPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_BFR_" /D "USE_LOGGING" /Yu"c3.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\libs\anet\win\lib\anet2.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /pdb:none /map:"ctp2.map" /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"ctp2.exe"
# SUBTRACT BASE LINK32 /profile /debug
# ADD LINK32 ..\libs\anet\win\lib\anet2.lib ..\libs\GameWatch\gamewatch.lib ..\libs\GameWatch\gwciv.lib kernel32.lib ole32.lib amstrmid.lib strmbasd.lib quartz.lib oleaut32.lib winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib ..\libs\tiff\libtiff.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ..\ui\freetype\freetype.lib vfw32.lib ..\libs\miles\mss32.lib ..\libs\zlib\zlib.lib wsock32.lib /nologo /subsystem:windows /pdb:none /map:"ctp2log.map" /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"ctp2log.exe"

!ENDIF 

# Begin Target

# Name "ctp2 - Win32 Release"
# Name "ctp2 - Win32 Debug"
# Name "ctp2 - Win32 Test"
# Name "ctp2 - Win32 NDebug"
# Name "ctp2 - Win32 Final"
# Name "ctp2 - Win32 Debug Browse"
# Name "ctp2 - Win32 Optimized Test"
# Name "ctp2 - SDL Debug"
# Name "ctp2 - SDL Final"
# Name "ctp2 - Win32 Final with Logging"
# Begin Group "CTP"

# PROP Default_Filter ""
# Begin Group "ctp2_rsrc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ctp2_rsrc\civ3.rc
# End Source File
# Begin Source File

SOURCE=.\ctp2_rsrc\resource.h
# End Source File
# End Group
# Begin Group "ctp2_utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ctp2_utils\appstrings.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\appstrings.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\AvlTree.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\BitMask.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3cmdline.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /I "..\ai\personality"
# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD CPP /Zd /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3cmdline.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3cpu.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3cpu.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3debug.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3debug.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3debugstl.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3errors.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3errors.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3files.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3files.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\c3mem.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\cheatkey.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\cheatkey.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\ctpregistry.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\ctpregistry.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\ddhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\ddhelp.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\minmax.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\netconsole.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\netconsole.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\PointerList.h
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\tracklen.cpp
# End Source File
# Begin Source File

SOURCE=.\ctp2_utils\tracklen.h
# End Source File
# End Group
# Begin Group "debugtools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\debugtools\breakpoint.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\debug.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\debugtools\debug.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugassert.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugassert.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugcallstack.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugcallstack.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugexception.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugexception.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugmemory.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\debugtools\debugmemory.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\log.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\debugtools\log.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\log_on.h
# End Source File
# Begin Source File

SOURCE=.\debugtools\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\debugtools\Timer.h
# End Source File
# End Group
# Begin Group "fingerprint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fingerprint\ctp_finger.cpp
# End Source File
# Begin Source File

SOURCE=.\fingerprint\ctp_finger.h
# End Source File
# Begin Source File

SOURCE=.\fingerprint\Endecode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\fingerprint\Shroud.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\fingerprint\Shroud.h
# End Source File
# Begin Source File

SOURCE=.\fingerprint\Verify.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\fingerprint\Verify.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\c3.cpp
# ADD CPP /Yc"c3.h"
# End Source File
# Begin Source File

SOURCE=.\c3.h
# End Source File
# Begin Source File

SOURCE=.\c3types.h
# End Source File
# Begin Source File

SOURCE=.\civ3_main.cpp
# End Source File
# Begin Source File

SOURCE=.\civ3_main.h
# End Source File
# Begin Source File

SOURCE=.\civapp.cpp
# End Source File
# Begin Source File

SOURCE=.\civapp.h
# End Source File
# Begin Source File

SOURCE=.\display.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\display.h
# End Source File
# End Group
# Begin Group "gfx"

# PROP Default_Filter ""
# Begin Group "spritesys"

# PROP Default_Filter ""
# Begin Group "Actors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gfx\spritesys\action.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Action.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Actor.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Actor.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\ActorPath.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\ActorPath.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Anim.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Anim.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\BattleViewActor.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\BattleViewActor.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\EffectActor.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\EffectActor.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\GoodActor.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\GoodActor.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\TradeActor.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\TradeActor.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\UnitActor.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\UnitActor.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\workeractor.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\workeractor.h
# End Source File
# End Group
# Begin Group "Sprites"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gfx\spritesys\CitySpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\CitySpriteGroup.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\EffectSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\EffectSpriteGroup.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\FacedSprite.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\FacedSprite.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\FacedSpriteWshadow.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\FacedSpriteWshadow.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\GoodSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\GoodSpriteGroup.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Sprite.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteFile.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteFile.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteGroup.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteGroupList.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteGroupList.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteHitTest.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteList.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteList.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\spritelow555.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\spritelow565.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteState.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\SpriteState.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\spriteutils.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\spriteutils.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\UnitSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\UnitSpriteGroup.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\gfx\spritesys\Director.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\Director.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\DirectorActions.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\DirectorActions.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\directorevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\directorevent.h
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\screenmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\spritesys\screenmanager.h
# End Source File
# End Group
# Begin Group "tilesys"

# PROP Default_Filter ""
# Begin Group "maputils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gfx\tilesys\maputils.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\maputils.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\gfx\tilesys\BaseTile.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\BaseTile.h
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\resourcemap.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\resourcemap.h
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TiledMap.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\tiledmap.h
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\tiledraw.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TileDrawRoad.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TileDrawRoad.h
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TileHighlight.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TileInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TileInfo.h
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TileSet.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\TileSet.h
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\tileutils.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\tileutils.h
# End Source File
# End Group
# Begin Group "gfx_utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gfx\gfx_utils\arproces.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\arproces.h
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\ColorSet.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\colorset.h
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\gfx_options.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\gfx_options.h
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\pixelutils.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\pixelutils.h
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\rimutils.h
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\targautils.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\targautils.h
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\tiffutils.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\tiffutils.h
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\videoutils.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\gfx_utils\videoutils.h
# End Source File
# End Group
# Begin Group "layers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gfx\layers\citylayer.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\layers\citylayer.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\gfx\gfx_utils\Queue.h
# End Source File
# End Group
# Begin Group "gs"

# PROP Default_Filter ""
# Begin Group "database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gs\database\DB.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\DB.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\FilenameDB.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\FilenameDB.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\highscoredb.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\highscoredb.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\MovieDB.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\MovieDB.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\PlayListDB.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\PlayListDB.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\profileDB.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\profileDB.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\Rec.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\Rec.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\StrDB.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\StrDB.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\StrRec.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\StrRec.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\thronedb.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\thronedb.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\UVDB.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\UVDB.h
# End Source File
# Begin Source File

SOURCE=..\gs\database\UVRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\database\UVRecord.h
# End Source File
# End Group
# Begin Group "gameobj"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gs\gameobj\AchievementTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\AchievementTracker.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Advances.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Advances.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\advanceutil.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\advanceutil.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Agreement.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Agreement.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\AgreementData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\AgreementData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\AgreementPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\AgreementPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Army.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Army.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ArmyData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ArmyData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ArmyEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ArmyEvent.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ArmyPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ArmyPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Barbarians.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Barbarians.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\BldQue.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\BldQue.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\buildingutil.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\buildingutil.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityEvent.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityInfluenceIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityInfluenceIterator.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityRadius.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CityRadius.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Civilisation.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Civilisation.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CivilisationData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CivilisationData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CivilisationPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CivilisationPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\combatevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\combatevent.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CreateUnit.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CriticalMessagesPrefs.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CriticalMessagesPrefs.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CTP2Combat.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\CTP2Combat.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Diffcly.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Diffcly.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Diplomacy_Log.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Diplomacy_Log.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\DiplomacyOffer.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\DiplomaticRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\DiplomaticRequest.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\DiplomaticRequestData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\DiplomaticRequestData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\DiplomaticRequestPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\DiplomaticRequestPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Events.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Events.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\EventTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\EventTracker.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Exclusions.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Exclusions.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\FeatTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\FeatTracker.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GaiaController.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GaiaController.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GameObj.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GameObj.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GameOver.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GameSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GameSettings.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Gold.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Gold.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GoodyHuts.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GoodyHuts.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\GSLogs.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Happy.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Happy.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\HappyTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ID.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ID.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\improvementevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\improvementevent.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\installation.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\installation.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\installationdata.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\installationdata.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\installationpool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\installationpool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\installationtree.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\MaterialPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\MaterialPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Message.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Message.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\MessageData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\MessageData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\MessagePool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\MessagePool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ObjPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\ObjPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Order.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Order.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Player.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Player.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\PlayerEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\PlayerEvent.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\PlayerTurn.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\PlayHap.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\PlayHap.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Pollution.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Pollution.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\PollutionConst.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Readiness.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Readiness.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Regard.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Regard.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Resources.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Resources.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Sci.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Sci.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Score.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Score.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Strengths.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Strengths.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TaxRate.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TaxRate.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\terrainutil.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\terrainutil.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TerrImprove.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TerrImprove.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TerrImproveData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TerrImproveData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TerrImprovePool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TerrImprovePool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TopTen.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TopTen.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeBids.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeBids.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\tradeevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\tradeevent.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeOffer.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeOffer.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeOfferData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeOfferData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeOfferPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeOfferPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradePool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradePool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeRoute.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeRoute.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeRouteData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\TradeRouteData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\tradeutil.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\tradeutil.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Unit.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Unit.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitData.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitData.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitEvent.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitPool.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitPool.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\UnitUtil.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Vision.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Vision.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\WonderTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\WonderTracker.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\wonderutil.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\wonderutil.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Wormhole.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\Wormhole.h
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\XY_Coordinates.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\gameobj\XY_Coordinates.h
# End Source File
# End Group
# Begin Group "utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gs\utility\AgreementDynArr.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\AgreementDynArr.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\checksum.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\Checksum.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\CivDynArr.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\CivDynArr.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\clock.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\clock.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\DataCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\DataCheck.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\DiplDynArr.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\DiplDynArr.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\ErrMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\ErrMsg.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\gameinit.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\gameinit.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\Globals.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\MapFile.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\MapFile.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\MsgDynArr.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\MsgDynArr.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\NewTurnCount.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\NewTurnCount.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\PQueue.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\QuadTree.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\RandGen.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\RandGen.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\SimpleDynArr.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\StringUtils.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\TradeDynArr.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\TradeDynArr.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\TurnCnt.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\TurnCnt.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\TurnCntEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\TurnCntEvent.h
# End Source File
# Begin Source File

SOURCE=..\gs\utility\UnitDynArr.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\utility\UnitDynArr.h
# End Source File
# End Group
# Begin Group "world"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gs\world\Cell.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\Cell.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\CellUnitList.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\CellUnitList.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\MapPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\MapPoint.h
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\StartingPosition.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\UnseenCell.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\UnseenCell.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\UnseenCellQuadTree.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\WldGen.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\gs\world\World.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\WorldDistance.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\WorldDistance.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\worldevent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\worldevent.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\worldutils.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\worldutils.h
# End Source File
# Begin Source File

SOURCE=..\gs\world\WrldCity.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\WrldCont.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\WrldPoll.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\WrlEnv.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\WrlImprove.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\world\WrlUnit.cpp
# End Source File
# End Group
# Begin Group "fileio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gs\fileio\CivPaths.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\CivPaths.h
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\CivScenarios.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\CivScenarios.h
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\GameFile.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\GameFile.h
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\prjfile.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\prjfile.h
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\Token.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\fileio\Token.h
# End Source File
# End Group
# Begin Group "slic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gs\slic\lex.sc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\gs\slic\lex.yy.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\gs\slic\QuickSlic.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\QuickSlic.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\sc.tab.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\gs\slic\sc.tab.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\slic.l

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build - snorkeling slic.l
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build - snorkeling...
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build - snorkeling...
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build - snorkeling...
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build - snorkeling slic.l
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build - snorkeling slic.l
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build - snorkeling...
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build - snorkeling...
ProjDir=.
InputPath=..\gs\slic\slic.l

"$(ProjDir)\..\gs\slic\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -o$(ProjDir)\..\gs\slic\lex.yy.c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\slic\slic.y

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build - burgling slic.y
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build - burgling...
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build - burgling...
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build - burgling...
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build - burgling slic.y
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build - burgling...
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build - burgling...
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build - burgling...
ProjDir=.
InputPath=..\gs\slic\slic.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\y  ..\\gs\\slic\\slic.y

"$(ProjDir)\..\gs\slic\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicArray.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicArray.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicBuiltin.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicBuiltin.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicButton.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicButton.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\sliccmd.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\sliccmd.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\sliccmd.l

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build - Combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build - combobulating sliccmd.l
ProjDir=.
InputPath=..\gs\slic\sliccmd.l

"$(ProjDir)\..\gs\slic\lex.sc.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Psc -o$(ProjDir)\..\gs\slic\lex.sc.c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\slic\sliccmd.y

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build - discombobulating sliccmd.y
ProjDir=.
InputPath=..\gs\slic\sliccmd.y

BuildCmds= \
	$(CDKDIR)\byacc -d -b..\\gs\\slic\\sc  ..\\gs\\slic\\sliccmd.y

"$(ProjDir)\..\gs\slic\sc.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\slic\sc.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicConditional.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicConditional.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicConst.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicConst.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicContext.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicContext.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicDBConduit.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicEngine.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicEngine.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicError.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicEyePoint.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicEyePoint.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicFrame.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicFunc.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /I "..\ai\personality"
# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicFunc.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\slicfuncai.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /I "..\ai\personality"
# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\slic\slicfuncai.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\slicif.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\slicif.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\slicif_sym.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\sliciffile.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicModFuncEnum.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicModFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicModFunction.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicNamedSymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicNamedSymbol.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\slicobject.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicObject.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicSegment.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicStack.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicStack.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicStruct.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicStruct.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicSymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicSymbol.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicSymTab.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\slic\SlicSymTab.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\StringHash.h
# End Source File
# Begin Source File

SOURCE=..\gs\slic\y.tab.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\gs\slic\y.tab.h
# End Source File
# End Group
# Begin Group "events"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gs\events\GameEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEvent.h
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventArgList.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventArgList.h
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventArgument.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventArgument.h
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventDescription.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventDescription.h
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventHook.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventHook.h
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventManager.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventManager.h
# End Source File
# Begin Source File

SOURCE=..\gs\events\GameEventTypes.h
# End Source File
# End Group
# Begin Group "newdb"

# PROP Default_Filter ""
# Begin Group "ai database"

# PROP Default_Filter "*.cpp *.h"
# Begin Source File

SOURCE=..\gs\newdb\AdvanceListRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AdvanceListRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\BuildingBuildListRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\BuildingBuildListRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\BuildListSequenceRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\BuildListSequenceRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\diplomacy.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__DIPLO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\diplomacy.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Goal.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__GOAL_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Goal.cdb

"$(ProjDir)\..\gs\newdb\GoalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\GoalRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\GoalRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ImprovementListRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ImprovementListRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\personality.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__PERSO="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\personality.cdb

"$(ProjDir)\..\gs\newdb\PersonalityRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\PersonalityRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\PersonalityRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Strategy.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__STRAT="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Strategy.cdb

"$(ProjDir)\..\gs\newdb\StrategyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\StrategyRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\StrategyRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\UnitBuildListRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\UnitBuildListRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\WonderBuildListRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\WonderBuildListRecord.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\gs\newdb\advance.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\AdvanceBranchRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\AgeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__ADVAN="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\advance.cdb

"$(ProjDir)\..\gs\newdb\AdvanceRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AdvanceBranchRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AdvanceBranchRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AdvanceRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AdvanceRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AgeCityStyleRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AgeCityStyleRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AgeRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\AgeRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\BitArray.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\building.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__BUILD="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\building.cdb

"$(ProjDir)\..\gs\newdb\BuildingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\BuildingRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\BuildingRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\citysize.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__CITYS="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citysize.cdb

"$(ProjDir)\..\gs\newdb\CitySizeRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CitySizeRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CitySizeRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\citystyle.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__CITYST="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\citystyle.cdb

BuildCmds= \
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

"$(ProjDir)\..\gs\newdb\AgeCityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\gs\newdb\CityStyleRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CityStyleRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CityStyleRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Civilisation.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__CIVIL="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Civilisation.cdb

"$(ProjDir)\..\gs\newdb\CivilisationRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CivilisationRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CivilisationRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Concept.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Concept.cdb

"$(ProjDir)\..\gs\newdb\ConceptRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ConceptRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ConceptRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Const.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Const.cdb

"$(ProjDir)\..\gs\newdb\ConstRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ConstRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ConstRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CTPDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CTPDatabase.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CTPRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CTPRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\culture.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\culture.cdb

"$(ProjDir)\..\gs\newdb\CultureRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CultureRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\CultureRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DBLexer.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DBLexer.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DBLexer.l

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build - Pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build - pummeling DBLexer.l
ProjDir=.
InputPath=..\gs\newdb\DBLexer.l

"$(ProjDir)\..\gs\newdb\lex.dbl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pdbl -o$(ProjDir)\..\gs\newdb\lex.dbl.c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DBTokens.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Difficulty.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__DIFFI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Difficulty.cdb

"$(ProjDir)\..\gs\newdb\DifficultyRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DifficultyRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DifficultyRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyProposal.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__DIPLOM="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyProposal.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyProposalRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyProposalRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyProposalRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyThreat.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__DIPLOMA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\DiplomacyThreat.cdb

"$(ProjDir)\..\gs\newdb\DiplomacyThreatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyThreatRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\DiplomacyThreatRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\EndGameObject.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__ENDGA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\EndGameObject.cdb

"$(ProjDir)\..\gs\newdb\EndGameObjectRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\EndGameObjectRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\EndGameObjectRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\feat.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__FEAT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\feat.cdb

"$(ProjDir)\..\gs\newdb\FeatRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\FeatRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\FeatRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\GlobalWarming.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__GLOBA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\GlobalWarming.cdb

"$(ProjDir)\..\gs\newdb\GlobalWarmingRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\GlobalWarmingRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\GlobalWarmingRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\government.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__GOVER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\government.cdb

"$(ProjDir)\..\gs\newdb\GovernmentRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\GovernmentRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\GovernmentRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\icon.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__ICON_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\icon.cdb

"$(ProjDir)\..\gs\newdb\IconRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\IconRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\IconRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Leader.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Leader.cdb

"$(ProjDir)\..\gs\newdb\LeaderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\LeaderRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\LeaderRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\lex.dbl.c

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Map.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Map.cdb

"$(ProjDir)\..\gs\newdb\MapRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\MapIconRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\MapIconRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\MapRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\MapRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\order.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__ORDER="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\order.cdb

"$(ProjDir)\..\gs\newdb\OrderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\OrderRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\OrderRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Pollution.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__POLLU="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Pollution.cdb

"$(ProjDir)\..\gs\newdb\PollutionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\PollutionRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\PollutionRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\pop.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__POP_C="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\pop.cdb

"$(ProjDir)\..\gs\newdb\PopRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\PopRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\PopRecord.h
# End Source File
# Begin Source File

SOURCE=..\GS\newdb\religion.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\GS\newdb\religion.cdb

"$(ProjDir)\..\gs\newdb\ReligionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ReligionRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ReligionRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ResourceRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\ResourceRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\Risk.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__RISK_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\Risk.cdb

"$(ProjDir)\..\gs\newdb\RiskRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\RiskRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\RiskRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SoundRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SoundRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SpecialAttackInfoRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SpecialAttackInfoRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SpecialEffectRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SpecialEffectRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SpriteRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\SpriteRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\terrain.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__TERRA="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\terrain.cdb

"$(ProjDir)\..\gs\newdb\TerrainRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\TerrainImprovement.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__TERRAI="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\TerrainImprovement.cdb

"$(ProjDir)\..\gs\newdb\TerrainImprovementRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\TerrainImprovementRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\TerrainImprovementRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\TerrainRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\TerrainRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\unit.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__UNIT_="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unit.cdb

"$(ProjDir)\..\gs\newdb\UnitRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\unitpromotion.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\unitpromotion.cdb

"$(ProjDir)\..\gs\newdb\UnitpromotionRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\UnitpromotionRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\UnitpromotionRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\UnitRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\UnitRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\wonder.cdb

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__WONDE="$(ProjDir)\..\gs\dbgen\ctpdb.exe"	
# Begin Custom Build
ProjDir=.
InputPath=..\gs\newdb\wonder.cdb

"$(ProjDir)\..\gs\newdb\WonderRecord.stamp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\gs\dbgen\ctpdb.exe $(ProjDir)\..\gs\newdb < $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gs\newdb\WonderMovieRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\WonderMovieRecord.h
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\WonderRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\newdb\WonderRecord.h
# End Source File
# End Group
# Begin Group "outcom"

# PROP Default_Filter "*.h *.cpp"
# Begin Source File

SOURCE=..\gs\outcom\AICause.h
# End Source File
# Begin Source File

SOURCE=..\gs\outcom\c3rand.cpp
# End Source File
# Begin Source File

SOURCE=..\gs\outcom\C3Rand.h
# End Source File
# Begin Source File

SOURCE=..\gs\outcom\Ic3CivArchive.h
# End Source File
# Begin Source File

SOURCE=..\gs\outcom\IC3Rand.h
# End Source File
# End Group
# End Group
# Begin Group "net"

# PROP Default_Filter ""
# Begin Group "general"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\net\general\chatlist.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_achievement.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_achievement.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_action.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_action.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_agreement.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_agreement.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_army.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_army.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_cell.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_cell.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_chat.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_chat.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_cheat.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_cheat.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_city.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_city.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_civ.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_civ.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_crc.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_crc.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_diff.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_diff.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_diplomacy.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_diplomacy.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_endgame.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_endgame.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_exclusions.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_exclusions.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_feat.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_feat.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_gameobj.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_gameobj.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_gamesettings.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_guid.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_guid.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_happy.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_happy.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_hash.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_info.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_info.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_installation.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_installation.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_keys.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_keys.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_message.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_message.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_order.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_order.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_packet.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_player.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_player.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_pollution.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_pollution.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_rand.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_rand.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_ready.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_ready.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_report.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_report.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_research.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_research.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_strengths.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_strengths.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_terrain.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_terrain.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_tradeoffer.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_tradeoffer.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_traderoute.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_traderoute.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_unit.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_unit.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_vision.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_vision.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_wonder.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_wonder.h
# End Source File
# Begin Source File

SOURCE=..\net\general\net_world.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\net_world.h
# End Source File
# Begin Source File

SOURCE=..\net\general\network.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\network.h
# End Source File
# Begin Source File

SOURCE=..\net\general\networkevent.cpp
# End Source File
# Begin Source File

SOURCE=..\net\general\networkevent.h
# End Source File
# End Group
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\net\io\net_anet.cpp
# End Source File
# Begin Source File

SOURCE=..\net\io\net_anet.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_array.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_debug.cpp
# End Source File
# Begin Source File

SOURCE=..\net\io\net_debug.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_error.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_io.cpp
# End Source File
# Begin Source File

SOURCE=..\net\io\net_io.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_list.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_thread.cpp
# End Source File
# Begin Source File

SOURCE=..\net\io\net_thread.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_types.h
# End Source File
# Begin Source File

SOURCE=..\net\io\net_util.h
# End Source File
# End Group
# End Group
# Begin Group "robot"

# PROP Default_Filter ""
# Begin Group "utility No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\robot\utility\RandTest.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\utility\RoboInit.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\utility\RoboInit.h
# End Source File
# End Group
# Begin Group "pathing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\robot\pathing\Astar.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\Astar.h
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\AstarPnt.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\AstarPnt.h
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\AVLHeap.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\AVLHeap.h
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\CityAstar.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\CityAstar.h
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\Path.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\Path.h
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\RobotAstar2.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\RobotAstar2.h
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\TradeAstar.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\TradeAstar.h
# End Source File
# Begin Source File

SOURCE=..\robot\pathing\UnitAstar.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\robot\pathing\UnitAstar.h
# End Source File
# End Group
# Begin Group "aibackdoor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\robot\aibackdoor\AVL.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\Bit_Table.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\BSet.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\BSet.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\civarchive.cpp
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\civarchive.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\common.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\DynArr.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\list_array.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\pool.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\priorityqueue.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\sdl_endian.h
# End Source File
# Begin Source File

SOURCE=..\robot\aibackdoor\semi_dynamic_array.h
# End Source File
# End Group
# End Group
# Begin Group "os"

# PROP Default_Filter ""
# Begin Group "common"

# PROP Default_Filter ""
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\os\include\config_win32.h
# End Source File
# Begin Source File

SOURCE=..\os\include\ctp2_config.h
# End Source File
# Begin Source File

SOURCE=..\os\include\ctp2_inttypes.h
# End Source File
# End Group
# Begin Group "win32"

# PROP Default_Filter ""
# End Group
# End Group
# Begin Group "ui"

# PROP Default_Filter ""
# Begin Group "aui_common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\aui_common\aui.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_action.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_action.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_animate.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_animate.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_audiomanager.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_audiomanager.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_base.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_base.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_bitmapfont.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /FAcs /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_bitmapfont.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_blitter.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_blitter.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_button.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_button.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_control.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_control.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_cursor.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_cursor.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dimension.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dimension.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dirtylist.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dirtylist.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dragdropwindow.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dragdropwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dropdown.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_dropdown.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_Factory.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_Factory.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_font.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_font.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_gamespecific.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_gamespecific.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_header.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_header.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_hypertextbase.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_hypertextbase.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_hypertextbox.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_hypertextbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_image.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_image.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_imagebase.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_imagebase.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_imagelist.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_imagelist.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_input.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_input.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_item.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_item.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_joystick.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_joystick.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_keyboard.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_keyboard.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_ldl.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_ldl.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_listbox.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_listbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_memmap.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_memmap.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_mouse.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_mouse.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_movie.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_movie.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_moviebutton.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_moviebutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_moviemanager.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_moviemanager.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_music.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_music.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_pixel.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_pixel.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_progressbar.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_progressbar.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_radio.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_radio.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_ranger.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_ranger.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_rectangle.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_rectangle.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_region.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_region.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_regiondispatch.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_resource.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_screen.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_screen.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_shell.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_shell.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_sound.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_sound.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_soundbase.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_soundbase.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_static.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_static.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_stringtable.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_stringtable.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_surface.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_surface.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_switch.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_switch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_switchgroup.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_switchgroup.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_tab.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_tab.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_tabgroup.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_tabgroup.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_textbase.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_textbase.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_textbox.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_textbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_textfield.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_textfield.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_thumb.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_thumb.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_tipwindow.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_tipwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_ui.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_ui.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_undo.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_undo.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_uniqueid.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_uniqueid.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_win.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_win.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_window.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\aui_window.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\auicfg.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\auidebug.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\auitypes.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\dxver.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\tech_memmap.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\tech_memmap.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\tech_memory.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_common\tech_wllist.h
# End Source File
# End Group
# Begin Group "aui_utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\aui_utils\primitives.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_utils\primitives.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_utils\textutils.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_utils\textutils.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_utils\vidplay.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_utils\vidplay.h
# End Source File
# End Group
# Begin Group "aui_directx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directaudiomanager.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directaudiomanager.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directblitter.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directblitter.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directinput.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directinput.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directjoystick.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directjoystick.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directkeyboard.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directkeyboard.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directmouse.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directmouse.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directmovie.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directmovie.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directmoviemanager.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directmoviemanager.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directsound.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directsound.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directsurface.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directsurface.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directui.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directui.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directx.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_directx\aui_directx.h
# End Source File
# End Group
# Begin Group "aui_sdl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdl.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdl.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlinput.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlinput.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdljoystick.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdljoystick.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlkeyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlkeyboard.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlmouse.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlmouse.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlsound.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlsound.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlsurface.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_sdl\aui_sdlsurface.h
# End Source File
# End Group
# Begin Group "aui_ctp2"

# PROP Default_Filter ""
# Begin Group "Original"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\aui_ctp2\background.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\background.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\battleorderbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\battleorderbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\battleorderboxactions.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\battleorderboxactions.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\bevellesswindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\bevellesswindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\buttonbank.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\buttonbank.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_button.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_button.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_checkbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_checkbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_coloredstatic.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_coloredstatic.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_coloredswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_coloredswitch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_coloriconbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_coloriconbutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_dropdown.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_dropdown.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_fancywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_fancywindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_header.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_header.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_headerswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_headerswitch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_hypertextbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_hypertextbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_hypertipwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_hypertipwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_icon.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_icon.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_image.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_image.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_listbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_listitem.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_listitem.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_popupwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_popupwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_radio.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_radio.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_ranger.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_ranger.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_slidometer.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_slidometer.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_static.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_static.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_switch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_switch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_thumb.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_thumb.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_updateaction.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_updateaction.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_utilitydialogbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3_utilitydialogbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3blitter.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3blitter.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3dropdown.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3dropdown.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3fancywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3fancywindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3imageformats.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3imageformats.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3listbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3memmap.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3memmap.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3scroller.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3scroller.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3slider.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3slider.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3spinner.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3spinner.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3textfield.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3textfield.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3thumb.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3thumb.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3ui.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3ui.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3window.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3window.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3windows.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\c3windows.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\chart.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\chart.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\checkbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\cityinventorylistbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\cityinventorylistbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\coloriconbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\coloriconbutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\coloriconswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\coloriconswitch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\controlsheet.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\controlsheet.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\directvideo.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\directvideo.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\grabitem.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\grabitem.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\icon.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\icon.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\iconbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\iconbutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\iconswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\iconswitch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\icontab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\icontab.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\InfoBar.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\InfoBar.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\iteminfo.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\iteminfo.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\keyboardhandler.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\keymap.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\keymap.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\keypress.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /FAcs
# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\keypress.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\linegraph.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\linegraph.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\pattern.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\pattern.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\patternbase.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\patternbase.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\picture.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\picture.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\picturebutton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\picturebutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\pictureswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\pictureswitch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\picturetab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\picturetab.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\radarmap.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\radarmap.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\radiogroup.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\radiogroup.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\SelItem.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# ADD BASE CPP /FAcs
# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\SelItem.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\SelItemClick.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\SelItemEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\SelItemEvent.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\staticpicture.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\staticpicture.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\statictextitem.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\statictextitem.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\statuswindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\statuswindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textbutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textradio.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textradio.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\textswitch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\texttab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\texttab.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\thermometer.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\thermometer.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\thronecontrol.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\thronecontrol.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\thumbnailmap.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\thumbnailmap.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\tilecontrol.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\tilecontrol.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\tipwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\tipwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\unittabbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\unittabbutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\videowindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\videowindow.h
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\workmap.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx\tilesys\workmap.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_button.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_button.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_commandline.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_commandline.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_dropdown.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_dropdown.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_hypertextbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_hypertextbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_listbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_listitem.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_listitem.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Menu.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Menu.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_menubar.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_menubar.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_MenuButton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_MenuButton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_spinner.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_spinner.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_static.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_static.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Switch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Switch.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Tab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Tab.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_TabButton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_TabButton.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_TabGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_TabGroup.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_textbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_textbuffer.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_textfield.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_textfield.h
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Window.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\aui_ctp2\ctp2_Window.h
# End Source File
# End Group
# Begin Group "ldl"

# PROP Default_Filter ".cpp .hpp .h"
# Begin Source File

SOURCE=..\ui\ldl\ldl.h
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl.l

!IF  "$(CFG)" == "ctp2 - Win32 Release"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

USERDEP__LDL_L="..\ui\ldl\ldl.y"	
# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.l

"$(ProjDir)\..\ui\ldl\lex.ldl.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(CDKDIR)\flex -i -Pldl -o$(ProjDir)\..\ui\ldl\lex.ldl.c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl.tab.c

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl.tab.h
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl.y

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# Begin Custom Build
ProjDir=.
InputPath=..\ui\ldl\ldl.y

BuildCmds= \
	$(CDKDIR)\byacc -d -v -b..\\ui\\ldl\\ldl  ..\\ui\\ldl\\ldl.y

"$(ProjDir)\..\ui\ldl\ldl.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\ui\ldl\ldl.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_attr.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_attr.hpp
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_data.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_data.hpp
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_data_info.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_file.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_file.hpp
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_memmap.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldl_memmap.h
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldlif.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\ldlif.h
# End Source File
# Begin Source File

SOURCE=..\ui\ldl\lex.ldl.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "interface"

# PROP Default_Filter ""
# Begin Group "ctp2_MainUI"

# PROP Default_Filter ""
# Begin Group "ctp2_ControlPanel"

# PROP Default_Filter ""
# Begin Group "ctp2_ControlPanelTabs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\CityControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\CityControlPanel.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\DomesticControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\DomesticControlPanel.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\MessageControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\MessageControlPanel.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\TilesControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\TilesControlPanel.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\UnitControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\UnitControlPanel.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ui\interface\ControlTabPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ControlTabPanel.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\EndTurnButton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\EndTurnButton.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\MainControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\MainControlPanel.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ShortcutPad.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ShortcutPad.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\StatusBar.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\StatusBar.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\TurnYearStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\TurnYearStatus.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ZoomPad.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ZoomPad.h
# End Source File
# End Group
# Begin Group "ctp2_MenuBar"

# PROP Default_Filter ""
# End Group
# Begin Group "ctp2_MiniMap"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=..\ui\interface\controlpanelwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\controlpanelwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\interfaceevent.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\interfaceevent.h
# End Source File
# End Group
# Begin Group "Etc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\agesscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\agesscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ancientwindows.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ancientwindows.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\armymanagerwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\armymanagerwindow.h
# End Source File
# Begin Source File

SOURCE=..\Ui\interface\AttractWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\Ui\interface\AttractWindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\backgroundwin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\backgroundwin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\c3dialogs.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\c3dialogs.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ChatBox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ChatBox.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\CityEspionage.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\cityespionage.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\citymanager.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\citymanager.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\creditsscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\creditsscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\CursorManager.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\CursorManager.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\custommapscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\custommapscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\debugwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\debugwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\DiplomacyDetails.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\DiplomacyDetails.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\diplomacywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\diplomacywindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\dipwizard.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\dipwizard.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\editqueue.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\editqueue.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\FileDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\FileDialog.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\gameplayoptions.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\gameplayoptions.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\graphicsresscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\graphicsresscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\graphicsscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\graphicsscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\greatlibrary.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\greatlibrary.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\greatlibrary_util.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\greatlibrarywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\greatlibrarywindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\helptile.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\helptile.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\hotseatlist.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\hotseatlist.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\infowin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\infowin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\initialplayscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\initialplaywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\initialplaywindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\intelligencewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\intelligencewindow.h
# End Source File
# Begin Source File

SOURCE=..\Ui\interface\IntroMovieWin.cpp
# End Source File
# Begin Source File

SOURCE=..\Ui\interface\IntroMovieWin.h
# End Source File
# Begin Source File

SOURCE=..\Ui\interface\IntroMovieWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\Ui\interface\IntroMovieWindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\km_screen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\km_screen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\loadsavemapscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\loadsavemapwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\loadsavemapwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\loadsavescreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\loadsavewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\loadsavewindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\MapCopyBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\MapCopyBuffer.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageactions.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageactions.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageadvice.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageadvice.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageeyepoint.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageeyepoint.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageiconbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageiconbutton.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageiconwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageiconwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagelist.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagelist.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagemodal.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagemodal.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageresponse.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messageresponse.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagewin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagewin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\messagewindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\musicscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\musicscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\musictrackscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\musictrackscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\optionsscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\optionswindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\optionswindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\optionwarningscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\optionwarningscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ProfileEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ProfileEdit.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\progresswindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\progresswindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\radarwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\radarwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scenarioeditor.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scenarioeditor.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scenarioscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scenariowindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scenariowindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\sciencewin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\sciencewin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scorewarn.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scorewarn.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\screenutils.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\screenutils.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\soundscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\soundscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\SpecialAttackWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\SpecialAttackWindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\splash.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\splash.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamediffscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamediffscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamemapshapescreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamemapshapescreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamemapsizescreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamemapsizescreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgameplayersscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgameplayersscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamerandomcustomscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamerandomcustomscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamerulesscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamerulesscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spNewGameScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgametribescreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgametribescreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spnewgamewindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spriteeditor.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\spriteeditor.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\statictextbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\statictextbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\statswindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\statswindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\String_Search.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\String_Search.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\Text_Hasher.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\tileimptracker.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\tileimptracker.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\trademanager.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\trademanager.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\tutorialwin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\tutorialwin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\UIUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\UIUtils.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\unitmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\unitmanager.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\victorymoviewin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\victorymoviewin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\VictoryMovieWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\VictoryMovieWindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\victorywin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\victorywin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\victorywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\victorywindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\wondermoviewin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\wondermoviewin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\WonderMovieWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\WonderMovieWindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\workwin.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\workwin.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\workwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\workwindow.h
# End Source File
# End Group
# Begin Group "ctp2_Utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\InterfaceEventRegister.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\InterfaceEventRegister.h
# End Source File
# End Group
# Begin Group "ctp2_Dialogs"

# PROP Default_Filter ""
# Begin Group "DomesticManagement"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\CauseAndEffectTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\CauseAndEffectTab.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\DomesticManagementDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\DomesticManagementDialog.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\GovernmentTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\GovernmentTab.h
# End Source File
# End Group
# Begin Group "City_Management"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\citywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\citywindow.h
# End Source File
# End Group
# Begin Group "Science"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\sci_advancescreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\sci_advancescreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ScienceManagementDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\ScienceManagementDialog.h
# End Source File
# End Group
# Begin Group "NationalManagement"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\NationalManagementDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\NationalManagementDialog.h
# End Source File
# End Group
# Begin Group "MessageBox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\MessageBoxDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\MessageBoxDialog.h
# End Source File
# End Group
# Begin Group "BattleView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\Battle.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\Battle.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\BattleEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\BattleEvent.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\BattleView.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\BattleView.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\BattleViewWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\BattleViewWindow.h
# End Source File
# End Group
# Begin Group "InformationDialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\infowindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\infowindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\rankingtab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\rankingtab.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scoretab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\scoretab.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\timelinetab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\timelinetab.h
# End Source File
# Begin Source File

SOURCE=..\ui\interface\WonderTab.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\WonderTab.h
# End Source File
# End Group
# Begin Group "ScienceVictory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\interface\sciencevictorydialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\interface\sciencevictorydialog.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "freetype No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\freetype\freetype.h
# End Source File
# Begin Source File

SOURCE=..\ui\freetype\fterrid.h
# End Source File
# Begin Source File

SOURCE=..\ui\freetype\ftnameid.h
# End Source File
# End Group
# Begin Group "netshell"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\netshell\allinonewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\allinonewindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\connectionselectwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\connectionselectwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\dialogboxwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\dialogboxwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\gameselectwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\gameselectwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\lobbychangewindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\lobbychangewindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\lobbywindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\lobbywindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\netfunc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\netfunc.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\netshell.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\netshell.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\netshell_game.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\netshell_game.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_accessor.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_aiplayer.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_aiplayer.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_aiplayersetup.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_aiplayersetup.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_chatbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_chatbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_civlistbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_civlistbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_customlistbox.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_customlistbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_game.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_game.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_gamesetup.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_gamesetup.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_gplayer.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_gplayer.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_header.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_header.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_headerswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_headerswitch.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_improvements.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_improvements.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_item.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_item.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_listbox.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_lobby.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_lobby.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_object.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_player.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_player.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_playersetup.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_playersetup.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_rplayer.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_rplayer.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_server.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_server.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_session.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_session.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_spinner.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_spinner.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_string.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_string.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_transport.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_transport.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_tribes.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_tribes.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_units.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_units.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_window.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_window.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_wonders.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\ns_wonders.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\passwordscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\passwordscreen.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\playereditwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\playereditwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\playerselectwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\playerselectwindow.h
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\serverselectwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\netshell\serverselectwindow.h
# End Source File
# End Group
# Begin Group "slic_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ui\slic_debug\segmentlist.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\slic_debug\segmentlist.h
# End Source File
# Begin Source File

SOURCE=..\ui\slic_debug\sourcelist.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\slic_debug\sourcelist.h
# End Source File
# Begin Source File

SOURCE=..\ui\slic_debug\watchlist.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\slic_debug\watchlist.h
# End Source File
# End Group
# End Group
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sound\civsound.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\civsound.h
# End Source File
# Begin Source File

SOURCE=..\sound\GameSounds.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\GameSounds.h
# End Source File
# Begin Source File

SOURCE=..\libs\miles\MSS.H
# End Source File
# Begin Source File

SOURCE=..\sound\soundevent.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\soundevent.h
# End Source File
# Begin Source File

SOURCE=..\sound\SoundManager.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\SoundManager.h
# End Source File
# End Group
# Begin Group "ai"

# PROP Default_Filter ""
# Begin Group "Diplomacy"

# PROP Default_Filter ""
# Begin Group "EventHandlers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ai\diplomacy\CounterResponseEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\CounterResponseEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\DStateEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\DStateEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\MotivationEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\MotivationEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\NProposalEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\NProposalEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ProposalResponseEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ProposalResponseEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ReactEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ReactEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\RegardEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\RegardEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\RejectResponseEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\RejectResponseEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ResponseEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ResponseEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\SStateEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\SStateEvent.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ThreatResponseEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ThreatResponseEvent.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ai\diplomacy\AgreementMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\AgreementMatrix.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\diplomacyutil.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\diplomacyutil.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\Diplomat.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /I "..\ai\personality" /I "..\ctp\ctp2_utils"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /I "..\ai\personality" /I "..\ctp\ctp2_utils"
# ADD CPP /I "..\ai\personality" /I "..\ctp\ctp2_utils"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /I "..\ai\personality" /I "..\ctp\ctp2_utils"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\Diplomat.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\diplomattypes.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\DiplomatTypes.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\Foreigner.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /I "..\ai\personality"
# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\Foreigner.h
# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ProposalAnalysis.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /I "..\ai\personality"
# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /I "..\ai\personality"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ai\diplomacy\ProposalAnalysis.h
# End Source File
# End Group
# Begin Group "strategy"

# PROP Default_Filter ""
# Begin Group "scheduler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ai\strategy\Scheduler\Goal_And_Squad_Types.h
# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Scheduler\Plan.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Scheduler\Plan.h
# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Scheduler\Scheduler.cpp

!IF  "$(CFG)" == "ctp2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug"

# ADD CPP /GR-

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Test"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 NDebug"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Debug Browse"

# ADD BASE CPP /GR-
# ADD CPP /GR-

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Optimized Test"

!ELSEIF  "$(CFG)" == "ctp2 - SDL Debug"

# ADD CPP /GR-

!ELSEIF  "$(CFG)" == "ctp2 - SDL Final"

!ELSEIF  "$(CFG)" == "ctp2 - Win32 Final with Logging"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Scheduler\Scheduler.h
# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Scheduler\scheduler_types.h
# End Source File
# End Group
# Begin Group "goals"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ai\strategy\Goals\Goal.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Goals\Goal.h
# End Source File
# End Group
# Begin Group "squads"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ai\strategy\Squads\squad_Strength.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Squads\squad_strength.h
# End Source File
# End Group
# Begin Group "agents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ai\strategy\Agents\agent.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\strategy\Agents\agent.h
# End Source File
# End Group
# End Group
# Begin Group "MapAnalysis"

# PROP Default_Filter "*.cpp *.h"
# Begin Source File

SOURCE=..\ai\mapanalysis\BoundingRect.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\mapanalysis\BoundingRect.h
# End Source File
# Begin Source File

SOURCE=..\ai\mapanalysis\MapAnalysis.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\mapanalysis\mapanalysis.h
# End Source File
# Begin Source File

SOURCE=..\ai\mapanalysis\mapgrid.h
# End Source File
# Begin Source File

SOURCE=..\ai\mapanalysis\SettleMap.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\mapanalysis\SettleMap.h
# End Source File
# End Group
# Begin Group "CityManagement"

# PROP Default_Filter "*.cpp *.h"
# Begin Source File

SOURCE=..\ai\CityManagement\Governor.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\CityManagement\Governor.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ai\CtpAi.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\CtpAi.h
# End Source File
# Begin Source File

SOURCE=..\ai\CtpAiDebug.cpp
# End Source File
# Begin Source File

SOURCE=..\ai\CtpAiDebug.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ctp2_rsrc\CivCTP.ico
# End Source File
# Begin Source File

SOURCE=.\ctp2_rsrc\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\ctp2_rsrc\icon2.ico
# End Source File
# End Target
# End Project
