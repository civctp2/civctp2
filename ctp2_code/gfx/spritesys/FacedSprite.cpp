//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite with facings.
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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added separate counters in Sprite-derived classes to prevent crashes.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "FacedSprite.h"

#include "tiffutils.h"
#include "pixelutils.h"
#include "spriteutils.h"
#include "screenmanager.h"

#include "aui.h"
#include "aui_surface.h"

#include "Sprite.h"
#include "tiledmap.h"

#include "Token.h"

extern ScreenManager *g_screenManager;
extern TiledMap		*g_tiledMap;



FacedSprite::FacedSprite()
:   Sprite              (),
    m_facedFrameCount   (0)
{
	for (size_t facing = 0; facing < k_NUM_FACINGS; ++facing)
	{
		m_frames[facing]        = NULL;
		m_framesSizes[facing] = NULL;
		m_miniframes[facing]    = NULL;
		m_miniframesSizes[facing] = NULL;
	}
	m_type = SPRITETYPE_FACED;
}



FacedSprite::~FacedSprite()
{
	sint32		i;

	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++) {
		if (m_frames[facing]) {
			for (i=0; i<m_numFrames; i++) {
				delete[] m_frames[facing][i];
			}
			delete[] m_frames[facing];
		}
		if (m_framesSizes[facing]) {
			delete[] m_framesSizes[facing];
		}
		if (m_miniframes[facing]) {
			for (i=0; i<m_numFrames; i++) {
				delete[] m_miniframes[facing][i];
			}
			delete [] m_miniframes[facing];
		}
		if (m_miniframesSizes[facing]) {
			delete[] m_miniframesSizes[facing];
		}
	}
}



void FacedSprite::Import(uint16 nframes, char *imageFiles[k_NUM_FACINGS][k_MAX_NAMES], char *shadowFiles[k_NUM_FACINGS][k_MAX_NAMES]) {
    printf("%s L%d: imageFiles[0][0] = %s\n", __FILE__, __LINE__, imageFiles[0][0]);
    AllocateFrameArrays(nframes);
 
	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++) 
	{
		for (uint16 i=0; i < nframes; i++) 
		{
			char ext[_MAX_DIR] = { 0 };
		
			Pixel32 *   image       = NULL;      
			Pixel32 *   miniimage	= NULL;
			Pixel32 *   shadow		= NULL;
			Pixel32 *   minishadow	= NULL;
			char *      fname       = imageFiles[facing][i];

			
			char *dot = strrchr(fname, '.');
			if (dot && (*++dot)) {
				char *end = strncpy(ext, dot, _MAX_DIR - 1);
				*++end = '\0';
			}
                        printf("%s L%d: fname = %s\n", __FILE__, __LINE__, fname);
			if (strstr(strupr(ext),"TIF"))
				ImportTIFF(i,imageFiles[facing],&image);
			else
				if (strstr(strupr(ext),"TGA"))
					ImportTGA(i,imageFiles[facing],&image);
				else
					printf("Unknown image file \"%s\"\n",fname);
		
			
			fname = shadowFiles[facing][i];

			dot = strrchr(fname, '.');
			if (dot && (*++dot)) {
				char *end = strncpy(ext, dot, _MAX_DIR - 1);
				*++end = '\0';
			}
		
			if (strstr(strupr(ext),"TIF"))
				ImportTIFF(i,shadowFiles[facing],&shadow);
			else
				if (strstr(strupr(ext),"TGA"))
					ImportTGA(i,shadowFiles[facing],&shadow);

			
			if (image)
			{
				spriteutils_CreateQuarterSize(image, m_width, m_height,&miniimage, TRUE);
		
				
				m_frames[facing][i]     = spriteutils_RGB32ToEncoded(image,shadow, m_width, m_height, &(m_framesSizes[facing][i])); //battlebug?
                                printf("%s L%d: m_frames[facing][i] = %#X\n", __FILE__, __LINE__, m_frames[facing][i]);
		
				if (shadow)
					spriteutils_CreateQuarterSize(shadow, m_width, m_height,&minishadow, FALSE);
		
				
				m_miniframes[facing][i] = spriteutils_RGB32ToEncoded(miniimage, minishadow, m_width >> 1, m_height >> 1, &(m_miniframesSizes[facing][i]));
			}
			else 
			{
				
				printf("Could not locate %s.  Aborting.\n\n", imageFiles[facing][i]);
				exit(-1);
			}

			
			delete [] image;
			delete [] shadow;
			delete [] miniimage;
			delete [] minishadow;

			printf(".");
		}
	}
}


void FacedSprite::SetFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size)
{
	Assert(facing < k_NUM_FACINGS);
	Assert(frame < m_numFrames);
	Assert(m_frames[facing] != NULL);
	Assert(m_framesSizes[facing] != NULL);
	
	if (facing >= k_NUM_FACINGS)
		return;
	if (frame >= m_numFrames)
		return;

	m_frames[facing][frame] = data;//battlebug?
        printf("%s L%d: m_frames[facing][i] = %#X\n", __FILE__, __LINE__, m_frames[facing][frame]);
	m_framesSizes[facing][frame] = size;
}

void FacedSprite::SetMiniFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size)
{
	Assert(facing < k_NUM_FACINGS);
	Assert(frame < m_numFrames);
	Assert(m_miniframes[facing] != NULL);
	Assert(m_miniframesSizes[facing] != NULL);

	if (facing >= k_NUM_FACINGS)
		return;
	if (frame >= m_numFrames)
		return;
	
	m_miniframes[facing][frame] = data;
	m_miniframesSizes[facing][frame] = size;
}

//----------------------------------------------------------------------------
//
// Name       : FacedSprite::Draw
//
// Description: Draw a sprite 
//
// Parameters : drawX, drawY    : position
//              facing          : facing
//              scale           : magnification (zoom level)
//              transparency    : 
//              outlineColor    :
//              flags
//
// Globals    : g_tiledMap
//
// Returns    : -
//
// Remark(s)  : Refactored a bit to better understand what is going on. 
//              But the problem wasn't here after all.
//
//----------------------------------------------------------------------------
void FacedSprite::Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

    bool const      isReversed  = facing >= k_NUM_FACINGS;
    size_t          facingIndex = isReversed ? k_MAX_FACINGS - facing : facing;
    Pixel16 *       frame       = (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST))
                                  ? m_miniframes[facingIndex][m_currentFrame]
        : m_frames[facingIndex][m_currentFrame]; //battlebug? //spritebug?

    if (!frame) {
        //printf("%s L%d: frames = %#X\n", __FILE__, __LINE__, frame);
        return;
    }

	if (isReversed) 
    {
		drawX -= (sint32)((double)(m_width - m_hotPoints[facingIndex].x) * scale);
	    drawY -= (sint32)((double)m_hotPoints[facingIndex].y * scale);
	} 
    else 
    {
		drawX -= (sint32)((double)m_hotPoints[facingIndex].x * scale);
	    drawY -= (sint32)((double)m_hotPoints[facingIndex].y * scale);
	}


	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) 
    {
		if (isReversed) 
        {
			(this->*_DrawLowReversed)(frame, drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		} 
        else
        {
			(this->*_DrawLow)(frame, drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
        }
	} 
    else if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) 
    {
		if (isReversed)
        {
			(this->*_DrawLowReversed)(frame, drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
        }
		else
        {
			(this->*_DrawLow)(frame, drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
        }
	} 
    else 
    {
		sint32 const    destWidth   = (sint32) (m_width * scale);
		sint32 const    destHeight  = (sint32) (m_height * scale);

		(this->*_DrawScaledLow)(frame, drawX, drawY, destWidth, destHeight,
								transparency, outlineColor, flags, isReversed
                               );
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
				return HitTestLow(mousePt, m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else
				return HitTestLowReversed(mousePt, m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
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




void FacedSprite::DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing, double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags){
    BOOL wasNull = FALSE;

    if (surf == NULL) {
        SetSurface();
        surf = m_surface;
        wasNull = TRUE;
        } 
    else
        LockSurface(surf);

       if (facing < 5) {
        drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
        drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
        } 
    else {
        drawX -= (sint32)((double)(m_width - m_hotPoints[k_MAX_FACINGS - facing].x) * scale);
        drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
        }
/* from Sprite.cpp Draw(...
    if (facing < 5) {
        drawX -= (sint32)((double)m_hotPoint.x * scale);
	} 
    else {
        drawX -= (sint32)((double)(m_width-m_hotPoint.x) * scale);
	}

    drawY -= (sint32)((double)m_hotPoint.y * scale);
*/ 		
    if (drawX > surf->Width() - 1 || drawX < -(m_width*scale)) {
        UnlockSurface();
        return;
        }
	
    if (drawY > surf->Height() - 1 || drawY < -(m_height*scale)) 
        {
        UnlockSurface();
        return;
        }

    //printf("FacedSprite.cpp L363: m_frames %p, m_frames[k_MAX_FACINGS - facing] = %p, facing = %d, k_MAX_FACINGS = %d, m_currentFrame %d\n", m_frames, m_frames[k_MAX_FACINGS - facing], facing, k_MAX_FACINGS, m_currentFrame);
/*    for (size_t w = 0; w < m_facedFrameCount; ++w)
        {
        //printf("FacedSprite.cpp L366: m_frames[k_MAX_FACINGS - facing][w] = %p\n", m_frames[k_MAX_FACINGS - facing][w]);
        //printf("FacedSprite.cpp L366: m_frames[0][w] = %p\n", m_frames[0][w]);
        //check what this is for!!!
			m_framesSizes[facing][w]     = 0;
			m_miniframes[facing][w]      = 0;
			m_miniframesSizes[facing][w] = 0;
        }
*/	


    if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
        if (facing < 5) {
            (this->*_DrawLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
            } 
        else {
            (this->*_DrawLowReversed)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
            }
        } 
    else {
        if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
            if (facing < 5) {
                (this->*_DrawLow)((Pixel16 *)m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
                } 
            else {
                (this->*_DrawLowReversed)((Pixel16 *)m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
                }
            } 
        else {
			
            sint32 destWidth = (sint32)(m_width * scale);
            sint32 destHeight = (sint32)(m_height * scale);

            if (facing < 5) {
                (this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight, transparency, outlineColor, flags, FALSE);
                } 
            else {
                (this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight, transparency, outlineColor, flags, TRUE);
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
				(this->*_DrawLow)(m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else if (facing == 4 || facing == 0)
			{
				(this->*_DrawLowReversed)(m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else
			{
				(this->*_DrawLowReversed)(m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
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
	m_facedFrameCount = tmp;

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

//----------------------------------------------------------------------------
//
// Name       : FacedSprite::AllocateFrameArrays
//
// Description: Allocate memory for the faced sprites.
//
// Parameters : count   : number of sprites per facing to reserve
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Assumption: Memory has not been allocated before.
//
//----------------------------------------------------------------------------
void FacedSprite::AllocateFrameArrays(size_t count)
{
	Assert(0 == m_facedFrameCount);

	for (size_t facing = 0; facing < k_NUM_FACINGS; ++facing) 
	{
		m_frames[facing]          = new Pixel16 *[count];
		m_framesSizes[facing]     = new size_t   [count];
		m_miniframes[facing]      = new Pixel16 *[count];
		m_miniframesSizes[facing] = new size_t   [count];

		for (size_t i = 0; i < count; ++i)
		{
			m_frames[facing][i]          = NULL;
			m_framesSizes[facing][i]     = 0;
			m_miniframes[facing][i]      = NULL;
			m_miniframesSizes[facing][i] = 0;
		}
	}

	m_facedFrameCount   = count;
}

void FacedSprite::Export(FILE *file)
{
	extern TokenData	g_allTokens[];
	sint32				i;

	fprintf(file, "\t{\n");

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_NUM_FRAMES].keyword, m_facedFrameCount);

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

