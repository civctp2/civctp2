#
# $Id$
#

include $(ctp2_code)/os/autoconf/Makefile.common

noinst_LTLIBRARIES = libuiinterface.la

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

libuiinterface_la_SOURCES = \
	CityControlPanel.cpp \
	DomesticControlPanel.cpp \
	MessageControlPanel.cpp \
	TilesControlPanel.cpp \
	UnitControlPanel.cpp \
	ControlTabPanel.cpp \
	EndTurnButton.cpp \
	MainControlPanel.cpp \
	ShortcutPad.cpp \
	StatusBar.cpp \
	TurnYearStatus.cpp \
	ZoomPad.cpp \
	controlpanelwindow.cpp \
	interfaceevent.cpp \
	agesscreen.cpp \
	ancientwindows.cpp \
	armymanagerwindow.cpp \
	AttractWindow.cpp \
	backgroundwin.cpp \
	c3dialogs.cpp \
	chatbox.cpp \
	CityEspionage.cpp \
	citymanager.cpp \
	creditsscreen.cpp \
	cursormanager.cpp \
	custommapscreen.cpp \
	debugwindow.cpp \
	DiplomacyDetails.cpp \
	diplomacywindow.cpp \
	dipwizard.cpp \
	EditQueue.cpp \
	FileDialog.cpp \
	gameplayoptions.cpp \
	graphicsresscreen.cpp \
	graphicsscreen.cpp \
	greatlibrary.cpp \
	greatlibrary_util.cpp \
	greatlibrarywindow.cpp \
	helptile.cpp \
	hotseatlist.cpp \
	infowin.cpp \
	initialplayscreen.cpp \
	initialplaywindow.cpp \
	intelligencewindow.cpp \
	IntroMovieWin.cpp \
	IntroMovieWindow.cpp \
	km_screen.cpp \
	loadsavemapscreen.cpp \
	loadsavemapwindow.cpp \
	loadsavescreen.cpp \
	loadsavewindow.cpp \
	MapCopyBuffer.cpp \
	messageactions.cpp \
	messageadvice.cpp \
	messageeyepoint.cpp \
	messageiconbutton.cpp \
	messageiconwindow.cpp \
	messagelist.cpp \
	messagemodal.cpp \
	messageresponse.cpp \
	messagewin.cpp \
	messagewindow.cpp \
	musicscreen.cpp \
	musictrackscreen.cpp \
	optionsscreen.cpp \
	optionswindow.cpp \
	optionwarningscreen.cpp \
	ProfileEdit.cpp \
	progresswindow.cpp \
	radarwindow.cpp \
	scenarioeditor.cpp \
	scenarioscreen.cpp \
	scenariowindow.cpp \
	sciencewin.cpp \
	scorewarn.cpp \
	screenutils.cpp \
	soundscreen.cpp \
	splash.cpp \
	spnewgamediffscreen.cpp \
	spnewgamemapshapescreen.cpp \
	spnewgamemapsizescreen.cpp \
	spnewgameplayersscreen.cpp \
	spnewgamerandomcustomscreen.cpp \
	spnewgamerulesscreen.cpp \
	spnewgamescreen.cpp \
	spnewgametribescreen.cpp \
	spnewgamewindow.cpp \
	spriteeditor.cpp \
	spscreen.cpp \
	spwindow.cpp \
	statictextbox.cpp \
	statswindow.cpp \
	String_Search.cpp \
	tileimptracker.cpp \
	trademanager.cpp \
	tutorialwin.cpp \
	UIUtils.cpp \
	unitmanager.cpp \
	victorymoviewin.cpp \
	victorymoviewindow.cpp \
	victorywin.cpp \
	victorywindow.cpp \
	wondermoviewin.cpp \
	wondermoviewindow.cpp \
	WonderTab.cpp \
	workwin.cpp \
	workwindow.cpp \
	InterfaceEventRegister.cpp \
	CauseAndEffectTab.cpp \
	DomesticManagementDialog.cpp \
	GovernmentTab.cpp \
	citywindow.cpp \
	sci_advancescreen.cpp \
	ScienceManagementDialog.cpp \
	NationalManagementDialog.cpp \
	MessageBoxDialog.cpp \
	battle.cpp \
	battleevent.cpp \
	battleview.cpp \
	battleviewwindow.cpp \
	infowindow.cpp \
	rankingtab.cpp \
	scoretab.cpp \
	timelinetab.cpp \
	sciencevictorydialog.cpp

libuiinterface_la_LDFLAGS = -static

AM_CFLAGS=\
	$(CTP2_CFLAGS)

AM_CXXFLAGS=\
	$(CTP2_CXXFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

