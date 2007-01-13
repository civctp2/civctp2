
#ifndef _GOODY_HUTS_H_
#define _GOODY_HUTS_H_

class CivArchive;

enum GOODY {
	GOODY_BOGUS,
	GOODY_GOLD,
	GOODY_ADVANCE,
	GOODY_UNIT,
	GOODY_CITY,
	GOODY_BARBARIANS,
	GOODY_MAX
};

class MapPoint;

#define k_VALUE_RANGE 10000

class GoodyHut {
private:
	
	uint32 m_value;
	uint32 m_typeValue;

	GOODY ChooseType(sint32 owner);
	friend class NetCellList;
public:
	GoodyHut();
	GoodyHut(uint32 type, uint32 value) :
		m_typeValue(type),
		m_value(value)
	{}

	GoodyHut(CivArchive &archive);

	
	void OpenGoody(sint32 owner, const MapPoint &point);

	void Serialize(CivArchive &archive);
};

#endif
