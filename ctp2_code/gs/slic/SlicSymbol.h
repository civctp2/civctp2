#ifndef __SLIC_SYMBOL_H__
#define __SLIC_SYMBOL_H__

#include "slicif.h"
#include "SlicTriggerLists.h"
#include "SlicStack.h"

class SlicSegment;
typedef sint32 StringId;
class SlicFunc;
class CivArchive;
class MapPoint;
typedef sint32 PLAYER_INDEX;
struct PSlicRegion;
struct PSlicComplexRegion;
class Unit;
class SlicArray;
class SlicStructDescription;
class SlicStructInstance;
class Army;
template <class T> class PointerList;
class TerrainImprovement;

#include "GameEventTypes.h"

enum SLIC_SYM_SERIAL_TYPE {
	SLIC_SYM_SERIAL_GENERIC,
	SLIC_SYM_SERIAL_NAMED,
	SLIC_SYM_SERIAL_PARAMETER,
	SLIC_SYM_SERIAL_MEMBER,
	SLIC_SYM_SERIAL_BUILTIN
};

struct SlicSymMapPoint {
	sint16 x, y, z;
};


class SlicSymbolWatchCallback {
public:
	virtual void WatchCallback(SlicSymbolData *symbol, bool isAddCallback) = 0;
	virtual void WatchVariableDeleted(SlicSymbolData *symbol) {}
};





class SlicSymbolDebugInfo {
public:
	SlicSymbolDebugInfo(SlicSymbolData *sym);
	~SlicSymbolDebugInfo();

	PointerList<SlicSymbolWatchCallback> *GetWatchList() { return m_watchList; }
	void AddWatch(SlicSymbolWatchCallback *watch);
	void RemoveWatch(SlicSymbolWatchCallback *watch);
	void NotifyChange(SlicSymbolData *sym);

private:
	SlicSymbolData *m_symbol;
	PointerList<SlicSymbolWatchCallback> *m_watchList;
};

class SlicSymbolData {
protected:
	SLIC_SYM m_type;
	union {
		sint32 m_int_value;
#ifdef SLIC_DOUBLES
		double m_double_value;
#endif
		StringId m_string_value;
		SlicFunc *m_function_object;
		SlicSegment *m_segment;
		struct PSlicRegion *m_region;
		struct PSlicComplexRegion *m_complexRegion;
		char *m_hard_string;
		uint32 m_city_id;
		uint32 m_unit_id;
		uint32 m_army_id;
		uint32 m_improvement_id;
		SlicSymMapPoint m_location;
		SlicArray *m_array;
		SlicStructInstance *m_struct;

	} m_val;

	SlicSymbolDebugInfo *m_debugInfo;

public:
	SlicSymbolData(SLIC_SYM type = SLIC_SYM_UNDEFINED);
	SlicSymbolData(SlicArray *array);
	SlicSymbolData(SlicStructInstance *aStruct);
	SlicSymbolData(SlicSymbolData const & copy);
	SlicSymbolData(SlicStructDescription *structDesc);

	void Serialize(CivArchive &archive);
	virtual SLIC_SYM_SERIAL_TYPE GetSerializeType() { return SLIC_SYM_SERIAL_GENERIC; }
	virtual ~SlicSymbolData();

	virtual const char *GetName() const;
	virtual BOOL GetIntValue(sint32 &value) const;
	virtual BOOL GetPlayer(PLAYER_INDEX &player) const;
	virtual BOOL GetPos(MapPoint &point) const;
	virtual SLIC_SYM GetType() const { return m_type; }
	virtual BOOL GetCity(Unit &city) const;
	virtual BOOL GetUnit(Unit &unit) const;
	virtual BOOL GetArmy(Army &army) const;
	virtual BOOL GetText(MBCHAR *text, sint32 maxLen) const;
	virtual BOOL GetImprovement(TerrainImprovement &imp) const;
	virtual StringId GetStringId() const;
	virtual void SetStringId(StringId id);
	virtual bool IsParameter() const { return false; }

	void SetType(SLIC_SYM type);

	void SetIntValue(sint32 val);
	BOOL SetValueFrom(SlicSymbolData *sym);
	BOOL SetValueFromStackValue(SS_TYPE type, SlicStackValue value);
	void SetString(MBCHAR const * str);

	SlicFunc *GetFunction() const;

	void SetFunction(SlicFunc *func) { m_val.m_function_object = func; }
	BOOL SetUnit(Unit &unit);
	BOOL SetCity(const Unit &city);
	BOOL SetArmy(const Army &army);
	BOOL SetPos(MapPoint &pos);

	BOOL GetUnitType(sint32 &type) const;
	BOOL GetRegion(struct PSlicRegion &region) const;
	BOOL GetComplexRegion(const struct PSlicComplexRegion *&region) const;
	void GetDebugText(MBCHAR *text, size_t len) const;

	BOOL ArrayLookup(sint32 index, SS_TYPE &retType, SlicStackValue &retValue);
	BOOL SetArrayValue(sint32 index, SS_TYPE insType, SlicStackValue insValue);

	GAME_EVENT_ARGUMENT GetArgumentType() const;

	SlicStructInstance *GetStruct();
	void SetStruct(SlicStructInstance *aStruct);

	SlicArray *GetArray() const;

	SlicSegment *GetSegment();
	void SetSegment(SlicSegment *segment);

	void SetArrayType(SLIC_SYM type);

	void AddWatch(SlicSymbolWatchCallback *watch);
	void RemoveWatch(SlicSymbolWatchCallback *watch);
	void NotifyChange();

private:
   	void Init();
};

SlicSymbolData *slicsymbol_Load(CivArchive &archive, SlicSymbolData *useSymbol);
#endif
