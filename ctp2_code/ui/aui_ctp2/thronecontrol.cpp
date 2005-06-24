



#include "c3.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_surface.h"

#include "aui_Factory.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_action.h"

#include "c3ui.h"
#include "CivPaths.h"
#include "c3_static.h"
#include "c3_button.h"
#include "c3_image.h"
#include "c3windows.h"

#include "primitives.h"

#include "thronecontrol.h"

extern C3UI		*g_c3ui;
extern CivPaths	*g_civPaths;

static MBCHAR *s_throneImage[ k_THRONE_IMAGES + 1 ];

static POINT	s_upgradeLoc[k_THRONE_IMAGES] = { 0, 230, 204, 204, 210, 300, 284, 380, 410, 250 };

static	Pixel16 s_highlightColor[k_THRONE_IMAGES] = { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff };

static MBCHAR s_currentName[k_THRONE_IMAGES][255] = {
	"castlecurrent",
	"entertainmentcurrent",
	"fountaincurrent",
	"statuecurrent",
	"religiouscurrent"
};

static MBCHAR s_upgradeName[k_THRONE_IMAGES][255] = {
	"castleupgrade",
	"entertainmentupgrade",
	"fountainupgrade",
	"statueupgrade",
	"religiousupgrade"
};

void ZoomedImageActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	ThroneControl *tc = (ThroneControl *)cookie;
	tc->SetZoomedMode( FALSE );
}


ThroneControl::ThroneControl(AUI_ERRCODE *retval,
							sint32 id,
							MBCHAR *ldlBlock,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:	aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
		aui_ImageBase(ldlBlock),
		aui_TextBase(ldlBlock),
		PatternBase(ldlBlock, NULL)
{
	InitCommonLdl(ldlBlock);
}


ThroneControl::ThroneControl(AUI_ERRCODE *retval,
							uint32 id,
							sint32 x,
							sint32 y,
							sint32 width,
							sint32 height,
							MBCHAR *pattern,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:	aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
		aui_ImageBase((sint32)0),
		aui_TextBase((MBCHAR *)NULL),
		PatternBase(pattern)
{
	InitCommon();	
}

ThroneControl::~ThroneControl()
{
#define cleanup(x)	 if (x) { delete x; x = NULL; }
	cleanup( m_throneSurface );
	cleanup( m_background );

	for ( sint32 i = 0;i < k_THRONE_IMAGES;i++ ) {
		cleanup( m_upgradeImage[i] );
	}

	cleanup( m_oldCutout );
	cleanup( m_newCutout );
#undef cleanup
}


void ThroneControl::InitCommonLdl(MBCHAR *ldlBlock)
{
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return;

	
	ldl_datablock *datablock = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( datablock != NULL );
	if ( !datablock ) return;

	sint32 speed = datablock->GetInt( k_THRONE_LDL_BLENDSPEED );
	SetBlendSpeed( speed );

	s_throneImage[0] = datablock->GetString( k_THRONE_LDL_BACKGROUND );
	s_throneImage[1] = datablock->GetString( k_THRONE_LDL_CASTLE );
	s_throneImage[2] = datablock->GetString( k_THRONE_LDL_ENTERTAINMENT );
	s_throneImage[3] = datablock->GetString( k_THRONE_LDL_FOUNTAIN );
	s_throneImage[4] = datablock->GetString( k_THRONE_LDL_STATUE );
	s_throneImage[5] = datablock->GetString( k_THRONE_LDL_RELIGIOUS );

	
	AUI_ERRCODE errcode;
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_THRONE_LDL_MESSAGE );

		if ( theLdl->GetLdl()->FindDataBlock( block ) ) {
			m_messageText = new c3_Static( &errcode, aui_UniqueId(), block );
		}

		for ( sint32 i = 0;i < k_THRONE_IMAGES;i++ ) {
			sprintf( block, "%s.%s", ldlBlock, s_currentName[i] );

			if ( theLdl->GetLdl()->FindDataBlock( block ) ) {
				m_currentText[i] = new c3_Static( &errcode, aui_UniqueId(), block );
			}

			sprintf( block, "%s.%s", ldlBlock, s_upgradeName[i] );

			if ( theLdl->GetLdl()->FindDataBlock( block ) ) {
				m_upgradeText[i] = new c3_Static( &errcode, aui_UniqueId(), block );
				m_upgradeText[i]->Hide();
			}
		}
	}

	InitCommon();
}


void ThroneControl::InitCommon(void)
{
	AUI_ERRCODE errcode;
	MBCHAR s[_MAX_PATH];

	if (!g_civPaths->GetSpecificPath(C3DIR_PICTURES, s, FALSE)) return;
	strcat( s, "\\" );
	strcat( s, s_throneImage[0] );

	
	m_background = new c3_Image( &errcode, s );
	Assert( AUI_NEWOK(m_background, errcode) );
	if ( !AUI_NEWOK(m_background, errcode) ) return;

	m_background->Load();

	
	for ( sint32 i = 0;i < k_THRONE_IMAGES;i++ ) {
		if (!g_civPaths->GetSpecificPath(C3DIR_PICTURES, s, FALSE)) return;
		strcat( s, "\\" );
		strcat( s, s_throneImage[i+1] );

		m_upgradeImage[i] = new c3_Image( &errcode, s );
		Assert( AUI_NEWOK(m_upgradeImage[i], errcode) );
		if ( !AUI_NEWOK(m_upgradeImage[i], errcode) ) return;

		m_upgradeImage[i]->Load();

		
		m_upgradeRect[i].left = s_upgradeLoc[i].x;
		m_upgradeRect[i].top = s_upgradeLoc[i].y;
		m_upgradeRect[i].right = m_upgradeRect[i].left + m_upgradeImage[i]->TheSurface()->Width();
		m_upgradeRect[i].bottom = m_upgradeRect[i].top + m_upgradeImage[i]->TheSurface()->Height();

		
		m_drawOrder[i] = (sint8)i;
	}

	
	m_throneSurface = aui_Factory::new_Surface( errcode, m_width, m_height, 16);
	Assert( AUI_NEWOK(m_throneSurface, errcode) );

	m_zoomedImage = NULL;

	RenderThrone( m_throneSurface );

	m_oldCutout = NULL;
	m_newCutout = NULL;
	m_isCrossFade = FALSE;
	m_blendVal = k_THRONE_BLENDVAL_MAX;
	m_hilightedImage = -1;
	m_wasHilighted = -1;
	m_isZoomed = FALSE;
}


void ThroneControl::UpdateThrone(aui_Surface *surf, sint32 x, sint32 y)
{
	RECT		rect = {0, 0, m_throneSurface->Width(), m_throneSurface->Height()};

	
	g_c3ui->TheBlitter()->Blt(surf, x, y, m_throneSurface, &rect, k_AUI_BLITTER_FLAG_COPY);
}


sint32 ThroneControl::RenderThrone( aui_Surface *surf )
{
	RECT	rect = { 0, 0, m_background->TheSurface()->Width(), m_background->TheSurface()->Height() };

	

	g_c3ui->TheBlitter()->Blt( surf, 0, 0, m_background->TheSurface(), &rect, k_AUI_BLITTER_FLAG_CHROMAKEY );

	
	for ( sint32 i = 0;i < k_THRONE_IMAGES;i++ ) {

		
		sint8 j = m_drawOrder[i];

		
		rect.right = m_upgradeRect[j].right - m_upgradeRect[j].left;
		rect.bottom = m_upgradeRect[j].bottom - m_upgradeRect[j].top;
		
		
		g_c3ui->TheBlitter()->Blt( surf, m_upgradeRect[j].left, m_upgradeRect[j].top, m_upgradeImage[j]->TheSurface(),

			&rect, k_AUI_BLITTER_FLAG_CHROMAKEY );

	}


#ifdef _SHOW_UPGRADE_RECTS
		RepaintFrames( surf );
#endif

	return 0;
}


sint32 ThroneControl::RepaintFrames( aui_Surface *surf )
{
	
	for ( sint32 i = 0;i < k_THRONE_IMAGES;i++ ) {

		
		sint8 j = m_drawOrder[i];

		primitives_FrameRect16( surf, &m_upgradeRect[j], s_highlightColor[i] );
	}

	return 0;
}




void ThroneControl::SetDrawOrder( sint8 a, sint8 b, sint8 c, sint8 d, sint8 e )
{
	m_drawOrder[0] = a;
	m_drawOrder[1] = b;
	m_drawOrder[2] = c;
	m_drawOrder[3] = d;
	m_drawOrder[4] = e;

	RenderThrone( m_throneSurface );
}


aui_Surface *ThroneControl::InitializeNewBG( MBCHAR *filename )
{
	AUI_ERRCODE errcode;
	aui_Surface *tempBG = NULL;

	
	if ( !tempBG ) {
		tempBG = aui_Factory::new_Surface( errcode, m_width, m_height, 16);
		Assert( AUI_NEWOK(tempBG, errcode) );
		if ( !AUI_NEWOK(tempBG, errcode) ) return NULL;
	}

	
	MBCHAR s[_MAX_PATH];
	if (!g_civPaths->GetSpecificPath(C3DIR_PICTURES, s, FALSE)) {
		delete tempBG;
		return NULL;
	}
	strcat( s, "\\" );
	strcat( s, filename );

	
	m_upgradeImage[ m_selectedImage ]->Unload();
	
	
	m_upgradeImage[ m_selectedImage ]->SetFilename( s );
	m_upgradeImage[ m_selectedImage ]->Load();

	
	RenderThrone( tempBG );

	return tempBG;
}


void ThroneControl::CrossFadeImage( MBCHAR *filename )
{
	AUI_ERRCODE errcode;
	aui_Surface *tempBG = NULL;

	sint32 width = m_upgradeImage[ m_selectedImage ]->TheSurface()->Width();
	sint32 height = m_upgradeImage[ m_selectedImage ]->TheSurface()->Height();
	
	
	m_isCrossFade = TRUE;

	this->Enable( FALSE );

	
	if ( !m_oldCutout ) {
		m_oldCutout = aui_Factory::new_Surface( errcode, width, height, 16);
		Assert( AUI_NEWOK(m_oldCutout, errcode) );

		
		g_c3ui->TheBlitter()->Blt( m_oldCutout, 0, 0, m_throneSurface, &m_upgradeRect[m_selectedImage], 
			k_AUI_BLITTER_FLAG_COPY );
	}

	
	if ( !tempBG ) {
		tempBG = InitializeNewBG( filename );
	}
	
	
	if ( !m_newCutout ) {
		m_newCutout = aui_Factory::new_Surface( errcode, width, height, 16);
		Assert( AUI_NEWOK(m_newCutout, errcode) );

		
		g_c3ui->TheBlitter()->Blt( m_newCutout, 0, 0, tempBG, &m_upgradeRect[m_selectedImage],
			k_AUI_BLITTER_FLAG_COPY );
	}

	
	if ( tempBG ) {
		delete tempBG;
	}
}


void ThroneControl::DisplayZoomedImage( MBCHAR *filename )
{
	AUI_ERRCODE errcode;

	if ( m_zoomedImage ) {
		RemoveSubControl( m_zoomedImage->Id() );

		delete m_zoomedImage;
		m_zoomedImage = NULL;
	}

	m_zoomedImage = new c3_Button( &errcode, aui_UniqueId(), 0, 0, 0, 0, 
		k_PatternName, ZoomedImageActionCallback, this );
	Assert( AUI_NEWOK(m_zoomedImage, errcode) );
	if ( !AUI_NEWOK(m_zoomedImage, errcode) ) return;

	m_zoomedImage->SetImage( filename );


	sint32 width = m_zoomedImage->GetImage()->TheSurface()->Width();
	sint32 height = m_zoomedImage->GetImage()->TheSurface()->Height();
	sint32 x = ( m_width - width ) / 2;
	sint32 y = ( m_height - height ) / 2;

	m_zoomedImage->Move( x, y );
	m_zoomedImage->Resize( width, height );

	InsertSubControl( m_zoomedImage, 0 );

	m_isZoomed = TRUE;
}


void ThroneControl::HilightImage( sint32 index )
{
	sint32 width = m_upgradeRect[index].right - m_upgradeRect[index].left;
	sint32 height = m_upgradeRect[index].bottom - m_upgradeRect[index].top;

	RECT rect = { 0, 0, width, height };
	AUI_ERRCODE errcode;	

	
	if ( !m_oldCutout ) {
		m_oldCutout = aui_Factory::new_Surface( errcode, width, height, 16 );
		Assert( AUI_NEWOK(m_oldCutout, errcode) );

		
		g_c3ui->TheBlitter()->Blt( m_oldCutout, 0, 0, m_upgradeImage[index]->TheSurface(),
		&rect, k_AUI_BLITTER_FLAG_CHROMAKEY );
	}

	
	primitives_LightenSurface( m_oldCutout, k_THRONE_HIGHLIGHT_IMAGE );

	
	g_c3ui->TheBlitter()->Blt( m_throneSurface, m_upgradeRect[index].left, m_upgradeRect[index].top, m_oldCutout,
		&rect, k_AUI_BLITTER_FLAG_CHROMAKEY );

	delete m_oldCutout;
	m_oldCutout = NULL;
}


void ThroneControl::UnhilightImage( sint32 index )
{
	RECT rect = { 0, 0, m_upgradeRect[index].right - m_upgradeRect[index].left, m_upgradeRect[index].bottom - m_upgradeRect[index].top };

	
	g_c3ui->TheBlitter()->Blt( m_throneSurface, m_upgradeRect[index].left, m_upgradeRect[index].top, m_upgradeImage[index]->TheSurface(),
		&rect, k_AUI_BLITTER_FLAG_CHROMAKEY );
}


AUI_ERRCODE ThroneControl::DrawThis(aui_Surface *surface, sint32 x,	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) 
		surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	UpdateThrone(surface, rect.left, rect.top);


	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}


void ThroneControl::MouseLDropInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	
	data->position.x -= X();
	data->position.y -= Y();

	
	for ( sint32 i = 0;i < k_THRONE_IMAGES;i++ ) {
		if ( PtInRect(&m_upgradeRect[i], data->position) ) {
			
			
			POINT p = { data->position.x - m_upgradeRect[i].left, data->position.y - m_upgradeRect[i].top };

			if ( m_upgradeImage[i]->PtOnImage( &p ) ) {
				s_highlightColor[i] = 0x07e0;

				m_selectedImage = i;

				
				if ( m_ActionFunc )
					m_ActionFunc( this, C3_THRONE_ACTION_SELECTEDIMAGE, 0, m_cookie );
				else if ( m_action )
					m_action->Execute( this, C3_THRONE_ACTION_SELECTEDIMAGE, 0 );

			}
		}
		else {
			s_highlightColor[i] = 0xffff;
		}
	}


}


void ThroneControl::MouseMoveInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	
	data->position.x -= X();
	data->position.y -= Y();

	m_wasHilighted = m_hilightedImage;
	m_hilightedImage = -1;

	
	for ( sint32 i = 0;i < k_THRONE_IMAGES;i++ ) {
		if ( PtInRect(&m_upgradeRect[i], data->position) && !m_isZoomed ) {
			
			
			POINT p = { data->position.x - m_upgradeRect[i].left, data->position.y - m_upgradeRect[i].top };

			Assert(m_upgradeImage[i]);
			if (m_upgradeImage[i] && m_upgradeImage[i]->PtOnImage( &p ) ) {
				m_hilightedImage = i;

				
				if ( m_ActionFunc )
					m_ActionFunc( this, C3_THRONE_ACTION_HIGHLIGHTIMAGE, 0, m_cookie );
				else if ( m_action )
					m_action->Execute( this, C3_THRONE_ACTION_HIGHLIGHTIMAGE, 0 );

			}
			else {
				m_upgradeText[i]->Hide();
			}
		}
		else {
			m_upgradeText[i]->Hide();
		}
	}

	if ( m_wasHilighted != -1 ) {
		if ( m_wasHilighted != m_hilightedImage ) {
			UnhilightImage( m_wasHilighted );
		}
	}
}


void ThroneControl::DisplayHilightedText( void )
{
	m_upgradeText[ m_hilightedImage ]->Show(); 
}


void ThroneControl::SetZoomedMode( BOOL on )
{
	m_isZoomed = on;

	if ( !on ) {
		if ( m_zoomedImage ) {
			RemoveSubControl( m_zoomedImage->Id() );

			delete m_zoomedImage;
			m_zoomedImage = NULL;
		}
	}
}


AUI_ERRCODE ThroneControl::Idle( void )
{
	static uint32 lastDraw = 0;
	if (GetTickCount() - lastDraw > 100) lastDraw = GetTickCount();
	else return AUI_ERRCODE_OK;

	if ( m_isCrossFade ) {
		primitives_BlendSurfaces( m_oldCutout, m_newCutout, m_throneSurface, 
			&m_upgradeRect[ m_selectedImage ], m_blendVal );

		m_blendVal -= m_blendSpeed;

		
		if ( m_blendVal < 0 ) {
			if ( m_oldCutout ) {
				delete m_oldCutout;
				m_oldCutout = NULL;
			}

			if ( m_newCutout ) {
				delete m_newCutout;
				m_newCutout = NULL;
			}

			m_isCrossFade = FALSE;
			m_blendVal = k_THRONE_BLENDVAL_MAX;

			this->Enable( TRUE );
		}

		ShouldDraw( TRUE );
	}

	



	return AUI_ERRCODE_OK;
}



