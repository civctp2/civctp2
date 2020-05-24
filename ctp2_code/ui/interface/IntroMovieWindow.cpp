//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The intro movie window object
// Id           : $Id$
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
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_moviebutton.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"

#include "c3window.h"
#include "pattern.h"
#include "patternbase.h"
#include "c3_static.h"

#include "IntroMovieWindow.h"
#include "IntroMovieWin.h"


IntroMovieWindow::IntroMovieWindow(
									 AUI_ERRCODE *retval,
									 uint32 id,
									 MBCHAR *ldlBlock,
									 sint32 bpp,
									 AUI_WINDOW_TYPE type )
									 :
C3Window(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


IntroMovieWindow::IntroMovieWindow(
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
	*retval = InitCommon();
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}


IntroMovieWindow::~IntroMovieWindow()
{
	if (m_movieButton)
		delete m_movieButton;

}




AUI_ERRCODE IntroMovieWindow::InitCommonLdl(MBCHAR *ldlBlock)
{
	MBCHAR			buttonBlock[k_AUI_LDL_MAXBLOCK+1];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf(buttonBlock, "%s.%s", ldlBlock, "MovieButton");
	m_movieButton = new aui_MovieButton(&errcode, aui_UniqueId(), buttonBlock, intromoviewin_MovieButtonCallback);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	AddControl(m_movieButton);

	m_movieButton->SetFullScreen(true);

	return InitCommon();
}

AUI_ERRCODE IntroMovieWindow::InitCommon(void)
{
	return C3Window::InitCommon();
}


void IntroMovieWindow::SetMovie(MBCHAR *filename)
{
	if (!m_movieButton) return;

	m_movieButton->SetMovie(filename);
}




AUI_ERRCODE IntroMovieWindow::Idle()
{

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE IntroMovieWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;








	return AUI_ERRCODE_OK;
}
