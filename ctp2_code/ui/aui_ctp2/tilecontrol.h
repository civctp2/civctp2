













#pragma once
#ifndef __TILECONTROL_H__
#define __TILECONTROL_H__



class aui_Surface;
class MapPoint;



class TileControl : public aui_Control
{
public:
	
	TileControl(AUI_ERRCODE *retval,
						uint32 id,
						MBCHAR *ldlBlock );
	TileControl(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~TileControl() {};

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void SetMouseTile(const MapPoint &p) { m_currentTile = p; };

protected:
	MapPoint			m_currentTile;

	sint32 DrawTile(aui_Surface *surface,
			sint32 i,	
			sint32 j,	
			sint32 x,
			sint32 y);
};


#endif 
