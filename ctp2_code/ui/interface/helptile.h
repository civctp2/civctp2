#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef THIS_IS_MY_CONSTANT_SO_YOU_BETTER_NOT_USE_IT
#define  THIS_IS_MY_CONSTANT_SO_YOU_BETTER_NOT_USE_IT

class c3_ListItem;
class MapPoint;

sint32 helptile_Initialize();
void helptile_Cleanup( void );
void helptile_displayData(const MapPoint&);
void helptile_setPosition(const MapPoint&);
bool helptile_isShown(void);
void helptile_closeWindow(void);

#include "c3_listitem.h"

class TileImprovementListItem : public c3_ListItem
{
public:
	TileImprovementListItem(AUI_ERRCODE *retval, const MBCHAR *ldlBlock, sint32 time, uint32 name);
	void Update();
private:
	sint32 m_time;
	sint32 m_name;
};

#endif // THIS_IS_MY_CONSTANT_SO_YOU_BETTER_NOT_USE_IT
