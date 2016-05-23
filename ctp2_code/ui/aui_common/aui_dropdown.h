//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface drop down box
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
// _MSC_VER
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
//
//----------------------------------------------------------------------------

#ifndef __AUI_DROPDOWN_H__
#define __AUI_DROPDOWN_H__

#include "aui_control.h"
#include "aui_listbox.h"

class aui_Surface;
class aui_Button;
class aui_Window;
class aui_Item;
class aui_Static;


#define k_AUI_DROPDOWN_LDL_BUTTONSIZE	"buttonsize"
#define k_AUI_DROPDOWN_LDL_WINDOWSIZE	"windowsize"
#define k_AUI_DROPDOWN_LDL_BUTTON		"button"
#define k_AUI_DROPDOWN_LDL_LISTBOX		"listbox"
#define k_AUI_DROPDOWN_LDL_WINDOW		"window"
#define k_AUI_DROPDOWN_LDL_STATICPANE	"staticpane"
#define k_AUI_DROPDOWN_LDL_ALWAYSPOPUP	"alwayspopup"
#define k_AUI_DROPDOWN_LDL_INVERTED     "inverted"

enum AUI_DROPDOWN_ACTION
{
	AUI_DROPDOWN_ACTION_FIRST = 0,
	AUI_DROPDOWN_ACTION_NULL = 0,
	AUI_DROPDOWN_ACTION_SELECT,
	AUI_DROPDOWN_ACTION_LAST
};


class aui_DropDown : public aui_Control
{
public:

	aui_DropDown(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_DropDown(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 buttonSize = 0,
		sint32 windowSize = 0,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_DropDown();

protected:
	aui_DropDown() : aui_Control() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( sint32 buttonSize, sint32 windowSize );
	AUI_ERRCODE CreateComponents( MBCHAR *ldlBlok = NULL );

public:

	BOOL ExtractEndUserTriggeredEvent( uint32 data ) const
	{ return (data & 0x1) == 0; }

	virtual AUI_ERRCODE	Resize( sint32 width, sint32 height );

	virtual AUI_ERRCODE	Hide( void );

	aui_Button	*GetButton( void ) const { return m_button; }
	aui_ListBox	*GetListBox( void ) const { return m_listBox; }
	aui_Window	*GetListBoxWindow( void ) const { return m_listBoxWindow; }

	aui_Static	*GetStaticPane( void ) const { return m_staticPane; }

	sint32		GetButtonSize( void ) const { return m_buttonSize; }
	AUI_ERRCODE	SetButtonSize( sint32 buttonSize )
		{ m_buttonSize = buttonSize; return RepositionButton(); }

	void		SetAlwaysPopup( BOOL always ) { m_alwaysPopup = always; }

	AUI_ERRCODE	AddItem( aui_Item *item );
	AUI_ERRCODE	RemoveItem( uint32 itemId );
	aui_Item	*GetItem( uint32 itemId ) const
		{ return (aui_Item *)m_listBox->GetChild( itemId ); }

	sint32		GetSelectedItem( void ) const {
            //printf("%s L%d: m_selectedItem= %d!\n", __FILE__, __LINE__, m_selectedItem);
            return m_selectedItem;
            }
	sint32		SetSelectedItem( sint32 itemIndex, uint32 data = 0 );

	sint32		GetWindowSize( void ) const { return m_windowSize; }
	sint32		SetWindowSize( sint32 windowSize );

	AUI_ERRCODE	ShowListBoxWindow( BOOL showIt );
	AUI_ERRCODE	ToggleListBoxWindow( void );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	AUI_ERRCODE UpdateSelectedItem( BOOL update = TRUE );

protected:
	virtual AUI_ERRCODE	RepositionButton( void );
	virtual AUI_ERRCODE	RepositionListBoxWindow( void );

	AUI_ERRCODE	DrawSelectedItem( aui_Surface *surface, sint32 x, sint32 y );

	aui_Button	*m_button;
	aui_ListBox	*m_listBox;
	aui_Window	*m_listBoxWindow;

	aui_Static	*m_staticPane;

	sint32		m_buttonSize;
	sint32		m_windowSize;

	BOOL		m_alwaysPopup;
	BOOL        m_inverted;

	sint32		m_selectedItem;

	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLGrabOutside(aui_MouseEvent * mouseData);

	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData);
};

aui_Control::ControlActionCallback DropDownButtonActionCallback;
aui_Control::ControlActionCallback DropDownListBoxActionCallback;

#endif
