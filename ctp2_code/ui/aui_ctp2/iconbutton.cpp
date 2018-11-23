#include "c3.h"

#include "aui.h"
#include "pattern.h"
#include "icon.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_ldl.h"

#include "pixelutils.h"
#include "colorset.h"

#include "CivPaths.h"

#include "primitives.h"

#include "iconbutton.h"

#include "c3ui.h"

extern C3UI			*g_c3ui;
extern CivPaths		*g_civPaths;
extern ColorSet		*g_colorSet;

IconButton::IconButton(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *pattern,
	const MBCHAR *icon,
	uint16 color,
	ControlActionCallback *ActionFunc,
	void *cookie )
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase(NULL),
	aui_Button( retval, id, x, y, width, height, ActionFunc, cookie ),
	PatternBase( pattern ),
	m_color(color )
{
	InitCommon(icon);

}

IconButton::IconButton(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Button( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase( ldlBlock, (MBCHAR *)NULL )
{
	InitCommon(ldlBlock, TRUE);

}

AUI_ERRCODE IconButton::InitCommon(const MBCHAR *ldlBlock,BOOL isLDL)
{
	const MBCHAR		*name;

	if (isLDL) {
		aui_Ldl *theLdl = g_c3ui->GetLdl();

		BOOL valid = theLdl->IsValid( ldlBlock );
		Assert( valid );
		if ( !valid ) return AUI_ERRCODE_HACK;

		ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
		Assert( block != NULL );
		if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;


		name = block->GetString( "icon" );
		Assert( name != NULL );
	} else {
		name = ldlBlock;
	}

	m_filename = new MBCHAR[_MAX_PATH];

	if (g_civPaths->FindFile(C3DIR_ICONS, name, m_filename)) {
		m_icon = g_c3ui->LoadIcon(m_filename);
	} else {
		m_icon = NULL;
	}

	return AUI_ERRCODE_OK;
}

IconButton::~IconButton()
{
	if (m_filename) {
		delete[] m_filename;
	}
}

AUI_ERRCODE IconButton::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	RECT offsetRect = rect;

	offsetRect.left += 1;
	offsetRect.top += 1;

	m_pattern->Draw( surface, &rect );

	if ( IsDown() )
	{
		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	}
	else
	{
		primitives_BevelRect16( surface, &rect, 1, 0, 16, 16 );
	}

	if ( IsActive() )
	{
		m_icon->Draw( surface, &offsetRect, g_colorSet->GetColor(COLOR_BUTTON_TEXT_DROP));
		m_icon->Draw( surface, &rect, g_colorSet->GetColor(COLOR_BUTTON_TEXT_HILITE));
	}
	else
	{
		m_icon->Draw( surface, &offsetRect, g_colorSet->GetColor(COLOR_BUTTON_TEXT_DROP));
		m_icon->Draw( surface, &rect, g_colorSet->GetColor(COLOR_BUTTON_TEXT_PLAIN));
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
