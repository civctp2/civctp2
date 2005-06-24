#
# $Id$
#

include $(ctp2_code)/os/autoconf/Makefile.common

noinst_LTLIBRARIES = libgsgameobj.la

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

libgsgameobj_la_SOURCES = \
	AchievementTracker.cpp \
	Advances.cpp \
	advanceutil.cpp \
	Agreement.cpp \
	AgreementData.cpp \
	AgreementPool.cpp \
	Army.cpp \
	ArmyData.cpp \
	armyevent.cpp \
	ArmyPool.cpp \
	barbarians.cpp \
	bldque.cpp \
	buildingutil.cpp \
	CityData.cpp \
	CityEvent.cpp \
	CityInfluenceIterator.cpp \
	CityRadius.cpp \
	Civilisation.cpp \
	CivilisationData.cpp \
	civilisationpool.cpp \
	combatevent.cpp \
	CriticalMessagesPrefs.cpp \
	CTP2Combat.cpp \
	Diffcly.cpp \
	Diplomacy_Log.cpp \
	DiplomaticRequest.cpp \
	DiplomaticRequestData.cpp \
	DiplomaticRequestPool.cpp \
	EmptyTile.cpp \
	endgame.cpp \
	Events.cpp \
	EventTracker.cpp \
	Exclusions.cpp \
	FeatTracker.cpp \
	gaiacontroller.cpp \
	GameObj.cpp \
	GameSettings.cpp \
	Gold.cpp \
	GoodyHuts.cpp \
	GSLogs.cpp \
	Happy.cpp \
	happytracker.cpp \
	ID.cpp \
	improvementevent.cpp \
	installation.cpp \
	installationdata.cpp \
	installationpool.cpp \
	MaterialPool.cpp \
	Message.cpp \
	messagedata.cpp \
	MessagePool.cpp \
	ObjPool.cpp \
	Order.cpp \
	Player.cpp \
	PlayerAI.cpp \
	PlayerEvent.cpp \
	PlayerTurn.cpp \
	PlayHap.cpp \
	Pollution.cpp \
	Readiness.cpp \
	Regard.cpp \
	Resources.cpp \
	Sci.cpp \
	Score.cpp \
	Strengths.cpp \
	TaxRate.cpp \
	terrainutil.cpp \
	TerrImprove.cpp \
	TerrImproveData.cpp \
	TerrImprovePool.cpp \
	TopTen.cpp \
	TradeBids.cpp \
	tradeevent.cpp \
	TradeOffer.cpp \
	TradeOfferData.cpp \
	TradeOfferPool.cpp \
	TradePool.cpp \
	TradeRoute.cpp \
	TradeRouteData.cpp \
	tradeutil.cpp \
	Unit.cpp \
	UnitData.cpp \
	unitevent.cpp \
	UnitPool.cpp \
	unitutil.cpp \
	Vision.cpp \
	WonderTracker.cpp \
	wonderutil.cpp \
	Wormhole.cpp \
	XY_Coordinates.cpp

libgsgameobj_la_LDFLAGS = -static

AM_CFLAGS=\
	$(CTP2_CFLAGS)

AM_CXXFLAGS=\
	$(CTP2_CXXFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

