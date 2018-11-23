//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Victory movie window (win lose video)
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

#include "victorymoviewindow.h"
#include "victorymoviewin.h"


VictoryMovieWindow::VictoryMovieWindow(
									 AUI_ERRCODE *retval,
									 uint32 id,
									 const MBCHAR *ldlBlock,
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


VictoryMovieWindow::VictoryMovieWindow(
									 AUI_ERRCODE *retval,
									 uint32 id,
									 sint32 x,
									 sint32 y,
									 sint32 width,
									 sint32 height,
									 sint32 bpp,
									 const MBCHAR *pattern,
									 AUI_WINDOW_TYPE type)
									 :
C3Window( retval, id, x, y, width, height, bpp, pattern, type )
{
	m_sequence = NULL;

	*retval = InitCommon();
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}


VictoryMovieWindow::~VictoryMovieWindow()
{
	if (m_movieButton)
		delete m_movieButton;





}




AUI_ERRCODE VictoryMovieWindow::InitCommonLdl(const MBCHAR *ldlBlock)
{
	MBCHAR			buttonBlock[k_AUI_LDL_MAXBLOCK+1];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf(buttonBlock, "%s.%s", ldlBlock, "MovieButton");
	m_movieButton = new aui_MovieButton(&errcode, aui_UniqueId(), buttonBlock, victorymoviewin_MovieButtonCallback);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return AUI_ERRCODE_LOADFAILED;
	m_movieButton->SetFullScreen(true);

	AddControl(m_movieButton);







	return InitCommon();
}

AUI_ERRCODE VictoryMovieWindow::InitCommon(void)
{
	return C3Window::InitCommon();
}


void VictoryMovieWindow::SetMovie(const MBCHAR *filename)
{
	if (!m_movieButton) return;

	m_movieButton->SetMovie(filename);
}


void VictoryMovieWindow::SetVictoryName(const MBCHAR *name)
{
	if (m_wonderName)
		m_wonderName->SetText(name);
}


AUI_ERRCODE VictoryMovieWindow::Idle()
{

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE VictoryMovieWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;








	return AUI_ERRCODE_OK;
}
