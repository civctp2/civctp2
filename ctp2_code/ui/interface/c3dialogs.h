#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3DIALOGS_H__
#define __C3DIALOGS_H__

class ForeignTradeBidInfo;

#include "ctp2_inttypes.h"
#include "Unit.h"

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
void c3dialogs_ForeignTradeBidDialogCallback
(
    MBCHAR const *  text,
    sint32          accepted,
    void *          data
);

#endif
