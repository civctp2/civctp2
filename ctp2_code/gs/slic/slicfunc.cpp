//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SLIC functions
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - GetNearestWater function fixed by Martin Gühmann November 2nd 2003.
// - New Slic functions of CTP2.1 readded by Martin Gühmann and JJB.
// - Enable automatic selection of a unit (or city) when clicking an eyepoint.
// - Fixed cut-and-paste error (no apparent impact, but might prevent crash).
// - FreeAllSlaves slic function added by The Big MC November 24th 2003.
// - Filled code for GetRoundsToNextDisaster and GetCurrentPollutionLevel.
// - open_GreatLibrary calls with incorrect first argument type replaced.
// - HasAdvance function now accepts also advance indices in addition to
//   advance strings by Martin Gühmann.
// - StringCompare function overloaded to allow the comparision between strings
//   string IDs and strings retrieved from builtins, by Martin Gühmann.
// - New slic functions added by Martin Gühmann:
//   - CargoCapacity:    Gets number of additional units a unit can carry.
//   - MaxCargoSize:     Gets the maximum number of units a unit can carry.
//   - CargoSize:        Gets the current number of units a unit is carrying.
//   - GetUnitFromCargo: Gets the i'th unit a unit is carrying.
//   - GetContinent:     Gets the continent ID of an location.
//   - IsWater:          Gets whether a location is water.
// - ArmyIsValid    : Added reading of the argument - to make a valid result
//                    possible.
// - GrantAdvance   : Added input checks and an (optional) reason argument.
// - Ambiguous sqrt resolved.
// - CreateUnit function doesn't crash anymore if the unit type argument
//   represents an invalid unit type. - Feb. 24th 2005 Martin Gühmann
// - New slic function by Solver: IsOnSameContinent - Checks whether two
//   locations are on the same continent.
// - Added AddSlaves function modelled after the AddPops function.
// - Improved argument checking of Get<Type> functions.
// - AOM facilitation: set player[0] to the recipient when undefined.
// - Replaced old civilisation database by new one. (Aug 20th 2005 Martin Gühmann)
// - Removed the old endgame database. (Aug 29th 2005 Martin Gühmann)
// - Made government modified for units work here. (July 29th 2006 Martin Gühmann)
// - Added GetContinentSize slic function. (Dec 24th 2006 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

//TO ADD NEW SLIC FUNCTIONS YOU NEED TO MODIFY
//..\ctp2_code\gs\slic\SlicFunc.h
//..\ctp2_code\gs\slic\slicfunc.cpp
//..\ctp2_code\gs\slic\SlicEngine.cpp

#include "c3.h"
#include "SlicFunc.h"
#include "slicif.h"
#include "StrDB.h"
#include "civarchive.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "MapPoint.h"
#include "SlicSymbol.h"
#include "AdvanceRecord.h"
#include "player.h"
#include "World.h"
#include "Unit.h"
#include "WonderRecord.h"
#include "Token.h"
#include "Cell.h"
#include "UnitDynArr.h"
#include "SlicEyePoint.h"
#include "Strengths.h"
#include "Regard.h"
#include "message.h"
#include "Agreement.h"
#include "AgreementMatrix.h"
#include "TradeOffer.h"
#include "Civilisation.h"
#include "screenutils.h"
#include "civapp.h"
#include "filenamedb.h"
#include "UnitPool.h"
#include "Order.h"
#include "Army.h"
#include "ArmyPool.h"
#include "network.h"
#include "TurnCnt.h"
#include "TradeOfferPool.h"
#include "screenutils.h"
#include "greatlibrary.h"
#include "SelItem.h"
#include "BuildingRecord.h"
#include "buildingutil.h"	//EMOD
#include "GovernmentRecord.h" //EMOD
#include "AttractWindow.h"
#include "texttab.h"
#include "UnitData.h"
#include "citydata.h"
#include "profileDB.h"
#include "statswindow.h"
#include "cellunitlist.h"
#include "AICause.h"
#include "RandGen.h"

#include "Happy.h"
#include "SoundRecord.h"
#include "soundmanager.h"
#include "TerrainRecord.h"
#include "installationtree.h"
#include "tiledmap.h"
#include "Advances.h"
#include "GameOver.h"
#include "gamefile.h"
#include "Sci.h"
#include "MessagePool.h"
#include "CivilisationPool.h"
#include "AgreementPool.h"
#include "net_action.h"
#include "net_info.h"
#include "MaterialPool.h"
#include "SoundRecord.h"
#include "CivilisationRecord.h"
#include "civ3_main.h"
#include "GameEventManager.h"
#include "GameEventArgList.h"
#include "GameEventArgument.h"
#include "UnitRecord.h"
#include "ResourceRecord.h"
#include "MoveFlags.h"
#include "director.h"
#include "MapFile.h"
#include "SlicStruct.h"
#include "SpecialEffectRecord.h"
#include "Globals.h"
#include "wonderutil.h"
#include "ConstRecord.h"
#include "FeatTracker.h"
#include "FeatRecord.h"
#include "Gold.h"

#include "radarwindow.h"
#include "controlpanelwindow.h"

#include "pollution.h"

#include "TerrainImprovementRecord.h"
#include "terrainutil.h"

#include "Diplomat.h"

#include "scenarioeditor.h"
#include "EditQueue.h"

bool g_forceTurnDisplay = false;

#include "MessageData.h"

#include "TerrImprove.h"
#include "CTP2Combat.h"

extern CivApp		*g_civApp;
extern FilenameDB	*g_theMessageIconFileDB;
extern GreatLibrary *g_greatLibrary;
extern AttractWindow *g_attractWindow;
extern SoundManager *g_soundManager;
extern Pollution *		g_thePollution;

#define k_MESSAGE_TYPE_HASH_SIZE 16

char g_missingSegment[256];

SlicArgList::SlicArgList()
:
    m_argValue  ()
{
}

void SlicArgList::ReleaseSymbols(void)
{
    for (size_t arg = 0; arg < m_argValue.size(); ++arg)
    {
	    delete m_argValue[arg].m_symbol;
    }
}

void SlicArgList::AddArg(SA_TYPE type, sint32 value)
{
    m_argValue.push_back(SlicArg(type, value, NULL, NULL));
}

void SlicArgList::AddArg(SlicSegment *segment, SlicSymbolData *symbol)
{
    m_argValue.push_back(SlicArg(SA_TYPE_SEGMENT, 0, segment, symbol));
}

void SlicArgList::AddArg(SA_TYPE type, SlicSymbolData *symbol)
{
	m_argValue.push_back(SlicArg(type, 0, NULL, symbol));
}

//----------------------------------------------------------------------------
//
// Name       : SlicArgList::Get<Type>
//
// Description: Try to read an argument value of a specific type.
//
// Parameters : arg     : argument index
//
// Globals    : -
//
// Returns    : BOOL    : an argument value has been read successfully
//              value   : the read value
//
// Remark(s)  : - value is only valid when TRUE is returned
//              - <Type> can one of: Int, Unit, Army, City, Player,
//                                   Pos, String, StringId
//
//----------------------------------------------------------------------------
bool SlicArgList::GetInt(sint32 arg, sint32 &value) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

    switch (m_argValue[arg].m_type)
    {
    default:
        return false;

    case SA_TYPE_INT:
		value = m_argValue[arg].m_int;
		return true;

    case SA_TYPE_INT_VAR:
    case SA_TYPE_BUILTIN:
		return m_argValue[arg].m_symbol &&
		       m_argValue[arg].m_symbol->GetIntValue(value);
	}
}

bool SlicArgList::GetUnit(sint32 arg, Unit &u) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

    switch (m_argValue[arg].m_type)
    {
    default:
        return false;

    case SA_TYPE_INT_VAR:
    case SA_TYPE_BUILTIN:
		return m_argValue[arg].m_symbol &&
		       m_argValue[arg].m_symbol->GetUnit(u);
	}
}

bool SlicArgList::GetArmy(sint32 arg, Army &a) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

    switch (m_argValue[arg].m_type)
    {
    default:
        return false;

    case SA_TYPE_INT_VAR:
    case SA_TYPE_BUILTIN:
		return m_argValue[arg].m_symbol &&
		       m_argValue[arg].m_symbol->GetArmy(a);
	}
}

bool SlicArgList::GetCity(sint32 arg, Unit &city) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

    switch (m_argValue[arg].m_type)
    {
    default:
        return false;

    case SA_TYPE_INT_VAR:
    case SA_TYPE_BUILTIN:
		return m_argValue[arg].m_symbol &&
		       m_argValue[arg].m_symbol->GetCity(city);
	}
}

bool SlicArgList::GetPos(sint32 arg, MapPoint &pos) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

    switch (m_argValue[arg].m_type)
    {
    default:
        return false;

    case SA_TYPE_INT_VAR:
    case SA_TYPE_BUILTIN:
		return m_argValue[arg].m_symbol &&
		       m_argValue[arg].m_symbol->GetPos(pos);
	}
}

bool SlicArgList::GetPlayer(sint32 arg, sint32 &value) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

    switch (m_argValue[arg].m_type)
    {
    default:
        return false;

    case SA_TYPE_INT:
        value = m_argValue[arg].m_int;
        return (value >= 0) && (value < k_MAX_PLAYERS);

    case SA_TYPE_INT_VAR:
    case SA_TYPE_BUILTIN:
		return m_argValue[arg].m_symbol &&
		       m_argValue[arg].m_symbol->GetPlayer(value);
	}
}

bool SlicArgList::GetString(sint32 arg, char *&value) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

	if ((m_argValue[arg].m_type == SA_TYPE_HARD_STRING) &&
	     m_argValue[arg].m_symbol
	   )
	{
		value = const_cast<char *>(m_argValue[arg].m_symbol->GetName());
		return true;
	}

	return false;
}

bool SlicArgList::GetStringId(sint32 arg, StringId & value) const
{
	Assert((arg >= 0) && (static_cast<size_t>(arg) < m_argValue.size()));

	switch (m_argValue[arg].m_type)
	{
	default:
		return false;

	case SA_TYPE_STRING:
		value = m_argValue[arg].m_int;
		return true;
	}
}

GameEventArgList *SlicArgList::CreateGameEventArgs(GAME_EVENT ev)
{
	GameEventArgList *newArgs = new GameEventArgList();
	Army a;
	Unit u;
	Unit city;
	sint32 value;
	MapPoint pos;

	Assert(m_argValue.size() == g_gevManager->GetNumArgs(ev));

	for (size_t i = 0; i < Count(); ++i)
	{
		EVENTLOG(("%s ", g_gevManager->ArgCharToName(g_gevManager->ArgChar(ev, i))));
		switch(g_gevManager->ArgChar(ev, i)) {
			case GEAC_ARMY:
				if(GetArmy(i, a)) {
					newArgs->Add(new GameEventArgument(GEA_Army, a));
					EVENTLOG(("0x%lx", a.m_id));
				}
				break;
			case GEAC_UNIT:
				if(GetUnit(i, u)) {
					newArgs->Add(new GameEventArgument(GEA_Unit, u));
					EVENTLOG(("0x%lx", u.m_id));
				}
				break;
			case GEAC_CITY:
				if(GetCity(i, city)) {
					newArgs->Add(new GameEventArgument(GEA_City, city));
					EVENTLOG(("0x%lx", city.m_id));
				}
				break;
			case GEAC_POP:

				Assert(FALSE);
				break;
			case GEAC_GOLD:
				if(GetInt(i, value)) {
					newArgs->Add(new GameEventArgument(GEA_Gold, value));
					EVENTLOG(("%d", value));
				}
				break;
			case GEAC_PATH:

				Assert(FALSE);
				break;
			case GEAC_MAPPOINT:
				if(GetPos(i, pos)) {
					newArgs->Add(new GameEventArgument(GEA_MapPoint, pos));
					EVENTLOG(("(%d,%d)", pos.x, pos.y));
				}
				break;
			case GEAC_PLAYER:
				if(GetPlayer(i, value)) {
					newArgs->Add(new GameEventArgument(GEA_Player, value));
					EVENTLOG(("%d", value));
				}
				break;
			case GEAC_INT:
				if(GetInt(i, value)) {
					newArgs->Add(new GameEventArgument(GEA_Int, value));
					EVENTLOG(("%d", value));
				}
				break;
			case GEAC_DIRECTION:
				if(GetInt(i, value)) {
					newArgs->Add(new GameEventArgument(GEA_Direction, value));
					EVENTLOG(("%d", value));
				}
				break;
			case GEAC_ADVANCE:
				if(GetInt(i, value)) {
					newArgs->Add(new GameEventArgument(GEA_Advance, value));
					EVENTLOG(("%d", value));
				}
				break;
			case GEAC_WONDER:
				if(GetInt(i, value)) {
					newArgs->Add(new GameEventArgument(GEA_Wonder, value));
					EVENTLOG(("%d", value));
				}
				break;
			case GEAC_IMPROVEMENT:

				Assert(FALSE);
				break;
			default:
				Assert(FALSE);
				break;
		}

		if (i + 1 < m_argValue.size())
		{
			EVENTLOG((", "));
		}
	}

	return newArgs;
}

SlicFunc::SlicFunc(char const * name, SLIC_FUNC_RET_TYPE type)
{
	m_type = type;
	m_name = new char[strlen(name) + 2];
	m_name[0] = '_';
	strcpy(m_name + 1, name);
	m_result.m_int = 0;
}

SlicFunc::~SlicFunc()
{
	delete [] m_name;
}

//----------------------------------------------------------------------------
//
// Name       : Slic_...::Call
//
// Description: Execute a SLIC command.
//
// Parameters : args			: variable length argument list
//
// Globals    : various
//
// Returns    : SFN_ERROR		: indication whether the execution of the
//                                command has started.
//
// Remark(s)  : SFN_ERROR_OK is returned when the execution of the command
//              has started.
//              Other return values indicate errors in the number or type of
//              the arguments.
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_PrintInt::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PrintText::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 ival;
	if (args->m_argValue[0].m_type == SA_TYPE_STRING) {
		DPRINTF(k_DBG_SLIC, ("Slic_PrintText: %s\n",
							 g_theStringDB->GetNameStr(args->m_argValue[0].m_int)));
	} else if(args->GetInt(0, ival)) {
		DPRINTF(k_DBG_SLIC, ("Slic_PrintText: %s\n",
							 g_theStringDB->GetNameStr(ival)));
	} else
		return SFN_ERROR_TYPE_ARGS;

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Text::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_STRING)
		return SFN_ERROR_TYPE_ARGS;

	DPRINTF(k_DBG_SLIC, ("Slic_Text: %s\n",
						 g_theStringDB->GetNameStr(args->m_argValue[0].m_int)));

	if(g_slicEngine->GetContext() == NULL)
		return SFN_ERROR_INTERNAL;

	g_slicEngine->GetContext()->SetMessageText(g_theStringDB->GetNameStr(args->m_argValue[0].m_int));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddMessage::Call(SlicArgList *args)
{
	PLAYER_INDEX recip;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, recip))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	if(!(args->m_argValue[1].m_segment)) {
		SlicSymbolData *sym = args->m_argValue[1].m_symbol;
		if(sym) {
			strcpy(g_missingSegment, sym->GetName());
		} else {
			strcpy(g_missingSegment, "???");
		}
		return SFN_ERROR_NOT_SEGMENT;
	}

	DPRINTF(k_DBG_SLIC, ("Slic_Message: ->%d: %s\n",
						 recip,
						 args->m_argValue[1].m_segment->GetName()));

	SlicObject *obj = new SlicObject(args->m_argValue[1].m_segment,
									 g_slicEngine->GetContext());
	obj->AddRecipient(recip);
	obj->CopyFromBuiltins();
	if (obj->GetNumPlayers() == 0)
	{
		obj->AddPlayer(recip);
	}
	g_slicEngine->Execute(obj);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Message::Call(SlicArgList *args)
{
	PLAYER_INDEX recip;

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, recip))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	if(!(args->m_argValue[1].m_segment)) {
		SlicSymbolData *sym = args->m_argValue[1].m_symbol;
		if(sym) {
			strcpy(g_missingSegment, sym->GetName());
		} else {
			strcpy(g_missingSegment, "???");
		}
		return SFN_ERROR_NOT_SEGMENT;
	}

	DPRINTF(k_DBG_SLIC, ("Slic_Message: ->%d: %s\n",
						 recip,
						 args->m_argValue[1].m_segment->GetName()));

	SlicObject *obj = new SlicObject(args->m_argValue[1].m_segment,
									 g_slicEngine->GetContext());
	obj->AddRecipient(recip);
	obj->CopyFromBuiltins();
	if (obj->GetNumPlayers() == 0)
	{
		obj->AddPlayer(recip);
	}
	g_slicEngine->Execute(obj);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_MessageAll::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	DPRINTF(k_DBG_SLIC, ("Slic_MessageAll: %s\n",
						 args->m_argValue[0].m_segment->GetName()));
	SlicObject *obj = new SlicObject(args->m_argValue[0].m_segment,
									 g_slicEngine->GetContext());
	obj->AddAllRecipients();
	g_slicEngine->Execute(obj);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_MessageAllBut::Call(SlicArgList *args)
{
	PLAYER_INDEX antiRecip;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, antiRecip))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	DPRINTF(k_DBG_SLIC, ("Slic_Message: ->!%d: %s\n",
						 antiRecip,
						 args->m_argValue[1].m_segment->GetName()));

	SlicObject *obj = new SlicObject(args->m_argValue[1].m_segment,
									 g_slicEngine->GetContext());
	obj->AddAllRecipientsBut(antiRecip);
	g_slicEngine->Execute(obj);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EyePoint::Call(SlicArgList *args)
{
	MapPoint point;
	BOOL res;
	Unit unit;




	if (args->Count() != 1 && args->Count() != 3)
		return SFN_ERROR_NUM_ARGS;

	res = args->GetPos(0, point);
	// Attempt to find a city or army always, so unit will have been filled
	// when constructing the SlicEyePoint later.
	if (args->GetCity(0, unit) || args->GetUnit(0, unit))
	{
		if ((!res) && unit.IsValid())
		{
			point	= unit.RetPos();
			res		= true;
		}
	}

	if (!res)
	{
		return SFN_ERROR_TYPE_BUILTIN;
	}

	MBCHAR text[k_MAX_MSG_LEN];
	res = args->m_argValue[0].m_symbol->GetText(text, k_MAX_MSG_LEN);
	if(!res)
		text[0] = 0;

	DPRINTF(k_DBG_SLIC, ("Slic_EyePoint: (%d,%d,%d)\n",
						 point.x, point.y, 0));

	SlicEyePoint *ep = NULL;
	if (args->Count() > 1) {
		sint32 player;
		if(!args->GetPlayer(1, player))
			return SFN_ERROR_TYPE_ARGS;

		if(args->m_argValue[2].m_type != SA_TYPE_SEGMENT) {
			return SFN_ERROR_TYPE_ARGS;
		}
		if(!(args->m_argValue[2].m_segment)) {
			SlicSymbolData *sym = args->m_argValue[2].m_symbol;
			if(sym) {
				strcpy(g_missingSegment, sym->GetName());
			} else {
				strcpy(g_missingSegment, "???");
			}
			return SFN_ERROR_NOT_SEGMENT;
		}

		ep = new SlicEyePoint(point, text, 0, EYE_POINT_TYPE_GENERIC, unit,
							  player, args->m_argValue[2].m_segment);
	} else {
		ep = new SlicEyePoint(point, text, 0, EYE_POINT_TYPE_GENERIC, unit,
							  0, NULL);
	}
	g_slicEngine->GetContext()->AddEyePoint(ep);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DisableTrigger::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if (args->m_argValue[0].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	if (!args->m_argValue[0].m_segment)
		return SFN_ERROR_TYPE_ARGS;

	DPRINTF(k_DBG_SLIC, ("Slic_DisableTrigger: %s\n", args->m_argValue[0].m_segment->GetName()));
	args->m_argValue[0].m_segment->Disable();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EnableTrigger::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if (args->m_argValue[0].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	if (!args->m_argValue[0].m_segment)
		return SFN_ERROR_TYPE_ARGS;

	DPRINTF(k_DBG_SLIC, ("Slic_EnableTrigger: %s\n", args->m_argValue[0].m_segment->GetName()));
	args->m_argValue[0].m_segment->Enable();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Return1::Call(SlicArgList *args)
{
	m_result.m_int = 1;
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;
	DPRINTF(k_DBG_SLIC, ("Slic_Return1\n"));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Return0::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;
	DPRINTF(k_DBG_SLIC, ("Slic_Return0\n"));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_HasAdvance::Call(SlicArgList *args)
{
	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	//Added by Martin Gühmann to overload this function to allow also advance
	//indices directly instead of advance strings only.
	sint32 adv;
	if(args->m_argValue[1].m_type == SA_TYPE_STRING){
		for(adv = 0; adv < g_theAdvanceDB->NumRecords(); adv++) {
			if(g_theAdvanceDB->Get(adv)->m_name == args->m_argValue[1].m_int)
				break;
		}
	}
	else if(!args->GetInt(1, adv))
		return SFN_ERROR_TYPE_ARGS;

	if(adv >= g_theAdvanceDB->NumRecords()) {
		return SFN_ERROR_NOT_ADVANCE;
	}

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player]) {
		return SFN_ERROR_DEAD_PLAYER;
	}

	m_result.m_int = g_player[player]->HasAdvance(adv);
	DPRINTF(k_DBG_SLIC, ("Slic_HasAdvance: %d\n", m_result.m_int));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsContinentBiggerThan::Call(SlicArgList *args)
{
	MapPoint point;
	BOOL res;
	sint32 size;

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetInt(1, size))
		return SFN_ERROR_TYPE_ARGS;

	res = args->GetPos(0, point);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	m_result.m_int = g_theWorld->IsContinentBiggerThan(size, point);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsHostile::Call(SlicArgList *args)
{
	sint32 p1, p2;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, p1))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(1, p2))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = ((g_player[p1]->m_mask_hostile & (1 << p2)) != 0);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_TradePoints::Call(SlicArgList *args)
{
	sint32 p;

	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, p)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	m_result.m_int = g_player[p]->m_tradeTransportPoints;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_TradePointsInUse::Call(SlicArgList *args)
{
	sint32 p;

	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, p)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	m_result.m_int = g_player[p]->m_usedTradeTransportPoints;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_TradeRoutes::Call(SlicArgList *args)
{
	sint32 p;
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, p)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	m_result.m_int = g_player[p]->GetNumTradeRoutes(); // GetNumTradeRoutes from Player is expected to return only # of active routes (i.e. no need for checks on route.IsActive())
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_HasSameGoodAsTraded::Call(SlicArgList *args)
{
	sint32 p;
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;
	if(!args->GetPlayer(0, p))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[p]->HasSameGoodAsTraded();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddCity::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	g_slicEngine->GetContext()->AddCity(city);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsSecondRowUnit::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	m_result.m_int = (unit.GetDBRec()->GetZBRangeAttack() > 1);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsFlankingUnit::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}
	m_result.m_int = (unit.GetDBRec()->GetIsFlanker());
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsBombardingUnit::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();
	m_result.m_int = rec->GetCanBombard();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsWormholeProbe::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();
	m_result.m_int = rec->GetWormholeProbe();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsUnderseaCity::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	MapPoint pos;
	city.GetPos(pos);
	m_result.m_int = g_theWorld->IsWater(pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsSpaceCity::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsSpaceUnit::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();
	m_result.m_int = rec->GetMovementTypeSpace();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsWonderType::Call(SlicArgList *args)
{
	sint32 wonder;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;
	if(!args->GetInt(0, wonder)) {
		return SFN_ERROR_TYPE_ARGS;
	}
	if(args->m_argValue[1].m_type != SA_TYPE_STRING)
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_theWonderDB->Get(wonder)->GetName() == args->m_argValue[1].m_int;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsCounterBombardingUnit::Call(SlicArgList *args)
{

	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();
	m_result.m_int = rec->GetCanCounterBombard();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsCleric::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();

	m_result.m_int = rec->HasIndulgenceSales() ||
		             rec->HasConvertCities();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsSlaver::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();
	m_result.m_int = rec->HasSlaveRaids() || rec->HasSettlerSlaveRaids();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsActiveDefender::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();
	m_result.m_int = rec->GetActiveDefenseRange() > 0;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsDiplomat::Call(SlicArgList *args)
{

	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	const UnitRecord *rec = unit.GetDBRec();
	m_result.m_int = rec->GetEstablishEmbassy() || rec->GetThrowParty() || rec->GetHearGossip();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsInRegion::Call(SlicArgList *args)
{
	Unit unit;
	MapPoint point;
	BOOL res;
	struct PSlicRegion region;
	const struct PSlicComplexRegion *cregion;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	res = args->GetPos(0, point);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	if(args->m_argValue[1].m_type == SA_TYPE_REGION) {
		res = args->m_argValue[1].m_symbol->GetRegion(region);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;

		m_result.m_int = (point.x >= region.x1 && point.x <= region.x2 &&
					point.y >= region.y1 && point.y <= region.y2);
	} else if(args->m_argValue[1].m_type == SA_TYPE_COMPLEX_REGION) {
		res = args->m_argValue[1].m_symbol->GetComplexRegion(cregion);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;

		for(; cregion && !m_result.m_int; cregion = cregion->next) {
			m_result.m_int = point.x >= cregion->x1 && point.x <= cregion->x2 &&
				point.y >= cregion->y1 && point.y <= cregion->y2;
		}
	} else {
		return SFN_ERROR_TYPE_ARGS;
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_UnitHasFlag::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_UnitsInCell::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint point;
	if (!args->GetPos(0, point))
		return SFN_ERROR_TYPE_BUILTIN;

	m_result.m_int = g_theWorld->GetCell(point)->GetNumUnits();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerCityCount::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if (g_player[player])
    {
		m_result.m_int = g_player[player]->m_all_cities->Num();
    }
    else
    {
		m_result.m_int = 0;
    }

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_RegardLevel::Call(SlicArgList *args)
{
	PLAYER_INDEX pl1, pl2;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, pl1))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(1, pl2))
		return SFN_ERROR_TYPE_ARGS;

	if (g_player[pl1] && g_player[pl2])
    {
		m_result.m_int = g_player[pl1]->GetRegard()->GetUpdatedRegard(pl1, pl2);
    }
    else
    {
		m_result.m_int = 0;
    }

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ChangeRegardLevel::Call(SlicArgList *args)
{
	PLAYER_INDEX pl1, pl2;
	sint32 level;

	if (args->Count() != 3)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, pl1))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, level))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(2, pl2))
		return SFN_ERROR_TYPE_ARGS;

	if(g_player[pl1] && g_player[pl2]) {





        Assert(0);
		g_player[pl1]->GetRegard()->SetForPlayer(pl2, (REGARD_TYPE)(g_player[pl1]->GetRegard()->GetForPlayer(pl2) + level));
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Kill::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	Message message;
	g_slicEngine->GetCurrentMessage(message);

	if(message == Message())
		return SFN_ERROR_NOT_IN_BUTTON;

	g_slicEngine->KillCurrentMessage();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_MinimizeAction::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	Message message;
	g_slicEngine->GetCurrentMessage(message);

	if(message == Message())
		return SFN_ERROR_NOT_IN_BUTTON;

	g_slicEngine->AddCurrentMessage();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DeactivateTutorial::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	g_slicEngine->SetTutorialActive(FALSE);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ControlsRegion::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	struct PSlicRegion region;
	const struct PSlicComplexRegion *cregion;
	BOOL res;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	BOOL done = FALSE;
	sint32 ownedCells = 0;

	if(args->m_argValue[1].m_type == SA_TYPE_REGION) {
		res = args->m_argValue[1].m_symbol->GetRegion(region);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;

		m_result.m_int = TRUE;
		for(sint32 x = region.x1; x <= region.x2 && !done; x++) {
			for(sint32 y = region.y1; y <= region.y2 && !done; y++) {
				sint32 owner = g_theWorld->GetCell(x,y)->GetOwner();
				if(owner != player && owner >= 0) {
					m_result.m_int = FALSE;
					done = TRUE;
				} else if(owner == player) {
					ownedCells++;
				}
			}
		}
	} else if(args->m_argValue[1].m_type == SA_TYPE_COMPLEX_REGION) {
		res = args->m_argValue[1].m_symbol->GetComplexRegion(cregion);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;

		m_result.m_int = TRUE;
		for(; cregion && !done; cregion = cregion->next) {
			for(sint32 x = cregion->x1; x <= cregion->x2 && !done; x++) {
				for(sint32 y = cregion->y1; y <= cregion->y2 && !done; y++) {
					sint32 owner = g_theWorld->GetCell(x,y)->GetOwner();
					if(owner != player && owner >= 0) {
						m_result.m_int = FALSE;
						done = TRUE;
					} else if(owner == player) {
						ownedCells++;
					}
				}
			}
		}
	}

	if(m_result.m_int && ownedCells <= 0)
		m_result.m_int = FALSE;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DemandWarFromAllies::Call(SlicArgList *args)
{
	PLAYER_INDEX demandingPlayer, againstPlayer;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, demandingPlayer)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!args->GetPlayer(1, againstPlayer)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && i != demandingPlayer && i != againstPlayer &&
		   g_player[demandingPlayer]->IsFriendly(i)) {
			g_player[demandingPlayer]->RequestDemandAttackEnemy(i, againstPlayer);
		}
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_KnowledgeRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_KNOWLEDGE);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_MilitaryRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_MILITARY);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_TradeRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_TRADE);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GoldRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_GOLD);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PopulationRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_POPULATION);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CitiesRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_CITIES);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GeographicRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_GEOGRAPHICAL);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SpaceRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_SPACE);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_UnderseaRank::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player]->GetRank(STRENGTH_CAT_UNDERSEA);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Accept::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	Message message;
	g_slicEngine->GetCurrentMessage(message);

	if(message == Message())
		return SFN_ERROR_NOT_IN_BUTTON;

	message.Accept() ;
	if(g_theMessagePool->IsValid(message)) {

		message.AccessData()->KillMessageWindow();
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Reject::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	Message message;
	g_slicEngine->GetCurrentMessage(message);

	if(message == Message())
		return SFN_ERROR_NOT_IN_BUTTON;

	message.Reject() ;
	if(g_theMessagePool->IsValid(message)) {

		message.AccessData()->KillMessageWindow();
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EyeDropdown::Call(SlicArgList *args)
{
	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 startAt;

	if(!args->GetInt(0, startAt))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_BUILTIN) {
		return SFN_ERROR_TYPE_ARGS;
	}

#if 0   /// @todo Find out what this is supposed to do
	startAt--;

	SlicSymbolData *sym = args->m_argValue[1].m_symbol;
	SlicObject *context = g_slicEngine->GetContext();
#endif

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CaptureCity::Call(SlicArgList *args)
{
	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	Unit city;
	if(!args->GetPlayer(0, player)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!args->GetCity(1, city)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	g_player[player]->MakeCaptureCityPriority(city);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CaptureRegion::Call(SlicArgList *args)
{
	sint32 player;
	BOOL res;

	struct PSlicRegion region;
	const struct PSlicComplexRegion *cregion;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, player)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(args->m_argValue[1].m_type == SA_TYPE_REGION) {
		res = args->m_argValue[1].m_symbol->GetRegion(region);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;
		static PSlicComplexRegion tempRegion = *(struct PSlicComplexRegion*)&region;
		g_player[player]->MakeCaptureRegionPriority(&tempRegion);
	} else if(args->m_argValue[1].m_type == SA_TYPE_COMPLEX_REGION) {
		res = args->m_argValue[1].m_symbol->GetComplexRegion(cregion);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;
		g_player[player]->MakeCaptureRegionPriority(cregion);
	} else {
		return SFN_ERROR_TYPE_ARGS;
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LeaveRegion::Call(SlicArgList *args)
{
	sint32 player;

	BOOL res;
	struct PSlicRegion region;
	const struct PSlicComplexRegion *cregion;

	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, player)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(args->m_argValue[1].m_type == SA_TYPE_REGION) {
		res = args->m_argValue[1].m_symbol->GetRegion(region);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;
		static PSlicComplexRegion tempRegion = *(struct PSlicComplexRegion*)&region;
		g_player[player]->MakeLeaveRegionPriority(&tempRegion);
	} else if(args->m_argValue[1].m_type == SA_TYPE_COMPLEX_REGION) {
		res = args->m_argValue[1].m_symbol->GetComplexRegion(cregion);
		if(!res)
			return SFN_ERROR_TYPE_ARGS;
		g_player[player]->MakeLeaveRegionPriority(cregion);
	} else {
		return SFN_ERROR_TYPE_ARGS;
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Surrender::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	g_player[player]->Surrender();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Research::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	Message message;
	g_slicEngine->GetCurrentMessage(message);

	if(message == Message())
		return SFN_ERROR_NOT_IN_BUTTON;

	sint32 player = message.GetOwner();
	AdvanceType adv = message.GetSelectedAdvance();

	g_player[player]->StartResearching(adv);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_MessageType::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_HARD_STRING)
		return SFN_ERROR_TYPE_ARGS;

	const char *tname = args->m_argValue[0].m_symbol->GetName();
	if (!tname)
		return SFN_ERROR_NOT_MESSAGE_TYPE;

	char fullselectedname[1024];
	sprintf(fullselectedname, "%s_SELECTED", tname);

	sint32 msgTypeIndex = g_theMessageIconFileDB->FindTypeIndex(tname);
	if(msgTypeIndex < 0) {
		msgTypeIndex = 0;
	}

	sint32 msgTypeSelectedIndex = g_theMessageIconFileDB->FindTypeIndex(fullselectedname);
	if(msgTypeSelectedIndex < 0) {
		msgTypeSelectedIndex= 1;
	}

	g_slicEngine->GetContext()->SetMessageType((MESSAGE_TYPE)msgTypeIndex,
											   (MESSAGE_TYPE)msgTypeSelectedIndex);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Caption::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_STRING)
		return SFN_ERROR_TYPE_ARGS;

	if(g_slicEngine->GetContext() == NULL)
		return SFN_ERROR_INTERNAL;

	g_slicEngine->GetContext()->SetMessageCaption(g_theStringDB->GetNameStr(args->m_argValue[0].m_int));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Duration::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 duration;

	if(!args->GetInt(0, duration))
		return SFN_ERROR_TYPE_ARGS;

	g_slicEngine->GetContext()->SetMessageDuration(duration);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_BreakAgreement::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;











	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AcceptTradeOffer::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	SlicObject *context = g_slicEngine->GetContext();
	TradeOffer offer = context->GetTradeOffer(index - 1);
	if(g_theTradeOfferPool->IsValid(offer)) {

		if(offer.Accept(context->GetPlayer(0),
						context->GetCity(2),
						context->GetCity(3))) {
			if(g_theTradeOfferPool->IsValid(offer)) {
				offer.Kill();
			}
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DontAcceptTradeOffer::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	SlicObject *context = g_slicEngine->GetContext();
	sint32 player = context->GetPlayer(0);




	if(g_network.IsHost()) {
		g_network.QueuePacket(g_network.IndexToId(player),
							  new NetInfo(NET_INFO_CODE_OFFER_REJECTED_MESSAGE,
										  player,
										  context->GetRecipient(0)));
	} else if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_OFFER_REJECTED_MESSAGE,
										   player, context->GetRecipient(0)));
	}

	SlicObject *so = new SlicObject("91OfferRejected");
	so->AddRecipient(player);
	so->AddCivilisation(context->GetRecipient(0));
	g_slicEngine->Execute(so);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetGovernment::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 gov;
	if(!args->GetInt(0, gov))
		return SFN_ERROR_TYPE_ARGS;

	Message message;
	g_slicEngine->GetCurrentMessage(message);

	if(message == Message())
		return SFN_ERROR_NOT_IN_BUTTON;

	sint32 player = message.GetOwner();
	g_player[player]->SetGovernmentType(gov);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_StealRandomAdvance::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	SlicObject *context = g_slicEngine->GetContext();
	Unit u = context->GetUnit(0);
	sint32 owner = u.GetOwner();

	if(g_network.IsClient()) {
		if(owner == g_selected_item->GetVisiblePlayer()) {
			g_network.SendAction(new NetAction(NET_ACTION_STEAL_TECHNOLOGY,
										   u.m_id, context->GetCity(0).m_id,
										   -1));
		}
	}

	ORDER_RESULT res = u.StealTechnology(context->GetCity(0), (sint32)-1);
	if(res != ORDER_RESULT_ILLEGAL) {
		sint32 index = g_orderInfoMap[UNIT_ORDER_STEAL_TECHNOLOGY];
		OrderInfo *oi = NULL;
		if(index >= 0 && index < g_numOrderInfo) {
			oi = &g_orderInfo[index];
		}

		if(oi && oi->m_goldCost > 0) {
			g_player[owner]->m_gold->SubGold(oi->m_goldCost);
		}

		if(g_theUnitPool->IsValid(u)) {
			u.SetFlag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN);
			if(oi && oi->m_moveCost > 0) {
				bool out_of_fuel;
				u.DeductMoveCost(oi->m_moveCost, out_of_fuel);
			}
			u.ClearFlag(k_UDF_FIRST_MOVE);
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_StealSpecificAdvance::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	SlicObject *context = g_slicEngine->GetContext();
	Message message;
	g_slicEngine->GetCurrentMessage(message);

	if(message == Message())
		return SFN_ERROR_NOT_IN_BUTTON;

	AdvanceType adv = message.GetSelectedAdvance();

	Unit u = context->GetUnit(0);
	sint32 owner = u.GetOwner();

	if(g_network.IsClient()) {
		if(u.GetOwner() == g_selected_item->GetVisiblePlayer()) {
			g_network.SendAction(new NetAction(NET_ACTION_STEAL_TECHNOLOGY,
										   u.m_id, context->GetCity(0).m_id,
										   adv));
		}
	}

	ORDER_RESULT res = u.StealTechnology(context->GetCity(0), adv);
	if(res != ORDER_RESULT_ILLEGAL) {
		sint32 index = g_orderInfoMap[UNIT_ORDER_STEAL_TECHNOLOGY];
		OrderInfo *oi = NULL;
		if(index >= 0 && index < g_numOrderInfo) {
			oi = &g_orderInfo[index];
		}

		if(oi && oi->m_goldCost > 0) {
			g_player[owner]->m_gold->SubGold(oi->m_goldCost);
		}

		if(g_theUnitPool->IsValid(u)) {
			u.SetFlag(k_UDF_USED_SPECIAL_ACTION_THIS_TURN);
			if(oi && oi->m_moveCost > 0) {
				bool out_of_fuel;
				u.DeductMoveCost(oi->m_moveCost, out_of_fuel);
			}
			u.ClearFlag(k_UDF_FIRST_MOVE);
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DisableTileImprovementButton::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DisableScreensButton::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EnableTileImprovementButton::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EnableScreensButton::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenCiv::Call(SlicArgList *args)
{
	open_CivStatus();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenCity::Call(SlicArgList *args)
{
	open_CityStatus();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenUnit::Call(SlicArgList *args)
{
	open_UnitStatus();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenScience::Call(SlicArgList *args)
{
	open_ScienceStatus();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenDiplomacy::Call(SlicArgList *args)
{
	open_Diplomacy();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenTrade::Call(SlicArgList *args)
{
	open_TradeStatus();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenInfo::Call(SlicArgList *args)
{
	open_InfoScreen();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenOptions::Call(SlicArgList *args)
{
	open_OptionsScreen(1);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenCivTab::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenMaxTab::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenLaborTab::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenProductionTab::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenCityTab::Call(SlicArgList *args)
{

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ExitToShell::Call(SlicArgList *args)
{
	if (g_network.IsActive() || g_network.IsNetworkLaunch()) {
		g_civApp->PostQuitToLobbyAction();
	} else {
		g_civApp->PostEndGameAction();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SendTradeBid::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	SlicObject *context = g_slicEngine->GetContext();
	if(context->GetNumRecipients() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumCities() < 2)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumGoods() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	PLAYER_INDEX sender = context->GetRecipient(0);
	Assert(g_player[sender]);
	if(!g_player[sender])
		return SFN_ERROR_INTERNAL;

	g_player[sender]->SendTradeBid(context->GetCity(0),
								   context->GetGood(0),
								   context->GetCity(1),
								   15);
	Message message;
	g_slicEngine->GetCurrentMessage(message);
	if(message != Message()) {
		g_slicEngine->KillCurrentMessage();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AcceptTradeBid::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	SlicObject *context = g_slicEngine->GetContext();
	if(context->GetNumRecipients() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumCities() < 2)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumGoods() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumGolds() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	PLAYER_INDEX acceptor = context->GetRecipient(0);
	Assert(g_player[acceptor]);
	if(!g_player[acceptor])
		return SFN_ERROR_INTERNAL;

	g_player[acceptor]->AcceptTradeBid(context->GetCity(0),
									 context->GetGood(0),
									 context->GetCity(1),
									 context->GetGold(0));
	Message message;
	g_slicEngine->GetCurrentMessage(message);
	if(message != Message()) {
		g_slicEngine->KillCurrentMessage();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_RejectTradeBid::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	SlicObject *context = g_slicEngine->GetContext();
	if(context->GetNumRecipients() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumCities() < 2)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumGoods() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	if(context->GetNumGolds() < 1)
		return SFN_ERROR_NOT_TRADE_BID;

	PLAYER_INDEX rejecter = context->GetRecipient(0);
	Assert(g_player[rejecter]);
	if(!g_player[rejecter])
		return SFN_ERROR_INTERNAL;

	g_player[rejecter]->RejectTradeBid(context->GetCity(0),
									 context->GetGood(0),
									 context->GetCity(1),
									 context->GetGold(0));
	Message message;
	g_slicEngine->GetCurrentMessage(message);
	if(message != Message()) {
		g_slicEngine->KillCurrentMessage();
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_BreakAlliance::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	SlicObject *context = g_slicEngine->GetContext();

	if(context->GetNumRecipients() != 1)
		return SFN_ERROR_NOT_SINGLE_RECIPIENT;

	PLAYER_INDEX breaker = context->GetRecipient(0);
	PLAYER_INDEX breakee;
	if(!args->GetPlayer(0, breakee))
		return SFN_ERROR_TYPE_ARGS;

	if(g_player[breaker]) {

		Diplomat & defending_diplomat = Diplomat::GetDiplomat(breakee);

		defending_diplomat.LogViolationEvent(breaker, PROPOSAL_TREATY_ALLIANCE);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddOrder::Call(SlicArgList *args)
{
	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	if(!g_theUnitPool->IsValid(unit)) {

		return SFN_ERROR_OK;
	}

	MapPoint pos;
	if(!args->GetPos(1, pos))
		return SFN_ERROR_TYPE_BUILTIN;

	MapPoint oldPos;
	unit.GetPos(oldPos);




	Army army = unit.GetArmy();
	if(!g_theArmyPool->IsValid(army)) {
		return SFN_ERROR_OK;
	}

	UNIT_ORDER_TYPE order = UNIT_ORDER_NONE;
	if(g_slicEngine->GetContext()->GetNumOrders() > 0) {
		order = g_slicEngine->GetContext()->GetOrder(0);
	}
	if(order == UNIT_ORDER_NONE)
		order = UNIT_ORDER_MOVE_TO;

	if(order == UNIT_ORDER_MOVE_TO || order == UNIT_ORDER_MOVE) {
		MapPoint curPos;
		army.GetPos(curPos);
		if(pos == curPos)

			return SFN_ERROR_OK;

		if(!pos.IsNextTo(curPos)) {
			g_selected_item->EnterMovePath(army.GetOwner(), army,
										   curPos, pos);
			return SFN_ERROR_OK;
		} else if(order == UNIT_ORDER_MOVE) {

			order = UNIT_ORDER_MOVE_TO;
		}
	}

	army.AddOrders(order, pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EndTurn::Call(SlicArgList *args)
{


	g_director->AddEndTurn();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_FinishBuilding::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	if(!g_theUnitPool->IsValid(city)) {
		return SFN_ERROR_OK;
	}

	city.FinishBuilding();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Abort::Call(SlicArgList *args)
{
	if(args->Count() > 0)
		return SFN_ERROR_NUM_ARGS;
	g_slicEngine->GetContext()->Abort();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Show::Call(SlicArgList *args)
{
	if(args->Count() > 0)
		return SFN_ERROR_NUM_ARGS;
	g_slicEngine->GetContext()->SetInstant();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoAutoUnload::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_BUILTIN;

	sint32 autounload;
	if(!args->GetInt(1, autounload))
		return SFN_ERROR_TYPE_ARGS;

	g_selected_item->SetAutoUnload(autounload != 0);
	g_selected_item->EnterArmyMove(g_selected_item->GetVisiblePlayer(), pos);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoLandInOcean::Call(SlicArgList *args) //Disabled?
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoOutOfFuel::Call(SlicArgList *args) //Disabled?
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoPillageOwnLand::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

	Unit unit = g_slicEngine->GetContext()->GetUnit(0);

	if(!g_theUnitPool->IsValid(unit)) {
		return SFN_ERROR_OK;
	}

	MapPoint pos;
	unit.GetPos(pos);

	Army army = unit.GetArmy();
	if(!g_theArmyPool->IsValid(army)) {
		return SFN_ERROR_OK;
	}

	army.AddOrders(UNIT_ORDER_PILLAGE_UNCONDITIONALLY, pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoSellImprovement::Call(SlicArgList *args)  //Disabled?
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoCertainRevolution::Call(SlicArgList *args) //Disabled?
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoFreeSlaves::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

	Unit city = g_slicEngine->GetContext()->GetCity(0);

	if(!g_theUnitPool->IsValid(city)) {
		return SFN_ERROR_OK;
	}

	if(!city.GetData()->GetCityData()->CapturedThisTurn()) {
		if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_PROBABLE_CHEATER);
		}
		return SFN_ERROR_OK;
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_FREE_SLAVES, city.m_id));
	}

    city.FreeSlaves();

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoCannotAffordMaintenance::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoCannotAffordSupport::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoCityWillStarve::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_DoYouWillBreakRoute::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_TerrainType::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_BUILTIN;

	m_result.m_int = g_theWorld->GetTerrainType(pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryUnit::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	if(args->GetUnit(0, unit)) {
		if(!g_theUnitPool->IsValid(unit))
			return SFN_ERROR_OK;
		type = unit.GetType();
	} else {
		res = args->GetInt(0, type);
		if(!res) {
			return SFN_ERROR_TYPE_ARGS;
		}
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_UNITS);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryBuilding::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	res = args->GetInt(0, type);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_BUILDINGS);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryWonder::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	res = args->GetInt(0, type);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_WONDERS);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryAdvance::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	res = args->GetInt(0, type);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_ADVANCES);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryTerrain::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	res = args->GetInt(0, type);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_TERRAIN);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryConcept::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	res = args->GetInt(0, type);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_CONCEPTS);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryGovernment::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	res = args->GetInt(0, type);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_GOVERNMENTS);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LibraryTileImprovement::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	BOOL res;
	sint32 type;

	res = args->GetInt(0, type);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (open_GreatLibrary())
	{
		g_greatLibrary->SetLibrary(type, DATABASE_TILE_IMPROVEMENTS);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_UnitCount::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 type;
	if(!args->GetInt(1, type))
    {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 player;
	if(!args->GetPlayer(0, player)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 count = 0;
	if(g_player[player])
    {
		for(sint32 i = 0; i < g_player[player]->m_all_units->Num(); i++) {
			if(g_player[player]->m_all_units->Access(i).GetType() == type) {
				count++;
			}
		}
	}
	m_result.m_int = count;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_UnitType::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;
	if(args->m_argValue[0].m_type != SA_TYPE_HARD_STRING)
		return SFN_ERROR_TYPE_ARGS;

	const char *name = args->m_argValue[0].m_symbol->GetName();
	StringId sname;
	if(!g_theStringDB->GetStringID(name, sname)) {
		return SFN_ERROR_UNKNOWN_UNIT_TYPE;
	}

	sint32 i;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		if(g_theUnitDB->Get(i)->m_name == sname) {
			m_result.m_int = i;
			return SFN_ERROR_OK;
		}
	}
	return SFN_ERROR_UNKNOWN_UNIT_TYPE;
}

SFN_ERROR Slic_KillMessages::Call(SlicArgList *args)
{
	PLAYER_INDEX recip;

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, recip))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	if(!(args->m_argValue[1].m_segment)) {
		SlicSymbolData *sym = args->m_argValue[1].m_symbol;
		if(sym) {
			strcpy(g_missingSegment, sym->GetName());
		} else {
			strcpy(g_missingSegment, "???");
		}
		return SFN_ERROR_NOT_SEGMENT;
	}

	sint32 i;
	if(recip < 0 || recip >= k_MAX_PLAYERS || !g_player[recip])
		return SFN_ERROR_OK;

	for(i = g_player[recip]->m_messages->Num() - 1; i >= 0; i--) {
		if(g_player[recip]->m_messages->Access(i).AccessData()->GetSlicSegment() == args->m_argValue[1].m_segment) {
			g_player[recip]->m_messages->Access(i).Kill();
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_MessageClass::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 type;
	BOOL res;
	res = args->GetInt(0, type);

	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	g_slicEngine->GetContext()->SetClass(type);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_KillClass::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 recip;
	if(!args->GetPlayer(0, recip))
		return SFN_ERROR_TYPE_ARGS;

	sint32 type;
	BOOL res;
	res = args->GetInt(1, type);

	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}
	sint32 i;
	if(recip < 0 || recip >= k_MAX_PLAYERS || !g_player[recip])
		return SFN_ERROR_OK;

	for(i = g_player[recip]->m_messages->Num() - 1; i >= 0; i--) {
		if(g_player[recip]->m_messages->Access(i).GetClass() == type) {
			g_player[recip]->m_messages->Access(i).Kill();
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityHasBuilding::Call(SlicArgList *args)
{
	BOOL res;
	Unit city;

	if(args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	res = args->GetCity(0, city);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	if(args->m_argValue[1].m_type != SA_TYPE_HARD_STRING)
		return SFN_ERROR_TYPE_ARGS;
	const char *name = args->m_argValue[1].m_symbol->GetName();
	StringId sname;
	if(!g_theStringDB->GetStringID(name, sname)) {
		return SFN_ERROR_UNKNOWN_BUILDING;
	}

	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		if(g_theBuildingDB->Get(i)->m_name == sname) {
			if(city.HaveImprovement(i)) {
				m_result.m_int = 1;
			} else {
				m_result.m_int = 0;
			}
			return SFN_ERROR_OK;
		}
	}
	return SFN_ERROR_UNKNOWN_BUILDING;
}

SFN_ERROR Slic_Title::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_STRING)
		return SFN_ERROR_TYPE_ARGS;

	if(g_slicEngine->GetContext() == NULL)
		return SFN_ERROR_INTERNAL;

	g_slicEngine->GetContext()->SetMessageTitle(g_theStringDB->GetNameStr(args->m_argValue[0].m_int));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_NetworkAccept::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	g_network.ResetGuid(player);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_NetworkEject::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	g_network.KickPlayer(player);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Attract::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	char *string;
	if(!args->GetString(0, string))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_attractWindow) {
		AttractWindow::Initialize();
	}
	Assert(g_attractWindow);
	if(g_attractWindow) {
		g_attractWindow->HighlightControl(string);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_StopAttract::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	char *string;
	if(!args->GetString(0, string))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_attractWindow) {
		AttractWindow::Initialize();
	}
	Assert(g_attractWindow);
	if(g_attractWindow) {
		g_attractWindow->RemoveControl(string);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DontSave::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	g_slicEngine->GetContext()->SetDontSave();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsUnitSelected::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	ID item;
	SELECT_TYPE state;

	g_selected_item->GetTopCurItem(player, item, state);

	m_result.m_int = 0;

	if(state != SELECT_TYPE_LOCAL_ARMY) {
		return SFN_ERROR_OK;
	}

	if(args->Count() == 0) {
		m_result.m_int = 1;
		return SFN_ERROR_OK;
	} else if(args->Count() == 1) {
		Unit u;
		if(!args->GetUnit(0, u)) {
			return SFN_ERROR_TYPE_ARGS;
		}
		if(g_theUnitPool->IsValid(u)) {
			Army army(item);
			if(army.IsPresent(u)) {
				m_result.m_int = 1;
			}
		}
		return SFN_ERROR_OK;
	}
	return SFN_ERROR_NUM_ARGS;
}

SFN_ERROR Slic_IsCitySelected::Call(SlicArgList *args)
{
	PLAYER_INDEX player;
	ID item;
	SELECT_TYPE state;

	g_selected_item->GetTopCurItem(player, item, state);
	m_result.m_int = 0;

	if(state != SELECT_TYPE_LOCAL_CITY) {
		return SFN_ERROR_OK;
	}

	if(args->Count() == 0) {
		m_result.m_int = 1;
		return SFN_ERROR_OK;
	} else if(args->Count() == 1) {
		Unit city;
		if(!args->GetCity(0, city)) {
			return SFN_ERROR_TYPE_ARGS;
		}
		if(g_theUnitPool->IsValid(city)) {
			if(city.m_id == item.m_id) {
				m_result.m_int = 1;
			}
		}
		return SFN_ERROR_OK;
	}
	return SFN_ERROR_NUM_ARGS;
}

SFN_ERROR Slic_BuildingType::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;
	if(args->m_argValue[0].m_type != SA_TYPE_HARD_STRING)
		return SFN_ERROR_TYPE_ARGS;

	const char *name = args->m_argValue[0].m_symbol->GetName();
	StringId sname;
	if(!g_theStringDB->GetStringID(name, sname)) {
		return SFN_ERROR_UNKNOWN_UNIT_TYPE;
	}

	sint32 i;
	for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		if(g_theBuildingDB->Get(i)->m_name == sname) {
			m_result.m_int = i;
			return SFN_ERROR_OK;
		}
	}
	return SFN_ERROR_UNKNOWN_BUILDING;
}

SFN_ERROR Slic_IsHumanPlayer::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(g_network.IsNetworkLaunch()
	&&!g_network.IsActive()
	){
		m_result.m_int = 0;
	}
	else if(g_network.IsActive())
	{
		if( player == g_network.GetPlayerIndex()
		|| (g_network.IsHost()
		&&  g_player[player]->IsNetwork())
		){
			m_result.m_int = 1;
		}
		else
		{
			if(args->Count() != 2)
				return SFN_ERROR_NUM_ARGS;

			sint32 index;
			if(!args->GetInt(0, index))
				return SFN_ERROR_TYPE_ARGS;

			char *keystring;
			if(!args->GetString(1, keystring))
				return SFN_ERROR_TYPE_ARGS;
		}
	}
	else
	{
		if(!g_player[player]
		||  g_player[player]->IsRobot()
		){
			m_result.m_int = 0;
		}
		else
		{
			m_result.m_int = 1;
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerWagesExp::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = static_cast<sint32>(g_player[player]->GetWagesExpectation());

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerWorkdayExp::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = static_cast<sint32>(g_player[player]->GetWorkdayExpectation());

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerRationsExp::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = static_cast<sint32>(g_player[player]->GetRationsExpectation());

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerWagesLevel::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = (sint32) (g_player[player]->GetWagesExpectation() - (sint32) g_player[player]->GetUnitlessWages());

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerWorkdayLevel::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = (sint32) (g_player[player]->GetWorkdayExpectation() - (sint32) g_player[player]->GetUnitlessWorkday());

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerRationsLevel::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = (sint32) (g_player[player]->GetRationsExpectation() - (sint32) g_player[player]->GetUnitlessRations());

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityFoodDelta::Call(SlicArgList *args)
{
	if(args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}

	BOOL res;
	Unit city;
	res = args->GetCity(0, city);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	m_result.m_int = city.AccessData()->GetCityData()->GetNetCityFood();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityStarvationTurns::Call(SlicArgList *args)
{
	if(args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}

	BOOL res;
	Unit city;
	res = args->GetCity(0, city);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	m_result.m_int = city.AccessData()->GetCityData()->GetStarvationTurns();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DisableClose::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	g_slicEngine->GetContext()->DisableClose();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EnableCloseMessage::Call(SlicArgList *args)
{
	PLAYER_INDEX recip;

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetPlayer(0, recip))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_SEGMENT)
		return SFN_ERROR_TYPE_ARGS;

	if(!(args->m_argValue[1].m_segment)) {
		SlicSymbolData *sym = args->m_argValue[1].m_symbol;
		if(sym) {
			strcpy(g_missingSegment, sym->GetName());
		} else {
			strcpy(g_missingSegment, "???");
		}
		return SFN_ERROR_NOT_SEGMENT;
	}

	sint32 i;
	if(recip < 0 || recip >= k_MAX_PLAYERS || !g_player[recip])
		return SFN_ERROR_OK;

	for(i = g_player[recip]->m_messages->Num() - 1; i >= 0; i--) {
		if(g_player[recip]->m_messages->Access(i).AccessData()->GetSlicSegment() == args->m_argValue[1].m_segment) {
			g_player[recip]->m_messages->Access(i).AccessData()->DisableClose(FALSE);
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EnableCloseClass::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 recip;
	if(!args->GetPlayer(0, recip))
		return SFN_ERROR_TYPE_ARGS;

	sint32 type;
	BOOL res;
	res = args->GetInt(1, type);

	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}
	sint32 i;
	if(recip < 0 || recip >= k_MAX_PLAYERS || !g_player[recip])
		return SFN_ERROR_OK;

	for(i = g_player[recip]->m_messages->Num() - 1; i >= 0; i--) {
		if(g_player[recip]->m_messages->Access(i).GetClass() == type) {
			g_player[recip]->m_messages->Access(i).AccessData()->DisableClose(FALSE);
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddGoods::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	BOOL res;
	Unit city;
	res = args->GetCity(0, city);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	city.AddGoods(g_slicEngine->GetContext());
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GoodType::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_HARD_STRING)
		return SFN_ERROR_TYPE_ARGS;

	const char *name = args->m_argValue[0].m_symbol->GetName();
	StringId sname;
	if(!g_theStringDB->GetStringID(name, sname)) {
		return SFN_ERROR_UNKNOWN_UNIT_TYPE;
	}

	sint32 i;
	for(i = 0; i < g_theResourceDB->NumRecords(); i++) {
		if(g_theResourceDB->Get(i)->GetName() == sname) {
			m_result.m_int = i;
			return SFN_ERROR_OK;
		}
	}
	return SFN_ERROR_UNKNOWN_UNIT_TYPE;
}

SFN_ERROR Slic_GoodCount::Call(SlicArgList *args)
{
	if (args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	BOOL res;
	Unit city;
	res = args->GetCity(0, city);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	sint32 good;

	if(!args->GetInt(1, good)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	m_result.m_int = city.AccessData()->GetCityData()->GetGoodCountInRadius(good);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GoodVisibutik::Call(SlicArgList *args)
{
	return g_slicEngine->GetFunction("GoodCount")->Call(args);
}

SFN_ERROR Slic_GoodCountTotal::Call(SlicArgList *args)
{
	if (args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}

	BOOL res;
	Unit city;
	res = args->GetCity(0, city);
	if(!res)
		return SFN_ERROR_TYPE_BUILTIN;

	m_result.m_int = city.AccessData()->GetCityData()->GetGoodCountInRadius(-1);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_StartTimer::Call(SlicArgList *args)
{
	if (args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 timer, duration;
	if(!args->GetInt(0, timer))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, duration))
		return SFN_ERROR_TYPE_ARGS;

	g_slicEngine->StartTimer(timer, duration);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_StopTimer::Call(SlicArgList *args)
{
	if (args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}
	sint32 timer;
	if(!args->GetInt(0, timer))
		return SFN_ERROR_TYPE_ARGS;

	g_slicEngine->StopTimer(timer);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DisableMessageClass::Call(SlicArgList *args)
{
	if (args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}
	sint32 msgClass;
	if(!args->GetInt(0, msgClass))
		return SFN_ERROR_TYPE_ARGS;

	g_slicEngine->DisableMessageClass(msgClass);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EnableMessageClass::Call(SlicArgList *args)
{
	if (args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}
	sint32 msgClass;
	if(!args->GetInt(0, msgClass))
		return SFN_ERROR_TYPE_ARGS;
	g_slicEngine->EnableMessageClass(msgClass);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CreateUnit::Call(SlicArgList *args)
{
	if (args->Count() < 4 || args->Count() > 5) {
		return SFN_ERROR_NUM_ARGS;
	}
	sint32 owner;
	if(!args->GetPlayer(0, owner)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 type;
	if(!args->GetInt(1, type)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	MapPoint pos;
	BOOL res = args->GetPos(2, pos);
	if(!res)
		return SFN_ERROR_TYPE_ARGS;

	sint32 distance;
	if(!args->GetInt(3, distance)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	const UnitRecord *rec = g_theUnitDB->Get(type, g_player[owner]->GetGovernmentType());
	// Check if the function was called with a valid unit type.
	if(!rec) return SFN_ERROR_UNKNOWN_UNIT_TYPE;

	sint32 x, y;
	BOOL found = FALSE;
	DynamicArray<MapPoint> legalPoints;
	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			MapPoint chk(x, y);
			sint32 dist = pos.NormalizedDistance(chk);
			Cell *cell = g_theWorld->GetCell(chk);
			if(dist == distance &&
			   g_theWorld->CanEnter(chk, rec->GetMovementType()) &&
			   (!cell->UnitArmy() ||
				(cell->UnitArmy()->GetOwner() == owner &&
				 cell->UnitArmy()->Num() < k_MAX_ARMY_SIZE)) &&
			   (cell->GetCity().m_id == (0) ||
				cell->GetCity().GetOwner() == owner)) {
				legalPoints.Insert(chk);
				found = TRUE;
			}
		}
	}

	if(found) {
		MapPoint upos;
		upos = legalPoints[g_rand->Next(legalPoints.Num())];
		Unit unit;
		if(g_player[owner]) {
			unit = g_player[owner]->CreateUnit(type, upos,
													Unit(),
													FALSE,
													CAUSE_NEW_ARMY_SCENARIO);
		}
		if(unit.m_id == 0) {
			m_result.m_int = 0;
		} else {
			m_result.m_int = 1;
			if (args->Count() > 4) {
				if(args->m_argValue[4].m_type == SA_TYPE_INT_VAR) {
					SlicSymbolData *sym = args->m_argValue[4].m_symbol;
					sym->SetUnit(unit);

				}
			}
		}
	} else {
		m_result.m_int = 0;
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Random::Call(SlicArgList *args)
{
	if (args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}
	sint32 range;
	if(!args->GetInt(0, range)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(range < 1 || range > 0x7fffffff) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	m_result.m_int = g_rand->Next(range);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddCityByIndex::Call(SlicArgList *args)
{
	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 owner;
	if(!args->GetInt(0, owner)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 index;
	if(!args->GetInt(1, index)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!g_player[owner]) {
		return SFN_ERROR_DEAD_PLAYER;
	}

	if(index >= g_player[owner]->m_all_cities->Num() || index < 0) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	g_slicEngine->GetContext()->AddCity(g_player[owner]->m_all_cities->Access(index));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DetachRobot::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;
	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_network.IsActive()
	|| (g_network.IsHost()
	&& !g_player[index]->IsNetwork())
	){
		g_player[index]->m_playerType = PLAYER_TYPE_HUMAN;
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AttachRobot::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;
	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_network.IsActive()
	|| (g_network.IsHost()
	&& !g_player[index]->IsNetwork())
	){
		g_player[index]->m_playerType = PLAYER_TYPE_ROBOT;
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ForceRegard::Call(SlicArgList *args)
{
	if (args->Count() != 3)
		return SFN_ERROR_NUM_ARGS;
	sint32 ofPlayer, forPlayer, toRegard;
	if(!args->GetInt(0, ofPlayer))
		return SFN_ERROR_TYPE_ARGS;
	if(!args->GetInt(1, forPlayer))
		return SFN_ERROR_TYPE_ARGS;
	if(!args->GetInt(2, toRegard))
		return SFN_ERROR_TYPE_ARGS;




	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Cities::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetInt(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_player[player]) {
		m_result.m_int = 0;
	} else {
		m_result.m_int = g_player[player]->m_all_cities->Num();
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddPops::Call(SlicArgList *args)
{
	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;
	Unit city;
	BOOL res;

	res = args->GetCity(0, city);
	if(!res) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 count;
	if(!args->GetInt(1, count))
		return SFN_ERROR_TYPE_ARGS;

	MapPoint pos;
	city.GetPos(pos);

	sint32 i;
	if(count > 0) {
		for (i=0; i<count; i++) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
								   GEA_City, city.m_id,
								   GEA_End);

		}
	} else {
		for(i = count; i < 0; i++) {
		}
	}

	sint32 delta_martial_law;
	CityData *cd = city.GetData()->GetCityData();
	cd->GetHappy()->CalcHappiness(*cd, false, delta_martial_law, true);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_KillUnit::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit))
		return SFN_ERROR_TYPE_ARGS;

	unit.Kill(CAUSE_REMOVE_ARMY_SCENARIO, -1);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlaySound::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if (args->m_argValue[0].m_type != SA_TYPE_HARD_STRING)
		return SFN_ERROR_TYPE_ARGS;

	const char *name = args->m_argValue[0].m_symbol->GetName();
	sint32 sound = g_theSoundDB->FindTypeIndex(name);

	if(sound >= 0 && g_soundManager) {
		g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
								 sound, 0, 0);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CreateCity::Call(SlicArgList *args)
{
	if (args->Count() < 3 || args->Count() > 4)
		return SFN_ERROR_NUM_ARGS;

	sint32 owner;
	if(!args->GetPlayer(0, owner)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	MapPoint pos;
	if(!args->GetPos(1, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 distance;
	if(!args->GetInt(2, distance))
		return SFN_ERROR_TYPE_ARGS;

	sint32 x, y;
	BOOL found = FALSE;
	static DynamicArray<MapPoint> legalPoints;
	legalPoints.Clear();

	sint16 origContinent = g_theWorld->GetContinent(pos);

	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			MapPoint chk(x, y);
			if(g_theWorld->GetContinent(chk) == origContinent &&
			   !g_theWorld->IsNextToCity(chk) &&
			   !g_theWorld->IsCity(chk)) {
				legalPoints.Insert(chk);
				found = TRUE;
			}
		}
	}

	if(!found) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	}

	sint32 bestDistance = 0x7fffffff;
	sint32 bestDiff = 0x7fffffff;
	sint32 i;
	for(i = 0; i < legalPoints.Num(); i++) {
		sint32 dist = pos.NormalizedDistance(legalPoints[i]);
		if(abs(distance - dist) < bestDiff) {
			bestDiff = abs(distance - dist);
			bestDistance = dist;
		}
	}

	for(i = legalPoints.Num() - 1; i >= 0; i--) {
		sint32 dist = pos.NormalizedDistance(legalPoints[i]);
		if(dist != bestDistance) {
			legalPoints.DelIndex(i);
		}
	}

	MapPoint cpos;
	cpos = legalPoints[g_rand->Next(legalPoints.Num())];
	const UnitRecord *rec;
	for(i = 0; i < g_theUnitDB->NumRecords() - 1; i++) {
		rec = g_theUnitDB->Get(i, g_player[owner]->GetGovernmentType());
		if(!rec->GetHasPopAndCanBuild())
			continue;
		if((g_theWorld->IsLand(cpos) || g_theWorld->IsMountain(cpos)) &&
		   !g_theWorld->IsWater(cpos) && rec->GetMovementTypeLand())
			break;
		if(g_theWorld->IsWater(cpos) && rec->GetMovementTypeSea())
			break;
		if(g_theWorld->IsSpace(cpos) && rec->GetMovementTypeSpace())
			break;
	}
	if(i >= g_theUnitDB->NumRecords() - 1) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	}

	Unit city = g_player[owner]->CreateCity(i, cpos,
											CAUSE_NEW_CITY_SCENARIO,
											NULL, -1);
	if(city.m_id == 0) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	}

	m_result.m_int = 1;
	if (args->Count() > 3) {
		if(args->m_argValue[3].m_type == SA_TYPE_INT_VAR) {
			SlicSymbolData *sym = args->m_argValue[3].m_symbol;
			if(sym->GetType() == SLIC_SYM_CITY) {
				sym->SetCity(city);
			}
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CreateCoastalCity::Call(SlicArgList *args)
{
	if (args->Count() < 3 || args->Count() > 4)
		return SFN_ERROR_NUM_ARGS;

	sint32 owner;
	if(!args->GetPlayer(0, owner)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	MapPoint pos;
	if(!args->GetPos(1, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 distance;
	if(!args->GetInt(2, distance))
		return SFN_ERROR_TYPE_ARGS;

	sint32 x, y;
	BOOL found = FALSE;
	static DynamicArray<MapPoint> legalPoints;
	legalPoints.Clear();

	sint16 origContinent = g_theWorld->GetContinent(pos);

	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			MapPoint chk(x, y);
			if(g_theWorld->GetContinent(chk) == origContinent &&
			   !g_theWorld->IsNextToCity(chk) &&
			   !g_theWorld->IsCity(chk) &&
			   g_theWorld->IsNextToWater(chk.x, chk.y)) {
				legalPoints.Insert(chk);
				found = TRUE;
			}
		}
	}

	if(!found) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	}

	sint32 bestDistance = 0x7fffffff;
	sint32 bestDiff = 0x7fffffff;
	sint32 i;
	for(i = 0; i < legalPoints.Num(); i++) {
		sint32 dist = pos.NormalizedDistance(legalPoints[i]);
		if(abs(distance - dist) < bestDiff) {
			bestDiff = abs(distance - dist);
			bestDistance = dist;
		}
	}

	for(i = legalPoints.Num() - 1; i >= 0; i--) {
		sint32 dist = pos.NormalizedDistance(legalPoints[i]);
		if(dist != bestDistance) {
			legalPoints.DelIndex(i);
		}
	}

	MapPoint cpos;
	cpos = legalPoints[g_rand->Next(legalPoints.Num())];
	const UnitRecord *rec;
	for(i = 0; i < g_theUnitDB->NumRecords() - 1; i++) {
		rec = g_theUnitDB->Get(i, g_player[owner]->GetGovernmentType());
		if(!rec->GetHasPopAndCanBuild())
			continue;
		if((g_theWorld->IsLand(cpos) || g_theWorld->IsMountain(cpos)) &&
		   !g_theWorld->IsWater(cpos) && rec->GetMovementTypeLand())
			break;
		if(g_theWorld->IsWater(cpos) && rec->GetMovementTypeSea())
			break;
		if(g_theWorld->IsSpace(cpos) && rec->GetMovementTypeSpace())
			break;
	}
	if(i >= g_theUnitDB->NumRecords() - 1) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	}

	Unit city = g_player[owner]->CreateCity(i, cpos,
											CAUSE_NEW_CITY_SCENARIO,
											NULL, -1);
	if(city.m_id == 0) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	}

	m_result.m_int = 1;
	if(args->Count() > 3) {
		if(args->m_argValue[3].m_type == SA_TYPE_INT_VAR) {
			SlicSymbolData *sym = args->m_argValue[3].m_symbol;
			if(sym->GetType() == SLIC_SYM_CITY) {
				sym->SetCity(city);
			}
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ExtractLocation::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	SlicSymbolData *sym = args->m_argValue[1].m_symbol;
	if(sym->GetType() != SLIC_SYM_LOCATION)
		return SFN_ERROR_TYPE_ARGS;

	sym->SetPos(pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_FindCoastalCity::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetInt(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_INT_VAR)
		return SFN_ERROR_TYPE_ARGS;

	if(!g_player[player]) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	}

	sint32 i;
	for(i = 0; i < g_player[player]->m_all_cities->Num(); i++) {
		MapPoint pos;
		g_player[player]->m_all_cities->Access(i).GetPos(pos);
		if(g_theWorld->IsNextToWater(pos.x, pos.y)) {

			SlicSymbolData *sym = args->m_argValue[1].m_symbol;
			if(sym->GetType() != SLIC_SYM_CITY) {
				return SFN_ERROR_TYPE_ARGS;
			}
			sym->SetCity(g_player[player]->m_all_cities->Access(i));
			m_result.m_int = 1;
			return SFN_ERROR_OK;
		}
	}
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Terraform::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint tpos, pos;
	if(!args->GetPos(0, tpos)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 terrain;
	if(!args->GetInt(1, terrain)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(terrain < 0 || terrain >= g_theTerrainDB->NumRecords()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	g_theWorld->SmartSetTerrain(tpos, terrain, 0);
#if 0
	Cell *cell = g_theWorld->GetCell(tpos);
	cell->SetTerrain(terrain);
	cell->SetEnv(cell->GetEnv() & ~(k_MASK_ENV_ROAD |
										  k_MASK_ENV_IRRIGATION |
										  k_MASK_ENV_MINE |
										  k_MASK_ENV_CITY |
										  k_MASK_ENV_INSTALLATION |
										  k_MASK_ENV_CITY_RADIUS |
										  k_MASK_ENV_GOOD));
	DynamicArray<Installation> instArray;
	g_theInstallationTree->RemoveAt(tpos, instArray);
	instArray.KillList();

	switch(terrain) {
		case TERRAIN_FOREST:
		case TERRAIN_PLAINS:
		case TERRAIN_TUNDRA:
		case TERRAIN_GLACIER:
		case TERRAIN_GRASSLAND:
		case TERRAIN_DESERT:
		case TERRAIN_SWAMP:
		case TERRAIN_JUNGLE:
		case TERRAIN_HILL:
		case TERRAIN_DEAD:
		case TERRAIN_BROWN_HILL:
		case TERRAIN_WHITE_HILL:
		case TERRAIN_DEAD_HILL:
			g_theWorld->SetMovementType(tpos.x, tpos.y, k_MOVEMENT_TYPE_LAND);
			break;
		case TERRAIN_MOUNTAIN:
		case TERRAIN_BROWN_MOUNTAIN:
		case TERRAIN_WHITE_MOUNTAIN:
			g_theWorld->SetMovementType(tpos.x, tpos.y, k_MOVEMENT_TYPE_MOUNTAIN);
			break;
		case TERRAIN_WATER_SHALLOW:
		case TERRAIN_WATER_BEACH:
			g_theWorld->SetMovementType(tpos.x, tpos.y, k_MOVEMENT_TYPE_SHALLOW_WATER | k_MOVEMENT_TYPE_WATER);
			break;
		case TERRAIN_WATER_DEEP:
		case TERRAIN_WATER_VOLCANO:
		case TERRAIN_WATER_SHELF:
		case TERRAIN_WATER_TRENCH:
		case TERRAIN_WATER_RIFT:
			g_theWorld->SetMovementType(tpos.x, tpos.y, k_MOVEMENT_TYPE_WATER);
			break;
		case TERRAIN_SPACE:
			g_theWorld->SetMovementType(tpos.x, tpos.y, k_MOVEMENT_TYPE_SPACE);
			break;
	}

	g_tiledMap->PostProcessTile(tpos, g_theWorld->GetTileInfo(tpos));
	g_tiledMap->TileChanged(tpos);
	for(WORLD_DIRECTION d = NORTH; d < UP; d = (WORLD_DIRECTION)((sint32)d + 1)) {
		if(tpos.GetNeighborPosition(d, pos)) {
			g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
			g_tiledMap->TileChanged(pos);
			g_tiledMap->RedrawTile(&pos);
		}
	}
	g_tiledMap->RedrawTile(&tpos);
#endif
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlantGood::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	g_theWorld->SetRandomGood(pos.x, pos.y);
	g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
	g_tiledMap->TileChanged(pos);
	g_tiledMap->RedrawTile(&pos);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_HasGood::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 good;
	if(!g_theWorld->GetGood(pos, good)) {
		m_result.m_int = -1;
	} else {
		m_result.m_int = good;
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetRiotLevel::Call(SlicArgList *args)
{
	m_result.m_int = g_theConstDB->Get(0)->GetRiotLevel();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetRevolutionLevel::Call(SlicArgList *args)
{
	m_result.m_int = g_theConstDB->Get(0)->GetRevolutionLevel();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetRandomNeighbor::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos, npos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_INT_VAR)
		return SFN_ERROR_TYPE_ARGS;

	SlicSymbolData *sym = args->m_argValue[1].m_symbol;




	sint32 d = g_rand->Next(sint32(NOWHERE));
	BOOL retry = TRUE;
	sint32 count = 9;
	do {
		if(pos.GetNeighborPosition((WORLD_DIRECTION)d, npos)) {
			sym->SetPos(npos);
			if((g_theWorld->IsLand(pos) || g_theWorld->IsMountain(pos)) &&
			   !g_theWorld->IsWater(pos)) {
				if((g_theWorld->IsLand(npos) || g_theWorld->IsMountain(npos)) &&
				   !g_theWorld->IsWater(npos)) {
					retry = FALSE;
				}
			} else if(g_theWorld->IsWater(pos)) {
				if(g_theWorld->IsWater(npos)) {
					retry = FALSE;
				}
			}
		}
		d++;
		if(d >= sint32(NOWHERE))
			d = 0;
		count--;
	} while (retry && count >= 0);

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Name       : Slic_GrantAdvance::Call
//
// Description: Give an advance to a player.
//
// Parameters : args[0]				: the player to receive the advance
//				args[1]				: the advance
//				args[2] (optional)	: a reason why the advance is given
//
// Globals    : g_gevManager		: game event handler
//
// Returns    : SFN_ERROR			: indication whether the command has been
//									  passed to the game event handler
//
// Remark(s)  : * SFN_ERROR_OK is returned when the command has been passed to
// 			      the event handler.
//                Other return values indicate errors in the number or type of
//                the arguments.
//
//				* SFN_ERROR_OK does not indicate the result of the command
//				  execution: when e.g. the player already has the advance,
//				  SFN_ERROR_OK will be returned, but nothing will happen.
//
//----------------------------------------------------------------------------

SFN_ERROR Slic_GrantAdvance::Call(SlicArgList *args)
{
	if ((args->Count() < 2) || (args->Count() > 3))
	{
		return SFN_ERROR_NUM_ARGS;
	}

	PLAYER_INDEX	player;
	if (!args->GetPlayer(0, player))
	{
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32			adv;
	if (args->GetInt(1, adv))
	{
		if (adv >= g_theAdvanceDB->NumRecords())
		{
			return SFN_ERROR_NOT_ADVANCE;
		}
	}
	else
	{
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32			reason	= CAUSE_SCI_UNKNOWN;
	if (args->Count() >= 3)
	{
		if (args->GetInt(2, reason))
		{
			if ((reason < 0) || (reason > CAUSE_SCI_INITIAL))
			{
				return SFN_ERROR_OUT_OF_RANGE;
			}
		}
		else
		{
			return SFN_ERROR_TYPE_ARGS;
		}
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail,
						   GEV_GrantAdvance,
						   GEA_Player,	player,
						   GEA_Int,		adv,
						   GEA_Int,		reason,
						   GEA_End
						  );

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddUnit::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	g_slicEngine->GetContext()->AddUnit(u);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddExpelOrder::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	Unit u;
	if(!args->GetUnit(0, u)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!args->GetPos(1, pos)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	Army army = u.GetArmy();
	if(!g_theArmyPool->IsValid(army))
		return SFN_ERROR_OK;

	army.AddOrders(UNIT_ORDER_EXPEL, pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AllUnitsCanBeExpelled::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	Cell *cell = g_theWorld->GetCell(pos);
	if(cell->GetNumUnits() < 1) {
		m_result.m_int = 0;
		return SFN_ERROR_OK;
	} else {
		sint32 i;
		for(i = 0; i < cell->GetNumUnits(); i++) {
			if(!cell->AccessUnit(i).CanBeExpelled()) {
				m_result.m_int = 0;
				return SFN_ERROR_OK;
			}
		}
	}
	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetMessageClass::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;




	m_result.m_int = 0;

	sint32 player;
	if(!args->GetInt(0, player))
		return SFN_ERROR_TYPE_ARGS;

	sint32 messageIndex;
	if(!args->GetInt(1, messageIndex))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_player[player])
		return SFN_ERROR_OK;

	if(messageIndex < 0 || messageIndex >= g_player[player]->m_messages->Num())
		return SFN_ERROR_OK;

	m_result.m_int = g_player[player]->m_messages->Access(messageIndex).GetClass();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetPlayer::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 index, player;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_player[player])
		return SFN_ERROR_OK;

	if(index < 1)
		return SFN_ERROR_OUT_OF_RANGE;

	index--;

	SlicObject *context = g_slicEngine->GetContext();
	context->SetPlayer(index, player);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityCollectingGood::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 good;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, good))
		return SFN_ERROR_TYPE_ARGS;

#ifdef CTP1_TRADE
	m_result.m_int = city.GetData()->GetCityData()->GetLocalResourceCount(good);
#else
	m_result.m_int = (*city.CD()->GetCollectingResources())[good];
#endif
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetNearestWater::Call(SlicArgList *args)
{
	m_result.m_int = 0;

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 x, y;
	MapPoint nearest;
	sint32 minDist = 0x7fffffff;

	for(y = 0; y < g_theWorld->GetYHeight(); y++) {
		for(x = 0; x < g_theWorld->GetXWidth(); x++) {
			if(!g_theWorld->IsWater(x,y))
				continue;
			MapPoint chk(x,y);
			sint32 dist = pos.NormalizedDistance(chk);
			if(dist < minDist) {
				minDist = dist;
				nearest.Set(x,y);
			}
		}
	}
	if(minDist >= 0x7fffffff) {

		return SFN_ERROR_OK;
	}

	if(args->m_argValue[1].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	SlicSymbolData *sym = args->m_argValue[1].m_symbol;
	//Outcommented by Martin Gühmann
	//in order to fix this function
	//The porpose of this if statement is check whether
	//the second argument is a location_t or not unfortunatly
	//it only accepts something's location like location[0].location.
	//But in this case SetPos function fails.
	//As it is now the function accepts also a city[0] as second argument
	//but can't fill it with a location obviously.
//	if(sym->GetType() == SLIC_SYM_LOCATION) {
		m_result.m_int = minDist;
		sym->SetPos(nearest);
		return SFN_ERROR_OK;
//	}
//	return SFN_ERROR_TYPE_ARGS;
}

SFN_ERROR Slic_IsPlayerAlive::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetInt(0, player))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_player[player] != NULL;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GameOver::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	sint32 reason;
	if(!args->GetInt(1, reason))
		return SFN_ERROR_TYPE_ARGS;

	if(reason == 0) {
		g_player[player]->GameOver(GAME_OVER_LOST_OUT_OF_TIME, -1);
	} else if(reason == 1) {
		g_player[player]->GameOver(GAME_OVER_WON_SCENARIO, -1);
	} else if(reason == 2) {
		g_player[player]->GameOver(GAME_OVER_LOST_SCENARIO, -1);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SaveGame::Call(SlicArgList *args)
{
	if(args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}

	char *string;
	if(!args->GetString(0, string))
		return SFN_ERROR_TYPE_ARGS;


	g_isScenario = FALSE;

	GameFile::SaveGame(string, NULL);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_LoadGame::Call(SlicArgList *args)
{
	if(args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}
	char *string;
	if(!args->GetString(0, string))
		return SFN_ERROR_TYPE_ARGS;
	g_slicEngine->SetLoadGame(string);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_HasRiver::Call(SlicArgList *args)
{
	if(args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_theWorld->IsRiver(pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetScience::Call(SlicArgList *args)
{
	if(args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 player, level;
	if(!args->GetInt(0, player)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!args->GetInt(1, level)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	g_player[player]->m_science->SetLevel(level);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetResearching::Call(SlicArgList *args)
{
	if(args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 player, what;
	if(!args->GetInt(0, player)) {
		return SFN_ERROR_TYPE_ARGS;
	}
	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	if(!args->GetInt(1, what))
		return SFN_ERROR_TYPE_ARGS;

	g_player[player]->m_advances->SetResearching(what);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsInZOC::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 player;
	MapPoint pos;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPos(1, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 d;
	for(d = 0; d < sint32(NOWHERE); d++) {
		MapPoint n;
		if(pos.GetNeighborPosition((WORLD_DIRECTION)d, n)) {
			Cell *theCell = g_theWorld->GetCell(n);
			if(theCell->UnitArmy() && theCell->UnitArmy()->GetOwner() == player) {
				m_result.m_int = 1;
				return SFN_ERROR_OK;
			}
		}
	}
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DisableChooseResearch::Call(SlicArgList *args)
{
	g_player[g_slicEngine->GetTutorialPlayer()]->m_disableChooseResearch = TRUE;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_EnableChooseResearch::Call(SlicArgList *args)
{
	g_player[g_slicEngine->GetTutorialPlayer()]->m_disableChooseResearch = FALSE;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetTimerGranularity::Call(SlicArgList *args)
{
	sint32 gran;
	if(!args->GetInt(0, gran))
		return SFN_ERROR_TYPE_ARGS;

	g_slicEngine->SetTimerGranularity(gran);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_QuitToLobby::Call(SlicArgList *args)
{
	g_civApp->PostQuitToLobbyAction();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_KillEyepointMessage::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	if(g_theMessagePool->IsValid(g_slicEngine->GetEyepointMessage())) {
		g_slicEngine->GetEyepointMessage().Kill();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ClearBuildQueue::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;
	Unit city;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;
	if(g_theUnitPool->IsValid(city)) {
		city.GetData()->GetCityData()->GetBuildQueue()->Clear();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_BreakLeaveOurLands::Call(SlicArgList *args)
{
	if(args->Count() != 0) {
		return SFN_ERROR_NUM_ARGS;
	}

	SlicObject *context = g_slicEngine->GetContext();
	if(!g_player[context->GetPlayer(0)]) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!g_player[context->GetPlayer(1)]) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 unitOwner, cellOwner;
	unitOwner = context->GetPlayer(0);
	cellOwner = context->GetPlayer(1);

	if(!g_player[unitOwner])
		return SFN_ERROR_DEAD_PLAYER;

	if(!g_player[cellOwner])
		return SFN_ERROR_DEAD_PLAYER;

	if (AgreementMatrix::s_agreements.HasAgreement(
		cellOwner,
		unitOwner,
		PROPOSAL_REQUEST_WITHDRAW_TROOPS)){
	    /* expecting sync of agrrements over network to be handled by Diplomat::LogViolationEvent or dependent events
		Agreement ag = g_player[unitOwner]->m_agreed->Access(i);
		if(g_theAgreementPool->IsValid(ag) &&
		   ag.GetRecipient() == unitOwner &&
		   ag.GetAgreement() == AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS) {
			if(g_network.IsClient()) {
				g_network.SendAction(new NetAction(NET_ACTION_VIOLATE_AGREEMENT,
												   ag.m_id));
			} else if(g_network.IsHost()) {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_VIOLATE_AGREEMENT,
											  ag.m_id, unitOwner));
			}
		}
	    */
	    
	    Diplomat & diplomat = Diplomat::GetDiplomat(cellOwner);
	    diplomat.LogViolationEvent(unitOwner, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_BreakNoPiracy::Call(SlicArgList *args)
{
	if(args->Count() != 0) {
		return SFN_ERROR_NUM_ARGS;
	}

	SlicObject *context = g_slicEngine->GetContext();
	if(!g_player[context->GetPlayer(0)]) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!g_player[context->GetPlayer(1)]) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 pirate, victim;
	pirate = context->GetPlayer(0);
	victim = context->GetPlayer(1);

	if(!g_player[pirate])
		return SFN_ERROR_DEAD_PLAYER;

	if(!g_player[victim])
		return SFN_ERROR_DEAD_PLAYER;

	if (AgreementMatrix::s_agreements.HasAgreement(
		victim,
		pirate,
		PROPOSAL_REQUEST_STOP_PIRACY)){

	    /* expecting sync of agrrements over network to be handled by Diplomat::LogViolationEvent or dependent events
	    Agreement ag = g_player[victim]->FindAgreement(AGREEMENT_TYPE_NO_PIRACY, pirate);
	    if(g_theAgreementPool->IsValid(ag)){
		if(g_network.IsClient()) {
		    g_network.SendAction(new NetAction(NET_ACTION_VIOLATE_AGREEMENT,
			    ag.m_id));
		} else if(g_network.IsHost()) {
		    g_network.Enqueue(new NetInfo(NET_INFO_CODE_VIOLATE_AGREEMENT,
			    ag.m_id, pirate));
		}
	    }
	    */
	    
	    Diplomat & route_diplomat = Diplomat::GetDiplomat(victim);
	    route_diplomat.LogViolationEvent(pirate, PROPOSAL_REQUEST_STOP_PIRACY);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_BreakTradePact::Call(SlicArgList *args)
{
	if(args->Count() != 0) {
		return SFN_ERROR_NUM_ARGS;
	}

	SlicObject *context = g_slicEngine->GetContext();
	if(!g_player[context->GetPlayer(0)]) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(!g_player[context->GetPlayer(1)]) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 pirate, victim;
	pirate = context->GetPlayer(0);
	victim = context->GetPlayer(1);

	if(!g_player[pirate])
		return SFN_ERROR_DEAD_PLAYER;

	if(!g_player[victim])
		return SFN_ERROR_DEAD_PLAYER;

	if (AgreementMatrix::s_agreements.HasAgreement(
		victim,
		pirate,
		PROPOSAL_TREATY_TRADE_PACT)){

	    /* expecting sync of agrrements over network to be handled by Diplomat::LogViolationEvent or dependent events
	    Agreement ag = g_player[victim]->FindAgreement(AGREEMENT_TYPE_TRADE_PACT, pirate);
	    if(g_theAgreementPool->IsValid(ag)){
		if(g_network.IsClient()) {
		    g_network.SendAction(new NetAction(NET_ACTION_VIOLATE_AGREEMENT,
			    ag.m_id));
		} else if(g_network.IsHost()) {
		    g_network.Enqueue(new NetInfo(NET_INFO_CODE_VIOLATE_AGREEMENT,
			    ag.m_id, pirate));
		}
	    }
	    */
	    
	    Diplomat & route_diplomat = Diplomat::GetDiplomat(victim);
	    route_diplomat.LogViolationEvent(pirate, PROPOSAL_TREATY_TRADE_PACT);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_UseDirector::Call(SlicArgList *args)
{
	if(args->Count() != 0) {
		return SFN_ERROR_NUM_ARGS;
	}

	g_slicEngine->GetContext()->SetUseDirector();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ClearOrders::Call(SlicArgList *args)
{
	if(args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if(g_theUnitPool->IsValid(u) &&
		g_theArmyPool->IsValid(u.GetArmy())) {
		u.GetArmy().ClearOrders();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetUnit::Call(SlicArgList *args)
{
	if(args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	Assert(index >= 1);
	if(index < 1)
		return SFN_ERROR_OUT_OF_RANGE;

	Unit u;
	if(!args->GetUnit(1, u))
		return SFN_ERROR_TYPE_ARGS;

	index--;

	g_slicEngine->GetContext()->SetUnit(index, u);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetUnitByIndex::Call(SlicArgList *args)
{
	if(args->Count() != 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	Assert(index >= 1);
	if(index < 1)
		return SFN_ERROR_OUT_OF_RANGE;

	sint32 player;
	if(!args->GetInt(1, player))
		return SFN_ERROR_TYPE_ARGS;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	sint32 unitIndex;
	if(!args->GetInt(2, unitIndex))
		return SFN_ERROR_TYPE_ARGS;

	if(unitIndex < 0 || unitIndex >= g_player[player]->m_all_units->Num())
		return SFN_ERROR_OUT_OF_RANGE;

	index--;
	g_slicEngine->GetContext()->SetUnit(index, g_player[player]->m_all_units->Access(unitIndex));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetCity::Call(SlicArgList *args)
{
	if(args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	Assert(index >= 1);
	if(index < 1)
		return SFN_ERROR_OUT_OF_RANGE;

	Unit c;
	if(!args->GetCity(1, c))
		return SFN_ERROR_TYPE_ARGS;

	index--;

	g_slicEngine->GetContext()->SetCity(index, c);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetCityByIndex::Call(SlicArgList *args)
{
	if(args->Count() != 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	Assert(index >= 1);
	if(index < 1)
		return SFN_ERROR_OUT_OF_RANGE;

	sint32 player;
	if(!args->GetInt(1, player))
		return SFN_ERROR_TYPE_ARGS;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	sint32 cityIndex;
	if(!args->GetInt(2, cityIndex))
		return SFN_ERROR_TYPE_ARGS;

	if(cityIndex < 0 || cityIndex >= g_player[player]->m_all_cities->Num())
		return SFN_ERROR_OUT_OF_RANGE;

	index--;
	g_slicEngine->GetContext()->SetCity(index, g_player[player]->m_all_cities->Access(cityIndex));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetLocation::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	MapPoint point;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPos(1, point))
		return SFN_ERROR_TYPE_ARGS;

	g_slicEngine->GetContext()->SetLocation(index, point);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_MakeLocation::Call(SlicArgList *args)
{

    if(args->Count() != 3)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_INT_VAR)
		return SFN_ERROR_TYPE_ARGS;

	SlicSymbolData *sym = args->m_argValue[0].m_symbol;
	if(sym->GetType() == SLIC_SYM_STRUCT)
		sym = sym->GetStruct()->GetDataSymbol();

	if(sym->GetType() != SLIC_SYM_LOCATION)
		return SFN_ERROR_TYPE_ARGS;

	sint32 x, y;
	MapPoint point;
	if(!args->GetInt(1, x)) return SFN_ERROR_TYPE_ARGS;
	if(!args->GetInt(2, y)) return SFN_ERROR_TYPE_ARGS;

	long width,height;

	width=g_theWorld->GetXWidth();
	height=g_theWorld->GetYHeight();

	if(x < 0 || x >= width)
		return SFN_ERROR_OUT_OF_RANGE;

	if(y < 0 || y >= height)
		return SFN_ERROR_OUT_OF_RANGE;




	point.x = (sint16)x;
	point.y = (sint16)y;

	sym->SetPos(point);
	return SFN_ERROR_OK;

}

SFN_ERROR Slic_SetOrder::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	sint32 order;
	MapPoint location;

	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	if(index < 1)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!args->GetInt(1, order))
		return SFN_ERROR_TYPE_ARGS;

	index--;

	g_slicEngine->GetContext()->SetOrder(index, (UNIT_ORDER_TYPE)order);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Flood::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 stage;
	if(!args->GetInt(0, stage))
		return SFN_ERROR_TYPE_ARGS;

	g_theWorld->GlobalWarming(stage);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Ozone::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	g_theWorld->OzoneDepletion();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GodMode::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	extern sint32 g_god;
	extern sint32 g_fog_toggle;

	sint32 on;
	if(!args->GetInt(0, on))
		return SFN_ERROR_TYPE_ARGS;

	g_god = on;
	g_fog_toggle = on;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ExecuteAllOrders::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	if(g_selected_item && g_player && g_player[g_selected_item->GetCurPlayer()])
		g_player[g_selected_item->GetCurPlayer()]->ProcessUnitOrders();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CatchUp::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	g_director->CatchUp();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Deselect::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	if(g_selected_item && g_player && g_player[g_selected_item->GetVisiblePlayer()])
		g_selected_item->Deselect(g_selected_item->GetVisiblePlayer());
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Preference::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	char *prefname;
	if(!args->GetString(0, prefname))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_theProfileDB->GetValueByName(prefname);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetPreference::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	char *prefname;
	if(!args->GetString(0, prefname))
		return SFN_ERROR_TYPE_ARGS;

	sint32 val;
	if(!args->GetInt(1, val))
		return SFN_ERROR_TYPE_ARGS;

	g_theProfileDB->SetValueByName(prefname, val);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddMovement::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	sint32 amt;
	if(!args->GetInt(1, amt))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	u.SetMovementPoints(u.GetMovementPoints() + amt);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ToggleVeteran::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	sint32 on;
	if(!args->GetInt(1, on))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	if(on)
		u.SetFlag(k_UDF_IS_VET);
	else
		u.ClearFlag(k_UDF_IS_VET);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsVeteran::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	if(!u.Flag(k_UDF_IS_VET))
		m_result.m_int = 0;
	else
		m_result.m_int = 1;

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsCivilian::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	if(u.GetDBRec()->GetCanBeExpelled())
		m_result.m_int = 1;
	else
		m_result.m_int = 0;

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CantAttackUnit::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	sint32 on;
	if(!args->GetInt(1, on))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	if(on)
		u.SetFlag(k_UDF_IS_VET);
	else
		u.ClearFlag(k_UDF_IS_VET);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CantAttackCity::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	sint32 on;
	if(!args->GetInt(1, on))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_OK;

	if(on)
		city.SetFlag(k_UDF_CANT_BE_ATTACKED);
	else
		city.ClearFlag(k_UDF_CANT_BE_ATTACKED);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityCantRiotOrRevolt::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	sint32 on;
	if(!args->GetInt(1, on))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_OK;

	if(on)
		city.SetFlag(k_UDF_CANT_RIOT_OR_REVOLT);
	else
		city.ClearFlag(k_UDF_CANT_RIOT_OR_REVOLT);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SelectUnit::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	if(!g_theArmyPool->IsValid(u.GetArmy()))
		return SFN_ERROR_OK;

	g_selected_item->SetSelectUnit(u);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SelectCity::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_OK;

	g_selected_item->SetSelectCity(city);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CantEndTurn::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 onoff;
	if(!args->GetInt(0, onoff))
		return SFN_ERROR_TYPE_ARGS;

	extern sint32 g_cantEndTurn;
	g_cantEndTurn = onoff;

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Heal::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	u.SetHPToMax();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddGold::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 pl;
	if(!args->GetInt(0, pl))
		return SFN_ERROR_TYPE_ARGS;

	sint32 amt;
	if(!args->GetInt(1, amt))
		return SFN_ERROR_TYPE_ARGS;

	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player || !g_player[pl])
		return SFN_ERROR_DEAD_PLAYER;

	if(amt >= 0)
		g_player[pl]->m_gold->AddGold(amt);
	else
		g_player[pl]->m_gold->SubGold(-amt);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetActionKey::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	char *keystring;
	if(!args->GetString(1, keystring))
		return SFN_ERROR_TYPE_ARGS;

	if(keystring[0] != '^') {
		g_slicEngine->SetTriggerKey(index - 1, keystring[0]);
	} else {
		g_slicEngine->SetTriggerKey(index - 1, keystring[1] + 128);
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetCityByLocation::Call(SlicArgList *args)
{
	m_result.m_int = 0;

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	SlicSymbolData *sym = args->m_argValue[1].m_symbol;
	if(sym->GetType() != SLIC_SYM_CITY && sym->GetType() != SLIC_SYM_STRUCT) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(pos.x < 0 || pos.x >= g_theWorld->GetXWidth())
		return SFN_ERROR_OUT_OF_RANGE;

	if(pos.y < 0 || pos.y >= g_theWorld->GetYHeight())
		return SFN_ERROR_OUT_OF_RANGE;

	Cell *cell = g_theWorld->GetCell(pos);
	if(cell->GetCity().m_id == 0) {
		return SFN_ERROR_OK;
	}

	sym->SetCity(cell->GetCity());
	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetNeighbor::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 3)
		return SFN_ERROR_NUM_ARGS;

	MapPoint posIn;
	if(!args->GetPos(0, posIn))
		return SFN_ERROR_TYPE_ARGS;

	sint32 dir;
	if(!args->GetInt(1, dir))
		return SFN_ERROR_TYPE_ARGS;

	MapPoint posOut;
	if(args->m_argValue[2].m_type != SA_TYPE_INT_VAR)
		return SFN_ERROR_TYPE_ARGS;

	SlicSymbolData *sym = args->m_argValue[2].m_symbol;


	m_result.m_int = posIn.GetNeighborPosition((WORLD_DIRECTION)dir, posOut);
	sym->SetPos(posOut);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_DamageUnit::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	sint32 hp;
	if(!args->GetInt(1, hp))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(u))
		return SFN_ERROR_OK;

	u.DeductHP((double)hp);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsUnitInBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = 0;

	Unit city;
	sint32 index;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	PointerList<BuildNode>::Walker walk(city.AccessData()->GetCityData()->GetBuildQueue()->GetList());
	while(walk.IsValid()) {
		BuildNode *bn = walk.GetObj();
		if(bn->m_category == k_GAME_OBJ_TYPE_UNIT &&
		   bn->m_type == index) {
			m_result.m_int = 1;
			return SFN_ERROR_OK;
		}
		walk.Next();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsBuildingInBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = 0;

	Unit city;
	sint32 index;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	PointerList<BuildNode>::Walker walk(city.AccessData()->GetCityData()->GetBuildQueue()->GetList());
	while(walk.IsValid()) {
		BuildNode *bn = walk.GetObj();
		if(bn->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT &&
		   bn->m_type == index) {
			m_result.m_int = 1;
			return SFN_ERROR_OK;
		}
		walk.Next();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsWonderInBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = 0;

	Unit city;
	sint32 index;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	PointerList<BuildNode>::Walker walk(city.AccessData()->GetCityData()->GetBuildQueue()->GetList());
	while(walk.IsValid()) {
		BuildNode *bn = walk.GetObj();
		if(bn->m_category == k_GAME_OBJ_TYPE_WONDER &&
		   bn->m_type == index) {
			m_result.m_int = 1;
			return SFN_ERROR_OK;
		}
		walk.Next();
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsEndgameInBuildList::Call(SlicArgList *args)
{
	return SFN_ERROR_BAD_FUNCTION;
#if 0
// Endgame now building wonder
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = 0;

	Unit city;
	sint32 index;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	BuildNode *bn = city.AccessData()->GetCityData()->GetBuildQueue()->GetHead();
	while(bn) {
		if(bn->m_category == k_GAME_OBJ_TYPE_ENDGAME_OBJECT &&
		   bn->m_type == index) {
			m_result.m_int = 1;
			return SFN_ERROR_OK;
		}
	}
	return SFN_ERROR_OK;
#endif
}

SFN_ERROR Slic_IsUnitAtHead::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 blah = -1;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(city.AccessData()->GetCityData()->GetBuildQueue()->GetHead()) {
		BuildNode *bn = city.AccessData()->GetCityData()->GetBuildQueue()->GetHead();
		if(bn->m_category == k_GAME_OBJ_TYPE_UNIT) {
			blah = bn->m_type;
			m_result.m_int = blah;
			return SFN_ERROR_OK;
		}
	}
	m_result.m_int = blah;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsBuildingAtHead::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 blah = -1;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(city.AccessData()->GetCityData()->GetBuildQueue()->GetHead()) {
		BuildNode *bn = city.AccessData()->GetCityData()->GetBuildQueue()->GetHead();
		if(bn->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT) {
			blah = bn->m_type;
			m_result.m_int = blah;
			return SFN_ERROR_OK;
		}
	}
	m_result.m_int = blah;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsWonderAtHead::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 bleah = -1;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(city.AccessData()->GetCityData()->GetBuildQueue()->GetHead()) {
		BuildNode *bn = city.AccessData()->GetCityData()->GetBuildQueue()->GetHead();
		if(bn->m_category == k_GAME_OBJ_TYPE_WONDER) {
			bleah = bn->m_type;
			m_result.m_int = bleah;
			return SFN_ERROR_OK;
		}
	}
	m_result.m_int = bleah;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddUnitToBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theUnitDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	city.BuildUnit(type);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddBuildingToBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theBuildingDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	city.BuildImprovement(type);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddWonderToBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theWonderDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	city.BuildWonder(type);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddEndgameToBuildList::Call(SlicArgList *args)
{
	return SFN_ERROR_BAD_FUNCTION;
#if 0
// Endgame now building wonder
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theEndGameDB->m_nRec)
		return SFN_ERROR_OUT_OF_RANGE;

	city.BuildEndGame(type);
	return SFN_ERROR_OK;
#endif
}

SFN_ERROR Slic_KillUnitFromBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theUnitDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	BuildQueue *bq = city.GetData()->GetCityData()->GetBuildQueue();
	BuildNode *bn;

	PointerList<BuildNode>::Walker walk(bq->GetList());
	while(walk.IsValid()) {
		bn = walk.GetObj();
		walk.Next();

		if(bn->m_category == k_GAME_OBJ_TYPE_UNIT &&
		   bn->m_type == type) {
			bq->RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_MANUAL);
		}
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_KillBuildingFromBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theBuildingDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	BuildQueue *bq = city.GetData()->GetCityData()->GetBuildQueue();
	BuildNode *bn;
	PointerList<BuildNode>::Walker walk(bq->GetList());
	while(walk.IsValid()) {
		bn = walk.GetObj();
		walk.Next();

		if(bn->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT &&
		   bn->m_type == type) {
			bq->RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_MANUAL);
		}
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_KillWonderFromBuildList::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theWonderDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	BuildQueue *bq = city.GetData()->GetCityData()->GetBuildQueue();
	BuildNode *bn;

	PointerList<BuildNode>::Walker walk(bq->GetList());
	while(walk.IsValid()) {
		bn = walk.GetObj();
		walk.Next();

		if(bn->m_category == k_GAME_OBJ_TYPE_WONDER &&
		   bn->m_type == type) {
			bq->RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_MANUAL);
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_KillEndgameFromBuildList::Call(SlicArgList *args)
{
	return SFN_ERROR_BAD_FUNCTION;
#if 0
// Endgame now building wonder
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	sint32 type;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city))
		return SFN_ERROR_TYPE_ARGS;

	if(type < 0 || type >= g_theEndGameDB->m_nRec)
		return SFN_ERROR_OUT_OF_RANGE;

	BuildQueue *bq = city.GetData()->GetCityData()->GetBuildQueue();
	BuildNode *bn;
	PointerList<BuildNode>::Walker walk(bq->GetList());
	while(walk.IsValid()) {
		bn = walk.GetObj();
		walk.Next();

		if(bn->m_category == k_GAME_OBJ_TYPE_ENDGAME_OBJECT &&
		   bn->m_type == type) {
			bq->RemoveNode(bn, CAUSE_REMOVE_BUILD_ITEM_MANUAL);
		}
	}
	return SFN_ERROR_OK;
#endif
}

SFN_ERROR Slic_SetPW::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player, amount;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, amount))
		return SFN_ERROR_TYPE_ARGS;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	g_player[player]->m_materialPool->SetLevel(amount);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Stacked::Call(SlicArgList *args)
{
	m_result.m_int = 0;

	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	CellUnitList *units = g_theWorld->GetCell(pos)->UnitArmy();
	if(!units || units->Num() < 1)
		return SFN_ERROR_OK;

	Army army = units->Access(0).GetArmy();
	sint32 i;
	for(i = 1; i < units->Num(); i++) {
		if(units->Access(i).GetArmy().m_id != army.m_id)
			return SFN_ERROR_OK;
	}
	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetString::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_STRING)
		return SFN_ERROR_TYPE_ARGS;

	const MBCHAR *str = g_theStringDB->GetNameStr(args->m_argValue[1].m_int);
	if(!str)
		return SFN_ERROR_INTERNAL;

	g_slicEngine->GetContext()->SetAction(index, str);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetStringByDBIndex::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	sint32 stringId;
	if(!args->GetInt(1, stringId))
		return SFN_ERROR_TYPE_ARGS;

	const MBCHAR *str = g_theStringDB->GetNameStr(stringId);
	if(!str)
		return SFN_ERROR_OUT_OF_RANGE;

	index--;

	g_slicEngine->GetContext()->SetAction(index, str);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetStringDBIndex::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	if(args->m_argValue[0].m_type != SA_TYPE_STRING)
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = args->m_argValue[0].m_int;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_UnitHasUserFlag::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit u;

	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	sint32 flag;
	if(!args->GetInt(1, flag))
		return SFN_ERROR_TYPE_ARGS;

	if(flag < 1 || flag > 32)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = 0;

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_BlankScreen::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 blank;
	if(!args->GetInt(0, blank))
		return SFN_ERROR_TYPE_ARGS;

	g_slicEngine->BlankScreen(blank != 0);
	if(!blank && g_selected_item) {
		g_selected_item->KeyboardSelectFirstUnit();
		if(g_selected_item->GetState() != SELECT_TYPE_LOCAL_ARMY &&
		   (g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Num() > 0)) {
			g_selected_item->SetSelectCity(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0));
			g_director->AddCenterMap(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0).RetPos());
		}
		g_director->AddCenterMap(g_selected_item->GetCurSelectPos());
		radarwindow_Show();
		g_controlPanel->Show();
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddCenter::Call(SlicArgList *args)
{

	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	g_director->AddCenterMap(pos);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddEffect::Call(SlicArgList *args)
{

	if(args->Count() != 3)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	MBCHAR *effectName;
	MBCHAR *soundName;
	sint32 effectId;
	sint32 soundId;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetString(1, effectName))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetString(2, soundName))
		return SFN_ERROR_TYPE_ARGS;

	sint32 typeIndex = g_theSpecialEffectDB->FindTypeIndex(effectName);
	if(typeIndex < 0) {
		return SFN_ERROR_EFFECT_NOT_FOUND;
	}

	effectId = g_theSpecialEffectDB->Get(typeIndex)->GetValue();
	if(effectId < 0)
		return SFN_ERROR_EFFECT_NOT_FOUND;

	soundId = g_theSoundDB->FindTypeIndex(soundName);
	if(soundId < 0)
		return SFN_ERROR_SOUND_NOT_FOUND;

	g_director->AddSpecialEffect(pos, effectId, soundId);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerCivilization::Call(SlicArgList *args)
{
	m_result.m_int = 0;

	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	m_result.m_int = g_player[player]->m_civilisation->GetCivilisation();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CivilizationIndex::Call(SlicArgList *args)
{
	m_result.m_int = -1;
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	char *civName;
	if(!args->GetString(0, civName))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_theCivilisationDB->FindRecordNameIndex(civName);
	if(m_result.m_int > -1){
		return SFN_ERROR_OK;
	}

	return SFN_ERROR_CIV_NOT_FOUND;
}

SFN_ERROR Slic_ExitToDesktop::Call(SlicArgList *args)
{
	ExitGame();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Import::Call(SlicArgList *args)
{
	char *filename;
	if(!args->GetString(0, filename))
		return SFN_ERROR_TYPE_ARGS;
	MapFile mf;

	mf.Load(filename);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_Export::Call(SlicArgList *args)
{
	char *filename;
	if(!args->GetString(0, filename))
		return SFN_ERROR_TYPE_ARGS;

	MapFile mf;
	mf.Save(filename);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetUnitFromArmy::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	Army a;
	sint32 index;

	if(!args->GetArmy(0, a))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(args->Count() < 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	if(args->m_argValue[2].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(index < 0 || index >= a.Num()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	SlicSymbolData *sym = args->m_argValue[2].m_symbol;
	sym->SetUnit(a[index]);
	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetArmyFromUnit::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	Unit u;

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[1].m_type != SA_TYPE_INT_VAR)
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theArmyPool->IsValid(u.GetArmy()))
		return SFN_ERROR_OK;

	SlicSymbolData *sym = args->m_argValue[1].m_symbol;
	sym->SetArmy(u.GetArmy());

	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetUnitByIndex::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 player;
	if(!args->GetInt(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	sint32 index;
	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[2].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(index < 0 || index >= g_player[player]->m_all_units->Num()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	SlicSymbolData *sym = args->m_argValue[2].m_symbol;
	sym->SetUnit(g_player[player]->m_all_units->Access(index));
	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetArmyByIndex::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 player;
	if(!args->GetInt(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	sint32 index;
	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[2].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(index < 0 || index >= g_player[player]->m_all_armies->Num()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	SlicSymbolData *sym = args->m_argValue[2].m_symbol;
	sym->SetArmy(g_player[player]->m_all_armies->Access(index));

	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetCityByIndex::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	sint32 player;
	if(!args->GetInt(0, player))
		return SFN_ERROR_TYPE_ARGS;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[player])
		return SFN_ERROR_DEAD_PLAYER;

	sint32 index;
	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[2].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(index < 0 || index >= g_player[player]->m_all_cities->Num()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	SlicSymbolData *sym = args->m_argValue[2].m_symbol;
	sym->SetCity(g_player[player]->m_all_cities->Access(index));

	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetUnitsAtLocation::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 1) {
		return SFN_ERROR_NUM_ARGS;
	}

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	if(pos.x < 0 || pos.x >= g_theWorld->GetXWidth())
		return SFN_ERROR_OUT_OF_RANGE;

	if(pos.y < 0 || pos.y >= g_theWorld->GetYHeight())
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = g_theWorld->GetCell(pos)->GetNumUnits();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetUnitFromCell::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 index;
	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(args->m_argValue[2].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	Cell *cell = g_theWorld->GetCell(pos);

	if(index < 0 || index >= cell->GetNumUnits()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	SlicSymbolData *sym = args->m_argValue[2].m_symbol;
	sym->SetUnit(cell->AccessUnit(index));
	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsRoad::Call(SlicArgList *args)
	{

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;
	sint32 owner;
	if(!args->GetPlayer(0, owner))
		return SFN_ERROR_TYPE_ARGS;
	MapPoint pos;
	if(!args->GetPos(1, pos))
		return SFN_ERROR_TYPE_ARGS;

	if (g_theWorld->IsRoad(0,pos)&&
		(g_theWorld->GetOwner(pos)==owner))
	  m_result.m_int = 1;
	else
	  m_result.m_int = 0;
	return SFN_ERROR_OK;
	}

SFN_ERROR Slic_GetMapHeight::Call(SlicArgList *args)
	{

	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = g_theWorld->GetYHeight();
	return SFN_ERROR_OK;
	}

SFN_ERROR Slic_GetMapWidth::Call(SlicArgList *args)
	{

	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = g_theWorld->GetXWidth();
	return SFN_ERROR_OK;
	}

SFN_ERROR Slic_AddFeat::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 feat, player;

	if(!args->GetInt(0, feat))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, player))
		return SFN_ERROR_TYPE_ARGS;

	if(feat < 0 || feat >= g_theFeatDB->NumRecords()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AccomplishFeat,
						   GEA_Int, feat,
						   GEA_Player, player,
						   GEA_End);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_IsFortress::Call(SlicArgList *args)
	{

	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;
	sint32 owner;
	if(!args->GetPlayer(0, owner))
		return SFN_ERROR_TYPE_ARGS;
	MapPoint pos;
	if(!args->GetPos(1, pos))
		return SFN_ERROR_TYPE_ARGS;

    m_result.m_int = 0;

	Cell *cell = g_theWorld->GetCell(pos);

	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);

		Assert(rec);
		if(rec) {

			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(rec, pos);
			Assert(eff);
			if(eff) {
				double bonus;
				if(eff->GetDefenseBonus(bonus)) {

					m_result.m_int = 1;
				}
			}
		}
	}

	return SFN_ERROR_OK;
	}

SFN_ERROR Slic_Distance::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint p1, p2;
	if(!args->GetPos(0, p1))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPos(1, p2))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = static_cast<sint32>
		(sqrt(static_cast<double>(MapPoint::GetSquaredDistance(p1, p2))));
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SquaredDistance::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint p1, p2;
	if(!args->GetPos(0, p1))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPos(1, p2))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = MapPoint::GetSquaredDistance(p1, p2);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetCurrentRound::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = NewTurnCount::GetCurrentRound();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetCurrentYear::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = NewTurnCount::GetCurrentYear();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityIsValid::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	m_result.m_int = args->GetCity(0, city) && city.IsValid() && city.CD();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CellOwner::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	Cell *cell = g_theWorld->GetCell(pos);
	if(!cell) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	m_result.m_int = cell->GetOwner();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityIsNamed::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;

	if(!args->GetCity(0, city)) {
		//Changed by Martin Gühmann
		return SFN_ERROR_TYPE_ARGS;
	//	return SFN_ERROR_OK;
	}

	char *city_name;

	if(!args->GetString(1, city_name))
		return SFN_ERROR_TYPE_ARGS;

	if(strcmp(city.AccessData()->GetCityData()->GetName(), city_name) == 0)
		m_result.m_int = 1;
	else
		m_result.m_int = 0;

	return SFN_ERROR_OK;
}






SFN_ERROR Slic_StringCompare::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	//Added by Martin Gühmann to allow string comparision, between string IDs and plain strings
	char *string1 = 0;
	char *string2 = 0;

	const char *cstring1, *cstring2;
	StringId stringId1, stringId2;

	if(!args->GetString(0, string1)){
		if(args->GetStringId(0, stringId1)){
			cstring1 = g_theStringDB->GetNameStr(stringId1);
		}
		else if(args->m_argValue[0].m_type == SA_TYPE_INT_VAR){
			cstring1 = g_theStringDB->GetNameStr(args->m_argValue[0].m_symbol->GetStringId());
		}
		else{
			return SFN_ERROR_TYPE_ARGS;
		}
		string1 = const_cast<char *>(cstring1);
	}

	if(!args->GetString(1, string2)){
		if(args->GetStringId(1, stringId2)){
			cstring2 = g_theStringDB->GetNameStr(stringId2);
		}
		else if(args->m_argValue[1].m_type == SA_TYPE_INT_VAR){
			cstring2 = g_theStringDB->GetNameStr(args->m_argValue[1].m_symbol->GetStringId());
		}
		else{
			return SFN_ERROR_TYPE_ARGS;
		}
		string2 = const_cast<char *>(cstring2);
	}

	if ( !stricmp(string1, string2) )
	{
		m_result.m_int = 1;
	}
	else {
		m_result.m_int = 0;
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_CityNameCompare::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	StringId stringId;
	if(!args->GetStringId(1, stringId))
		return SFN_ERROR_TYPE_ARGS;

	const MBCHAR *str = g_theStringDB->GetNameStr(stringId);
	if(!str)
		return SFN_ERROR_OUT_OF_RANGE;

	if (!stricmp(city.GetName(), str))
		m_result.m_int = 1;
	else
		m_result.m_int = 0;

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ChangeGlobalRegard::Call(SlicArgList *args)
{
	if (args->Count() < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	sint32 delta;
	if(!args->GetInt(1, delta))
		return SFN_ERROR_TYPE_ARGS;

	StringId explain;
	if(!args->GetStringId(2, explain))
		return SFN_ERROR_TYPE_ARGS;

	sint32 duration;
	if(!args->GetInt(3, duration))
		duration = -1;

	for (int i = 0; i < k_MAX_PLAYERS; i++)
	{
		if ( (g_player[i]) && (i != player) )
		{
			Diplomat::GetDiplomat(i).LogRegardEvent(
						player,
						delta,
						REGARD_EVENT_SCENARIO,
						explain,
						static_cast<sint16>(duration));
		}
	}

	return SFN_ERROR_OK;

}


SFN_ERROR Slic_SetAllCitiesVisible::Call(SlicArgList *args)
{
	if (args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	for (int i = 1; i < k_MAX_PLAYERS; i++)
	{
		if(g_player[i])
		{
			UnitDynamicArray * cityList = g_player[i]->GetAllCitiesList();
			int maxCity = cityList->Num();
			for (int j = 0; j < maxCity; j++)
			{
				cityList->Access(j).SetVisible(player);
			}
		}
	}
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_SetCityVisible::Call(SlicArgList *args)
{
	if (args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

	StringId stringId;
	if(!args->GetStringId(1, stringId))
		return SFN_ERROR_TYPE_ARGS;

	const MBCHAR *str = g_theStringDB->GetNameStr(stringId);
	if(!str)
		return SFN_ERROR_OUT_OF_RANGE;

	for (int i = 1; i < k_MAX_PLAYERS; i++)
	{
		if (g_player[i])
		{
			UnitDynamicArray * cityList = g_player[i]->GetAllCitiesList();
			int maxCity = cityList->Num();
			for (int j = 0; j < maxCity; j++)
			{
				Unit city = cityList->Access(j);
				if ( !stricmp(city.GetName(), str) )
				{
					city.SetVisible(player);

					return SFN_ERROR_OK;
				}
			}
		}
	}

	return SFN_ERROR_OUT_OF_RANGE;
}

SFN_ERROR Slic_FinishImprovements::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	Cell *cell = g_theWorld->GetCell(pos);

	g_gevManager->Pause();

	for (sint32 i = 0; i < cell->GetNumImprovements(); i++)
    {
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_ImprovementComplete,
							   GEA_Improvement, cell->AccessImprovement(i),
							   GEA_End);
	}
	g_gevManager->Resume();

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_RemoveAdvance::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 pl, ad;
	if(!args->GetPlayer(0, pl))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, ad))
		return SFN_ERROR_TYPE_ARGS;

	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[pl]) {
		return SFN_ERROR_DEAD_PLAYER;
	}

	g_player[pl]->m_advances->TakeAdvance(ad);
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_PlayerGold::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 pl;
	if(!args->GetPlayer(0, pl))
		return SFN_ERROR_TYPE_ARGS;

	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[pl]) {
		return SFN_ERROR_DEAD_PLAYER;
	}

	m_result.m_int = g_player[pl]->m_gold->GetLevel();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_ClearBattleFlag::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit u;
	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if (u.IsValid() && u.Flag(k_UDF_FOUGHT_THIS_TURN))
    {
		u.ClearFlag(k_UDF_FOUGHT_THIS_TURN);
		u.SetMovementPoints(u.GetMaxMovePoints());
	}

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenScenarioEditor::Call(SlicArgList *args)
{
	//Wrong number of arguments added by Martin Gühmann
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	open_ScenarioEditor();
	return SFN_ERROR_OK;
}

//New Slic functions of CTP2.1 readded by Martin Gühmann

SFN_ERROR Slic_DestroyBuilding::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_BUILTIN;

	sint32 building;
	if(!args->GetInt(1, building))
		return SFN_ERROR_TYPE_ARGS;

	g_gevManager->Pause();
	city->GetCityData()->DestroyImprovement(building);
	g_gevManager->Resume();
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_OpenBuildQueue::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_BUILTIN;

	EditQueue::Display(city->GetCityData());
	return SFN_ERROR_OK;
}

SFN_ERROR Slic_TileHasImprovement::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 imp;
	if(!args->GetInt(1, imp))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = 0;
	Cell *cell = g_theWorld->GetCell(pos);
	for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {

		if(imp == cell->GetDBImprovement(i)){
			m_result.m_int = 1;
			return SFN_ERROR_OK;
		}
	}
	return SFN_ERROR_OK;
}

// JJB filled this function
SFN_ERROR Slic_PlayerHasWonder::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 pl;
	if(!args->GetPlayer(0, pl))
		return SFN_ERROR_TYPE_ARGS;

	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player[pl]) {
		return SFN_ERROR_DEAD_PLAYER;
	}

	sint32 wonder;

	if(!args->GetInt(1, wonder)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	m_result.m_int = (pl == wonderutil_GetOwner(wonder));

	return SFN_ERROR_OK;
}

// JJB filled this function
SFN_ERROR Slic_WonderOwner::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 wonder;

	if(!args->GetInt(0, wonder)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	sint32 owner = wonderutil_GetOwner(wonder);

	if (owner == PLAYER_INDEX_INVALID)
		m_result.m_int = -1;
	else
		m_result.m_int = owner;

	return SFN_ERROR_OK;
}

// JJB filled this function
SFN_ERROR Slic_CityHasWonder::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	Unit city;

	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_BUILTIN;

	sint32 wonder;

	if(!args->GetInt(1, wonder)) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if (city.GetCityData()->GetBuiltWonders() & ((uint64)1 << wonder))
    {
		m_result.m_int = 1;
	}
    else
    {
        m_result.m_int = 0;
    }

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Name       : Slic_ArmyIsValid
//
// Description: Determine whether an army is valid.
//
// Parameters : args	: army variable
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
// Remark(s)  : Fills m_result.m_int with an indication whether the army is
//              valid. When the army is valid, 1 (true) is returned. When the
//              army is invalid, 0 (false) is returned.
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_ArmyIsValid::Call(SlicArgList *args)
{
	m_result.m_int = 0;

	if (args->Count() != 1)
	{
		return SFN_ERROR_NUM_ARGS;
	}

	Army	a;
	m_result.m_int = args->GetArmy(0, a) && a.IsValid();

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Name       : Slic_GetRoundsToNextDisaster
//
// Description: Get estimated number of rounds until next disaster.
//
// Parameters : -
//
// Globals    : g_thePollution	: pollution information
//
// Returns    : SFN_ERROR		: execution result
//
// Remark(s)  : Fills m_result.m_int with the requested data. When no pollution
//              information is available, ROUNDS_COUNT_IMMEASURABLE (no
//              disaster expected for a long time) is returned.
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_GetRoundsToNextDisaster::Call(SlicArgList *args)
{
	if(args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

	m_result.m_int = (g_thePollution)
                     ? g_thePollution->GetRoundsToNextDisaster()
                     : Pollution::ROUNDS_COUNT_IMMEASURABLE;

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Name       : Slic_GetCurrentPollutionLevel
//
// Description: Get current pollution level.
//
// Parameters : -
//
// Globals    : g_thePollution	: pollution information
//
// Returns    : SFN_ERROR		: execution result
//
// Remark(s)  : Fills m_result.m_int with the requested data. When no pollution
//              information is available, 0 (no pollution) is returned.
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_GetCurrentPollutionLevel::Call(SlicArgList *args)
{
	if (args->Count() != 0)
		return SFN_ERROR_NUM_ARGS;

    m_result.m_int =
        (g_thePollution) ? g_thePollution->GetGlobalPollutionLevel() : 0;

	return SFN_ERROR_OK;
}

//Added by The Big MC
SFN_ERROR Slic_FreeAllSlaves::Call(SlicArgList *args)
{
	if (args->Count() > 0)
		return SFN_ERROR_NUM_ARGS;

	Unit city = g_slicEngine->GetContext()->GetCity(0);
	if (!city.IsValid())
    {
		return SFN_ERROR_OK;
	}

	if(g_network.IsClient())
	{
		g_network.SendAction(new NetAction(NET_ACTION_FREE_SLAVES, city.m_id));
	}

	city.FreeSlaves();
	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : MrBaggins
//
// Name       : Slic_PlantSpecificGood
//
// Description: Rather than planting a random good, as per the standard SLIC
//              function, this allows the specification of which good subtype
//              to plant
//
// Parameters : SlicArg 0: location
//              SlicArg 1: int
//
// Globals    : g_theWorld	: World gamestate functionality
//                   g_tiledMap  : UI update functionality
//
// Returns    : SFN_ERROR		: execution result
//
// Remark(s)  : Allows for the specification of a good sub
//		a range of 0 (no good), or 1-4 (the specific good subtype,
//		as defined in ctp2_data\default\gamedata\goods.txt//
//----------------------------------------------------------------------------
SFN_ERROR Slic_PlantSpecificGood::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	sint32 goodsubtype;
	if(!args->GetInt(1, goodsubtype))
		return SFN_ERROR_TYPE_ARGS;

	if(goodsubtype<0 || goodsubtype>4)
		return SFN_ERROR_OUT_OF_RANGE;

	g_theWorld->SetGood(pos.x, pos.y, goodsubtype);
	g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
	g_tiledMap->TileChanged(pos);
	g_tiledMap->RedrawTile(&pos);

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : MrBaggins
//
// Name       : Slic_RemoveGood
//
// Description: New complimentary function to PlantGood
//
// Parameters : SlicArg 0: location
//
// Globals    : g_theWorld	: World gamestate functionality
//				g_tiledMap  : UI update functionality
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_RemoveGood::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	g_theWorld->SetGood(pos.x, pos.y, 0);
	// plants a good of subtype 0 AKA no good, at location x,y.
	g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
	g_tiledMap->TileChanged(pos);
	g_tiledMap->RedrawTile(&pos);

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Martin Gühmann
//
// Name       : Slic_CargoCapacity
//
// Description: New function to figure out how much space is left for
//              cargo units.
//
// Parameters : SlicArg 0: unit
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_CargoCapacity::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	m_result.m_int = unit.GetCargoCapacity();

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Martin Gühmann
//
// Name       : Slic_MaxCargoSize
//
// Description: New function to figure out how much cargo a unit can carry.
//
// Parameters : SlicArg 0: unit
//
// Globals    : g_theUnitDB
//
// Returns    : SFN_ERROR        : execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_MaxCargoSize::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	if (unit.GetDBRec()->GetCargoDataPtr()){
		m_result.m_int = unit.GetDBRec()->GetCargoDataPtr()->GetMaxCargo();
	}
	else{
		m_result.m_int = 0;
	}

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Martin Gühmann
//
// Name       : Slic_CargoSize
//
// Description: New function to figure out how much cargo a unit is carrying.
//
// Parameters : SlicArg 0: unit
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_CargoSize::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	m_result.m_int = unit.GetNumCarried();

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Martin Gühmann
//
// Name       : Slic_GetUnitFromCargo
//
// Description: New function to figure out how much cargo a unit can carry.
//
// Parameters : SlicArg 0: unit
//              SlicArg 1: int
//              SlicArg 2: unit
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_GetUnitFromCargo::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	Unit u;
	sint32 index;

	if(!args->GetUnit(0, u))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(1, index))
		return SFN_ERROR_TYPE_ARGS;

	if(args->Count() < 3) {
		return SFN_ERROR_NUM_ARGS;
	}

	if(args->m_argValue[2].m_type != SA_TYPE_INT_VAR) {
		return SFN_ERROR_TYPE_ARGS;
	}

	if(index < 0 || index >= u.GetNumCarried()) {
		return SFN_ERROR_OUT_OF_RANGE;
	}

	SlicSymbolData *sym = args->m_argValue[2].m_symbol;
	Unit u2 = u.GetData()->GetCargoList()->Get(index);
	sym->SetUnit(u2);
	m_result.m_int = 1;
	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Martin Gühmann
//
// Name       : Slic_GetContinent
//
// Description: Gets the continent ID of a location.
//
// Parameters : SlicArg 0: location
//
// Globals    : g_theWorld
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_GetContinent::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_theWorld->GetContinent(pos);

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Martin Gühmann
//
// Name       : Slic_GetContinentSize
//
// Description: Get the size of position's continent.
//
// Parameters : SlicArg 0: location
//
// Globals    : g_theWorld
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_GetContinentSize::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	if(g_theWorld->IsWater(pos)){
		m_result.m_int = g_theWorld->GetWaterContinentSize(g_theWorld->GetContinent(pos));
	}
	else{
		m_result.m_int = g_theWorld->GetLandContinentSize(g_theWorld->GetContinent(pos));
	}

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Martin Gühmann
//
// Name       : Slic_IsWater
//
// Description: Gets whether this location is water.
//
// Parameters : SlicArg 0: location
//
// Globals    : g_theWorld
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_IsWater::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = g_theWorld->IsWater(pos);

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Solver
//
// Name       : Slic_IsOnSameContinent
//
// Description: Checks if two positions are at same continent.
//
// Parameters : SlicArg 0,1: locations
//
// Globals    : g_theWorld
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_IsOnSameContinent::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	MapPoint pos;
	if(!args->GetPos(0, pos))
		return SFN_ERROR_TYPE_ARGS;

	MapPoint pos2;
	if (!args->GetPos(1, pos2))
		return SFN_ERROR_TYPE_ARGS;

    m_result.m_int = g_theWorld->IsOnSameContinent(pos, pos2);

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddSlaves::Call(SlicArgList *args)
{
	if(args->Count() != 3)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city))
		return SFN_ERROR_TYPE_ARGS;

	sint32 count;
	if(!args->GetInt(1, count))
		return SFN_ERROR_TYPE_ARGS;

	sint32 victim;
	if(!args->GetPlayer(2, victim))
		return SFN_ERROR_TYPE_ARGS;

	MapPoint pos;
	city.GetPos(pos);

	sint32 i;
	if(count > 0) {
		for (i=0;  i<count; i++) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
						   GEA_City, city.m_id,
						   GEA_Player, victim,
						   GEA_End);

		}
	} else {
		// Add code to convert count slaves to normal pops here later.
		for(i = count; i < 0; i++) {
			// Add pop remove code here later (KillPop event).
		}
	}

	sint32 delta_martial_law;
	CityData *cd = city.GetData()->GetCityData();
	cd->GetHappy()->CalcHappiness(*cd, FALSE, delta_martial_law, TRUE);

	return SFN_ERROR_OK;
}
//EMODs
SFN_ERROR Slic_KillCity::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

	Unit city = g_slicEngine->GetContext()->GetCity(0);
	if (!city.IsValid())
    {
		return SFN_ERROR_OK;
	}

	if(!city.GetData()->GetCityData()->CapturedThisTurn()) {
		if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_PROBABLE_CHEATER);
		}
		return SFN_ERROR_OK;
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_FREE_SLAVES, city.m_id));
	}
	CityData *cd = city.GetData()->GetCityData();
	sint32 PCount = cd->PopCount();
	city.GetData()->GetCityData()->ChangePopulation(-PCount);
	//city.GetData()->GetCityData()->Disband();

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_Pillage::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

	Unit city = g_slicEngine->GetContext()->GetCity(0);

	if (!city.IsValid())
    {
		return SFN_ERROR_OK;
	}

	if(!city.GetData()->GetCityData()->CapturedThisTurn()) {
		if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_PROBABLE_CHEATER);
		}
		return SFN_ERROR_OK;
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_FREE_SLAVES, city.m_id));
	}

	sint32 pl;
	if(!args->GetInt(0, pl))
		return SFN_ERROR_TYPE_ARGS;

	sint32 amt;
	if(!args->GetInt(1, amt))
		return SFN_ERROR_TYPE_ARGS;

	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player || !g_player[pl])
		return SFN_ERROR_DEAD_PLAYER;

	sint32 p = 0;
	uint64 buildings = city->GetCityData()->GetEffectiveBuildings()&(((uint64)1<<(uint64)g_theBuildingDB->NumRecords())-1);
	for(sint32 i=0; buildings!=0; i++,buildings>>=1)
	{

		if ((buildings&0xFF) == 0) {
			buildings>>=8;
			i+=8;
		}

		if (buildings&1)
		{

			p += buildingutil_GetProductionCost(i, city->GetOwner());
			city->GetCityData()->DestroyImprovement(i);
		}
	}

	amt = p / g_theGovernmentDB->Get(g_player[pl]->m_government_type)->GetBuildingRushModifier();
	if (amt >= 0)
		g_player[pl]->m_gold->AddGold(amt);

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_Plunder::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

	Unit city = g_slicEngine->GetContext()->GetCity(0);
	if (!city.IsValid())
    {
		return SFN_ERROR_OK;
	}

	sint32 amt;
	if(!args->GetInt(1, amt))
		return SFN_ERROR_TYPE_ARGS;

	sint32 pl;
	if(!args->GetInt(0, pl))
		return SFN_ERROR_TYPE_ARGS;

	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player || !g_player[pl])
		return SFN_ERROR_DEAD_PLAYER;

	if(!city.GetData()->GetCityData()->CapturedThisTurn()) {
		if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_PROBABLE_CHEATER);
		}
		return SFN_ERROR_OK;
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_FREE_SLAVES, city.m_id));
	}

	sint32 p = 0;
	uint64 buildings = city->GetCityData()->GetEffectiveBuildings()&(((uint64)1<<(uint64)g_theBuildingDB->NumRecords())-1);
	for(sint32 i=0; buildings!=0; i++,buildings>>=1)
	{

		if ((buildings&0xFF) == 0) {
			buildings>>=8;
			i+=8;
		}

		if (buildings&1)
		{
			p += buildingutil_GetProductionCost(i, city->GetOwner());
			city->GetCityData()->DestroyImprovement(i);
		}
	}

	amt = p / g_theGovernmentDB->Get(g_player[pl]->m_government_type)->GetBuildingRushModifier();

    if(amt >= 0)
		g_player[pl]->m_materialPool->AddMaterials(amt);

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_Liberate::Call(SlicArgList *args)
{
    if (args->Count() > 0)
        return SFN_ERROR_NUM_ARGS;

	Unit city = g_slicEngine->GetContext()->GetCity(0);

	if (!city.IsValid())
    {
		return SFN_ERROR_OK;
	}

	if(!city.GetData()->GetCityData()->CapturedThisTurn()) {
		if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_PROBABLE_CHEATER);
		}
		return SFN_ERROR_OK;
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_FREE_SLAVES, city.m_id));
	}

	//if(!args->GetInt(0, cause))
	//	return GEV_HD_Continue;
    
    city.ResetCityOwner(PLAYER_INDEX_VANDALS, FALSE, CAUSE_REMOVE_CITY_DIPLOMACY); // must be before sending armies home otherwise NearestFriendlyCityWithRoom returns the current city

    //// send conquering armies home, based on code from ArmyData::Expel
    Cell *cell = g_theWorld->GetCell(city.RetPos());
    sint32 i, n = cell->GetNumUnits();

    bool 		foundCity	= false;
    MapPoint 		cpos;
    CellUnitList 	expelled;
    for(i = 0; i < n; i++) {
	    Unit u = cell->AccessUnit(i);	    
	    foundCity = u.NearestFriendlyCityWithRoom(cpos, n, u.GetArmy());
	    expelled.Insert(u);
	}

    n = expelled.Num();
    if(n > 0) {
	for(i = 0; i < n; i++) {
	    if(foundCity) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_GetExpelledOrder,
		    GEA_Army, expelled[i].GetArmy(),
		    GEA_MapPoint, cpos,
		    GEA_Player, PLAYER_INDEX_VANDALS,
		    GEA_End);
		}
	    else {
		expelled[i].Kill(CAUSE_REMOVE_ARMY_EXPELLED_NO_CITIES, PLAYER_INDEX_VANDALS);
		}
	    }
	}

    return SFN_ERROR_OK;
}

SFN_ERROR Slic_AddPW::Call(SlicArgList *args)
{
	if(args->Count() != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 pl;
	if(!args->GetInt(0, pl))
		return SFN_ERROR_TYPE_ARGS;

	sint32 amt;
	if(!args->GetInt(1, amt))
		return SFN_ERROR_TYPE_ARGS;

	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	if(!g_player || !g_player[pl])
		return SFN_ERROR_DEAD_PLAYER;

	g_player[pl]->m_materialPool->AddMaterials(amt);

	return SFN_ERROR_OK;
}
//----------------------------------------------------------------------------
//
// Authored   : Maq
//
// Name       : Slic_CreateBuilding
//
// Description: Function to create a building in a city.
//
// Parameters : SlicArg 0: city
//				SlicArg 1: buildingdb index
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_CreateBuilding::Call(SlicArgList *args)
{
	if (args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	Unit city;
	sint32 type;
	if(!args->GetCity(0, city)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city)) {
		return SFN_ERROR_OK;
	}

	if(type < 0 || type >= g_theBuildingDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	city.CD()->AddImprovement(type);
	return SFN_ERROR_OK;

}
//----------------------------------------------------------------------------
//
// Authored   : Maq
//
// Name       : Slic_CreateWonder
//
// Description: Function to create a wonder in a city.
//
// Parameters : SlicArg 0: city
//				SlicArg 1: wonderdb index
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_CreateWonder::Call(SlicArgList *args)
{
	if (args->Count() != 2) {
		return SFN_ERROR_NUM_ARGS;
	}

	Unit city;
	sint32 type;
	if(!args->GetCity(0, city)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	if(!args->GetInt(1, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!g_theUnitPool->IsValid(city)) {
		return SFN_ERROR_OK;
	}

	if(type < 0 || type >= g_theWonderDB->NumRecords())
		return SFN_ERROR_OUT_OF_RANGE;

	city.CD()->AddWonder(type);
	return SFN_ERROR_OK;

}
//----------------------------------------------------------------------------
//
// Authored   : Maq
//
// Name       : Slic_UnitMovementLeft
//
// Description: Function to figure out how much movement is left for
//              a unit.
//
// Parameters : SlicArg 0: unit
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_UnitMovementLeft::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit unit;
	if(!args->GetUnit(0, unit)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	m_result.m_int = unit.GetMovementPoints();

	return SFN_ERROR_OK;
}

//----------------------------------------------------------------------------
//
// Authored   : Maq
//
// Name       : Slic_GetStoredProduction
//
// Description: Function to find out how much production a city has stored.
//
// Parameters : SlicArg 0: city
//
// Globals    : -
//
// Returns    : SFN_ERROR		: execution result
//
//----------------------------------------------------------------------------
SFN_ERROR Slic_GetStoredProduction::Call(SlicArgList *args)
{
	if(args->Count() != 1)
		return SFN_ERROR_NUM_ARGS;

	Unit city;
	if(!args->GetCity(0, city)) {
		return SFN_ERROR_TYPE_BUILTIN;
	}

	m_result.m_int = city.GetStoredCityProduction();

	return SFN_ERROR_OK;
}
