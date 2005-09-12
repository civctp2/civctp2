//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite with facings and shadows.
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
#include "FacedSpriteWshadow.h"

#include "tiffutils.h"
#include "pixelutils.h"
#include "spriteutils.h"
#include "c3errors.h"
#include "screenmanager.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_directsurface.h"

#include "Sprite.h"

#include "Token.h"

extern ScreenManager *g_screenManager;



FacedSpriteWshadow::FacedSpriteWshadow()
:   Sprite              (),
    m_hasShadow         (TRUE),
    m_shadowFrameCount  (0)
{
	for (size_t facing = 0; facing < k_NUM_FACINGS; ++facing)
	{
		m_frames[facing] = NULL;
		m_miniframes[facing] = NULL;
		m_shadowFrames[facing] = NULL;
		m_miniShadowFrames[facing] = NULL;
	}
	m_type = SPRITETYPE_FACEDWSHADOW;
}



FacedSpriteWshadow::~FacedSpriteWshadow()
{
	for (size_t facing = 0; facing < k_NUM_FACINGS; ++facing)
	{
        for (size_t i = 0; i < m_shadowFrameCount; ++i)
		{
            delete m_frames[facing][i];
            delete m_miniframes[facing][i];
            delete m_shadowFrames[facing][i];
            delete m_miniShadowFrames[facing][i];
		}

		delete [] m_frames[facing];
		delete [] m_miniframes[facing];
        delete [] m_shadowFrames[facing];
		delete [] m_miniShadowFrames[facing];
		}
		}


void FacedSpriteWshadow::Import(uint16 nframes, char *imageFiles[k_NUM_FACINGS][k_MAX_NAMES], char *shadowFiles[k_NUM_FACINGS][k_MAX_NAMES])
{
	AllocateFrameArrays(nframes);

	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++) 
	{
		for (sint32 i=0; i < nframes; i++) 
		{
			char *tif = StripTIF2Mem(imageFiles[facing][i], &m_width, &m_height);

			if (tif) 
			{
				char *minitif = NULL;
				spriteutils_CreateQuarterSize((Pixel32 *)tif, m_width, m_height, (Pixel32 **)&minitif, TRUE);
							
				m_frames[facing][i]     = spriteutils_RGB32ToEncoded((Pixel32 *)tif, m_width, m_height);
                m_miniframes[facing][i] = spriteutils_RGB32ToEncoded((Pixel32 *)minitif, m_width >> 1, m_height >> 1);

	            uint16		width;
                uint16      height;
				char *shadowTif = StripTIF2Mem(shadowFiles[facing][i], &width, &height);

				if (shadowTif) 
				{
					m_shadowFrames[facing][i] = spriteutils_RGB32ToEncoded((Pixel32 *)shadowTif, m_width, m_height);

				    char *minishadow = NULL;
					spriteutils_CreateQuarterSize((Pixel32 *)shadowTif, m_width, m_height, (Pixel32 **)&minishadow, FALSE);

					if(minishadow)
					{
						m_miniShadowFrames[facing][i] = 
                            spriteutils_RGB32ToEncoded((Pixel32 *)minishadow, m_width >> 1, m_height >> 1);
						}
					
					free(shadowTif);
					}

				free(tif);
			} 
					
			printf(".");
		}
	}
}




void FacedSpriteWshadow::Import(uint16 nframes, char *imageFiles[k_NUM_FACINGS][k_MAX_NAMES])
{
    AllocateFrameArrays(nframes);
 
	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++) 
	{
		for (sint32 i=0; i < nframes; i++) 
		{
			char *tif = StripTIF2Mem(imageFiles[facing][i], &m_width, &m_height);

			if (tif) 
			{
				char *minitif = NULL;
				spriteutils_CreateQuarterSize((Pixel32 *)tif, m_width, m_height, (Pixel32 **)&minitif, TRUE);
			
				m_frames[facing][i]     = spriteutils_RGB32ToEncoded((Pixel32 *)tif, m_width, m_height);
				m_miniframes[facing][i] = spriteutils_RGB32ToEncoded((Pixel32 *)minitif, m_width >> 1, m_height >> 1);

				free(tif);
			} 
					
			printf(".");
		}
	}
}


void FacedSpriteWshadow::Draw(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) 
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} 
	else 
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid for Draw");
		return;
	}
	

	
	
	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	
	if (scale == 1.0) 
	{
		if (facing < 5) 
		{
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		} 
		else
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
	} 
	else 
	{
		if (scale == 0.5) 
		{
			if (facing < 5)
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
		} 
		else 
		{
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) 
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} 
			else 
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}


void FacedSpriteWshadow::DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	LockSurface(surf);

	if (facing < 5) 
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} 
	else 
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid for Draw");
		UnlockSurface();
		return;
	}
	

	
	
	if (drawX > surf->Width() - (m_width*scale) || drawX < 0) {
		UnlockSurface();
		return;
	}
	if (drawY > surf->Height() - (m_height*scale) || drawY < 0) {
		UnlockSurface();
		return;
	}

	
	if (scale == 1.0) 
	{
		if (facing < 5) 
		{
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		} 
		else {
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	} 
	else 
	{
		if (scale == 0.5) 
		{
			if (facing < 5)
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else {
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		} 
		else 
		{
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) 
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} 
			else 
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
	UnlockSurface();

}




void FacedSpriteWshadow::DirectionalDraw(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) 
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} 
	else 
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	
	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid for Directional Draw");
		return;
	}

	
	
	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	
	if (scale == 1.0) 
	{
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
	} 
	else 
	{
		if (scale == 0.5) 
		{
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
		} 
		else 
		{
			
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




void FacedSpriteWshadow::DrawShadow(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) 
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} 
	else 
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{
		
		return;
	}

	
	
	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	
	if (scale == 1.0) 
	{
		if (facing < 5) 
		{
			(this->*_DrawLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		} 
		else
			(this->*_DrawLowReversed)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
	} 
	else 
	{
		if (scale == 0.5) 
		{
			if (facing < 5)
				(this->*_DrawLow)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else
				(this->*_DrawLowReversed)((Pixel16 *)m_miniShadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
		} 
		else 
		{
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) 
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} 
			else 
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}




void FacedSpriteWshadow::DrawFlashEffect(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) 
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} 
	else 
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	
	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid Draw Flash Effect");
		return;
	}

	
	
	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	
	if (scale == 1.0) 
	{
		if (facing < 5) 
		{
			(this->*_DrawFlashLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
		else
		{
			(this->*_DrawFlashLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	} 
	else 
	{
		if (scale == 0.5) 
		{
			if (facing < 5)
			{
				(this->*_DrawFlashLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else
			{
				(this->*_DrawFlashLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		}
		else 
		{
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) 
			{
				(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
					transparency, outlineColor, flags, FALSE);
			}	
			else 
			{
				(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
					transparency, outlineColor, flags, TRUE);
			}
		}
	}
}





void FacedSpriteWshadow::DirectionalDrawShadow(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) 
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	} 
	else 
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	
	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{
		return;
	}

	
	
	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	
	if (scale == 1.0) 
	{
		if (facing < 4 && facing > 0) 
		{
			(this->*_DrawLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		} 
		else if (facing == 4 || facing == 0)
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
		else
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	} 
	else 
	{
		if (scale == 0.5) 
		{
			if (facing < 4 && facing > 0)
			{
				(this->*_DrawLow)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else if (facing == 4 || facing == 0)
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		} 
		else 
		{
			
			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 4 && facing > 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} 
			else if(facing == 4 || facing == 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			} 
			else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}



sint32 FacedSpriteWshadow::ParseFromTokens(Token *theToken)
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
	for (i=0; i<k_NUM_FACINGS; i++) 
	{
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
// Name       : FacedSpriteWshadow::AllocateFrameArrays
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
void FacedSpriteWshadow::AllocateFrameArrays(size_t count)
{
    Assert(0 == m_shadowFrameCount);

	for (size_t facing = 0; facing < k_NUM_FACINGS; ++facing) 
	{
		m_frames[facing]            = new Pixel16 * [count];
		m_miniframes[facing]        = new Pixel16 * [count];
		m_shadowFrames[facing]      = new Pixel16 * [count];
		m_miniShadowFrames[facing]  = new Pixel16 * [count];

		for (size_t i = 0; i < count; ++i) 
		{
			m_frames[facing][i]             = NULL;
			m_miniframes[facing][i]         = NULL;
			m_shadowFrames[facing][i]       = NULL;
			m_miniShadowFrames[facing][i]   = NULL;
	}
}

    m_shadowFrameCount  = count;
}
