#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TILECONTROL_H__
#define __TILECONTROL_H__

class aui_Surface;
class MapPoint;
class GoodActor;

class TileControl : public aui_Control
{
public:
	TileControl(AUI_ERRCODE * retval, uint32 id, MBCHAR * ldlBlock);
	TileControl(AUI_ERRCODE * retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height,
			 ControlActionCallback * ActionFunc = NULL, Cookie cookie = NULL);
	virtual ~TileControl();

	virtual AUI_ERRCODE DrawThis(aui_Surface * surface = NULL, sint32 x = 0, sint32 y = 0);

	void SetMouseTile(const MapPoint & pos);

protected:
	virtual void PostChildrenCallback(aui_MouseEvent * mouseData);

private:
	bool DrawTile(aui_Surface * surface, sint32 x, sint32 y);

	MapPoint    m_currentTile;
	GoodActor * m_goodActor;
	uint32      m_nextProcessTime;
};

#endif
