#
# $Id$
#

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
CTP2_NOWIN32_INC=\
	-I$(ctp2_code)/os/nowin32
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

newdb_CDBSRCFILES = \
	../gs/newdb/AdvanceBranchRecord.cpp \
	../gs/newdb/AdvanceListRecord.cpp \
	../gs/newdb/AdvanceRecord.cpp \
	../gs/newdb/AgeCityStyleRecord.cpp \
	../gs/newdb/AgeRecord.cpp \
	../gs/newdb/BuildListSequenceRecord.cpp \
	../gs/newdb/BuildingBuildListRecord.cpp \
	../gs/newdb/BuildingRecord.cpp \
	../gs/newdb/CitySizeRecord.cpp \
	../gs/newdb/CityStyleRecord.cpp \
	../gs/newdb/DiplomacyProposalRecord.cpp \
	../gs/newdb/DiplomacyRecord.cpp \
	../gs/newdb/DiplomacyThreatRecord.cpp \
	../gs/newdb/EndGameObjectRecord.cpp \
	../gs/newdb/FeatRecord.cpp \
	../gs/newdb/GoalRecord.cpp \
	../gs/newdb/GovernmentRecord.cpp \
	../gs/newdb/IconRecord.cpp \
	../gs/newdb/ImprovementListRecord.cpp \
	../gs/newdb/OrderRecord.cpp \
	../gs/newdb/PersonalityRecord.cpp \
	../gs/newdb/PopRecord.cpp \
	../gs/newdb/ResourceRecord.cpp \
	../gs/newdb/SoundRecord.cpp \
	../gs/newdb/SpecialAttackInfoRecord.cpp \
	../gs/newdb/SpecialEffectRecord.cpp \
	../gs/newdb/SpriteRecord.cpp \
	../gs/newdb/StrategyRecord.cpp \
	../gs/newdb/TerrainImprovementRecord.cpp \
	../gs/newdb/TerrainRecord.cpp \
	../gs/newdb/UnitBuildListRecord.cpp \
	../gs/newdb/UnitRecord.cpp \
	../gs/newdb/WonderBuildListRecord.cpp \
	../gs/newdb/WonderMovieRecord.cpp \
	../gs/newdb/WonderRecord.cpp

newdb_CDBHDRFILES = \
	../gs/newdb/AdvanceBranchRecord.h \
	../gs/newdb/AdvanceListRecord.h \
	../gs/newdb/AdvanceRecord.h \
	../gs/newdb/AgeCityStyleRecord.h \
	../gs/newdb/AgeRecord.h \
	../gs/newdb/BuildListSequenceRecord.h \
	../gs/newdb/BuildingBuildListRecord.h \
	../gs/newdb/BuildingRecord.h \
	../gs/newdb/CitySizeRecord.h \
	../gs/newdb/CityStyleRecord.h \
	../gs/newdb/DiplomacyProposalRecord.h \
	../gs/newdb/DiplomacyRecord.h \
	../gs/newdb/DiplomacyThreatRecord.h \
	../gs/newdb/EndGameObjectRecord.h \
	../gs/newdb/FeatRecord.h \
	../gs/newdb/GoalRecord.h \
	../gs/newdb/GovernmentRecord.h \
	../gs/newdb/IconRecord.h \
	../gs/newdb/ImprovementListRecord.h \
	../gs/newdb/OrderRecord.h \
	../gs/newdb/PersonalityRecord.h \
	../gs/newdb/PopRecord.h \
	../gs/newdb/ResourceRecord.h \
	../gs/newdb/SoundRecord.h \
	../gs/newdb/SpecialAttackInfoRecord.h \
	../gs/newdb/SpecialEffectRecord.h \
	../gs/newdb/SpriteRecord.h \
	../gs/newdb/StrategyRecord.h \
	../gs/newdb/TerrainImprovementRecord.h \
	../gs/newdb/TerrainRecord.h \
	../gs/newdb/UnitBuildListRecord.h \
	../gs/newdb/UnitRecord.h \
	../gs/newdb/WonderBuildListRecord.h \
	../gs/newdb/WonderMovieRecord.h \
	../gs/newdb/WonderRecord.h

newdb_CDBSTAMPFILES = \
	../gs/newdb/AdvanceBranchRecord.stamp \
	../gs/newdb/AdvanceListRecord.stamp \
	../gs/newdb/AdvanceRecord.stamp \
	../gs/newdb/AgeCityStyleRecord.stamp \
	../gs/newdb/AgeRecord.stamp \
	../gs/newdb/BuildListSequenceRecord.stamp \
	../gs/newdb/BuildingBuildListRecord.stamp \
	../gs/newdb/BuildingRecord.stamp \
	../gs/newdb/CitySizeRecord.stamp \
	../gs/newdb/CityStyleRecord.stamp \
	../gs/newdb/DiplomacyProposalRecord.stamp \
	../gs/newdb/DiplomacyRecord.stamp \
	../gs/newdb/DiplomacyThreatRecord.stamp \
	../gs/newdb/EndGameObjectRecord.stamp \
	../gs/newdb/FeatRecord.stamp \
	../gs/newdb/GoalRecord.stamp \
	../gs/newdb/GovernmentRecord.stamp \
	../gs/newdb/IconRecord.stamp \
	../gs/newdb/ImprovementListRecord.stamp \
	../gs/newdb/OrderRecord.stamp \
	../gs/newdb/PersonalityRecord.stamp \
	../gs/newdb/PopRecord.stamp \
	../gs/newdb/ResourceRecord.stamp \
	../gs/newdb/SoundRecord.stamp \
	../gs/newdb/SpecialAttackInfoRecord.stamp \
	../gs/newdb/SpecialEffectRecord.stamp \
	../gs/newdb/SpriteRecord.stamp \
	../gs/newdb/StrategyRecord.stamp \
	../gs/newdb/TerrainImprovementRecord.stamp \
	../gs/newdb/TerrainRecord.stamp \
	../gs/newdb/UnitBuildListRecord.stamp \
	../gs/newdb/UnitRecord.stamp \
	../gs/newdb/WonderBuildListRecord.stamp \
	../gs/newdb/WonderMovieRecord.stamp \
	../gs/newdb/WonderRecord.stamp

ctp2_SOURCES = \
	ctp2_utils/appstrings.cpp \
	ctp2_utils/c3cmdline.cpp \
	ctp2_utils/c3cpu.cpp \
	ctp2_utils/c3debug.cpp \
	ctp2_utils/c3errors.cpp \
	ctp2_utils/c3files.cpp \
	ctp2_utils/c3mem.cpp \
	ctp2_utils/cheatkey.cpp \
	ctp2_utils/ctpregistry.cpp \
	ctp2_utils/ddhelp.cpp \
	ctp2_utils/netconsole.cpp \
	ctp2_utils/tracklen.cpp \
	debugtools/debug.c \
	debugtools/debugassert.cpp \
	debugtools/debugcallstack.cpp \
	debugtools/debugexception.cpp \
	debugtools/debugmemory.cpp \
	debugtools/log.cpp \
	fingerprint/ctp_finger.cpp \
	fingerprint/endecode.c \
	fingerprint/shroud.c \
	fingerprint/verify.c \
	$(newdb_CDBSRCFILES) \
	../gs/newdb/lex.dbl.c \
	../gs/newdb/CTPDatabase.cpp \
	../gs/newdb/CTPRecord.cpp \
	../gs/newdb/DBLexer.cpp \
	../gs/newdb/DBLexer.cpp \
	c3.cpp \
	civ3_main.cpp \
	civapp.cpp \
	display.cpp \
	../gfx/spritesys/action.cpp \
	../gfx/spritesys/Actor.cpp \
	../gfx/spritesys/ActorPath.cpp \
	../gfx/spritesys/Anim.cpp \
	../gfx/spritesys/battleviewactor.cpp \
	../gfx/spritesys/EffectActor.cpp \
	../gfx/spritesys/goodactor.cpp \
	../gfx/spritesys/TradeActor.cpp \
	../gfx/spritesys/UnitActor.cpp \
	../gfx/spritesys/workeractor.cpp \
	../gfx/spritesys/CitySpriteGroup.cpp \
	../gfx/spritesys/effectspritegroup.cpp \
	../gfx/spritesys/FacedSprite.cpp \
	../gfx/spritesys/FacedSpriteWshadow.cpp \
	../gfx/spritesys/goodspritegroup.cpp \
	../gfx/spritesys/Sprite.cpp \
	../gfx/spritesys/spritefile.cpp \
	../gfx/spritesys/SpriteGroup.cpp \
	../gfx/spritesys/SpriteGroupList.cpp \
	../gfx/spritesys/SpriteHitTest.cpp \
	../gfx/spritesys/SpriteList.cpp \
	../gfx/spritesys/SpriteLow555.cpp \
	../gfx/spritesys/SpriteLow565.cpp \
	../gfx/spritesys/SpriteState.cpp \
	../gfx/spritesys/SpriteStateDB.cpp \
	../gfx/spritesys/spriteutils.cpp \
	../gfx/spritesys/UnitSpriteGroup.cpp \
	../gfx/spritesys/director.cpp \
	../gfx/spritesys/directorevent.cpp \
	../gfx/spritesys/screenmanager.cpp \
	../gfx/tilesys/maputils.cpp \
	../gfx/tilesys/BaseTile.cpp \
	../gfx/tilesys/resourcemap.cpp \
	../gfx/tilesys/tiledmap.cpp \
	../gfx/tilesys/tiledraw.cpp \
	../gfx/tilesys/tiledrawroad.cpp \
	../gfx/tilesys/TileHighlight.cpp \
	../gfx/tilesys/TileInfo.cpp \
	../gfx/tilesys/tileset.cpp \
	../gfx/tilesys/tileutils.cpp \
	../gfx/gfx_utils/arproces.cpp \
	../gfx/gfx_utils/ColorSet.cpp \
	../gfx/gfx_utils/gfx_options.cpp \
	../gfx/gfx_utils/pixelutils.cpp \
	../gfx/gfx_utils/targautils.cpp \
	../gfx/gfx_utils/tiffutils.cpp \
	../gfx/gfx_utils/videoutils.cpp \
	../gfx/layers/citylayer.cpp \
	../gs/database/CivilisationDB.cpp \
	../gs/database/CivilisationRec.cpp \
	../gs/database/conceptdb.cpp \
	../gs/database/ConstDB.cpp \
	../gs/database/DB.cpp \
	../gs/database/DiffDB.cpp \
	../gs/database/EndGameDB.cpp \
	../gs/database/EndGameRecord.cpp \
	../gs/database/filenamedb.cpp \
	../gs/database/gwdb.cpp \
	../gs/database/GWRecord.cpp \
	../gs/database/highscoredb.cpp \
	../gs/database/InstDB.cpp \
	../gs/database/InstRec.cpp \
	../gs/database/MapDB.cpp \
	../gs/database/MapRecord.cpp \
	../gs/database/moviedb.cpp \
	../gs/database/orderdb.cpp \
	../gs/database/PlayListDB.cpp \
	../gs/database/pollutiondb.cpp \
	../gs/database/PollutionRecord.cpp \
	../gs/database/profileDB.cpp \
	../gs/database/Rec.cpp \
	../gs/database/RiskDB.cpp \
	../gs/database/StrDB.cpp \
	../gs/database/StrRec.cpp \
	../gs/database/thronedb.cpp \
	../gs/database/UVDB.cpp \
	../gs/database/UVRecord.cpp \
	../gs/gameobj/AchievementTracker.cpp \
	../gs/gameobj/Advances.cpp \
	../gs/gameobj/advanceutil.cpp \
	../gs/gameobj/Agreement.cpp \
	../gs/gameobj/AgreementData.cpp \
	../gs/gameobj/AgreementPool.cpp \
	../gs/gameobj/Army.cpp \
	../gs/gameobj/ArmyData.cpp \
	../gs/gameobj/armyevent.cpp \
	../gs/gameobj/ArmyPool.cpp \
	../gs/gameobj/barbarians.cpp \
	../gs/gameobj/bldque.cpp \
	../gs/gameobj/buildingutil.cpp \
	../gs/gameobj/CityData.cpp \
	../gs/gameobj/CityEvent.cpp \
	../gs/gameobj/CityInfluenceIterator.cpp \
	../gs/gameobj/CityRadius.cpp \
	../gs/gameobj/Civilisation.cpp \
	../gs/gameobj/CivilisationData.cpp \
	../gs/gameobj/civilisationpool.cpp \
	../gs/gameobj/combatevent.cpp \
	../gs/gameobj/CriticalMessagesPrefs.cpp \
	../gs/gameobj/CTP2Combat.cpp \
	../gs/gameobj/Diffcly.cpp \
	../gs/gameobj/Diplomacy_Log.cpp \
	../gs/gameobj/DiplomaticRequest.cpp \
	../gs/gameobj/DiplomaticRequestData.cpp \
	../gs/gameobj/DiplomaticRequestPool.cpp \
	../gs/gameobj/EmptyTile.cpp \
	../gs/gameobj/endgame.cpp \
	../gs/gameobj/Events.cpp \
	../gs/gameobj/EventTracker.cpp \
	../gs/gameobj/Exclusions.cpp \
	../gs/gameobj/FeatTracker.cpp \
	../gs/gameobj/gaiacontroller.cpp \
	../gs/gameobj/GameObj.cpp \
	../gs/gameobj/GameSettings.cpp \
	../gs/gameobj/Gold.cpp \
	../gs/gameobj/GoodyHuts.cpp \
	../gs/gameobj/GSLogs.cpp \
	../gs/gameobj/Happy.cpp \
	../gs/gameobj/happytracker.cpp \
	../gs/gameobj/ID.cpp \
	../gs/gameobj/improvementevent.cpp \
	../gs/gameobj/installation.cpp \
	../gs/gameobj/installationdata.cpp \
	../gs/gameobj/installationpool.cpp \
	../gs/gameobj/MaterialPool.cpp \
	../gs/gameobj/Message.cpp \
	../gs/gameobj/messagedata.cpp \
	../gs/gameobj/MessagePool.cpp \
	../gs/gameobj/ObjPool.cpp \
	../gs/gameobj/Order.cpp \
	../gs/gameobj/Player.cpp \
	../gs/gameobj/PlayerAI.cpp \
	../gs/gameobj/PlayerEvent.cpp \
	../gs/gameobj/PlayerTurn.cpp \
	../gs/gameobj/PlayHap.cpp \
	../gs/gameobj/Pollution.cpp \
	../gs/gameobj/Readiness.cpp \
	../gs/gameobj/Regard.cpp \
	../gs/gameobj/Resources.cpp \
	../gs/gameobj/Sci.cpp \
	../gs/gameobj/Score.cpp \
	../gs/gameobj/Strengths.cpp \
	../gs/gameobj/TaxRate.cpp \
	../gs/gameobj/terrainutil.cpp \
	../gs/gameobj/TerrImprove.cpp \
	../gs/gameobj/TerrImproveData.cpp \
	../gs/gameobj/TerrImprovePool.cpp \
	../gs/gameobj/TopTen.cpp \
	../gs/gameobj/TradeBids.cpp \
	../gs/gameobj/tradeevent.cpp \
	../gs/gameobj/TradeOffer.cpp \
	../gs/gameobj/TradeOfferData.cpp \
	../gs/gameobj/TradeOfferPool.cpp \
	../gs/gameobj/TradePool.cpp \
	../gs/gameobj/TradeRoute.cpp \
	../gs/gameobj/TradeRouteData.cpp \
	../gs/gameobj/tradeutil.cpp \
	../gs/gameobj/Unit.cpp \
	../gs/gameobj/UnitData.cpp \
	../gs/gameobj/unitevent.cpp \
	../gs/gameobj/UnitPool.cpp \
	../gs/gameobj/unitutil.cpp \
	../gs/gameobj/Vision.cpp \
	../gs/gameobj/WonderTracker.cpp \
	../gs/gameobj/wonderutil.cpp \
	../gs/gameobj/Wormhole.cpp \
	../gs/gameobj/XY_Coordinates.cpp \
	../gs/utility/AgreementDynArr.cpp \
	../gs/utility/checksum.cpp \
	../gs/utility/CivDynArr.cpp \
	../gs/utility/clock.cpp \
	../gs/utility/DataCheck.cpp \
	../gs/utility/DiplDynArr.cpp \
	../gs/utility/ErrMsg.cpp \
	../gs/utility/gameinit.cpp \
	../gs/utility/MapFile.cpp \
	../gs/utility/MsgDynArr.cpp \
	../gs/utility/newturncount.cpp \
	../gs/utility/RandGen.cpp \
	../gs/utility/stringutils.cpp \
	../gs/utility/TradeDynArr.cpp \
	../gs/utility/TurnCnt.cpp \
	../gs/utility/TurnCntEvent.cpp \
	../gs/utility/UnitDynArr.cpp \
	../gs/world/Cell.cpp \
	../gs/world/cellunitlist.cpp \
	../gs/world/MapPoint.cpp \
	../gs/world/UnseenCell.cpp \
	../gs/world/wldgen.cpp \
	../gs/world/WorldDistance.cpp \
	../gs/world/worldevent.cpp \
	../gs/world/WrldCity.cpp \
	../gs/world/WrldCont.cpp \
	../gs/world/WrldPoll.cpp \
	../gs/world/WrlEnv.cpp \
	../gs/world/WrlImprove.cpp \
	../gs/world/WrlUnit.cpp \
	../gs/fileio/CivPaths.cpp \
	../gs/fileio/civscenarios.cpp \
	../gs/fileio/GameFile.cpp \
	../gs/fileio/prjfile.cpp \
	../gs/fileio/Token.cpp \
	../gs/slic/slic.l \
	../gs/slic/sliccmd.y

# Rules for generated sources

../gs/newdb/lex.dbl.c: $(ctp2_code)/gs/newdb/DBLexer.l
	$(LEXCOMPILE) -i -Pdbl -o../gs/newdb/lex.dbl.c $<

CTPDB=$(top_builddir)/ctp2_code/gs/dbgen/ctpdb$(EXEEXT)

../gs/newdb/AdvanceBranchRecord.cpp \
../gs/newdb/AdvanceBranchRecord.h: $(ctp2_code)/gs/newdb/advance.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/AdvanceListRecord.cpp \
../gs/newdb/AdvanceListRecord.h: $(ctp2_code)/gs/newdb/strategy.cdb \
		../gs/newdb/AdvanceRecord.h \
		$(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/AdvanceRecord.cpp \
../gs/newdb/AdvanceRecord.h: $(ctp2_code)/gs/newdb/advance.cdb \
		../gs/newdb/AgeRecord.h \
		../gs/newdb/CityStyleRecord.h ../gs/newdb/GovernmentRecord.h \
		../gs/newdb/FeatRecord.h ../gs/newdb/IconRecord.h \
		$(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/AgeCityStyleRecord.cpp \
../gs/newdb/AgeCityStyleRecord.h: $(ctp2_code)/gs/newdb/citystyle.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/AgeRecord.cpp \
../gs/newdb/AgeRecord.h: $(ctp2_code)/gs/newdb/advance.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/BuildListSequenceRecord.cpp \
../gs/newdb/BuildListSequenceRecord.h: $(ctp2_code)/gs/newdb/strategy.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/BuildingBuildListRecord.cpp \
../gs/newdb/BuildingBuildListRecord.h: $(ctp2_code)/gs/newdb/strategy.cdb \
		../gs/newdb/BuildingRecord.h \
		$(CTPDB) 
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/BuildingRecord.cpp \
../gs/newdb/BuildingRecord.h: $(ctp2_code)/gs/newdb/building.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/CitySizeRecord.cpp \
../gs/newdb/CitySizeRecord.h: $(ctp2_code)/gs/newdb/citysize.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/CityStyleRecord.cpp \
../gs/newdb/CityStyleRecord.h: $(ctp2_code)/gs/newdb/citystyle.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/DiplomacyProposalRecord.cpp \
../gs/newdb/DiplomacyProposalRecord.h: $(ctp2_code)/gs/newdb/dipproposal.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/DiplomacyRecord.cpp \
../gs/newdb/DiplomacyRecord.h: $(ctp2_code)/gs/newdb/diplomacy.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/DiplomacyThreatRecord.cpp \
../gs/newdb/DiplomacyThreatRecord.h: $(ctp2_code)/gs/newdb/dipthreat.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/EndGameObjectRecord.cpp \
../gs/newdb/EndGameObjectRecord.h: $(ctp2_code)/gs/newdb/EndGameObject.cdb \
		../gs/newdb/TerrainImprovementRecord.h \
		$(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/FeatRecord.cpp \
../gs/newdb/FeatRecord.h: $(ctp2_code)/gs/newdb/feat.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/GoalRecord.cpp \
../gs/newdb/GoalRecord.h: $(ctp2_code)/gs/newdb/goal.cdb \
		../gs/newdb/OrderRecord.h \
		$(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/GovernmentRecord.cpp \
../gs/newdb/GovernmentRecord.h: $(ctp2_code)/gs/newdb/government.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/IconRecord.cpp \
../gs/newdb/IconRecord.h: $(ctp2_code)/gs/newdb/icon.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/ImprovementListRecord.cpp \
../gs/newdb/ImprovementListRecord.h: $(ctp2_code)/gs/newdb/strategy.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/OrderRecord.cpp \
../gs/newdb/OrderRecord.h: $(ctp2_code)/gs/newdb/order.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/PersonalityRecord.cpp \
../gs/newdb/PersonalityRecord.h: $(ctp2_code)/gs/newdb/personality.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/PopRecord.cpp \
../gs/newdb/PopRecord.h: $(ctp2_code)/gs/newdb/pop.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/ResourceRecord.cpp \
../gs/newdb/ResourceRecord.h: $(ctp2_code)/gs/newdb/terrain.cdb \
		../gs/newdb/SoundRecord.h \
		$(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/SoundRecord.cpp \
../gs/newdb/SoundRecord.h: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/SpecialAttackInfoRecord.cpp \
../gs/newdb/SpecialAttackInfoRecord.h: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/SpecialEffectRecord.cpp \
../gs/newdb/SpecialEffectRecord.h: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/SpriteRecord.cpp \
../gs/newdb/SpriteRecord.h: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/StrategyRecord.cpp \
../gs/newdb/StrategyRecord.h: $(ctp2_code)/gs/newdb/strategy.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/TerrainImprovementRecord.cpp \
../gs/newdb/TerrainImprovementRecord.h: $(ctp2_code)/gs/newdb/terrimprove.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/TerrainRecord.cpp \
../gs/newdb/TerrainRecord.h: $(ctp2_code)/gs/newdb/terrain.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/UnitBuildListRecord.cpp \
../gs/newdb/UnitBuildListRecord.h: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/UnitRecord.cpp \
../gs/newdb/UnitRecord.h: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/WonderBuildListRecord.cpp \
../gs/newdb/WonderBuildListRecord.h: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/WonderMovieRecord.cpp \
../gs/newdb/WonderMovieRecord.h: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<

../gs/newdb/WonderRecord.cpp \
../gs/newdb/WonderRecord.h: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) ../gs/newdb < $<


AM_CXXFLAGS=\
	$(SDL_CFLAGS)

AM_CPPFLAGS=\
	$(CTP2_NOWIN32_INC) \
	$(DIRECTX_INCLUDES) \
	$(DXMEDIA_INCLUDES) \
	-I$(ctp2_code)/os/include \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ctp/ctp2_utils \
	-I$(ctp2_code)/ctp/ctp2_rsrc \
	$(CTP2_WIN32_RSRC_INC) \
	-I$(ctp2_code)/ctp/debugtools \
		$(INCORDER_ctp2_code__libs__freetype__lib) \
		$(INCORDER_ctp2_code__libs__freetype__utils) \
		$(INCORDER_ctp2_code__libs__freetype__win32) \
	-I$(ctp2_code)/ui/interface \
	-I$(ctp2_code)/ui/netshell \
	-I$(ctp2_code)/robot/utility \
	-I$(ctp2_code)/robot/pathing \
	-I$(ctp2_code)/robot/aibackdoor \
	$(CTP2_WIN32_FREETYPE_INC) \
	-I$(ctp2_code)/gfx/spritesys \
	-I$(ctp2_code)/gfx/tilesys \
	-I$(ctp2_code)/gfx/gfx_utils \
	-I$(ctp2_code)/gs/database \
	-I$(ctp2_code)/gs/fileio \
	-I$(ctp2_code)/gs/gameobj \
	-I$(ctp2_code)/gs/utility \
	-I$(ctp2_code)/gs/world \
	-I$(ctp2_code)/net/io \
	-I$(ctp2_code)/net/general \
	-I$(ctp2_code)/ui/aui_utils \
	-I$(ctp2_code)/ui/aui_sdl \
	-I$(ctp2_code)/ui/aui_directx \
	-I$(ctp2_code)/ui/aui_ctp2 \
	-I$(ctp2_code)/ui/aui_common \
		$(INCORDER_ctp2_code__libs__tiff) \
		$(CTP2_ANET_INC) \
	-I$(ctp2_code)/mm \
	-I$(ctp2_code)/robotcom/backdoor \
	-I$(ctp2_code)/gs/slic \
	-I$(ctp2_code)/gfx/layers \
	-I$(ctp2_code)/mapgen \
	-I$(ctp2_code)/ui/freetype \
	-I$(ctp2_code)/sound \
	-I$(ctp2_code)/GameWatch/gamewatch \
	-I$(ctp2_code)/GameWatch/gwciv \
	$(CTP2_WIN32_MILES_INC) \
	-I$(ctp2_code)/ctp/fingerprint \
	$(CTP2_WIN32_ZLIB_INC) \
	-I$(ctp2_code)/ui/slic_debug \
	-I$(ctp2_code)/gs/outcom \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ui/aui_common \
	-I$(ctp2_code)/ui/ldl \
		$(INCORDER_ctp2_code__libs__dxmedia__classes__base) \
		$(INCORDER_ctp2_code__libs__dxmedia__include) \
		$(INCORDER_ctp2_code__libs__directx__include) \
	-I$(ctp2_code)/gs/events \
	-I$(top_builddir)/ctp2_code/gs/newdb \
	-I$(ctp2_code)/gs/newdb \
	-I$(ctp2_code)/ai/diplomacy \
	-I$(ctp2_code)/ai/mapanalysis \
	-I$(ctp2_code)/ai/strategy/scheduler \
	-I$(ctp2_code)/ai/strategy/agents \
	-I$(ctp2_code)/ai/strategy/goals \
	-I$(ctp2_code)/ai/strategy/squads \
	-I$(ctp2_code)/ai/citymanagement
