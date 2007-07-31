# Microsoft Developer Studio Generated NMAKE File, Based on Plasma2.dsp
!IF "$(CFG)" == ""
CFG=Plasma2 - Win32 Test
!MESSAGE No configuration specified. Defaulting to Plasma2 - Win32 Test.
!ENDIF 

!IF "$(CFG)" != "Plasma2 - Win32 Release" && "$(CFG)" !=\
 "Plasma2 - Win32 Debug" && "$(CFG)" != "Plasma2 - Win32 Test"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Plasma2.mak" CFG="Plasma2 - Win32 Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Plasma2 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plasma2 - Win32 Test" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Plasma2 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\civ3\dll\map\plasma2.dll"

!ELSE 

ALL : "..\civ3\dll\map\plasma2.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\PlasmaGen2.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\plasma2.exp"
	-@erase "$(OUTDIR)\plasma2.lib"
	-@erase "..\civ3\dll\map\plasma2.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\libs\dxmedia\classes\base" /I\
 "..\libs\dxmedia\include" /I "..\civ3\\" /I "..\civ3\civ3_utils" /I\
 "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I\
 "..\ui\freetype" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Plasma2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Plasma2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=uuid.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\plasma2.pdb" /machine:I386 /def:".\Plasma2.def"\
 /out:"..\civ3\dll\map\plasma2.dll" /implib:"$(OUTDIR)\plasma2.lib" 
DEF_FILE= \
	".\Plasma2.def"
LINK32_OBJS= \
	"$(INTDIR)\PlasmaGen2.obj"

"..\civ3\dll\map\plasma2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\ctp\dll\map\plasma2.dll"

!ELSE 

ALL : "..\ctp\dll\map\plasma2.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\PlasmaGen2.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\plasma2.exp"
	-@erase "$(OUTDIR)\plasma2.lib"
	-@erase "..\ctp\dll\map\plasma2.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\libs\dxmedia\classes\base" /I\
 "..\libs\dxmedia\include" /I "..\ctp\\" /I "..\ctp\civ3_utils" /I\
 "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I "..\ui\aui_directx" /I\
 "..\ui\freetype" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Plasma2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Plasma2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=uuid.lib /nologo /subsystem:windows /dll /profile /debug\
 /machine:I386 /nodefaultlib:"libcd.lib" /def:".\Plasma2.def"\
 /out:"..\ctp\dll\map\plasma2.dll" /implib:"$(OUTDIR)\plasma2.lib" 
DEF_FILE= \
	".\Plasma2.def"
LINK32_OBJS= \
	"$(INTDIR)\PlasmaGen2.obj"

"..\ctp\dll\map\plasma2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Test"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\ctp\dll\map\plasma2.dll"

!ELSE 

ALL : "..\ctp\dll\map\plasma2.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\PlasmaGen2.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\plasma2.exp"
	-@erase "$(OUTDIR)\plasma2.lib"
	-@erase "$(OUTDIR)\plasma2.pdb"
	-@erase "..\ctp\dll\map\plasma2.dll"
	-@erase "..\ctp\dll\map\plasma2.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /O2 /Ob2 /I\
 "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\ctp\\" /I\
 "..\ctp\civ3_utils" /I "..\gs\outcom" /I "..\ui\aui_common" /I "..\ui\ldl" /I\
 "..\ui\aui_directx" /I "..\ui\freetype" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_TEST" /Fp"$(INTDIR)\Plasma2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\
 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Plasma2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=uuid.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\plasma2.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib"\
 /def:".\Plasma2.def" /out:"..\ctp\dll\map\plasma2.dll"\
 /implib:"$(OUTDIR)\plasma2.lib" /pdbtype:sept 
DEF_FILE= \
	".\Plasma2.def"
LINK32_OBJS= \
	"$(INTDIR)\PlasmaGen2.obj"

"..\ctp\dll\map\plasma2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "Plasma2 - Win32 Release" || "$(CFG)" ==\
 "Plasma2 - Win32 Debug" || "$(CFG)" == "Plasma2 - Win32 Test"
SOURCE=.\PlasmaGen2.cpp

!IF  "$(CFG)" == "Plasma2 - Win32 Release"

DEP_CPP_PLASM=\
	"..\gs\outcom\IC3Rand.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
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
	"..\ui\aui_common\aui_gamespecific.h"\
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
	"..\ui\aui_common\tech_memmap.h"\
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
	".\PlasmaGen2.h"\
	"c:\mssdk\include\dinput.h"\
	
NODEP_CPP_PLASM=\
	"..\civ3\c3.h"\
	"..\civ3\c3aui.h"\
	"..\civ3\c3types.h"\
	"..\civ3\civ3_utils\c3debug.h"\
	"..\civ3\civ3_utils\c3errors.h"\
	

"$(INTDIR)\PlasmaGen2.obj" : $(SOURCE) $(DEP_CPP_PLASM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Debug"

DEP_CPP_PLASM=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\outcom\IC3Rand.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\imapgen.h"\
	".\PlasmaGen2.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\PlasmaGen2.obj" : $(SOURCE) $(DEP_CPP_PLASM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Plasma2 - Win32 Test"

DEP_CPP_PLASM=\
	"..\ctp\c3.h"\
	"..\ctp\c3types.h"\
	"..\ctp\civ3_utils\c3debug.h"\
	"..\ctp\civ3_utils\c3errors.h"\
	"..\gs\outcom\IC3Rand.h"\
	"..\libs\dxmedia\classes\base\reftime.h"\
	"..\libs\dxmedia\include\amstream.h"\
	"..\libs\dxmedia\include\austream.h"\
	"..\libs\dxmedia\include\ddstream.h"\
	"..\libs\dxmedia\include\mmstream.h"\
	"..\libs\dxmedia\include\strmif.h"\
	"..\ui\aui_common\aui.h"\
	"..\ui\aui_common\aui_gamespecific.h"\
	"..\ui\aui_common\auicfg.h"\
	"..\ui\aui_common\auidebug.h"\
	"..\ui\aui_common\auitypes.h"\
	"..\ui\aui_common\tech_memmap.h"\
	"..\ui\aui_common\tech_memory.h"\
	"..\ui\aui_common\tech_wllist.h"\
	"..\ui\freetype\freetype.h"\
	"..\ui\freetype\fterrid.h"\
	"..\ui\freetype\ftnameid.h"\
	"..\ui\ldl\ldl.h"\
	"..\ui\ldl\ldl_attr.hpp"\
	"..\ui\ldl\ldl_data.hpp"\
	"..\ui\ldl\ldl_file.hpp"\
	"..\ui\ldl\ldl_memmap.h"\
	".\imapgen.h"\
	".\PlasmaGen2.h"\
	"c:\mssdk\include\dinput.h"\
	

"$(INTDIR)\PlasmaGen2.obj" : $(SOURCE) $(DEP_CPP_PLASM) "$(INTDIR)"


!ENDIF 


!ENDIF 

