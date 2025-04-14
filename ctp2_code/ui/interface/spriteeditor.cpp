//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite editor
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
#include "spriteeditor.h"

#include "aui.h"
#include "aui_blitter.h"
#include "aui_dimension.h"
#include "aui_ldl.h"
#include "aui_stringtable.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "director.h"           // g_director
#include "maputils.h"
#include "pixelutils.h"
#include "primitives.h"
#include "ctp2_button.h"
#include "c3textfield.h"
#include "c3_static.h"
#include "c3_coloredstatic.h"
#include "c3_switch.h"
#include "c3_utilitydialogbox.h"
#include "c3window.h"
#include "c3windows.h"
#include "tiledmap.h"
#include "colorset.h"
#include "SelItem.h"
#include "player.h"
#include "UnitDynArr.h"
#include "screenutils.h"
#include "UIUtils.h"
#include "bevellesswindow.h"
#include "network.h"
#include "victorywin.h"
#include "spnewgamewindow.h"
#include "GoodSpriteGroup.h"
#include "FacedSprite.h"
#include "Sprite.h"
#include "screenmanager.h"
#include "CivPaths.h"
#include "c3files.h"
#include "SpriteFile.h"
#include "Anim.h"
#include "colorset.h"
#include "Token.h"
#include "profileDB.h"
#include "Actor.h"

extern sint32            g_ScreenWidth;
extern sint32            g_ScreenHeight;
extern C3UI             *g_c3ui;
extern TiledMap         *g_tiledMap;
extern C3Window         *g_toolbar;
extern SelectedItem     *g_selected_item;
extern Player          **g_player;
extern ProfileDB        *g_theProfileDB;
extern uint8            *g_compression_buff;

SpriteEditWindow *g_spriteEditWindow = NULL;

#if 0
static ctp2_Button **s_statsButton = NULL;
static c3_Switch *s_screensSwitch;

static MBCHAR	s_buttonName[k_STATS_NUM_BUTTONS][256] ={
	"CivButton",
	"TradeButton",
	"SciButton",
	"UnitButton",
	"LibButton",
	"DipButton",
	"OptionsButton",
	"CityButton",

};

static aui_StringTable *s_statsString;
#endif

void SpriteEditSwitchActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
}

void FileButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_spriteEditWindow->m_loopInProgress = false;

	if ((int)cookie)
		g_spriteEditWindow->LoadSprite();
	else
		g_spriteEditWindow->SaveSprite();
}

void AnimCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_spriteEditWindow->SetAnimation((sint32)cookie);
	g_spriteEditWindow->m_frame             = 0;
	g_spriteEditWindow->m_facing            = k_DEFAULTSPRITEFACING;
	g_spriteEditWindow->m_loopInProgress    = false;;
	g_spriteEditWindow->m_stopAfterLoop     = true;
	g_spriteEditWindow->BeginAnimation();
}

void StepCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_spriteEditWindow->AddFrame((sint32)cookie);
	g_spriteEditWindow->m_loopInProgress = false;

}

void PlayCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_spriteEditWindow->m_loopInProgress = !g_spriteEditWindow->m_loopInProgress;
	g_spriteEditWindow->m_stopAfterLoop  = false;

	if ((int)cookie)
		g_spriteEditWindow->m_stopAfterLoop = true;

	if (g_spriteEditWindow->m_loopInProgress)
		g_spriteEditWindow->BeginAnimation();
}

void FacingCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_spriteEditWindow->AddFacing((sint32)cookie);
}

int SpriteEditWindow_Initialize( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_spriteEditWindow ) return 0;

	strcpy(windowBlock, "SpriteEditor");

	g_spriteEditWindow = new SpriteEditWindow(&errcode, aui_UniqueId(), windowBlock, 16 );

	Assert( AUI_NEWOK(g_spriteEditWindow, errcode));
	if ( !AUI_NEWOK(g_spriteEditWindow, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	g_spriteEditWindow->Show();

	if (g_c3ui)
	{
		g_c3ui->RegisterCleanup(&SpriteEditWindow_Cleanup);
	}

	return 0;
}

void SpriteEditWindow_Cleanup(void)
{
	if (g_c3ui && g_spriteEditWindow)
	{
		g_c3ui->RemoveWindow(g_spriteEditWindow->Id());
	}

	delete g_compression_buff;
	g_compression_buff = NULL;

	delete g_spriteEditWindow;
	g_spriteEditWindow = NULL;
}

SpriteEditWindow::SpriteEditWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	C3Window(retval, id, ldlBlock, bpp, type)
{
	m_Load=NULL;
	m_Save=NULL;
	m_drawX=400;
	m_drawY=300;

	m_largeSurface = NULL;
	m_largeImage = NULL;

	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	m_largeSurface = NULL;

	InitializeControls(retval,ldlBlock);

	m_loopInProgress = false;
	m_stopAfterLoop  = true;
	m_lastTime       = 0;

	m_facing         = k_DEFAULTSPRITEFACING;
	m_frame          = 0;
	m_animation      = GOODACTION_IDLE;
	m_currentAnim    = NULL;

	aui_Dimension	*dimension=GetDim();

	if (dimension != NULL)
	{
		sint32 height=dimension->VerticalSizeData();

		m_mouseChangeY = g_ScreenHeight-height;

		Move(0, m_mouseChangeY);

		Resize(g_ScreenWidth, height);
	}

	dimension=m_largeImage->GetDim();

	if (dimension != NULL)
	{
		sint32	height = dimension->VerticalSizeData();
		sint32  width  = dimension->HorizontalSizeData();
		sint32	posX   = dimension->HorizontalPositionData();

		m_largeImage->Move(posX,g_ScreenHeight-height);

		m_largeRectAbs.left   = posX;
		m_largeRectAbs.top    = g_ScreenHeight-height;
		m_largeRectAbs.right  = posX+width;
		m_largeRectAbs.bottom = m_largeRectAbs.top+height;

		m_largeRect.left      = 0;
		m_largeRect.top       = 0;
		m_largeRect.right     = width;
		m_largeRect.bottom    = height;
	}

	m_currentSprite=NULL;
	m_spriteSurface=NULL;

	g_compression_buff = new unsigned char[COM_BUFF_SIZE];
	LoadSprite("GG001.spr"); //alligator
}

SpriteEditWindow::~SpriteEditWindow()
{
//	m_spriteData: Not deleted (reference only)
//	delete m_largeSurface   : TODO (crashes)
	delete m_currentAnim;
	delete m_currentSprite;
	delete m_spriteSurface;
	delete m_Load;
	delete m_Save;
	delete m_fileName;
	delete m_MOVEAnim;
	delete m_ATTACKAnim;
	delete m_IDLEAnim;
	delete m_VICTORYAnim;
	delete m_WORKAnim;
	delete m_stepPlus;
	delete m_stepMinus;
	delete m_playOnce;
	delete m_playLoop;
	delete m_facingPlus;
	delete m_facingMinus;
	delete m_largeImage;
	delete m_hotCoordsCurrent;
	delete m_hotCoordsMouse;
	delete m_hotCoordsHerald;
}

void
SpriteEditWindow::AddFrame	(sint32 add)
{
	m_frame +=add;

	SetFrame(m_frame);
}

void
SpriteEditWindow::SetFrame	(sint32 frame)
{
	m_frame = frame;

	if ((m_frame < 0) ||
	    (static_cast<size_t>(m_frame) >=
	        m_currentSprite->GetNumFrames((GAME_ACTION) m_animation)
	    )
	   )
	{
		m_frame = 0;
	}
}

void
SpriteEditWindow::AddFacing	(sint32 add)
{
	m_facing += add;

	SetFacing(m_facing);
}

void
SpriteEditWindow::SetFacing	(sint32 facing)
{
	m_facing = facing;

	if (m_facing >= k_MAX_FACINGS)
		m_facing = 0;
	else
		if (m_facing < 0)
			m_facing = k_MAX_FACINGS-1;
}

void
SpriteEditWindow::SetAnimation(sint32 anim)
{
	m_animation  = anim;
	m_spriteData = m_currentSprite->GetGroupSprite((GAME_ACTION)m_animation);

	delete m_currentAnim;
	Anim * origAnim = m_currentSprite->GetAnim       ((GAME_ACTION)m_animation);
	m_currentAnim   = origAnim ? Anim::CreateSequential(*origAnim) : NULL;
}

void
SpriteEditWindow::InitializeControls(AUI_ERRCODE *errcode,MBCHAR *windowBlock)
{
	m_Load          = spNew_ctp2_Button(errcode, windowBlock, "STLoadButton", "No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_Save          = spNew_ctp2_Button(errcode, windowBlock, "STSaveButton", "No Data", NULL, "CTP2_BUTTON_TITLE_BAR");

	m_fileName      = spNewTextEntry   (errcode, windowBlock, "Name");

	m_MOVEAnim      = spNew_ctp2_Button(errcode, windowBlock, "STMOVEAnim"   ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_ATTACKAnim    = spNew_ctp2_Button(errcode, windowBlock, "STATTACKAnim" ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_IDLEAnim      = spNew_ctp2_Button(errcode, windowBlock, "STIDLEAnim"   ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_VICTORYAnim   = spNew_ctp2_Button(errcode, windowBlock, "STVICTORYAnim","No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_WORKAnim      = spNew_ctp2_Button(errcode, windowBlock, "STWORKAnim"   ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");

	m_MOVEAnim      ->SetActionFuncAndCookie(AnimCallback, (void *)GOODACTION_MAX );
	m_ATTACKAnim    ->SetActionFuncAndCookie(AnimCallback, (void *)GOODACTION_MAX );
	m_IDLEAnim      ->SetActionFuncAndCookie(AnimCallback, (void *)GOODACTION_IDLE);
	m_VICTORYAnim   ->SetActionFuncAndCookie(AnimCallback, (void *)GOODACTION_MAX );
	m_WORKAnim      ->SetActionFuncAndCookie(AnimCallback, (void *)GOODACTION_MAX );

	m_stepPlus      = spNew_ctp2_Button(errcode, windowBlock, "STPlayStepPlus"  ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_stepMinus     = spNew_ctp2_Button(errcode, windowBlock, "STPlayStepMinus" ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_playOnce      = spNew_ctp2_Button(errcode, windowBlock, "STPlayOnce"      ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_playLoop      = spNew_ctp2_Button(errcode, windowBlock, "STPlayLoop"      ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_facingPlus    = spNew_ctp2_Button(errcode, windowBlock, "STFacingPlus"    ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");
	m_facingMinus   = spNew_ctp2_Button(errcode, windowBlock, "STFacingMinus"   ,"No Data", NULL, "CTP2_BUTTON_TITLE_BAR");

	m_Load         ->SetActionFuncAndCookie(FileButtonActionCallback, (void *)true );
	m_Save         ->SetActionFuncAndCookie(FileButtonActionCallback, (void *)false);
	m_stepPlus     ->SetActionFuncAndCookie(StepCallback            , (void *) 1);
	m_stepMinus    ->SetActionFuncAndCookie(StepCallback            , (void *)-1);
	m_playOnce     ->SetActionFuncAndCookie(PlayCallback            , (void *)true);
	m_playLoop     ->SetActionFuncAndCookie(PlayCallback            , (void *)false);
	m_facingPlus   ->SetActionFuncAndCookie(FacingCallback          , (void *) 1);
	m_facingMinus  ->SetActionFuncAndCookie(FacingCallback          , (void *)-1);

	m_largeImage = new C3Window(errcode, aui_UniqueId(), "STLargeImage", 16 );

	g_c3ui->AddWindow( m_largeImage );

	m_hotCoordsCurrent = new c3_Static(errcode, aui_UniqueId(), "SpriteEditor.CoordsHotSpot");
	m_hotCoordsMouse   = new c3_Static(errcode, aui_UniqueId(), "SpriteEditor.CoordsCursor" );
	m_hotCoordsHerald  = new c3_Static(errcode, aui_UniqueId(), "SpriteEditor.CoordsHerald");

	m_Load       ->SetParent(g_spriteEditWindow);
	m_Save       ->SetParent(g_spriteEditWindow);

	m_fileName   ->SetParent(g_spriteEditWindow);

	m_MOVEAnim   ->SetParent(g_spriteEditWindow);
	m_ATTACKAnim ->SetParent(g_spriteEditWindow);
	m_IDLEAnim   ->SetParent(g_spriteEditWindow);
	m_VICTORYAnim->SetParent(g_spriteEditWindow);
	m_WORKAnim   ->SetParent(g_spriteEditWindow);

	m_stepPlus   ->SetParent(g_spriteEditWindow);
	m_stepMinus  ->SetParent(g_spriteEditWindow);
	m_playOnce   ->SetParent(g_spriteEditWindow);
	m_playLoop   ->SetParent(g_spriteEditWindow);
	m_facingPlus ->SetParent(g_spriteEditWindow);
	m_facingMinus->SetParent(g_spriteEditWindow);

	m_largeImage ->SetParent(g_spriteEditWindow);

	m_hotCoordsCurrent->SetParent(g_spriteEditWindow);
	m_hotCoordsMouse  ->SetParent(g_spriteEditWindow);
	m_hotCoordsHerald ->SetParent(g_spriteEditWindow);

	m_Load      ->Show();
	m_Save      ->Show();

	m_fileName  ->Show();

	m_MOVEAnim   ->Show();
	m_ATTACKAnim ->Show();
	m_IDLEAnim   ->Show();
	m_VICTORYAnim->Show();
	m_WORKAnim   ->Show();

	m_stepPlus   ->Show();
	m_stepMinus  ->Show();
	m_playOnce   ->Show();
	m_playLoop   ->Show();
	m_facingPlus ->Show();
	m_facingMinus->Show();

	m_largeImage ->Show();

	m_hotCoordsCurrent->Show();
	m_hotCoordsMouse  ->Show();
	m_hotCoordsHerald ->Show();

	m_largeSurface=m_largeImage->TheSurface();
}

AUI_ERRCODE SpriteEditWindow::InitCommon(void)
{
	m_scroll  = FALSE;
	m_dest    = 0;
	m_current = 0;

	return C3Window::InitCommon();
}

AUI_ERRCODE SpriteEditWindow::InitCommonLdl(MBCHAR *ldlBlock)
{
	InitCommon();
	return C3Window::InitCommon();
}

AUI_ERRCODE SpriteEditWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if ( IsHidden() )
		return AUI_ERRCODE_OK;

	return AUI_ERRCODE_OK;
}

bool SpriteEditWindow::FileExists(char *name)
{
	if (name == NULL)
		return false;

	MBCHAR spritePath[_MAX_PATH];
	MBCHAR fullPath[_MAX_PATH];
	g_civPaths->GetSpecificPath(C3DIR_SPRITES, spritePath, FALSE);
	sprintf(fullPath, "%s%s%s", spritePath, FILE_SEP, name);

	return c3files_PathIsValid(fullPath);
}

void SpriteEditWindow::LoadSprite(char *name)
{
	char tbuffer[256];

	m_loopInProgress = false;;
	m_stopAfterLoop  = true;

	if (name == NULL)
	{
		m_fileName->GetFieldText(tbuffer,128);

		name = tbuffer;
	}

	if (name == NULL)
		return;

	sprintf(tbuffer, "%s", name);

	m_frame=0;
	m_facing=k_DEFAULTSPRITEFACING;

	delete m_currentSprite;
	delete m_spriteSurface;

	m_currentSprite = new GoodSpriteGroup(GROUPTYPE_UNIT);

#if defined(LINUX)
	if(strcasestr(tbuffer, ".SPR")) // Does not exist on windows, @ToDo port it, without using something that requires us stating that we used it
#else
	if(strstr(tbuffer, ".SPR")) // Obviously, this is crap on Windows, but for now it compiles, and is not a feature of the release version.
#endif
	{
		fprintf(stderr, "%s L%d: name= %s!\n", __FILE__, __LINE__, tbuffer);
		if (!FileExists(tbuffer))
			return;

		m_currentSprite->LoadFull(tbuffer);
	}
#if defined(LINUX)
	else if(strcasestr(tbuffer, ".TXT"))
#else
	else if(strstr(tbuffer, ".TXT"))
#endif
	{
		if (!FileExists(tbuffer))
			return;

		fprintf(stderr, "%s L%d: name= %s!\n", __FILE__, __LINE__, strupr(tbuffer));
		sint16 id = -1;
		sscanf(strupr(tbuffer), "GG%hd.TXT", &id);
		fprintf(stderr, "%s L%d: id= %d!\n", __FILE__, __LINE__, id);
		m_currentSprite->Parse(id, GROUPTYPE_GROUP); // GROUPTYPE is not used
	}
	else
	{
		fprintf(stderr, "%s L%d: name= %s!\n", __FILE__, __LINE__, tbuffer);
		return;
	}

	uint32 i;
	uint16 w = 0;
	uint16 h = 0;

	m_spriteRect.left   = 0;
	m_spriteRect.right  = 100;
	m_spriteRect.top    = 0;
	m_spriteRect.bottom = 100;

	for (i = 0; i < GOODACTION_MAX; i++)
	{
		m_spriteData = m_currentSprite->GetGroupSprite((GAME_ACTION)i);

		if (m_spriteData != NULL)
		{
			w =  m_spriteData->GetWidth();
			h =  m_spriteData->GetHeight();

			if (m_spriteRect.right < w)
				m_spriteRect.right = w;

			if (m_spriteRect.bottom < h)
				m_spriteRect.bottom = h;
		}
	}

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_spriteSurface=new aui_Surface(&errcode,m_spriteRect.right,m_spriteRect.bottom,16);

	m_spriteData = m_currentSprite->GetGroupSprite((GAME_ACTION)m_animation);

	delete m_currentAnim;
	Anim * origAnim = m_currentSprite->GetAnim((GAME_ACTION)m_animation);
	m_currentAnim   = origAnim ? Anim::CreateSequential(*origAnim) : NULL;

	float largew=(float)(m_largeRect.right);
	float largeh=(float)(m_largeRect.bottom);

	if (largew > 0)
		m_widthRatio = (float)w / (float)largew;
	else
		m_widthRatio = 1.0f;

	if (largeh > 0)
		m_heightRatio = (float)h / (float)largeh;
	else
		m_heightRatio = 1.0f;

	m_oneOverWidthRatio  = 1.0f / m_widthRatio;
	m_oneOverHeightRatio = 1.0f / m_heightRatio;

	m_drawFlag      = true;
}

void SpriteEditWindow::SaveSprite(char *name)
{
	char tbuffer[256];

	if(name==NULL)
	{
		m_fileName->GetFieldText(tbuffer, 128);
		name = tbuffer;
	}

	if((m_currentSprite == NULL) || (name == NULL))
		return;

#if defined(LINUX)
	if(strcasestr(tbuffer, ".SPR"))
#else
	if(strstr(tbuffer, ".SPR"))
#endif
	{
		fprintf(stderr, "%s L%d: name= %s!\n", __FILE__, __LINE__, tbuffer); // Actually debug printing, should be go through the system
		m_currentSprite->Save(name,k_SPRITEFILE_VERSION2,SPRDATA_LZW1);
	}
#if defined(LINUX)
	else if(strcasestr(tbuffer, ".TXT"))
#else
	else if(strstr(tbuffer, ".TXT"))
#endif
	{
		fprintf(stderr, "%s L%d: name= %s!\n", __FILE__, __LINE__, tbuffer); // Actually debug printing, should be go through the system
		m_currentSprite->ExportScript(name);
	}
}

void SpriteEditWindow::BeginAnimation()
{
	if (m_currentAnim==NULL)
		return;

	m_frame = 0;

	m_currentAnim->Rewind();
}

void SpriteEditWindow::Animate()
{
	bool animation_over = false;

	if (m_currentAnim)
	{
		m_currentAnim->Process();

		m_frame = m_currentAnim->GetCurrentFrame();

		if (m_currentAnim->IsFinished())
		{
			if (m_stopAfterLoop) {
				animation_over = true;
				m_frame = 0;
			} else {
				m_currentAnim->Rewind();
			}
		}
	}
	else
	{
		AddFrame(1);
		animation_over = (m_frame == 0);
	}

	if (animation_over)
	{
		if (m_stopAfterLoop)
			m_loopInProgress = false;
	}
}

void SpriteEditWindow::DrawSprite()
{
	if ((m_currentSprite!=NULL)&&(g_tiledMap!=NULL))
	{
		RECT rect;

		rect.left   = m_drawX - m_spriteRect.right;
		rect.top    = m_drawY - m_spriteRect.bottom;
		rect.right  = m_drawX + m_spriteRect.right;
		rect.bottom = m_drawY + m_spriteRect.bottom;

		if (m_drawFlag)
			m_currentSprite->Draw((GOODACTION)m_animation, m_frame, m_drawX, m_drawY, m_facing, 1.0, 15, 0, k_DRAWFLAGS_NORMAL);

		POINT flag;
		maputils_MapXY2PixelXY(m_drawPoint.x, m_drawPoint.y, flag);

		MAPICON icon = MAPICON_HERALD;

		Pixel16 blue = pixelutils_RGB(0,0,255);

		g_tiledMap->DrawColorizedOverlayIntoMix(g_tiledMap->GetTileSet()->GetMapIconData(icon), flag.x, flag.y, blue);

		g_tiledMap->AddDirtyRectToMix(rect);
	}
}

void SpriteEditWindow::ReDrawLargeSprite()
{
	if (m_largeSurface==NULL)
		return;

	if ((m_currentSprite!=NULL)&&(g_tiledMap!=NULL))
	{
		POINT sav;
		POINT pt;
		static int cval = 0;

		g_c3ui->TheBlitter()->ColorBlt(m_spriteSurface, &m_spriteRect, RGB(64, 64, 64), k_AUI_BLITTER_FLAG_COPY);

		pt.x = 0;
		pt.y = 0;

		if (m_facing >= k_NUM_FACINGS)
			pt.x = m_spriteData->GetWidth();

		sav = m_currentSprite->GetHotPoint((GOODACTION)m_animation);
		m_currentSprite->SetHotPoint((GOODACTION)m_animation, pt); // set only for large view
		m_currentSprite->DrawDirect(m_spriteSurface, (GOODACTION)m_animation, m_frame, 0, 0, m_facing, 1.0, 15, 0, k_DRAWFLAGS_NORMAL);
		m_currentSprite->SetHotPoint((GOODACTION)m_animation, sav); // reset for drawing on map

		g_c3ui->TheBlitter()->StretchBlt(m_largeSurface, &m_largeRect, m_spriteSurface, &m_spriteRect, k_AUI_BLITTER_FLAG_COPY);

		char tbuffer[256];

		sprintf(tbuffer, "HotSpot:\t%d,%d", sav.x, sav.y);
		m_hotCoordsCurrent  ->SetText(tbuffer);

		RECT HotRect; // rect to indicate hot spot position in large view

		HotRect.left    = (int)(m_oneOverWidthRatio  * (float)sav.x) - 3;
		HotRect.top     = (int)(m_oneOverHeightRatio * (float)sav.y) - 3;
		HotRect.right   = HotRect.left + 6;
		HotRect.bottom  = HotRect.top  + 6;

		COLORREF color  = RGB(cval,cval,cval);

		g_c3ui->TheBlitter()->ColorBlt(m_largeSurface, &HotRect, color, k_AUI_BLITTER_FLAG_COPY);

		m_largeImage->ShouldDraw(TRUE);
		m_largeImage->Invalidate(&m_largeRect);

		cval += 16;

		if (cval > 255)
			cval=0;
	}
}

AUI_ERRCODE SpriteEditWindow::Idle( void )
{
	if ((g_c3ui->TheMouse() == NULL) || (g_tiledMap == NULL))
		return AUI_ERRCODE_OK;

	sint32 curTime = g_director->GetMasterCurTime();

	if ((curTime-m_lastTime) > 100)
	{
		m_lastTime = curTime;

		if (m_loopInProgress)
			Animate();
	}

	ReDrawLargeSprite();

	aui_MouseEvent *me=g_c3ui->TheMouse()->GetLatestMouseEvent();

	char tbuffer[256];

	if (PtInRect(&m_largeRectAbs,me->position))
	{
		sint32 mx = (int)((float)(me->position.x-m_largeRectAbs.left) * m_widthRatio);
		sint32 my = (int)((float)(me->position.y-m_largeRectAbs.top ) * m_heightRatio);

		sprintf(tbuffer, "Cursor:\t%d,%d", mx, my);
		m_hotCoordsMouse  ->SetText(tbuffer);
	}

	if (!me->lbutton)
		return AUI_ERRCODE_OK;

	if (g_c3ui->TheMouse()->Y() < m_mouseChangeY)
	{
		MapPoint point;
		g_tiledMap->GetMouseTilePos(point);
		maputils_MapXY2PixelXY(point.x, point.y, &m_drawX, &m_drawY);
		m_drawX += k_ACTOR_CENTER_OFFSET_X;
		m_drawY += k_ACTOR_CENTER_OFFSET_Y;
		m_drawPoint = point;
	}
	else
	{
		if (PtInRect(&m_largeRectAbs, me->position))
		{
			POINT new_hot=me->position;

			new_hot.x -= m_largeRectAbs.left;
			new_hot.y -= m_largeRectAbs.top;

			new_hot.x = (int)((float)new_hot.x * m_widthRatio);
			new_hot.y = (int)((float)new_hot.y * m_heightRatio);

			m_currentSprite->SetHotPoint((GOODACTION)m_animation, new_hot);
		}
	}

	return AUI_ERRCODE_OK;
}
