






















#pragma once
#ifndef __BOUNDING_RECT_H__
#define __BOUNDING_RECT_H__

#include "MapPoint.h"
















class BoundingRect {

public:
	BoundingRect() : m_isValid(false) {}

	BoundingRect( const BoundingRect & rect ) { *this = rect; }

	const BoundingRect & operator=( const BoundingRect & rval )
		{
			m_isValid = rval.m_isValid;
			m_upperLeft = rval.m_upperLeft;
			m_lowerRight = rval.m_lowerRight;
			m_xWrapOk = rval.m_xWrapOk;
			m_yWrapOk = rval.m_yWrapOk;
			m_mapSize = rval.m_mapSize;
			m_ul_x_wrap = rval.m_ul_x_wrap;
			m_lr_x_wrap = rval.m_lr_x_wrap;
			m_ul_y_wrap = rval.m_ul_y_wrap;
			m_lr_y_wrap = rval.m_lr_y_wrap;

			return *this;
		}

	
	BoundingRect(const MapPoint & center, 
				 const sint16 radius, 
				 const MapPoint & map_size,
				 const bool & x_wrap_ok, 
				 const bool & y_wrap_ok)
		{
			Initialize(center, radius, map_size, x_wrap_ok, y_wrap_ok);
		}

	
	bool IsValid() const { return m_isValid; }

	
	void Initialize(const MapPoint & center, 
					const sint16 radius,
					const MapPoint & map_size,
					const bool & x_wrap_ok,
					const bool & y_wrap_ok);

	
	bool Normalize();

	
	bool Add(const BoundingRect & rect);

	
	
	bool Get(const sint32 & index, MapPoint & pos, const sint32 & max_rows, const sint32 & max_columns) const;

	
	const MapPoint & Get(const sint32 & index) const;

	
	bool Expand(const sint16 & size);

	
	bool IsMaxRect() const;

	
	MapPoint GetCenter() const;

	
	bool WithinRectangle(const MapPoint & pos) const;

	
	void Get(sint16 row, sint16 col, MapPoint & pos);
	sint16 GetMaxCols();
	sint16 GetMaxRows();

private:
	bool m_isValid;

	
	MapPoint m_upperLeft;
	MapPoint m_lowerRight;

	
	bool m_xWrapOk;
	bool m_yWrapOk;
	MapPoint m_mapSize;

	
	
	bool m_ul_x_wrap;
	bool m_lr_x_wrap;
	bool m_ul_y_wrap;
	bool m_lr_y_wrap;
};

#endif __BOUNDING_RECT_H


