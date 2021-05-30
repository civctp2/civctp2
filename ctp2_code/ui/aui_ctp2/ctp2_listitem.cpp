//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : CTP2 list item GUI component
// Id           : $Id net_civ.cpp 442 2005-08-28 18:05:04Z Martin Gühmann $
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added own Draw method. (2-Jan-2008 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "ctp2_listitem.h"

#include "aui.h"
#include "aui_item.h"
#include "aui_uniqueid.h"
#include "pattern.h"
#include "patternbase.h"

ctp2_ListItem::ctp2_ListItem(AUI_ERRCODE *retval, const MBCHAR *ldlBlock)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Item            (retval, aui_UniqueId(), ldlBlock),
	PatternBase         (ldlBlock, (MBCHAR *) NULL),
    m_userData          (NULL),
    m_compareCallback   (NULL)
{
}

ctp2_ListItem::~ctp2_ListItem()
{
	DeleteChildren();
}

AUI_ERRCODE ctp2_ListItem::Draw(aui_Surface *surface, sint32 x, sint32 y)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if(!IsHidden())
	{
		errcode = DrawThis(surface, x, y);

		if(errcode == AUI_ERRCODE_OK)
			DrawChildren( surface, x, y );
	}

	m_draw = 0;

	return AUI_ERRCODE_OK;
}

sint32 ctp2_ListItem::Compare(ctp2_ListItem *item2, uint32 column)
{
	if (m_compareCallback)
	{
		return m_compareCallback(this, item2, column);
	}

	return 0;
}
