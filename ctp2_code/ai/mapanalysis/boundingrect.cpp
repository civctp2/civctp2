//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : BoundingRect
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
// - Corrected the bounding Rec computation (problem with wraping)
// 
//----------------------------------------------------------------------------

#include "c3.h"
#include "boundingrect.h"

#include <algorithm>        // std::max, std::min

void BoundingRect::Initialize
(
    const MapPoint & xy_center,
    const sint16     radius,
    const MapPoint & map_size,
    const bool     & x_wrap_ok,
    const bool     & y_wrap_ok
)
{
	m_xWrapOk = x_wrap_ok;
	m_yWrapOk = y_wrap_ok;
	m_mapSize = map_size;

	m_upperLeft.x = xy_center.x - radius;
	m_upperLeft.y = xy_center.y - radius;

	m_lowerRight.x = xy_center.x + radius;
	m_lowerRight.y = xy_center.y + radius;

	m_ul_x_wrap = m_lr_x_wrap = m_ul_y_wrap = m_lr_y_wrap = false;

	m_isValid = Normalize();
}

bool BoundingRect::Normalize()
{
	if (m_upperLeft.x < 0)
	{
		m_ul_x_wrap = m_xWrapOk;
		if(m_xWrapOk)
		{
			m_upperLeft.x += m_mapSize.x;
		}
		else
		{
			m_upperLeft.x = 0;
		}
	}
	else
	{
		if(m_lowerRight.x > m_upperLeft.x)
			m_ul_x_wrap = false;
	}

	if(m_upperLeft.y < 0)
	{
		m_ul_y_wrap = m_yWrapOk;
		if(m_yWrapOk)
		{
			m_upperLeft.y += m_mapSize.y;
		}
		else
		{
			m_upperLeft.y = 0;
		}
	}
	else
	{
		if(m_lowerRight.y > m_upperLeft.y)
			m_ul_y_wrap = false;
	}

	if(m_lowerRight.x > m_mapSize.x)
	{
		m_lr_x_wrap = m_xWrapOk;
		if(m_xWrapOk)
			m_lowerRight.x -= m_mapSize.x;
		else
			m_lowerRight.x = m_mapSize.x;
	}
	else
	{
		if(m_lowerRight.x > m_upperLeft.x)
			m_lr_x_wrap = false;
	}

	if(m_lowerRight.y > m_mapSize.y)
	{
		m_lr_y_wrap = m_yWrapOk;

		if(m_yWrapOk)
			m_lowerRight.y -= m_mapSize.y;
		else
			m_lowerRight.y = m_mapSize.y;
	}
	else
	{
		if (m_lowerRight.y > m_upperLeft.y)
			m_lr_y_wrap = false;
	}

	Assert(!((m_ul_x_wrap || m_lr_x_wrap) && (!m_xWrapOk)));
	Assert(!((m_ul_y_wrap || m_lr_y_wrap) && (!m_yWrapOk)));

	if
	  (
	   (
	        !m_lr_x_wrap
	     && !m_ul_x_wrap
	     &&  m_lowerRight.x < m_upperLeft.x
	   )
	   ||
	   (
	        !m_lr_y_wrap
	     && !m_ul_y_wrap
	     &&  m_lowerRight.y < m_upperLeft.y
	   )
	  )
	{
		Assert(false);
		m_isValid = false;
		return false;
	}

	return true;
}

bool BoundingRect::Add(const BoundingRect & add_rect)
{
	Assert(add_rect.m_xWrapOk == m_xWrapOk);
	Assert(add_rect.m_yWrapOk == m_yWrapOk);
	Assert(add_rect.m_mapSize == m_mapSize);

	if
	  (
	      add_rect.m_xWrapOk != m_xWrapOk
	   || add_rect.m_yWrapOk != m_yWrapOk
	   || add_rect.m_mapSize != m_mapSize
	  )
	{
		return false;
	}

	bool is_add_rect_ul_xWrapOk = false;
	bool is_add_rect_ul_yWrapOk = false;
	bool is_add_rect_lr_xWrapOk = false;
	bool is_add_rect_lr_yWrapOk = false;

	if(m_xWrapOk)
	{
		is_add_rect_ul_xWrapOk =
		                        (
		                            (
		                               std::min(m_upperLeft.x, add_rect.m_upperLeft.x)
		                             + m_mapSize.x
		                             - std::max(m_upperLeft.x, add_rect.m_upperLeft.x)
		                            )
		                          <
		                            (
		                               std::max(m_upperLeft.x, add_rect.m_upperLeft.x)
		                             - std::min(m_upperLeft.x, add_rect.m_upperLeft.x)
		                            )
		                        );

		is_add_rect_lr_xWrapOk =
		                        (
		                            (
		                               std::min(m_lowerRight.x, add_rect.m_lowerRight.x)
		                             + m_mapSize.x
		                             - std::max(m_lowerRight.x, add_rect.m_lowerRight.x)
		                            )
		                          <
		                            (
		                               std::max(m_lowerRight.x, add_rect.m_lowerRight.x)
		                             - std::min(m_lowerRight.x, add_rect.m_lowerRight.x)
		                            )
		                        );
	}

	if(m_yWrapOk)
	{
		is_add_rect_ul_yWrapOk =
		                        (
		                            (
		                               std::min(m_upperLeft.y, add_rect.m_upperLeft.y)
		                             + m_mapSize.y
		                             - std::max(m_upperLeft.y, add_rect.m_upperLeft.y)
		                            )
		                          <
		                            (
		                               std::max(m_upperLeft.y, add_rect.m_upperLeft.y)
		                             - std::min(m_upperLeft.y, add_rect.m_upperLeft.y)
		                            )
		                        );

		is_add_rect_lr_yWrapOk =
		                        (
		                            (
		                               std::min(m_lowerRight.y, add_rect.m_lowerRight.y)
		                             + m_mapSize.y
		                             - std::max(m_lowerRight.y, add_rect.m_lowerRight.y)
		                            )
		                          <
		                            (
		                               std::max(m_lowerRight.y, add_rect.m_lowerRight.y)
		                             - std::min(m_lowerRight.y, add_rect.m_lowerRight.y)
		                            )
		                        );
	}

	if((m_ul_x_wrap != add_rect.m_ul_x_wrap) || (is_add_rect_ul_xWrapOk && (m_ul_x_wrap == add_rect.m_ul_x_wrap)))
	{
		if (add_rect.m_upperLeft.x > m_upperLeft.x)
		{
			m_upperLeft.x = add_rect.m_upperLeft.x;
		}
	}
	else if(add_rect.m_upperLeft.x < m_upperLeft.x)
	{
		m_upperLeft.x = add_rect.m_upperLeft.x;
	}

	if((m_ul_y_wrap != add_rect.m_ul_y_wrap) || (is_add_rect_ul_yWrapOk && (m_ul_y_wrap == add_rect.m_ul_y_wrap)))
	{
		if (add_rect.m_upperLeft.y > m_upperLeft.y)
		{
			m_upperLeft.y = add_rect.m_upperLeft.y;
		}
	}
	else if (add_rect.m_upperLeft.y < m_upperLeft.y)
	{
		m_upperLeft.y = add_rect.m_upperLeft.y;
	}

	if((m_lr_x_wrap != add_rect.m_lr_x_wrap) || (is_add_rect_lr_xWrapOk && (m_lr_x_wrap == add_rect.m_lr_x_wrap))) 
	{
		if (add_rect.m_lowerRight.x < m_lowerRight.x)
		{
			m_lowerRight.x = add_rect.m_lowerRight.x;
		}
	}
	else if(add_rect.m_lowerRight.x > m_lowerRight.x)
	{
		m_lowerRight.x = add_rect.m_lowerRight.x;
	}

	if((m_lr_y_wrap != add_rect.m_lr_y_wrap) || (is_add_rect_lr_yWrapOk && (m_lr_y_wrap == add_rect.m_lr_y_wrap))) 
	{
		if(add_rect.m_lowerRight.y < m_lowerRight.y)
		{
			m_lowerRight.y = add_rect.m_lowerRight.y;
		}
	}
	else if(add_rect.m_lowerRight.y > m_lowerRight.y)
	{
		m_lowerRight.y = add_rect.m_lowerRight.y;
	}

	m_ul_x_wrap |= add_rect.m_ul_x_wrap || is_add_rect_ul_xWrapOk;
	m_ul_y_wrap |= add_rect.m_ul_y_wrap || is_add_rect_ul_yWrapOk;
	m_lr_x_wrap |= add_rect.m_lr_x_wrap || is_add_rect_lr_xWrapOk;
	m_lr_y_wrap |= add_rect.m_lr_y_wrap || is_add_rect_lr_yWrapOk;

	if
	  (
	   (
	        !m_lr_x_wrap
	     && !m_ul_x_wrap
	     &&  m_lowerRight.x < m_upperLeft.x
	   )
	   ||
	   (
	        !m_lr_y_wrap
	     && !m_ul_y_wrap
	     &&  m_lowerRight.y < m_upperLeft.y
	   )
	  )
	{
		return false;
	}

	if((m_lr_x_wrap || m_ul_x_wrap) && (m_lowerRight.x > m_upperLeft.x))
	{
		m_upperLeft.x  = 0;
		m_lowerRight.x = m_mapSize.x;
	}

	if((m_lr_y_wrap || m_ul_y_wrap) && (m_lowerRight.y > m_upperLeft.y))
	{
		m_upperLeft.y  = 0;
		m_lowerRight.y = m_mapSize.y;
	}

	return true;
}

sint16 BoundingRect::GetMaxRows()
{
	sint16 max_rows;

	if (m_ul_y_wrap || m_lr_y_wrap)
	{
		max_rows = m_lowerRight.y;

		max_rows += m_mapSize.y - m_upperLeft.y;
	}
	else
	{
		max_rows = m_lowerRight.y - m_upperLeft.y;
	}

	return max_rows;
}

sint16 BoundingRect::GetMaxCols()
{
	sint16 max_columns;

	if (m_ul_x_wrap || m_lr_x_wrap)
	{
		max_columns = m_lowerRight.x;

		max_columns += m_mapSize.x - m_upperLeft.x;
	}
	else
	{
		max_columns = m_lowerRight.x - m_upperLeft.x;
	}

	return max_columns;
}

void BoundingRect::Get(sint16 row, sint16 col, MapPoint & pos)
{
	pos.x = col + m_upperLeft.x;
	if(pos.x >= m_mapSize.x) pos.x -= m_mapSize.x;

	pos.y = row + m_lowerRight.x;
	if(pos.y >= m_mapSize.y) pos.y -= m_mapSize.y;
}


bool BoundingRect::Get(const sint32 & index, MapPoint & pos, const sint32 & max_rows, const sint32 & max_columns) const
{
	pos.y = (sint16)(index / max_columns);
	pos.x = (sint16)(index - (pos.y * max_columns));

	if(pos.y >= max_rows)
		return false;

	pos.y = ((pos.y + m_upperLeft.y) % m_mapSize.y);
	pos.x = ((pos.x + m_upperLeft.x) % m_mapSize.x);

#ifdef _DEBUG
	if(pos.y >= m_mapSize.y)
	{
		Assert(false);
		return false;
	}

	if (pos.x >= m_mapSize.x)
	{
		Assert(false);
		return false;
	}
#endif _DEBUG

	return true;
}

bool BoundingRect::Expand(const sint16 & size)
{
	if
	  (
	       m_upperLeft.y  != 0
	    || m_lowerRight.y != m_mapSize.y
	  )
	{
		m_upperLeft.y  -= size;
		m_lowerRight.y += size;
	}

	if
	  (
	       m_upperLeft.x  != 0
	    || m_lowerRight.x != m_mapSize.x
	  )
	{
		m_upperLeft.x  -= size;
		m_lowerRight.x += size;
	}

	bool err = Normalize();
	Assert(err);

	return err;
}

bool BoundingRect::IsMaxRect() const
{
	return
	      (
	           m_upperLeft.y  == 0
	        && m_lowerRight.y == m_mapSize.y
	        && m_upperLeft.x  == 0
	        && m_lowerRight.x == m_mapSize.x
	      );
}

MapPoint BoundingRect::GetCenter() const
{
	MapPoint center;
	sint16 width;

	if(m_lowerRight.x > m_upperLeft.x)
	{
		width    = m_lowerRight.x - m_upperLeft.x;
		center.x = m_upperLeft.x + (width / 2);
	}
	else
	{
		width    = (m_mapSize.x - m_upperLeft.x) + m_lowerRight.x;
		center.x = m_upperLeft.x + (width / 2);

		center.x = center.x > m_mapSize.x ? center.x - m_mapSize.x : center.x;
	}

	if(m_lowerRight.y > m_upperLeft.y)
	{
		width    = m_lowerRight.y - m_upperLeft.y;
		center.y = m_upperLeft.y + (width / 2);
	}
	else
	{
		width    = (m_mapSize.y - m_upperLeft.y) + m_lowerRight.y;
		center.x = m_upperLeft.y + (width / 2);

		center.y = (center.y > m_mapSize.y ? center.y - m_mapSize.y : center.y);
	}

	return center;
}

bool BoundingRect::WithinRectangle(const MapPoint & pos) const
{
	if(pos.x < m_upperLeft.x && pos.x >= m_lowerRight.x)
		return false;

	if(pos.y < m_upperLeft.y && pos.y >= m_lowerRight.y)
		return false;

	return true;
}
