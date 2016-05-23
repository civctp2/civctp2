#ifndef __AUI_TABGROUP_H__
#define __AUI_TABGROUP_H__

#include "aui_switchgroup.h"

class aui_Tab;


enum AUI_TABGROUP_ALIGNMENT
{
	AUI_TABGROUP_ALIGNMENT_FIRST = 0,
	AUI_TABGROUP_ALIGNMENT_LEFT = 0,
	AUI_TABGROUP_ALIGNMENT_TOP,
	AUI_TABGROUP_ALIGNMENT_RIGHT,
	AUI_TABGROUP_ALIGNMENT_BOTTOM,
	AUI_TABGROUP_ALIGNMENT_LAST
};


#define k_AUI_TABGROUP_LDL_AUTOARRANGE		"autoarrange"
#define k_AUI_TABGROUP_LDL_PANEOFFSETX		"paneoffsetx"
#define k_AUI_TABGROUP_LDL_PANEOFFSETY		"paneoffsety"
#define k_AUI_TABGROUP_LDL_ALIGNMENT		"alignment"
#define k_AUI_TABGROUP_LDL_LEFT				"left"
#define k_AUI_TABGROUP_LDL_TOP				"top"
#define k_AUI_TABGROUP_LDL_RIGHT			"right"
#define k_AUI_TABGROUP_LDL_BOTTOM			"bottom"


class aui_TabGroup : public aui_SwitchGroup
{
public:

	aui_TabGroup(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	aui_TabGroup(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		AUI_TABGROUP_ALIGNMENT alignment = AUI_TABGROUP_ALIGNMENT_TOP,
		BOOL autoArrange = TRUE,
		sint32 paneOffsetX = 0,
		sint32 paneOffsetY = 0 );
	virtual ~aui_TabGroup() {}

protected:
	aui_TabGroup() : aui_SwitchGroup() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(
		AUI_TABGROUP_ALIGNMENT alignment,
		BOOL autoArrange,
		sint32 paneOffsetX,
		sint32 paneOffsetY );

public:
	AUI_ERRCODE	AddTab( aui_Tab *tab ) { return AddChild( (aui_Region *)tab ); }
	AUI_ERRCODE RemoveTab( uint32 tabId ) { return RemoveChild( tabId ); }
	aui_Tab		*GetTab( uint32 tabId ) { return (aui_Tab *)GetChild( tabId ); }
	virtual		AUI_ERRCODE	AddChild( aui_Region *child );
	virtual		AUI_ERRCODE	RemoveChild( uint32 tabId );

	AUI_TABGROUP_ALIGNMENT GetTabGroupAlignment( void ) const
		{ return m_alignment; }
	AUI_TABGROUP_ALIGNMENT SetTabGroupAlignment(
		AUI_TABGROUP_ALIGNMENT alignment );

protected:

	virtual AUI_ERRCODE	FormatTabs( void );

	AUI_TABGROUP_ALIGNMENT	m_alignment;
	BOOL					m_autoArrange;

	sint32 m_paneOffsetX;
	sint32 m_paneOffsetY;
};

#endif
