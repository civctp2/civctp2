










#ifndef __AUI_HYPERTEXTBASE_H__
#define __AUI_HYPERTEXTBASE_H__


#include "tech_wllist.h"


class aui_Static;
class aui_Surface;
class aui_BitmapFont;



#define k_AUI_HYPERTEXTBASE_DEFAULT_MAXLEN		1024
#define k_AUI_HYPERTEXTBOX_LDL_MAXSTATICS		100



#define k_AUI_HYPERTEXTBASE_LDL_TEXT			"hypertext"
#define k_AUI_HYPERTEXTBASE_LDL_MAXLEN			"hypermaxlength"



class aui_HyperTextBase
{
public:
	
	aui_HyperTextBase(
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock );
	aui_HyperTextBase(
		AUI_ERRCODE *retval,
		MBCHAR *hyperText,
		uint32 hyperMaxLen );
	virtual ~aui_HyperTextBase();

protected:
	aui_HyperTextBase() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(
		MBCHAR *hyperText,
		uint32 hyperMaxLen );

public:
	MBCHAR *GetHyperText( void ) const { return (MBCHAR *)m_hyperText; }
	virtual AUI_ERRCODE	SetHyperText(
		const MBCHAR *hyperText,
		uint32 maxlen = 0xffffffff );
	virtual AUI_ERRCODE	AppendHyperText( const MBCHAR *hyperText );

protected:
	virtual AUI_ERRCODE AddHyperStatics( const MBCHAR *hyperText );
	void RemoveHyperStatics( void );
	static aui_Static *CreateHyperStatic(
		const MBCHAR *string,
		uint32 len,
		MBCHAR ttffile[ MAX_PATH + 1 ],
		sint32 pointSize,
		sint32 bold,
		sint32 italic,
		COLORREF color,
		sint32 underline,
		BOOL shadow,
		COLORREF shadowColor,
		uint32 flags );

	AUI_ERRCODE DrawThisHyperText(
		aui_Surface *destSurf,
		RECT *clipRect,
		sint32 x = 0,
		sint32 y = 0 );

	MBCHAR	*m_hyperText;
	uint32	m_hyperMaxLen;
	uint32	m_hyperCurLen;

	tech_WLList<aui_Static *>	*m_hyperStaticList;

	
	MBCHAR		m_hyperTtffile[ MAX_PATH + 1 ];
	sint32		m_hyperPointSize;
	sint32		m_hyperBold;
	sint32		m_hyperItalic;
	COLORREF	m_hyperColor;
	COLORREF	m_hyperColorOld;
	sint32		m_hyperUnderline;
	BOOL		m_hyperShadow;
	COLORREF	m_hyperShadowColor;
	uint32		m_hyperFlags;
};


#endif 
