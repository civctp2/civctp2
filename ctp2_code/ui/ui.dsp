# Microsoft Developer Studio Project File - Name="ui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ui - Win32 Test
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak" CFG="ui - Win32 Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ui - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ui - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ui - Win32 Test" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "ui - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\libs\freetype\lib\\" /I "..\libs\freetype\utils\\" /I "..\libs\freetype\win32\\" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\mm\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ui___Win"
# PROP BASE Intermediate_Dir "ui___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /I "..\robotcom\backdoor" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\gs\slic" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /Fr /YX"c3.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ui - Win32 Test"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ui___Win"
# PROP BASE Intermediate_Dir "ui___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Test"
# PROP Intermediate_Dir "Test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /I "..\robotcom\backdoor" /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /I "..\gs\slic" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Ob2 /I "..\libs\freetype\utils" /I "..\libs\freetype\win32" /I "..\gs\slic" /I "..\robotcom\backdoor" /I "..\ui\interface" /I "..\gs\outcom" /I "..\robotcom\incom" /I "..\robot\utility" /I "..\robot\pathing" /I "..\libs\freetype\\" /I "..\civ3\\" /I "..\civ3\civ3_utils\\" /I "..\civ3\civ3_rsrc\\" /I "..\gfx\spritesys\\" /I "..\gfx\tilesys\\" /I "..\gfx\gfx_utils\\" /I "..\gs\database" /I "..\gs\fileio\\" /I "..\gs\gameobj\\" /I "..\gs\utility\\" /I "..\gs\world\\" /I "..\net\io\\" /I "..\net\general\\" /I "..\ui\aui_common" /I "..\ui\aui_utils\\" /I "..\ui\aui_directx\\" /I "..\ui\aui_civ3\\" /I "..\ui\aui_common\\" /I "..\ui\ldl" /I "..\libs\tiff\\" /I "..\libs\anet\h\\" /I "..\libs\dxmedia\classes\base" /I "..\libs\dxmedia\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AI_BACKDOOR" /YX"c3.h" /FD /c
# SUBTRACT CPP /Fr
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ui - Win32 Release"
# Name "ui - Win32 Debug"
# Name "ui - Win32 Test"
# Begin Group "aui_common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aui_common\aui.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_action.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_action.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_animate.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_animate.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_audiomanager.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_audiomanager.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_base.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_base.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_blitter.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_blitter.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_button.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_button.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_control.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_control.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_cursor.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_dimension.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_dimension.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_dirtylist.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_dirtylist.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_dropdown.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_dropdown.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_font.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_font.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_header.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_header.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_image.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_image.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_imagebase.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_imagebase.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_input.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_input.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_item.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_item.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_joystick.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_joystick.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_keyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_keyboard.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_ldl.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_ldl.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_listbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_memmap.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_memmap.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_mouse.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_music.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_music.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_pixel.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_pixel.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_progressbar.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_progressbar.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_radio.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_radio.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_ranger.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_ranger.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_rectangle.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_rectangle.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_region.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_region.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_regiondispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_resource.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_screen.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_screen.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_shell.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_shell.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_sound.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_soundbase.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_soundbase.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_static.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_static.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_stringtable.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_stringtable.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_surface.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_surface.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_switch.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_switch.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_switchgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_switchgroup.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_tab.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_tab.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_tabgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_tabgroup.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_textbase.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_textbase.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_textbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_textbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_textfield.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_textfield.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_thumb.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_thumb.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_tipwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_tipwindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_ui.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_ui.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_undo.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_undo.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_uniqueid.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_uniqueid.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_win.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_win.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_window.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_common\aui_window.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\auicfg.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\auidebug.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\auitypes.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\Dxver.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\tech_memory.h
# End Source File
# Begin Source File

SOURCE=.\aui_common\tech_wllist.h
# End Source File
# End Group
# Begin Group "aui_utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aui_utils\primitives.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_utils\primitives.h
# End Source File
# Begin Source File

SOURCE=.\aui_utils\textutils.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_utils\textutils.h
# End Source File
# End Group
# Begin Group "aui_directx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aui_directx\aui_directaudiomanager.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directaudiomanager.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directblitter.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directblitter.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directinput.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directinput.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directjoystick.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directjoystick.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directkeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directkeyboard.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directmouse.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directmouse.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directsound.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directsound.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directsurface.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directsurface.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directui.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directui.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directwindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directx.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_directx\aui_directx.h
# End Source File
# End Group
# Begin Group "aui_civ3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aui_civ3\background.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\background.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\battleorderbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\battleorderbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\battleorderboxactions.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\battleorderboxactions.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\bevellesswindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\bevellesswindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\buttonbank.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\buttonbank.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_button.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_button.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_coloredstatic.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_coloredstatic.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_dropdown.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_dropdown.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_header.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_header.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_headerswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_headerswitch.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_listbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_listitem.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_listitem.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_ranger.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_ranger.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_static.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_static.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_thumb.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_thumb.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_tradelistitem.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3_tradelistitem.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3blitter.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3blitter.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3dropdown.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3dropdown.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3fancywindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3fancywindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3imageformats.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3imageformats.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3listbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3memmap.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3memmap.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3scroller.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3scroller.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3slider.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3slider.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3spinner.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3spinner.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3textfield.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3textfield.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3thumb.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3thumb.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3ui.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3ui.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3window.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3window.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3windows.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\c3windows.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\chart.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\chart.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\checkbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\checkbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\cityinventorylistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\cityinventorylistbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\coloriconbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\coloriconbutton.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\coloriconswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\coloriconswitch.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\controlsheet.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\controlsheet.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\directvideo.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\directvideo.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\GrabItem.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\GrabItem.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\icon.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\icon.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\iconbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\iconbutton.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\iconswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\iconswitch.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\icontab.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\icontab.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\iteminfo.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\iteminfo.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\keymap.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\keymap.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\keypress.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\keypress.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\LineGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\LineGraph.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\pattern.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\pattern.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\patternbase.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\patternbase.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\picture.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\picturebutton.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\picturebutton.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\pictureswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\pictureswitch.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\picturetab.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\picturetab.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\radarcontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\radarcontrol.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\radarwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\radarwindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\radiogroup.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\radiogroup.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\SelItem.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\SelItem.h
# End Source File
# Begin Source File

SOURCE=.\interface\statictextbox.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\statictextbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\statictextitem.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\statictextitem.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\statuswindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\statuswindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textbox.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textbox.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textbutton.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textradio.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textradio.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\textswitch.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\texttab.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\texttab.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\TextTable.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\TextTable.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\thumbnailmap.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\thumbnailmap.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\tilecontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\tilecontrol.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\tipwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\tipwindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\videowindow.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\videowindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\workcontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\workcontrol.h
# End Source File
# End Group
# Begin Group "ldl"

# PROP Default_Filter ".cpp .hpp .h"
# Begin Source File

SOURCE=.\ldl\ldl.h
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_attr.cpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_attr.hpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_data.cpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_data.hpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_data_info.cpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_file.cpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_file.hpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_memmap.cpp
# End Source File
# Begin Source File

SOURCE=.\ldl\ldl_memmap.h
# End Source File
# End Group
# Begin Group "interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\interface\advancespad.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\advancespad.h
# End Source File
# Begin Source File

SOURCE=.\interface\ancientwindows.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\ancientwindows.h
# End Source File
# Begin Source File

SOURCE=.\interface\backgroundwin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\backgroundwin.h
# End Source File
# Begin Source File

SOURCE=.\interface\citypad.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\citypad.h
# End Source File
# Begin Source File

SOURCE=.\interface\citystatuswin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\citystatuswin.h
# End Source File
# Begin Source File

SOURCE=.\interface\citystatuswindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\citystatuswindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\cityviewwin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cityviewwin.h
# End Source File
# Begin Source File

SOURCE=.\interface\cityviewwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cityviewwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\civstatuswin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\civstatuswin.h
# End Source File
# Begin Source File

SOURCE=.\interface\civstatuswindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\civstatuswindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\controlpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\controlpanel.h
# End Source File
# Begin Source File

SOURCE=.\interface\controlpanelwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\controlpanelwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\cp_cityselection.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cp_cityselection.h
# End Source File
# Begin Source File

SOURCE=.\interface\cp_macrotab.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cp_macrotab.h
# End Source File
# Begin Source File

SOURCE=.\interface\cp_microtab.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cp_microtab.h
# End Source File
# Begin Source File

SOURCE=.\interface\cp_productiontab.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cp_productiontab.h
# End Source File
# Begin Source File

SOURCE=.\interface\cp_tileimp.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cp_tileimp.h
# End Source File
# Begin Source File

SOURCE=.\interface\cp_tradetab.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cp_tradetab.h
# End Source File
# Begin Source File

SOURCE=.\interface\cp_unitstab.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cp_unitstab.h
# End Source File
# Begin Source File

SOURCE=.\interface\cv_citytabgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cv_citytabgroup.h
# End Source File
# Begin Source File

SOURCE=.\interface\cv_happinesstab.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cv_happinesstab.h
# End Source File
# Begin Source File

SOURCE=.\interface\cv_pollutiontab.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cv_pollutiontab.h
# End Source File
# Begin Source File

SOURCE=.\interface\cv_statuscontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cv_statuscontrol.h
# End Source File
# Begin Source File

SOURCE=.\interface\cv_tradetabgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\cv_tradetabgroup.h
# End Source File
# Begin Source File

SOURCE=.\interface\debugwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\debugwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\DipWin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\DipWin.h
# End Source File
# Begin Source File

SOURCE=.\interface\DipWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\DipWindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\fliwin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\fliwin.h
# End Source File
# Begin Source File

SOURCE=.\interface\fliwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\fliwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\greatlibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\greatlibrary.h
# End Source File
# Begin Source File

SOURCE=.\interface\greatlibrarywindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\greatlibrarywindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\helpcity.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\helpcity.h
# End Source File
# Begin Source File

SOURCE=.\interface\helptile.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\helptile.h
# End Source File
# Begin Source File

SOURCE=.\interface\helpunit.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\helpunit.h
# End Source File
# Begin Source File

SOURCE=.\interface\initialplayscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\initialplaywindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\initialplaywindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\knowledgewin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\knowledgewin.h
# End Source File
# Begin Source File

SOURCE=.\interface\knowledgewindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\knowledgewindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\messageactions.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messageactions.h
# End Source File
# Begin Source File

SOURCE=.\interface\messageadvice.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messageadvice.h
# End Source File
# Begin Source File

SOURCE=.\interface\messageeyepoint.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messageeyepoint.h
# End Source File
# Begin Source File

SOURCE=.\interface\messageiconbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messageiconbutton.h
# End Source File
# Begin Source File

SOURCE=.\interface\messageiconwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messageiconwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\messagelist.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messagelist.h
# End Source File
# Begin Source File

SOURCE=.\interface\messagemodal.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messagemodal.h
# End Source File
# Begin Source File

SOURCE=.\interface\messageresponse.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messageresponse.h
# End Source File
# Begin Source File

SOURCE=.\interface\messagewin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messagewin.h
# End Source File
# Begin Source File

SOURCE=.\interface\messagewindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\messagewindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\objectpad.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\objectpad.h
# End Source File
# Begin Source File

SOURCE=.\interface\playerpad.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\playerpad.h
# End Source File
# Begin Source File

SOURCE=.\interface\pollutionpad.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\pollutionpad.h
# End Source File
# Begin Source File

SOURCE=.\interface\queuewin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\queuewin.h
# End Source File
# Begin Source File

SOURCE=.\interface\queuewindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\queuewindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\radarwin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\radarwin.h
# End Source File
# Begin Source File

SOURCE=.\interface\splash.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\splash.h
# End Source File
# Begin Source File

SOURCE=.\interface\statswindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\statswindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\test.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\test.h
# End Source File
# Begin Source File

SOURCE=.\interface\spNewGameScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\spnewgamewindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\spnewgamewindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\spscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\spwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\spwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\statswindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\statswindow.h
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\thermometer.cpp
# End Source File
# Begin Source File

SOURCE=.\aui_civ3\thermometer.h
# End Source File
# Begin Source File

SOURCE=.\interface\tilepad.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\tilepad.h
# End Source File
# Begin Source File

SOURCE=.\interface\toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\toolbar.h
# End Source File
# Begin Source File

SOURCE=.\interface\tradewin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\tradewin.h
# End Source File
# Begin Source File

SOURCE=.\interface\tradewindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\tradewindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\turnwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\turnwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\unitpad.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\unitpad.h
# End Source File
# Begin Source File

SOURCE=.\interface\unitpadwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\unitpadwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\unitstatuswin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\unitstatuswin.h
# End Source File
# Begin Source File

SOURCE=.\interface\unitstatuswindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\unitstatuswindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\workwin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\workwin.h
# End Source File
# Begin Source File

SOURCE=.\interface\workwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\workwindow.h
# End Source File
# End Group
# End Target
# End Project
