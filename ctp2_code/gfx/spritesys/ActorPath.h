











#pragma once
#ifndef __ACTORPATH_H__
#define __ACTORPATH_H__


enum POINTSPOSTYPE {
	POINTSPOSTYPE_STARTPOS,
	POINTSPOSTYPE_ENDPOS,

	POINTSPOSTYPE_MAX
};

class ActorPath
{
public:
	ActorPath(uint16 numPoints);
	ActorPath(sint32 x1, sint32 y1, sint32 x2, sint32 y2);
	~ActorPath();


	void SetPos(uint16 pointNum, sint32 x, sint32 y);

	void CalcPosition(sint32 start, sint32 end, sint32 current, POINT *pos);
	void CalcFacing(sint32 start, sint32 end, sint32 current, uint16 *facing);

	BOOL	Finished(void) { return m_finished; }
	void	SetFinished(BOOL finished) { m_finished = finished; }

protected:
	uint16		m_numPoints;
	POINT		*m_points;
	POINT		*m_pos;
	uint16		m_numFrames;
	BOOL		m_finished;
};

#endif