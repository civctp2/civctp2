//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description	: Wonder movie pop-up window
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Memory leak repaired.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_moviebutton.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_movie.h"

#include "c3window.h"
#include "Pattern.h"
#include "PatternBase.h"
#include "c3_static.h"

#include "WonderMovieWindow.h"
#include "wondermoviewin.h"
#include "ctp2_hypertextbox.h"



WonderMovieWindow::WonderMovieWindow(
									 AUI_ERRCODE *retval,
									 uint32 id,
									 MBCHAR *ldlBlock,
									 sint32 bpp,
									 AUI_WINDOW_TYPE type )
									 :
C3Window(retval, id, ldlBlock, bpp, type)
{
	m_sequence = NULL;

	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



WonderMovieWindow::WonderMovieWindow(
									 AUI_ERRCODE *retval,
									 uint32 id,
									 sint32 x,
									 sint32 y,
									 sint32 width,
									 sint32 height,
									 sint32 bpp,
									 MBCHAR *pattern,
									 AUI_WINDOW_TYPE type)
									 :
C3Window( retval, id, x, y, width, height, bpp, pattern, type )
{
	m_sequence = NULL;

	*retval = InitCommon();
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}



WonderMovieWindow::~WonderMovieWindow()
{
	delete m_movieButton;
	delete m_wonderName;
	delete m_topBorder;
	delete m_leftBorder;
	delete m_rightBorder;
	delete m_bottomBorder;
	delete m_textBox;
}




AUI_ERRCODE WonderMovieWindow::InitCommonLdl(MBCHAR *ldlBlock)
{
	MBCHAR			buttonBlock[k_AUI_LDL_MAXBLOCK+1];
	AUI_ERRCODE		errcode;

	sprintf(buttonBlock, "%s.%s", ldlBlock, "MovieButton");
	m_movieButton = new aui_MovieButton(&errcode, aui_UniqueId(), buttonBlock, wondermoviewin_MovieButtonCallback);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_movieButton);

	sprintf(buttonBlock, "%s.%s", ldlBlock, "WonderName");
	m_wonderName = new c3_Static(&errcode, aui_UniqueId(), buttonBlock);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_wonderName);

	sprintf(buttonBlock, "%s.%s", ldlBlock, "TopBorder");
	m_topBorder = new c3_Static(&errcode, aui_UniqueId(), buttonBlock);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_topBorder);

	sprintf(buttonBlock, "%s.%s", ldlBlock, "LeftBorder");
	m_leftBorder = new c3_Static(&errcode, aui_UniqueId(), buttonBlock);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_leftBorder);

	sprintf(buttonBlock, "%s.%s", ldlBlock, "RightBorder");
	m_rightBorder = new c3_Static(&errcode, aui_UniqueId(), buttonBlock);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_rightBorder);

	sprintf(buttonBlock, "%s.%s", ldlBlock, "BottomBorder");
	m_bottomBorder = new c3_Static(&errcode, aui_UniqueId(), buttonBlock);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_bottomBorder);

	sprintf(buttonBlock, "%s.%s", ldlBlock, "Text");
	m_textBox = new ctp2_HyperTextBox(&errcode, aui_UniqueId(), buttonBlock);
	Assert(errcode == AUI_ERRCODE_OK);
	if(errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_textBox);

	return InitCommon();
}


AUI_ERRCODE WonderMovieWindow::InitCommon(void)
{		
	return C3Window::InitCommon();
}



void WonderMovieWindow::SetMovie(const MBCHAR *filename)
{
	if (!m_movieButton) return;

	m_movieButton->SetMovie(filename);
	m_movieButton->SetFlags(k_AUI_MOVIE_PLAYFLAG_PLAYANDHOLD);
}



void WonderMovieWindow::SetWonderName(MBCHAR *name)
{
	if (m_wonderName)
		m_wonderName->SetText(name);
}

void WonderMovieWindow::SetText(const MBCHAR *text)
{
	if(m_textBox)
		m_textBox->SetHyperText(text);
}



AUI_ERRCODE WonderMovieWindow::Idle()
{
	
	
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE WonderMovieWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;
	

	if (m_pattern) {
		RECT rect = { 0, 0, m_width, m_height };
		m_pattern->Draw( m_surface, &rect );

		
		m_dirtyList->AddRect( &rect );
	}

	return AUI_ERRCODE_OK;
}
