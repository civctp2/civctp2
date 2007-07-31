# Microsoft Developer Studio Generated NMAKE File, Based on Plasma1.dsp
!IF "$(CFG)" == ""
CFG=Plasma1 - Win32 Release
!MESSAGE No configuration specified. Defaulting to Plasma1 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Plasma1 - Win32 Release" && "$(CFG)" !=\
 "Plasma1 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Plasma1.mak" CFG="Plasma1 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Plasma1 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma1 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Plasma1 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\civ3\dll\map\plasma1.dll"

!ELSE 

ALL : "..\civ3\dll\map\plasma1.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\PlasmaGen1.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\plasma1.exp"
	-@erase "$(OUTDIR)\plasma1.lib"
	-@erase "..\civ3\dll\map\plasma1.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\civ3\\" /I "..\civ3\civ3_utils" /I\
 "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I\
 "..\ui\freetype" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Plasma1.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Plasma1.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\plasma1.pdb" /machine:I386 /def:".\Plasma1.def"\
 /out:"..\civ3\dll\map\plasma1.dll" /implib:"$(OUTDIR)\plasma1.lib" 
DEF_FILE= \
	".\Plasma1.def"
LINK32_OBJS= \
	"$(INTDIR)\PlasmaGen1.obj"

"..\civ3\dll\map\plasma1.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Plasma1 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\civ3\dll\map\plasma1.dll"

!ELSE 

ALL : "..\civ3\dll\map\plasma1.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\PlasmaGen1.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\plasma1.exp"
	-@erase "$(OUTDIR)\plasma1.lib"
	-@erase "$(OUTDIR)\plasma1.pdb"
	-@erase "..\civ3\dll\map\plasma1.dll"
	-@erase "..\civ3\dll\map\plasma1.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\civ3\\" /I\
 "..\civ3\civ3_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I\
 "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Plasma1.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Plasma1.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\plasma1.pdb" /debug\
 /machine:I386 /def:".\Plasma1.def" /out:"..\civ3\dll\map\plasma1.dll"\
 /implib:"$(OUTDIR)\plasma1.lib" /pdbtype:sept 
DEF_FILE= \
	".\Plasma1.def"
LINK32_OBJS= \
	"$(INTDIR)\PlasmaGen1.obj"

"..\civ3\dll\map\plasma1.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "Plasma1 - Win32 Release" || "$(CFG)" ==\
 "Plasma1 - Win32 Debug"
SOURCE=.\PlasmaGen1.cpp

!IF  "$(CFG)" == "Plasma1 - Win32 Release"

DEP_CPP_PLASM=\
	"..\civ3\c3.h"\
	"..\civ3\c3aui.h"\
	"..\civ3\c3types.h"\
	"..\civ3\civ3_utils\c3debug.h"\
	"..\civ3\civ3_utils\c3errors.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_action.h"\
	"..\ui\aui_common\aui_animate.h"\
	"..\ui\aui_common\aui_audiomanager.h"\
	"..\ui\aui_common\aui_base.h"\
	"..\ui\aui_common\aui_bitmapfont.h"\
	"..\ui\aui_common\aui_blitter.h"\
	"..\ui\aui_common\aui_button.h"\
	"..\ui\aui_common\aui_control.h"\
	"..\ui\aui_common\aui_cursor.h"\
	"..\ui\aui_common\aui_dimension.h"\
	"..\ui\aui_common\aui_dirtylist.h"\
	"..\ui\aui_common\aui_dropdown.h"\
	"..\ui\aui_common\aui_font.h"\
	"..\ui\aui_common\aui_header.h"\
	"..\ui\aui_common\aui_image.h"\
	"..\ui\aui_common\aui_imagebase.h"\
	"..\ui\aui_common\aui_input.h"\
	"..\ui\aui_common\aui_item.h"\
	"..\ui\aui_common\aui_joystick.h"\
	"..\ui\aui_common\aui_keyboard.h"\
	"..\ui\aui_common\aui_ldl.h"\
	"..\ui\aui_common\aui_listbox.h"\
	"..\ui\aui_common\aui_memmap.h"\
	"..\ui\aui_common\aui_mouse.h"\
	"..\ui\aui_common\aui_movie.h"\
	"..\ui\aui_common\aui_moviemanager.h"\
	"..\ui\aui_common\aui_music.h"\
	"..\ui\aui_common\aui_pixel.h"\
	"..\ui\aui_common\aui_progressbar.h"\
	"..\ui\aui_common\aui_radio.h"\
	"..\ui\aui_common\aui_ranger.h"\
	"..\ui\aui_common\aui_rectangle.h"\
	"..\ui\aui_common\aui_region.h"\
	"..\ui\aui_common\aui_resource.h"\
	"..\ui\aui_common\aui_screen.h"\
	"..\ui\aui_common\aui_shell.h"\
	"..\ui\aui_common\aui_sound.h"\
	"..\ui\aui_common\aui_soundbase.h"\
	"..\ui\aui_common\aui_static.h"\
	"..\ui\aui_common\aui_stringtable.h"\
	"..\ui\aui_common\aui_surface.h"\
	"..\ui\aui_common\aui_switch.h"\
	"..\ui\aui_common\aui_switchgroup.h"\
	"..\ui\aui_common\aui_tab.h"\
	"..\ui\aui_common\aui_tabgroup.h"\
	"..\ui\aui_common\aui_textbase.h"\
	"..\ui\aui_common\aui_textbox.h"\
	"..\ui\aui_common\aui_textfield.h"\
	"..\ui\aui_common\aui_thumb.h"\
	"..\ui\aui_common\aui_tipwindow.h"\
	"..\ui\aui_common\aui_ui.h"\
	"..\ui\aui_common\aui_undo.h"\
	"..\ui\aui_common\aui_uniqueid.h"\
	"..\ui\aui_common\aui_win.h"\
	"..\ui\aui_common\aui_window.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\aui_directx\aui_directaudiomanager.h"\
	"..\ui\aui_directx\aui_directblitter.h"\
	"..\ui\aui_directx\aui_directinput.h"\
	"..\ui\aui_directx\aui_directjoystick.h"\
	"..\ui\aui_directx\aui_directkeyboard.h"\
	"..\ui\aui_directx\aui_directmouse.h"\
	"..\ui\aui_directx\aui_directsound.h"\
	"..\ui\aui_directx\aui_directsurface.h"\
	"..\ui\aui_directx\aui_directui.h"\
	"..\ui\aui_directx\aui_directwindow.h"\
	"..\ui\aui_directx\aui_directx.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\imapgen.h"\
	".\PlasmaGen1.h"\
	{$(INCLUDE)}"dinput.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\PlasmaGen1.obj" : $(SOURCE) $(DEP_CPP_PLASM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Plasma1 - Win32 Debug"

DEP_CPP_PLASM=\
	"..\civ3\c3.h"\
	"..\civ3\c3types.h"\
	"..\civ3\civ3_utils\c3debug.h"\
	".\imapgen.h"\
	".\PlasmaGen1.h"\
	{$(INCLUDE)}"dinput.h"\
	

"$(INTDIR)\PlasmaGen1.obj" : $(SOURCE) $(DEP_CPP_PLASM) "$(INTDIR)"


!ENDIF 


!ENDIF 

