#ifndef __NS_TRIBES_H__
#define __NS_TRIBES_H__

class AllinoneWindow;

class ns_Tribes;
extern ns_Tribes *g_nsTribes;

class ns_HPlayerItem;

#define k_TRIBES_MAX 100

#include "ns_item.h"
#include "c3_dropdown.h"
#include "aui_stringtable.h"

class ns_Tribes
{
public:
	ns_Tribes();
	virtual ~ns_Tribes();

	sint32 GetNumTribes( void ) const { return m_stringtable->GetNumStrings(); }
	aui_StringTable *GetStrings( void ) const { return m_stringtable; }

private:
	aui_StringTable *m_stringtable;
};


class ns_TribesDropDown : public c3_DropDown
{
public:
	ns_TribesDropDown(
		AUI_ERRCODE *retval,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_TribesDropDown();
};

#endif
