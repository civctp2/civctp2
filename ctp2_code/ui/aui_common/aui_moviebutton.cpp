#include "c3.h"
#include "aui_moviebutton.h"

#include "aui_ui.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_movie.h"

#include "c3ui.h"
extern C3UI			*g_c3ui;

#include "profileDB.h"
extern ProfileDB	*g_theProfileDB;

#include "ldl_data.hpp"

aui_MovieButton::aui_MovieButton(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Button( retval, id, ldlBlock, ActionFunc, cookie )
{
	m_flags = 0;
	m_fullScreen = false;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

aui_MovieButton::aui_MovieButton(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *movie,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Button( retval, id, x, y, width, height, ActionFunc, cookie )
{
	m_flags = 0;
	m_fullScreen = false;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( movie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE aui_MovieButton::InitCommonLdl( const MBCHAR *ldlBlock )
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon(
		block->GetString( k_AUI_MOVIEBUTTON_LDL_MOVIE ) );
	Assert( AUI_SUCCESS(errcode) );
	return errcode;
}

AUI_ERRCODE aui_MovieButton::InitCommon( const MBCHAR *movie )
{
	m_movie = NULL;

	SetMovie( movie );


	m_drawMask = 0;

	return AUI_ERRCODE_OK;
}

aui_MovieButton::~aui_MovieButton()
{
	if ( m_movie )
	{
		g_ui->UnloadMovie( m_movie );
		m_movie = NULL;
	}
}

aui_Movie *aui_MovieButton::SetMovie( const MBCHAR *movie )
{
	aui_Movie *prevMovie = m_movie;

	if ( movie )
	{
		m_movie = g_ui->LoadMovie(movie);
		Assert( m_movie != NULL );
		if ( !m_movie )
		{
			m_movie = prevMovie;
			return NULL;
		}

		m_movie->SetDestSurface( m_window ? m_window->TheSurface() : NULL );
		m_movie->SetDestRect( m_x, m_y, m_x + m_width, m_y + m_height );

		if (m_window) {
			m_window->SetDynamic(FALSE);
			m_movie->SetWindowSurface(m_window->TheSurface());

			RECT windowRect = {m_x, m_y, m_x + m_width, m_y + m_height};
			m_movie->SetWindowRect(&windowRect);
		}
	}
	else
		m_movie = NULL;

	if ( prevMovie ) g_ui->UnloadMovie( prevMovie );

	return prevMovie;
}

AUI_ERRCODE aui_MovieButton::Idle( void )
{
	if ( m_movie )
	{
		if ( !m_movie->GetDestSurface() )
		{
			m_movie->SetDestSurface( m_window->TheSurface() );
		}

		if ( !m_movie->IsOpen() )
		{
			uint32 flags = m_flags;

			if (m_fullScreen)
			{
				flags |= k_AUI_MOVIE_PLAYFLAG_ONSCREEN;
			}

			RECT adjustedRect = {m_x, m_y, m_x+m_width, m_y+m_height};

			ToScreen(&adjustedRect);

			if (m_movie->Open(flags, g_c3ui->Secondary(), &adjustedRect) != AUI_ERRCODE_OK) {

				SetMovie(NULL);
				if (GetActionFunc())
					GetActionFunc()(this, AUI_BUTTON_ACTION_EXECUTE, 0, 0);
			}
		}

		if (m_movie)
		{
			if ( !m_movie->IsPlaying() && !m_movie->IsFinished())
			{
				m_movie->Play();
			}

			(void) m_movie->Process();

			if (m_movie->IsFinished() && !(m_flags & k_AUI_MOVIE_PLAYFLAG_PLAYANDHOLD))
			{
				if (m_ActionFunc)
					m_ActionFunc((aui_Control *)this, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
			}
#if defined(USE_SDL)
			else {
				m_window->AddDirtyRect( m_x, m_y, m_x + m_width, m_y + m_height );
			}
#endif // USE_SDL
		}
	}

	return AUI_ERRCODE_OK;
}
