#ifndef __AUI_TEXTBOX_H__
#define __AUI_TEXTBOX_H__

#include "aui_listbox.h"

class aui_Static;

#define k_AUI_TEXTBOX_MAXTEXT		256
#define k_AUI_TEXTBOX_MAXITEMS		100


class aui_TextBox : public aui_ListBox
{
public:

	aui_TextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_TextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_TextBox();

protected:
	aui_TextBox() : aui_ListBox() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );

public:
	virtual AUI_ERRCODE	SetText(
		const MBCHAR *text,
		uint32 maxlen = 0xffffffff );
	virtual AUI_ERRCODE	AppendText(MBCHAR const *text);

	virtual AUI_ERRCODE AppendText(
		MBCHAR const *  text,
		COLORREF color,
		sint32 bold = 0,
		sint32 italic = 0 );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	virtual void SetTextFont( MBCHAR const * ttffile );
	virtual void SetTextFontSize( sint32 pointSize );

protected:

	AUI_ERRCODE	CalculateItems(MBCHAR const * text = NULL);
	AUI_ERRCODE	CalculateAppendedItems(MBCHAR const * text = NULL);

	sint32		m_numItems;
	sint32		m_curItem;
	aui_Static	*m_items[ k_AUI_TEXTBOX_MAXITEMS ];

	COLORREF	m_curColor;
	sint32		m_curBold;
	sint32		m_curItalic;
};

#endif
