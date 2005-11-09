











#include "c3.h"
#include "ActorPath.h"

#include "maputils.h"           // maputils_MapXY2PixelXY
#include "spriteutils.h"        // spriteutils_DeltaToFacing


ActorPath::ActorPath(uint16 numPoints)
{
	Assert(numPoints < POINTSPOSTYPE_MAX);
	if(numPoints >= POINTSPOSTYPE_MAX) return;
	
	m_numPoints = numPoints;
	
	m_points = new POINT[numPoints];
	m_pos = new POINT[numPoints];

	Assert(m_points != NULL && m_pos != NULL);
	if (m_points == NULL || m_pos == NULL) return;

	m_finished = FALSE;
}

ActorPath::ActorPath(sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
	m_numPoints = POINTSPOSTYPE_MAX;
	
	m_points = new POINT[m_numPoints];
	m_pos = new POINT[m_numPoints];
	
	Assert(m_points != NULL && m_pos != NULL);
	if (m_points == NULL || m_pos == NULL) return;

	m_pos[POINTSPOSTYPE_STARTPOS].x = x1;
	m_pos[POINTSPOSTYPE_STARTPOS].y = y1;
	m_pos[POINTSPOSTYPE_ENDPOS].x = x2;
	m_pos[POINTSPOSTYPE_ENDPOS].y = y2;

	m_finished = FALSE;
}

ActorPath::~ActorPath()
{
	delete m_points;
	delete m_pos;
}











void ActorPath::SetPos(uint16 pointNum, sint32 x, sint32 y)
{
	Assert(pointNum < m_numPoints);
	if (pointNum >= m_numPoints) return;

	POINT p = {x, y};

	m_pos[pointNum] = p;
}

void ActorPath::CalcPosition(sint32 start, sint32 end, sint32 current, POINT *pos)
{
	if (start == end) {
		*pos = m_pos[POINTSPOSTYPE_STARTPOS];
		return;
	}

	Assert(m_points != NULL && m_pos != NULL);
	if (m_points == NULL || m_pos == NULL) return;

	
	
	maputils_MapXY2PixelXY(m_pos[POINTSPOSTYPE_STARTPOS].x, m_pos[POINTSPOSTYPE_STARTPOS].y, 
							&(m_points[POINTSPOSTYPE_STARTPOS].x), &(m_points[POINTSPOSTYPE_STARTPOS].y));
	maputils_MapXY2PixelXY(m_pos[POINTSPOSTYPE_ENDPOS].x, m_pos[POINTSPOSTYPE_ENDPOS].y, 
							&(m_points[POINTSPOSTYPE_ENDPOS].x), &(m_points[POINTSPOSTYPE_ENDPOS].y));

	double const rat = (double) current / (double)(end - start);

	pos->x = m_points[POINTSPOSTYPE_STARTPOS].x + (uint32)((m_points[POINTSPOSTYPE_ENDPOS].x - m_points[POINTSPOSTYPE_STARTPOS].x) * rat);
	pos->y = m_points[POINTSPOSTYPE_STARTPOS].y + (uint32)((m_points[POINTSPOSTYPE_ENDPOS].y - m_points[POINTSPOSTYPE_STARTPOS].y) * rat);
}

sint32 ActorPath::CalcFacing(sint32 start, sint32 end, sint32 current) const
{
	sint32 const x = m_points[POINTSPOSTYPE_ENDPOS].x - m_points[POINTSPOSTYPE_STARTPOS].x;
	sint32 const y = m_points[POINTSPOSTYPE_ENDPOS].y - m_points[POINTSPOSTYPE_STARTPOS].y;

	return spriteutils_DeltaToFacing(x, y);
}
