










 


#include "c3.h"

#include "tiffutils.h"
#include "pixelutils.h"
#include "spriteutils.h"
#include "ScreenManager.h"

#include "aui.h"
#include "aui_surface.h"

#include "Sprite.h"
#include "FacedSprite.h"
#include "TiledMap.h"

#include "Token.h"

extern ScreenManager *g_screenManager;
extern TiledMap		*g_tiledMap;



FacedSprite::FacedSprite()
{
	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++)
	{
		m_frames[facing] = NULL;
		m_miniframes[facing] = NULL;
	}
	m_type = SPRITETYPE_FACED;
}



FacedSprite::~FacedSprite()
{
	sint32		i;

	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++) {
		if (m_frames[facing] != NULL) {
			for (i=0; i<m_numFrames; i++) {
				delete[] m_frames[facing][i];
			}
			delete[] m_frames[facing];
			m_frames[facing] = NULL;
		}
		if (m_miniframes[facing] != NULL) {
			for (i=0; i<m_numFrames; i++) {
				delete[] m_miniframes[facing][i];
			}
			delete [] m_miniframes[facing];
			m_miniframes[facing] = NULL;
		}
	}
}



void FacedSprite::Import(uint16 nframes, char *imageFiles[k_NUM_FACINGS][k_MAX_NAMES], char *shadowFiles[k_NUM_FACINGS][k_MAX_NAMES])
{
	m_numFrames = nframes;
 
	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++) 
	{
		
		m_frames[facing] = (Pixel16 **)malloc(sizeof(Pixel16 *) * m_numFrames);
		
		if (m_frames[facing] == NULL) 
			return;
		
		
		m_miniframes[facing] = (Pixel16 **)malloc(sizeof(Pixel16 *) * m_numFrames);
		
		if (m_miniframes[facing] == NULL) 
			return;

		
		Pixel32 *image,*miniimage;
		Pixel32 *shadow,*minishadow;
		char *fname;

		
		for (uint16 i=0; i<m_numFrames; i++) 
		{
			char ext[_MAX_DIR];
		
			
			image		= NULL;      
			miniimage	= NULL;
			shadow		= NULL;
			minishadow	= NULL;
		
			
			fname = imageFiles[facing][i];

			
			_splitpath(fname,NULL,NULL,NULL,ext);
		
			if (strstr(strupr(ext),"TIF"))
				ImportTIFF(i,imageFiles[facing],&image);
			else
				if (strstr(strupr(ext),"TGA"))
					ImportTGA(i,imageFiles[facing],&image);
				else
					printf("Unknown image file \"%s\"\n",fname);
		
			
			fname = shadowFiles[facing][i];

			
			_splitpath(fname,NULL,NULL,NULL,ext);
		
			if (strstr(strupr(ext),"TIF"))
				ImportTIFF(i,shadowFiles[facing],&shadow);
			else
				if (strstr(strupr(ext),"TGA"))
					ImportTGA(i,shadowFiles[facing],&shadow);

			
			if (image)
			{
				spriteutils_CreateQuarterSize(image, m_width, m_height,&miniimage, TRUE);
		
				
				m_frames[facing][i]     = spriteutils_RGB32ToEncoded(image,shadow, m_width, m_height);
		
				if (shadow)
					spriteutils_CreateQuarterSize(shadow, m_width, m_height,&minishadow, FALSE);
		
				
				m_miniframes[facing][i] = spriteutils_RGB32ToEncoded(miniimage, minishadow, m_width >> 1, m_height >> 1);
			}
			else 
			{
				
				printf("Could not locate %s.  Aborting.\n\n", imageFiles[facing][i]);
				exit(-1);
			}

			
			if (image) 		delete []image;
			if (shadow)		delete []shadow;
			if (miniimage) 	delete []miniimage;
			if (minishadow)	delete []minishadow;

			printf(".");
		}
	}
}




void FacedSprite::Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} else {
		drawX -=  (sint32)((double)(m_width - m_hotPoints[k_MAX_FACINGS - facing].x) * scale);
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	
	



	
	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
		if (facing < 5) {
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		} else
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 5)
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
		} else {
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) {
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} else {
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}

BOOL FacedSprite::HitTest(POINT mousePt, sint32 drawX, sint32 drawY, sint32 facing, double scale, sint16 transparency, 
						Pixel16 outlineColor, uint16 flags)
{
	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} else {
		drawX -=  (sint32)((double)(m_width - m_hotPoints[k_MAX_FACINGS - facing].x) * scale);
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}


	
	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
		if (facing < 5) {
			return HitTestLow(mousePt, (Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		} else
			return HitTestLowReversed(mousePt, (Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 5)
				return HitTestLow(mousePt, (Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else
				return HitTestLowReversed(mousePt, (Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
		} else {
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) {
				return HitTestScaledLow(mousePt, (Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} else {
				return HitTestScaledLow(mousePt, (Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}




void FacedSprite::DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	BOOL wasNull = FALSE;

	if (surf == NULL) {
		SetSurface();
		surf = m_surface;
		wasNull = TRUE;
	} else
		LockSurface(surf);

	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} else {
		drawX -=  (sint32)((double)(m_width - m_hotPoints[k_MAX_FACINGS - facing].x) * scale);
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	
	
	if (drawX > surf->Width() - 1 || drawX < -(m_width*scale)) {
		UnlockSurface();
		return;
	}
	
	if (drawY > surf->Height() - 1 || drawY < -(m_height*scale)) 
	{
		UnlockSurface();
		return;
	}

	
	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) 
	{
		if (facing < 5) {
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		} else {
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 5) {
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			} else {
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		} else {
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) {
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} else {
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}

	if (surf != NULL && !wasNull)
		UnlockSurface();
}





void FacedSprite::DirectionalDraw(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} else {
		drawX -=  (m_width - (sint32)((double)(m_width - m_hotPoints[k_MAX_FACINGS - facing].x) * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	
	
	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	
	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
		if (facing < 4 && facing > 0) 
		{
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		} 
		else if (facing == 4 || facing == 0)
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
		else
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 4 && facing > 0)
			{
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else if (facing == 4 || facing == 0)
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		} else {
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 4 && facing > 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} 
			else if(facing == 4 || facing == 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			} 
			else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}



sint32 FacedSprite::ParseFromTokens(Token *theToken)
{
	sint32		tmp; 
	sint32		i;

	if (!token_ParseAnOpenBraceNext(theToken)) return FALSE; 

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_NUM_FRAMES, tmp)) return FALSE;
	m_numFrames = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_FIRST_FRAME, tmp)) return FALSE;
	m_firstFrame = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_WIDTH, tmp)) return FALSE;
	m_width = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_HEIGHT, tmp)) return FALSE;
	m_height = (uint16)tmp;

	
	if (!token_ParseKeywordNext(theToken, TOKEN_SPRITE_HOT_POINTS)) return FALSE;
	for (i=0; i<k_NUM_FACINGS; i++) {
		POINT		p;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
		else return FALSE;

		p.x = tmp;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
		else return FALSE;

		p.y = tmp;

		m_hotPoints[i] = p;
	}

	if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;

	return TRUE;
}

void FacedSprite::AllocateFrameArrays(void)
{
	sint32 i;

	for (i=0; i<k_NUM_FACINGS; i++) {
		m_frames[i] = (Pixel16 **)new uint8[sizeof(Pixel16 *) * GetNumFrames()];
		m_miniframes[i] = (Pixel16 **)new uint8[sizeof(Pixel16 *) * GetNumFrames()];
	}
}

void FacedSprite::Export(FILE *file)
{
	extern TokenData	g_allTokens[];
	sint32				i;

	fprintf(file, "\t{\n");

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_NUM_FRAMES].keyword, m_numFrames);

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_FIRST_FRAME].keyword, m_firstFrame);

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_WIDTH].keyword, m_width);

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_HEIGHT].keyword, m_height);

	fprintf(file, "\t\t%s\n", g_allTokens[TOKEN_SPRITE_HOT_POINTS].keyword);
	for (i=0; i<k_NUM_FACINGS; i++) {
		fprintf(file, "\t\t\t%ld %ld\n", m_hotPoints[i].x, m_hotPoints[i].y);
	}

	fprintf(file, "\t}\n\n");

	return;
}

