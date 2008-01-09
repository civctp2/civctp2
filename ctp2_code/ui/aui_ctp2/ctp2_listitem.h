//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : CTP2 list item GUI component
// Id           : $Id net_civ.cpp 442 2005-08-28 18:05:04Z Martin GÃ¼hmann $
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
// - Added own Draw method. (2-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __CTP2_LISTITEM_H__
#define __CTP2_LISTITEM_H__

class ctp2_ListItem;

#include "aui_item.h"       // aui_Item
#include "auitypes.h"       // AUI_ERRCODE
#include "ctp2_inttypes.h"  // sint32
#include "patternbase.h"    // PatternBase

class aui_Surface;

class ctp2_ListItem : public aui_Item, public PatternBase
{
public:
	typedef sint32 (CTP2ItemCompareCallback)(ctp2_ListItem *item1,
											 ctp2_ListItem *item2,
											 sint32 column);

	void *                      m_userData;
	CTP2ItemCompareCallback *   m_compareCallback;

	
	ctp2_ListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock);
	virtual ~ctp2_ListItem();

	virtual AUI_ERRCODE Draw( aui_Surface *surface, sint32 x, sint32 y );
	
	
	
	
	
	virtual void Update(void) {}

	
	
	
	
	
	
	
	
	
	
	
	
	virtual sint32 Compare(ctp2_ListItem *item2, uint32 column);

	
	
	void SetUserData(void *data) { m_userData = data; }
	void *GetUserData() { return m_userData; }
	
	void SetCompareCallback(CTP2ItemCompareCallback *cb) { m_compareCallback = cb; }

protected:
	ctp2_ListItem() 
    : 
        aui_Item            (), 
        PatternBase         (),
        m_userData          (NULL),
        m_compareCallback   (NULL)
    { ; };

};


#endif
