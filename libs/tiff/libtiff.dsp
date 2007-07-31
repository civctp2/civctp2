# Microsoft Developer Studio Project File - Name="libtiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtiff - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtiff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtiff.mak" CFG="libtiff - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtiff - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libtiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "libtiff - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtiff - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zd /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /D /D /Oxa /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"libtiff.lib"

!ENDIF 

# Begin Target

# Name "libtiff - Win32 Release"
# Name "libtiff - Win32 Debug"
# Begin Source File

SOURCE=.\tif_aux.c
# End Source File
# Begin Source File

SOURCE=.\tif_close.c
# End Source File
# Begin Source File

SOURCE=.\tif_codec.c
# End Source File
# Begin Source File

SOURCE=.\tif_compress.c
# End Source File
# Begin Source File

SOURCE=.\tif_dir.c
# End Source File
# Begin Source File

SOURCE=.\tif_dirinfo.c
# End Source File
# Begin Source File

SOURCE=.\tif_dirread.c
# End Source File
# Begin Source File

SOURCE=.\tif_dirwrite.c
# End Source File
# Begin Source File

SOURCE=.\tif_dumpmode.c
# End Source File
# Begin Source File

SOURCE=.\tif_error.c
# End Source File
# Begin Source File

SOURCE=.\tif_flush.c
# End Source File
# Begin Source File

SOURCE=.\tif_getimage.c
# End Source File
# Begin Source File

SOURCE=.\tif_jpeg.c
# End Source File
# Begin Source File

SOURCE=.\tif_lzw.c
# End Source File
# Begin Source File

SOURCE=.\tif_next.c
# End Source File
# Begin Source File

SOURCE=.\tif_open.c
# End Source File
# Begin Source File

SOURCE=.\tif_packbits.c
# End Source File
# Begin Source File

SOURCE=.\tif_predict.c
# End Source File
# Begin Source File

SOURCE=.\tif_print.c
# End Source File
# Begin Source File

SOURCE=.\tif_read.c
# End Source File
# Begin Source File

SOURCE=.\tif_strip.c
# End Source File
# Begin Source File

SOURCE=.\tif_swab.c
# End Source File
# Begin Source File

SOURCE=.\tif_thunder.c
# End Source File
# Begin Source File

SOURCE=.\tif_tile.c
# End Source File
# Begin Source File

SOURCE=.\tif_version.c
# End Source File
# Begin Source File

SOURCE=.\tif_warning.c
# End Source File
# Begin Source File

SOURCE=.\tif_win32.c
# End Source File
# Begin Source File

SOURCE=.\tif_write.c
# End Source File
# Begin Source File

SOURCE=.\tif_zip.c
# End Source File
# End Target
# End Project
