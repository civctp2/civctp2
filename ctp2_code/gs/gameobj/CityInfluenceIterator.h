
#ifndef CITY_INFLUENCE_ITERATOR_H__
#define CITY_INFLUENCE_ITERATOR_H__

class CityInfluenceIterator {
protected:
	MapPoint m_center;
	MapPoint m_cur;
	MapPoint m_wrappedCur;
	sint16 m_startX, m_endX;
	sint16 m_row;
	sint16 m_intRadius;
	uint32 m_cityId;

public:
	
	CityInfluenceIterator(const MapPoint &center, sint32 size);
	CityInfluenceIterator();

	
	virtual void Init(const MapPoint &center, sint32 size);

	virtual void Start();
	bool End();
	virtual void Next();
	MapPoint &Pos();
};


class RadiusIterator : public CityInfluenceIterator {
protected:
	sint16 m_squaredRadius;

public:
	RadiusIterator(const MapPoint &center, sint32 size);
	RadiusIterator(const MapPoint &center, sint32 size, sint32 squaredSize);
	RadiusIterator();

	
	void Init(const MapPoint &center, sint32 size, sint32 squaredSize);
	void Start();

	
	void Next();
};


class SquareIterator : public CityInfluenceIterator {
public:
	SquareIterator(const MapPoint &center, sint32 size);
	SquareIterator();

	
	void Init(const MapPoint &center, sint32 size);
	void Start();

	
	void Next();
};

void GenerateCityInfluence(const MapPoint &cpos, sint32 size);
void GenerateBorders(const MapPoint &cpos, sint32 player, sint32 intRad, sint32 sqRad);

#endif
