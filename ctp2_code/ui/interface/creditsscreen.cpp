//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The credit screen
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
// - Corrected initialisations that were causing ambiguity. 
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "creditsscreen.h"

#include <algorithm>	// std::fill

#include "aui_action.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_directsurface.h"
#include "aui_blitter.h"
#include "aui_stringtable.h"
#include "aui_static.h"
#include "aui_button.h"
#include "c3ui.h"
#include "c3window.h"
#include "ctp2_button.h"
#include "UIUtils.h"
#include "primitives.h"
#include "soundmanager.h"

#include "aui_bitmapfont.h"
#include "MessageBoxDialog.h"
#include "colorset.h"


#include "StrDB.h"
#include "aui_Factory.h"


#define k_C3_ANIMATION_MODIFIER				20
#define k_C3_ANIMATION_SPEED				"animationSpeed"
#define k_C3_ANIMATION_FRAMES				"FrameTable"
#define k_C3_ANIMATION_BITS_PER_PIXEL		16
#define k_C3_ANIMATION_MAXBLEND				32
#define k_C3_ANIMATION_BLEND_STEP			1
#define k_C3_ANIMATION_BLEND_SPEED			"blendSpeed"


const sint32 k_CREDITS_BITS_PER_PIXEL =			16;
#define k_LDL_CREDITS_WINDOW				"CreditsScreen"
#define k_LDL_CREDITS_EXIT_BUTTON			"ExitButton"
#define k_LDL_CREDITS_PAUSE_BUTTON			"PauseButton"
#define k_LDL_CREDITS_SECRET_BUTTON			"SecretButton"


#define k_LDL_CREDITS_BACKGROUND			"BackgroundImage"
#define k_LDL_CREDITS_BACKANIM_COUNT		"backgroundAnims"
#define k_LDL_CREDITS_BACKANIM_BASE			"BackgroundAnim"
#define k_LDL_CREDITS_TRIGGERANIM_COUNT		"triggeredAnims"
#define k_LDL_CREDITS_TRIGGERANIM_BASE		"TriggeredAnim"
#define k_LDL_CREDITS_CREDITANIM			"CreditAnim"
#define k_LDL_CREDITS_BORDER				"BorderImage"
#define k_LDL_CREDITS_SECRET_IMAGE			"SecretImage"

#define k_CREDITS_FILENAME					"credits.txt"






const size_t k_CreditsLineLen = 80;
const size_t kCreditsTextNumFonts = 6;



extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;
extern C3UI*		g_c3ui;
extern StringDB*	g_theStringDB;
extern SoundManager	*g_soundManager;


CreditsWindow*			g_creditsWindow = NULL;		


C3Window *GetInitialPlayScreen();


class RemoveCreditsAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};


void RemoveCreditsAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	creditsscreen_Cleanup();
}


void creditsscreen_ExitButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	
	if(action == (uint32)AUI_BUTTON_ACTION_EXECUTE) {
		
		AUI_ERRCODE auiErr = g_c3ui->RemoveWindow(g_creditsWindow->Id());

		
		Assert(auiErr == AUI_ERRCODE_OK);
		if(auiErr != AUI_ERRCODE_OK) return;

		
		RemoveCreditsAction	*actionObj = new RemoveCreditsAction;
		g_c3ui->AddAction(actionObj);
	}
}


void creditsscreen_PauseButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	
	if(action == (uint32)AUI_BUTTON_ACTION_EXECUTE) {
		g_creditsWindow->ToggleAnimation();
	}
}


void creditsscreen_SecretButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	
	if(action == (uint32)AUI_BUTTON_ACTION_EXECUTE) {
		g_creditsWindow->ShowSecretImage();
	}
}


sint32 creditsscreen_Initialize()
{
	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	if(g_creditsWindow) return(0);

	
	g_creditsWindow = new CreditsWindow(&errcode, aui_UniqueId(), k_LDL_CREDITS_WINDOW, 
		k_CREDITS_BITS_PER_PIXEL, AUI_WINDOW_TYPE_FLOATING);
	TestControl(g_creditsWindow);

	
	Assert(AUI_SUCCESS(errcode));
	if(!AUI_SUCCESS(errcode)) return(-1);

	
	
	C3Window *initPlayWindow = GetInitialPlayScreen();
	
	
	g_creditsWindow->Move((g_ScreenWidth - g_creditsWindow->Width()) / 2,
		(g_ScreenHeight - g_creditsWindow->Height()) / 2);
	
	Assert(AUI_SUCCESS(errcode));
	if(!AUI_SUCCESS(errcode)) return(-1);


	
	return(0);
}


sint32 creditsscreen_Cleanup()
{
	
	if(!g_creditsWindow) return(0);

	
	g_c3ui->RemoveWindow(g_creditsWindow->Id());

	
	delete g_creditsWindow;
	g_creditsWindow = NULL;

	
	return(0);
}




class c3_SimpleAnimation : public aui_Static {
public:

	c3_SimpleAnimation(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock) 
	:	aui_Static(retval, id, ldlBlock),
		m_frames(NULL),
		m_currentFrame(0),
		m_animationSpeed(100),
		lastIdle(GetTickCount())
	{ 	
		InitCommonLdl(ldlBlock);
	};
	
	c3_SimpleAnimation(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height,
		const MBCHAR *text = NULL, uint32 maxLength = 0 ) 
	:	aui_Static(retval, id, x, y, width, height, text, maxLength),
		m_frames(NULL),
		m_currentFrame(0),
		m_animationSpeed(100),
		lastIdle(GetTickCount())
	{ 
	};

	virtual ~c3_SimpleAnimation() 
	{ 
		delete m_frames; 
	};
	

	
	
	virtual AUI_ERRCODE Idle();

protected:

	c3_SimpleAnimation()
	:	aui_Static(),
		m_frames(NULL)
	{
	};

	
	void InitCommonLdl(MBCHAR *ldlBlock);
	
	
	void UpdateAnimation(sint32 deltaTime);

private:

	
	aui_StringTable *m_frames;

	
	sint32 m_currentFrame;

	
	sint32 m_animationSpeed;

	
	uint32 lastIdle;
};



AUI_ERRCODE c3_SimpleAnimation::Idle()
{
	
	int index = 0;

	
	sint32 deltaTime = GetTickCount() - lastIdle;

	
	if(deltaTime < m_animationSpeed) return AUI_ERRCODE_OK;

	
	
	sint32 animationTime = 0;
	do {
		
		animationTime++;

		
		deltaTime -= m_animationSpeed;
	} while((deltaTime - m_animationSpeed) > 0);

	
	lastIdle = GetTickCount() + (m_animationSpeed - deltaTime);

	
	UpdateAnimation(animationTime);

	
	return AUI_ERRCODE_OK;
}


void c3_SimpleAnimation::InitCommonLdl(MBCHAR *ldlBlock)
{
	
	AUI_ERRCODE errcode;						
	MBCHAR ldlString[k_AUI_LDL_MAXBLOCK + 1];	

	
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert(valid);
	if(!valid) return;

	
	ldl_datablock *datablock = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert(datablock != NULL);
	if(!datablock) return;

	
	m_animationSpeed			= datablock->GetInt(k_C3_ANIMATION_SPEED);

	
	sprintf(ldlString, "%s.%s", ldlBlock, k_C3_ANIMATION_FRAMES);		
	m_frames = new aui_StringTable(&errcode, ldlString);				
	Assert(m_frames);													

	
	m_currentFrame = 0;
	SetImage(m_frames->GetString(m_currentFrame));
}


void c3_SimpleAnimation::UpdateAnimation(sint32 deltaTime)
{
	
	if(!m_frames) return;

	
	m_currentFrame += deltaTime;

	
	while(m_currentFrame >= m_frames->GetNumStrings())
		m_currentFrame -= m_frames->GetNumStrings();

	
	SetImage(m_frames->GetString(m_currentFrame));

	
	GetParentWindow()->ShouldDraw();
}





class c3_TriggeredAnimation : public aui_Static {
public:

	
	c3_TriggeredAnimation(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock) 
	:	aui_Static(retval, id, ldlBlock),
		m_frames(NULL),
		m_currentFrame(0),
		m_blendSpeed(100),
		m_blendVal(k_C3_ANIMATION_MAXBLEND),
		lastIdle(GetTickCount())
	{ 	
		InitCommonLdl(ldlBlock);
	};
	
	c3_TriggeredAnimation(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height,
		const MBCHAR *text = NULL, uint32 maxLength = 0 ) 
	:	aui_Static(retval, id, x, y, width, height, text, maxLength),
		m_frames(NULL),
		m_currentFrame(0),
		m_blendSpeed(100),
		m_blendVal(k_C3_ANIMATION_MAXBLEND),
		lastIdle(GetTickCount())
	{
	};
	
	virtual ~c3_TriggeredAnimation() 
	{ 
		delete m_frames; 
	};
	
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0);

	
	
	virtual AUI_ERRCODE Idle();

	
	void TriggerAnimationStep();

	
	
	void TriggerAnimationBlend();

protected:

	
	c3_TriggeredAnimation()
	:	aui_Static(),
		m_frames(NULL)
	{
	};

	
	virtual AUI_ERRCODE DrawBlendImage(aui_Surface *destSurf, RECT *destRect);

	
	void InitCommonLdl(MBCHAR *ldlBlock);
	
private:

	
	aui_StringTable *m_frames;

	
	sint32 m_currentFrame;

	
	sint32 m_blendSpeed;

	
	sint32 m_blendVal;

	
	uint32 lastIdle;
};


AUI_ERRCODE c3_TriggeredAnimation::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{
	
	if (IsHidden()) return AUI_ERRCODE_OK;

	
	if (!surface) surface = m_window->TheSurface();

	
	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	DrawBlendImage(surface, &rect);

	
	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_TriggeredAnimation::DrawBlendImage(aui_Surface *destSurf, RECT *destRect)
{
	
	AUI_ERRCODE errcode;						

	
	if(m_blendVal >= k_C3_ANIMATION_MAXBLEND) {
		DrawThisStateImage(0, destSurf, destRect);
		return AUI_ERRCODE_OK;
	}

	
	
	aui_Image *image = GetImage( 0, AUI_IMAGEBASE_SUBSTATE_STATE );
	if(!image) return AUI_ERRCODE_OK;

	
	aui_Surface *srcSurf = image->TheSurface();

	
	RECT srcRect = { 0, 0, srcSurf->Width(), srcSurf->Height() };

	
	
	aui_Image *lastImage = GetImage( 1, AUI_IMAGEBASE_SUBSTATE_STATE );
	if(!lastImage) return AUI_ERRCODE_OK;

	
	aui_Surface *lastSurf = lastImage->TheSurface();

	
	RECT lastRect = { 0, 0, lastSurf->Width(), lastSurf->Height() };

	
	aui_Surface *backSurface = aui_Factory::new_Surface(errcode, srcRect.right, srcRect.bottom,
		k_C3_ANIMATION_BITS_PER_PIXEL);
	Assert(AUI_NEWOK(backSurface, errcode));

	
	g_c3ui->TheBlitter()->Blt(backSurface, 0, 0, destSurf, destRect, k_AUI_BLITTER_FLAG_COPY);

	
	
	if(m_imagebltflag == AUI_IMAGEBASE_BLTFLAG_CHROMAKEY) {
		
		aui_Surface *frontSurface = aui_Factory::new_Surface(errcode, lastRect.right, lastRect.bottom, k_C3_ANIMATION_BITS_PER_PIXEL);
		Assert(AUI_NEWOK(frontSurface, errcode));
		
		
		g_c3ui->TheBlitter()->Blt(frontSurface, 0, 0, destSurf, destRect, k_AUI_BLITTER_FLAG_COPY);

		
		g_c3ui->TheBlitter()->Blt(frontSurface, 0, 0, lastSurf, &lastRect, k_AUI_BLITTER_FLAG_CHROMAKEY);

		
		
		primitives_BlendSurfaces(frontSurface, backSurface, destSurf, destRect,
			k_C3_ANIMATION_MAXBLEND - m_blendVal);

		
		delete frontSurface;
	} else {
		
		primitives_BlendSurfaces(lastSurf, backSurface, destSurf, destRect,
			k_C3_ANIMATION_MAXBLEND - m_blendVal);
	}

	
	g_c3ui->TheBlitter()->Blt(backSurface, 0, 0, destSurf, destRect, k_AUI_BLITTER_FLAG_COPY);

	
	
	if(m_imagebltflag == AUI_IMAGEBASE_BLTFLAG_CHROMAKEY) {
		
		aui_Surface *frontSurface = aui_Factory::new_Surface(errcode, srcRect.right, srcRect.bottom,
			k_C3_ANIMATION_BITS_PER_PIXEL);
		Assert(AUI_NEWOK(frontSurface, errcode));
		
		
		g_c3ui->TheBlitter()->Blt(frontSurface, 0, 0, destSurf, destRect, k_AUI_BLITTER_FLAG_COPY);

		
		g_c3ui->TheBlitter()->Blt(frontSurface, 0, 0, srcSurf, &srcRect, k_AUI_BLITTER_FLAG_CHROMAKEY);

		
		primitives_BlendSurfaces(frontSurface, backSurface, destSurf, destRect, m_blendVal);

		
		delete frontSurface;
	} else {
		
		primitives_BlendSurfaces(srcSurf, backSurface, destSurf, destRect, m_blendVal);
	}

	
	delete backSurface;

	
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_TriggeredAnimation::Idle()
{
	
	if(m_blendVal >= k_C3_ANIMATION_MAXBLEND) return AUI_ERRCODE_OK;

	
	int index = 0;

	
	sint32 deltaTime = GetTickCount() - lastIdle;

	
	if(deltaTime < m_blendSpeed) return AUI_ERRCODE_OK;

	
	
	sint32 blendTime = 0;
	do {
		
		blendTime += k_C3_ANIMATION_BLEND_STEP;

		
		deltaTime -= m_blendSpeed;
	} while((deltaTime - m_blendSpeed) > 0);

	
	lastIdle = GetTickCount() + (m_blendSpeed - deltaTime);

	
	
	if(!m_blendVal) blendTime = k_C3_ANIMATION_BLEND_STEP;

	
	m_blendVal += blendTime;
	if(m_blendVal > k_C3_ANIMATION_MAXBLEND) m_blendVal = k_C3_ANIMATION_MAXBLEND;

	
	GetParentWindow()->ShouldDraw();

	
	return AUI_ERRCODE_OK;
}


void c3_TriggeredAnimation::InitCommonLdl(MBCHAR *ldlBlock)
{
	
	AUI_ERRCODE errcode;						
	MBCHAR ldlString[k_AUI_LDL_MAXBLOCK + 1];	

	
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert(valid);
	if(!valid) return;

	
	ldl_datablock *datablock = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert(datablock != NULL);
	if(!datablock) return;

	
	m_blendSpeed			= datablock->GetInt(k_C3_ANIMATION_BLEND_SPEED);

	
	sprintf(ldlString, "%s.%s", ldlBlock, k_C3_ANIMATION_FRAMES);		
	m_frames = new aui_StringTable(&errcode, ldlString);				
	Assert(m_frames);													

	
	m_currentFrame = 0;
	SetImage(m_frames->GetString(m_currentFrame));
}


void c3_TriggeredAnimation::TriggerAnimationStep()
{
	
	if(!m_frames) return;

	
	m_currentFrame++;

	
	while(m_currentFrame >= m_frames->GetNumStrings())
		m_currentFrame -= m_frames->GetNumStrings();

	
	SetImage(m_frames->GetString(m_currentFrame));

	
	GetParentWindow()->ShouldDraw();
}



void c3_TriggeredAnimation::TriggerAnimationBlend()
{
	
	if(!m_frames) return;

	
	SetImage(m_frames->GetString(m_currentFrame), 1);

	
	m_blendVal = 0;

	
	TriggerAnimationStep();
}









struct sCreditsLine
{
	MBCHAR			 m_text[k_CreditsLineLen];
	uint32			 m_font;
	sCreditsLine	*m_pNext;

	sCreditsLine(uint32 font, MBCHAR *pText);
};



class cCreditsPage
{
public:
	void			 AddLine(uint32 font, MBCHAR *pText);
	void			 ResetLines(void); 
	cCreditsPage(void);
	~cCreditsPage();
	
	cCreditsPage	*m_pNext;

	uint32			 m_numLines;
	sCreditsLine	*m_pLines;
	sCreditsLine	*m_pCurrLine;

};


enum eTokenType
{
	kBadToken,
	kFont,
	kText,
	kFontDefinition,
	kPageBreak,
	kEndOfCredits,
	kComment,
	kFontSize
};




class c3_CreditsText : public aui_Static
{
public:
	c3_CreditsText *Parse(FILE *textfile);
	void Draw(aui_Surface *surface);
	void ResetPages();

	c3_CreditsText
	(
		AUI_ERRCODE *	retval, 
		uint32			id, 
		MBCHAR *		ldlBlock
	) 
	:	aui_Static(retval, id, ldlBlock),
		m_lastIdle(GetTickCount()),
		m_numPages(0),
		m_pPages(NULL),
		m_pCurrPage(NULL),
		m_definingFont(false),
		m_currFontNumber(0),
		m_currFontSize(0),
		m_numFonts(0)
	{
		std::fill(m_fonts, m_fonts + kCreditsTextNumFonts, (aui_BitmapFont *) NULL);

		aui_Ldl *		theLdl		= g_c3ui->GetLdl();
		BOOL const		valid		= theLdl->IsValid(ldlBlock);
		Assert(valid);
		if (!valid) return;

		ldl_datablock *	datablock	= theLdl->GetLdl()->FindDataBlock(ldlBlock);
		Assert(datablock);
		if (!datablock) return;

		m_animationSpeed			= datablock->GetInt(k_C3_ANIMATION_SPEED);
		InitCommonLdl(ldlBlock);
	};

	c3_CreditsText
	(
		AUI_ERRCODE *	retval, 
		uint32			id, 
		sint32			x, 
		sint32			y, 
		sint32			width, 
		sint32			height,
		const MBCHAR *	text		= NULL, 
		uint32			maxLength	= 0 
	) 
	:	aui_Static(retval, id, x, y, width, height, text, maxLength),
		m_lastIdle(GetTickCount()),
		m_numPages(0),
		m_pPages(NULL),
		m_pCurrPage(NULL),
		m_definingFont(false),
		m_currFontNumber(0),
		m_currFontSize(0),
		m_animationSpeed(3000),
		m_numFonts(0)
	{
		std::fill(m_fonts, m_fonts + kCreditsTextNumFonts, (aui_BitmapFont *) NULL);
	};


	virtual ~c3_CreditsText(void)
	{
		cCreditsPage *pFoo;
		ResetPages();
		while (m_pCurrPage)
		{
			pFoo = m_pCurrPage;
			m_pCurrPage = m_pCurrPage->m_pNext;
			delete pFoo;
		}

		for (int i = 0; (unsigned) i < kCreditsTextNumFonts; i++)
		{
			if (m_fonts[i])
				g_c3ui->UnloadBitmapFont(m_fonts[i]);
		}

	};

	void NewPage(void);

	
	virtual AUI_ERRCODE DrawThis(aui_Surface *pSurface = NULL, sint32 x = 0, sint32 y = 0);

	
	
	virtual AUI_ERRCODE Idle();

	
	uint32 m_lastIdle;


private:

	bool ParseFontDef(MBCHAR *pToken);
	uint32 GetFontNumber(MBCHAR *pToken);
	uint32 GetFontSize(MBCHAR *pToken);
	uint32 GetNumberFromToken(MBCHAR *pToken);
	eTokenType TokenType(MBCHAR *pToken);
	bool IsDelimeter(char c);
	bool IsComment(char c);
	void ReadToEOL(FILE *textfile);

	uint32			 m_numPages;
	cCreditsPage	*m_pPages;
	cCreditsPage	*m_pCurrPage;

	aui_BitmapFont	*m_fonts[kCreditsTextNumFonts];

	bool			 m_definingFont;
	uint32			 m_currFontNumber;
	uint32			 m_currFontSize;
	uint32			 m_currTextfileLine;

	
	sint32 m_animationSpeed;

	uint32 m_numFonts;

};









CreditsWindow::CreditsWindow(AUI_ERRCODE *retval, sint32 id, MBCHAR *ldlBlock, sint32 bpp,
							 AUI_WINDOW_TYPE type, bool bevel)
	:	C3Window(retval, id, ldlBlock, bpp, type, bevel)
{
	
	m_animationSpeed = 10000;
	m_animating = true;
	lastIdle = GetTickCount();

	
	CleanPointers();

	
	InitCommonLdl(ldlBlock);
}


CreditsWindow::CreditsWindow(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y,
							 sint32 width, sint32 height, sint32 bpp, MBCHAR *pattern,
							 AUI_WINDOW_TYPE type, bool bevel)
	:	C3Window(retval, id, x, y, width, height, bpp, pattern, type, bevel)
{
	
	m_animationSpeed = 10000;
	m_animating = true;
	lastIdle = GetTickCount();

	
	CleanPointers();
}


CreditsWindow::~CreditsWindow()
{
	
	int index;

	
	CleanUp(m_exitButton);
	CleanUp(m_secretButton);
	CleanUp(m_pauseButton);
	CleanUp(m_border);
	CleanUp(m_secretImage);

	
	
	CleanUp(m_creditsText);

	if(m_backgroundAnim) {
		for(index = m_numberOfBackgroundAnims-1; index >= 0; index--) CleanUp(m_backgroundAnim[index]);
		delete [] m_backgroundAnim;
	}
	if(m_triggeredAnim) {
		for(index = m_numberOfTriggeredAnims-1; index >= 0; index--) CleanUp(m_triggeredAnim[index]);
		delete [] m_triggeredAnim;
	}
	CleanUp(m_background);

	
	CleanPointers();
}


AUI_ERRCODE CreditsWindow::Idle()
{
	
	if(!m_animating) return AUI_ERRCODE_OK;

	
	int index = 0;

	sint32 tickCount = GetTickCount();

	
	sint32 deltaTime = GetTickCount() - lastIdle;

	
	if(deltaTime < m_animationSpeed) return AUI_ERRCODE_OK;

	
	
	do {
		
		deltaTime -= m_animationSpeed;
	} while((deltaTime - m_animationSpeed) > 0);

	
	lastIdle = GetTickCount() - deltaTime;
		

	
	for(index = m_numberOfTriggeredAnims-1; index >= 0; index--)
		m_triggeredAnim[index]->TriggerAnimationStep();

	
	
	

	
	return AUI_ERRCODE_OK;
}


void CreditsWindow::InitCommonLdl(MBCHAR *ldlBlock)
{
	
	AUI_ERRCODE errcode;						
	MBCHAR ldlString[k_AUI_LDL_MAXBLOCK + 1];	
	int index;									

	
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert(valid);
	if(!valid) return;

	
	ldl_datablock *datablock = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert(datablock != NULL);
	if(!datablock) return;

	
	

	
	sprintf(ldlString, "%s.%s", ldlBlock, k_LDL_CREDITS_BACKGROUND);	
	m_background = new aui_Static(&errcode, aui_UniqueId(), ldlString); 
	Assert(m_background);												
	m_background->IgnoreEvents(TRUE);

	
	m_numberOfBackgroundAnims = datablock->GetInt(k_LDL_CREDITS_BACKANIM_COUNT);
	m_backgroundAnim = m_numberOfBackgroundAnims > 0 ? new c3_SimpleAnimation *[m_numberOfBackgroundAnims] : NULL;

	
	for(index = 0; index < m_numberOfBackgroundAnims; index++) {
		sprintf(ldlString, "%s.%s%d", ldlBlock, k_LDL_CREDITS_BACKANIM_BASE, index+1);			
		m_backgroundAnim[index] = new c3_SimpleAnimation(&errcode, aui_UniqueId(), ldlString);	
		Assert(m_backgroundAnim[index]);														
		m_backgroundAnim[index]->IgnoreEvents(TRUE);
	}

	
	m_animationSpeed			= datablock->GetInt(k_C3_ANIMATION_SPEED);

	
	m_numberOfTriggeredAnims = datablock->GetInt(k_LDL_CREDITS_TRIGGERANIM_COUNT);
	m_triggeredAnim = m_numberOfTriggeredAnims > 0 ? new c3_TriggeredAnimation *[m_numberOfTriggeredAnims] : NULL;

	
	for(index = 0; index < m_numberOfTriggeredAnims; index++) {
		sprintf(ldlString, "%s.%s%d", ldlBlock, k_LDL_CREDITS_TRIGGERANIM_BASE, index+1);		
		m_triggeredAnim[index] = new c3_TriggeredAnimation(&errcode, aui_UniqueId(), ldlString);
		Assert(m_triggeredAnim[index]);															
		m_triggeredAnim[index]->IgnoreEvents(TRUE);
	}








	
	
	if (m_creditsText == NULL)
	{
		FILE *fp = c3files_fopen(C3DIR_UIDATA, k_CREDITS_FILENAME, "r");
		Assert(fp);

		
		m_creditsText = new c3_CreditsText(&errcode, aui_UniqueId(), ldlBlock);
		Assert(m_creditsText);
		
		
		m_creditsText = m_creditsText->Parse(fp);
		
		
		fclose(fp);

		if (m_creditsText)
		{
			m_creditsText->IgnoreEvents(TRUE);
			m_creditsText->m_lastIdle = GetTickCount();
		}
	}

	if (m_creditsText)
	{
		m_creditsText->ResetPages();
	}


	
	sprintf(ldlString, "%s.%s", ldlBlock, k_LDL_CREDITS_SECRET_IMAGE);		
	m_secretImage = new aui_Static(&errcode, aui_UniqueId(), ldlString);	
	Assert(m_secretImage);													
	m_secretImage->IgnoreEvents(TRUE);
	m_secretImage->Hide();													

	
	sprintf(ldlString, "%s.%s", ldlBlock, k_LDL_CREDITS_BORDER);		
	m_border = new aui_Static(&errcode, aui_UniqueId(), ldlString);		
	Assert(m_border);													
	m_border->IgnoreEvents(TRUE);

	
	sprintf(ldlString, "%s.%s", ldlBlock, k_LDL_CREDITS_PAUSE_BUTTON);
	m_pauseButton = new aui_Button(&errcode, aui_UniqueId(), ldlString,
		creditsscreen_PauseButtonActionCallback);
	Assert(m_pauseButton);

	
	sprintf(ldlString, "%s.%s", ldlBlock, k_LDL_CREDITS_SECRET_BUTTON);
	m_secretButton = new aui_Button(&errcode, aui_UniqueId(), ldlString,
		creditsscreen_SecretButtonActionCallback);
	Assert(m_secretButton);

	
	sprintf(ldlString, "%s.%s", ldlBlock, k_LDL_CREDITS_EXIT_BUTTON);
	m_exitButton = new ctp2_Button(&errcode, aui_UniqueId(), ldlString,
		creditsscreen_ExitButtonActionCallback);
	Assert(m_exitButton);

	
	AddControl(m_exitButton);
	AddControl(m_secretButton);
	AddControl(m_pauseButton);
	AddControl(m_border);
	AddControl(m_secretImage);

	
	
	
	if (m_creditsText)
		AddControl(m_creditsText);

	for(index = m_numberOfBackgroundAnims-1; index >= 0; index--)
		AddControl(m_backgroundAnim[index]);
	for(index = m_numberOfTriggeredAnims-1; index >= 0; index--)
		AddControl(m_triggeredAnim[index]);
	AddControl(m_background);
}


void CreditsWindow::CleanPointers()
{
	
	m_numberOfBackgroundAnims = 0;
	m_numberOfTriggeredAnims = 0;

	
	m_background = NULL;
	m_backgroundAnim = NULL;
	m_triggeredAnim = NULL;

	
	
	
	m_creditsText = NULL;
	m_secretImage = NULL;
	m_border = NULL;
	m_pauseButton = NULL;
	m_secretButton = NULL;
	m_exitButton = NULL;
}


void CreditsWindow::CleanUp(aui_Control *control)
{
	
	if(!control) return;

	
	RemoveControl(control);

	
	delete control;
}


void CreditsWindow::ToggleAnimation()
{
	
	m_animating = !m_animating;

	
	if(!(m_secretImage->IsHidden())) {
		m_secretImage->Hide();
		ShouldDraw();
	}
}


void CreditsWindow::ShowSecretImage()
{
	
	if(m_secretImage->IsHidden()) {
		
		m_animating = false;

		
		m_secretImage->Show();

		
		ShouldDraw();
	} else {	
		
		
		ToggleAnimation();
	}
}










sCreditsLine::sCreditsLine(uint32 font, MBCHAR *pText)
{
	strcpy(m_text, pText);
	m_font = font;
	m_pNext = NULL;
};

cCreditsPage::cCreditsPage(void)
{
	m_numLines = 0;
	m_pLines = NULL;
	m_pCurrLine = NULL;
	m_pNext = NULL;
}


cCreditsPage::~cCreditsPage()
{
	sCreditsLine *pFoo;
	ResetLines();
	while (m_pCurrLine)
	{
		pFoo = m_pCurrLine;
		m_pCurrLine = m_pCurrLine->m_pNext;
		delete pFoo;
	}
}


void cCreditsPage::AddLine(uint32 font, MBCHAR *pText)
{
	sCreditsLine *pLine = new sCreditsLine(font, pText);
	if (m_numLines)
	{
		m_pCurrLine->m_pNext = pLine;
	}
	else
	{
		m_pLines = pLine;
	}

	m_numLines++;
	m_pCurrLine = pLine;
}



void cCreditsPage::ResetLines(void)
{
	m_pCurrLine = m_pLines;
}




extern ColorSet		*g_colorSet;
AUI_ERRCODE c3_CreditsText::DrawThis(aui_Surface *pSurface, sint32 x, sint32 y)
{

	
	if (IsHidden()) return AUI_ERRCODE_OK;

	
	if (!pSurface) pSurface = m_window->TheSurface();

	
	if (m_pCurrPage)
	{
		RECT rect;

		m_pCurrPage->ResetLines();

		sCreditsLine *pCurrLine = m_pCurrPage->m_pLines;
		
		
		sint32 initialX = 8;
		sint32 initialY = 8;
		sint32 centerX = (((g_creditsWindow->Width() - 8) - initialX)/2)+initialX;

		sint32 currY = 0;

		while (pCurrLine)
		{
			aui_BitmapFont *pCurrFont = m_fonts[pCurrLine->m_font];

			MBCHAR *pText = pCurrLine->m_text;
	
			sint32 width = pCurrFont->GetStringWidth(pText);
			sint32 height = pCurrFont->GetMaxHeight();
			sint32 x = centerX - width/2;

			

			SetRect(&rect, x, initialY+currY, x+width, initialY+currY+pCurrFont->GetMaxHeight());

			pCurrFont->DrawString(pSurface, &rect, &rect, pText, 0,
								  g_colorSet->GetColorRef(COLOR_WHITE), 0);

			currY += height + (height / 8);
			pCurrLine = pCurrLine->m_pNext;
		}


		
		if ( pSurface == m_window->TheSurface() )
			m_window->AddDirtyRect( &rect );


	}


	
	return AUI_ERRCODE_OK;
}


c3_CreditsText * c3_CreditsText::Parse(FILE *textfile)
{

	MBCHAR currToken[128];
	MBCHAR errorStr[128];
	char c;
	eTokenType tokenType;
	uint32 currFont = (unsigned) -1;
	int i;
	bool done = false;
	bool gettingText = false;


	NewPage();

	m_currTextfileLine = 0;

	while (done == false)
	{
		if (gettingText == false)
		{
			bool sawOpen = false;
			
			c = (char) fgetc(textfile);
			
			for (i = 0; ((i < 127) && (!IsDelimeter(c))); i++, c = (char)fgetc(textfile) )
			{
				if (c == '\r')
				{
					i--;
					continue;
				}
				currToken[i] = c;
				if (IsComment(c))
				{
					if (sawOpen == false)
					{
						i++;
						break;
					}
					
				}
				if (c == '<')
					sawOpen = true;
			}
		}
		else
		{
			c = (char)fgetc(textfile);
			for (i = 0; ((i < 127) && (c != '\n'));
				i++, c = (char)fgetc(textfile) )
			{
				if (c == '\r')
				{
					i--;
					continue;
				}
				currToken[i] = c;
#if 0
				if (IsComment(c)) 
				{
					ReadToEOL(textfile);
				}
#endif
			}

			
			currToken[i] = '\0';

			m_pCurrPage->AddLine(currFont, currToken);
			m_currTextfileLine++;

			gettingText = false;
			continue;
		}


		
		currToken[i] = '\0';


		
		tokenType = TokenType(currToken);
		switch (tokenType)
		{
			case kFont:
			{
				currFont = GetFontNumber(currToken);
				if ((currFont == (unsigned) -1) || (currFont > m_numFonts) )
				{
					sprintf(errorStr, "%s line %d: Bad font specifier '%s'", k_CREDITS_FILENAME, m_currTextfileLine, currToken);
					MessageBoxDialog::Information(errorStr, "CreditsError");

					delete this;
					return NULL;
				}

				gettingText = TRUE;
				break;
			}
			case kText:
			{
				if (m_definingFont)
				{
					if (ParseFontDef(currToken))
					{
						m_definingFont = FALSE;
					}
					else
					{
						sprintf(errorStr, "%s line %d: Bad font filename '%s'", k_CREDITS_FILENAME, m_currTextfileLine, currToken);
						MessageBoxDialog::Information(errorStr, "CreditsError");

						delete this;
						return NULL;
					}
					break;
				}
				sprintf(errorStr, "%s line %d: Illegal text'%s'", k_CREDITS_FILENAME, m_currTextfileLine, currToken);
				MessageBoxDialog::Information(errorStr, "CreditsError");
				
				delete this;
				return NULL;
			}
			case kFontDefinition:
			{
				m_definingFont = TRUE;
				m_currFontNumber = GetFontNumber(currToken);
				if (m_currFontNumber == (unsigned) -1)
				{
					sprintf(errorStr, "%s line %d: Bad font definition '%s'", k_CREDITS_FILENAME, m_currTextfileLine, currToken);
					MessageBoxDialog::Information(errorStr, "CreditsError");
					
					delete this;
					return NULL;
				}
				break;
			}
			case kFontSize:
			{
				m_currFontSize = GetFontSize(currToken);
				if (m_currFontSize == (unsigned) -1)
				{
					sprintf(errorStr, "%s line %d: Bad font size '%s'", k_CREDITS_FILENAME, m_currTextfileLine, currToken);
					MessageBoxDialog::Information(errorStr, "CreditsError");
					
					delete this;
					return NULL;
				}
				break;
			}

			case kPageBreak:
			{
				NewPage();
				ReadToEOL(textfile); 
				break;
			}
			case kComment:
			{
				ReadToEOL(textfile);
				break;
			}

			case kEndOfCredits:
			{
				
				return this;
			}
			case kBadToken:
			{
				sprintf(errorStr, "%s line %d: Bad token '%s'", k_CREDITS_FILENAME, m_currTextfileLine, currToken);
				MessageBoxDialog::Information(errorStr, "CreditsError");

				delete this;
				return NULL;
			}
		}



	}

	sprintf(errorStr, "%s line %d: Unexpected end of file found", k_CREDITS_FILENAME, m_currTextfileLine);
	MessageBoxDialog::Information(errorStr, "CreditsError");
	
	delete this;
	return NULL;
}

void c3_CreditsText::NewPage(void)
{
	cCreditsPage *pNewPage;

	pNewPage = new cCreditsPage();

	if (m_numPages)
	{
		m_pCurrPage->m_pNext = pNewPage;
		m_pCurrPage = pNewPage;
	}
	else
	{
		m_pPages = pNewPage;
		m_pCurrPage = m_pPages;
	}

	m_numPages++;
}


void c3_CreditsText::ReadToEOL(FILE *textfile)
{
	char c = (char)fgetc(textfile);
	while( ((c != '\n') && !feof(textfile) ))
	{
		c = (char)fgetc(textfile);
	}

	if (c == '\n')
		m_currTextfileLine++;
}


bool c3_CreditsText::IsComment(char c)
{
	if (c == ';')
		return true;

	return false;
}


bool c3_CreditsText::IsDelimeter(char c)
{
	switch (c)
	{
		case '\n':
		{
			m_currTextfileLine++;
			
		}
		case '>':
		case ' ':
		{
			return true;
		}
	}

	return false;
}




eTokenType c3_CreditsText::TokenType(MBCHAR *pToken)
{
	if (!strnicmp(pToken, "<fontDef", 8))
	{
		return kFontDefinition;
	}

	if (!strnicmp(pToken, "<font", 5))
	{
		return kFont;
	}

	if (!strnicmp(pToken, "<size", 5))
	{
		return kFontSize;
	}

	if (!stricmp(pToken, "<page"))
	{
		return kPageBreak;
	}

	if (!stricmp(pToken, "<end"))
	{
		return kEndOfCredits;
	}

	if (IsComment(*pToken))
	{
		return kComment;
	}

	if (*pToken == '>')
	{
		
		return kBadToken;
	}

	return kText;
}


bool c3_CreditsText::ParseFontDef(MBCHAR *pToken)
{
	MBCHAR errorStr[128];

	
	if (m_fonts[m_currFontNumber])
	{
		
		sprintf(errorStr, "%s line %d: Font already defined '%s'", k_CREDITS_FILENAME, m_currTextfileLine, pToken);
		MessageBoxDialog::Information(errorStr, "CreditsError");	

		return FALSE;
	}


	m_fonts[m_currFontNumber] = g_c3ui->LoadBitmapFont(pToken, m_currFontSize);
	Assert(m_fonts);

	m_fonts[m_currFontNumber]->SetPointSize(m_currFontSize);

	if ((m_currFontNumber + 1)> m_numFonts)
		m_numFonts = m_currFontNumber + 1; 

	return (m_fonts[m_currFontNumber] != NULL);
}



uint32 c3_CreditsText::GetFontSize(MBCHAR *pToken)
{
	
	return GetNumberFromToken(pToken);
}


uint32 c3_CreditsText::GetNumberFromToken(MBCHAR *pToken)
{

	MBCHAR *pFoo = pToken;
	bool error = true;
	int i;
	int len = strlen(pToken);

	for (i = 0; i < len; i++)
	{
		if (isdigit(*pFoo))
		{
			error = false;
			break;
		}
		pFoo = pFoo + 1;
	}

	if (error)
	{
		return (unsigned) -1;
	}

	return atoi(pFoo);
}



uint32 c3_CreditsText::GetFontNumber(MBCHAR *pToken)
{
	uint32 retval = GetNumberFromToken(pToken);

	if ((retval < 0)  || (retval > kCreditsTextNumFonts))
	{
		return (unsigned) -1;
	}

	return retval;
}


void c3_CreditsText::ResetPages()
{
	m_pCurrPage = m_pPages;

	cCreditsPage *pFoo = m_pPages;
	
	while (pFoo)
	{
		pFoo->ResetLines();
		pFoo = pFoo->m_pNext;
	}
}


AUI_ERRCODE c3_CreditsText::Idle()
{
	
	int index = 0;

	
	sint32 deltaTime = GetTickCount() - m_lastIdle;

	
	if(deltaTime < m_animationSpeed) return AUI_ERRCODE_OK;

	
	
	sint32 animationTime = 0;
	do {
		
		animationTime++;

		
		deltaTime -= m_animationSpeed;
	} while((deltaTime - m_animationSpeed) > 0);

	
	m_lastIdle = GetTickCount() - deltaTime;
		

	
	
	if (m_pCurrPage)
	{
		m_pCurrPage = m_pCurrPage->m_pNext;
		if (m_pCurrPage == NULL)
		{
			
			ResetPages();
		}
	}

	
	GetParentWindow()->ShouldDraw();


	
	return AUI_ERRCODE_OK;
}
