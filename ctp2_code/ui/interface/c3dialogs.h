
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3DIALOGS_H__
#define __C3DIALOGS_H__

#include "Unit.h"

#include "c3_utilitydialogbox.h"




class ForeignTradeBidInfo 
{
public:
	ForeignTradeBidInfo(sint32 player, Unit &fromCity, Unit &toCity, sint32 resource);

	sint32		m_player;
	Unit		m_fromCity;
	Unit		m_toCity;
	sint32		m_resource;
};

void c3dialogs_PostForeignTradeBidDialog(sint32 player, Unit &fromCity, Unit &toCity, sint32 resource);
void c3dialogs_ForeignTradeBidDialogCallback(MBCHAR *text, sint32 accepted, void *data);





#endif
