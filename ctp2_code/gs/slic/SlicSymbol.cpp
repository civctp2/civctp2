//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of SLIC variables.
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leaks.
// - Removed assert to prevent lots of pop-ups with e.g. the LOTR scenario.
// - GetText function now returns FALSE if there is no proper string to
//   retrieve, so that this can be checked if. - Nov 5th 2004 Martin G�hmann
// - Added validity checks to GetPos.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicSymbol.h"

#include "c3errors.h"
#include "SlicFunc.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "slicif.h"
#include "StrDB.h"
#include "SlicEngine.h"
#include "civarchive.h"
#include "SelItem.h"
#include "SlicContext.h"
#include "Unit.h"
#include "UnitData.h"
#include "Civilisation.h"
#include "player.h"
#include "Gold.h"
#include "TurnCnt.h"
#include "TaxRate.h"
#include "Agreement.h"
#include "MaterialPool.h"
#include "UnitPool.h"
#include "pollution.h"
#include "Score.h"
#include "DiffDB.h"
#include "profileDB.h"
#include "CivilisationPool.h"
#include "Readiness.h"
#include "message.h"
#include "SlicFunc.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "SlicArray.h"
#include "SlicStack.h"
#include "SlicStruct.h"
#include "ArmyPool.h"
#include "UnitRecord.h"
#include "SlicNamedSymbol.h"
#include "TerrImprove.h"
#include "TerrImprovePool.h"

SlicSymbolData::SlicSymbolData(SLIC_SYM type)
:   m_type  (type)
{
	Init();
}

SlicSymbolData::SlicSymbolData(SlicArray *array)
:   m_type  (SLIC_SYM_ARRAY)
{
	Init();
	m_val.m_array = array;
}

SlicSymbolData::SlicSymbolData(SlicStructInstance *aStruct)
:   m_type  (SLIC_SYM_STRUCT)
{
	Init();
	m_val.m_struct = aStruct;
}

SlicSymbolData::SlicSymbolData(SlicStructDescription *structDesc)
:   m_type  (SLIC_SYM_STRUCT)
{
	Init();
	m_val.m_struct = new SlicStructInstance(structDesc);
}

SlicSymbolData::SlicSymbolData(SlicSymbolData const & copy)
:   m_type  (copy.GetType())
{
	Init();
    // TODO: check memory leaks?
	memcpy(&m_val, &copy.m_val, sizeof(m_val));
}

void SlicSymbolData::Init()
{
	memset(&m_val, 0, sizeof(m_val));
	m_debugInfo = NULL;
}

//----------------------------------------------------------------------------
//
// Name       : ~SlicSymbolData
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
SlicSymbolData::~SlicSymbolData()
{
    switch (GetType())
    {
    default:
        // No action: no specific data to delete
        break;

    case SLIC_SYM_REGION:
		delete m_val.m_region;
        break;

    case SLIC_SYM_COMPLEX_REGION:
        while (m_val.m_complexRegion)
        {
			PSlicComplexRegion *next = m_val.m_complexRegion->next;
			delete m_val.m_complexRegion;
			m_val.m_complexRegion = next;
		}
        break;

    case SLIC_SYM_STRUCT:
		delete m_val.m_struct;
        break;

    case SLIC_SYM_ARRAY:
		delete m_val.m_array;
        break;

	case SLIC_SYM_STRING:
		delete m_val.m_hard_string;
        break;
    } // switch

	delete m_debugInfo;
}

BOOL SlicSymbolData::GetIntValue(sint32 &value) const
{
	if(GetType() == SLIC_SYM_IVAR) {

		value = m_val.m_int_value;
		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(m_val.m_struct->GetDataSymbol()) {
			return m_val.m_struct->GetDataSymbol()->GetIntValue(value);
		}
	}
	return FALSE;
}

void SlicSymbolData::SetIntValue(sint32 value)
{
	if(GetType() == SLIC_SYM_IVAR) {
		m_val.m_int_value = value;
		NotifyChange();
	}
}

BOOL SlicSymbolData::SetValueFrom(SlicSymbolData *sym)
{
	switch(GetType()) {
		case SLIC_SYM_IVAR:
			if (!sym->GetIntValue(m_val.m_int_value))
            {
                return FALSE;
            }
			break;
		case SLIC_SYM_UNIT:
		{
			Unit u;
			if(!sym->GetUnit(u)) {
				return FALSE;
			}
			m_val.m_unit_id = u.m_id;
			break;
		}
		case SLIC_SYM_CITY:
		{
			Unit city;
			if(!sym->GetCity(city)) {
				return FALSE;
			}
			m_val.m_city_id = city.m_id;
			break;
		}
		case SLIC_SYM_ARMY:
		{
			Army army;
			if(!sym->GetArmy(army)) {
				return FALSE;
			}
			m_val.m_army_id = army.m_id;
			break;
		}
		case SLIC_SYM_LOCATION:
		{
			MapPoint pos;
			if(!sym->GetPos(pos)) {
				return FALSE;
			}
			m_val.m_location.x = pos.x;
			m_val.m_location.y = pos.y;
			break;
		}
		case SLIC_SYM_STRUCT:
			if(m_val.m_struct->GetDataSymbol()) {
				BOOL res = m_val.m_struct->GetDataSymbol()->SetValueFrom(sym);
				if(res)
					NotifyChange();
				return res;
			} else {
				return FALSE;
			}

		default:
			return FALSE;
	}

	NotifyChange();
	return TRUE;
}

BOOL SlicSymbolData::SetValueFromStackValue(SS_TYPE type, SlicStackValue value)
{
	switch(GetType()) {
		case SLIC_SYM_IVAR:
			m_val.m_int_value = SlicStack::Eval(type, value);
			NotifyChange();
			return TRUE;
		case SLIC_SYM_UNIT:
		{
			Unit u;
			if(!SlicStack::GetUnit(type, value, u)) {
				m_val.m_unit_id = u.m_id;

				return FALSE;
			}
			m_val.m_unit_id = u.m_id;
			NotifyChange();
			return TRUE;
		}
		case SLIC_SYM_CITY:
		{
			Unit city;
			if(!SlicStack::GetCity(type, value, city)) {
				m_val.m_city_id = city.m_id;

				return FALSE;
			}
			m_val.m_city_id = city.m_id;
			NotifyChange();
			return TRUE;
		}
		case SLIC_SYM_ARMY:
		{
			Army army;
			if(!SlicStack::GetArmy(type, value, army)) {

				m_val.m_army_id = army.m_id;

				return FALSE;
			}
			m_val.m_army_id = army.m_id;
			NotifyChange();
			return TRUE;
		}

		case SLIC_SYM_LOCATION:
		{
			MapPoint pos;
			if(!SlicStack::GetPos(type, value, pos)) {
				m_val.m_location.x = pos.x;
				m_val.m_location.y = pos.y;

				return FALSE;
			}
			m_val.m_location.x = pos.x;
			m_val.m_location.y = pos.y;
			NotifyChange();
			return TRUE;
		}
		case SLIC_SYM_STRUCT:
			if(m_val.m_struct->GetDataSymbol()) {
				BOOL res = m_val.m_struct->GetDataSymbol()->SetValueFromStackValue(type, value);
				NotifyChange();
				return res;
			} else {
				Assert(FALSE);
				return FALSE;
			}
		case SLIC_SYM_STRING:
			Assert(type == SS_TYPE_SYM);
			if(type == SS_TYPE_SYM) {
				char buf[k_MAX_MSG_LEN];
				if(value.m_sym->GetText(buf, k_MAX_MSG_LEN))
                {
                    delete [] m_val.m_hard_string;
					m_val.m_hard_string = new MBCHAR[strlen(buf) + 1];
					strcpy(m_val.m_hard_string, buf);
					return TRUE;
				} else {
					Assert(FALSE);
				}
			}
			return FALSE;
			break;
#ifdef SLIC_DOUBLES
		case SLIC_SYM_DVAR:
			// Modelled after SLIC_SYM_IVAR, untested.
			m_val.m_double_value = SlicStack::Eval(type, value);
			NotifyChange();
			return TRUE;

#endif
		case SLIC_SYM_SVAR:
			// Modelled after SLIC_SYM_IVAR, untested.
			m_val.m_string_value = SlicStack::Eval(type, value);
			NotifyChange();
			return TRUE;

		case SLIC_SYM_IMPROVEMENT:
			// Assert removed to get the LOTR scenario somewhat debuggable..
			// This code is reached when having a statement of the form
			//	"improvement_t" var = "int_t" value;
			// Probably these statements are rubbish anyway, because the
			// improvement_t var expects to get the  - unique game object - ID,
			// but the value is usually filled with the database index of the
			// improvement descriptor.
			return FALSE;

		case SLIC_SYM_ID:
		case SLIC_SYM_REGION:
		case SLIC_SYM_COMPLEX_REGION:
		case SLIC_SYM_ARRAY:
		case SLIC_SYM_BUILTIN:
		case SLIC_SYM_STRUCT_MEMBER:
		case SLIC_SYM_PLAYER:
		case SLIC_SYM_UFUNC:
		case SLIC_SYM_POP:
		case SLIC_SYM_PATH:
		default:
			Assert(FALSE);
			return FALSE;
	}
}

SlicFunc *SlicSymbolData::GetFunction() const
{
	if(GetType() == SLIC_SYM_FUNC) {
		return m_val.m_function_object;
	}
	return NULL;
}

StringId SlicSymbolData::GetStringId() const
{
	if(GetType() == SLIC_SYM_SVAR) {
		return m_val.m_string_value;
	} else {
		return -1;
	}
}

void SlicSymbolData::SetStringId(StringId id)
{
	if(GetType() == SLIC_SYM_SVAR) {
		m_val.m_string_value = id;
	} else {
		Assert(FALSE);
	}
}

BOOL SlicSymbolData::GetPlayer(PLAYER_INDEX &player) const
{
	if(GetType() == SLIC_SYM_IVAR) {

		player = m_val.m_int_value;
		if(player < 0 || player >= k_MAX_PLAYERS)
			return FALSE;

		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(m_val.m_struct && m_val.m_struct->GetDataSymbol()) {
			return m_val.m_struct->GetDataSymbol()->GetPlayer(player);
		}
	}
	return FALSE;
}

BOOL SlicSymbolData::GetPos(MapPoint &point) const
{
	if(GetType() == SLIC_SYM_LOCATION) {
		point.x = sint16(m_val.m_location.x);
		point.y = sint16(m_val.m_location.y);
		return point.IsValid();
	} else if(GetType() == SLIC_SYM_UNIT) {
		MapPoint upos;
		Unit u(m_val.m_unit_id);
		if(!u.IsValid())
			return FALSE;
		u.GetPos(point);
		return TRUE;
	} else if(GetType() == SLIC_SYM_CITY) {
		MapPoint cpos;
		Unit c(m_val.m_city_id);
		if(!c.IsValid())
			return FALSE;
		c.GetPos(point);
		return TRUE;
	} else if(GetType() == SLIC_SYM_ARMY) {
		MapPoint apos;
		Army a(m_val.m_army_id);
		if(!a.IsValid())
			return FALSE;
		a.GetPos(point);
		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(m_val.m_struct && m_val.m_struct->GetDataSymbol()) {
			return m_val.m_struct->GetDataSymbol()->GetPos(point);
		}
	}

	return FALSE;
}

BOOL SlicSymbolData::GetCity(Unit &city) const
{
	if(GetType() == SLIC_SYM_CITY) {
		city = Unit(m_val.m_city_id);
		if(g_theUnitPool->IsValid(city))
			return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(m_val.m_struct->GetDataSymbol()) {
			return m_val.m_struct->GetDataSymbol()->GetCity(city);
		}
	}
	return FALSE;
}

BOOL SlicSymbolData::GetUnit(Unit &unit) const
{
	if(GetType() == SLIC_SYM_UNIT) {
		unit = Unit(m_val.m_unit_id);
		if(unit.IsValid())
			return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(m_val.m_struct->GetDataSymbol()) {
			return m_val.m_struct->GetDataSymbol()->GetUnit(unit);
		}
	}
	return FALSE;
}

BOOL SlicSymbolData::GetImprovement(TerrainImprovement &imp) const
{
	if(GetType() == SLIC_SYM_IMPROVEMENT) {
		imp = TerrainImprovement(m_val.m_improvement_id);
		if(g_theTerrainImprovementPool->IsValid(imp))
			return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(m_val.m_struct->GetDataSymbol()) {
			return m_val.m_struct->GetDataSymbol()->GetImprovement(imp);
		}
	}
	return FALSE;
}

BOOL SlicSymbolData::GetArmy(Army &army) const
{
	if(GetType() == SLIC_SYM_ARMY) {
		army = Army(m_val.m_army_id);
		if(!army.IsValid())
			return FALSE;
		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(m_val.m_struct->GetDataSymbol()) {
			return m_val.m_struct->GetDataSymbol()->GetArmy(army);
		}
	}
	return FALSE;
}

BOOL SlicSymbolData::SetUnit(Unit &unit)
{
	if(GetType() == SLIC_SYM_UNIT) {
		m_val.m_unit_id = unit.m_id;
		NotifyChange();
		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(GetStruct()->GetDataSymbol()) {
			return GetStruct()->GetDataSymbol()->SetUnit(unit);
		}
	}
	return FALSE;
}

BOOL SlicSymbolData::SetCity(const Unit &city)
{
	if(GetType() == SLIC_SYM_CITY) {
		m_val.m_city_id = city.m_id;
		NotifyChange();
		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(GetStruct()->GetDataSymbol()) {
			return GetStruct()->GetDataSymbol()->SetCity(city);
		}
	}

	return FALSE;
}

BOOL SlicSymbolData::SetArmy(const Army &army)
{
	if(GetType() == SLIC_SYM_ARMY) {
		m_val.m_army_id = army.m_id;
		NotifyChange();
		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(GetStruct()->GetDataSymbol()) {
			return GetStruct()->GetDataSymbol()->SetArmy(army);
		}
	}

	return FALSE;
}

BOOL SlicSymbolData::SetPos(MapPoint &pos)
{
	if(GetType() == SLIC_SYM_LOCATION) {
		m_val.m_location.x = pos.x;
		m_val.m_location.y = pos.y;
		NotifyChange();
		return TRUE;
	} else if(GetType() == SLIC_SYM_STRUCT) {
		if(GetStruct()->GetDataSymbol()) {
			return GetStruct()->GetDataSymbol()->SetPos(pos);
		}
	}

	return FALSE;
}

BOOL SlicSymbolData::GetUnitType(sint32 &type) const
{
	return FALSE;
}

BOOL SlicSymbolData::GetRegion(struct PSlicRegion &region) const
{

	if(GetType() != SLIC_SYM_REGION)
		return FALSE;
	region = *m_val.m_region;
	return TRUE;
}

BOOL SlicSymbolData::GetComplexRegion(const struct PSlicComplexRegion *&region) const
{

	if(GetType() != SLIC_SYM_COMPLEX_REGION)
		return FALSE;
	region = m_val.m_complexRegion;
	return TRUE;
}

BOOL SlicSymbolData::GetText(MBCHAR *text, sint32 maxLen) const
{
	sint32 ival;
	SlicSymbolData *dataSym;

	switch(GetType()) {
		case SLIC_SYM_IVAR:
			GetIntValue(ival);
			sprintf(text, "%d", ival);
			break;
		case SLIC_SYM_STRUCT:
			dataSym = m_val.m_struct->GetDataSymbol();
			dataSym->GetText(text, maxLen);
			break;
		case SLIC_SYM_STRING:
			strcpy(text, m_val.m_hard_string);
			break;
		case SLIC_SYM_SVAR:
			if(g_theStringDB->GetNameStr(m_val.m_string_value)) {
				strcpy(text, g_theStringDB->GetNameStr(m_val.m_string_value));
			} else {
				sprintf(text, "<Bad String ID %d>", m_val.m_string_value);
			}
			break;
		default:
			sprintf(text, "??? (Symbol type %d)", GetType());
// Added by Martin G�hmann to figure out whether there is a string for a given slic symbol.
			return FALSE;
			break;
	}

	return TRUE;
}

void SlicSymbolData::GetDebugText(MBCHAR *text, size_t len) const
{
	SlicSymbolData *dataSym;
	sint32 ival;
	size_t alen;
	MapPoint pos;
	Unit u;
	Unit city;
	Army army;

	switch(GetType()) {
	case SLIC_SYM_IVAR:
		GetIntValue(ival);
		alen = snprintf(text, len, "%d", ival);
		break;
	case SLIC_SYM_SVAR:
		alen = snprintf(text, len, "\"%s\"", m_val.m_hard_string);
		break;
	case SLIC_SYM_ID:
		alen = snprintf(text, len, "'%s'", GetName());
		break;
	case SLIC_SYM_FUNC:
		alen = snprintf(text, len, "%s()", GetName());
		break;
	case SLIC_SYM_STRING:
		alen = snprintf(text, len, "<%d> ID_%s: \"%s\"", m_val.m_string_value,
				g_theStringDB->GetIdStr(m_val.m_string_value),
				g_theStringDB->GetNameStr(m_val.m_string_value));
		break;
	case SLIC_SYM_CITY:
		if(GetCity(city)) {
			alen = snprintf(text, len, "City %x: %s", city.m_id,
					city.GetName());
		} else {
			alen = snprintf(text, len, "Invalid city (%x?)", m_val.m_city_id);
		}
		break;
	case SLIC_SYM_UNIT:
		if(GetUnit(u)) {

			alen = snprintf(text, len, "Unit %x: %s", u.m_id,
					u.IsValid() ? u.GetName() : "<DEAD>");
		} else {
			alen = snprintf(text, len, "Invalid unit (%x?)", m_val.m_unit_id);
		}
		break;
	case SLIC_SYM_ARMY:
		if(GetArmy(army)) {
			alen = snprintf(text, len, "Army %x", army.m_id);
		} else {
			alen = snprintf(text, len, "Invalid army (%x?)", m_val.m_army_id);
		}
		break;
	case SLIC_SYM_LOCATION:
		if(GetPos(pos)) {
			alen = snprintf(text, len, "Location: %d,%d",
					pos.x,
					pos.y);
		} else {
			alen = snprintf(text, len, "Invalid location (%x,%x)?",
					m_val.m_location.x,
					m_val.m_location.y);
		}
		break;
	case SLIC_SYM_ARRAY:
		alen = snprintf(text, len, "Array");
		break;
	case SLIC_SYM_STRUCT:
		dataSym = m_val.m_struct->GetDataSymbol();
		dataSym->GetDebugText(text, len);
		return;
	case SLIC_SYM_PLAYER:
		alen = snprintf(text, len, "Player");
		break;
	default:
		alen = snprintf(text, len, "???");
	}
	if (alen > len)
		fprintf(stderr, "%s: Provided text-size %zd is too short, "
		    "need at least %zd", __func__, len, alen);
}

const char *SlicSymbolData::GetName() const
{
	return "noname";
}

SlicSegment *SlicSymbolData::GetSegment()
{
	if(GetType() == SLIC_SYM_ID) {
		return m_val.m_segment;
	} else {
		return NULL;
	}
}

void SlicSymbolData::SetSegment(SlicSegment *segment)
{
	Assert((GetType() == SLIC_SYM_ID) || (GetType() == SLIC_SYM_UFUNC));
	if(GetType() == SLIC_SYM_ID || GetType() == SLIC_SYM_UFUNC)
		m_val.m_segment = segment;
}

BOOL SlicSymbolData::ArrayLookup(sint32 index, SS_TYPE &retType, SlicStackValue &retValue)
{
	if(GetType() != SLIC_SYM_ARRAY) {
		return FALSE;
	}

	if(!m_val.m_array) {
		return FALSE;
	}

	return m_val.m_array->Lookup(index, retType, retValue);
}

BOOL SlicSymbolData::SetArrayValue(sint32 index, SS_TYPE insType, SlicStackValue insValue)
{
	if(GetType() != SLIC_SYM_ARRAY) {
		return FALSE;
	}

	if(!m_val.m_array) {
		return FALSE;
	}

	return m_val.m_array->Insert(index, insType, insValue);
}

GAME_EVENT_ARGUMENT SlicSymbolData::GetArgumentType() const
{
	return GEA_Null;
}

SlicStructInstance *SlicSymbolData::GetStruct()
{

	Assert(GetType() == SLIC_SYM_STRUCT);
	if(GetType() != SLIC_SYM_STRUCT)
		return NULL;

	return m_val.m_struct;
}

void SlicSymbolData::SetStruct(SlicStructInstance *aStruct)
{
	Assert(GetType() == SLIC_SYM_STRUCT);
	if(GetType() != SLIC_SYM_STRUCT)
		return;

	m_val.m_struct = aStruct;
}

void SlicSymbolData::SetType(SLIC_SYM type)
{
//  Assert((m_type == type) || (m_type == SLIC_SYM_UNDEFINED));

    m_type = type;
	sint32 res;

	switch((type = GetType())) {
		case SLIC_SYM_ARRAY:
            Assert(!m_val.m_array);
			m_val.m_array = new SlicArray(SS_TYPE_SYM, SLIC_SYM_UNDEFINED);
			break;
		case SLIC_SYM_FUNC:
			m_val.m_function_object = g_slicEngine->GetFunction(GetName());
			break;
		case SLIC_SYM_SVAR:
			res = g_theStringDB->GetStringID(GetName(), m_val.m_string_value);
			Assert(res);
			break;
		default:
			fprintf(stderr, "%s: Unexpected type %d\n", __func__, type);
	}
}

void SlicSymbolData::SetArrayType(SLIC_SYM type)
{
	Assert(GetType() == SLIC_SYM_ARRAY);
	if(GetType() == SLIC_SYM_ARRAY) {
		m_val.m_array->SetType(type == SLIC_SYM_IVAR ? SS_TYPE_INT : SS_TYPE_SYM,
							   type);
	}
}

SlicArray * SlicSymbolData::GetArray() const
{
	Assert(GetType() == SLIC_SYM_ARRAY);
	if(GetType() == SLIC_SYM_ARRAY) {
		return m_val.m_array;
	}
	return NULL;
}

#if 0
void SlicSymbolData::SetStructMember(SlicStructInstance::Member *member)
{
	if(GetType() == SLIC_SYM_UNDEFINED) {
		GetType() = SLIC_SYM_STRUCT_MEMBER;
	} else {
		Assert(GetType() == SLIC_SYM_STRUCT_MEMBER);
	}

	m_val.m_structMember = member;
}
#endif

void SlicSymbolData::NotifyChange()
{
	if(!m_debugInfo)
		return;

	m_debugInfo->NotifyChange(this);
}

void SlicSymbolData::AddWatch(SlicSymbolWatchCallback *watch)
{
	if(!m_debugInfo) {
		m_debugInfo = new SlicSymbolDebugInfo(this);
	}

	m_debugInfo->AddWatch(watch);
}

void SlicSymbolData::RemoveWatch(SlicSymbolWatchCallback *watch)
{
	if(!m_debugInfo)
		return;

	m_debugInfo->RemoveWatch(watch);
}




SlicSymbolDebugInfo::SlicSymbolDebugInfo(SlicSymbolData *sym)
{
	m_symbol = sym;
	m_watchList = new PointerList<SlicSymbolWatchCallback>;
}

SlicSymbolDebugInfo::~SlicSymbolDebugInfo()
{
	if(m_watchList) {
		PointerList<SlicSymbolWatchCallback>::Walker walk(m_watchList);
		while(walk.IsValid()) {
			walk.GetObj()->WatchVariableDeleted(m_symbol);
			walk.Next();
		}
		delete m_watchList;
		m_watchList = NULL;
	}
}

void SlicSymbolDebugInfo::AddWatch(SlicSymbolWatchCallback *watch)
{
	Assert(m_watchList);
	PointerList<SlicSymbolWatchCallback>::Walker walk(m_watchList);
	while(walk.IsValid()) {

		if(walk.GetObj() == watch)
			return;
		walk.Next();
	}
	m_watchList->AddTail(watch);
}

void SlicSymbolDebugInfo::RemoveWatch(SlicSymbolWatchCallback *watch)
{
	PointerList<SlicSymbolWatchCallback>::Walker walk(m_watchList);
	while(walk.IsValid()) {
		if(walk.GetObj() == watch) {
			walk.Remove();
		} else {

			walk.Next();
		}
	}
}

void SlicSymbolDebugInfo::NotifyChange(SlicSymbolData *sym)
{
	PointerList<SlicSymbolWatchCallback>::Walker walk(m_watchList);
	while(walk.IsValid()) {
		walk.GetObj()->WatchCallback(sym, false);
		walk.Next();
	}
}

void SlicSymbolData::SetString(MBCHAR const * str)
{
	if(GetType() == SLIC_SYM_STRING) {
        delete [] m_val.m_hard_string;
        if (str)
        {
		    m_val.m_hard_string = new char[strlen(str) + 1];
            strcpy(m_val.m_hard_string, str);
        }
        else
        {
            m_val.m_hard_string = NULL;
        }
	} else if(GetType() == SLIC_SYM_STRUCT) {
		m_val.m_struct->GetDataSymbol()->SetString(str);
	} else {
		Assert(FALSE);
	}
}

void SlicSymbolData::Serialize(CivArchive &archive)
{
	uint16 len;
	MBCHAR buf[_MAX_PATH];

	if(archive.IsStoring()) {
		archive.PutUINT8(GetSerializeType());
		archive.PutUINT8(m_type);
		switch(GetType()) {
			case SLIC_SYM_IVAR:
#ifdef SLIC_DOUBLES
			case SLIC_SYM_DVAR:
#endif
			case SLIC_SYM_SVAR:
			case SLIC_SYM_CITY:
			case SLIC_SYM_UNIT:
			case SLIC_SYM_ARMY:
			case SLIC_SYM_LOCATION:
			case SLIC_SYM_PLAYER:

				archive.Store((uint8*)&m_val, sizeof(m_val));
				break;


			case SLIC_SYM_FUNC:
				if(m_val.m_function_object) {
					len = strlen(m_val.m_function_object->GetName());
					archive << len;
					archive.Store((uint8*)m_val.m_function_object->GetName(), len);
				} else {
					len = 0;
					archive << len;
				}
				break;
			case SLIC_SYM_REGION:
			case SLIC_SYM_COMPLEX_REGION:

				Assert(FALSE);
				break;
			case SLIC_SYM_STRING:
				if(!m_val.m_hard_string)
					len = 0;
				else
					len = strlen(m_val.m_hard_string);

				archive << len;

				if(len > 0)
					archive.Store((uint8*)m_val.m_hard_string, len);
				break;
			case SLIC_SYM_ARRAY:
				m_val.m_array->Serialize(archive);
				break;
			case SLIC_SYM_BUILTIN:
				Assert(FALSE);
				break;
			case SLIC_SYM_STRUCT:
				m_val.m_struct->Serialize(archive);
				break;
			case SLIC_SYM_STRUCT_MEMBER:

				break;
			case SLIC_SYM_UFUNC:
			case SLIC_SYM_ID:
				if(!m_val.m_segment) {
					len = 0;
					archive << len;

				} else {
					len = strlen(m_val.m_segment->GetName());
					archive << len;
					archive.Store((uint8*)m_val.m_segment->GetName(), len);
				}
				break;
			case SLIC_SYM_POP:
			case SLIC_SYM_PATH:
				Assert(FALSE);
				break;
			case SLIC_SYM_UNDEFINED:

				break;
			default:
				fprintf(stderr, "%s: type %d not handled\n", __func__, GetType());
		}
	} else {

		m_type = (SLIC_SYM)archive.GetUINT8();
		switch(GetType() == SLIC_SYM_UNDEFINED ? m_type : GetType()) {
			case SLIC_SYM_IVAR:
#ifdef SLIC_DOUBLES
			case SLIC_SYM_DVAR:
#endif
			case SLIC_SYM_SVAR:
			case SLIC_SYM_CITY:
			case SLIC_SYM_UNIT:
			case SLIC_SYM_ARMY:
			case SLIC_SYM_LOCATION:
			case SLIC_SYM_PLAYER:

				archive.Load((uint8*)&m_val, sizeof(m_val));
				break;


			case SLIC_SYM_FUNC:
				archive >> len;
				if(len > 0) {
					archive.Load((uint8*)buf, len);
				}
				buf[len] = 0;
				m_val.m_function_object = g_slicEngine->GetFunction(buf);
				Assert(m_val.m_function_object);
				break;
			case SLIC_SYM_REGION:
			case SLIC_SYM_COMPLEX_REGION:

				Assert(FALSE);
				break;
			case SLIC_SYM_STRING:
				archive >> len;
				if(len == 0)
					m_val.m_hard_string = NULL;
				else {
					m_val.m_hard_string = new MBCHAR[len + 1];
					archive.Load((uint8*)m_val.m_hard_string, len);
					m_val.m_hard_string[len] = 0;
				}
				break;
			case SLIC_SYM_ARRAY:
				m_val.m_array = new SlicArray(archive);
				break;
			case SLIC_SYM_BUILTIN:
				Assert(FALSE);
				break;
			case SLIC_SYM_STRUCT:
				m_val.m_struct = new SlicStructInstance(archive);
				break;
			case SLIC_SYM_STRUCT_MEMBER:

				break;
			case SLIC_SYM_UFUNC:
			case SLIC_SYM_ID:
				archive >> len;
				if(len > 0) {
					archive.Load((uint8*)buf, len);
					buf[len] = 0;
					m_val.m_segment = g_slicEngine->GetSegment(buf);
				} else {
					m_val.m_segment = NULL;
				}
				break;
			case SLIC_SYM_POP:
			case SLIC_SYM_PATH:
				Assert(FALSE);
				break;
			case SLIC_SYM_UNDEFINED:

				break;
			default:
				fprintf(stderr, "%s: type %d not handled\n", __func__, GetType());
		}
	}

	switch(GetSerializeType()) {
		case SLIC_SYM_SERIAL_GENERIC:

			break;
		case SLIC_SYM_SERIAL_NAMED:
			((SlicNamedSymbol *)this)->SlicNamedSymbol::Serialize(archive);
			break;
		case SLIC_SYM_SERIAL_PARAMETER:
			((SlicParameterSymbol *)this)->SlicParameterSymbol::Serialize(archive);
			break;
		case SLIC_SYM_SERIAL_MEMBER:
			((SlicStructMemberData *)this)->SlicStructMemberData::Serialize(archive);
			break;
		case SLIC_SYM_SERIAL_BUILTIN:
			((SlicBuiltinNamedSymbol *)this)->SlicBuiltinNamedSymbol::Serialize(archive);
			break;
		default:
			Assert(FALSE);
			break;
	}
}

SlicSymbolData *slicsymbol_Load(CivArchive &archive, SlicSymbolData *useSymbol)
{
	Assert(!archive.IsStoring());
	SLIC_SYM_SERIAL_TYPE loadType = (SLIC_SYM_SERIAL_TYPE)archive.GetUINT8();
	SlicSymbolData *sym = NULL;
	switch(loadType) {
		case SLIC_SYM_SERIAL_GENERIC:
			sym = new SlicSymbolData;
			break;

		case SLIC_SYM_SERIAL_NAMED:
			sym = new SlicNamedSymbol;
			break;

		case SLIC_SYM_SERIAL_PARAMETER:
			sym = new SlicParameterSymbol;
			break;

		case SLIC_SYM_SERIAL_MEMBER:
			sym = useSymbol;
			break;
		case SLIC_SYM_SERIAL_BUILTIN:
			sym = new SlicBuiltinNamedSymbol;
			break;

		default:
			Assert(FALSE);
			break;
	}

	if(sym) {
		sym->SlicSymbolData::Serialize(archive);
	}
	return sym;
}
