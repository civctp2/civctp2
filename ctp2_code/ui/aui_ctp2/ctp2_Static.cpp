#include "c3.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_ui.h"
#include "aui_window.h"
#include "aui_gamespecific.h"

#include "ctp2_Static.h"

#include "patternbase.h"
#include "pattern.h"
#include "primitives.h"
#include "colorset.h"

extern aui_UI		*g_ui;
extern ColorSet		*g_colorSet;

ctp2_Static::ctp2_Static(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Static( retval, id, ldlBlock ),
	PatternBase(ldlBlock, NULL)
{
	m_drawFunc			= NULL;
	m_drawCookie		= NULL;
	m_imageMapCallback  = NULL;
	m_imageMapCookie    = NULL;
	m_multiImageStatic	= false;
	m_drawCallbackExclusive	= true;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


ctp2_Static::ctp2_Static(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	MBCHAR *text,
	uint32 maxLength,
	uint32 bevelWidth,
	uint32 bevelType)
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text, maxLength ),
	aui_Static( retval, id, x, y, width, height, text, maxLength),
	PatternBase(pattern)
{

	m_multiImageStatic	= false;
	m_drawCallbackExclusive	= true;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(bevelWidth, bevelType);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE ctp2_Static::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	sint32 chromaRed=0,chromaGreen=0,chromaBlue=0;
	bool   chromaSpecified=false;

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );

	if ( !block )
		return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	if (block->GetAttributeType(k_AUI_IMAGEBASE_LDL_CHROMAKEY_RED  ) == ATTRIBUTE_TYPE_INT)
	{
		chromaRed		 =block->GetInt(k_AUI_IMAGEBASE_LDL_CHROMAKEY_RED);
		chromaSpecified=true;
	}

	if (block->GetAttributeType(k_AUI_IMAGEBASE_LDL_CHROMAKEY_GREEN) == ATTRIBUTE_TYPE_INT)
	{
		chromaGreen	 =block->GetInt(k_AUI_IMAGEBASE_LDL_CHROMAKEY_GREEN);
		chromaSpecified=true;
	}

	if (block->GetAttributeType(k_AUI_IMAGEBASE_LDL_CHROMAKEY_BLUE ) == ATTRIBUTE_TYPE_INT)
	{
		chromaBlue	 =block->GetInt(k_AUI_IMAGEBASE_LDL_CHROMAKEY_BLUE);
		chromaSpecified=true;
	}

	if (block->GetAttributeType( k_CTP2_STATIC_LDL_BEVELWIDTH ) == ATTRIBUTE_TYPE_INT) {
		m_bevelWidth = block->GetInt( k_CTP2_STATIC_LDL_BEVELWIDTH );
	} else {
		m_bevelWidth = 0;
	}

	if (block->GetAttributeType( k_CTP2_STATIC_LDL_BEVELTYPE ) == ATTRIBUTE_TYPE_INT) {
		m_bevelType = block->GetInt( k_CTP2_STATIC_LDL_BEVELTYPE );
	} else {
		m_bevelType = 0;
	}

	if (chromaSpecified)
		SetChromaKey(chromaRed,chromaGreen,chromaBlue);

	if (block->GetBool(k_CTP2_STATIC_LDL_STATICIMAGETYPE))
		m_multiImageStatic = BuildImageList();

	m_ignoreHighlight = block->GetBool("nohighlight") ? true : false;







	if (m_width == 0 && m_height == 0 && !m_multiImageStatic) {
		FitToBitmap();
	}

	if(!m_width && !m_height) {

		TextReloadFont();

		if(GetText() && GetTextFont()) {

			Resize(GetTextFont()->GetStringWidth(GetText()),
				GetTextFont()->GetPointSize());
		}
	}

	return AUI_ERRCODE_OK;

	return InitCommon(m_bevelWidth, m_bevelType);
}


AUI_ERRCODE ctp2_Static::InitCommon(uint32 bevelWidth, uint32 bevelType )
{
	m_bevelWidth = bevelWidth;
	m_bevelType = bevelType;

	return AUI_ERRCODE_OK;
}

bool
ctp2_Static::ConstructImageRect(uint32 index)
{

	if (index>=STATIC_IMAGE_MAX)
		return false;

	RECT *rect=&(m_textRects[index]);

	rect->left = 0;
	rect->top  = 0;

	aui_Image	*image = GetImage(index, AUI_IMAGEBASE_SUBSTATE_FIRST);

	if (image == NULL)
		return false;

	aui_Surface	*surf = image->TheSurface();

	Assert(surf != NULL);

	if (surf == NULL)
		return false;

	rect->right  = surf->Width();
	rect->bottom = surf->Height();

	return true;
}

bool
ctp2_Static::BuildImageList()
{
	bool images_valid = true;

	images_valid &= ConstructImageRect(STATIC_IMAGE_LEFT  );
	images_valid &= ConstructImageRect(STATIC_IMAGE_CENTER);
	images_valid &= ConstructImageRect(STATIC_IMAGE_RIGHT );

	if (!images_valid)
		return false;

	sint32 total_width=0;
	sint32 total_height=0;
	uint32 i;

	for (i=STATIC_IMAGE_LEFT;i<STATIC_IMAGE_MAX;i++)
	{
		total_width  +=	m_textRects[i].right;
		if (total_height<m_textRects[i].bottom)
			total_height=m_textRects[i].bottom;
	}

	sint32 w=Width();

	if (w<total_width)
		w=total_width;

	Resize(w,total_height);

	m_textRects[STATIC_IMAGE_CENTER].right=Width()-(m_textRects[STATIC_IMAGE_LEFT].right+m_textRects[STATIC_IMAGE_RIGHT].right);

	return true;
}


void
ctp2_Static::SetChromaKey(sint32 r,sint32 g,sint32 b)
{
	sint32 i=0;
	aui_Image	*image;
	sint32 substate=AUI_IMAGEBASE_SUBSTATE_FIRST;
	do
	{
		image = GetImage(i++, (AUI_IMAGEBASE_SUBSTATE)substate);

		if (image!=NULL)
			image->SetChromakey(r,g,b);
		else
		{
			i = 0;
			substate +=1;
		}

	} while(substate<AUI_IMAGEBASE_SUBSTATE_LAST);
}

AUI_ERRCODE
ctp2_Static::Resize(sint32 width, sint32 height)
{
	aui_Static::Resize(width, height);

	if(m_multiImageStatic) {
		m_textRects[STATIC_IMAGE_CENTER].right=Width()-(m_textRects[STATIC_IMAGE_LEFT].right+m_textRects[STATIC_IMAGE_RIGHT].right);
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE
ctp2_Static::DrawImages(aui_Surface *surface,RECT *destRect)
{

	if (destRect==NULL)
		return AUI_ERRCODE_OK;

	if (!surface)
		surface = m_window->TheSurface();

	sint32 width=0;

	RECT drawRect;

	for(uint32 i=0;i<STATIC_IMAGE_MAX;i++)
	{
		drawRect.top	= destRect->top;
		drawRect.bottom = destRect->bottom;
		drawRect.left	= destRect->left + width;
		drawRect.right	= destRect->left + width+m_textRects[i].right;

		width += m_textRects[i].right;

		DrawThisStateImage(i,surface,&drawRect);
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ctp2_Static::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if((m_drawFunc)&&(m_drawCallbackExclusive)) {
		if (m_window)
			if ( surface == m_window->TheSurface() )
				m_window->AddDirtyRect( &rect );
		return m_drawFunc(this, surface, rect, m_drawCookie);
	}

	if ( m_pattern )
	{
		if ( m_srcWidthPix || m_srcHeightPix )
		{
			RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
			m_pattern->Draw( surface, &rect, &srcRect );
		}
		else
			m_pattern->Draw( surface, &rect );
	}

	if(GetNumberOfLayers()) {

		DrawLayers(surface, &rect);
	} else {

		if (m_multiImageStatic)
			DrawImages(surface,&rect);
		else
			DrawThisStateImage(0,surface,&rect );
	}









	if (m_bevelWidth > 0)
	{
		if ( m_bevelType == 3 ) {
			RECT oobRect = {
				rect.left -2,
				rect.top - 2,
				rect.right + 2,
				rect.bottom + 2
			};
			primitives_FrameThickRect16( surface, &oobRect, g_colorSet->GetColor( COLOR_UI_BOX ), m_bevelWidth );
		} else if(m_bevelType == 2) {
			primitives_FrameThickRect16( surface, &rect, g_colorSet->GetColor( COLOR_UI_BOX ), m_bevelWidth );
		} else
			primitives_BevelRect16( surface, &rect, m_bevelWidth, m_bevelType, 16, 16 );
	}

	if (m_window)
		if ( surface == m_window->TheSurface() )
			m_window->AddDirtyRect( &rect );

	if((m_drawFunc)&&(!m_drawCallbackExclusive))
		m_drawFunc(this, surface, rect, m_drawCookie);

	DrawThisText(surface,&rect );

	return AUI_ERRCODE_OK;
}

void ctp2_Static::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );

	m_mouseCode = AUI_ERRCODE_HANDLED;
}

void ctp2_Static::MouseRGrabInside( aui_MouseEvent *mouseData)
{
	if (!GetWhichSeesMouse())
		SetWhichSeesMouse(this);

	m_mouseCode = AUI_ERRCODE_HANDLED;
}


void ctp2_Static::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );

	if(!HandleGameSpecificLeftClick((void *)this)) {
		if(m_ActionFunc) {
			m_ActionFunc(this, k_CTP2_STATIC_ACTION_LMOUSE, 0, m_cookie);
		}
	}

	if(m_imageMapCallback) {
		m_imageMapCallback(this, mouseData, m_imageMapCookie);
	}

	m_mouseCode = AUI_ERRCODE_HANDLED;
}

void ctp2_Static::MouseRDropInside( aui_MouseEvent *mouseData )
{

#if 0
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );
	}

	HandleGameSpecificRightClick((void *)this);
	m_mouseCode = AUI_ERRCODE_HANDLED;








#else
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );

		if(m_ActionFunc) {
			m_ActionFunc(this, k_CTP2_STATIC_ACTION_RMOUSE, 0, m_cookie);
		} else {
			HandleGameSpecificRightClick((void *)this);
		}

		m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else {
		MouseRDropOutside( mouseData );
	}
#endif
}

void ctp2_Static::SetDrawCallbackAndCookie(StaticDrawCallback *func, void *cookie,bool exclusive)
{
	m_drawFunc		= func;
	m_drawCookie	= cookie;
	m_drawCallbackExclusive	= exclusive;
}

void ctp2_Static::SetImageMapCallback(ImageMapCallback *func, void *cookie)
{
	m_imageMapCallback = func;
	m_imageMapCookie = cookie;
}

void ctp2_Static::SetBevel(uint32 width, uint32 type)
{
	m_bevelWidth = width;
	m_bevelType = type;
	ShouldDraw(TRUE);
}


void ctp2_Static::FitToBitmap(void)
{

	if (m_multiImageStatic)
		return;

	aui_Image		*image = GetImage(0, AUI_IMAGEBASE_SUBSTATE_FIRST);
	if (image == NULL)
		return;

	aui_Surface		*surf = image->TheSurface();
	if (surf == NULL)
		return;

	Resize(surf->Width(), surf->Height());
}
