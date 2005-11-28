#
# $Id$
#

SUBDIRS = \
	os \
	mapgen \
	gs \
	gfx \
	libs \
	sound

bin_PROGRAMS = ctp2
# This hack shortens include path list,
# but prohibits using a separate build dir
#ctp2_code=..

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
else
CTP2_LIBANET_LDADD = \
	libs/anet/src/linux/dp/libdp2.la
# These are dp2 compiled with different compiler flags
#	$(ctp2_code)/libs/anet/linux86/lib/dp2d.a \
#	$(ctp2_code)/libs/anet/linux86/lib/dp2p.a \
#	$(ctp2_code)/libs/anet/linux86/lib/dp2l.a
CTP2_LIBOSNOWIN32_LDADD = \
	os/nowin32/libosnowin32.la
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

include $(ctp2_code)/os/autoconf/Makefile.common

ctp2_LDADD = \
	gfx/gfx_utils/libgfxgfx_utils.la \
	gfx/layers/libgfxlayers.la \
	gfx/spritesys/libgfxspritesys.la \
	gfx/tilesys/libgfxtilesys.la \
	gs/database/libgsdatabase.la \
	gs/events/libgsevents.la \
	gs/fileio/libgsfileio.la \
	gs/gameobj/libgsgameobj.la \
	gs/newdb/libgsnewdb.a \
	gs/outcom/libgsoutcom.la \
	gs/slic/libgsslic.la \
	gs/utility/libgsutility.la \
	gs/world/libgsworld.la \
	sound/libsound.la \
	$(CTP2_LIBANET_LDADD) \
	$(CTP2_LIBOSNOWIN32_LDADD)

ctp2_LDFLAGS = \
	$(CTP2_LDFLAGS)

ctp2_SOURCES = \
	ctp/ctp2_utils/appstrings.cpp \
	ctp/ctp2_utils/c3cmdline.cpp \
	ctp/ctp2_utils/c3cpu.cpp \
	ctp/ctp2_utils/c3debug.cpp \
	ctp/ctp2_utils/c3errors.cpp \
	ctp/ctp2_utils/c3files.cpp \
	ctp/ctp2_utils/c3mem.cpp \
	ctp/ctp2_utils/cheatkey.cpp \
	ctp/ctp2_utils/ctpregistry.cpp \
	ctp/ctp2_utils/ddhelp.cpp \
	ctp/ctp2_utils/netconsole.cpp \
	ctp/ctp2_utils/tracklen.cpp \
	ctp/debugtools/debug.c \
	ctp/debugtools/debugassert.cpp \
	ctp/debugtools/debugcallstack.cpp \
	ctp/debugtools/debugexception.cpp \
	ctp/debugtools/debugmemory.cpp \
	ctp/debugtools/log.cpp \
	ctp/fingerprint/ctp_finger.cpp \
	ctp/fingerprint/endecode.c \
	ctp/fingerprint/shroud.c \
	ctp/fingerprint/verify.c \
	ctp/c3.cpp \
	ctp/civ3_main.cpp \
	ctp/civapp.cpp \
	ctp/display.cpp \
	gfx/layers/citylayer.cpp \
	gfx/spritesys/directorevent.cpp \
	net/general/net_achievement.cpp \
	net/general/net_action.cpp \
	net/general/net_agreement.cpp \
	net/general/net_army.cpp \
	net/general/net_cell.cpp \
	net/general/net_chat.cpp \
	net/general/net_cheat.cpp \
	net/general/net_city.cpp \
	net/general/net_city.h \
	net/general/net_civ.cpp \
	net/general/net_crc.cpp \
	net/general/net_diff.cpp \
	net/general/net_diplomacy.cpp \
	net/general/net_endgame.cpp \
	net/general/net_exclusions.cpp \
	net/general/net_feat.cpp \
	net/general/net_gameobj.cpp \
	net/general/net_gamesettings.cpp \
	net/general/net_guid.cpp \
	net/general/net_happy.cpp \
	net/general/net_info.cpp \
	net/general/net_installation.cpp \
	net/general/net_keys.cpp \
	net/general/net_message.cpp \
	net/general/net_order.cpp \
	net/general/net_player.cpp \
	net/general/net_pollution.cpp \
	net/general/net_rand.cpp \
	net/general/net_ready.cpp \
	net/general/net_report.cpp \
	net/general/net_research.cpp \
	net/general/net_strengths.cpp \
	net/general/net_terrain.cpp \
	net/general/net_tradeoffer.cpp \
	net/general/net_traderoute.cpp \
	net/general/net_unit.cpp \
	net/general/net_vision.cpp \
	net/general/net_wonder.cpp \
	net/general/net_world.cpp \
	net/general/network.cpp \
	net/general/networkevent.cpp \
	net/io/net_anet.cpp \
	net/io/net_debug.cpp \
	net/io/net_io.cpp \
	net/io/net_thread.cpp \
	robot/utility/RandTest.cpp \
	robot/utility/roboinit.cpp \
	robot/pathing/A_Star_Heuristic_Cost.cpp \
	robot/pathing/astar.cpp \
	robot/pathing/AstarPnt.cpp \
	robot/pathing/AVLHeap.cpp \
	robot/pathing/CityAstar.cpp \
	robot/pathing/Path.cpp \
	robot/pathing/RobotAstar.cpp \
	robot/pathing/robotastar2.cpp \
	robot/pathing/TradeAstar.cpp \
	robot/pathing/unitastar.cpp \
	robot/aibackdoor/bset.cpp \
	robot/aibackdoor/civarchive.cpp \
	ui/aui_common/aui_action.cpp \
	ui/aui_common/aui_animate.cpp \
	ui/aui_common/aui_audiomanager.cpp \
	ui/aui_common/aui_base.cpp \
	ui/aui_common/aui_bitmapfont.cpp \
	ui/aui_common/aui_blitter.cpp \
	ui/aui_common/aui_button.cpp \
	ui/aui_common/aui_control.cpp \
	ui/aui_common/aui_cursor.cpp \
	ui/aui_common/aui_dimension.cpp \
	ui/aui_common/aui_dirtylist.cpp \
	ui/aui_common/aui_dragdropwindow.cpp \
	ui/aui_common/aui_dropdown.cpp \
	ui/aui_common/aui_font.cpp \
	ui/aui_common/aui_gamespecific.cpp \
	ui/aui_common/aui_header.cpp \
	ui/aui_common/aui_hypertextbase.cpp \
	ui/aui_common/aui_hypertextbox.cpp \
	ui/aui_common/aui_image.cpp \
	ui/aui_common/aui_imagebase.cpp \
	ui/aui_common/aui_imagelist.cpp \
	ui/aui_common/aui_input.cpp \
	ui/aui_common/aui_item.cpp \
	ui/aui_common/aui_joystick.cpp \
	ui/aui_common/aui_keyboard.cpp \
	ui/aui_common/aui_ldl.cpp \
	ui/aui_common/aui_listbox.cpp \
	ui/aui_common/aui_memmap.cpp \
	ui/aui_common/aui_mouse.cpp \
	ui/aui_common/aui_movie.cpp \
	ui/aui_common/aui_moviebutton.cpp \
	ui/aui_common/aui_moviemanager.cpp \
	ui/aui_common/aui_music.cpp \
	ui/aui_common/aui_pixel.cpp \
	ui/aui_common/aui_progressbar.cpp \
	ui/aui_common/aui_radio.cpp \
	ui/aui_common/aui_ranger.cpp \
	ui/aui_common/aui_rectangle.cpp \
	ui/aui_common/aui_region.cpp \
	ui/aui_common/aui_regiondispatch.cpp \
	ui/aui_common/aui_screen.cpp \
	ui/aui_common/aui_shell.cpp \
	ui/aui_common/aui_sound.cpp \
	ui/aui_common/aui_soundbase.cpp \
	ui/aui_common/aui_static.cpp \
	ui/aui_common/aui_stringtable.cpp \
	ui/aui_common/aui_surface.cpp \
	ui/aui_common/aui_switch.cpp \
	ui/aui_common/aui_switchgroup.cpp \
	ui/aui_common/aui_tab.cpp \
	ui/aui_common/aui_tabgroup.cpp \
	ui/aui_common/aui_textbase.cpp \
	ui/aui_common/aui_textbox.cpp \
	ui/aui_common/aui_textfield.cpp \
	ui/aui_common/aui_thumb.cpp \
	ui/aui_common/aui_tipwindow.cpp \
	ui/aui_common/aui_ui.cpp \
	ui/aui_common/aui_undo.cpp \
	ui/aui_common/aui_uniqueid.cpp \
	ui/aui_common/aui_win.cpp \
	ui/aui_common/aui_window.cpp \
	ui/aui_common/tech_memmap.cpp \
	ui/aui_common/aui_Factory.cpp \
	ui/aui_utils/primitives.cpp \
	ui/aui_utils/textutils.cpp \
	ui/aui_utils/vidplay.cpp \
	ui/aui_directx/aui_directaudiomanager.cpp \
	ui/aui_directx/aui_directblitter.cpp \
	ui/aui_directx/aui_directinput.cpp \
	ui/aui_directx/aui_directjoystick.cpp \
	ui/aui_directx/aui_directkeyboard.cpp \
	ui/aui_directx/aui_directmouse.cpp \
	ui/aui_directx/aui_directmovie.cpp \
	ui/aui_directx/aui_directmoviemanager.cpp \
	ui/aui_directx/aui_directsound.cpp \
	ui/aui_directx/aui_directsurface.cpp \
	ui/aui_directx/aui_directui.cpp \
	ui/aui_directx/aui_directx.cpp \
	ui/aui_sdl/aui_sdl.cpp \
	ui/aui_sdl/aui_sdlblitter.cpp \
	ui/aui_sdl/aui_sdlinput.cpp \
	ui/aui_sdl/aui_sdlinput.h \
	ui/aui_sdl/aui_sdljoystick.cpp \
	ui/aui_sdl/aui_sdlkeyboard.cpp \
	ui/aui_sdl/aui_sdlmouse.cpp \
	ui/aui_sdl/aui_sdlsound.cpp \
	ui/aui_sdl/aui_sdlsurface.cpp \
	ui/aui_sdl/aui_sdlui.cpp \
	ui/aui_ctp2/background.cpp \
	ui/aui_ctp2/battleorderbox.cpp \
	ui/aui_ctp2/battleorderboxactions.cpp \
	ui/aui_ctp2/bevellesswindow.cpp \
	ui/aui_ctp2/buttonbank.cpp \
	ui/aui_ctp2/c3_button.cpp \
	ui/aui_ctp2/c3_checkbox.cpp \
	ui/aui_ctp2/c3_coloredstatic.cpp \
	ui/aui_ctp2/c3_coloredswitch.cpp \
	ui/aui_ctp2/c3_coloriconbutton.cpp \
	ui/aui_ctp2/c3_dropdown.cpp \
	ui/aui_ctp2/c3_fancywindow.cpp \
	ui/aui_ctp2/c3_header.cpp \
	ui/aui_ctp2/c3_headerswitch.cpp \
	ui/aui_ctp2/c3_hypertextbox.cpp \
	ui/aui_ctp2/c3_hypertipwindow.cpp \
	ui/aui_ctp2/c3_icon.cpp \
	ui/aui_ctp2/c3_image.cpp \
	ui/aui_ctp2/c3_listbox.cpp \
	ui/aui_ctp2/c3_listitem.cpp \
	ui/aui_ctp2/c3_popupwindow.cpp \
	ui/aui_ctp2/c3_radio.cpp \
	ui/aui_ctp2/c3_ranger.cpp \
	ui/aui_ctp2/c3_slidometer.cpp \
	ui/aui_ctp2/c3_static.cpp \
	ui/aui_ctp2/c3_switch.cpp \
	ui/aui_ctp2/c3_thumb.cpp \
	ui/aui_ctp2/c3_updateaction.cpp \
	ui/aui_ctp2/c3_utilitydialogbox.cpp \
	ui/aui_ctp2/c3blitter.cpp \
	ui/aui_ctp2/c3dropdown.cpp \
	ui/aui_ctp2/c3fancywindow.cpp \
	ui/aui_ctp2/c3imageformats.cpp \
	ui/aui_ctp2/c3listbox.cpp \
	ui/aui_ctp2/c3memmap.cpp \
	ui/aui_ctp2/c3scroller.cpp \
	ui/aui_ctp2/C3slider.cpp \
	ui/aui_ctp2/c3spinner.cpp \
	ui/aui_ctp2/c3textfield.cpp \
	ui/aui_ctp2/c3thumb.cpp \
	ui/aui_ctp2/c3ui.cpp \
	ui/aui_ctp2/c3window.cpp \
	ui/aui_ctp2/c3windows.cpp \
	ui/aui_ctp2/chart.cpp \
	ui/aui_ctp2/checkbox.cpp \
	ui/aui_ctp2/cityinventorylistbox.cpp \
	ui/aui_ctp2/coloriconbutton.cpp \
	ui/aui_ctp2/coloriconswitch.cpp \
	ui/aui_ctp2/controlsheet.cpp \
	ui/aui_ctp2/directvideo.cpp \
	ui/aui_ctp2/grabitem.cpp \
	ui/aui_ctp2/icon.cpp \
	ui/aui_ctp2/iconbutton.cpp \
	ui/aui_ctp2/iconswitch.cpp \
	ui/aui_ctp2/icontab.cpp \
	ui/aui_ctp2/InfoBar.cpp \
	ui/aui_ctp2/iteminfo.cpp \
	ui/aui_ctp2/keymap.cpp \
	ui/aui_ctp2/keypress.cpp \
	ui/aui_ctp2/linegraph.cpp \
	ui/aui_ctp2/pattern.cpp \
	ui/aui_ctp2/patternbase.cpp \
	ui/aui_ctp2/picture.cpp \
	ui/aui_ctp2/picturebutton.cpp \
	ui/aui_ctp2/pictureswitch.cpp \
	ui/aui_ctp2/picturetab.cpp \
	ui/aui_ctp2/radarmap.cpp \
	ui/aui_ctp2/radiogroup.cpp \
	ui/aui_ctp2/SelItem.cpp \
	ui/aui_ctp2/SelItemClick.cpp \
	ui/aui_ctp2/SelItemEvent.cpp \
	ui/aui_ctp2/staticpicture.cpp \
	ui/aui_ctp2/statictextitem.cpp \
	ui/aui_ctp2/statuswindow.cpp \
	ui/aui_ctp2/textbox.cpp \
	ui/aui_ctp2/textbutton.cpp \
	ui/aui_ctp2/textradio.cpp \
	ui/aui_ctp2/textswitch.cpp \
	ui/aui_ctp2/texttab.cpp \
	ui/aui_ctp2/thermometer.cpp \
	ui/aui_ctp2/thronecontrol.cpp \
	ui/aui_ctp2/thumbnailmap.cpp \
	ui/aui_ctp2/tilecontrol.cpp \
	ui/aui_ctp2/tipwindow.cpp \
	ui/aui_ctp2/unittabbutton.cpp \
	ui/aui_ctp2/videowindow.cpp \
	ui/aui_ctp2/ctp2_button.cpp \
	ui/aui_ctp2/ctp2_commandline.cpp \
	ui/aui_ctp2/ctp2_dropdown.cpp \
	ui/aui_ctp2/ctp2_hypertextbox.cpp \
	ui/aui_ctp2/ctp2_listbox.cpp \
	ui/aui_ctp2/ctp2_listitem.cpp \
	ui/aui_ctp2/ctp2_Menu.cpp \
	ui/aui_ctp2/ctp2_menubar.cpp \
	ui/aui_ctp2/ctp2_MenuButton.cpp \
	ui/aui_ctp2/ctp2_spinner.cpp \
	ui/aui_ctp2/ctp2_Static.cpp \
	ui/aui_ctp2/ctp2_Switch.cpp \
	ui/aui_ctp2/ctp2_Tab.cpp \
	ui/aui_ctp2/ctp2_TabButton.cpp \
	ui/aui_ctp2/ctp2_TabGroup.cpp \
	ui/aui_ctp2/ctp2_textbuffer.cpp \
	ui/aui_ctp2/ctp2_textfield.cpp \
	ui/aui_ctp2/ctp2_Window.cpp \
	ui/interface/CityControlPanel.cpp \
	ui/interface/DomesticControlPanel.cpp \
	ui/interface/EndgameWindow.cpp \
	ui/interface/MessageControlPanel.cpp \
	ui/interface/TilesControlPanel.cpp \
	ui/interface/UnitControlPanel.cpp \
	ui/interface/ControlTabPanel.cpp \
	ui/interface/EndTurnButton.cpp \
	ui/interface/MainControlPanel.cpp \
	ui/interface/ShortcutPad.cpp \
	ui/interface/StatusBar.cpp \
	ui/interface/TurnYearStatus.cpp \
	ui/interface/ZoomPad.cpp \
	ui/interface/controlpanelwindow.cpp \
	ui/interface/interfaceevent.cpp \
	ui/interface/agesscreen.cpp \
	ui/interface/ancientwindows.cpp \
	ui/interface/armymanagerwindow.cpp \
	ui/interface/AttractWindow.cpp \
	ui/interface/backgroundwin.cpp \
	ui/interface/c3dialogs.cpp \
	ui/interface/chatbox.cpp \
	ui/interface/CityEspionage.cpp \
	ui/interface/citymanager.cpp \
	ui/interface/creditsscreen.cpp \
	ui/interface/cursormanager.cpp \
	ui/interface/custommapscreen.cpp \
	ui/interface/debugwindow.cpp \
	ui/interface/DiplomacyDetails.cpp \
	ui/interface/diplomacywindow.cpp \
	ui/interface/dipwizard.cpp \
	ui/interface/EditQueue.cpp \
	ui/interface/FileDialog.cpp \
	ui/interface/gameplayoptions.cpp \
	ui/interface/graphicsresscreen.cpp \
	ui/interface/graphicsscreen.cpp \
	ui/interface/greatlibrary.cpp \
	ui/interface/greatlibrary_util.cpp \
	ui/interface/greatlibrarywindow.cpp \
	ui/interface/helptile.cpp \
	ui/interface/hotseatlist.cpp \
	ui/interface/infowin.cpp \
	ui/interface/initialplayscreen.cpp \
	ui/interface/initialplaywindow.cpp \
	ui/interface/intelligencewindow.cpp \
	ui/interface/IntroMovieWin.cpp \
	ui/interface/IntroMovieWindow.cpp \
	ui/interface/km_screen.cpp \
	ui/interface/loadsavemapscreen.cpp \
	ui/interface/loadsavemapwindow.cpp \
	ui/interface/loadsavescreen.cpp \
	ui/interface/loadsavewindow.cpp \
	ui/interface/MapCopyBuffer.cpp \
	ui/interface/messageactions.cpp \
	ui/interface/messageadvice.cpp \
	ui/interface/messageeyepoint.cpp \
	ui/interface/messageiconbutton.cpp \
	ui/interface/messageiconwindow.cpp \
	ui/interface/messagelist.cpp \
	ui/interface/messagemodal.cpp \
	ui/interface/messageresponse.cpp \
	ui/interface/messagewin.cpp \
	ui/interface/messagewindow.cpp \
	ui/interface/musicscreen.cpp \
	ui/interface/musictrackscreen.cpp \
	ui/interface/optionsscreen.cpp \
	ui/interface/optionswindow.cpp \
	ui/interface/optionwarningscreen.cpp \
	ui/interface/ProfileEdit.cpp \
	ui/interface/progresswindow.cpp \
	ui/interface/radarwindow.cpp \
	ui/interface/scenarioeditor.cpp \
	ui/interface/scenarioscreen.cpp \
	ui/interface/scenariowindow.cpp \
	ui/interface/sciencewin.cpp \
	ui/interface/scorewarn.cpp \
	ui/interface/screenutils.cpp \
	ui/interface/soundscreen.cpp \
	ui/interface/splash.cpp \
	ui/interface/spnewgamediffscreen.cpp \
	ui/interface/spnewgamemapshapescreen.cpp \
	ui/interface/spnewgamemapsizescreen.cpp \
	ui/interface/spnewgameplayersscreen.cpp \
	ui/interface/spnewgamerandomcustomscreen.cpp \
	ui/interface/spnewgamerulesscreen.cpp \
	ui/interface/spnewgamescreen.cpp \
	ui/interface/spnewgametribescreen.cpp \
	ui/interface/spnewgamewindow.cpp \
	ui/interface/spriteeditor.cpp \
	ui/interface/statictextbox.cpp \
	ui/interface/statswindow.cpp \
	ui/interface/String_Search.cpp \
	ui/interface/tileimptracker.cpp \
	ui/interface/trademanager.cpp \
	ui/interface/tutorialwin.cpp \
	ui/interface/UIUtils.cpp \
	ui/interface/unitmanager.cpp \
	ui/interface/victorymoviewin.cpp \
	ui/interface/victorymoviewindow.cpp \
	ui/interface/victorywin.cpp \
	ui/interface/victorywindow.cpp \
	ui/interface/wondermoviewin.cpp \
	ui/interface/wondermoviewindow.cpp \
	ui/interface/WonderTab.cpp \
	ui/interface/workwin.cpp \
	ui/interface/workwindow.cpp \
	ui/interface/InterfaceEventRegister.cpp \
	ui/interface/CauseAndEffectTab.cpp \
	ui/interface/DomesticManagementDialog.cpp \
	ui/interface/GovernmentTab.cpp \
	ui/interface/citywindow.cpp \
	ui/interface/sci_advancescreen.cpp \
	ui/interface/ScienceManagementDialog.cpp \
	ui/interface/NationalManagementDialog.cpp \
	ui/interface/MessageBoxDialog.cpp \
	ui/interface/battle.cpp \
	ui/interface/battleevent.cpp \
	ui/interface/battleview.cpp \
	ui/interface/battleviewwindow.cpp \
	ui/interface/infowindow.cpp \
	ui/interface/rankingtab.cpp \
	ui/interface/scoretab.cpp \
	ui/interface/timelinetab.cpp \
	ui/interface/sciencevictorydialog.cpp \
	ui/ldl/ldl.tab.c \
	ui/ldl/lex.ldl.c \
	ui/ldl/ldl_attr.cpp \
	ui/ldl/ldl_data.cpp \
	ui/ldl/ldl_data_info.cpp \
	ui/ldl/ldl_file.cpp \
	ui/ldl/ldl_memmap.cpp \
	ui/ldl/ldlif.cpp \
	ui/netshell/allinonewindow.cpp \
	ui/netshell/connectionselectwindow.cpp \
	ui/netshell/dialogboxwindow.cpp \
	ui/netshell/gameselectwindow.cpp \
	ui/netshell/lobbychangewindow.cpp \
	ui/netshell/lobbywindow.cpp \
	ui/netshell/netfunc.cpp \
	ui/netshell/netshell.cpp \
	ui/netshell/netshell_game.cpp \
	ui/netshell/ns_aiplayer.cpp \
	ui/netshell/ns_aiplayersetup.cpp \
	ui/netshell/ns_chatbox.cpp \
	ui/netshell/ns_civlistbox.cpp \
	ui/netshell/ns_customlistbox.cpp \
	ui/netshell/ns_game.cpp \
	ui/netshell/ns_gamesetup.cpp \
	ui/netshell/ns_gplayer.cpp \
	ui/netshell/ns_header.cpp \
	ui/netshell/ns_headerswitch.cpp \
	ui/netshell/ns_improvements.cpp \
	ui/netshell/ns_item.cpp \
	ui/netshell/ns_lobby.cpp \
	ui/netshell/ns_player.cpp \
	ui/netshell/ns_playersetup.cpp \
	ui/netshell/ns_rplayer.cpp \
	ui/netshell/ns_server.cpp \
	ui/netshell/ns_session.cpp \
	ui/netshell/ns_spinner.cpp \
	ui/netshell/ns_string.cpp \
	ui/netshell/ns_transport.cpp \
	ui/netshell/ns_tribes.cpp \
	ui/netshell/ns_units.cpp \
	ui/netshell/ns_window.cpp \
	ui/netshell/ns_wonders.cpp \
	ui/netshell/passwordscreen.cpp \
	ui/netshell/playereditwindow.cpp \
	ui/netshell/playerselectwindow.cpp \
	ui/netshell/serverselectwindow.cpp \
	ui/slic_debug/segmentlist.cpp \
	ui/slic_debug/sourcelist.cpp \
	ui/slic_debug/watchlist.cpp \
	ai/diplomacy/counterresponseevent.cpp \
	ai/diplomacy/dstateevent.cpp \
	ai/diplomacy/motivationevent.cpp \
	ai/diplomacy/nproposalevent.cpp \
	ai/diplomacy/proposalresponseevent.cpp \
	ai/diplomacy/ReactEvent.cpp \
	ai/diplomacy/regardevent.cpp \
	ai/diplomacy/RejectResponseEvent.cpp \
	ai/diplomacy/ResponseEvent.cpp \
	ai/diplomacy/sstateevent.cpp \
	ai/diplomacy/threatresponseevent.cpp \
	ai/diplomacy/agreementmatrix.cpp \
	ai/diplomacy/diplomacyutil.cpp \
	ai/diplomacy/diplomat.cpp \
	ai/diplomacy/diplomattypes.cpp \
	ai/diplomacy/foreigner.cpp \
	ai/diplomacy/proposalanalysis.cpp \
	ai/strategy/scheduler/Plan.cpp \
	ai/strategy/scheduler/scheduler.cpp \
	ai/strategy/goals/ctpgoal.cpp \
	ai/strategy/goals/Goal.cpp \
	ai/strategy/squads/Squad.cpp \
	ai/strategy/squads/squad_Strength.cpp \
	ai/strategy/agents/agent.cpp \
	ai/strategy/agents/ctpagent.cpp \
	ai/mapanalysis/boundingrect.cpp \
	ai/mapanalysis/mapanalysis.cpp \
	ai/mapanalysis/settlemap.cpp \
	ai/CityManagement/governor.cpp \
	ai/ctpai.cpp \
	ai/ctpaidebug.cpp

gs/slic/lex.yysc.c: $(ctp2_code)/gs/slic/sliccmd.l gs/slic/ysc.tab.h
	$(LEX) -i -Pyysc -o$@ $<

gs/slic/lex.yysl.c: $(ctp2_code)/gs/slic/slic.l gs/slic/ysl.tab.h
	$(LEX) -i -Pyysl -o$@ $<

gs/slic/ysc.tab.c \
gs/slic/ysc.tab.h: $(ctp2_code)/gs/slic/sliccmd.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p yysc -bgs/slic/ysc $<

gs/slic/ysl.tab.c \
gs/slic/ysl.tab.h: $(ctp2_code)/gs/slic/slic.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p yysl -bgs/slic/ysl $<

ui:
	$(INSTALL) -d $@

ui/ldl: ui
	$(INSTALL) -d $@

ui/ldl/lex.ldl.c: $(ctp2_code)/ui/ldl/ldl.l ui/ldl
	$(LEX) -i -Pldl -o$@ $<

ui/ldl/ldl.tab.h: ui/ldl/ldl.tab.c ui/ldl

ui/ldl/ldl.tab.c: $(ctp2_code)/ui/ldl/ldl.y ui/ldl
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -bui/ldl/ldl $<

AM_CFLAGS=\
	$(CTP2_CFLAGS)

AM_CXXFLAGS=\
	$(CTP2_CXXFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

newdb_CDBTYPES = \
	AdvanceBranchRecord \
	AdvanceListRecord \
	AdvanceRecord \
	AgeCityStyleRecord \
	AgeRecord \
	BuildListSequenceRecord \
	BuildingBuildListRecord \
	BuildingRecord \
	CitySizeRecord \
	CityStyleRecord \
	DiplomacyProposalRecord \
	DiplomacyRecord \
	DiplomacyThreatRecord \
	EndGameObjectRecord \
	FeatRecord \
	GoalRecord \
	GovernmentRecord \
	IconRecord \
	ImprovementListRecord \
	OrderRecord \
	PersonalityRecord \
	PopRecord \
	ResourceRecord \
	SoundRecord \
	SpecialAttackInfoRecord \
	SpecialEffectRecord \
	SpriteRecord \
	StrategyRecord \
	TerrainImprovementRecord \
	TerrainRecord \
	UnitBuildListRecord \
	UnitRecord \
	WonderBuildListRecord \
	WonderMovieRecord \
	WonderRecord

NEWDB_TARGET=gs/newdb/

newdb_CDBSRCFILES = \
	$(foreach type,$(newdb_CDBTYPES),$(NEWDB_TARGET)$(type).cpp )

newdb_CDBHDRFILES = \
	$(foreach type,$(newdb_CDBTYPES),$(NEWDB_TARGET)$(type).h )

newdb_CDBSTAMPFILES = \
	$(foreach type,$(newdb_CDBTYPES),$(NEWDB_TARGET)$(type).stamp )

# Rules for generated sources

$(NEWDB_TARGET)lex.dbl.c: $(ctp2_code)/gs/newdb/DBLexer.l
	$(LEX) -i -Pdbl -o$@ $<

CTPDB=$(top_builddir)/ctp2_code/gs/dbgen/ctpdb$(EXEEXT)
$(CTPDB): gs/dbgen/Makefile
	$(MAKE) -C gs/dbgen

$(NEWDB_TARGET)AdvanceBranchRecord.cpp \
$(NEWDB_TARGET)AdvanceBranchRecord.h: $(NEWDB_TARGET)AdvanceBranchRecord.stamp

$(NEWDB_TARGET)AdvanceBranchRecord.stamp: $(ctp2_code)/gs/newdb/advance.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)AdvanceListRecord.cpp \
$(NEWDB_TARGET)AdvanceListRecord.h: $(NEWDB_TARGET)AdvanceListRecord.stamp

$(NEWDB_TARGET)AdvanceListRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		$(NEWDB_TARGET)AdvanceRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)AdvanceRecord.cpp \
$(NEWDB_TARGET)AdvanceRecord.h: $(NEWDB_TARGET)AdvanceRecord.stamp

$(NEWDB_TARGET)AdvanceRecord.stamp: $(ctp2_code)/gs/newdb/advance.cdb \
		$(NEWDB_TARGET)AgeRecord.h \
		$(NEWDB_TARGET)CityStyleRecord.h $(NEWDB_TARGET)GovernmentRecord.h \
		$(NEWDB_TARGET)FeatRecord.h $(NEWDB_TARGET)IconRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)AgeCityStyleRecord.cpp \
$(NEWDB_TARGET)AgeCityStyleRecord.h: $(NEWDB_TARGET)AgeCityStyleRecord.stamp

$(NEWDB_TARGET)AgeCityStyleRecord.stamp: $(NEWDB_TARGET)citystyle.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)AgeRecord.cpp \
$(NEWDB_TARGET)AgeRecord.h: $(NEWDB_TARGET)AgeRecord.stamp

$(NEWDB_TARGET)AgeRecord.stamp: $(ctp2_code)/gs/newdb/advance.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)BuildListSequenceRecord.cpp \
$(NEWDB_TARGET)BuildListSequenceRecord.h: $(NEWDB_TARGET)BuildListSequenceRecord.stamp

$(NEWDB_TARGET)BuildListSequenceRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)BuildingBuildListRecord.cpp \
$(NEWDB_TARGET)BuildingBuildListRecord.h: $(NEWDB_TARGET)BuildingBuildListRecord.stamp

$(NEWDB_TARGET)BuildingBuildListRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		$(NEWDB_TARGET)BuildingRecord.h \
		$(CTPDB) 
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)BuildingRecord.cpp \
$(NEWDB_TARGET)BuildingRecord.h: $(NEWDB_TARGET)BuildingRecord.stamp

$(NEWDB_TARGET)BuildingRecord.stamp: $(ctp2_code)/gs/newdb/building.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)CitySizeRecord.cpp \
$(NEWDB_TARGET)CitySizeRecord.h: $(NEWDB_TARGET)CitySizeRecord.stamp

$(NEWDB_TARGET)CitySizeRecord.stamp: $(ctp2_code)/gs/newdb/citysize.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)CityStyleRecord.cpp \
$(NEWDB_TARGET)CityStyleRecord.h: $(NEWDB_TARGET)CityStyleRecord.stamp

$(NEWDB_TARGET)CityStyleRecord.stamp: $(ctp2_code)/gs/newdb/citystyle.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)DiplomacyProposalRecord.cpp \
$(NEWDB_TARGET)DiplomacyProposalRecord.h: $(NEWDB_TARGET)DiplomacyProposalRecord.stamp

$(NEWDB_TARGET)DiplomacyProposalRecord.stamp: $(ctp2_code)/gs/newdb/dipproposal.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)DiplomacyRecord.cpp \
$(NEWDB_TARGET)DiplomacyRecord.h: $(NEWDB_TARGET)DiplomacyRecord.stamp

$(NEWDB_TARGET)DiplomacyRecord.stamp: $(ctp2_code)/gs/newdb/diplomacy.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)DiplomacyThreatRecord.cpp \
$(NEWDB_TARGET)DiplomacyThreatRecord.h: $(NEWDB_TARGET)DiplomacyThreatRecord.stamp

$(NEWDB_TARGET)DiplomacyThreatRecord.stamp: $(ctp2_code)/gs/newdb/dipthreat.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)EndGameObjectRecord.cpp \
$(NEWDB_TARGET)EndGameObjectRecord.h: $(NEWDB_TARGET)EndGameObjectRecord.stamp

$(NEWDB_TARGET)EndGameObjectRecord.stamp: $(ctp2_code)/gs/newdb/EndGameObject.cdb \
		$(NEWDB_TARGET)TerrainImprovementRecord.h \
		$(NEWDB_TARGET)WonderRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)FeatRecord.cpp \
$(NEWDB_TARGET)FeatRecord.h: $(NEWDB_TARGET)FeatRecord.stamp

$(NEWDB_TARGET)FeatRecord.stamp: $(ctp2_code)/gs/newdb/feat.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)GoalRecord.cpp \
$(NEWDB_TARGET)GoalRecord.h: $(NEWDB_TARGET)GoalRecord.stamp

$(NEWDB_TARGET)GoalRecord.stamp: $(ctp2_code)/gs/newdb/goal.cdb \
		$(NEWDB_TARGET)OrderRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)GovernmentRecord.cpp \
$(NEWDB_TARGET)GovernmentRecord.h: $(NEWDB_TARGET)GovernmentRecord.stamp

$(NEWDB_TARGET)GovernmentRecord.stamp: $(ctp2_code)/gs/newdb/government.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)IconRecord.cpp \
$(NEWDB_TARGET)IconRecord.h: $(NEWDB_TARGET)IconRecord.stamp

$(NEWDB_TARGET)IconRecord.stamp: $(ctp2_code)/gs/newdb/icon.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)ImprovementListRecord.cpp \
$(NEWDB_TARGET)ImprovementListRecord.h: $(NEWDB_TARGET)ImprovementListRecord.stamp

$(NEWDB_TARGET)ImprovementListRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		$(NEWDB_TARGET)TerrainImprovementRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)OrderRecord.cpp \
$(NEWDB_TARGET)OrderRecord.h: $(NEWDB_TARGET)OrderRecord.stamp

$(NEWDB_TARGET)OrderRecord.stamp: $(ctp2_code)/gs/newdb/order.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)PersonalityRecord.cpp \
$(NEWDB_TARGET)PersonalityRecord.h: $(NEWDB_TARGET)PersonalityRecord.stamp

$(NEWDB_TARGET)PersonalityRecord.stamp: $(ctp2_code)/gs/newdb/personality.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)PopRecord.cpp \
$(NEWDB_TARGET)PopRecord.h: $(NEWDB_TARGET)PopRecord.stamp

$(NEWDB_TARGET)PopRecord.stamp: $(ctp2_code)/gs/newdb/pop.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)ResourceRecord.cpp \
$(NEWDB_TARGET)ResourceRecord.h: $(NEWDB_TARGET)ResourceRecord.stamp

$(NEWDB_TARGET)ResourceRecord.stamp: $(ctp2_code)/gs/newdb/terrain.cdb \
		$(NEWDB_TARGET)SoundRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)SoundRecord.cpp \
$(NEWDB_TARGET)SoundRecord.h: $(NEWDB_TARGET)SoundRecord.stamp

$(NEWDB_TARGET)SoundRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)SpecialAttackInfoRecord.cpp \
$(NEWDB_TARGET)SpecialAttackInfoRecord.h: $(NEWDB_TARGET)SpecialAttackInfoRecord.stamp

$(NEWDB_TARGET)SpecialAttackInfoRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)SpecialEffectRecord.cpp \
$(NEWDB_TARGET)SpecialEffectRecord.h: $(NEWDB_TARGET)SpecialEffectRecord.stamp

$(NEWDB_TARGET)SpecialEffectRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)SpriteRecord.cpp \
$(NEWDB_TARGET)SpriteRecord.h: $(NEWDB_TARGET)SpriteRecord.stamp

$(NEWDB_TARGET)SpriteRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)StrategyRecord.cpp \
$(NEWDB_TARGET)StrategyRecord.h: $(NEWDB_TARGET)StrategyRecord.stamp

$(NEWDB_TARGET)StrategyRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		$(NEWDB_TARGET)GoalRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)TerrainImprovementRecord.cpp \
$(NEWDB_TARGET)TerrainImprovementRecord.h: $(NEWDB_TARGET)TerrainImprovementRecord.stamp

$(NEWDB_TARGET)TerrainImprovementRecord.stamp: $(ctp2_code)/gs/newdb/terrimprove.cdb \
		$(NEWDB_TARGET)TerrainRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)TerrainRecord.cpp \
$(NEWDB_TARGET)TerrainRecord.h: $(NEWDB_TARGET)TerrainRecord.stamp

$(NEWDB_TARGET)TerrainRecord.stamp: $(ctp2_code)/gs/newdb/terrain.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)UnitBuildListRecord.cpp \
$(NEWDB_TARGET)UnitBuildListRecord.h: $(NEWDB_TARGET)UnitBuildListRecord.stamp

$(NEWDB_TARGET)UnitBuildListRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)UnitRecord.cpp \
$(NEWDB_TARGET)UnitRecord.h: $(NEWDB_TARGET)UnitRecord.stamp

$(NEWDB_TARGET)UnitRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb \
		$(NEWDB_TARGET)TerrainImprovementRecord.h \
		$(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)WonderBuildListRecord.cpp \
$(NEWDB_TARGET)WonderBuildListRecord.h: $(NEWDB_TARGET)WonderBuildListRecord.stamp

$(NEWDB_TARGET)WonderBuildListRecord.stamp: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)WonderMovieRecord.cpp \
$(NEWDB_TARGET)WonderMovieRecord.h: $(NEWDB_TARGET)WonderMovieRecord.stamp

$(NEWDB_TARGET)WonderMovieRecord.stamp: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

$(NEWDB_TARGET)WonderRecord.cpp \
$(NEWDB_TARGET)WonderRecord.h: $(NEWDB_TARGET)WonderRecord.stamp

$(NEWDB_TARGET)WonderRecord.stamp: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) $(NEWDB_TARGET) < $<
	touch $@

MOSTLYCLEANFILES=\
	gs/newdb/lex.dbl.c \
	gs/slic/lex.yysc.c \
	gs/slic/lex.yysl.c \
	gs/slic/ysc.tab.c \
	gs/slic/ysc.tab.h \
	gs/slic/ysc.output \
	gs/slic/ysl.tab.c \
	gs/slic/ysl.tab.h \
	gs/slic/ysl.output \
	$(newdb_CDBSRCFILES) \
	$(newdb_CDBHDRFILES) \
	$(newdb_CDBSTAMPFILES) \
	$(foreach src,$(newdb_CDBSRCFILES),$(src).old $(src).new ) \
	$(foreach hdr,$(newdb_CDBHDRFILES),$(hdr).old $(hdr).new ) \
	$(foreach stamp,$(newdb_CDBSTAMPFILES),$(stamp).old $(stamp).new )

