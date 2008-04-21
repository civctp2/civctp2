//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Id           : $Id:$
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added functionality to support pattern information saving
//
//----------------------------------------------------------------------------
//
/// \file c3_popupwindow.cpp
/// \brief Pop-up window (definitions)

#include "c3.h"
#include "c3_popupwindow.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3window.h"
#include "ctp2_button.h"
#include "keypress.h"
#include "pattern.h"
#include "primitives.h"
#include "UIUtils.h"


c3_PopupWindow::c3_PopupWindow
(
	AUI_ERRCODE *   retval,
	uint32          id,
	MBCHAR *        ldlBlock,
	sint32          bpp,
	AUI_WINDOW_TYPE type,
	bool            bevel
)
:
	C3Window    (retval, id, ldlBlock, bpp, type, bevel),
	m_title     (NULL),
	m_titleText (NULL),
	m_cancel    (NULL),
	m_ok        (NULL)
{
	std::fill(m_border, m_border + POPUP_BORDER_MAX, (c3_Static *) NULL);

	*retval = InitCommon();
}

c3_PopupWindow::c3_PopupWindow
(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type,
	bool bevel
)
:
	C3Window    (retval, id, x, y, width, height, bpp, pattern, type, bevel),
	m_title     (NULL),
	m_titleText (NULL),
	m_cancel    (NULL),
	m_ok        (NULL)
{
	std::fill(m_border, m_border + POPUP_BORDER_MAX, (c3_Static *) NULL);

	*retval = InitCommon();
}


AUI_ERRCODE c3_PopupWindow::InitCommon(void)
{		
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_border[POPUP_BORDER_UL] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupUL");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_UL], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_UL], errcode) ) return errcode;
	m_border[POPUP_BORDER_UL]->Move( 0, 0 );

	m_border[POPUP_BORDER_UR] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupUR");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_UR], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_UR], errcode) ) return errcode;
	m_border[POPUP_BORDER_UR]->Move( m_width - m_border[POPUP_BORDER_UR]->Width(), 0 );

	m_border[POPUP_BORDER_LL] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupLL");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_LL], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_LL], errcode) ) return errcode;
	m_border[POPUP_BORDER_LL]->Move( 0, m_height - m_border[POPUP_BORDER_LL]->Height() );

	m_border[POPUP_BORDER_LR] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupLR");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_LR], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_LR], errcode) ) return errcode;
	m_border[POPUP_BORDER_LR]->Move( m_width - m_border[POPUP_BORDER_LR]->Width(), 
		m_height - m_border[POPUP_BORDER_LR]->Height() );

	sint32 cornerHeight = m_border[POPUP_BORDER_UL]->Height();
	sint32 cornerWidth = m_border[POPUP_BORDER_UL]->Width();

	m_border[POPUP_BORDER_LEFT] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupLeft");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_LEFT], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_LEFT], errcode) ) return errcode;
	m_border[POPUP_BORDER_LEFT]->Move( 0, cornerHeight );
	m_border[POPUP_BORDER_LEFT]->Resize( m_border[POPUP_BORDER_LEFT]->Width(), m_height - cornerHeight * 2 );

	m_border[POPUP_BORDER_TOP] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupTop");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_TOP], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_TOP], errcode) ) return errcode;
	m_border[POPUP_BORDER_TOP]->Move( cornerWidth, 0 );
	m_border[POPUP_BORDER_TOP]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_TOP]->Height() );

	m_border[POPUP_BORDER_RIGHT] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupRight");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_RIGHT], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_RIGHT], errcode) ) return errcode;
	m_border[POPUP_BORDER_RIGHT]->Move( m_width - m_border[POPUP_BORDER_RIGHT]->Width(), cornerHeight );
	m_border[POPUP_BORDER_RIGHT]->Resize( m_border[POPUP_BORDER_RIGHT]->Width(), m_height - cornerHeight * 2 );

	m_border[POPUP_BORDER_BOTTOM] = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupBottom");
	Assert( AUI_NEWOK(m_border[POPUP_BORDER_BOTTOM], errcode) );
	if ( !AUI_NEWOK(m_border[POPUP_BORDER_BOTTOM], errcode) ) return errcode;
	m_border[POPUP_BORDER_BOTTOM]->Move( cornerWidth, m_height - m_border[POPUP_BORDER_BOTTOM]->Height() );
	m_border[POPUP_BORDER_BOTTOM]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_BOTTOM]->Height() );

	for (int i = POPUP_BORDER_UL; i < POPUP_BORDER_MAX; ++i)
	{
		m_border[i]->IgnoreEvents( TRUE );
		AddControl( m_border[i] );
	}

	return C3Window::InitCommon();
}


AUI_ERRCODE c3_PopupWindow::Resize( sint32 width, sint32 height )
{
	AUI_ERRCODE errcode = C3Window::Resize(width, height);

	m_border[POPUP_BORDER_UL]->Move( 0, 0 );
	
	m_border[POPUP_BORDER_UR]->Move( m_width - m_border[POPUP_BORDER_UR]->Width(), 0 );
	
	m_border[POPUP_BORDER_LL]->Move( 0, m_height - m_border[POPUP_BORDER_LL]->Height() );

	m_border[POPUP_BORDER_LR]->Move( m_width - m_border[POPUP_BORDER_LR]->Width(), 
		m_height - m_border[POPUP_BORDER_LR]->Height() );

	sint32 cornerHeight = m_border[POPUP_BORDER_UL]->Height();
	sint32 cornerWidth = m_border[POPUP_BORDER_UL]->Width();

	m_border[POPUP_BORDER_LEFT]->Move( 0, cornerHeight );
	m_border[POPUP_BORDER_LEFT]->Resize( m_border[POPUP_BORDER_LEFT]->Width(), m_height - cornerHeight * 2 );

	m_border[POPUP_BORDER_TOP]->Move( cornerWidth, 0 );
	m_border[POPUP_BORDER_TOP]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_TOP]->Height() );

	m_border[POPUP_BORDER_RIGHT]->Move( m_width - m_border[POPUP_BORDER_RIGHT]->Width(), cornerHeight );
	m_border[POPUP_BORDER_RIGHT]->Resize( m_border[POPUP_BORDER_RIGHT]->Width(), m_height - cornerHeight * 2 );

	m_border[POPUP_BORDER_BOTTOM]->Move( cornerWidth, m_height - m_border[POPUP_BORDER_BOTTOM]->Height() );
	m_border[POPUP_BORDER_BOTTOM]->Resize( m_width - cornerWidth * 2, m_border[POPUP_BORDER_BOTTOM]->Height() );

	return errcode;
}



c3_PopupWindow::~c3_PopupWindow( void )
{
	for (int i = 0; i < POPUP_BORDER_MAX; ++i)
	{
		delete m_border[i];
	}

	delete m_title;
	delete m_titleText;
	delete m_cancel;
	delete m_ok;
}



AUI_ERRCODE c3_PopupWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if (!IsHidden())
	{
		RECT rect = { 0, 0, m_width, m_height };
		m_pattern->Draw( m_surface, &rect );
		m_dirtyList->AddRect( &rect );
	}

	return AUI_ERRCODE_OK;
}



sint32 c3_PopupWindow::AddTitle( MBCHAR *titleBlock )
{
	if (m_title)
	{
		// No action: already created
	}
	else
	{
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_title = new c3_Static(&errcode, aui_UniqueId(), "c3_PopupTitle");
		TestControl(m_title);

		m_titleText = 
			new c3_Static
		        (&errcode, 
		         aui_UniqueId(), 
		         (titleBlock) ? titleBlock : "c3_PopupTitle.c3_PopupTitleText"
		        );
		TestControl(m_titleText);

		m_title->AddSubControl(m_titleText);
		m_title->Move((m_width - m_title->Width()) / 2, 0);

		InsertChild(m_title, 0);
		m_title->SetParentWindow(this);
		m_title->SetBlindness(true);
	}

	return 1;
}



sint32 c3_PopupWindow::AddCancel
(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock
)
{
	if (m_cancel)
	{
		// No action: already created
	}
	else
	{
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_cancel = new c3_Button(&errcode, aui_UniqueId(), buttonBlock, actionFunc, cookie);
		TestControl(m_cancel);
		m_cancel->Move(17, m_height - m_cancel->Height() - 17);
		InsertChild(m_cancel, 0);
		m_cancel->SetParentWindow(this);
	}

	return 1;
}



sint32 c3_PopupWindow::AddOk
(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock
)
{
	if (m_ok)
	{
		// No action: already created
	}
	else
	{
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_ok = new ctp2_Button( &errcode, aui_UniqueId(), buttonBlock, actionFunc, cookie );
		TestControl( m_ok );
		m_ok->Move( m_width - m_ok->Width() - 17, m_height - m_ok->Height() - 17);
		InsertChild(m_ok, 0);
		m_ok->SetParentWindow(this);
	}

	return 1;
}


sint32 c3_PopupWindow::AddClose
(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock
)
{
	return AddOk(actionFunc, cookie, buttonBlock);
}

sint32 c3_PopupWindow::AddYes
(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock
)
{
	return AddOk(actionFunc, cookie, buttonBlock);
}

sint32 c3_PopupWindow::AddNo
(
	void (*actionFunc)( aui_Control *, uint32, uint32, void *),
	void *cookie,
	MBCHAR *buttonBlock
)
{
	return AddCancel(actionFunc, cookie, buttonBlock);
}


void c3_PopupWindow::kh_Close()
{
	if (m_cancel)
	{
		m_cancel->GetActionFunc()(m_cancel, AUI_BUTTON_ACTION_EXECUTE, 0, m_cancel->GetCookie());
	}
	else if (m_ok)
	{
		m_ok->GetActionFunc()(m_ok, AUI_BUTTON_ACTION_EXECUTE, 0, m_ok->GetCookie());
	}
	// else No action: no close handler assigned
}

/// Restore data from any previously saved pattern information
void c3_PopupWindow::PatternInfoRestore(void)
{
	if (Pattern * background = ThePattern())
	{
		background->SetFilename(m_patternInfo[POPUP_BORDER_MAX].c_str());
		background->Load();
	}

	for (int i = 0; i < POPUP_BORDER_MAX; ++i)
	{
		if (m_border[i])
		{
			if (Pattern * border = m_border[i]->ThePattern())
			{
				border->SetFilename(m_patternInfo[i].c_str());
				border->Load();
			}
		}
	}
}

/// Save pattern information for later restore
void c3_PopupWindow::PatternInfoSave(void)
{
	if (Pattern * background = ThePattern())
	{
		m_patternInfo[POPUP_BORDER_MAX] = background->GetFilename();
		background->Unload();
	}

	for (int i = 0; i < POPUP_BORDER_MAX; ++i)
	{
		if (m_border[i])
		{
			if (Pattern * border = m_border[i]->ThePattern())
			{
				m_patternInfo[i] = border->GetFilename();
				border->Unload();
			}
		}
	}
}

